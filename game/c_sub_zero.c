#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void sub_zero_ready(Battle *battle, Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(battle, fighter, S_IDLE);
  }
}

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord jab_record = {0xf333, -0x47ae, 0xcccc, 0x7333, 37, 11, 1, 0, 0xa3d};            /* 0x4ebd38, query A selector 1 */
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x116c8, -0xde76, 0x1476c, 0x12395, 16, 13, 6, 0, 0x2147};       /* 0x4ebaf8, B selector 2 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Battle *battle, const Fighter *fighter) {
  return fighter_opponent(battle, fighter)->action;
}

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */
static void sub_zero_jab_contact(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int action = victim->action;
  if (fighter->connected || action == 0x2001 || action == 0x106 || action == 0x107) {
    return;
  }
  if ((action == 0x10c && fighter_sample(victim) < 5) || (action == 0x102 && victim->avoids_high)) {
    return;
  }
  battle_contact(battle, fighter, &jab_record);
}

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void sub_zero_kick_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected && victim_action(battle, fighter) != 0x2001) {
    battle_contact(battle, fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void sub_zero_sweep_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(battle, fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void sub_zero_uppercut_contact(Battle *battle, Fighter *fighter) {
  if (fighter->connected || victim_action(battle, fighter) == 0x2001) {
    return;
  }
  if (battle_contact(battle, fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveEvent kick_events[] = {{11, sub_zero_kick_contact}, {12, sub_zero_kick_contact}, {13, sub_zero_kick_contact}};
static const MoveSegment kick_segments[] = {
  {0, 0, 1, 1},
  {11, 10, 1, 1},
  {14, 12, 1, 1},
  {16, 15, 1, 1},
};
// clang-format on

static const MoveSequence kick_sequence = {kick_segments, (int)(sizeof kick_segments / sizeof kick_segments[0]), kick_events, 3, 29, sub_zero_ready, NULL};

static void sub_zero_jab_next(Battle *battle, Fighter *fighter);
static void sub_zero_jab_second_end(Battle *battle, Fighter *fighter);
static void sub_zero_jab_finish_end(Battle *battle, Fighter *fighter);
static void sub_zero_uppercut_recover(Battle *battle, Fighter *fighter);
static void sub_zero_uppercut_loop_end(Battle *battle, Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment jab_segments[] = {
  {0, 0, 1, 1},
  {8, 7, 0, 1},
};
static const MoveSegment jab_recover_segments[] = {
  {0, 9, 2, 1},
  {3, 17, 0, 1},
  {5, 19, 2, 1},
};
static const MoveSegment jab_second_segments[] = {{0, 0, 1, 1}};
static const MoveSegment jab_stop_segments[] = {{0, 10, 1, 1}};
static const MoveSegment jab_finish_segments[] = {
  {0, 11, 1, 1},
  {8, 18, 0, 1},
};
static const MoveSegment sweep_segments[] = {
  {0, 0, 2, 1},
  {4, 10, 0, 1},
  {5, 10, 1, 1},
};
static const MoveSegment uppercut_segments[] = {
  {0, 0, 1, 1},
  {6, 5, 0, 1},
  {7, 6, 1, 1},
};
static const MoveSegment uppercut_loop_segments[] = {
  {0, 8, -1, 1},
  {3, 7, 1, 1},
};
static const MoveSegment uppercut_late_segments[] = {
  {0, 9, 0, 1},
  {2, 8, -1, 1},
  {5, 7, 1, 1},
};
static const MoveSegment uppercut_miss_end_segments[] = {{0, 8, 1, 1}};
static const MoveSegment uppercut_hit_end_segments[] = {{0, 9, 1, 1}};
static const MoveEvent jab_events[] = {{8, sub_zero_jab_contact}};
static const MoveEvent jab_strike_events[] = {{0, sub_zero_jab_contact}};
static const MoveEvent sweep_events[] = {{18, sub_zero_sweep_contact}, {19, sub_zero_sweep_contact}};
static const MoveEvent uppercut_events[] = {{7, sub_zero_uppercut_contact}, {8, sub_zero_uppercut_contact}, {9, sub_zero_uppercut_contact}};
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
#define EVENT_SEQUENCE(segments, events, count, end) {segments, COUNT(segments), events, COUNT(events), count, end, NULL}

static const MoveSequence jab_sequence = EVENT_SEQUENCE(jab_segments, jab_events, 9, sub_zero_jab_next);
static const MoveSequence jab_recover_sequence = SEQUENCE(jab_recover_segments, 7, sub_zero_ready);
static const MoveSequence jab_second_sequence = SEQUENCE(jab_second_segments, 10, sub_zero_jab_second_end);
static const MoveSequence jab_stop_sequence = EVENT_SEQUENCE(jab_stop_segments, jab_strike_events, 3, sub_zero_ready);
static const MoveSequence jab_finish_sequence = EVENT_SEQUENCE(jab_finish_segments, jab_strike_events, 9, sub_zero_jab_finish_end);
static const MoveSequence sweep_sequence = EVENT_SEQUENCE(sweep_segments, sweep_events, 41, sub_zero_ready);
static const MoveSequence uppercut_sequence = EVENT_SEQUENCE(uppercut_segments, uppercut_events, 10, sub_zero_uppercut_recover);
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, sub_zero_uppercut_loop_end);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, sub_zero_uppercut_loop_end);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, sub_zero_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, sub_zero_ready);

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .sequence = &jab_sequence, .action = 0x10b};
static const MoveDef s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .sequence = &kick_sequence, .action = 0x100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .sequence = &sweep_sequence, .action = 0x10c};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .sequence = &uppercut_sequence, .action = 0x109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */
static const MoveDef s_jab_recover = {.clip = 294, .flags = RETAIL, .sequence = &jab_recover_sequence, .action = 0x10b};
static const MoveDef s_jab_second = {.clip = 295, .flags = RETAIL, .sequence = &jab_second_sequence, .action = 0x10b};
static const MoveDef s_jab_stop = {.clip = 295, .flags = RETAIL, .sequence = &jab_stop_sequence, .action = 0x10b};
static const MoveDef s_jab_finish = {.clip = 295, .flags = RETAIL, .sequence = &jab_finish_sequence, .action = 0x10b};
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_loop_sequence, .action = 0x109};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_late_sequence, .action = 0x109};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_miss_end_sequence, .action = 0x109};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_hit_end_sequence, .action = 0x109};

static const MoveDef s_throw = {.clip = 75, .last = 64, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = 0x500};
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

static bool sub_zero_repeat_jab(const Battle *battle, const Fighter *fighter) {
  const Fighter *other = fighter_opponent(battle, fighter);
  q16 dx = other->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  return (fighter->buffered & BUTTON_BIT(BUTTON_LOW_PUNCH)) != 0 && (fighter->facing > 0 ? dx >= 0 : dx <= 0);
}

static void sub_zero_jab_next(Battle *battle, Fighter *fighter) {
  const MoveDef *next = sub_zero_repeat_jab(battle, fighter) ? &s_jab_second : &s_jab_recover;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void sub_zero_jab_second_end(Battle *battle, Fighter *fighter) {
  const MoveDef *next = sub_zero_repeat_jab(battle, fighter) ? &s_jab_finish : &s_jab_stop;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void sub_zero_jab_finish_end(Battle *battle, Fighter *fighter) {
  /* Native 0x46ddd0 queries contact before 0x46de60 consumes the next press. */
  sub_zero_jab_contact(battle, fighter);
  sub_zero_jab_next(battle, fighter);
}

static void sub_zero_uppercut_recover(Battle *battle, Fighter *fighter) {
  bool late = fighter->contact_tick == 9;
  ContactResult result = fighter->contact;
  fighter->move_loops = result == CONTACT_MISS ? 3 : result == CONTACT_BLOCK ? 2 : 8;
  fighter_start_move(battle, fighter, S_UPPERCUT, late ? &s_uppercut_late : &s_uppercut_loop);
  /* Only the no-contact path holds sample 8 before final forward recovery. */
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

static void sub_zero_uppercut_loop_end(Battle *battle, Fighter *fighter) {
  ContactResult result = fighter->contact;
  const MoveDef *next;
  fighter->move_loops--;
  if (fighter->move_loops > 0) {
    next = &s_uppercut_loop;
  } else {
    next = result == CONTACT_MISS ? &s_uppercut_miss_end : &s_uppercut_hit_end;
  }
  fighter_start_move(battle, fighter, S_UPPERCUT, next);
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

static void sub_zero_jab(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
}

static void sub_zero_kick(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KICK, &s_kick);
}

static void armed_idle(Battle *battle, Fighter *fighter);
static void armed_walk(Battle *battle, Fighter *fighter);
static void armed_crouch(Battle *battle, Fighter *fighter);
static void armed_block(Battle *battle, Fighter *fighter);

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = armed_idle,
    [S_WALK] = armed_walk,
    [S_CROUCH] = armed_crouch,
    [S_BLOCK] = armed_block,
    [S_PUNCH] = sub_zero_jab,
    [S_KICK] = sub_zero_kick,
};

static Fighter *opponent(Battle *battle, Fighter *fighter) {
  return &battle->fighters[fighter == &battle->fighters[0] ? 1 : 0];
}

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static const MoveDef s_land = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};

static void landing_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    sub_zero_ready(battle, fighter);
  }
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
  {{.clip = 70, .last = MOVE_END, .flags = RETAIL, .action = 0x102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = 0x107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = 0x106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 52, .last = MOVE_END, .flags = RETAIL, .action = 0x104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = 0x108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 532, .last = MOVE_END, .flags = RETAIL, .action = 0x108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 11, 29, 32},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = 0x103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = 0x10d}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = 0x10a};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = 0x10a};

static void open_followup(Fighter *fighter, const ButtonTable *buttons) {
  fighter->buttons = buttons;
  fighter->flags = (fighter->flags | FF_FOLLOW_UP) & ~(unsigned)FF_BUSY;
  fighter->action = 0x1006;
}

static void ground_attack_tick(Battle *battle, Fighter *fighter) {
  int index = fighter->script_phase;
  const GroundAttack *attack = &ground_attacks[index];
  int t = fighter->ticks, fast = index == G_ROUNDHOUSE && fighter->script_timer ? 5 : 0;
  int active = attack->active - fast;
  fighter->avoids_high = index == G_LOW_KICK && t >= 6 && t < 11;
  bool gate = (index == G_CROUCH_LP || index == G_CROUCH_LK) && victim_action(battle, fighter) == 0x2011;
  if (index == G_CLOSE_HP) {
    int action = victim_action(battle, fighter);
    gate = action == 0x2001 || action == 0x106 || action == 0x107;
  }
  if (!gate && !fighter->connected && t >= active && t <= active + (index == G_ROUNDHOUSE ? 2 : 0)) {
    if (battle_contact(battle, fighter, &attack->record) != CONTACT_MISS) {
      if (index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CLOSE_HK) {
        open_followup(fighter, index == G_CLOSE_HP ? &close_hp_followup : index == G_COMBO_HP ? &combo_hp_followup : &close_hk_followup);
      }
    }
  }
  if (t >= active) {
    fighter->action = 0x1006;
  }
  int end = (fighter->connected ? attack->end_contact : attack->end_miss) - fast;
  switch (index) {
    case G_LOW_KICK: {
      int hold = fighter->connected ? 4 : 2;
      fighter->sample_override = t <= 11 ? t : t < 11 + hold ? 11 : mini(27, t - hold);
      break;
    }
    case G_CROUCH_LP:
    case G_CROUCH_LK: {
      int reverse = active + (fighter->connected ? 4 : 1);
      fighter->sample_override = t < active ? mini(9, t + (index == G_CROUCH_LP ? 2 : 1)) : maxi(0, 9 - maxi(0, t - reverse));
      break;
    }
    case G_CROUCH_HK: {
      fighter->sample_override = t < 11 ? t : ((t - 11) / 2) % 2 ? 9 : 10;
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
      if (t <= 10) {
        static const int opening[] = {0, 1, 2, 3, 4, 6, 8, 9, 10, 11, 11};
        fighter->sample_override = opening[t];
      } else {
        fighter->sample_override = mini(28, t - (fighter->connected ? mini(3, t - active) : 0));
      }
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
      fighter->sample_override = t <= 6 ? t * 2 : t < active ? mini(21, 12 + (t - 6) * (fast ? 2 : 1)) : mini(38, 22 + t - active);
      if (fighter->contact == CONTACT_BLOCK) {
        end = attack->end_miss - fast;
      }
      break;
  }
  if (t >= end) {
    sub_zero_ready(battle, fighter);
  }
}

static void ground_attack(Battle *battle, Fighter *fighter, int index) {
  bool followup = fighter_fast_start(battle, fighter);
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(battle, fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script = ground_attack_tick;
  fighter->script_phase = index;
  fighter->script_timer = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
}

static void hp_tick(Battle *battle, Fighter *fighter) {
  int phase = fighter->script_phase, t = fighter->ticks;
  if (phase >= 4) {
    fighter->sample_override = mini(phase == 4 ? 20 : 18, 11 + t);
    if (t >= (phase == 4 ? 10 : 8)) {
      sub_zero_ready(battle, fighter);
    }
    return;
  }
  int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
  int active = phase == 0 ? 8 : phase == 1 ? 7 : phase == 2 ? 6 : 6;
  fighter->sample_override = phase == 2 && t == active + 2 ? 17 : first + t;
  const Fighter *victim = fighter_opponent(battle, fighter);
  bool ducking_kick = victim->action == 0x102 && victim->avoids_high;
  if (t >= active && t <= active + 2 && !fighter->connected && !ducking_kick) {
    battle_contact(battle, fighter, &hp_record);
  }
  if (t >= active + 3) {
    bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
    int next = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
    fighter_start_move(battle, fighter, S_PUNCH, next == 4 ? &s_hp : &s_hp_second);
    fighter->script = hp_tick;
    fighter->script_phase = next;
    fighter->sample_override = next == 1 ? 1 : next == 3 ? 2 : 11;
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

static void sub_zero_hp(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(battle, fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(battle, fighter, S_PUNCH, &s_hp);
  fighter->script = hp_tick;
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
}

static void sub_zero_lk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_LOW_KICK);
}

static void sub_zero_crouch_lp(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_LP);
}

static void sub_zero_crouch_lk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_LK);
}

static void sub_zero_crouch_hk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_HK);
}

static void sub_zero_combo_hp(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_COMBO_HP);
}

static void sub_zero_roundhouse(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_ROUNDHOUSE);
}

static void sub_zero_hk(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if (fighter_holds_back(battle, fighter)) {
    sub_zero_roundhouse(battle, fighter);
  } else if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != 0x4001 && victim->action != 0x4005) {
    ground_attack(battle, fighter, G_CLOSE_HK);
  } else {
    sub_zero_kick(battle, fighter);
  }
}

static bool can_ground_throw(const Battle *battle, const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int a = victim->action;
  float distance = fighter_distance(battle, fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != 0x10a && a != 0x10b && a != 0x100 && a != 0x102 && a != 0x10e && !(a >= 0x111 && a <= 0x113);
}

static void press_jab(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(battle, fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sub_zero_hp);
}

static void press_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? sub_zero_roundhouse : sub_zero_hk);
}

static void enter_uppercut(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_UPPERCUT);
}

static void enter_sweep(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_SWEEP);
}

static void enter_throw(Battle *battle, Fighter *fighter);
static void enter_lk_throw(Battle *battle, Fighter *fighter);

static void press_lp(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, can_ground_throw(battle, fighter, false) ? enter_throw
                             : fighter->armed                         ? (fighter_holds_back(battle, fighter) ? fighter->weapon->back_lp : fighter->weapon->lp)
                                                                      : sub_zero_jab);
}

static void press_lk(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? enter_sweep : can_ground_throw(battle, fighter, true) ? enter_lk_throw : sub_zero_lk);
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
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
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

static void air_land(Battle *battle, Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  fighter_start_move(battle, fighter, S_JUMP, &s_land);
  fighter->script = landing_tick;
  fighter->sample_override = 18;
}

static void air_attack_tick(Battle *battle, Fighter *fighter) {
  int index = 0;
  while (&air_attacks[index].move != fighter_move(fighter)) {
    ++index;
  }
  const AirAttack *attack = &air_attacks[index];
  int tick = fighter->ticks;
  if (fighter->script_phase == AIR_FREEZE || fighter->script_phase == AIR_COMBO_FREEZE) {
    if (--fighter->script_timer > 0) {
      return;
    }
    fighter->buttons = NULL;
    fighter->body.gravity = index >= 3 || fighter->script_phase == AIR_COMBO_FREEZE ? 0x51e : 0x28f;
    fighter->script_phase = AIR_FALL;
  }
  if (!fighter_airborne(fighter)) {
    air_land(battle, fighter);
    return;
  }
  if (fighter->script_phase == AIR_FALL) {
    return;
  }
  int retract_at = attack->extend + attack->hold;
  fighter->sample_override = tick < attack->extend ? tick : tick < retract_at ? attack->extend - 1 : maxi(0, attack->retract - 1 - tick + retract_at);
  if (tick >= attack->extend && tick < retract_at && !fighter->connected && battle_contact(battle, fighter, &attack->record) != CONTACT_MISS) {
    bool low = fighter->body.ground - fighter->body.pos[AXIS_Y] <= 0xcccc;
    bool combo = fighter->script_phase == AIR_COMBO;
    stop_body(fighter);
    fighter->action = 0x1006;
    fighter->script_phase = combo ? AIR_COMBO_FREEZE : AIR_FREEZE;
    fighter->script_timer = index >= 3 ? 8 : 12;
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
  } else if (tick >= retract_at + attack->retract) {
    if (fighter->script_phase == AIR_COMBO) {
      fighter->body.gravity = 0x51e;
    }
    fighter->script_phase = AIR_FALL;
  }
}

static void start_air_attack(Battle *battle, Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(battle, fighter, S_JUMP, &air_attacks[index].move);
  fighter->script = air_attack_tick;
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(battle, fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
}

static void air_hp(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 0);
}

static void air_lp(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 1);
}

static void air_kick(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 2);
}

static void air_hk_diagonal(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 3);
}

static void air_lk_diagonal(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 4);
}

static void press_air_hp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : air_hp);
}

static void press_air_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

static void press_air_hk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

static void press_air_lk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, air_kick);
}

static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static void press_crouch_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : sub_zero_crouch_lp);
}

static void press_uppercut(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, sub_zero_crouch_hk);
}

static void press_crouch_lk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, sub_zero_crouch_lk);
}

static void press_combo_hp(Battle *battle, Fighter *fighter) {
  (void)battle;
  if (fighter->armed) {
    press_jab(battle, fighter);
  } else {
    fighter_interrupt(fighter, sub_zero_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Battle *battle, Fighter *fighter) {
  if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(fighter_opponent(battle, fighter))) {
    ground_attack(battle, fighter, G_CLOSE_HP);
  } else {
    sub_zero_hp(battle, fighter);
  }
}

static void press_air_ground_hp(Battle *battle, Fighter *fighter) {
  (void)battle;
  if (fighter->armed) {
    press_jab(battle, fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

static const ButtonTable air_ground_followup = {
    {[BUTTON_HIGH_PUNCH] = press_air_ground_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};

static void press_combo_hk(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? sub_zero_roundhouse : sub_zero_kick);
}

static void press_knee_hp(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if ((victim->action == 0x4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, enter_uppercut);
  } else {
    press_jab(battle, fighter);
  }
}

static const ButtonTable close_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_combo_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable close_hk_followup = {
    {[BUTTON_HIGH_PUNCH] = press_knee_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_combo_hk, [BUTTON_LOW_KICK] = press_lk}};

/* Paired ground throws: 0x4de6e4 (LP), 0x4de2f8/318/328 (LK).
 * Native opponent-root alignment is represented by a shared world origin. */
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = 0x501};
static const MoveDef s_lp_thrown = {.clip = 152, .last = 41, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_lk_thrown_end = {.clip = 586, .first = 65, .last = 130, .flags = SF_LOCK, .action = 0x4003};

static void ground_thrown_tick(Battle *battle, Fighter *victim) {
  Fighter *owner = opponent(battle, victim);
  if (victim->current_move == &s_lk_thrown_end) {
    victim->sample_override = mini(130, 65 + victim->ticks);
    if (victim->ticks >= 66 && victim->health > 0) {
      sub_zero_ready(battle, victim);
    }
    return;
  }
  if ((owner->current_move != &s_throw && owner->current_move != &s_lk_throw) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->health > 0) {
      sub_zero_ready(battle, victim);
    } else {
      victim->script = NULL;
    }
    return;
  }
  victim->sample_override = mini(fighter_move(victim)->last, owner->ticks);
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
}

static void ground_throw_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  bool lk = fighter->current_move == &s_lk_throw;
  int tick = fighter->ticks, release = lk ? 65 : 42;
  if (tick < release && (victim->script != ground_thrown_tick || victim->pending != NULL)) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    sub_zero_ready(battle, fighter);
    return;
  }
  fighter->sample_override = mini(lk ? 65 : 64, tick);
  if (tick == 42) {
    if (victim->armed && victim->disarm != NULL) {
      victim->disarm(battle, victim);
    }
    battle_damage(battle, victim, lk ? 0x2e14 : 0x2666);
  }
  if (tick == release) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (lk) {
      fighter_start_move(battle, victim, S_KNOCKDOWN, &s_lk_thrown_end);
      victim->script = ground_thrown_tick;
      victim->sample_override = 65;
    } else {
      battle_react(battle, victim, 100);
    }
  }
  if (tick >= (lk ? 66 : 65)) {
    sub_zero_ready(battle, fighter);
  }
}

static void ground_throw(Battle *battle, Fighter *fighter, bool lk) {
  if (!can_ground_throw(battle, fighter, lk)) {
    sub_zero_ready(battle, fighter);
    return;
  }
  Fighter *victim = opponent(battle, fighter);
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(battle, fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(battle, victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->script = ground_throw_tick;
  victim->script = ground_thrown_tick;
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
}

static void enter_throw(Battle *battle, Fighter *fighter) {
  ground_throw(battle, fighter, false);
}

static void enter_lk_throw(Battle *battle, Fighter *fighter) {
  ground_throw(battle, fighter, true);
}

/* Freeze: 49bc60 -> 4ed588/4ed818 -> 49bcb0. Flight is an independent
 * task, so it survives owner interruption; bounds and contact own teardown. */
static const AttackRecord ice_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_ice = {.clip = 16, .last = 35, .flags = SF_LOCK | SF_RECORD, .action = 0x304};

static bool owned_object(const Battle *battle, const Fighter *fighter, int kind) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &battle->projectiles[i];
    if (p->kind == kind && &battle->fighters[p->owner] == fighter) {
      return true;
    }
  }
  return false;
}

static void ice_projectile(Battle *battle, Projectile *p) {
  if (battle->contacts_off) {
    p->kind = 0;
    return;
  }
  for (int i = 0; i < 3; ++i) {
    p->pos[i] += p->vel[i];
  }
  Fighter *owner = &battle->fighters[p->owner];
  if (opponent(battle, owner)->action != 0x2001 && battle_projectile_contact(battle, p, &ice_record) != CONTACT_MISS) {
    p->kind = 0;
  }
  int64_t radius2 = (int64_t)battle->arena_radius * battle->arena_radius + (int64_t)0xc0000 * Q16_ONE;
  if ((int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] > radius2) {
    p->kind = 0;
  }
}

static void ice_tick(Battle *battle, Fighter *fighter) {
  int startup = fighter->script_phase ? 8 : 15;
  int t = fighter->ticks;
  if (t < startup) {
    fighter->sample_override = mini(14, t * (fighter->script_phase ? 2 : 1));
    return;
  }
  if (t == startup) {
    battle_sound(battle, SOUND_C, 0);
    Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_ICE, 180, ice_projectile);
    if (p != NULL) {
      for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
        q16 heading = fighter_axis(battle, fighter, axis);
        p->pos[axis] += q16_mul(heading, 0xccc);
        p->vel[axis] = q16_mul(heading, 0x3333);
      }
      p->pos[AXIS_Y] -= 0x28f;
    }
  }
  int phase = t - startup - 1;
  fighter->action = 0x1007;
  fighter->sample_override = phase < 0    ? 14
                             : phase < 8  ? 15 + phase
                             : phase < 13 ? 23 - (phase - 8)
                             : phase < 18 ? 18 + phase - 13
                             : phase < 23 ? 23 - (phase - 18)
                                          : mini(35, 18 + phase - 23);
  if (phase >= 41) {
    sub_zero_ready(battle, fighter);
  }
}

static bool can_ice(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && fighter_opponent(battle, fighter)->hits <= 1 && !owned_object(battle, fighter, PROJECTILE_CLONE);
}

static void sub_zero_ice(Battle *battle, Fighter *fighter) {
  bool followup = fighter_fast_start(battle, fighter);
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_PUNCH, &s_ice);
  fighter->script = ice_tick;
  fighter->script_phase = followup;
  fighter->sample_override = 0;
}

/* 47f4d0 copies the current pose before the retreat. Both ice predicates
 * inspect clone task types 45/46, not the projectile task type 42. */
static const AttackRecord clone_record = {0x4ccc, -0xa3d, 0xbae1, 0x9e76, 67, 67, 0, ATTACK_MARK, 0};
static const MoveDef s_clone = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x405};
static const MoveDef s_clone_near = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x406};

static void clone_object(Battle *battle, Projectile *p) {
  Fighter *owner = &battle->fighters[p->owner];
  if (battle->contacts_off || battle_contact_from(battle, owner, &clone_record, p->pos, p->facing) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void clone_tick(Battle *battle, Fighter *fighter) {
  if (fighter->script_phase == 0) {
    fighter->sample_override = mini(8, fighter->ticks);
    if (fighter->ticks == 3) {
      fighter->body.root_y += 0x3333;
      fighter->body.pos[AXIS_Y] -= 0x3333;
      fighter->body.spin[AXIS_X] = -0x6147;
    }
    if (fighter->ticks >= 9 && fighter->body.vel[AXIS_Y] >= 0) {
      /* 47f880 stops angular motion, restores facing, then 429460
       * plays the clip backwards while 48f390 waits for landing. */
      memset(fighter->body.angle, 0, sizeof fighter->body.angle);
      memset(fighter->body.spin, 0, sizeof fighter->body.spin);
      fighter->script_phase = 1;
      fighter->script_timer = 0;
    }
  } else {
    fighter->sample_override = maxi(0, 8 - ++fighter->script_timer);
    if (fighter->script_timer >= 8 && !fighter_airborne(fighter)) {
      fighter->body.root_y = 0;
      air_land(battle, fighter);
    }
  }
}

static bool can_clone(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && battle->ticks >= fighter->clone_ready && fighter_opponent(battle, fighter)->action != 0x1003 &&
         !owned_object(battle, fighter, PROJECTILE_CLONE);
}

static void sub_zero_clone(Battle *battle, Fighter *fighter) {
  bool near = fighter_distance(battle, fighter) < 1.f;
  if (!near) {
    Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_CLONE, fighter_opponent(battle, fighter)->human ? 74 : 53, clone_object);
    if (p != NULL) {
      p->clip = fighter_move(fighter)->clip;
      p->sample = fighter_pose_sample(fighter);
    }
  }
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_JUMP, near ? &s_clone_near : &s_clone);
  fighter->script = clone_tick;
  fighter->sample_override = 0;
  fighter_set_speed(battle, fighter, -0x170a);
  fighter->body.vel[AXIS_Y] = -0x28f5;
  fighter->body.gravity = 0x28f;
  fighter->clone_ready = battle->ticks + 180;
  battle_sound(battle, SOUND_C, 0xa);
}

/* Slide: 47dc00/4e84d8 -> 47dc40; hit 4e84f8, block 4e8530.
 * Only an unblocked contact stamps cooldown slot 2 (47de8e). */
static const AttackRecord slide_record = {0xf0a3, 0x947a, 0x111eb, 0x8f5c, 122, 122, 165, 0, 0x147a};
static const MoveDef s_slide = {.clip = 95, .last = 10, .flags = SF_LOCK | SF_RECORD, .action = 0x204};

static void slide_tick(Battle *battle, Fighter *fighter) {
  if (fighter->script_phase == 0) {
    fighter->sample_override = mini(9, fighter->ticks);
    if (fighter->ticks >= 9 && fighter->ticks <= 25) {
      ContactResult contact = battle_contact(battle, fighter, &slide_record);
      if (contact != CONTACT_MISS) {
        fighter->script_phase = 1;
        fighter->script_timer = 0;
        if (contact == CONTACT_BLOCK) {
          stop_body(fighter);
          fighter->flags |= FF_NO_GUARD;
        } else {
          fighter->slide_ready = battle->ticks + 64;
        }
      }
    }
    if (fighter->script_phase == 0 && fighter->ticks >= 25) {
      stop_body(fighter);
      fighter->script_phase = 2;
      fighter->script_timer = 0;
    }
  }
  if (fighter->script_phase == 1) {
    static const int samples[] = {10, 9, 8, 9};
    int hold = fighter->contact == CONTACT_BLOCK ? 24 : 16;
    fighter->sample_override = samples[fighter->script_timer % 4];
    if (++fighter->script_timer >= hold) {
      stop_body(fighter);
      fighter->script_phase = 2;
      fighter->script_timer = 0;
    }
  } else if (fighter->script_phase == 2) {
    fighter->sample_override = maxi(0, 10 - fighter->script_timer++);
    if (fighter->script_timer > 11) {
      fighter->flags &= ~(unsigned)FF_NO_GUARD;
      sub_zero_ready(battle, fighter);
    }
  }
}

static bool can_slide(const Battle *battle, const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return battle->phase == PHASE_FIGHT && battle->ticks >= fighter->slide_ready && (fighter->held & mask) == chord;
}

static void sub_zero_slide(Battle *battle, Fighter *fighter) {
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_KICK, &s_slide);
  fighter->script = slide_tick;
  fighter->sample_override = 0;
  fighter_set_speed(battle, fighter, 0x2666);
  battle_sound(battle, SOUND_C, 0xf);
}

/* Shared weapon draw 4e62c0/4e62d8, air 4e66d0/4e66f0 and
 * throw 498290. Scepter ground strikes use character-4 descriptor 4ecdb8. */
/* Scepter descriptor 4ecdb8. Parameter blocks 4ec458/490/480/4b8
 * select the shared native attack runner and its recovery continuations. */
// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike scepter_attacks[] = {
  {{.clip = 231, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {128385, -35323, 92274, 22872, 51, 51, 119, 0, 11796}, 6, 10, 16, 4, 1, 0, 0, 0, 0x20},
  {{.clip = 250, .last = 31, .flags = SF_LOCK | SF_RECORD, .action = 0x111}, {115277, 20316, 73400, 51052, 150, 150, 118, ATTACK_LOW, 9830}, 0, 4, 12, 2, 9, 0, 0, 0, 0x22},
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x112}, {99614, -49152, 64880, 55050, 52, 52, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 8, 0x147a, 0x22},
  {{.clip = 228, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x113}, {123076, -26804, 91684, 14352, 151, 151, 118, 0, 0}, 0, 10, 16, 5, 5, 1, 14, 0x2666, 0x22},
};
// clang-format on

static void armed_idle(Battle *battle, Fighter *fighter) {
  weapon_stance(battle, fighter, S_IDLE);
}

static void armed_walk(Battle *battle, Fighter *fighter) {
  weapon_stance(battle, fighter, S_WALK);
}

static void armed_crouch(Battle *battle, Fighter *fighter) {
  weapon_stance(battle, fighter, S_CROUCH);
}

static void armed_block(Battle *battle, Fighter *fighter) {
  weapon_stance(battle, fighter, S_BLOCK);
}

static const WeaponDef scepter;

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Battle *battle, Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(battle, fighter, index == 0 || index == 2);
  } else {
    weapon_start_strike(battle, fighter, &scepter_attacks[index], true);
  }
}

static void scepter_hp(Battle *battle, Fighter *fighter) {
  scepter_attack(battle, fighter, 0);
}

static void scepter_lp(Battle *battle, Fighter *fighter) {
  scepter_attack(battle, fighter, 1);
}

static void scepter_back_hp(Battle *battle, Fighter *fighter) {
  scepter_attack(battle, fighter, 2);
}

static void scepter_back_lp(Battle *battle, Fighter *fighter) {
  scepter_attack(battle, fighter, 3);
}

static const WeaponDef scepter = {0x6b, scepter_hp, scepter_lp, scepter_back_hp, scepter_back_lp, NULL, weapon_throw, equip_scepter};

static bool can_weapon(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void sub_zero_weapon(Battle *battle, Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(battle, fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_scepter(fighter);
    weapon_followup_victim(battle, fighter);
    weapon_start_strike(battle, fighter, &scepter_attacks[2], false);
    fighter->action = 0x10e;
  } else {
    weapon_draw(battle, fighter, &scepter);
  }
}

static bool can_pickup(const Battle *battle, const Fighter *fighter) {
  return weapon_can_pickup(battle, fighter);
}

static void sub_zero_pickup(Battle *battle, Fighter *fighter) {
  weapon_pickup(battle, fighter);
}

/* 45e820: RUN latch is written after matching, so this sees the prior
 * press. 4692e0 uses shared descriptor 53ce18 (clip 300), not a roster
 * table. Down plays it forwards; otherwise it plays backwards. */
static const MoveDef s_sidestep = {.clip = 300, .last = 17, .flags = SF_LOCK};

static bool can_sidestep(const Battle *battle, const Fighter *fighter) {
  uint64_t previous = fighter->latch[BUTTON_RUN];
  return battle->phase == PHASE_FIGHT && !fighter_airborne(fighter) && previous != 0 && battle->ticks - previous <= 8 &&
         battle->ticks >= fighter->sidestep_ready;
}

static void sidestep_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->script_phase ? mini(17, fighter->ticks) : maxi(0, 17 - fighter->ticks);
  if (fighter->ticks >= 18) {
    stop_body(fighter);
    sub_zero_ready(battle, fighter);
  }
}

static void sub_zero_sidestep(Battle *battle, Fighter *fighter) {
  bool down = (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0;
  q16 along = 0x443, across = down ? -0xccc : 0xccc;
  if (fighter_airborne(opponent(battle, fighter))) {
    along = q16_mul(along, 0x4ccc);
    across = q16_mul(across, 0x4ccc);
  }
  q16 x = fighter_axis(battle, fighter, AXIS_X), z = fighter_axis(battle, fighter, AXIS_Z);
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_WALK, &s_sidestep);
  fighter->script = sidestep_tick;
  fighter->script_phase = down;
  fighter->sample_override = down ? 0 : 17;
  fighter->body.vel[AXIS_X] = q16_mul(x, along) - q16_mul(z, across);
  fighter->body.vel[AXIS_Z] = q16_mul(z, along) + q16_mul(x, across);
  fighter->sidestep_ready = battle->ticks + 80;
}

/* Finishers: character entries 0x467cd0/ce0/e00/e70. Presentation runs
 * through bounded C phase tables; no retail task/camera/object scheduler. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = 0x700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(385, 140), FATAL_MOVE(387, 73), FATAL_MOVE(552, 104),
  FATAL_MOVE(95, 10), FATAL_MOVE(258, 38), FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
  FATAL_MOVE(115, 18), FATAL_MOVE(116, 49), FATAL_MOVE(117, 18), FATAL_MOVE(118, 49),
  FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48),
};
// clang-format on
#undef FATAL_MOVE

typedef struct FatalPhase {
  int clip, first, last, repeats;
} FatalPhase;

// clang-format off: Keep table entries on one row, including long rows.
/* Native scene continuations use independent actor clocks. */
#define SCENE(c, a, b, r) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, a, b, r, 0, 0}
#define BLEND(c, sample, step, first) {{.clip = c, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x700}, sample, sample, 1, step, first}
static const SceneClip first_actor[] = {
  BLEND(385, 0, 0x32f1, 0x3333),
  SCENE(385, 0, 17, 1), SCENE(385, 18, 42, 1), SCENE(385, 41, 31, 1),
  SCENE(385, 32, 42, 1), SCENE(385, 41, 31, 1), SCENE(385, 32, 44, 1),
  SCENE(385, 45, 60, 1), SCENE(385, 61, 140, 1), SCENE(385, 96, 140, 1),
};
/* 475190/475340; 445510 blends to 46, 49 and 50 instead of
 * advancing through those samples at one sample per tick. */
static const SceneClip first_target[] = {
  BLEND(389, 0, 0x32f1, 0x3333), BLEND(387, 0, 0x32f1, 0x3333),
  SCENE(387, 0, 15, 1), SCENE(387, 14, 4, 1), SCENE(387, 5, 15, 1),
  SCENE(387, 14, 4, 1), SCENE(387, 5, 17, 1), SCENE(387, 18, 48, 1),
  BLEND(387, 46, 0x666, 0x666), BLEND(387, 49, 0x43f, 0x43f),
  BLEND(387, 50, 0x43f, 0x886), SCENE(387, 50, 63, 1), SCENE(387, 64, 73, 1),
};
static const SceneClip breath_actor[] = {
  SCENE(552, 0, 9, 1), SCENE(552, 10, 29, 3), SCENE(552, 30, 83, 1),
  SCENE(552, 83, 93, 5), SCENE(552, 94, 104, 1), SCENE(552, 104, 104, 20),
};
static const SceneClip breath_target[] = {SCENE(206, 0, 99, 1)};
static const SceneClip slide_actor[] = {SCENE(95, 0, 8, 1)};
static const SceneClip uppercut_actor[] = {SCENE(258, 25, 38, 1), SCENE(293, 0, 5, 1)};
static const SceneClip victory_actor[] = {SCENE(115, 0, 18, 1), SCENE(116, 0, 49, 1), SCENE(117, 0, 18, 1), SCENE(118, 0, 49, 1)};
#undef SCENE
#undef BLEND
/* 0x43d3d0 sets 31 repetitions; 0x4e0118..1a8 owns the paired poses. */
static const FatalPhase fan_owner[] = {{415, 0, 68, 1}, {415, 69, 77, 31}, {415, 78, 129, 1}, {412, 0, 48, 1}};
static const FatalPhase fan_victim[] = {{416, 0, 68, 1}, {416, 69, 77, 31}, {416, 78, 107, 1}};
// clang-format on

static void fatality_tick(Battle *battle, Fighter *fighter);

static void fatal_pose(Battle *battle, Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        int scene_timer = fighter->script_timer;
        fighter_start_move(battle, fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        fighter->script = owner ? fatality_tick : NULL;
        fighter->script_timer = scene_timer;
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool fatal_phases(Battle *battle, Fighter *fighter, int tick, const FatalPhase *phases, int count, bool owner) {
  for (int i = 0; i < count; ++i) {
    const FatalPhase *p = &phases[i];
    int length = abs(p->last - p->first) + 1;
    if (tick < length * p->repeats || i == count - 1) {
      bool done = tick >= length * p->repeats;
      int sample = done ? p->last : p->first + (p->last >= p->first ? 1 : -1) * (tick % length);
      fatal_pose(battle, fighter, p->clip, sample, owner);
      return done;
    }
    tick -= length * p->repeats;
  }
  return true;
}

static bool can_second_fatality(const Battle *battle, const Fighter *fighter) {
  float distance = fighter_distance(battle, fighter);
  return battle_can_finish(battle, fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_close_fatality(const Battle *battle, const Fighter *fighter) {
  return battle_can_finish(battle, fighter) && fighter_distance(battle, fighter) <= 1.f;
}

/* 45e460 -> mask 4e4178: HP + BLOCK + RUN; no Up/HK/LP/LK. */
static bool can_first_fatality(const Battle *battle, const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  return can_close_fatality(battle, fighter) && (fighter->held & mask) == chord;
}

static bool can_fan_fatality(const Battle *battle, const Fighter *fighter) {
  return battle->arena == 8 && can_close_fatality(battle, fighter);
}

static bool can_spike_fatality(const Battle *battle, const Fighter *fighter) {
  return battle->arena == 0 && can_close_fatality(battle, fighter);
}

static void first_fatality_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  if (fighter->script_phase == 0) {
    if (battle->phase_ticks == 8) {
      /* 445920's first local camera control points (4e0f00). */
      battle_scene_camera(battle, fighter, victim, 0x20000, -0x30000, -0x20000, 0);
    }
    if (battle->phase_ticks < 58) {
      return;
    }
    scene_begin(fighter, first_actor, COUNT(first_actor), 9);
    fighter->script_phase = 1;
    return;
  }
  int previous = fighter->scene.index;
  scene_step(fighter);
  if (previous != fighter->scene.index) {
    int phase = fighter->scene.index;
    if (phase == 1) {
      battle_sound(battle, SOUND_B, 0x30);
    } else if (phase == 3) {
      battle_sound(battle, SOUND_B, 0x1b);
      battle_sound(battle, SOUND_B, 0x29);
    } else if (phase == 4 || phase == 5 || phase == 6) {
      battle_sound(battle, SOUND_B, phase == 4 ? 0x2b : phase == 5 ? 0x2a : 0x1b);
    } else if (phase == 7) {
      battle_sound(battle, SOUND_B, 0x1c);
      battle_sound(battle, SOUND_B, 0);
    }
  }
  if (previous != fighter->scene.index && fighter->scene.index == 2) {
    /* The second camera path 4e0f58 opens at (-5,-7,-2.5). */
    battle_scene_camera(battle, fighter, victim, -0x50000, -0x70000, -0x28000, 30);
  }
  if (victim->scene.clips == NULL && fighter->scene.index >= 2) {
    scene_begin(victim, first_target, COUNT(first_target), -1);
  } else if (victim->scene.clips != NULL) {
    int before = victim->scene.index;
    scene_step(victim);
    if (before != victim->scene.index && victim->scene.index == 8) {
      battle_attach_head(battle, victim, fighter, 10);
      battle_particles(battle, victim, 2, 0xffa81420, 20, false);
    }
    if (before != victim->scene.index && victim->scene.index == 12) {
      battle_sound(battle, SOUND_B, 0x18);
      battle_particles(battle, victim, 5, 0xffa81420, 20, false);
    }
  }
  if (victim->scene.done && fighter->scene.index >= 9) {
    battle_end_finisher(battle);
  }
}

static void second_fatality_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  if (victim->scene.clips != NULL) {
    scene_step(victim);
    if (victim->sample_override == 20 || victim->sample_override == 90) {
      battle_sound(battle, SOUND_A, 0x13);
    }
  }
  switch (fighter->script_phase) {
    case 0: {
      int before = fighter->scene.index;
      if (fighter->scene.clips == NULL) {
        scene_begin(fighter, breath_actor, COUNT(breath_actor), -1);
        scene_begin(victim, breath_target, COUNT(breath_target), -1);
      } else if (scene_step(fighter)) {
        scene_begin(fighter, slide_actor, COUNT(slide_actor), -1);
        fighter_set_speed(battle, fighter, 0x2666);
        fighter->script_phase = 1;
        battle_sound(battle, SOUND_C, 0xf);
      }
      if (before != fighter->scene.index && fighter->script_phase == 0 && (fighter->scene.index == 1 || fighter->scene.index == 2)) {
        battle_sound(battle, SOUND_C, fighter->scene.index == 1 ? 0x3f : 0x40);
      }
      if (fighter->scene.index >= 3) {
        victim->appearance = APPEARANCE_FROZEN;
      }
      if (fighter->ticks >= 10 && fighter->ticks < 102) {
        /* 41bbb0 alternates joints 9 and 10 throughout its 46 passes. */
        battle_particles(battle, fighter, 9 + (fighter->ticks & 1), 0xffa6eaff, 2, true);
      }
    } break;
    case 1:
      if (scene_step(fighter)) {
        /* 41b450 uses the current pair distance after opening eight
         * samples, then truncates the Q16 result to a tick count. */
        q16 distance = (q16)(fighter_distance(battle, fighter) * Q16_ONE);
        fighter->script_timer = maxi(0, q16_mul(distance - 0xcccc, 0x69999) / Q16_ONE);
        fighter->sample_override = 9;
        fighter->script_phase = 2;
      }
      break;
    case 2:
      if (fighter->script_timer-- <= 0) {
        fighter_set_speed(battle, fighter, 0);
        scene_begin(fighter, uppercut_actor, COUNT(uppercut_actor), -1);
        fighter->script_phase = 3;
      }
      break;
    case 3:
      if (scene_step(fighter)) {
        battle_particles(battle, victim, 3, 0xffa6eaff, 32, true);
        battle_shatter(battle, victim);
        fighter->move_loops = 0;
        fighter->script_phase = 4;
      }
      break;
    case 4:
      if (++fighter->sample_override >= 8) {
        fighter->script_phase = 5;
      }
      break;
    case 5:
      if (--fighter->sample_override <= 6) {
        fighter->script_phase = ++fighter->move_loops < 8 ? 4 : 6;
      }
      break;
    case 6:
      if (++fighter->sample_override >= 19) {
        fighter->script_timer = 60;
        fighter->script_phase = 7;
      }
      break;
    case 7:
      if (--fighter->script_timer <= 0) {
        scene_begin(fighter, victory_actor, COUNT(victory_actor), -1);
        fighter->script_phase = 8;
      }
      break;
    case 8:
      if (scene_step(fighter)) {
        battle_end_finisher(battle);
      }
      break;
  }
}

static void fatality_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  int tick = battle->phase_ticks;
  bool owner_done = false, victim_done = false;
  if (battle->finisher == FINISH_SUB_ZERO_1) {
    first_fatality_tick(battle, fighter);
    return;
  } else if (battle->finisher == FINISH_SUB_ZERO_2) {
    second_fatality_tick(battle, fighter);
    return;
  } else if (battle->finisher == FINISH_FAN) {
    owner_done = fatal_phases(battle, fighter, tick, fan_owner, COUNT(fan_owner), true);
    victim_done = fatal_phases(battle, victim, tick, fan_victim, COUNT(fan_victim), false);
    if (tick >= 348) {
      victim->body.pos[AXIS_Y] = victim->body.ground - mini(30, tick - 348) * 0x3333;
    }
    if (victim_done) {
      victim->appearance = APPEARANCE_HIDDEN;
    }
  } else if (battle->finisher == FINISH_SPIKE) {
    /* 30-tick setup + script wait 28, uppercut to sample 5, reaction 175.
     * Stop at Y=-5.6, then the victim script waits 128 + 50 + 60 ticks. */
    int attack = tick - 58;
    if (attack >= 0) {
      fatal_pose(battle, fighter, 293, mini(19, attack), true);
    }
    if (tick >= 64) {
      int rise = mini(22, tick - 64);
      fatal_pose(battle, victim, 128, mini(31, rise * 2), false);
      victim->body.pos[AXIS_Y] = rise == 22 ? -0x59999 : victim->body.ground - rise * 0x3333;
    }
    if (tick >= 324) {
      fatal_pose(battle, fighter, 115, 18, true);
      owner_done = victim_done = true;
    }
  }
  if (owner_done && victim_done) {
    battle_end_finisher(battle);
  }
}

static void start_fatality(Battle *battle, Fighter *fighter, int id) {
  if (!battle_begin_finisher(battle, fighter, id)) {
    sub_zero_ready(battle, fighter);
    return;
  }
  Fighter *victim = opponent(battle, fighter);
  fatal_pose(battle, victim, 13, 0, false);
  fatal_pose(battle, fighter, id == FINISH_SUB_ZERO_1 ? 385 : id == FINISH_SUB_ZERO_2 ? 552 : id == FINISH_FAN ? 415 : 293, 0, true);
  if (id == FINISH_SUB_ZERO_1) {
    /* 445100 -> 475530 aligns the scene pair at 1.45 units. */
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(battle, fighter, axis), 0x17333);
    }
  }
  fighter->script = fatality_tick;
}

static void sub_zero_first_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_SUB_ZERO_1);
}

static void sub_zero_second_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_SUB_ZERO_2);
}

static void sub_zero_fan_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_FAN);
}

static void sub_zero_spike_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_SPIKE);
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

const CharacterDef mk4_sub_zero = {GAME_MK4, 4, "SUB ZERO", &mk4_motion, moves, S_COUNT, &controls, enter, -77660};
