#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* VM control flow is generated; native adapters reuse C fibers. Playback need not
 * match stored clip length. See docs/liu-kang-scripts.md for the evidence. */
static void liu_kang_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/liu_kang.inc"
#include "generated/frames_liu_kang.inc"

/* Retail attack records; source addresses identify the supplied EXE. */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x170a3, 0x8518, 0xdeb8, 0x8000, 19, 19, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb70, B selector 36 */
// clang-format on

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void liu_kang_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void liu_kang_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void liu_kang_uppercut_contact(Fighter *fighter) {
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
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, liu_kang_ready);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, liu_kang_ready);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, liu_kang_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, liu_kang_ready);

/* Attacks query retail records from their events (SF_RECORD). Generic
 * S_HIT/S_KNOCKDOWN entries remain shared recovery fallbacks. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {266, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_loop_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_late_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_miss_end_sequence, .action = ACTION_RETAIL_0109};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_hit_end_sequence, .action = ACTION_RETAIL_0109};

static const MoveDef s_throw = {.clip = 79, .last = 60, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {119, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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
    liu_kang_uppercut_contact(fighter);
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
  fighter_fiber_finish(liu_kang_ready);
}

static void liu_kang_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  fighter_start_fiber(fighter, retail_script_004e62a0, NULL);
}

static void liu_kang_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e67d8, NULL);
}

static void liu_kang_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void liu_kang_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter_start_fiber(fighter, retail_script_004e6c90, NULL);
}

static void armed_idle(Fighter *fighter);
static void armed_walk(Fighter *fighter);
static void armed_crouch(Fighter *fighter);
static void armed_block(Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = liu_kang_jab,
    [S_KICK] = liu_kang_kick,
    [S_SWEEP] = liu_kang_sweep,
    [S_UPPERCUT] = liu_kang_uppercut,
};

static Fighter *opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

/* Selected Liu Kang ground scripts and native tails: 4e6d08, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6e90/46f390. Times are our script ticks. */
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

typedef struct GroundAttack {
  MoveDef move;
  AttackRecord record;
  int active, end_miss, end_contact;
} GroundAttack;

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {.move = {.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}}, /* generated 4e6d08 */
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {.move = {.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}}, /* generated 4e60b8 */
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

static void open_followup(Fighter *fighter, const ButtonTable *buttons) {
  fighter->buttons = buttons;
  fighter->flags = (fighter->flags | FF_FOLLOW_UP) & ~(unsigned)FF_BUSY;
  fighter->action = ACTION_RETAIL_1006;
}

static void ground_native_tail(Fighter *fighter) {
  const int index = fighter->script_phase;
  const bool followup = fighter->script_timer != 0;
  for (;;) {
    const GroundAttack *attack = &ground_attacks[index];
    int t = fighter->ticks, fast = index == G_ROUNDHOUSE && followup ? 4 : 0;
    int active = attack->active - fast;
    bool gate = (index == G_CROUCH_LP || index == G_CROUCH_LK) && victim_action(fighter) == ACTION_RETAIL_2011;
    if (index == G_CLOSE_HP) {
      int action = victim_action(fighter);
      gate = action == ACTION_RETAIL_2001 || action == ACTION_RETAIL_0106 || action == ACTION_RETAIL_0107;
    }
    if (!gate && !fighter->connected && t >= active && t <= active + (index == G_ROUNDHOUSE ? 2 : 0)) {
      if (battle_contact(fighter, &attack->record) != CONTACT_MISS) {
        if (index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CLOSE_HK) {
          open_followup(fighter, index == G_CLOSE_HP ? &close_hp_followup : index == G_COMBO_HP ? &combo_hp_followup : &close_hk_followup);
        }
      }
    }
    if (t >= active) {
      fighter->action = ACTION_RETAIL_1006;
    }
    int end = (fighter->connected ? attack->end_contact : attack->end_miss) - fast;
    switch (index) {
      case G_CROUCH_LP:
      case G_CROUCH_LK: {
        int reverse = active + (fighter->connected ? 4 : 1);
        fighter->sample_override = t < active ? mini(9, t + (index == G_CROUCH_LP ? 2 : 1)) : maxi(0, 9 - maxi(0, t - reverse));
        break;
      }
      case G_CLOSE_HP: {
        int reverse = fighter->connected ? 18 : 12;
        fighter->sample_override = t < 10 ? t : t < reverse ? 9 : maxi(0, 8 - t + reverse);
        if (t >= reverse) {
          fighter->buttons = NULL;
          fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
        }
        break;
      }
      case G_COMBO_HP:
        fighter->sample_override = mini(27, t + 2 - (fighter->connected && t > active ? mini(3, t - active) : 0));
        break;
      case G_CLOSE_HK: {
        int reverse = fighter->connected ? 10 : 12;
        fighter->sample_override = t < 8 ? t + 5 : t < reverse ? 12 : maxi(0, 12 - t + reverse);
        if (t >= reverse) {
          fighter->buttons = NULL;
          fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
        }
        break;
      }
      case G_ROUNDHOUSE:
        fighter->sample_override = t <= 6 ? t * 2 : t < active ? mini(19, 12 + (t - 6) * (fast ? 2 : 1)) : mini(44, 20 + t - active);
        if (fighter->contact == CONTACT_BLOCK) {
          end = attack->end_miss - fast;
        }
        break;
    }
    if (t >= end) {
      fighter_fiber_finish(liu_kang_ready);
    }
    fighter_wait_ticks(1);
  }
}

static void ground_attack(Fighter *fighter, int index) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->script_timer = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_COMBO_HP || index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {retail_script_004e6d08, ground_native_tail, retail_script_004e6158, retail_script_004e60b8, retail_script_004e61e0,
      retail_script_004e69b0, retail_script_004e6190, retail_script_004e6e90};
  FighterFn script = scripts[index];
  fighter_start_fiber(fighter, script, NULL);
}

static void hp_script(Fighter *fighter) {
  for (int phase = 0;;) {
    if (phase >= 4) {
      for (int t = 1; t <= (phase == 4 ? 10 : 8); ++t) {
        fighter_wait_ticks(1);
        fighter->sample_override = mini(phase == 4 ? 20 : 18, 11 + t);
      }
      fighter_fiber_finish(liu_kang_ready);
    }
    int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
    int active = phase == 0 ? 8 : phase == 1 ? 7 : 6;
    for (int t = 1; t <= active + 3; ++t) {
      fighter_wait_ticks(1);
      fighter->sample_override = phase == 2 && t == active + 2 ? 17 : first + t;
      const Fighter *victim = fighter_opponent(fighter);
      bool ducking_kick = victim->action == ACTION_RETAIL_0102 && victim->avoids_high;
      if (t >= active && t <= active + 2 && !fighter->connected && !ducking_kick) {
        battle_contact(fighter, &hp_record);
      }
    }
    const Fighter *victim = fighter_opponent(fighter);
    bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
    phase = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
    fighter_fiber_move(fighter, S_PUNCH, phase == 4 ? &s_hp : &s_hp_second);
    fighter->script_phase = phase;
    fighter->sample_override = phase == 1 ? 1 : phase == 3 ? 2 : 11;
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

static void liu_kang_hp(Fighter *fighter) {
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

static void liu_kang_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void liu_kang_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void liu_kang_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void liu_kang_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void liu_kang_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void liu_kang_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void liu_kang_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    liu_kang_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    liu_kang_kick(fighter);
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
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : liu_kang_hp);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : liu_kang_hk);
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
                                                              : liu_kang_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : liu_kang_lk);
}

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */
typedef struct AirAttack {
  MoveDef move;
  AttackRecord record;
  int extend, hold, retract;
} AirAttack;

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 6, 3, 20},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
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

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  const AirAttack *attack = &air_attacks[index];
  const bool combo = fighter->script_phase == AIR_COMBO;
  const int retract_at = attack->extend + attack->hold;
  for (int tick = elapsed ? elapsed : 1;; ++tick) {
    if (tick != elapsed) {
      fighter_wait_ticks(1);
    }
    if (!fighter_airborne(fighter)) {
      fighter_fiber_finish(air_land);
    }
    fighter->sample_override = index == 2              ? mini(29, tick + 4)
                               : tick < attack->extend ? tick
                               : tick < retract_at     ? attack->extend - 1
                                                       : maxi(0, attack->retract - 1 - tick + retract_at);
    if (tick >= attack->extend && tick < retract_at && !fighter->connected && battle_contact(fighter, &attack->record) != CONTACT_MISS) {
      bool low = fighter->body.ground - fighter->body.pos[AXIS_Y] <= 0xcccc;
      mk4_script_stop_body(fighter);
      fighter->action = ACTION_RETAIL_1006;
      fighter->script_phase = combo ? AIR_COMBO_FREEZE : AIR_FREEZE;
      if (index == 0 || index == 2) {
        if (low) {
          fighter->body.pos[AXIS_Y] = fighter->body.ground;
          if (!combo) {
            fighter->buttons = &air_ground_followup;
            fighter->flags |= FF_FOLLOW_UP;
          }
        } else if (index == 0 && !combo) {
          fighter->buttons = &air_followup;
        }
      }
      fighter_wait_ticks(index >= 3 ? 8 : 12);
      fighter->buttons = NULL;
      fighter->body.gravity = index >= 3 || combo ? 0x51e : 0x28f;
      break;
    }
    if (tick >= retract_at + attack->retract) {
      if (combo) {
        fighter->body.gravity = 0x51e;
      }
      break;
    }
  }
  fighter->script_phase = AIR_FALL;
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_fiber_finish(air_land);
}

static void start_air_attack(Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  fighter_upright(fighter);
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {retail_script_004e6658, retail_script_004e6710, retail_script_004e6df0, retail_script_004e6500, retail_script_004e63d8};
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
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : liu_kang_crouch_lp);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, liu_kang_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, liu_kang_crouch_lk);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, liu_kang_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    liu_kang_hp(fighter);
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
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : liu_kang_kick);
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

/* Slot 3: LP 4de678, LK 42c040/4de3b0/410/428/468/478.
 * The victim owns its released recovery, even after owner interruption/KO. */
static const MoveDef s_lk_throw = {.clip = 35, .last = 43, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 151, .last = 45, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 133, .last = 35, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_release = {.clip = 174, .last = 26, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_getup = {.clip = 191, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

static void thrown_play_to(Fighter *victim, int sample, int step) {
  while (step > 0 ? victim->sample_override < sample : victim->sample_override > sample) {
    uint64_t submitted = g_battle.ticks;
    do {
      fighter_wait_ticks(1);
    } while (g_battle.ticks == submitted);
    victim->sample_override += step;
  }
}

static void thrown_finish(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_fiber_finish(victim->health > 0 ? fighter_ready : NULL);
}

static void liu_script_lp_getup(Fighter *victim) {
  if (victim->health <= 0) {
    thrown_finish(victim);
  }
  victim->facing = -victim->facing;
  fighter_fiber_move(victim, S_KNOCKDOWN, &s_lp_getup);
  victim->sample_override = 0;
  fighter_set_speed(victim, -0x147a);
  thrown_play_to(victim, 15, 1);
  fighter_wait_ticks(1);
  thrown_finish(victim);
}

static void lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  victim->sample_override = 30;
  fighter_wait_ticks(2);
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->reaction = 80;
  retail_script_004e9a90(victim);
}

static void ground_thrown_script(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  for (;;) {
    fighter_wait_ticks(1);
    if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
      victim->flags &= ~(unsigned)FF_PAIRED;
      thrown_finish(victim);
    }
  }
}

static void throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = opponent(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 151 || fighter_move(victim)->clip == 133)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->script_phase == 1) {
      return;
    }
    mk4_script_stop_body(victim);
    fighter_cancel_fiber(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

static void liu_script_lk_continue(Fighter *fighter) {
  retail_script_004de410(fighter);
}

static void liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  fighter_start_fiber(victim, lk_thrown_script, NULL);
  battle_damage(victim, 0x2e14);
  retail_script_004de428(fighter);
  retail_script_004de468(fighter);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    liu_kang_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.pos[axis] = fighter->body.pos[axis];
  }
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  fighter_start_fiber(fighter, lk ? retail_script_004de3b0 : retail_script_004de798, throw_cleanup);
  fighter_start_fiber(victim, ground_thrown_script, NULL);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

/* LK release 4e49a8 -> 45eab0/47eaf0, scripts 4e8610/678/688.
 * Native 47eb50 holds a hit pair for 58 ticks; damage is D[6] once. */
static const MoveDef s_bicycle = {.clip = 68, .last = 22, .flags = RETAIL, .action = ACTION_RETAIL_0206};
static const MoveDef s_bicycle_victim = {.clip = 136, .last = 23, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_bicycle_release = {.clip = 182, .last = 64, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const MoveDef s_bicycle_air_release = {.clip = 183, .last = 33, .flags = SF_LOCK, .action = ACTION_RETAIL_100B};
static const MoveDef s_bicycle_getup = {.clip = 185, .last = 15, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const AttackRecord bicycle_record = {0xf851, -0x547a, 0xf5c2, 0x70a3, 84, 84, 0, 0, 0x2666};

static void bicycle_victim_script(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  for (;;) {
    fighter_wait_ticks(1);
    if (victim->script_phase != 0 || owner->action != ACTION_RETAIL_0206 || owner->pending != NULL || !(owner->flags & FF_PAIRED)) {
      break;
    }
    victim->sample_override = victim->ticks < 24 ? victim->ticks : 4 + (victim->ticks - 24) % 20;
  }
  victim->flags &= ~(unsigned)FF_PAIRED;
  bool air = victim->body.ground - victim->body.pos[AXIS_Y] > 0x3333;
  fighter_fiber_move(victim, S_KNOCKDOWN, air ? &s_bicycle_air_release : &s_bicycle_release);
  victim->reaction = 81;
  victim->sample_override = 0;
  if (air) {
    retail_script_004e9ae8(victim);
  } else {
    victim->body.physics = fighter_friction;
    retail_script_004e9ad0(victim);
  }
}

static void bicycle_cleanup(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  if (victim->current_move == &s_bicycle_victim && (victim->flags & FF_PAIRED)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
  }
}

static void bicycle_script(Fighter *fighter) {
  for (int tick = 0; tick < 60; ++tick) {
    fighter_wait_ticks(1);
    fighter->sample_override = 7 + tick % 16;
    if (!fighter_airborne(fighter)) {
      break;
    }
    ContactResult result = battle_contact(fighter, &bicycle_record);
    if (result == CONTACT_MISS) {
      continue;
    }
    if (result == CONTACT_BLOCK) {
      retail_script_004e8688(fighter);
    }
    Fighter *victim = opponent(fighter);
    throw_disarm(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_bicycle_victim);
    victim->sample_override = 0;
    victim->reaction = 84;
    victim->flags |= FF_PAIRED | FF_REACTING;
    fighter->flags |= FF_PAIRED;
    fighter->body.pos[AXIS_Y] = mini(fighter->body.pos[AXIS_Y], -0x15999);
    fighter_set_speed(fighter, 0x10a3);
    fighter->body.vel[AXIS_Y] = fighter->body.gravity = 0;
    fighter_start_fiber(victim, bicycle_victim_script, NULL);
    for (int paired = 0; paired < 58; ++paired) {
      fighter_wait_ticks(1);
      if (victim->current_move != &s_bicycle_victim || victim->pending != NULL || !fighter_fiber_active(victim)) {
        break;
      }
      fighter->sample_override = 7 + paired % 16;
      for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
        victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xe666);
      }
    }
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter->body.pos[AXIS_Y] = fighter->body.ground; /* 4906c0 */
    mk4_script_stop_body(fighter);
    break;
  }
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0xc4;
    while (fighter_airborne(fighter)) {
      fighter_wait_ticks(1);
    }
  }
  retail_script_004e8678(fighter);
}

static bool can_bicycle(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->action != ACTION_RETAIL_100B && fighter->latch[BUTTON_LOW_KICK] != 0 &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_KICK] >= 120;
}

static void liu_kang_bicycle_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_opponent(fighter)->action == ACTION_RETAIL_100B) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_bicycle);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, retail_script_004e8610, bicycle_cleanup);
}

/* Fireballs: 49bf50/90/49bfe0 -> 4ed678/6c0/6f8.
 * Projectile task 64 has a 48-tick lifetime independent of its owner. */
static const MoveDef s_fireball_high = {.clip = 18, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030C};
static const MoveDef s_fireball_low = {.clip = 19, .last = 46, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030D};
static const MoveDef s_fireball_air = {.clip = 20, .last = 5, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030E};
static const AttackRecord fireball_high_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 64, 10, 2, ATTACK_MARK, 0x1999};
static const AttackRecord fireball_low_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 65, 10, 2, ATTACK_MARK, 0x1999};
static const MeshAnimation fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

static void fireball_projectile(Projectile *p) {
  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  bool low = p->phase != 0;
  if ((low || opponent(owner)->action != ACTION_RETAIL_2001) &&
      battle_projectile_contact(p, low ? &fireball_low_record : &fireball_high_record) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void fireball_spawn(Fighter *fighter, int type) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, fireball_projectile);
  if (p == NULL) {
    return;
  }
  p->phase = type == 1;
  p->mesh_animation = &fireball_mesh;
  p->initial_mesh = 50; /* 4ed730/740/760 -> rig 50678c +24. */
  q16 forward = type == 2 ? 0x9999 : type == 1 ? 0x28f5 : 0x1999;
  p->pos[AXIS_Y] += type == 2 ? -0x2147 : type == 1 ? 0x35c2 : -0x7333;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    p->pos[axis] += q16_mul(fighter_axis(fighter, axis), forward);
    p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void liu_script_spawn_low(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang low-fireball task type");
  fireball_spawn(fighter, 1);
}

static void liu_script_spawn_high(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang high-fireball task type");
  fireball_spawn(fighter, 0);
}

static void liu_script_spawn_air(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang air-fireball task type");
  fireball_spawn(fighter, 2);
}

static void liu_script_air_return(Fighter *fighter) {
  fighter->body.gravity = 0x20c;
  while (fighter_airborne(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_fiber_finish(mk4_land);
}

static void fireball(Fighter *fighter, bool low) {
  bool air = !low && fighter_airborne(fighter);
  if (g_battle.phase != PHASE_FIGHT || (low && fighter_airborne(fighter))) {
    return;
  }
  if (!air) {
    mk4_script_stop_body(fighter);
  }
  fighter_start_move(fighter, S_PUNCH, air ? &s_fireball_air : low ? &s_fireball_low : &s_fireball_high);
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter_upright(fighter);
  fighter->script_phase = air ? 2 : low ? 1 : 0;
  fighter->sample_override = 0;
  FighterFn script = air ? retail_script_004ed6f8 : low ? retail_script_004ed678 : retail_script_004ed6c0;
  fighter_start_fiber(fighter, script, NULL);
}

static void liu_kang_high_fireball(Fighter *fighter) {
  fireball(fighter, false);
}

static void liu_kang_low_fireball(Fighter *fighter) {
  fireball(fighter, true);
}

/* 47edc0/4e86a8 -> 47ee00/30; D[12], six opening visits.
 * Hit and blocked exits are 4e86e8 and 4e8718 respectively. */
static const MoveDef s_flying_kick = {.clip = 92, .last = 5, .flags = RETAIL, .action = ACTION_RETAIL_0207};
static const AttackRecord flying_record = {0x14f5c, -0xf5c, 0xf5c2, 0x51eb, 31, 31, 0, 0, 0x23d7};

static void flying_kick_script(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1); /* 47ee00 native startup before 47ee30 contact runner. */
  for (;;) {
    if (!fighter_airborne(fighter)) {
      retail_script_004e86d0(fighter);
      return;
    }
    ContactResult result = battle_contact(fighter, &flying_record);
    if (result == CONTACT_BLOCK) {
      retail_script_004e8718(fighter);
      return;
    }
    if (result != CONTACT_MISS) {
      retail_script_004e86e8(fighter);
      return;
    }
    fighter_wait_ticks(1);
  }
}

static void liu_kang_flying_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_flying_kick);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x23d7);
  fighter->body.vel[AXIS_Y] = -0x170a;
  fighter->body.gravity = 0x106;
  battle_sound(SOUND_INDEXED, 0);
  fighter_start_fiber(fighter, retail_script_004e86a8, NULL);
}

/* Default weapon descriptor 53d5c0[3] -> 4ece58: type 66,
 * rig 506cb0. The four weapon entries select distinct native programs. */
static const WeaponDef dragon_sword;
static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike sword_attacks[] = {
  {{.clip = 240, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {106168, -54394, 77987, 80609, 59, 59, 119, 0, 10485}, 0, 8, 13, 3, 4, 0, 0, 0, 34},
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {99614, -49152, 64880, 55050, 61, 61, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34},
  {{.clip = 242, .last = 30, .flags = RETAIL, .action = ACTION_RETAIL_0212}, {111411, -25552, 89784, 11796, 62, 62, 118, 0, 9830}, 8, 9, 11, 3, 8, 2, 18, 0x28f5, 34},
  {{.clip = 227, .last = 59, .flags = RETAIL, .action = ACTION_RETAIL_0112}, {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175}, 0, 16, 29, 2, 0, 8, 13, 0x1999, 31},
};
static const MoveDef s_sword_hp = {.clip = 233, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_021C};
// clang-format on

static void sword_motion(Fighter *fighter) {
  if (fighter->script_timer > 0) {
    fighter->script_timer--;
  } else if (fighter->move_loops > 0) {
    fighter_set_speed(fighter, fighter->saved_motion[0]);
    fighter->move_loops--;
  } else {
    fighter_set_speed(fighter, 0);
    fighter->body.physics = NULL;
  }
}

static void sword_travel(Fighter *fighter, int delay, int duration) {
  fighter_set_speed(fighter, 0);
  fighter->saved_motion[0] = fighter->weapon_strike == NULL ? 0x11eb : fighter->weapon_strike->speed;
  fighter->script_timer = delay;
  fighter->move_loops = duration;
  fighter->body.physics = sword_motion;
}

static void sword_cleanup(Fighter *fighter) {
  if (fighter->body.physics == sword_motion) {
    fighter->body.physics = NULL;
    fighter_set_speed(fighter, 0);
  }
}

static bool sword_repeat(Fighter *fighter) {
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) != 0;
  fighter->buffered &= ~BUTTON_BIT(BUTTON_HIGH_PUNCH);
  return repeat;
}

static void sword_blend(Fighter *fighter, int ticks) {
  fighter->blend_clip = fighter_move(fighter)->clip;
  fighter->blend_sample = fighter->sample_override;
  fighter->blend_ticks = 0;
  fighter->blend_duration = ticks;
  for (int i = 1; i <= ticks; ++i) {
    fighter_wait_ticks(1);
    fighter->blend_ticks = i;
  }
}

static void sword_strike_script(Fighter *fighter) {
  int index = fighter->script_phase;
  for (;;) {
    const WeaponStrike *strike = &sword_attacks[index];
    int opening = strike->windup - strike->first;
    for (int i = 1; i <= opening; ++i) {
      fighter_wait_ticks(1);
      fighter->sample_override = strike->first + i;
    }
    battle_sound(SOUND_C, strike->sound);
    fighter_fiber_play_to(fighter, strike->target, 1);
    for (int i = 0; i < strike->queries; ++i) {
      fighter_wait_ticks(1);
      fighter->sample_override++;
      if (battle_contact(fighter, &strike->record) != CONTACT_MISS) {
        break;
      }
    }
    bool hit = fighter->connected;
    fighter->action = ACTION_RETAIL_1006;
    if (index == 0 && hit && sword_repeat(fighter)) {
      index = 1; /* 496500 -> 4ec720/730, delayed travel then second strike. */
      fighter_fiber_move(fighter, S_PUNCH, &sword_attacks[index].move);
      fighter->weapon_strike = &sword_attacks[index];
      fighter->sample_override = sword_attacks[index].first;
      sword_travel(fighter, sword_attacks[index].delay, sword_attacks[index].travel);
      continue;
    }
    fighter_set_speed(fighter, 0);
    fighter->body.physics = NULL;
    fighter_fiber_play_to(fighter, strike->move.last, 1);
    if (index == 2 && hit) {
      sword_blend(fighter, 10); /* 494e10: fixed armed stance, step 0x1999. */
    }
    fighter_wait_ticks(1);
    fighter_fiber_finish(fighter_ready);
  }
}

static void equip_dragon_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &dragon_sword);
}

static void sword_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, index == 2, (index == 2) ? retail_script_004e66d0 : retail_script_004e66f0);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &sword_attacks[index].move);
  fighter->script_phase = index;
  fighter->weapon_strike = &sword_attacks[index];
  fighter->sample_override = sword_attacks[index].first;
  if (sword_attacks[index].travel != 0) {
    sword_travel(fighter, sword_attacks[index].delay, sword_attacks[index].travel);
  }
  fighter_start_fiber(fighter, index == 3 ? retail_script_004ec7f0 : sword_strike_script, sword_cleanup);
}

static void sword_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, true, retail_script_004e66d0);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_sword_hp);
  fighter->weapon_strike = NULL;
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, retail_script_004ec680, sword_cleanup);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, 0);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, 2);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, 3);
}

static const WeaponDef dragon_sword = {
    0x66, sword_hp, sword_lp, sword_back_hp, sword_back_lp, NULL, weapon_throw, equip_dragon_sword, SCENE_MODEL_DRAGON_SWORD};

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void liu_kang_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_dragon_sword(fighter);
    weapon_followup_victim(fighter);
    sword_lp(fighter); /* 4ecb40[type 66 - 60] -> 4964d0. */
  } else {
    weapon_draw(fighter, &dragon_sword);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void liu_kang_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

/* 45e820: RUN latch is written after matching, so this sees the prior
 * press. 4692e0 uses shared descriptor 53ce18 (clip 300), not a roster
 * table. Down plays it forwards; otherwise it plays backwards. */

static bool can_sidestep(const Fighter *fighter) {
  return mk4_can_sidestep(fighter);
}

static void liu_kang_sidestep(Fighter *fighter) {
  mk4_sidestep(fighter);
}

/* First scene: 4e0498[3] -> 450930. Second: 4dfe90[3] ->
 * 43c3d0/4dfff8/4e0020, victim reactions 176/177. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(119, 59), FATAL_MOVE(418, 518), FATAL_MOVE(419, 288),
  FATAL_MOVE(537, 59), FATAL_MOVE(538, 16), FATAL_MOVE(539, 37),
  FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48), FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
};
// clang-format on
#undef FATAL_MOVE

// clang-format off: Keep table entries on one row, including long rows.
#define SCENE(c, a, b) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, a, b, 1, 0, 0}
static const SceneClip dragon_actor[] = {SCENE(418, 0, 159), SCENE(418, 160, 518), SCENE(418, 446, 518)};
static const SceneClip dragon_target[] = {
  {{.clip = 389, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  {{.clip = 419, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, 0, 0, 1, 0x32f1, 0x3333},
  SCENE(419, 0, 288),
};
#undef SCENE
// clang-format on

static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        if (owner && fighter_fiber_running(fighter)) {
          fighter_fiber_move(fighter, S_VICTORY, &fatal_moves[i]);
        } else {
          fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        }
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static bool can_first_fatality(const Fighter *fighter) {
  /* 45e410 / 4e4158: word A requires BLK+HK, word B requires LK;
   * reject Up/HP/LP/RUN. Directions other than Up are outside the mask. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_RUN);
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f && (fighter->held & mask) == chord;
}

static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_close_fatality(fighter);
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void dragon_fatality_script(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  bool actor_started = false;
  for (;;) {
    fighter_wait_ticks(1);
    int tick = g_battle.phase_ticks;
    if (tick == 101) { /* 450930 setup 16, then 85, then spawn. */
      fighter->appearance = APPEARANCE_DRAGON;
      fighter->body.pos[AXIS_Y] = -0x12c49; /* Object group Y from 450a07. */
      fatal_pose(fighter, 418, 0, true);
      battle_sound(SOUND_B, 36);
      /* Portable wide framing keeps the complete native creature visible.
       * Retail 452000 uses several independently scheduled zoom/focus tasks. */
      float x = q16_float(fighter->heading[0]), z = q16_float(fighter->heading[1]);
      Vec3 center = {fighter_x(fighter) + x, -2.7f, fighter_z(fighter) + z};
      g_battle.camera = (SceneCamera){.active = true, .focal_scale = 0x9999, .eye = {center.x - z * 7.5f, -2.9f, center.z + x * 7.5f}, .target = center};
    }
    if (tick == 121) {
      battle_sound(SOUND_B, 46);
      scene_begin(fighter, dragon_actor, COUNT(dragon_actor), 2);
      actor_started = true;
    } else if (actor_started) {
      scene_step(fighter);
    }
    if (tick == 269) {
      scene_begin(victim, dragon_target, COUNT(dragon_target), -1);
    } else if (victim->scene.clips != NULL) {
      scene_step(victim);
    }
    /* 451300: wait 4, then 23 four-visit and 26 six-visit blood
     * phases, wait 8, remove victim joints 5/6. Bounded scene pools
     * replace native per-joint task allocation. */
    int tear = tick - 285;
    if ((tear >= 0 && tear < 92 && tear % 4 == 0) || (tear >= 92 && tear < 248 && (tear - 92) % 6 == 0)) {
      battle_particles(victim, 3, 3, false);
    }
    if (tick == 281) {
      battle_sound(SOUND_B, 37);
    }
    if (tick == 418) {
      battle_sound(SOUND_B, 65); /* 450ce0's 137-tick delayed cue. */
    }
    if (tick == 541) {
      battle_detach_piece(victim, 5, (q16[3]){0}, (q16[3]){0});
      battle_detach_piece(victim, 6, (q16[3]){0}, (q16[3]){0});
    }
    if (victim->scene.done && fighter->scene.index >= 2) {
      fighter_fiber_finish(finish_scene);
    }
  }
}

static void liu_script_fatal_watch(Fighter *fighter) {
  if (--fighter->script_timer <= 0) {
    fighter_cancel_all_fibers();
    battle_end_finisher();
  }
}

static void liu_script_fatal_hit(Fighter *owner) {
  Fighter *victim = opponent(owner);
  victim->script_phase = 2;
  victim->reaction = 177;
  battle_shatter(victim);
  battle_particles(victim, 3, 20, false);
  owner->script_timer = 160;
  owner->body.physics = liu_script_fatal_watch;
}

static void fatal_fireball_object(Projectile *p) {
  if (g_battle.phase != PHASE_FATALITY || g_battle.finisher != FINISH_LIU_KANG_2) {
    p->kind = 0;
    return;
  }
  Fighter *victim = opponent(&g_battle.fighters[p->owner]);
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  q16 dx = victim->body.pos[AXIS_X] - p->pos[AXIS_X];
  q16 dz = victim->body.pos[AXIS_Z] - p->pos[AXIS_Z];
  if ((int64_t)dx * dx + (int64_t)dz * dz <= (int64_t)0x28000 * 0x28000 || p->ticks >= 47) {
    p->kind = 0;
    liu_script_fatal_hit(&g_battle.fighters[p->owner]);
  }
}

static void liu_script_fatal_setup(Fighter *fighter) {
  fatal_pose(fighter, 537, 0, true);
  fatal_pose(opponent(fighter), 539, 0, false);
  fighter->flags |= FF_PAIRED;
  opponent(fighter)->flags |= FF_PAIRED;
  retail_script_004e0020(fighter);
}

static void liu_script_fatal_spawn(Fighter *fighter, int task_type) {
  require(task_type == 38, "Invalid fatality projectile task");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FATAL_FIREBALL, 48, fatal_fireball_object);
  if (p != NULL) {
    p->mesh_animation = &fireball_mesh;
    p->initial_mesh = 50;
    p->pos[AXIS_Y] -= 0x7333;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      p->pos[axis] += q16_mul(fighter_axis(fighter, axis), 0x1999);
      p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
    }
  } else {
    liu_script_fatal_hit(fighter);
  }
}

static void liu_script_fatal_align(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
}

static void liu_script_fan_cycle(Fighter *fighter) {
  (void)fighter; /* 43d530 returns to the bounded native scene coordinator. */
}

static void liu_script_fan_victim_end(Fighter *victim) {
  victim->appearance = APPEARANCE_HIDDEN;
  fighter_fiber_finish(NULL);
}

static void liu_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void liu_script_fan_watch(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  int rise = mini(30, ++fighter->script_timer);
  victim->body.pos[AXIS_Y] = victim->body.ground - rise * 0x3333;
  /* Native arena task owns completion and retires the generated pose loop. */
  if (fighter->script_timer >= 102) {
    fighter_cancel_all_fibers();
    battle_end_finisher();
  }
}

static void liu_script_fan_tail(Fighter *fighter) {
  for (int cycle = 0; cycle < 31; ++cycle) {
    retail_script_004e0138(fighter);
  }
  Fighter *victim = opponent(fighter);
  fighter_cancel_fiber(victim);
  victim->sample_override = 69;
  fighter_start_fiber(victim, retail_script_004e01a0, NULL);
  fighter->script_timer = 0;
  fighter->body.physics = liu_script_fan_watch;
  retail_script_004e0148(fighter);
}

static void liu_script_spike_tail(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  while (g_battle.phase_ticks < 324) {
    fighter_wait_ticks(1);
    int tick = g_battle.phase_ticks;
    fighter->sample_override = mini(19, fighter->sample_override + 1);
    if (tick >= 64) {
      int rise = mini(22, tick - 64);
      fatal_pose(victim, 128, mini(31, rise * 2), false);
      victim->body.pos[AXIS_Y] = rise == 22 ? -0x59999 : victim->body.ground - rise * 0x3333;
    }
  }
  fatal_pose(fighter, 119, 0, true);
  fighter_fiber_finish(finish_scene);
}

static void start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    return;
  }
  Fighter *victim = opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_LIU_KANG_2 ? 119 : id == FINISH_FAN ? 415 : id == FINISH_SPIKE ? 293 : 119, 0, true);
  if (id == FINISH_LIU_KANG_1) {
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x20ccc);
    }
  }
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
    fighter_start_fiber(victim, retail_script_004e0188, NULL);
  }
  fighter_start_fiber(fighter,
      id == FINISH_LIU_KANG_1   ? dragon_fatality_script
      : id == FINISH_LIU_KANG_2 ? retail_script_004dfff8
      : id == FINISH_FAN        ? retail_script_004e0118
                                : retail_script_004e6950,
      NULL);
}

static void liu_kang_first_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_LIU_KANG_1);
}

static void liu_kang_second_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_LIU_KANG_2);
}

static void liu_kang_fan_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_FAN);
}

static void liu_kang_spike_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SPIKE);
}

/* Typed native boundaries used by generated VM code. These contain native
 * contact/controller work; all VM playback and branching lives in the include. */

static void liu_script_scene_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && selector >= 42 && selector <= 44, "Invalid fatality orchestration event");
  /* battle_begin_finisher owns input lock, camera and scene activation. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

static void liu_script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = opponent(fighter);
  victim->reaction = reaction;
  if (reaction == 104) {
    throw_disarm(victim);
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_release);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    fighter_start_fiber(victim, retail_script_004e9b68, NULL);
  } else if (reaction == 80) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
  } else if (reaction == 176) {
    victim->facing = -victim->facing;
  }
}

static void liu_script_kick_tail(Fighter *fighter) {
  for (int i = 0; i < 3; ++i) {
    fighter_wait_ticks(1);
    liu_kang_kick_contact(fighter);
  }
  retail_script_004e5c70(fighter);
}

static void liu_script_sweep_tail(Fighter *fighter) {
  for (int i = 0; i < 2; ++i) {
    fighter_wait_ticks(1);
    liu_kang_sweep_contact(fighter);
  }
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(liu_kang_ready);
}

static void liu_script_crouch_lk_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void liu_script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void liu_script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void liu_script_combo_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void liu_script_roundhouse_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void liu_script_roundhouse_start(Fighter *fighter) {
  fighter->script_timer = fighter_fast_start(fighter);
  if (fighter->script_timer) {
    retail_script_004e6890(fighter);
  } else {
    retail_script_004e68a8(fighter);
  }
}

static void liu_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

static void liu_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

static void liu_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 6);
}

static void liu_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void liu_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 10);
}

static void liu_script_lk_thrown_tail(Fighter *victim) {
  fighter_set_speed(victim, -0x3333);
  victim->body.physics = fighter_friction;
  thrown_play_to(victim, 0, -1);
  fighter_wait_ticks(1);
  victim->facing = -victim->facing;
  thrown_finish(victim);
}

static void liu_script_bicycle_getup(Fighter *victim) {
  if (victim->health > 0) {
    fighter_fiber_move(victim, S_KNOCKDOWN, &s_bicycle_getup);
    victim->sample_override = 0;
    thrown_play_to(victim, 15, 1);
  }
  thrown_finish(victim);
}

static void liu_script_bicycle_motion(Fighter *fighter) {
  fighter->sample_override = 7 + fighter->ticks % 16;
}

static void liu_script_draw_equip(Fighter *fighter) {
  equip_dragon_sword(fighter);
  retail_script_004e62d8(fighter);
}

static unsigned liu_script_sword_gate(Fighter *fighter) {
  return sword_repeat(fighter) ? 0u : 4u;
}

static void liu_script_sword_blend(Fighter *fighter) {
  sword_blend(fighter, 5);
  fighter_fiber_finish(fighter_ready);
}

static void liu_script_motion(Fighter *fighter, int speed, int delay, int duration) {
  sword_travel(fighter, delay, duration);
  fighter->saved_motion[0] = speed;
}

static void liu_script_back_lp_tail(Fighter *fighter) {
  const WeaponStrike *strike = &sword_attacks[3];
  for (int i = 0; i < strike->queries; ++i) {
    fighter_wait_ticks(1);
    fighter->sample_override++;
    if (battle_contact(fighter, &strike->record) != CONTACT_MISS) {
      break;
    }
  }
  fighter->action = ACTION_RETAIL_1006;
  fighter_set_speed(fighter, 0);
  fighter->body.physics = NULL;
  fighter_fiber_play_to(fighter, strike->move.last, 1);
  fighter_wait_ticks(1);
  fighter_fiber_finish(fighter_ready);
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, CMD_AIR, NULL, liu_kang_high_fireball},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0, can_close_fatality, liu_kang_second_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, NULL, liu_kang_low_fireball},
  {BUTTON_LOW_PUNCH, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0, can_fan_fatality, liu_kang_fan_fatality},
  {BUTTON_BLOCK, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, liu_kang_first_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, NULL, liu_kang_flying_kick},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 3, 40, 0, can_spike_fatality, liu_kang_spike_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD}, 2, 24, 0, can_weapon, liu_kang_weapon},
  {BUTTON_LOW_KICK, {0}, 0, 0, CMD_RELEASE, can_bicycle, liu_kang_bicycle_kick},
  {BUTTON_RUN, {0}, 0, 0, 0, can_sidestep, liu_kang_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, liu_kang_pickup},
};
// clang-format on

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

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

/* 438e50/60/70/80 select 4df8e8/928/968/9a8[3]:
 * bicycle, low fireball, flying kick, low fireball. */
static void cpu_close_hp(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HP);
}

static void cpu_close_hk(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HK);
}

static void cpu_air_hp(Fighter *fighter) {
  if (fighter->armed) {
    weapon_air_attack(fighter, true, retail_script_004e66d0);
  } else {
    air_hp(fighter);
  }
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
            [CPU_JAB] = liu_kang_jab,
            [CPU_LOW_KICK] = liu_kang_lk,
            [CPU_HIGH_KICK] = liu_kang_hk,
            [CPU_SWEEP] = enter_sweep,
            [CPU_UPPERCUT] = enter_uppercut,
            [CPU_THROW] = enter_throw,
            [CPU_CLOSE_HP] = cpu_close_hp,
            [CPU_CLOSE_HK] = cpu_close_hk,
            [CPU_CROUCH_LP] = liu_kang_crouch_lp,
            [CPU_AIR_HP] = cpu_air_hp,
            [CPU_AIR_HK] = cpu_air_hk,
            [CPU_AIR_LK] = cpu_air_lk,
            [CPU_PROJECTILE] = liu_kang_low_fireball,
            [CPU_SPECIAL_1] = liu_kang_low_fireball,
            [CPU_SPECIAL_2] = liu_kang_flying_kick,
            [CPU_SPECIAL_NEAR] = liu_kang_bicycle_kick,
            [CPU_DRAW] = liu_kang_weapon,
            [CPU_FINISH_1] = liu_kang_first_fatality,
            [CPU_FINISH_2] = liu_kang_second_fatality,
            [CPU_FINISH_FAN] = liu_kang_fan_fatality,
            [CPU_FINISH_SPIKE] = liu_kang_spike_fatality,
        },
    .finish_min = {0x1cccc, 0},
    .finish_distance = {0x20000, 0x10000}, /* 4dfe10/4dfe50[3]. */
};

const CharacterDef mk4_liu_kang = {GAME_MK4, 3, "LIU KANG", &mk4_motion, moves, S_COUNT, &controls, enter, -70320, mk4_walk, mk4_jump, &cpu_moves};
