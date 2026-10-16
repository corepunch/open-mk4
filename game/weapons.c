#include "game.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

#define WEAPON_MOVE(c, a, end) {.clip = c, .last = end, .flags = SF_LOCK | SF_RECORD, .action = a}
static const MoveDef draw_move = WEAPON_MOVE(244, 0x1004, 49);
static const MoveDef throw_move = WEAPON_MOVE(208, 0x30f, 60);
static const MoveDef pickup_move = WEAPON_MOVE(209, 0x2006, 14);
static const MoveDef air_hp = WEAPON_MOVE(252, 0x603, 9);
static const MoveDef air_lp = WEAPON_MOVE(251, 0x604, 9);
static const MoveDef landing = WEAPON_MOVE(304, 0x2008, 27);
static const MoveDef blend_stance = WEAPON_MOVE(286, 0x1006, 0);
#undef WEAPON_MOVE
static const MoveDef idle = {.clip = 286, .last = MOVE_END};
static const MoveDef walk = {.clip = 277, .last = MOVE_END};
static const MoveDef back_walk = {.clip = 278, .last = MOVE_END};
static const MoveDef crouch = {.clip = 219, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef block = {.clip = 245, .last = MOVE_END, .flags = SF_HOLD};
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord air_records[] = {
  {110690, -60882, 68157, 62914, 156, 156, 4, 0, 11796},
  {101580, -4587, 64225, 53739, 155, 155, 4, 0, 11796},
};
static const AttackRecord thrown_record = {57606, -37945, 73990, 35979, 154, 154, 0, ATTACK_MARK, 16384};
// clang-format on

void weapon_stance(Battle *battle, Fighter *fighter, int state) {
  const MoveDef *move = roster_def(fighter->roster)->moves[state];
  if (fighter->armed) {
    move = state == S_IDLE ? &idle : state == S_WALK ? (fighter_holds_back(battle, fighter) ? &back_walk : &walk) : state == S_CROUCH ? &crouch : &block;
  }
  fighter_start_move(battle, fighter, state, move);
}

static void blend_tick(Battle *battle, Fighter *fighter) {
  fighter->blend_ticks++;
  if (fighter->blend_ticks >= fighter->blend_duration) {
    fighter_ready(battle, fighter);
  }
}

static void strike_tick(Battle *battle, Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  int active = strike->target - strike->first + 1;
  if (fighter->weapon_travel && !fighter->connected && fighter->ticks >= strike->delay && fighter->ticks < strike->delay + strike->travel) {
    fighter_set_speed(battle, fighter, strike->speed);
  } else {
    fighter_set_speed(battle, fighter, 0);
  }
  if (fighter->script_phase == 0) {
    fighter->sample_override = strike->first + fighter->ticks;
    if (fighter->ticks >= active) {
      battle_contact(battle, fighter, &strike->record);
      if (fighter->connected || fighter->ticks >= active + strike->queries - 1) {
        fighter->script_phase = 1;
        fighter->action = 0x1006;
        fighter_set_speed(battle, fighter, 0);
        /* 495280 / 495300 finish from the current cursor. Other used
         * recoveries increment it before finishing (494d00 / 495110). */
        fighter->script_timer = fighter->connected && (strike->recovery == 5 || strike->recovery == 9);
      }
    }
    return;
  }
  if (fighter->script_timer > 0) {
    fighter->script_timer--;
  } else if (fighter->sample_override < strike->move.last) {
    fighter->sample_override++;
  } else if (fighter->connected && strike->recovery == 9) {
    int clip = strike->move.clip, sample = fighter->sample_override;
    fighter_start_move(battle, fighter, S_PUNCH, &blend_stance);
    fighter->blend_clip = clip;
    fighter->blend_sample = sample;
    fighter->blend_ticks = 0;
    fighter->blend_duration = 10; /* 495300 -> 48fde0, step 0x1999 */
    fighter->sample_override = 0;
    fighter->script = blend_tick;
  } else {
    fighter_ready(battle, fighter);
  }
}

void weapon_start_strike(Battle *battle, Fighter *fighter, const WeaponStrike *strike, bool travel) {
  require(strike->first <= strike->windup && strike->windup <= strike->target && strike->queries > 0, "Invalid weapon phases");
  fighter_start_move(battle, fighter, S_PUNCH, &strike->move);
  fighter->weapon_strike = strike;
  fighter->weapon_travel = travel;
  fighter->sample_override = strike->first;
  fighter->script = strike_tick;
}

/* 494610 / 4946f0. This is separate from a back-punch's motion setup. */
void weapon_followup_victim(Battle *battle, Fighter *fighter) {
  Fighter *victim = &battle->fighters[fighter == &battle->fighters[0] ? 1 : 0];
  if (!fighter_airborne(victim)) {
    battle_react(battle, victim, 120);
  }
  if (victim->hits > 3) {
    victim->damage_scale = 0x4ccc;
  }
}

static void object_tick(Battle *battle, Projectile *p) {
  if (p->phase == 2) {
    p->lifetime = 2;
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  if (p->phase == 0) {
    ContactResult result = battle_contact_from(battle, &battle->fighters[p->owner], &thrown_record, p->pos, p->facing);
    if (result != CONTACT_MISS) {
      p->phase = 1;
      p->vel[AXIS_X] = p->vel[AXIS_Z] = 0;
      p->vel[AXIS_Y] = -0x2666;
      p->lifetime = 180;
    } else if (p->ticks >= 30 ||
               (int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] >= (int64_t)battle->arena_radius * battle->arena_radius) {
      p->kind = 0;
    }
  } else {
    p->vel[AXIS_Y] += 0x28f;
    if (p->pos[AXIS_Y] >= -0x1999 && p->vel[AXIS_Y] >= 0) {
      p->pos[AXIS_Y] = -0x1999;
      memset(p->vel, 0, sizeof p->vel);
      p->phase = 2;
      p->lifetime = 2;
    }
  }
}

static void drop(Battle *battle, Fighter *fighter) {
  if (!fighter->armed) {
    return;
  }
  Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_SWORD, 180, object_tick);
  if (p != NULL) {
    p->weapon = fighter->weapon;
    p->phase = 1;
    p->vel[AXIS_Y] = -0x2666;
  }
  fighter->armed = false;
}

void fighter_equip_weapon(Fighter *fighter, const WeaponDef *weapon) {
  fighter->armed = true;
  fighter->weapon_used = true;
  fighter->disarm = drop;
  fighter->weapon = weapon;
}

static void landing_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    fighter_ready(battle, fighter);
  }
}

static void air_tick(Battle *battle, Fighter *fighter) {
  bool high = fighter_move(fighter)->clip == 252;
  fighter->sample_override = mini(9, fighter->ticks);
  if (!fighter->connected && fighter->ticks >= (high ? 8 : 9) && fighter->ticks <= 10) {
    battle_contact(battle, fighter, &air_records[high ? 0 : 1]);
  }
  if (!fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, &landing);
    fighter->script = landing_tick;
    fighter->sample_override = 18;
  }
}

void weapon_air_attack(Battle *battle, Fighter *fighter, bool high) {
  fighter_start_move(battle, fighter, S_JUMP, high ? &air_hp : &air_lp);
  fighter->script = air_tick;
  fighter->sample_override = 0;
  fighter->body.on_land = NULL;
}

static void draw_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->ticks + (fighter->ticks >= 35);
  if (fighter->ticks == 22) {
    fighter->weapon->equip(fighter);
  }
  if (fighter->ticks >= 48) {
    fighter_ready(battle, fighter);
  }
}

void weapon_draw(Battle *battle, Fighter *fighter, const WeaponDef *weapon) {
  fighter_start_move(battle, fighter, S_PUNCH, &draw_move);
  fighter->weapon = weapon;
  fighter->script = draw_tick;
  fighter->sample_override = 0;
}

static void throw_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->ticks - (fighter->ticks >= 11);
  if (fighter->ticks == 22 && fighter->armed) {
    Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_SWORD, 90, object_tick);
    if (p != NULL) {
      p->weapon = fighter->weapon;
      for (int k = 0; k < 2; ++k) {
        int axis = k == 0 ? AXIS_X : AXIS_Z;
        p->pos[axis] += q16_mul(fighter->heading[k], 0xc000); /* 498480: .75 along heading */
        p->vel[axis] = q16_mul(fighter->heading[k], 0x3333);
      }
      p->pos[AXIS_Y] = -0x13333;
    }
    fighter->armed = false;
  }
  if (fighter->ticks >= 62) {
    fighter_ready(battle, fighter);
  }
}

void weapon_throw(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &throw_move);
  fighter->script = throw_tick;
  fighter->sample_override = 0;
}

static int pickup_index(const Battle *battle, const Fighter *fighter) {
  if (fighter->armed || fighter->action != 0x2001) {
    return -1;
  }
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &battle->projectiles[i];
    q16 dx = p->pos[AXIS_X] - fighter->body.pos[AXIS_X], dz = p->pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
    if (p->kind == PROJECTILE_SWORD && p->phase == 2 && q16_mul(dx, dx) + q16_mul(dz, dz) <= Q16_ONE) {
      return i;
    }
  }
  return -1;
}

bool weapon_can_pickup(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && pickup_index(battle, fighter) >= 0;
}

static void pickup_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(14, fighter->ticks);
  if (fighter->ticks >= 15) {
    fighter_ready(battle, fighter);
  }
}

void weapon_pickup(Battle *battle, Fighter *fighter) {
  int index = pickup_index(battle, fighter);
  if (index < 0) {
    fighter_ready(battle, fighter);
    return;
  }
  battle->projectiles[index].weapon->equip(fighter);
  battle->projectiles[index].kind = 0;
  fighter_start_move(battle, fighter, S_PUNCH, &pickup_move);
  fighter->script = pickup_tick;
  fighter->sample_override = 0;
}
