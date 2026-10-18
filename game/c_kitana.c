#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Native controller return used by generated VM programs and native runners. */
static void kitana_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/kitana.inc"
#include "generated/frames_kitana.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {83230, -51118, 65536, 51118, 16, 13, 6, 0, 8519};       /* 0x4ebac8, B selector 0 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void kitana_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void kitana_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void kitana_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment uppercut_loop_segments[] = {{0, 8, -1, 1}, {3, 7, 1, 1}};
static const MoveSegment uppercut_late_segments[] = {{0, 9, 0, 1}, {2, 8, -1, 1}, {5, 7, 1, 1}};
static const MoveSegment uppercut_miss_end_segments[] = {{0, 8, 1, 1}};
static const MoveSegment uppercut_hit_end_segments[] = {{0, 9, 1, 1}};
// clang-format on
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, kitana_ready);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, kitana_ready);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, kitana_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, kitana_ready);

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 65, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_loop_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_late_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_miss_end_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_hit_end_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};

static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {114, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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
  for (int sample = 6; sample <= 8; ++sample) {
    fighter_wait_ticks(sample == 6 ? 2 : 1);
    fighter->sample_override = sample;
    kitana_uppercut_contact(fighter);
  }
  fighter_wait_ticks(1);
  ContactResult result = fighter->contact;
  bool late = fighter->contact_tick == 9;
  int loops = result == CONTACT_MISS ? 3 : result == CONTACT_BLOCK ? 2 : 8;
  for (int i = 0; i < loops; ++i) {
    const MoveDef *move = late && i == 0 ? &s_uppercut_late : &s_uppercut_loop;
    fighter_fiber_move(fighter, S_UPPERCUT, move);
    fighter->contact = result;
    fighter->connected = result != CONTACT_MISS;
    if (result != CONTACT_MISS) {
      fighter->action = ACTION_RETAIL_1006;
    }
    fighter_fiber_sequence(fighter, move->sequence);
  }
  const MoveDef *end = result == CONTACT_MISS ? &s_uppercut_miss_end : &s_uppercut_hit_end;
  fighter_fiber_move(fighter, S_UPPERCUT, end);
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
  fighter_fiber_sequence(fighter, end->sequence);
  fighter_fiber_finish(kitana_ready);
}

static void kitana_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  fighter_start_fiber(fighter, retail_script_004e62a0, NULL);
}

static void kitana_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e6798, NULL);
}

static void kitana_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void kitana_shadow_low(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_low);
  fighter_start_fiber(fighter, retail_script_004ed268, NULL);
}

static void kitana_shadow_up(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_up);
  fighter_start_fiber(fighter, retail_script_004ed2c0, NULL);
}

static bool kitana_shadow_up_enabled(const Fighter *fighter) {
  return fighter->body.gravity != 0;
}

static void kitana_kick(Fighter *fighter) {
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
    [S_PUNCH] = kitana_jab,
    [S_KICK] = kitana_kick,
    [S_SWEEP] = kitana_sweep,
    [S_UPPERCUT] = kitana_uppercut,
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
  {{.clip = 308, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 53, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, kitana_ready, 5, 2, 27, 19, 20, 44, NULL};

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

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, kitana_ready};

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void kitana_hp(Fighter *fighter) {
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

static void kitana_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void kitana_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void kitana_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void kitana_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void kitana_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void kitana_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void kitana_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    kitana_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    kitana_kick(fighter);
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
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : kitana_hp);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kitana_roundhouse : kitana_hk);
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
                                                              : kitana_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : kitana_lk);
}

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 57, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {74055, 1310, 39321, 19660, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 60, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
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
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : kitana_crouch_lp);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_crouch_lk);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, kitana_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    kitana_hp(fighter);
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
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kitana_roundhouse : kitana_kick);
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
  for (int visit = 0; visit < 3; ++visit) {
    fighter_wait_ticks(1);
    kitana_kick_contact(fighter);
  }
  retail_script_004e5d20(fighter);
}

static void script_sweep_tail(Fighter *fighter) {
  for (int visit = 0; visit < 2; ++visit) {
    fighter_wait_ticks(1);
    kitana_sweep_contact(fighter);
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(kitana_ready);
}

static void script_crouch_lk_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_combo_hp_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_roundhouse_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

static void script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

static void script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

static void script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

static void script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void script_air_release(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 85;
  fighter_start_fiber(victim, retail_script_004e98a0, NULL);
  battle_damage(victim, 0x2666);
  mk4_script_finish_clip(fighter, 1);
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = -0x51e;
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_fiber_finish(mk4_land);
}

static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void fan_start(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void spike_start(Fighter *fighter) {
  while (g_battle.phase_ticks < 30) {
    fighter_wait_ticks(1);
  }
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

static void kitana_fan_fatality(Fighter *fighter) {
  arena_fatality(fighter, FINISH_FAN);
}

static void kitana_spike_fatality(Fighter *fighter) {
  arena_fatality(fighter, FINISH_SPIKE);
}

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent fibers after release; replacement cancels only a live pair. */
static const MoveDef s_lk_throw = {.clip = 43, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 163, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown = {.clip = 156, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

static void script_throw_getup(Fighter *victim) {
  while (fighter_airborne(victim)) {
    fighter_wait_ticks(1);
  }
  if (victim->health <= 0) {
    mk4_script_thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  mk4_script_bind(victim, 328, 15, 0);
  mk4_script_finish_clip(victim, 1);
  fighter_wait_ticks(1);
  mk4_script_thrown_finish(victim);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_throw_start(fighter, lk ? &s_lk_throw : &s_throw, lk ? &s_lk_thrown : &s_lp_thrown, lk ? retail_script_004de360 : retail_script_004de798);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void lk_victim(Fighter *victim) {
  mk4_script_bind(victim, 203, 112, 75);
  mk4_script_advance(victim, 7, 1);
  mk4_script_finish_clip(victim, 1);
  while (fighter_airborne(victim)) {
    fighter_wait_ticks(1);
  }
  mk4_script_thrown_finish(victim);
}

static void script_leg_victim_end(Fighter *victim) {
  mk4_script_bind(victim, 185, 16, 0);
  while (fighter_airborne(victim)) {
    fighter_wait_ticks(1);
  }
  script_throw_getup(victim);
}

static void script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 184) {
    Fighter *victim = opponent(fighter);
    mk4_script_scene_pose(victim, 561, 79, 0);
    fighter_start_fiber(victim, retail_script_004e59c8, NULL);
    return;
  }
  if (reaction == 175) {
    return;
  }
  require(reaction == 78 || reaction == 79 || reaction == 103, "Unexpected Kitana paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_fiber(victim, reaction == 78 ? lk_victim : reaction == 79 ? retail_script_004e9a68 : retail_script_004e9b88, NULL);
}

static void script_lk_return(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_wait_ticks(20); /* Native stance blend, step 0xccc. */
  fighter_fiber_finish(kitana_ready);
}

/* 49c300 spawns independently of the owner's generated recovery. */
static const MoveDef s_projectile = {.clip = 15, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x313};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 68, 10, 2, 2, 7864};
static const MeshAnimation projectile_mesh = {.first = 42, .count = 2, .period = 1, .scale = Q16_ONE};

static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

static void script_ring_spawn(Fighter *fighter, int task_type) {
  require(task_type == 68, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0xccc, -0x1999, 48);
}

static void kitana_ring(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  fighter_start_fiber(fighter, retail_script_004ed4c0, NULL);
}

static void kitana_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

static void kitana_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

static void kitana_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_fiber_finish(kitana_ready);
}

static void kitana_command_shadow_low(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_low);
}

static void kitana_command_shadow_up(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_up);
}

static void special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

static void script_ring_flash(Fighter *fighter, int task_type) {
  require(task_type == 38, "Incorrect ring flash task");
  battle_particles(fighter, 2, 3, true);
}

static const MoveDef s_leg_grab = {.clip = 102, .last = 64, .flags = SF_LOCK | SF_RECORD, .action = 0x203};
static const MoveDef s_leg_victim = {.clip = 201, .last = 34, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_square_wave = {.clip = 72, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x201};
static const MoveDef s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};
static const MoveDef s_bicycle = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x205};
static const AttackRecord leg_record = {92405, -29491, 85852, 76677, 88, 88, 8, 1, 11796};
static const AttackRecord square_record = {19005, -9830, 39321, 57671, 87, 87, 0, 0, 13107};
static const AttackRecord cartwheel_record = {75300, -45088, 72083, 89063, 121, 121, 9, 0, 4587};
static const AttackRecord bicycle_record = {80609, -1310, 62259, 20971, 86, 86, 0, 0, 9830};

static void script_leg_grab_tail(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_set_speed(fighter, 0);
  fighter_fiber_finish(kitana_ready);
}

static void leg_grab_script(Fighter *fighter) {
  fighter_set_speed(fighter, 0x3333);
  for (int sample = 1; sample <= 11; ++sample) {
    fighter_wait_ticks(1);
    fighter->sample_override = sample;
    if (sample == 7) {
      fighter_set_speed(fighter, 0);
    }
  }
  if (battle_contact(fighter, &leg_record) == CONTACT_MISS || fighter->contact == CONTACT_BLOCK) {
    fighter->action = 0x1012;
    retail_script_004e85a8(fighter);
  } else {
    mk4_script_pair_victim(fighter, &s_leg_victim);
    retail_script_004e8578(fighter);
  }
}

static void kitana_leg_grab(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter->leg_grab_ready = g_battle.ticks + 64;
  fighter_start_move(fighter, S_KICK, &s_leg_grab);
  battle_sound(SOUND_A, 9);
  battle_sound(SOUND_INDEXED, 15);
  fighter_start_fiber(fighter, leg_grab_script, special_cleanup);
}

static void script_square_wave_tail(Fighter *fighter) {
  while (fighter_height(fighter) < 2.4f && !fighter->connected) {
    fighter_wait_ticks(1);
    battle_contact(fighter, &square_record);
  }
  fighter->body.gravity = 0x28f;
  fighter_set_speed(fighter, fighter->connected ? -0x1999 : 0);
  if (fighter->connected) {
    fighter_wait_ticks(32);
  }
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_set_speed(fighter, 0);
  fighter_fiber_finish(kitana_ready);
}

static void kitana_square_wave(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_square_wave);
  fighter_start_fiber(fighter, retail_script_004e85c0, special_cleanup);
}

static void script_cartwheel_tail(Fighter *fighter) {
  fighter->cartwheel_ready = g_battle.ticks + 48;
  for (int count = 0; count < 9; ++count) {
    fighter_wait_ticks(1);
    ++fighter->sample_override;
    if (battle_contact(fighter, &cartwheel_record) != CONTACT_MISS) {
      break;
    }
  }
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    fighter_wait_ticks(20); /* Native 47b8a0 stance blend, step 0xccc. */
  }
  fighter_fiber_finish(kitana_ready);
}

static void kitana_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &s_cartwheel);
  fighter_start_fiber(fighter, retail_script_004e81d0, special_cleanup);
}

static void bicycle_script(Fighter *fighter) {
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = -0x2666;
  battle_sound(SOUND_A, 6);
  battle_sound(SOUND_INDEXED, 12);
  while (fighter_height(fighter) < 2.5f) {
    fighter_wait_ticks(1);
  }
  stop_body(fighter);
  mk4_script_bind(fighter, 101, 9, 0);
  fighter_set_speed(fighter, 0x3333);
  for (int sample = 0; sample < 9 && !fighter->connected; ++sample) {
    fighter_wait_ticks(1);
    fighter->sample_override = sample;
    battle_contact(fighter, &bicycle_record);
  }
  fighter_set_speed(fighter, 0);
  fighter->body.gravity = 0x28f;
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_fiber_finish(kitana_ready);
}

static void kitana_bicycle(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_bicycle);
  fighter_start_fiber(fighter, bicycle_script, special_cleanup);
}

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike weapon_attacks[] = {
  {{.clip = 230, .last = 44, .flags = RETAIL, .action = 0x111}, {124518, -34078, 83230, 51118, 54, 54, 119, 0, 11796}, 0, 8, 11, 3, 1, 0, 0, 0, 34}, /* 4ec760 */
  {{.clip = 229, .last = 44, .flags = RETAIL, .action = 0x111}, {126484, -32768, 64880, 57016, 56, 56, 119, 0, 9830}, 0, 4, 7, 3, 1, 0, 0, 0, 32}, /* 4ec778 */
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

static const MoveDef s_weapon_back_hp = {.clip = 228, .last = 39, .flags = RETAIL, .action = 0x112};
static const MoveDef s_weapon_back_lp = {.clip = 227, .last = 59, .flags = RETAIL, .action = 0x112};
static const AttackRecord weapon_back_hp_record = {98959, -28835, 67502, 36700, 55, 55, 119, 0, 13107};
static const AttackRecord weapon_back_lp_record = {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175};

static void script_weapon_hp_tail(Fighter *fighter) {
  mk4_script_weapon_tail(fighter, &weapon_back_hp_record, 7);
}

static void script_weapon_lp_tail(Fighter *fighter) {
  mk4_script_weapon_tail(fighter, &weapon_back_lp_record, 2);
}

static void weapon_back_hp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_weapon_back_hp);
  mk4_script_schedule_motion(fighter, 0x1999, 3, 12);
  fighter_start_fiber(fighter, retail_script_004ec7a0, special_cleanup);
}

static void weapon_back_lp(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_weapon_back_lp);
  mk4_script_schedule_motion(fighter, 0x1999, 8, 13);
  fighter_start_fiber(fighter, retail_script_004ec7f0, special_cleanup);
}

static const WeaponDef native_weapon = {
    /* Kitana's roster slot selects the same descriptor 4ece38 and rig as
     * Sonya, so both use the shared retail weapon model. */
    0x62, weapon_hp, weapon_lp, weapon_back_hp, weapon_back_lp, NULL, weapon_throw, equip_weapon, SCENE_MODEL_SONYA_WEAPON};

static void script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_script_004e62d8(fighter);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void kitana_weapon(Fighter *fighter) {
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

static void kitana_sidestep(Fighter *fighter) {
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

static void script_first_release(Fighter *fighter) {
  battle_shatter(opponent(fighter));
  fighter->body.physics = scene_end_watch;
  retail_script_004e01d0(fighter);
}

static void script_second_pause(Fighter *fighter) {
  (void)fighter;
  fighter_wait_ticks(15);
}

static void script_second_cycle_end(Fighter *fighter) {
  (void)fighter; /* 466000 returns to the native two-cycle coordinator. */
}

static void script_second_branch(Fighter *fighter) {
  if (fighter->script_phase == 2) {
    retail_script_004e5980(fighter);
  } else {
    retail_script_004e5990(fighter);
  }
}

static void script_second_repeat(Fighter *fighter) {
  for (int remaining = 2; remaining > 0; --remaining) {
    fighter->script_phase = remaining;
    retail_script_004e5960(fighter);
  }
  fighter->body.physics = scene_end_watch;
  retail_script_004e59a8(fighter);
}

static void script_second_open(Fighter *fighter) {
  battle_sound(SOUND_A, 0x62);
  retail_script_004e5940(fighter);
}

static void script_second_victim_end(Fighter *victim) {
  battle_particles(victim, 2, 12, true);
  retail_script_004e5a20(victim);
}

static void first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_KITANA_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 388, 129, 0);
  mk4_script_scene_pose(opponent(fighter), 389, 1, 0);
  fighter_start_fiber(fighter, retail_script_004e01c0, NULL);
}

static void second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_KITANA_2)) {
    return;
  }
  mk4_script_scene_pose(fighter, 558, 16, 0);
  mk4_script_scene_pose(opponent(fighter), 13, 56, 0);
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x18000);
  }
  fighter_start_fiber(fighter, retail_script_004e5930, NULL);
}

static const MoveDef s_air_owner = {.clip = 77, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_air_victim = {.clip = 161, .last = 39, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

static void kitana_air_throw(Fighter *fighter) {
  if (!mk4_can_air_throw(fighter)) {
    return;
  }
  fighter_upright(fighter);
  fighter_upright(opponent(fighter));
  mk4_script_throw_start(fighter, &s_air_owner, &s_air_victim, retail_script_004e5ed0);
}

static void script_air_getup(Fighter *victim) {
  mk4_script_finish_clip(victim, 1);
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    fighter_fiber_finish(NULL);
  }
  mk4_script_bind(victim, 197, 29, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
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

static bool can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_first_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= 1.f && distance <= 3.f;
}

static bool can_leg_grab(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH);
  unsigned mask =
      chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->leg_grab_ready && (fighter->held & mask) == chord;
}

static bool can_cartwheel(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->cartwheel_ready;
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK}, 2, 24, 0x0, NULL, kitana_bicycle},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_FORWARD}, 3, 40, 0x0, can_spike_fatality, kitana_spike_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 16, 0x0, NULL, kitana_ring},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0x0, NULL, kitana_command_shadow_low},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, CMD_AIR, kitana_shadow_up_enabled, kitana_command_shadow_up},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0x40, can_leg_grab, kitana_leg_grab},
  {BUTTON_BLOCK, {0}, 0, 0, 0x10, mk4_can_air_throw, kitana_air_throw},
  {BUTTON_BLOCK, {0}, 0, 0, 0x40, can_leg_grab, kitana_leg_grab},
  {BUTTON_HIGH_KICK, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN}, 3, 40, 0x0, NULL, kitana_square_wave},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_BACK, TOKEN_BACK}, 3, 40, 0x0, can_fan_fatality, kitana_fan_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_UP, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x0, can_second_fatality, second_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_DOWN, TOKEN_FORWARD}, 3, 29, 0x0, can_cartwheel, kitana_cartwheel},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0x0, can_weapon, kitana_weapon},
  {BUTTON_RUN, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x40, can_first_fatality, first_fatality},
  {BUTTON_RUN, {0}, 0, 0, 0x0, mk4_can_sidestep, kitana_sidestep},
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
            [CPU_JAB] = kitana_jab,
            [CPU_LOW_KICK] = kitana_lk,
            [CPU_HIGH_KICK] = kitana_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = kitana_crouch_lp,
            [CPU_AIR_HP] = air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_DRAW] = kitana_weapon,
            [CPU_FINISH_1] = first_fatality,
            [CPU_FINISH_2] = second_fatality,
            [CPU_FINISH_FAN] = kitana_fan_fatality,
            [CPU_FINISH_SPIKE] = kitana_spike_fatality,
            [CPU_PROJECTILE] = kitana_ring,
            [CPU_SPECIAL_NEAR] = kitana_leg_grab,
            [CPU_SPECIAL_1] = kitana_bicycle,
            [CPU_SPECIAL_2] = kitana_cartwheel,
        },
    .finish_min = {0x10000, 0x1cccc},
    .finish_distance = {0x30000, 0x30000},
};

const CharacterDef mk4_kitana = {GAME_MK4, 16, "KITANA", &mk4_motion, moves, S_COUNT, &controls, enter, -73629, mk4_walk, mk4_jump, &cpu_moves};

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

static void square_victim(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  mk4_script_bind(victim, 136, 24, 0);
  victim->flags |= FF_PAIRED;
  int visit = 0;
  while (owner->action == 0x201 && owner->pending == NULL) {
    fighter_wait_ticks(1);
    victim->sample_override = 4 + (++visit % 20);
  }
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->body.gravity = 0x28f;
  victim->body.vel[AXIS_Y] = 0x666;
  mk4_script_wait_land(victim);
  script_throw_getup(victim);
}

void kitana_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 24:
      script = retail_script_004e9110;
      break;
    case 55:
      script = retail_script_004e8b38;
      break;
    case 56:
      script = retail_script_004e8f50;
      break;
    case 68:
      script = retail_script_004e9de8;
      break;
    case 86:
      script = retail_script_004ea130;
      break;
    case 87:
      script = square_victim;
      break;
    case 121:
      script = retail_script_004ea0b8;
      battle_wall_recoil(victim);
      break;
    default:
      require(false, "Unexpected Kitana record reaction");
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
