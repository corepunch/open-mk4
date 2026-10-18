#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Native controller return used by generated VM programs and native runners. */
static void raiden_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/raiden.inc"
#include "generated/frames_raiden.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {71368, -56950, 83820, 74645, 16, 13, 6, 0, 8519};       /* 0x4ebaf8, B selector 2 */
static const AttackRecord sweep_record = {94371, 34072, 57016, 32768, 19, 19, 4, 1, 9175}; /* 0x4ebb70, B selector 36 */
// clang-format on

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void raiden_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void raiden_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void raiden_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */

static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {123, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

/* Phase branches are ordinary C; blocking playback owns only samples/events. */

static void uppercut_script(Fighter *fighter) {
  native_uppercut_tail(fighter, raiden_uppercut_contact, raiden_ready);
}

static void raiden_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  fighter_start_fiber(fighter, retail_script_004e62a0, NULL);
}

static void raiden_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e6798, NULL);
}

static void raiden_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void raiden_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter_start_fiber(fighter, retail_script_004e5ca0, NULL);
}

static void armed_idle(Fighter *fighter);
static void armed_walk(Fighter *fighter);
static void armed_crouch(Fighter *fighter);
static void armed_block(Fighter *fighter);

static void enter_victory(Fighter *fighter);
static void enter_throw(Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_THROW] = enter_throw,
    [S_VICTORY] = enter_victory,
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = raiden_jab,
    [S_KICK] = raiden_kick,
    [S_SWEEP] = raiden_sweep,
    [S_UPPERCUT] = raiden_uppercut,
};

static Fighter *opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */
enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK,
  G_ROUNDHOUSE
};

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 70, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 52, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 64, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14}, 14, 39, 67},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, raiden_ready, 5, 2, 27, 19, 20, 44, NULL};

static void ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

static void ground_attack(Fighter *fighter, int index) {
  bool followup = fighter_fast_start(fighter);
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {retail_script_004e5d90, ground_native_tail, retail_script_004e6158, retail_script_004e6020, retail_script_004e61e0,
      retail_script_004e69b0, retail_script_004e6190, retail_script_004e6800};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, raiden_ready};

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void raiden_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, hp_script, NULL);
}

static void raiden_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void raiden_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void raiden_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void raiden_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void raiden_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void raiden_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void raiden_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    raiden_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    raiden_kick(fighter);
  }
}

static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

static void press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : raiden_hp);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? raiden_roundhouse : raiden_hk);
}

static void enter_uppercut(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

static void enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

static void press_lp(Fighter *fighter) {
  fighter_interrupt(fighter, can_ground_throw(fighter, false) ? enter_throw
                             : fighter->armed                 ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                              : raiden_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : raiden_lk);
}

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 6, 3, 20},
  {{.clip = 56, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {71434, -4587, 66846, 24903, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {55705, -2621, 51773, 38010, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;

static void air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  mk4_land(fighter);
}

static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, air_land};

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

static void start_air_attack(Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  if (combo) {
    fighter->body.gravity = 0;
  }
  fighter_upright(fighter);
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {retail_script_004e6658, retail_script_004e6710, retail_script_004e61f8, retail_script_004e6500, retail_script_004e63d8};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static void air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void air_hk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void air_lk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

static void press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

static void press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

static void press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_kick);
}

static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static void press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : raiden_crouch_lp);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, raiden_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, raiden_crouch_lk);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, raiden_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    raiden_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

static const ButtonTable air_ground_followup = {
    {[BUTTON_HIGH_PUNCH] = press_air_ground_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};

static void press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? raiden_roundhouse : raiden_kick);
}

static void press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, enter_uppercut);
  } else {
    press_jab(fighter);
  }
}

static const ButtonTable close_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_combo_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable close_hk_followup = {
    {[BUTTON_HIGH_PUNCH] = press_knee_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_combo_hk, [BUTTON_LOW_KICK] = press_lk}};

static void script_kick_tail(Fighter *fighter) {
  native_calls(fighter, 3, 1, raiden_kick_contact);
  retail_script_004e6998(fighter);
}

static void script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, raiden_sweep_contact);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(raiden_ready);
}

static void script_crouch_lk_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  ground_native_tail(fighter);
}

static void script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_combo_hp_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  ground_native_tail(fighter);
}

static void script_roundhouse_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  ground_native_tail(fighter);
}

static void script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    retail_script_004e6890(fighter);
  } else {
    retail_script_004e68a8(fighter);
  }
}

static void script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

static void script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

static void script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 6);
}

static void script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

static void script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void fan_start(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void spike_start(Fighter *fighter) {
  native_delay(fighter, maxi(0, 30 - g_battle.phase_ticks));
  retail_script_004e6950(fighter);
}

static void arena_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_scene_pose(fighter, id == FINISH_FAN ? 415 : 293, id == FINISH_FAN ? 130 : 20, 0);
  mk4_script_scene_pose(opponent(fighter), id == FINISH_FAN ? 416 : 13, id == FINISH_FAN ? 108 : 56, 0);
  fighter_start_fiber(fighter, id == FINISH_FAN ? fan_start : spike_start, NULL);
}

static void raiden_fan_fatality(Fighter *fighter) {
  arena_fatality(fighter, FINISH_FAN);
}

static void raiden_spike_fatality(Fighter *fighter) {
  arena_fatality(fighter, FINISH_SPIKE);
}

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent fibers after release; replacement cancels only a live pair. */
static const MoveDef s_lk_throw = {.clip = 33, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 149, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown = {.clip = 154, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

static void script_throw_getup(Fighter *victim) {
  native_until(victim, native_grounded);
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  mk4_script_bind(victim, 328, 15, 0);
  mk4_script_finish_clip(victim, 1);
  native_delay(victim, 1);
  mk4_script_thrown_finish(victim);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_throw_start(fighter, lk ? &s_lk_throw : &s_throw, lk ? &s_lk_thrown : &s_lp_thrown, lk ? retail_script_004de218 : retail_script_004de798);
}

static void enter_throw(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
    return;
  }
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void script_thrown_return(Fighter *fighter) {
  if (fighter->reaction == 102 && fighter_move(fighter)->clip == 143) {
    script_throw_getup(fighter);
  }
  mk4_script_ready(fighter);
}

static void script_lk_victim_end(Fighter *victim) {
  native_until(victim, native_grounded);
  battle_scene_impact(victim);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

static void lk_victim(Fighter *victim) {
  mk4_script_bind(victim, 169, 43, 0);
  fighter_set_speed(victim, 0xf5c); /* 490ab0 consumes 4e99bc's speed. */
  retail_script_004e99c0(victim);
}

static void script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 187) {
    Fighter *victim = opponent(fighter);
    mk4_script_scene_pose(victim, 574, 39, 0);
    fighter_start_fiber(victim, retail_script_004e5770, NULL);
    return;
  }
  if (reaction == 175) {
    return; /* Arena spike coordinator owns the victim. */
  }
  require(reaction == 102 || reaction == 107 || reaction == 111, "Unexpected Raiden paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_fiber(victim, reaction == 111 ? lk_victim : reaction == 107 ? retail_script_004e9b40 : retail_script_004e9bd0, NULL);
}

static void script_lp_select(Fighter *fighter) {
  /* 42d1b0 compares camera-side ordering and squared pair range (40).
   * Resolve camera-side ordering along our fighter axis. */
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    retail_script_004de6b8(fighter);
  } else {
    retail_script_004de640(fighter);
  }
}

static void script_lp_end(Fighter *fighter) {
  retail_script_004de668(fighter);
}

static void script_lp_return(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(raiden_ready);
}

/* 49c300 spawns independently of the owner's generated recovery. */
static const MoveDef s_projectile = {.clip = 17, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x305};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 66, 10, 2, 2, 9175};
static const MeshAnimation projectile_mesh = {.first = 41, .count = 1, .period = 1, .scale = Q16_ONE};

static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

static void script_lightning_spawn(Fighter *fighter, int task_type) {
  require(task_type == 65, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0x28f5, 0, 48);
}

static void raiden_lightning(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  fighter_start_fiber(fighter, retail_script_004ed5f0, NULL);
}

static void special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static const MoveDef s_torpedo = {.clip = 94, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x202};
static const MoveDef s_torpedo_victim = {.clip = 135, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const AttackRecord torpedo_record = {85852, -3932, 62914, 20971, 82, 82, 0, 0, 9830};

static void torpedo_pose(Fighter *fighter, float pose) {
  fighter->sample_override = (int)pose;
}

static void torpedo_cycle(Fighter *fighter) {
  native_motion_begin(fighter, 22, NULL);
  for (int frame = 0; frame < 22; ++frame) {
    native_motion_frame(fighter, frame, torpedo_pose, (float)(20 + frame / 2), NULL);
  }
  fighter->native_motion.loop = true;
  native_motion_play(fighter);
}

static void script_torpedo_motion(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  int delay = fighter->ticks & 1;
  int count = delay + (30 - pose) * 2;
  if (count <= 0) {
    torpedo_cycle(fighter);
  } else {
    native_motion_begin(fighter, count, torpedo_cycle);
    for (int frame = 0; frame < count; ++frame) {
      int sample = pose + (frame >= delay ? 1 + (frame - delay) / 2 : 0);
      native_motion_frame(fighter, frame, torpedo_pose, (float)sample, NULL);
    }
    native_motion_play(fighter);
  }
  native_motion_step(fighter);
}

static unsigned script_torpedo_query(Fighter *fighter) {
  return battle_contact(fighter, &torpedo_record) == CONTACT_MISS ? 0u : 1u;
}

static void script_torpedo_miss(Fighter *fighter) {
  fighter->body.physics = NULL;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.vel[axis] = q16_mul(fighter->body.vel[axis], 0x4000);
  }
  fighter->body.gravity = 0x28f;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(raiden_ready);
}

static void script_torpedo_step(Fighter *fighter) {
  int64_t radius2 = 0, next2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    int64_t pos = fighter->body.pos[axis];
    int64_t next = pos + fighter->body.vel[axis];
    radius2 += pos * pos;
    next2 += next * next;
  }
  if (radius2 >= (int64_t)10 * Q16_ONE * 10 * Q16_ONE && next2 >= radius2) {
    script_torpedo_miss(fighter);
  }
}

static void torpedo_pair(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = fighter->body.pos[AXIS_Y];
  if (fighter_distance(fighter) > 10.f) {
    fighter_stop_mmove(fighter);
  }
}

static void script_torpedo_contact(Fighter *fighter) {
  fighter->body.physics = NULL;
  if (fighter->contact != CONTACT_BLOCK) {
    Fighter *victim = opponent(fighter);
    mk4_script_pair_victim(fighter, &s_torpedo_victim);
    fighter_start_fiber(victim, retail_script_004ea108, NULL);
    native_calls(fighter, 50, 0, torpedo_pair);
    mk4_script_release_pair(fighter);
    battle_react(victim, 83);
  }
  fighter->action = 0x1010;
  fighter_set_speed(fighter, -0xccc);
  fighter->body.vel[AXIS_Y] = -0x23d7;
  fighter->body.gravity = 0x168;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(raiden_ready);
}

static void raiden_torpedo(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  bool fast = fighter_fast_start(fighter);
  fighter_start_move(fighter, S_KICK, &s_torpedo);
  fighter_set_speed(fighter, 0x2666);
  mk4_script_save_motion(fighter);
  fighter_start_fiber(fighter, fast ? retail_script_004e8440 : retail_script_004e8458, special_cleanup);
}

static const MoveDef s_teleport = {.clip = 304, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x20c};

static bool teleport_below_ground(const Fighter *fighter) {
  return fighter->body.pos[AXIS_Y] >= fighter->body.ground + Q16_ONE;
}

static void teleport_script(Fighter *fighter) {
  fighter->body.vel[AXIS_Y] = 0x4ccc;
  fighter->body.gravity = 0;
  native_until(fighter, teleport_below_ground);
  const Fighter *victim = fighter_opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    fighter->body.pos[axis] = victim->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x10000);
  }
  fighter->body.pos[AXIS_Y] = fighter->body.ground - 3 * Q16_ONE;
  fighter->body.vel[AXIS_Y] = -0x451e;
  fighter->body.gravity = 0x28f;
  fighter->flags |= FF_UNTOUCHABLE;
  native_until(fighter, native_grounded);
  fighter->flags &= ~(unsigned)FF_UNTOUCHABLE;
  fighter->teleport_ready = g_battle.ticks + 32;
  native_delay(fighter, 4);
  fighter_fiber_finish(raiden_ready);
}

static void raiden_teleport(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_teleport);
  battle_sound(SOUND_A, 0x30);
  fighter_start_fiber(fighter, teleport_script, special_cleanup);
}

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 233, .last = 44, .flags = RETAIL, .action = 0x112}, {104857, -27525, 70778, 20971, 51, 51, 119, 0, 9830}, 0, 8, 16, 5, 0, 0, 0, 0, 34}, /* 4ec810 */
  {{.clip = 234, .last = 44, .flags = RETAIL, .action = 0x112}, {94371, -24903, 74711, 22282, 47, 47, 119, 0, 9830}, 0, 8, 16, 4, 0, 0, 0, 0, 34}, /* 4ec848 */
  {{.clip = 238, .last = 36, .flags = RETAIL, .action = 0x112}, {114688, -24903, 71434, 35389, 53, 10, 119, 0, 9830}, 0, 8, 25, 3, 0, 8, 17, 4587, 34}, /* 4ec838 */
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = 0x112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 5242, 34}, /* 4ec870 */
};
// clang-format on

static const WeaponDef native_weapon;

static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, true, retail_script_004e66d0);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[0], true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, false, retail_script_004e66f0);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[1], true);
  }
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[2], true);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[3], true);
}

static const WeaponDef native_weapon = {0x61, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_RAIDEN_STAFF};

static void script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_script_004e62d8(fighter);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void raiden_weapon(Fighter *fighter) {
  if (fighter->armed) {
    weapon_throw(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_hp(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
}

static void armed_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

static void armed_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

static void armed_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

static void armed_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

static void raiden_sidestep(Fighter *fighter) {
  mk4_sidestep(fighter);
}

/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
static void scene_end_watch(Fighter *fighter) {
  int clip = fighter_move(fighter)->clip;
  if (g_battle.phase != PHASE_FATALITY) {
    fighter->body.physics = NULL;
    return;
  }
  if (fighter->sample_override == fighter_move(fighter)->last && (clip == 573 || clip == 388 || clip == 114)) {
    fighter->body.physics = NULL;
    battle_end_finisher();
  }
}

static void script_victory(Fighter *fighter) {
  retail_script_004e6a40(fighter);
}

static void script_victory_effect(Fighter *fighter) {
  battle_particles(fighter, 10, 4, true);
  battle_particles(fighter, 13, 4, true);
}

static void script_first_release(Fighter *fighter) {
  mk4_script_scene_pose(opponent(fighter), 393, 25, 0);
  battle_particles(opponent(fighter), 2, 12, true);
  retail_script_004e0220(fighter);
}

static void first_end_shake(Fighter *fighter) {
  mk4_script_shake(fighter, 8);
}

static void first_end_particles(Fighter *fighter) {
  battle_particles(opponent(fighter), 2, 24, true);
}

static void script_first_end(Fighter *fighter) {
  first_end_shake(fighter);
  native_begin(fighter, 1, 60, NULL);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), frame == 30 ? first_end_shake : frame == 28 || frame == 58 ? first_end_particles : NULL);
  }
  native_wait(fighter);
  battle_shatter(opponent(fighter));
  battle_end_finisher();
  script_victory(fighter);
}

static void script_second_blend(Fighter *fighter) {
  mk4_script_blend_to(fighter, 123, 101, 100, 0xccc, 0x1999);
  retail_script_004e56d8(fighter);
}

static void script_second_particles(Fighter *fighter) {
  battle_particles(fighter, 10, 8, true);
}

static void script_second_effect(Fighter *fighter) {
  battle_particles(opponent(fighter), 18, 12, true);
  retail_script_004e56f0(fighter);
}

static void script_second_release(Fighter *fighter) {
  battle_particles(opponent(fighter), 2, 24, true);
  retail_script_004e5740(fighter);
}

static void script_second_end(Fighter *fighter) {
  fighter->body.physics = scene_end_watch;
  retail_script_004e5760(fighter);
}

static void script_second_victim_end(Fighter *fighter) {
  battle_particles(fighter, 2, 24, true);
  retail_script_004e5788(fighter);
}

static void first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 392, 25, 0);
  mk4_script_scene_pose(opponent(fighter), 393, 25, 0);
  fighter_start_fiber(fighter, retail_script_004e0210, NULL);
}

static void second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 123, 101, 0);
  mk4_script_scene_pose(opponent(fighter), 13, 56, 0);
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x14000);
  }
  fighter_start_fiber(fighter, retail_script_004e56c8, NULL);
}

static void enter_victory(Fighter *fighter) {
  fighter_start_move(fighter, S_VICTORY, &s_victory);
  fighter_start_fiber(fighter, script_victory, NULL);
}

static bool can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_close_fatality(fighter);
}

static bool can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready;
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_UP, {TOKEN_DOWN}, 1, 32, 0x0, can_teleport, raiden_teleport},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK}, 2, 26, 0x0, can_weapon, raiden_weapon},
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_UP, TOKEN_UP, TOKEN_UP}, 4, 50, 0x0, can_close_fatality, second_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 16, 0x0, NULL, raiden_lightning},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_DOWN}, 3, 40, 0x0, can_spike_fatality, raiden_spike_fatality},
  {BUTTON_BLOCK, {TOKEN_DOWN, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0x40, can_fan_fatality, raiden_fan_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_UP, TOKEN_UP}, 4, 50, 0x40, can_close_fatality, first_fatality},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 26, 0x10, NULL, raiden_torpedo},
  {BUTTON_RUN, {0}, 0, 0, 0x0, mk4_can_sidestep, raiden_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0x0, weapon_can_pickup, weapon_pickup},
};
// clang-format on

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

static void cpu_close_hp(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HP);
}

static void cpu_close_hk(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HK);
}

static void cpu_air_hk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 3 : 2);
}

static void cpu_air_lk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 4 : 2);
}

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = raiden_jab,
            [CPU_LOW_KICK] = raiden_lk,
            [CPU_HIGH_KICK] = raiden_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = raiden_crouch_lp,
            [CPU_AIR_HP] = air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_DRAW] = raiden_weapon,
            [CPU_FINISH_1] = first_fatality,
            [CPU_FINISH_2] = second_fatality,
            [CPU_FINISH_FAN] = raiden_fan_fatality,
            [CPU_FINISH_SPIKE] = raiden_spike_fatality,
            [CPU_PROJECTILE] = raiden_lightning,
            [CPU_SPECIAL_NEAR] = raiden_torpedo,
            [CPU_SPECIAL_1] = raiden_teleport,
            [CPU_SPECIAL_2] = raiden_torpedo,
        },
    .finish_min = {0, 0},
    .finish_distance = {0x10000, 0x10000},
};

const CharacterDef mk4_raiden = {GAME_MK4, 1, "RAIDEN", &mk4_motion, moves, S_COUNT, &controls, enter, -75497, mk4_walk, mk4_jump, &cpu_moves};

static void script_weapon_victim_getup(Fighter *victim) {
  mk4_script_finish_clip(victim, 1);
  script_throw_getup(victim);
}

static void script_reaction_side_getup(Fighter *victim) {
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  mk4_script_bind(victim, 185, 16, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

static void script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_delay(victim, 20);
  mk4_script_finish_clip(victim, 1);
  script_throw_getup(victim);
}

static void script_torpedo_victim_motion(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (!(victim->flags & FF_PAIRED)) {
    victim->body.physics = NULL;
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = owner->body.pos[axis] + q16_mul(fighter_axis(owner, axis), 0x18000);
  }
  victim->body.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
}

void raiden_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 47:
      script = retail_script_004ea068;
      battle_wall_recoil(victim);
      break;
    case 53:
      script = retail_script_004e8e18;
      break;
    case 66:
      script = retail_script_004e9d50;
      break;
    case 82:
      script = retail_script_004ea108;
      break;
    case 83:
      script = fighter_airborne(victim) ? retail_script_004e9ae8 : retail_script_004e9ad0;
      break;
    default:
      require(false, "Unexpected Raiden record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  fighter_start_fiber(victim, script, NULL);
}
