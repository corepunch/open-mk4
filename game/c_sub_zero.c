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

static const FighterFn enter[S_COUNT] = {
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
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
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
  fighter_interrupt(fighter, sub_zero_hp);
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
  fighter_interrupt(fighter, can_ground_throw(battle, fighter, false) ? enter_throw : sub_zero_jab);
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
  fighter_interrupt(fighter, air_hp);
}

static void press_air_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, air_lp);
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
  fighter_interrupt(fighter, sub_zero_crouch_lp);
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
  fighter_interrupt(fighter, sub_zero_combo_hp);
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
  fighter_interrupt(fighter, air_ground_hp);
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

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable crouching = {
    {[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable jumping = {
    {[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping}};

const CharacterDef mk4_sub_zero = {GAME_MK4, 4, "SUB ZERO", &mk4_motion, moves, S_COUNT, &controls, enter, -77660};
