#include "game.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

#define WEAPON_MOVE(c, a, end) {.clip = c, .last = end, .flags = SF_LOCK | SF_RECORD, .action = a}
static const MoveDef draw_move = WEAPON_MOVE(244, ACTION_RETAIL_1004, 49);
static const MoveDef throw_move = WEAPON_MOVE(208, ACTION_RETAIL_030F, 60);
static const MoveDef pickup_move = WEAPON_MOVE(209, ACTION_RETAIL_2006, 14);
static const MoveDef air_hp = WEAPON_MOVE(252, ACTION_RETAIL_0603, 9);
static const MoveDef air_lp = WEAPON_MOVE(251, ACTION_RETAIL_0604, 9);
static const MoveDef landing = WEAPON_MOVE(304, ACTION_RETAIL_2008, 27);
static const MoveDef blend_stance = WEAPON_MOVE(286, ACTION_RETAIL_1006, 0);
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

void weapon_stance(Fighter *fighter, int state) {
  const MoveDef *move = roster_def(fighter->roster)->moves[state];
  if (fighter->armed) {
    move = state == S_IDLE ? &idle : state == S_WALK ? (fighter_holds_back(fighter) ? &back_walk : &walk) : state == S_CROUCH ? &crouch : &block;
  }
  if (state == S_WALK && fighter->state == S_WALK && fighter->current_move == move) {
    return;
  }
  fighter_start_move(fighter, state, move);
}

static bool weapon_wait(const Fighter *fighter) {
  (void)fighter;
  return false;
}

static void weapon_hold(Fighter *fighter, const mmove_t *phase) {
  int sample = fighter_sample(fighter);
  fighter_mmove_phase(fighter, phase);
  fighter->sample_override = sample;
}

static void weapon_cleanup(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->blend_duration = 0;
}

static void blend_tick(Fighter *fighter) {
  ++fighter->blend_ticks;
}

static const int blend_frames_samples[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t blend_frames[10] = {
  [0] = {.thinkfunc = blend_tick},
  [1] = {.thinkfunc = blend_tick},
  [2] = {.thinkfunc = blend_tick},
  [3] = {.thinkfunc = blend_tick},
  [4] = {.thinkfunc = blend_tick},
  [5] = {.thinkfunc = blend_tick},
  [6] = {.thinkfunc = blend_tick},
  [7] = {.thinkfunc = blend_tick},
  [8] = {.thinkfunc = blend_tick},
  [9] = {.thinkfunc = blend_tick},
};
// clang-format on
static const mmove_t blend_phase = {
    .firstframe = 0, .lastframe = 9, .frame = blend_frames, .endfunc = fighter_ready, .samples = blend_frames_samples, .cleanup = weapon_cleanup};

static void strike_effects(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  if (fighter->ticks == strike->windup - strike->first + 1) {
    battle_sound(SOUND_C, strike->sound);
  }
  bool moving = fighter->weapon_travel && !fighter->connected && fighter->ticks >= strike->delay && fighter->ticks < strike->delay + strike->travel;
  fighter_set_speed(fighter, moving ? strike->speed : 0);
}

static void strike_done(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  if (fighter->connected && strike->recovery == 9) {
    int clip = strike->move.clip, sample = fighter_sample(fighter);
    fighter_start_mmove(fighter, S_PUNCH, &blend_stance, &blend_phase);
    fighter->blend_clip = clip;
    fighter->blend_sample = sample;
    fighter->blend_ticks = 0;
    fighter->blend_duration = 10;
  } else {
    fighter_finish_mmove(fighter, fighter_ready);
  }
}

static void strike_recover_tick(Fighter *fighter) {
  strike_effects(fighter);
  if (fighter_sample(fighter) >= fighter->weapon_strike->move.last) {
    strike_done(fighter);
  } else {
    fighter->sample_override = fighter_sample(fighter) + 1;
  }
}

static void strike_recover_frames_frame_0(Fighter *fighter) {
  uint64_t continuation = fighter->continuation;
  strike_recover_tick(fighter);
  if (fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  fighter->mmove_hold = !weapon_wait(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t strike_recover_frames[1] = {
  [0] = {.thinkfunc = strike_recover_frames_frame_0},
};
// clang-format on
static const mmove_t strike_recover_phase = {.firstframe = 0, .lastframe = 0, .frame = strike_recover_frames, .endfunc = fighter_stop_mmove};

static void strike_recover(Fighter *fighter) {
  weapon_hold(fighter, &strike_recover_phase);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t strike_pause_frames[1] = {
  [0] = {.thinkfunc = strike_effects},
};
// clang-format on
static const mmove_t strike_pause_phase = {.firstframe = 0, .lastframe = 0, .frame = strike_pause_frames, .endfunc = strike_recover};

static void strike_tick(Fighter *fighter) {
  const WeaponStrike *strike = fighter->weapon_strike;
  int active = strike->target - strike->first + 1;
  strike_effects(fighter);
  fighter->sample_override = strike->first + fighter->ticks;
  if (fighter->ticks >= active) {
    battle_contact(fighter, &strike->record);
  }
  if (!fighter->connected && fighter->ticks < active + strike->queries - 1) {
    return;
  }
  fighter->script_phase = 1;
  fighter->action = ACTION_RETAIL_1006;
  fighter_set_speed(fighter, 0);
  weapon_hold(fighter, fighter->connected && (strike->recovery == 5 || strike->recovery == 9) ? &strike_pause_phase : &strike_recover_phase);
}

static void strike_frames_frame_0(Fighter *fighter) {
  uint64_t continuation = fighter->continuation;
  strike_tick(fighter);
  if (fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  fighter->mmove_hold = !weapon_wait(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t strike_frames[1] = {
  [0] = {.thinkfunc = strike_frames_frame_0},
};
// clang-format on
static const mmove_t strike_phase = {.firstframe = 0, .lastframe = 0, .frame = strike_frames, .endfunc = fighter_stop_mmove, .cleanup = weapon_cleanup};

void weapon_start_strike(Fighter *fighter, const WeaponStrike *strike, bool travel) {
  require(strike->first <= strike->windup && strike->windup <= strike->target && strike->queries > 0, "Invalid weapon phases");
  fighter_start_mmove(fighter, S_PUNCH, &strike->move, &strike_phase);
  fighter->weapon_strike = strike;
  fighter->weapon_travel = travel;
  fighter->sample_override = strike->first;
}

/* 494610 / 4946f0. This is separate from a back-punch's motion setup. */
void weapon_followup_victim(Fighter *fighter) {
  Fighter *victim = &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
  if (!fighter_airborne(victim)) {
    battle_react(victim, 120);
  }
  if (victim->hits > 3) {
    victim->damage_scale = 0x4ccc;
  }
}

typedef struct WeaponBounce {
  q16 spin, rise, rest_angle;
} WeaponBounce;

/* 53d580 selects 4ecaf0 for both currently ported weapons.
 * 4941b0/230/390 play three explicit arcs, rather than one toss. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponBounce bounces[] = {
  {0x20c4, -0x2666, 0x4b5c2},
  {0x23d7, -0x1999, 0x191eb},
  {0, -0x147a, 0x191eb},
};
// clang-format on

static void begin_bounce(Projectile *p) {
  p->phase = 1;
  p->lifetime = 180;
  p->spin[AXIS_Z] = bounces[p->bounce].spin;
  p->vel[AXIS_Y] = bounces[p->bounce].rise;
  q16 speed = (q16)battle_random(2 * 0xa3d) - 0xa3d;
  p->vel[AXIS_X] = q16_mul(p->heading[0], speed);
  p->vel[AXIS_Z] = q16_mul(p->heading[1], speed);
}

static void object_tick(Projectile *p) {
  if (p->phase == 2) {
    p->lifetime = 2;
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
    p->angle[k] = (p->angle[k] + p->spin[k]) % 0x6487e;
  }
  if (p->phase == 0) {
    ContactResult result = battle_contact_from(&g_battle.fighters[p->owner], &thrown_record, p->pos, p->facing);
    if (result != CONTACT_MISS) {
      begin_bounce(p);
    } else if (p->ticks >= 30 ||
               (int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] >= (int64_t)g_battle.arena_radius * g_battle.arena_radius) {
      p->kind = 0;
    }
  } else {
    p->vel[AXIS_Y] += 0x28f;
    if (p->pos[AXIS_Y] >= -0x1999 && p->vel[AXIS_Y] >= 0) {
      p->pos[AXIS_Y] = -0x1999;
      memset(p->vel, 0, sizeof p->vel);
      memset(p->spin, 0, sizeof p->spin);
      p->angle[AXIS_Z] = bounces[p->bounce].rest_angle;
      battle_sound(SOUND_C, 0x15);
      if (++p->bounce < (int)(sizeof bounces / sizeof bounces[0])) {
        begin_bounce(p);
      } else {
        p->phase = 2;
        p->lifetime = 2;
      }
    }
  }
}

static void drop(Fighter *fighter) {
  if (!fighter->armed) {
    return;
  }
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SWORD, 180, object_tick);
  if (p != NULL) {
    p->weapon = fighter->weapon;
    Vec3 position;
    Mat3 unused;
    fighter_joint_position(fighter, 10, &position, &unused);
    p->pos[AXIS_X] = (q16)(position.x * Q16_ONE);
    p->pos[AXIS_Y] = (q16)(position.y * Q16_ONE);
    p->pos[AXIS_Z] = (q16)(position.z * Q16_ONE);
    begin_bounce(p);
  }
  fighter->armed = false;
}

void fighter_equip_weapon(Fighter *fighter, const WeaponDef *weapon) {
  fighter->armed = true;
  fighter->weapon_used = true;
  fighter->disarm = drop;
  fighter->weapon = weapon;
}

static void landing_script(Fighter *fighter) {
  fighter_fiber_play_to(fighter, 27, 1);
  fighter_wait_ticks(1);
  fighter_fiber_finish(fighter_ready);
}

static void air_script(Fighter *fighter) {
  const bool high = fighter_move(fighter)->clip == 252;
  for (int tick = 1;; ++tick) {
    fighter_wait_ticks(1);
    fighter->sample_override = mini(9, tick);
    if (!fighter->connected && tick >= (high ? 8 : 9) && tick <= 10) {
      battle_contact(fighter, &air_records[high ? 0 : 1]);
    }
    if (!fighter_airborne(fighter)) {
      break;
    }
  }
  fighter_fiber_move(fighter, S_JUMP, &landing);
  fighter->sample_override = 18;
  landing_script(fighter);
}

void weapon_air_native(Fighter *fighter, unsigned selector) {
  require(selector == 0x104 || selector == 0x108, "Invalid generated weapon-air selector");
  require(fighter_move(fighter)->clip == (selector == 0x104 ? 252 : 251), "Weapon-air native entry differs from selected program");
  air_script(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t weapon_land_frames[10] = {{0}};
// clang-format on
static const mmove_t weapon_land = {.firstframe = 18, .lastframe = 27, .frame = weapon_land_frames, .endfunc = fighter_ready};

static void weapon_air_tick(Fighter *fighter) {
  bool high = fighter_move(fighter)->clip == 252;
  int tick = ++fighter->script_timer;
  fighter->sample_override = mini(9, tick);
  if (!fighter->connected && tick >= (high ? 8 : 9) && tick <= 10) {
    battle_contact(fighter, &air_records[high ? 0 : 1]);
  }
  if (!fighter_airborne(fighter)) {
    fighter_start_mmove(fighter, S_JUMP, &landing, &weapon_land);
  }
}

static void weapon_air_frames_table_frame_0(Fighter *fighter) {
  uint64_t continuation = fighter->continuation;
  weapon_air_tick(fighter);
  if (fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  fighter->mmove_hold = !weapon_wait(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t weapon_air_frames_table[1] = {
  [0] = {.thinkfunc = weapon_air_frames_table_frame_0},
};
// clang-format on
static const mmove_t weapon_air_phase = {.firstframe = 0, .lastframe = 0, .frame = weapon_air_frames_table, .endfunc = fighter_stop_mmove};

void weapon_air_frames_native(Fighter *fighter, unsigned selector) {
  require(selector == 0x104 || selector == 0x108, "Invalid generated weapon-air selector");
  require(fighter_move(fighter)->clip == (selector == 0x104 ? 252 : 251), "Weapon-air phase differs from selected program");
  fighter->script_timer = 0;
  weapon_hold(fighter, &weapon_air_phase);
}

void weapon_air_frames(Fighter *fighter, bool high, FighterFn script) {
  fighter_start_move(fighter, S_JUMP, high ? &air_hp : &air_lp);
  fighter->sample_override = 0;
  fighter->body.on_land = NULL;
  script(fighter);
}

void weapon_air_attack(Fighter *fighter, bool high, FighterFn script) {
  fighter_start_move(fighter, S_JUMP, high ? &air_hp : &air_lp);
  fighter->sample_override = 0;
  fighter->body.on_land = NULL;
  fighter_start_fiber(fighter, script, NULL);
}

void weapon_draw(Fighter *fighter, const WeaponDef *weapon) {
  fighter_start_move(fighter, S_PUNCH, &draw_move);
  fighter->weapon = weapon;
  fighter->sample_override = 0;
  /* The character starts its generated 4e62c0/4e62d8 program. */
}

static void throw_release(Fighter *fighter) {
  if (!fighter->armed) {
    return;
  }
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SWORD, 90, object_tick);
  if (p != NULL) {
    p->weapon = fighter->weapon;
    p->spin[AXIS_Z] = -0x6666; /* 493a70 / 493c20 */
    for (int k = 0; k < 2; ++k) {
      int axis = k == 0 ? AXIS_X : AXIS_Z;
      p->pos[axis] += q16_mul(fighter->heading[k], 0xc000); /* 498480: .75 along heading */
      p->vel[axis] = q16_mul(fighter->heading[k], 0x3333);
    }
    p->pos[AXIS_Y] = -0x13333;
  }
  fighter->armed = false;
}

enum {
  THROW_FIRST,
  THROW_RELEASE = 21,
  THROW_LAST = 61
};

/* 498290: release on the 22nd simulation frame, then recover through 62.
 * Pose ten is repeated once; source poses and action frames are independent. */
static const int throw_samples[THROW_LAST + 1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t throw_frames[THROW_LAST + 1] = {
  [THROW_RELEASE] = {NULL, 0.0f, throw_release},
};
// clang-format on
static const mmove_t throw_phase = {
    .firstframe = THROW_FIRST, .lastframe = THROW_LAST, .frame = throw_frames, .endfunc = fighter_ready, .samples = throw_samples};

void weapon_throw(Fighter *fighter) {
  fighter_start_mmove(fighter, S_PUNCH, &throw_move, &throw_phase);
  fighter->sample_override = 0;
}

static int pickup_index(const Fighter *fighter) {
  if (fighter->armed || fighter->action != ACTION_RETAIL_2001) {
    return -1;
  }
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    q16 dx = p->pos[AXIS_X] - fighter->body.pos[AXIS_X], dz = p->pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
    if (p->kind == PROJECTILE_SWORD && p->phase == 2 && q16_mul(dx, dx) + q16_mul(dz, dz) <= Q16_ONE) {
      return i;
    }
  }
  return -1;
}

bool weapon_can_pickup(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && pickup_index(fighter) >= 0;
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t pickup_phase_frames[15] = {{0}};
// clang-format on
static const mmove_t pickup_phase = {.firstframe = 0, .lastframe = 14, .frame = pickup_phase_frames, .endfunc = fighter_ready};

void weapon_pickup(Fighter *fighter) {
  int index = pickup_index(fighter);
  if (index < 0) {
    fighter_ready(fighter);
    return;
  }
  g_battle.projectiles[index].weapon->equip(fighter);
  battle_sound(SOUND_C, fighter == &g_battle.fighters[0] ? 0x26 : 0x27);
  g_battle.projectiles[index].kind = 0;
  fighter_start_mmove(fighter, S_PUNCH, &pickup_move, &pickup_phase);
  fighter->sample_override = 0;
}
