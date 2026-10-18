#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Native controller return used by generated VM programs and native runners. */
static void sub_zero_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void script_thrown_return(Fighter *fighter);
static void script_lp_getup(Fighter *fighter);
static void script_ready(Fighter *fighter);
#include "generated/sub_zero.inc"
#include "generated/frames_sub_zero.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x116c8, -0xde76, 0x1476c, 0x12395, 16, 13, 6, 0, 0x2147};       /* 0x4ebaf8, B selector 2 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void sub_zero_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void sub_zero_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void sub_zero_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))

/* Native waits keep their pose while ordinary C tick callbacks decide the
 * next phase. The runner owns cancellation and once-per-tick scheduling. */
static bool phase_wait(const Fighter *fighter) {
  (void)fighter;
  return false;
}

static void hold_phase(Fighter *fighter, const mmove_t *phase) {
  int sample = fighter_sample(fighter);
  fighter_mmove_phase(fighter, phase);
  fighter->sample_override = sample;
}

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
static const MoveDef s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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

static void uppercut_recover(Fighter *fighter);
static const mframe_t uppercut_loop_frames[] = {
    {.sample = 8, .duration = 1},
    {.sample = 7, .duration = 1},
    {.sample = 6, .duration = 1},
    {.sample = 7, .duration = 1},
    {.sample = 8, .duration = 1},
};
static const mmove_t uppercut_loop = {.frame = uppercut_loop_frames, .lastframe = 4, .endfunc = uppercut_recover};
static const mframe_t uppercut_late_frames[] = {
    {.sample = 9, .duration = 1},
    {.sample = 9, .duration = 1},
    {.sample = 8, .duration = 1},
    {.sample = 7, .duration = 1},
    {.sample = 6, .duration = 1},
    {.sample = 7, .duration = 1},
    {.sample = 8, .duration = 1},
};
static const mmove_t uppercut_late = {.frame = uppercut_late_frames, .lastframe = 6, .endfunc = uppercut_recover};
static const mframe_t uppercut_miss_end_frames[] = {
    {.sample = 8, .duration = 1},
    {.sample = 9, .duration = 1},
    {.sample = 10, .duration = 1},
    {.sample = 11, .duration = 1},
    {.sample = 12, .duration = 1},
    {.sample = 13, .duration = 1},
    {.sample = 14, .duration = 1},
    {.sample = 15, .duration = 1},
    {.sample = 16, .duration = 1},
    {.sample = 17, .duration = 1},
    {.sample = 18, .duration = 1},
    {.sample = 19, .duration = 1},
};
static const mmove_t uppercut_miss_end = {.frame = uppercut_miss_end_frames, .lastframe = 11, .endfunc = sub_zero_ready};
static const mframe_t uppercut_hit_end_frames[] = {
    {.sample = 9, .duration = 1},
    {.sample = 10, .duration = 1},
    {.sample = 11, .duration = 1},
    {.sample = 12, .duration = 1},
    {.sample = 13, .duration = 1},
    {.sample = 14, .duration = 1},
    {.sample = 15, .duration = 1},
    {.sample = 16, .duration = 1},
    {.sample = 17, .duration = 1},
    {.sample = 18, .duration = 1},
    {.sample = 19, .duration = 1},
};
static const mmove_t uppercut_hit_end = {.frame = uppercut_hit_end_frames, .lastframe = 10, .endfunc = sub_zero_ready};

static void uppercut_recover(Fighter *fighter) {
  if (fighter->script_timer-- > 0) {
    fighter_mmove_phase(fighter, fighter->script_phase ? &uppercut_late : &uppercut_loop);
    fighter->script_phase = 0;
  } else {
    fighter_mmove_phase(fighter, fighter->contact == CONTACT_MISS ? &uppercut_miss_end : &uppercut_hit_end);
  }
}

static void uppercut_branch(Fighter *fighter) {
  fighter->script_timer = fighter->contact == CONTACT_MISS ? 3 : fighter->contact == CONTACT_BLOCK ? 2 : 8;
  fighter->script_phase = fighter->contact_tick == 9;
  uppercut_recover(fighter);
}

static void uppercut_contact(Fighter *fighter) {
  fighter->sample_override = fighter_sample(fighter) + 1;
  sub_zero_uppercut_contact(fighter);
}

static const mframe_t uppercut_contact_frames[] = {
    {.sample = 5, .duration = 2, .leave = uppercut_contact},
    {.sample = 6, .duration = 1, .leave = uppercut_contact},
    {.sample = 7, .duration = 1, .leave = uppercut_contact},
    {.sample = 8, .duration = 1},
};
static const mmove_t uppercut_contact_phase = {.frame = uppercut_contact_frames, .lastframe = 3, .endfunc = uppercut_branch};

static void uppercut_script(Fighter *fighter) {
  hold_phase(fighter, &uppercut_contact_phase);
}

static void sub_zero_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  retail_script_004e62a0(fighter);
}

static void sub_zero_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  retail_script_004e6798(fighter);
}

static void sub_zero_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  retail_script_004e62f0(fighter);
}

static void sub_zero_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  retail_script_004e5c80(fighter);
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
    [S_PUNCH] = sub_zero_jab,
    [S_KICK] = sub_zero_kick,
    [S_SWEEP] = sub_zero_sweep,
    [S_UPPERCUT] = sub_zero_uppercut,
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

typedef struct GroundAttack {
  MoveDef move;
  AttackRecord record;
  int active, end_miss, end_contact;
} GroundAttack;

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 70, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 52, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 532, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 11, 29, 32},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
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

static void ground_tick(Fighter *fighter) {
  const int index = fighter->script_phase;
  const bool followup = fighter->script_timer != 0;
  const GroundAttack *attack = &ground_attacks[index];
  int t = fighter->ticks, fast = index == G_ROUNDHOUSE && followup ? 5 : 0;
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
      fighter->sample_override = mini(28, t - (fighter->connected ? mini(3, t - active) : 0));
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
    case G_LOW_KICK:
    case G_CROUCH_HK:
      require(false, "Generated-only attack entered a native recovery runner");
      break;
    case G_ROUNDHOUSE:
      fighter->sample_override = t <= 6 ? t * 2 : t < active ? mini(21, 12 + (t - 6) * (fast ? 2 : 1)) : mini(38, 22 + t - active);
      if (fighter->contact == CONTACT_BLOCK) {
        end = attack->end_miss - fast;
      }
      break;
  }
  if (t >= end) {
    fighter_finish_mmove(fighter, sub_zero_ready);
  }
}

static const mframe_t ground_frames[] = {{.sample = 0, .duration = 1, .tick = ground_tick}};
static const mmove_t ground_phase = {.frame = ground_frames, .until = phase_wait};

static void ground_native_tail(Fighter *fighter) {
  hold_phase(fighter, &ground_phase);
  ground_tick(fighter);
}

static void ground_attack(Fighter *fighter, int index) {
  bool followup = fighter_fast_start(fighter);
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->script_timer = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {retail_script_004e6da0, ground_native_tail, retail_script_004e6158, retail_script_004e5fd8, retail_script_004e61e0,
      retail_script_004e69c8, retail_script_004e6190, retail_script_004e6800};
  scripts[index](fighter);
}

static void hp_script(Fighter *fighter);

static void hp_tick(Fighter *fighter) {
  int phase = fighter->script_phase;
  int t = ++fighter->script_timer;
  if (phase >= 4) {
    fighter->sample_override = mini(phase == 4 ? 20 : 18, 11 + t);
    if (t >= (phase == 4 ? 10 : 8)) {
      fighter_finish_mmove(fighter, sub_zero_ready);
    }
    return;
  }
  int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
  int active = phase == 0 ? 8 : phase == 1 ? 7 : 6;
  fighter->sample_override = phase == 2 && t == active + 2 ? 17 : first + t;
  const Fighter *victim = fighter_opponent(fighter);
  bool ducking_kick = victim->action == ACTION_RETAIL_0102 && victim->avoids_high;
  if (t >= active && t <= active + 2 && !fighter->connected && !ducking_kick) {
    battle_contact(fighter, &hp_record);
  }
  if (t < active + 3) {
    return;
  }
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
  phase = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
  fighter_start_move(fighter, S_PUNCH, phase == 4 ? &s_hp : &s_hp_second);
  fighter->script_phase = phase;
  fighter->script_timer = 0;
  fighter->sample_override = phase == 1 ? 1 : phase == 3 ? 2 : 11;
  fighter->flags &= ~(unsigned)FF_BUSY;
  hp_script(fighter);
}

static const mframe_t hp_frames[] = {{.sample = 0, .duration = 1, .tick = hp_tick}};
static const mmove_t hp_phase = {.frame = hp_frames, .until = phase_wait};

static void hp_script(Fighter *fighter) {
  hold_phase(fighter, &hp_phase);
}

static void sub_zero_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter->script_phase = fighter->script_timer = 0;
  hp_script(fighter);
}

static void sub_zero_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void sub_zero_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void sub_zero_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void sub_zero_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void sub_zero_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void sub_zero_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void sub_zero_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    sub_zero_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    sub_zero_kick(fighter);
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
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sub_zero_hp);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sub_zero_roundhouse : sub_zero_hk);
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
                                                              : sub_zero_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : sub_zero_lk);
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
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 56, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {71434, -4587, 66846, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {55705, -2621, 51773, 38010, 22, 22, 4, ATTACK_MARK, 9830}, 6, 10, 10},
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

static int air_index(const Fighter *fighter) {
  for (int i = 0; i < COUNT(air_attacks); ++i) {
    if (fighter_move(fighter)->clip == air_attacks[i].move.clip) {
      return i;
    }
  }
  require(false, "Unknown Sub-Zero air attack clip");
  return 0;
}

static bool air_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static const mframe_t air_fall_frames[] = {{.sample = 0, .duration = 1}};
static const mmove_t air_fall_phase = {.frame = air_fall_frames, .until = air_grounded, .endfunc = air_land};

static void air_fall(Fighter *fighter) {
  fighter->script_phase = AIR_FALL;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, air_land);
    return;
  }
  hold_phase(fighter, &air_fall_phase);
}

static void air_unfreeze(Fighter *fighter) {
  fighter->buttons = NULL;
  fighter->body.gravity = air_index(fighter) >= 3 || fighter->script_phase == AIR_COMBO_FREEZE ? 0x51e : 0x28f;
  air_fall(fighter);
}

static const mframe_t air_freeze_frames[] = {{.sample = 0, .duration = 12}};
static const mframe_t air_diagonal_freeze_frames[] = {{.sample = 0, .duration = 8}};
static const mmove_t air_freeze_phase = {.frame = air_freeze_frames, .endfunc = air_unfreeze};
static const mmove_t air_diagonal_freeze_phase = {.frame = air_diagonal_freeze_frames, .endfunc = air_unfreeze};

static void air_cleanup(Fighter *fighter) {
  if (fighter->script_phase == AIR_FREEZE || fighter->script_phase == AIR_COMBO_FREEZE) {
    fighter->body.gravity = 0x51e;
  }
  if (fighter->buttons == &air_followup || fighter->buttons == &air_ground_followup) {
    fighter->buttons = NULL;
  }
}

static void air_tick(Fighter *fighter) {
  int index = air_index(fighter);
  const AirAttack *attack = &air_attacks[index];
  bool combo = fighter->script_phase == AIR_COMBO;
  int retract_at = attack->extend + attack->hold;
  int tick = ++fighter->script_timer;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, air_land);
    return;
  }
  fighter->sample_override = tick < attack->extend ? tick : tick < retract_at ? attack->extend - 1 : maxi(0, attack->retract - 1 - tick + retract_at);
  if (tick >= attack->extend && tick < retract_at && !fighter->connected && battle_contact(fighter, &attack->record) != CONTACT_MISS) {
    bool low = fighter->body.ground - fighter->body.pos[AXIS_Y] <= 0xcccc;
    stop_body(fighter);
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
    hold_phase(fighter, index >= 3 ? &air_diagonal_freeze_phase : &air_freeze_phase);
    return;
  }
  if (tick >= retract_at + attack->retract) {
    if (combo) {
      fighter->body.gravity = 0x51e;
    }
    air_fall(fighter);
  }
}

static const mframe_t air_frames[] = {{.sample = 0, .duration = 1, .tick = air_tick}};
static const mmove_t air_phase = {.frame = air_frames, .until = phase_wait};

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  require(air_index(fighter) == index, "Air native phase differs from selected clip");
  fighter->script_timer = elapsed ? elapsed - 1 : 0;
  hold_phase(fighter, &air_phase);
  if (elapsed) {
    air_tick(fighter);
  }
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
  static const FighterFn scripts[] = {retail_script_004e6658, retail_script_004e6710, retail_script_004e61f8, retail_script_004e6500, retail_script_004e63d8};
  fighter->mmove_cleanup = air_cleanup;
  scripts[index](fighter);
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
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : sub_zero_crouch_lp);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, sub_zero_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, sub_zero_crouch_lk);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, sub_zero_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    sub_zero_hp(fighter);
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
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sub_zero_roundhouse : sub_zero_kick);
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

/* LP: roster script 4de798[4] -> 4de6b8/4de898; reaction 102 is
 * 485b70 -> 4e9bd0, then reverse getup 488b80/4fbcdc.
 * LK: 4ddfb8[4] -> 42c180 -> 4de490/4de4b8/4de4e0;
 * 42c300 releases victim through 4871e0/4e9ee0/487220.
 * Pair alignment uses our shared world origin, not retail task/group pointers. */
static const MoveDef s_lk_throw = {.clip = 41, .last = 74, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 25, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 165, .last = 46, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_throw_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown_rise = {.clip = 200, .last = 40, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown_getup = {.clip = 328, .last = 14, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};

enum {
  LP_THROW_RELEASE = 25,
  LK_THROW_DISARM = 39, /* Opening 10 + paired 29. */
  LK_THROW_VOICE = 44,  /* Another paired 5. */
  LK_THROW_DAMAGE = 52, /* Another paired 8. */
  LK_THROW_RELEASE = 57 /* Final paired 5 before the independent victim tail. */
};

/* A newly installed peer can take its P1/P2 turn in the installation tick.
 * A displayed victim sample must still last one simulation tick. Keep the
 * first wait on that sample as well; subsequent waits follow the same rule. */
static void thrown_finish(Fighter *victim) {
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

static void lp_thrown_cleanup(Fighter *victim) {
  if (victim->body.physics == mk4_script_landing_motion) {
    victim->body.physics = NULL;
  }
  if (victim->body.on_land == mk4_script_impact) {
    victim->body.on_land = NULL;
  }
}

static void script_ready(Fighter *fighter) {
  FighterFn next = fighter->reaction == 102 && fighter_move(fighter)->clip == 143 ? script_thrown_return : fighter_ready;
  fighter_finish_mmove(fighter, next);
}

static void script_thrown_return(Fighter *victim) {
  if (fighter_move(victim)->clip != 143) {
    mk4_script_thrown_ready(victim);
    return;
  }
  stop_body(victim);
  if (victim->health <= 0) {
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown_getup);
  victim->sample_override = 0;
  script_lp_getup(victim);
}

static void lp_getup_done(Fighter *victim) {
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

/* Native 488b80: reversed getup clip 328, final sample held for one tick. */
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lp_getup_frames[] = {
  {.sample = 0, .duration = 1},
  {.sample = 1, .duration = 1},
  {.sample = 2, .duration = 1},
  {.sample = 3, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 7, .duration = 1},
  {.sample = 8, .duration = 1},
  {.sample = 9, .duration = 1},
  {.sample = 10, .duration = 1},
  {.sample = 11, .duration = 1},
  {.sample = 12, .duration = 1},
  {.sample = 13, .duration = 1},
  {.sample = 14, .duration = 1},
};
// clang-format on
static const mmove_t lp_getup_phase = {.firstframe = 0, .lastframe = COUNT(lp_getup_frames) - 1, .frame = lp_getup_frames, .endfunc = lp_getup_done};

static void script_lp_getup(Fighter *victim) {
  fighter_mmove_phase(victim, &lp_getup_phase);
}

static void lk_thrown_release(Fighter *victim) {
  victim->sample_override = 46;
  victim->body.physics = fighter_friction;
  retail_script_004e9ee0(victim);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lk_thrown_release_phase_frames[] = {
  {.sample = 42, .duration = 1},
  {.sample = 43, .duration = 1},
  {.sample = 44, .duration = 1},
  {.sample = 45, .duration = 1},
  {.sample = 46, .duration = 1},
};
// clang-format on
static const mmove_t lk_thrown_release_phase = {.frame = lk_thrown_release_phase_frames, .lastframe = 4, .endfunc = lk_thrown_release};

static void lk_thrown_script(Fighter *victim) {
  victim->script_phase = 1;
  fighter_mmove_phase(victim, &lk_thrown_release_phase);
}

static void throw_disarm(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
}

static void throw_cleanup(Fighter *owner) {
  Fighter *victim = opponent(owner);
  if ((victim->flags & FF_PAIRED) && (fighter_move(victim)->clip == 153 || fighter_move(victim)->clip == 165)) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->health <= 0 && victim->script_phase == 1) {
      return;
    }
    stop_body(victim);
    fighter_cancel_fiber(victim);
    fighter_cancel_mmove(victim);
    if (victim->pending == NULL && victim->health > 0) {
      fighter_ready(victim);
    }
  }
}

static void script_lk_rebase(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  stop_body(fighter);
  stop_body(victim);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  retail_script_004de4b8(fighter);
}

static void script_lk_damage(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  memcpy(fighter->body.vel, fighter->saved_motion, sizeof fighter->body.vel);
  for (int axis = 0; axis < 3; ++axis) {
    victim->body.vel[axis] = -fighter->saved_motion[axis];
  }
  fighter->body.physics = fighter_friction;
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  retail_script_004de4e0(fighter);
}

static const mframe_t lk_owner_idle_frames[] = {{.sample = 0, .duration = 60}};
static const mmove_t lk_owner_idle_phase = {.frame = lk_owner_idle_frames, .endfunc = sub_zero_ready};

static void script_lk_owner_return(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_mmove(fighter, S_THROW, &s_lk_throw_idle, &lk_owner_idle_phase);
}

static void script_lp_owner_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, sub_zero_ready);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t lk_rise_frames[] = {
  {.sample = 0, .duration = 1},
  {.sample = 1, .duration = 1},
  {.sample = 2, .duration = 1},
  {.sample = 3, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 7, .duration = 1},
  {.sample = 8, .duration = 1},
  {.sample = 9, .duration = 1},
  {.sample = 10, .duration = 1},
  {.sample = 11, .duration = 1},
  {.sample = 12, .duration = 1},
  {.sample = 13, .duration = 1},
  {.sample = 14, .duration = 1},
  {.sample = 15, .duration = 1},
  {.sample = 16, .duration = 1},
  {.sample = 17, .duration = 1},
  {.sample = 18, .duration = 1},
  {.sample = 19, .duration = 1},
  {.sample = 20, .duration = 1},
  {.sample = 21, .duration = 1},
  {.sample = 22, .duration = 1},
  {.sample = 23, .duration = 1},
  {.sample = 24, .duration = 1},
  {.sample = 25, .duration = 1},
  {.sample = 26, .duration = 1},
  {.sample = 27, .duration = 1},
  {.sample = 28, .duration = 1},
  {.sample = 29, .duration = 1},
  {.sample = 30, .duration = 1},
  {.sample = 31, .duration = 1},
  {.sample = 32, .duration = 1},
  {.sample = 33, .duration = 1},
  {.sample = 34, .duration = 1},
  {.sample = 35, .duration = 1},
  {.sample = 36, .duration = 1},
  {.sample = 37, .duration = 1},
  {.sample = 38, .duration = 1},
  {.sample = 39, .duration = 1},
  {.sample = 40, .duration = 1},
};
// clang-format on
static const mmove_t lk_rise = {.frame = lk_rise_frames, .lastframe = 40, .endfunc = thrown_finish};

static void script_lk_victim_rise(Fighter *victim) {
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_lk_thrown_rise, &lk_rise);
}

static bool lp_pair_released(const Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  return !(owner->flags & FF_PAIRED) || owner->pending != NULL;
}

static void lp_capture_done(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

/* Owner submits paired samples; the victim only watches capture lifetime. */
static const mframe_t lp_capture_frames[] = {{.sample = 0, .duration = 1}};
static const mmove_t lp_capture_phase = {.firstframe = 0, .lastframe = 0, .frame = lp_capture_frames, .until = lp_pair_released, .endfunc = lp_capture_done};

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    sub_zero_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  if (!lk) {
    memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
    victim->body.pos[AXIS_Y] = victim->body.ground + fighter->body.pos[AXIS_Y] - fighter->body.ground;
    victim->facing = fighter->facing;
    memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  }
  if (lk) {
    fighter_set_speed(fighter, -0x1999);
    mk4_script_save_motion(fighter);
  }
  fighter_mmove_phase(victim, &lp_capture_phase);
  fighter->mmove_cleanup = throw_cleanup;
  (lk ? retail_script_004de490 : retail_script_004de798)(fighter);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

/* Freeze: 49bc60 -> 4ed588/4ed818 -> 49bcb0. Flight is an independent
 * task, so it survives owner interruption; bounds and contact own teardown. */
static const AttackRecord ice_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_ice = {.clip = 16, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0304};
/* 49c300 creates rig 506a48 (mesh 74); 49bcfe installs 4ed898:
 * shared meshes 75..82, one tick each, looping without the spawn rig.
 * The high bit invokes 40d720 for the separately managed ice trail. */
static const MeshAnimation ice_meshes = {.first = 75, .count = 8, .period = 1, .scale = Q16_ONE};
/* 40d720/40d920, 4d1b18/28/34: ten independent trail nodes,
 * ten-tick track, quarter scale shrinking by .02 each tick.
 * The supplied EXE's 405430 gate always succeeds. */
static const ParticleVisual ice_trail_visual = {.animation = {339, 1, 1, 0x4000}, .billboard = true, .additive = true, .shrink = 0x51e};
static const ParticleSpray ice_trail = {.visual = &ice_trail_visual,
    .count = 10,
    .lifetime = 10,
    .velocity = {0, -0x7ae, 0},
    .spread = {0, 0x28f, 0},
    .offset = {0x1000, 0xc00, 0},
    .position_spread = {0x6666, 0x6666, 0x6666}};

static bool owned_object(const Fighter *fighter, int kind) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == kind && &g_battle.fighters[p->owner] == fighter) {
      return true;
    }
  }
  return false;
}

static void ice_projectile(Projectile *p) {
  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int i = 0; i < 3; ++i) {
    p->pos[i] += p->vel[i];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  if (opponent(owner)->action != ACTION_RETAIL_2001 && battle_projectile_contact(p, &ice_record) != CONTACT_MISS) {
    p->kind = 0;
  }
  int64_t radius2 = (int64_t)g_battle.arena_radius * g_battle.arena_radius + (int64_t)0xc0000 * Q16_ONE;
  if ((int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] > radius2) {
    p->kind = 0;
  }
  if (p->kind != 0) {
    battle_spray(&ice_trail, (Vec3){q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])}, (Vec3){0}, 0, 0);
  }
}

static void script_spawn_ice(Fighter *fighter, int task_type) {
  require(task_type == 66, "Invalid ice projectile task type");
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_ICE, 180, ice_projectile);
  if (p != NULL) {
    p->mesh_animation = &ice_meshes;
    p->initial_mesh = 74;
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      q16 heading = fighter_axis(fighter, axis);
      p->pos[axis] += q16_mul(heading, 0xccc);
      p->vel[axis] = q16_mul(heading, 0x3333);
    }
    p->pos[AXIS_Y] -= 0x28f;
  }
}

static bool can_ice(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && fighter_opponent(fighter)->hits <= 1 && !owned_object(fighter, PROJECTILE_CLONE);
}

static void sub_zero_ice(Fighter *fighter) {
  bool followup = fighter_fast_start(fighter);
  stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_ice);
  fighter->script_phase = followup;
  fighter->sample_override = 0;
  retail_script_004ed588(fighter);
}

/* 47f4d0 copies the current pose before the retreat. Both ice predicates
 * inspect clone task types 45/46, not the projectile task type 42. */
static const AttackRecord clone_record = {0x4ccc, -0xa3d, 0xbae1, 0x9e76, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_clone = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0405};
static const MoveDef s_clone_near = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0406};
static const OrbitLightDef clone_light = {25 * Q16_ONE, -25 * Q16_ONE, 0x18000, 0xccc};

static void clone_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (g_battle.contacts_off || battle_contact_from(owner, &clone_record, p->pos, p->facing) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void clone_cleanup(Fighter *fighter) {
  fighter_upright(fighter);
}

static void clone_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, air_land);
}

static const mframe_t clone_land_frames[] = {{.sample = 0, .duration = 1}};
static const mmove_t clone_land_phase = {.frame = clone_land_frames, .until = air_grounded, .endfunc = clone_done};

static void clone_land(Fighter *fighter) {
  fighter->sample_override = 0;
  if (fighter_airborne(fighter)) {
    fighter_mmove_phase(fighter, &clone_land_phase);
  } else {
    clone_done(fighter);
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_reverse_frames[] = {
  {.sample = 8, .duration = 1},
  {.sample = 7, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 3, .duration = 1},
  {.sample = 2, .duration = 1},
  {.sample = 1, .duration = 1},
};
// clang-format on
static const mmove_t clone_reverse = {.frame = clone_reverse_frames, .lastframe = 7, .endfunc = clone_land};

static void clone_turn(Fighter *fighter) {
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &clone_reverse);
}

static bool clone_falling(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static const mframe_t clone_apex_frames[] = {{.sample = 8, .duration = 1}};
static const mmove_t clone_apex = {.frame = clone_apex_frames, .until = clone_falling, .endfunc = clone_turn};

static void clone_apex_wait(Fighter *fighter) {
  fighter_mmove_phase(fighter, &clone_apex);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_extend_frames[] = {
  {.sample = 3, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 7, .duration = 1},
};
// clang-format on
static const mmove_t clone_extend = {.frame = clone_extend_frames, .lastframe = 4, .endfunc = clone_apex_wait};

static void clone_spin(Fighter *fighter) {
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = -0x6147;
  fighter_mmove_phase(fighter, &clone_extend);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t clone_start_frames[] = {
  {.sample = 0, .duration = 1},
  {.sample = 1, .duration = 1},
  {.sample = 2, .duration = 1},
};
// clang-format on
static const mmove_t clone_start = {.frame = clone_start_frames, .lastframe = 2, .endfunc = clone_spin};

static bool can_clone(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->clone_ready && fighter_opponent(fighter)->action != ACTION_RETAIL_1003 &&
         !owned_object(fighter, PROJECTILE_CLONE);
}

static void sub_zero_clone(Fighter *fighter) {
  bool near = fighter_distance(fighter) < 1.f;
  if (!near) {
    Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_CLONE, fighter_opponent(fighter)->human ? 74 : 53, clone_object);
    if (p != NULL) {
      p->clip = fighter_move(fighter)->clip;
      p->sample = fighter_pose_sample(fighter);
      battle_object_light(p, &clone_light);
    }
  }
  stop_body(fighter);
  fighter_start_move(fighter, S_JUMP, near ? &s_clone_near : &s_clone);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, -0x170a);
  fighter->body.vel[AXIS_Y] = -0x28f5;
  fighter->body.gravity = 0x28f;
  fighter->clone_ready = g_battle.ticks + 180;
  battle_sound(SOUND_C, 0xa);
  fighter->mmove_cleanup = clone_cleanup;
  fighter_mmove_phase(fighter, &clone_start);
}

/* Slide: 47dc00/4e84d8 -> 47dc40; hit 4e84f8, block 4e8530.
 * Only an unblocked contact stamps cooldown slot 2 (47de8e). */
static const AttackRecord slide_record = {0xf0a3, 0x947a, 0x111eb, 0x8f5c, 122, 122, 165, 0, 0x147a};
static const MoveDef s_slide = {.clip = 95, .last = 10, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0204};

static void slide_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  stop_body(fighter);
}

static void slide_contact_tick(Fighter *fighter) {
  if (++fighter->script_timer < 9) {
    return;
  }
  fighter->sample_override = 9;
  ContactResult contact = battle_contact(fighter, &slide_record);
  if (contact == CONTACT_MISS) {
    return;
  }
  fighter->script_phase = 1;
  if (contact == CONTACT_BLOCK) {
    retail_script_004e8530(fighter);
  } else {
    fighter->slide_ready = g_battle.ticks + 64;
    retail_script_004e84f8(fighter);
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t slide_contact_frames[] = {
  {.sample = 0, .duration = 1, .tick = slide_contact_tick},
  {.sample = 1, .duration = 1, .tick = slide_contact_tick},
  {.sample = 2, .duration = 1, .tick = slide_contact_tick},
  {.sample = 3, .duration = 1, .tick = slide_contact_tick},
  {.sample = 4, .duration = 1, .tick = slide_contact_tick},
  {.sample = 5, .duration = 1, .tick = slide_contact_tick},
  {.sample = 6, .duration = 1, .tick = slide_contact_tick},
  {.sample = 7, .duration = 1, .tick = slide_contact_tick},
  {.sample = 8, .duration = 1, .tick = slide_contact_tick},
  {.sample = 9, .duration = 16, .tick = slide_contact_tick},
};
// clang-format on
static const mmove_t slide_contact_phase = {
    .firstframe = 0, .lastframe = COUNT(slide_contact_frames) - 1, .frame = slide_contact_frames, .endfunc = script_slide_return};

static void slide_script(Fighter *fighter) {
  fighter->script_timer = 0;
  fighter_mmove_phase(fighter, &slide_contact_phase);
}

static bool can_slide(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && (fighter->held & mask) == chord;
}

static void sub_zero_slide(Fighter *fighter) {
  stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_slide);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = slide_cleanup;
  retail_script_004e84d8(fighter);
}

/* Shared weapon draw 4e62c0/4e62d8, air 4e66d0/4e66f0 and
 * throw 498290. Scepter ground strikes use character-4 descriptor 4ecdb8. */
/* Scepter descriptor 4ecdb8. Parameter blocks 4ec458/490/480/4b8
 * select the shared native attack runner and its recovery continuations. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike scepter_attacks[] = {
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {128385, -35323, 92274, 22872, 51, 51, 119, 0, 11796}, 6, 10, 16, 4, 1, 0, 0, 0, 0x20},
  {{.clip = 250, .last = 31, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0111}, {115277, 20316, 73400, 51052, 150, 150, 118, ATTACK_LOW, 9830}, 0, 4, 12, 2, 9, 0, 0, 0, 0x22},
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 8, 0x147a, 0x22},
  {{.clip = 228, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0113}, {123076, -26804, 91684, 14352, 151, 151, 118, 0, 0}, 0, 10, 16, 5, 5, 1, 14, 0x2666, 0x22},
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

static const WeaponDef scepter;

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, index == 0 || index == 2, (index == 0 || index == 2) ? retail_script_004e66d0 : retail_script_004e66f0);
  } else {
    weapon_start_strike(fighter, &scepter_attacks[index], true);
  }
}

static void scepter_hp(Fighter *fighter) {
  scepter_attack(fighter, 0);
}

static void scepter_lp(Fighter *fighter) {
  scepter_attack(fighter, 1);
}

static void scepter_back_hp(Fighter *fighter) {
  scepter_attack(fighter, 2);
}

static void scepter_back_lp(Fighter *fighter) {
  scepter_attack(fighter, 3);
}

static const WeaponDef scepter = {0x6b, scepter_hp, scepter_lp, scepter_back_hp, scepter_back_lp, NULL, weapon_throw, equip_scepter, SCENE_MODEL_SCEPTER};

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void sub_zero_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_scepter(fighter);
    weapon_followup_victim(fighter);
    weapon_start_strike(fighter, &scepter_attacks[2], false);
    fighter->action = ACTION_RETAIL_010E;
  } else {
    weapon_draw(fighter, &scepter);
    retail_script_004e62c0(fighter);
  }
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void sub_zero_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

/* 45e820: RUN latch is written after matching, so this sees the prior
 * press. 4692e0 uses shared descriptor 53ce18 (clip 300), not a roster
 * table. Down plays it forwards; otherwise it plays backwards. */

static bool can_sidestep(const Fighter *fighter) {
  return mk4_can_sidestep(fighter);
}

static void sub_zero_sidestep(Fighter *fighter) {
  mk4_sidestep(fighter);
}

/* Generated VM finishers use shared scene, blend and object helpers. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(385, 140), FATAL_MOVE(387, 73), FATAL_MOVE(552, 104),
  FATAL_MOVE(95, 10), FATAL_MOVE(258, 38), FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
  FATAL_MOVE(115, 18), FATAL_MOVE(116, 49), FATAL_MOVE(117, 18), FATAL_MOVE(118, 49),
  FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48),
};
// clang-format on
#undef FATAL_MOVE

// clang-format off: Keep table entries on one row, including long rows.
/* These records are native blend operations. VM playback is generated. */
#define BLEND(c, sample, step, first) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, sample, sample, 1, step, first}
static const SceneClip first_actor_blend[] = {BLEND(385, 0, 0x32f1, 0x3333)};
static const SceneClip first_victim_setup[] = {BLEND(389, 0, 0x32f1, 0x3333), BLEND(387, 0, 0x32f1, 0x3333)};
static const SceneClip first_victim_blends[] = {BLEND(387, 46, 0x666, 0x666), BLEND(387, 49, 0x43f, 0x43f), BLEND(387, 50, 0x43f, 0x886)};
#undef BLEND
// clang-format on

static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        int scene_timer = fighter->script_timer;
        fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        fighter->script_timer = scene_timer;
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

/* 45e460 -> mask 4e4178: HP + BLOCK + RUN; no Up/HK/LP/LK. */
static bool can_first_fatality(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  return can_close_fatality(fighter) && (fighter->held & mask) == chord;
}

static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_close_fatality(fighter);
}

static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_close_fatality(fighter);
}

/* 445920: camera records 4e0f00/4e0f58, reordered from X,Z,Y. */
// clang-format off: Keep table entries on one row, including long rows.
static const q16 first_camera[][3] = {
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
  {131072, -131072, -196608},
};
static const q16 orbit_camera[][3] = {
  {-327680, -163840, -458752},
  {-393216, -183500, -393216},
  {-393216, -255590, 393216},
  {393216, -216268, 393216},
  {393216, -190054, -393216},
  {-458752, -170393, -458752},
  {-524288, -157286, 524288},
  {589824, -144179, 589824},
  {655360, -117964, -655360},
  {-655360, -104857, -655360},
  {-655360, -104857, 655360},
  {655360, -104857, 655360},
  {655360, -117964, -655360},
  {-655360, -131072, -655360},
  {-655360, -144179, 655360},
  {655360, -150732, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
  {-655360, -157286, -655360},
  {-655360, -157286, 655360},
  {655360, -157286, 655360},
  {655360, -157286, -655360},
};
static const CameraShot first_camera_shots[] = {
  {first_camera, COUNT(first_camera), 50, 0xccc, 0, 0, 0},
  {orbit_camera, COUNT(orbit_camera), 0, 0x45a, 3, 80, -0x4000},
};
/* 445740/446620 and 4e0440: independent head/spine playback. */
static const AttachmentDef first_spine = {.model = SCENE_MODEL_SPINE, .clip = 386, .first = 0, .last = 96, .loop_from = 52,
    .group_offset = {0, -0xccc / 65536.f, 0}, .piece_offset = {0, 0x2b43 / 65536.f, 0}, .piece_kind = 0, .scene_space = true};
// clang-format on

/* 445c40: eleven double sprays, eleven singles, thirteen 2/1-tick
 * pairs, then seventy-one singles two ticks apart. 445e20 aims from
 * joint 2 away from joint 3; 474330 selects the shared blood mesh. */
static const ParticleVisual neck_blood_visual = {.animation = {0, 1, 1, Q16_ONE}};
static const ParticleSpray neck_blood = {
    .visual = &neck_blood_visual, .count = 1, .lifetime = 180, .gravity = 0x51e, .spread = {0x7ae, 0x7ae, 0x7ae}, .splash = true};
// clang-format off: Keep table entries on one row, including long rows.
static const EmissionPhase neck_blood_phases[] = {
  {11, 1, -1, 2},
  {11, 1, -1, 1},
  {39, 3, 2, 1},
  {142, 2, -1, 1},
};
// clang-format on
static const JointSprayDef first_blood = {.spray = &neck_blood,
    .phases = neck_blood_phases,
    .count = COUNT(neck_blood_phases),
    .origin_joint = 2,
    .base_joint = 3,
    .speed = 0x23d7,
    .variation = 0x7ae};

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void scene_blend_cleanup(Fighter *fighter) {
  fighter->scene.clips = NULL;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

static void scene_blend_tick(Fighter *fighter) {
  if (scene_step(fighter)) {
    fighter->scene.clips = NULL;
  }
}

static bool scene_blend_done(const Fighter *fighter) {
  return fighter->scene.clips == NULL;
}

static const mframe_t scene_blend_frames[] = {{.sample = 0, .duration = 1, .tick = scene_blend_tick}};
static const mmove_t first_actor_blend_phase = {.frame = scene_blend_frames, .until = scene_blend_done, .endfunc = retail_script_004e0378};
static const mmove_t first_victim_setup_phase = {.frame = scene_blend_frames, .until = scene_blend_done, .endfunc = retail_script_004e03f0};
static const mmove_t first_victim_blend_phase = {.frame = scene_blend_frames, .until = scene_blend_done, .endfunc = retail_script_004e0420};

static void native_blends(Fighter *fighter, const SceneClip *clips, int count, const mmove_t *phase) {
  scene_begin(fighter, clips, count, -1);
  fighter->mmove_cleanup = scene_blend_cleanup;
  hold_phase(fighter, phase);
}

static void first_camera_start(Fighter *fighter) {
  battle_camera_sequence(fighter, opponent(fighter), first_camera_shots, COUNT(first_camera_shots));
  retail_script_004e0368(fighter);
}

static bool first_scene_due(const Fighter *fighter) {
  (void)fighter;
  return g_battle.phase_ticks >= 8;
}

static const mframe_t first_scene_wait_frames[] = {{.sample = 0, .duration = 1}};
static const mmove_t first_scene_wait = {.frame = first_scene_wait_frames, .until = first_scene_due, .endfunc = first_camera_start};

static void first_fatality_script(Fighter *fighter) {
  fighter_mmove_phase(fighter, &first_scene_wait);
}

static void first_victim_start(Fighter *victim) {
  native_blends(victim, first_victim_setup, COUNT(first_victim_setup), &first_victim_setup_phase);
}

static void script_first_begin(Fighter *fighter) {
  fighter->script_phase = 1;
  native_blends(fighter, first_actor_blend, COUNT(first_actor_blend), &first_actor_blend_phase);
}

static void script_first_pull(Fighter *fighter) {
  first_victim_start(opponent(fighter));
  retail_script_004e0388(fighter);
}

static void script_first_cues1(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  battle_sound(SOUND_B, 0x29);
  retail_script_004e0398(fighter);
}

static void script_first_cues2(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2b);
  retail_script_004e03a8(fighter);
}

static void script_first_cues3(Fighter *fighter) {
  battle_sound(SOUND_B, 0x2a);
  retail_script_004e03b8(fighter);
}

static void script_first_cues4(Fighter *fighter) {
  battle_sound(SOUND_B, 0x1b);
  retail_script_004e03c8(fighter);
}

static void script_first_head(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  battle_sound(SOUND_B, 0x1c);
  battle_sound(SOUND_B, 0);
  ScenePiece *head = battle_attach_head(victim, fighter, 10);
  battle_attach_piece_model(head, &first_spine, fighter);
  battle_joint_spray(victim, &first_blood);
  retail_script_004e03d8(fighter);
}

static void script_first_victim_blends(Fighter *victim) {
  native_blends(victim, first_victim_blends, COUNT(first_victim_blends), &first_victim_blend_phase);
}

static void script_first_victim_collapse(Fighter *victim) {
  battle_sound(SOUND_B, 0x18);
  battle_camera_shake(2, 3, 0x2e14);
  retail_script_004e0430(victim);
}

static void script_first_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, finish_scene);
}

static void script_breath_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
}

static void breath_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (owner->action != ACTION_RETAIL_0700 || fighter_move(owner)->clip != 552) {
    p->kind = 0;
    return;
  }
  battle_particles(owner, 9 + (p->ticks & 1), 2, true);
}

static void script_breath_object(Fighter *fighter, int type) {
  require(type == 0, "Unexpected breath task");
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 92, breath_object);
}

static void breath_freeze_object(Projectile *object) {
  Fighter *victim = opponent(&g_battle.fighters[object->owner]);
  if (victim->appearance != APPEARANCE_FROZEN || g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 8 == 0) {
    battle_particles(victim, 9, 1, true);
  }
}

static void script_breath_freeze(Fighter *fighter, int type) {
  require(type == 0, "Unexpected freeze task");
  opponent(fighter)->appearance = APPEARANCE_FROZEN;
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 120, breath_freeze_object);
}

static void second_fatality_script(Fighter *fighter) {
  retail_script_004d2bf0(opponent(fighter));
  retail_script_004d2c10(fighter);
}

static void script_breath_approach(Fighter *fighter) {
  retail_script_004d2c78(fighter);
}

static void breath_travel_tick(Fighter *fighter) {
  if (--fighter->script_timer == 0) {
    fighter_set_speed(fighter, 0);
    fighter->script_phase = 3;
    retail_script_004d2ca0(fighter);
  }
}

static const mframe_t breath_travel_frames[] = {{.sample = 9, .duration = 1, .tick = breath_travel_tick}};
static const mmove_t breath_travel_phase = {.frame = breath_travel_frames, .until = phase_wait};

static void breath_travel(Fighter *fighter) {
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  fighter->script_timer = maxi(0, q16_mul(distance - 0xcccc, 0x69999) / Q16_ONE) + 1;
  fighter->script_phase = 2;
  fighter_mmove_phase(fighter, &breath_travel_phase);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t breath_slide_phase_frames[] = {
  {.sample = 0, .duration = 1},
  {.sample = 1, .duration = 1},
  {.sample = 2, .duration = 1},
  {.sample = 3, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 7, .duration = 1},
  {.sample = 8, .duration = 1},
};
// clang-format on
static const mmove_t breath_slide_phase = {.frame = breath_slide_phase_frames, .lastframe = 8, .endfunc = breath_travel};

static void script_breath_slide(Fighter *fighter) {
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &breath_slide_phase);
}

static void script_breath_shatter(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  battle_particles(victim, 3, 32, true);
  battle_shatter(victim);
  retail_script_004d2cc8(fighter);
}

static void victory_watch(Fighter *fighter) {
  if (fighter_move(fighter)->clip == 118 && ++fighter->script_timer >= 50) {
    fighter_cancel_all_mmoves();
    battle_end_finisher();
  }
}

static void script_victory(Fighter *fighter) {
  fighter->script_timer = 0;
  fighter->body.physics = victory_watch;
  retail_script_004e6a40(fighter);
}

static void fan_watch(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  int rise = mini(30, ++fighter->script_timer);
  victim->body.pos[AXIS_Y] = victim->body.ground - rise * 0x3333;
  if (fighter->script_timer >= 102) {
    fighter_cancel_all_mmoves();
    battle_end_finisher();
  }
}

static void script_fan_cycle(Fighter *fighter) {
  if (--fighter->script_timer > 0) {
    retail_script_004e0138(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  retail_script_004e01a0(victim);
  fighter->script_timer = 0;
  fighter->body.physics = fan_watch;
  retail_script_004e0148(fighter);
}

static void script_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  fighter->script_timer = 31;
  retail_script_004e0138(fighter);
}

static void spike_tick(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  fighter->sample_override = mini(19, fighter_sample(fighter) + 1);
  if (g_battle.phase_ticks >= 64) {
    int rise = mini(22, g_battle.phase_ticks - 64);
    mk4_script_scene_pose(victim, 128, 110, mini(31, rise * 2));
    victim->body.pos[AXIS_Y] = rise == 22 ? -0x59999 : victim->body.ground - rise * 0x3333;
  }
  if (g_battle.phase_ticks >= 324) {
    const MoveDef *victory = roster_def(fighter->roster)->moves[S_VICTORY];
    fighter_start_move(fighter, S_VICTORY, victory);
    fighter->sample_override = victory->last == MOVE_END ? 0 : victory->last;
    finish_scene(fighter);
  }
}

static const mframe_t spike_frames[] = {{.sample = 5, .duration = 1, .tick = spike_tick}};
static const mmove_t spike_phase = {.frame = spike_frames, .until = phase_wait};

static void script_spike_tail(Fighter *fighter) {
  hold_phase(fighter, &spike_phase);
}

static void fan_start(Fighter *fighter) {
  retail_script_004e0188(opponent(fighter));
  retail_script_004e0118(fighter);
}

static bool spike_due(const Fighter *fighter) {
  (void)fighter;
  return g_battle.phase_ticks >= 30;
}

static const mframe_t spike_wait_frames[] = {{.sample = 0, .duration = 1}};
static const mmove_t spike_wait = {.frame = spike_wait_frames, .until = spike_due, .endfunc = retail_script_004e6950};

static void spike_start(Fighter *fighter) {
  fighter_mmove_phase(fighter, &spike_wait);
}

static void start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    sub_zero_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_SUB_ZERO_1 ? 385 : id == FINISH_SUB_ZERO_2 ? 552 : id == FINISH_FAN ? 415 : 293, 0, true);
  if (id == FINISH_SUB_ZERO_1) {
    /* 445100 -> 475530 aligns the scene pair at 1.45 units. */
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0x17333);
    }
  }
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
  }
  FighterFn start = id == FINISH_SUB_ZERO_1   ? first_fatality_script
                    : id == FINISH_SUB_ZERO_2 ? second_fatality_script
                    : id == FINISH_FAN        ? fan_start
                                              : spike_start;
  start(fighter);
}

static void sub_zero_first_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SUB_ZERO_1);
}

static void sub_zero_second_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SUB_ZERO_2);
}

static void sub_zero_fan_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_FAN);
}

static void sub_zero_spike_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SPIKE);
}

static void script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return; /* Spike's native arena continuation owns the victim. */
  }
  require(reaction == 102, "Unexpected Sub-Zero generated reaction");
  Fighter *victim = opponent(fighter);
  throw_disarm(victim);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  victim->reaction = reaction;
  victim->sample_override = 25;
  victim->flags |= FF_REACTING;
  victim->mmove_cleanup = lp_thrown_cleanup;
  retail_script_004e9bd0(victim);
}

static void kick_contact(Fighter *fighter) {
  fighter->sample_override = fighter_sample(fighter) + 1;
  sub_zero_kick_contact(fighter);
}

static const mframe_t kick_contact_frames[] = {
    {.sample = 10, .duration = 1, .leave = kick_contact},
    {.sample = 11, .duration = 1, .leave = kick_contact},
    {.sample = 12, .duration = 1, .leave = kick_contact},
};
static const mmove_t kick_contact_phase = {.frame = kick_contact_frames, .lastframe = 2, .endfunc = retail_script_004e6998};

static void script_kick_tail(Fighter *fighter) {
  hold_phase(fighter, &kick_contact_phase);
}

/* 46f080/46f130: two native contact visits, then remaining clip playback.
 * Keep the established portable timing; source-script startup is generated. */
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t sweep_recovery_frames[] = {
  {.sample = 22, .duration = 1},
  {.sample = 23, .duration = 1},
  {.sample = 24, .duration = 1},
  {.sample = 25, .duration = 1},
  {.sample = 26, .duration = 1},
  {.sample = 27, .duration = 1},
  {.sample = 28, .duration = 1},
  {.sample = 29, .duration = 1},
  {.sample = 30, .duration = 1},
  {.sample = 31, .duration = 1},
  {.sample = 32, .duration = 1},
  {.sample = 33, .duration = 1},
  {.sample = 34, .duration = 1},
  {.sample = 35, .duration = 1},
  {.sample = 36, .duration = 1},
  {.sample = 37, .duration = 1},
  {.sample = 38, .duration = 1},
  {.sample = 39, .duration = 1},
  {.sample = 40, .duration = 1},
  {.sample = 41, .duration = 1},
  {.sample = 42, .duration = 1},
  {.sample = 43, .duration = 1},
  {.sample = 44, .duration = 1},
  {.sample = 45, .duration = 1},
};
// clang-format on
static const mmove_t sweep_recovery = {
    .firstframe = 0, .lastframe = COUNT(sweep_recovery_frames) - 1, .frame = sweep_recovery_frames, .endfunc = sub_zero_ready};

static void sweep_recover(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_recovery);
}

static const mframe_t sweep_contact_frames[] = {{.sample = 22, .duration = 2, .tick = sub_zero_sweep_contact}};
static const mmove_t sweep_contact = {.firstframe = 0, .lastframe = 0, .frame = sweep_contact_frames, .endfunc = sweep_recover};

static void script_sweep_tail(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_contact);
}

static void script_crouch_lk_tail(Fighter *fighter) {
  hold_phase(fighter, &ground_phase);
}

static void script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_combo_hp_tail(Fighter *fighter) {
  hold_phase(fighter, &ground_phase);
}

static void script_roundhouse_tail(Fighter *fighter) {
  hold_phase(fighter, &ground_phase);
}

static void script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
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

static void script_draw_equip(Fighter *fighter) {
  equip_scepter(fighter);
  retail_script_004e62d8(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t slide_return_frames[] = {
  {.sample = 10, .duration = 1},
  {.sample = 9, .duration = 1},
  {.sample = 8, .duration = 1},
  {.sample = 7, .duration = 1},
  {.sample = 6, .duration = 1},
  {.sample = 5, .duration = 1},
  {.sample = 4, .duration = 1},
  {.sample = 3, .duration = 1},
  {.sample = 2, .duration = 1},
  {.sample = 1, .duration = 1},
  {.sample = 0, .duration = 1},
};
// clang-format on
static const mmove_t slide_return_phase = {
    .firstframe = 0, .lastframe = COUNT(slide_return_frames) - 1, .frame = slide_return_frames, .endfunc = sub_zero_ready};

static void script_slide_return(Fighter *fighter) {
  stop_body(fighter);
  fighter->script_phase = 2;
  fighter_mmove_phase(fighter, &slide_return_phase);
}

static void script_air_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static const MoveDef s_air_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t air_getup_frames[] = {
  {.sample = 46, .duration = 1},
  {.sample = 47, .duration = 1},
  {.sample = 48, .duration = 1},
  {.sample = 49, .duration = 1},
  {.sample = 50, .duration = 1},
  {.sample = 51, .duration = 1},
  {.sample = 52, .duration = 1},
  {.sample = 53, .duration = 1},
  {.sample = 54, .duration = 1},
  {.sample = 55, .duration = 1},
  {.sample = 56, .duration = 1},
  {.sample = 57, .duration = 1},
  {.sample = 58, .duration = 1},
  {.sample = 59, .duration = 1},
  {.sample = 60, .duration = 1},
  {.sample = 61, .duration = 1},
  {.sample = 62, .duration = 1},
  {.sample = 63, .duration = 1},
  {.sample = 64, .duration = 1},
  {.sample = 65, .duration = 1},
  {.sample = 66, .duration = 1},
  {.sample = 67, .duration = 1},
  {.sample = 68, .duration = 1},
  {.sample = 69, .duration = 1},
  {.sample = 70, .duration = 1},
  {.sample = 71, .duration = 1},
  {.sample = 72, .duration = 1},
  {.sample = 73, .duration = 1},
  {.sample = 74, .duration = 1},
  {.sample = 75, .duration = 1},
  {.sample = 76, .duration = 1},
  {.sample = 77, .duration = 1},
  {.sample = 78, .duration = 1},
  {.sample = 79, .duration = 1},
};
// clang-format on
static const mmove_t air_getup = {.frame = air_getup_frames, .lastframe = 33, .endfunc = fighter_ready};

static void script_air_getup(Fighter *victim) {
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_air_getup, &air_getup);
}

static void sub_zero_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void script_air_release(Fighter *fighter) {
  mk4_mmove_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN, TOKEN_BACK}, 4, 50, 0, can_second_fatality, sub_zero_second_fatality},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0, can_ice, sub_zero_ice},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 18, CMD_AIR, can_clone, sub_zero_clone},
  {BUTTON_BLOCK, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_BLOCK, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
  /* Requested extension: retail Sub-Zero has no 4e4a04 air-throw record.
   * Keep his slide/fatality priority, then reuse the shared retail throw. */
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, sub_zero_air_throw},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_UP, TOKEN_UP, TOKEN_UP}, 4, 48, 0, can_fan_fatality, sub_zero_fan_fatality},
  {BUTTON_LOW_KICK, {0}, 0, 0, 0, can_slide, sub_zero_slide},
  {BUTTON_LOW_KICK, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, can_spike_fatality, sub_zero_spike_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 24, 0, can_weapon, sub_zero_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, can_sidestep, sub_zero_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, sub_zero_pickup},
  {BUTTON_RUN, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, can_first_fatality, sub_zero_first_fatality},
};
// clang-format on

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

/* Shared CPU selections use these native entries directly, as 45f5f0 does.
 * Human command patterns and held-button predicates do not gate drone actions. */
static void cpu_close_hp(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HP);
}

static void cpu_close_hk(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HK);
}

static const CpuMoves cpu_moves = {.enter = {
                                       [CPU_JAB] = sub_zero_jab,
                                       [CPU_LOW_KICK] = sub_zero_lk,
                                       [CPU_HIGH_KICK] = sub_zero_hk,
                                       [CPU_SWEEP] = enter_sweep,
                                       [CPU_UPPERCUT] = enter_uppercut,
                                       [CPU_THROW] = enter_throw,
                                       [CPU_CLOSE_HP] = cpu_close_hp,
                                       [CPU_CLOSE_HK] = cpu_close_hk,
                                       [CPU_CROUCH_LP] = sub_zero_crouch_lp,
                                       [CPU_AIR_HP] = air_hp,
                                       [CPU_AIR_HK] = air_hk_diagonal,
                                       [CPU_AIR_LK] = air_lk_diagonal,
                                       [CPU_PROJECTILE] = sub_zero_ice,
                                       [CPU_SPECIAL_1] = sub_zero_clone,
                                       [CPU_SPECIAL_2] = sub_zero_slide,
                                       [CPU_DRAW] = sub_zero_weapon,
                                       [CPU_FINISH_1] = sub_zero_first_fatality,
                                       [CPU_FINISH_2] = sub_zero_second_fatality,
                                       [CPU_FINISH_FAN] = sub_zero_fan_fatality,
                                       [CPU_FINISH_SPIKE] = sub_zero_spike_fatality,
                                   }};

const CharacterDef mk4_sub_zero = {GAME_MK4, 4, "SUB ZERO", &mk4_motion, moves, S_COUNT, &controls, enter, -77660, mk4_walk, mk4_jump, &cpu_moves};
