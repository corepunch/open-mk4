#include "../character.h"
#include <math.h>
#include <string.h>

static void equip_weapon(Fighter *fighter);
static void weapon_hp(Fighter *fighter);
static void weapon_lp(Fighter *fighter);
static void weapon_back_hp(Fighter *fighter);

/* 4955b0 descriptor 4ec420 and its held-back alternate 4955e0 (travel
 * 4ec438, strike 4ec448): clip, first/windup/target/queries/recovery,
 * WEAPON selector 42/43 and sound 0x20. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 229, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {0x1a666, -0xc24d, 0x12b43, 0x12106, 169, 10, 119, 0, 0x28f5}, 0, 4, 7, 3, 1, 0, 0, 0, 0x20},
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {0x204dd, -0x89fb, 0x15439, 0xb062, 170, 10, 119, 0, 0x23d7}, 6, 10, 16, 4, 1, 3, 8, 0x147a, 0x20},
};
// clang-format on
/* T2[0x16]/[0x17] 497ae0/497ad0 -> 497af0: action 316 and VM 4ed190. */
static const MoveDef boomerang_throw = {.clip = 229, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x316};
/* 497c50 waiter descriptor 4ed1a8 selects C[20] 4ebde0. */
static const AttackRecord boomerang_record = {0xc76c, -0x424d, 0x9be7, 0x8c8b, 169, 154, 0, ATTACK_MARK, 0x3333};
static const MoveDef victim_hit = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};
static const MoveDef victim_fall = {.clip = 182, .last = 64, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};
static const WeaponDef boomerang = {0x6d, weapon_hp, weapon_lp, weapon_back_hp, NULL, NULL, weapon_throw, equip_weapon, SCENE_MODEL_TANYA_WEAPON};

enum {
  BOOMERANG_OUT,
  BOOMERANG_BACK,
  BOOMERANG_HOME
};

static void equip_weapon(Fighter *fighter) {
  require(tanya_weapon_descriptor[0] == (uint32_t)boomerang.type, "Tanya weapon identity must match the reviewed retail descriptor");
  fighter_equip_weapon(fighter, &boomerang);
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[1], true);
}

/* 494990 takes descriptor word 3 (4955e0) when back is held. */
static void weapon_ground_hp(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    weapon_back_hp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[0], false);
  }
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else {
    weapon_ground_hp(fighter);
  }
}

/* 497e20: one counter across all phases plays sound C 0x5a on the first
 * visit and every tenth visit after it. */
static void boomerang_whoosh(Projectile *projectile) {
  if (projectile->ticks % 10 == 1) {
    battle_sound(SOUND_C, 0x5a);
  }
}

/* 493c20 turns the free rig by its stored spin. */
static void boomerang_turn(Projectile *projectile) {
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
    projectile->angle[axis] = (projectile->angle[axis] + projectile->spin[axis]) % 0x6487e;
  }
  boomerang_whoosh(projectile);
}

/* 497e60 state 0: reflect at -.2 horizontally, rise .2, for eight visits. */
static void boomerang_bounce(Projectile *projectile) {
  projectile->phase = BOOMERANG_BACK;
  projectile->bounce = 8;
  projectile->vel[AXIS_X] = q16_mul(projectile->vel[AXIS_X], -0x3333);
  projectile->vel[AXIS_Z] = q16_mul(projectile->vel[AXIS_Z], -0x3333);
  projectile->vel[AXIS_Y] = -0x3333;
}

/* 497e60's catch branch: an unarmed, free, grounded owner is interrupted
 * by 470420 and re-equipped through 46a180; otherwise 48a2c0 picks list
 * 4ea918 (tokens f5/8c) and 494590 settles the free weapon. */
static void boomerang_arrive(Projectile *projectile) {
  Fighter *owner = &g_battle.fighters[projectile->owner];
  memset(projectile->vel, 0, sizeof projectile->vel);
  if (!owner->armed && !(owner->flags & (FF_BUSY | FF_REACTING)) && !fighter_airborne(owner) && owner->health > 0 && g_battle.phase == PHASE_FIGHT) {
    projectile->weapon->equip(owner);
    battle_sound(SOUND_C, projectile->owner == 0 ? 0x26 : 0x27);
    projectile->kind = 0;
    fighter_interrupt(owner, fighter_ready);
    return;
  }
  battle_sound(SOUND_B, battle_random(2) == 0 ? 0x72 : 0x3d);
  memset(projectile->spin, 0, sizeof projectile->spin);
  weapon_object_settle(projectile);
}

/* 490e60 steers .1 per visit toward the owner; below -1.5 it descends at
 * .05. Within squared distance .5 it reaches the owner's hand. */
static void boomerang_home(Projectile *projectile) {
  const Fighter *owner = &g_battle.fighters[projectile->owner];
  double dx = (double)owner->body.pos[AXIS_X] - projectile->pos[AXIS_X];
  double dz = (double)owner->body.pos[AXIS_Z] - projectile->pos[AXIS_Z];
  double length = hypot(dx, dz);
  projectile->vel[AXIS_X] = length > 0 ? (q16)(dx / length * 0x1999) : 0;
  projectile->vel[AXIS_Z] = length > 0 ? (q16)(dz / length * 0x1999) : 0;
  projectile->vel[AXIS_Y] = projectile->pos[AXIS_Y] <= -0x18000 ? 0xccc : 0;
  q16 d[3] = {
      owner->body.pos[AXIS_X] - projectile->pos[AXIS_X], owner->body.pos[AXIS_Y] - projectile->pos[AXIS_Y], owner->body.pos[AXIS_Z] - projectile->pos[AXIS_Z]};
  if (q16_mul(d[0], d[0]) + q16_mul(d[1], d[1]) + q16_mul(d[2], d[2]) <= 0x8000) {
    boomerang_arrive(projectile);
  }
}

/* 497c50/497de0: the outward flight probes C[20] through 49c690, which
 * skips a victim in action 2001, and turns back on contact or outside
 * the arena radius squared plus twelve. */
static void boomerang_step(Projectile *projectile) {
  if (projectile->phase == BOOMERANG_HOME) {
    boomerang_home(projectile);
    if (projectile->step == boomerang_step && projectile->kind != 0) {
      boomerang_turn(projectile);
    }
    return;
  }
  boomerang_turn(projectile);
  if (projectile->phase == BOOMERANG_BACK) {
    if (--projectile->bounce <= 0) {
      projectile->phase = BOOMERANG_HOME;
    }
    return;
  }
  const Fighter *victim = fighter_opponent(&g_battle.fighters[projectile->owner]);
  if (!g_battle.contacts_off && victim->action != ACTION_RETAIL_2001 && battle_projectile_contact(projectile, &boomerang_record) != CONTACT_MISS) {
    boomerang_bounce(projectile);
    return;
  }
  int64_t x = projectile->pos[AXIS_X], z = projectile->pos[AXIS_Z];
  int64_t radius = g_battle.arena_radius;
  if (x * x + z * z > radius * radius + (int64_t)12 * Q16_ONE * Q16_ONE) {
    boomerang_bounce(projectile);
  }
}

static void boomerang_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

/* 497b60 at sample 7: 498480 detaches the weapon, task 51 497c50 places it
 * .6 along the heading with speed .24 (the held-back variant also rises
 * 0xf5c), then 428cf0 finishes clip 229. */
void tanya_boomerang_release(Fighter *fighter) {
  Projectile *projectile = weapon_release_object(fighter, 600, boomerang_step);
  if (projectile != NULL) {
    projectile->phase = BOOMERANG_OUT;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      q16 heading = fighter_axis(fighter, axis);
      projectile->pos[axis] = fighter->body.pos[axis] + q16_mul(heading, 0x9999);
      projectile->vel[axis] = q16_mul(heading, 0x3d70);
    }
    projectile->vel[AXIS_Y] = fighter->native_variant ? -0xf5c : 0;
  }
  native_begin(fighter, 8, 44, boomerang_ready);
  for (int pose = 8; pose <= 44; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void boomerang_start(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &boomerang_throw);
  fighter->native_variant = fighter_holds_back(fighter);
  tanya_entry_retail_script_004ed190(fighter);
}

/* 495610: 46dbb0's LP throw gate first, then T2[0x16]/[0x17]. */
static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (mk4_can_ground_throw(fighter, false)) {
    fighter_enter_state(fighter, S_THROW);
  } else {
    boomerang_start(fighter);
  }
}

void tanya_script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  tanya_entry_retail_script_004e62d8(fighter);
}

/* 46d1d0: armed -> T2[0xd] throw; follow-up window -> 494610 equips, sets
 * up the victim and 4ecb40[0x6d-0x60] = 4955b0; otherwise draw 4e62c0. */
void tanya_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_ground_hp(fighter);
  } else {
    weapon_draw(fighter, &boomerang);
    tanya_entry_retail_script_004e62c0(fighter);
  }
}

/* 490bd0 as a group callback keeps the falling victim facing its peer. */

/* Reactions 169 (486710 -> 4e9db8) and 170 (4821a0 -> 4e8bb0) both disarm
 * through 494520 before their generated victim streams. */
void tanya_weapon_reaction(Fighter *fighter) {
  require(fighter->reaction == 169 || fighter->reaction == 170, "Invalid Tanya weapon victim reaction");
  mk4_script_disarm_self(fighter);
  if (fighter->reaction == 169) {
    fighter_start_move(fighter, S_HIT, &victim_hit);
    fighter->flags |= FF_REACTING;
    tanya_entry_retail_script_004e9db8(fighter);
  } else {
    fighter_start_move(fighter, S_KNOCKDOWN, &victim_fall);
    fighter->flags |= FF_REACTING;
    tanya_entry_retail_script_004e8bb0(fighter);
  }
}
