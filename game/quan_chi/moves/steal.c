#include "retail_vm.h"
#include "../character.h"
#include <math.h>

/* HP [F,B] record 4e47f8 selects native 469c80: action 401, sound C[37],
 * clip 7 (53ce1c) played to sample 10 by 428d30. */
static const MoveDef s_steal = {.clip = 7, .last = 21, .flags = SF_LOCK | SF_RECORD, .action = 0x401};
/* 469cea queries D[11]/4ec140 once, only against an armed victim. */
static const AttackRecord steal_record = {0x4a3d7, -0x17851, 0x4a3d7, 0x1e147, 108, 108, 0, 0, 0x7ae};
static const MoveDef s_stolen_ground = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_stolen_air = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION};

enum {
  STEAL_WAIT_TICKS = 30, /* 469d1d loads 53c440 with 0x1e */
};

/* 484100: while the thief keeps action 401 the weapon homes at 0.15 along
 * the X/Z line to the thief; afterwards 494590 drops it normally. */
static void stolen_weapon_step(Projectile *weapon) {
  const Fighter *thief = fighter_opponent(&g_battle.fighters[weapon->owner]);
  if (thief->action != 0x401) {
    weapon_object_settle(weapon);
    return;
  }
  double dx = (double)thief->body.pos[AXIS_X] - weapon->pos[AXIS_X];
  double dz = (double)thief->body.pos[AXIS_Z] - weapon->pos[AXIS_Z];
  double length = sqrt(dx * dx + dz * dz);
  if (length > 0) {
    weapon->vel[AXIS_X] = (q16)(dx / length * 0x2666);
    weapon->vel[AXIS_Z] = (q16)(dz / length * 0x2666);
  }
  weapon->pos[AXIS_X] += weapon->vel[AXIS_X];
  weapon->pos[AXIS_Z] += weapon->vel[AXIS_Z];
}

static Projectile *stolen_weapon(const Fighter *thief) {
  int victim = (int)(fighter_opponent(thief) - g_battle.fighters);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SWORD && p->step == stolen_weapon_step && p->owner == victim) {
      return p;
    }
  }
  return NULL;
}

static void steal_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void steal_stop(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  native_hold(fighter, 16, steal_ready);
}

static void steal_wait(Fighter *fighter) {
  native_hold(fighter, 32, steal_stop);
}

/* 469d7a: stop, play clip 7 out (428d10), wait 32, stop, wait 16, 46f650. */
static void steal_fail(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  int pose = fighter_sample(fighter);
  if (pose >= s_steal.last) {
    steal_wait(fighter);
    return;
  }
  native_begin(fighter, pose + 1, s_steal.last, steal_wait);
  for (int frame = pose + 1; frame <= s_steal.last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* 469d2c: 49cb30 finds the type-70 weapon task; within squared distance
 * 0x8000, 49cb20 retires it and 46a180 equips it with sound C[38/39]. */
static void steal_poll(Fighter *fighter) {
  Projectile *weapon = stolen_weapon(fighter);
  if (weapon == NULL) {
    steal_fail(fighter);
    return;
  }
  q16 dx = weapon->pos[AXIS_X] - fighter->body.pos[AXIS_X], dz = weapon->pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
  if (q16_mul(dx, dx) + q16_mul(dz, dz) >= 0x8000) {
    return;
  }
  const WeaponDef *def = weapon->weapon;
  weapon->kind = 0;
  def->equip(fighter);
  battle_sound(SOUND_C, fighter == &g_battle.fighters[0] ? 0x26 : 0x27);
  steal_ready(fighter);
}

static void steal_take(Fighter *fighter) {
  native_begin(fighter, 1, STEAL_WAIT_TICKS, steal_ready);
  for (int frame = 1; frame <= STEAL_WAIT_TICKS; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), steal_poll);
  }
  native_play(fighter, false);
}

/* 469caf: an armed victim and an unarmed thief allow the D[11] query. */
static void steal_query(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  ContactResult result = victim->armed && !fighter->armed ? battle_contact(fighter, &steal_record) : CONTACT_MISS;
  if (result == CONTACT_MISS || result == CONTACT_BLOCK) {
    steal_fail(fighter);
  } else {
    steal_take(fighter);
  }
}

void quan_chi_weapon_steal(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_steal);
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 37);
  native_begin(fighter, 1, 10, steal_query);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

/* Reaction 108 (484060): 493370 detaches the weapon from joint 10 into task
 * 70 (484100); 48ef30 then selects 4e9838 in the air or 4e9818 grounded. */
void quan_chi_steal_reaction(Fighter *victim) {
  if (!victim->armed) {
    fighter_ready(victim);
    return;
  }
  Projectile *weapon = battle_spawn_projectile(victim, PROJECTILE_SWORD, 600, stolen_weapon_step);
  if (weapon != NULL) {
    weapon->weapon = victim->weapon;
    Vec3 hand;
    Mat3 unused;
    fighter_joint_position(victim, 10, &hand, &unused);
    weapon->pos[AXIS_X] = (q16)(hand.x * Q16_ONE);
    weapon->pos[AXIS_Y] = (q16)(hand.y * Q16_ONE);
    weapon->pos[AXIS_Z] = (q16)(hand.z * Q16_ONE);
  }
  victim->armed = false;
  bool airborne = fighter_airborne(victim);
  fighter_start_move(victim, airborne ? S_KNOCKDOWN : S_HIT, airborne ? &s_stolen_air : &s_stolen_ground);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (airborne) {
    retail_vm_enter_owned(victim, 0x4e9838, fighter_ready, 8);
  } else {
    retail_vm_enter_owned(victim, 0x4e9818, fighter_ready, 8);
  }
}
