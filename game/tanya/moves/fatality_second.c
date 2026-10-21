#include "retail_vm.h"
#include "../character.h"

/* HK D,F,D,F record 4e453c -> 467cd0 -> 43bd70 -> 4dfe90[9] = 464f30. */
static const MoveDef victim_pose = {.clip = 13, .last = 55, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
/* 4611c0 selects the roster stance 53d408[9] = clip 439. */
static const MoveDef owner_pose = {.clip = 439, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

/* 45e570: close fatality range (<= 1 unit) and the shared finish window. */
/* Retail 489fc0 plays a raw sample token. These tokens are the same words
 * as these B-bank entries; 0x281 (owner cue) and 0x1bd (one random entry)
 * have no bank selector and stay silent. */
static void token_sound(int token) {
  // clang-format off: Keep table entries on one row, including long rows.
  static const int tokens[][2] = {
    {0xd2, 0x1c}, {0x27b, 0x20}, {0xbe, 0x3e}, {0x14d7, 0x29}, {0x14dc, 0x2a}, {0x14e1, 0x2b}, {0x2f8, 0x18}, {0x1b8, 0x19}, {0xcd, 0x47},
  };
  // clang-format on
  for (size_t i = 0; i < sizeof tokens / sizeof tokens[0]; ++i) {
    if (tokens[i][0] == token) {
      battle_sound(SOUND_B, tokens[i][1]);
      return;
    }
  }
  if (token == 0x1c2) {
    battle_sound(SOUND_C, 0x37);
  }
}

/* Task-619 cue lists: {visits before the action, token or random list}. */
// clang-format off: Keep table entries on one row, including long rows.
static const int owner_cue[][2] = {{33, 0xd2}, {20, 0x281}};
static const int victim_cue[][2] = {{45, 0x4ea8b0}, {12, 0x4ea8b0}, {8, 0x14dc}, {20, 0x14e1}, {23, 0x14d7}, {13, 0xbe}, {15, 0x14dc}, {20, 0x14dc}, {33, 0x2f8}};
// clang-format on

static bool cue_frame(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)owner;
  (void)opponent;
  if (event->frame_value == 0x4ea8b0) {
    /* 48a2a0 -> 489e80: one of four list 4ea8b0 tokens. */
    static const int list[] = {0x1b8, 0x1bd, 0x1c2, 0xcd};
    token_sound(list[battle_random(4)]);
  } else {
    token_sound(event->frame_value);
  }
  return true;
}

/* Each task-619 state sleeps, plays its token and selects the next state. */
static bool cue_next(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)owner;
  (void)opponent;
  const int (*steps)[2] = event->data[0] ? victim_cue : owner_cue;
  int count = event->data[0] ? (int)(sizeof victim_cue / sizeof victim_cue[0]) : (int)(sizeof owner_cue / sizeof owner_cue[0]);
  int index = event->data[1]++;
  if (index >= count) {
    return false;
  }
  battle_event_program(event, steps[index][0]);
  battle_event_frame(event, steps[index][0] - 1, steps[index][1], cue_frame);
  return true;
}

static void start_cue(Fighter *fighter, bool victim) {
  BattleEvent *event = battle_event_start(fighter, cue_next, false, false);
  event->data[0] = victim;
  event->data[1] = 0;
  cue_next(event, fighter, (Fighter *)fighter_opponent(fighter));
}

/* 465270: wait 33, token 4dd8fc (0xd2), wait 20, token 4dd8f8 (0x281). */
void tanya_second_owner_cue(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Tanya fatality cue task");
  start_cue(fighter, false);
}

/* 465390 jump table 46576c: 45, list, 12, list, 8, 14dc, 20, 14e1, 23,
 * 14d7, 13, be, 15, 14dc, 20, 14dc, 33, then 2f8 before retiring. */
void tanya_second_victim_cue(Fighter *fighter, int task_type) {
  require(task_type == 619, "Unexpected Tanya fatality victim cue task");
  start_cue(fighter, true);
}

void tanya_second_event(Fighter *fighter, int selector) {
  require(g_battle.phase == PHASE_FATALITY && (selector == 69 || selector == 70), "Unexpected Tanya second scene event");
  /* 4315b0 queues camera shots on scene task 4dea40; the portable camera
   * follows both live actors, and the finisher owns the input lock. */
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

/* 4650b0: list 4e33e8 (Sonya, Tanya, Kitana) lowers the owner 0x147a. */
void tanya_second_height(Fighter *fighter) {
  int roster = fighter_opponent(fighter)->roster;
  if (roster == 2 || roster == 9 || roster == 16) {
    fighter->body.pos[AXIS_Y] += 0x147a;
  }
  retail_vm_enter_owned(fighter, 0x4e57c0, fighter_ready, 9);
}

/* 465200 (reaction 186): 475690/4756f0 copy the owner's facing, angles
 * and X/Z onto the victim, whose 567/568 clips share that origin. */
static void second_victim_scene(Fighter *owner) {
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  victim->facing = owner->facing;
  memcpy(victim->body.angle, owner->body.angle, sizeof victim->body.angle);
  victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  victim->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  fighter_cancel_mmove(victim);
  /* The shared origin must not be separated by ordinary body collision. */
  owner->flags |= FF_PAIRED;
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  retail_vm_enter_owned(victim, 0x4e5820, fighter_ready, 9);
}

void tanya_second_reaction(Fighter *fighter) {
  second_victim_scene(fighter);
}

void tanya_second_victim_pause(Fighter *fighter) {
  /* 466df0 retires the auxiliary victim task; the body keeps its pose. */
  fighter_finish_mmove(fighter, NULL);
}

static void victory_finish(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

static void victory_watch(Fighter *fighter) {
  if (fighter_move(fighter)->clip == 124 && fighter->native_motion.count == 0) {
    native_motion_begin(fighter, 50, victory_finish);
    native_motion_play(fighter);
    native_motion_step(fighter);
  }
}

/* 4712e0 enters the looping 4e6a40/4e6b88 victory. The portable finisher
 * ends 50 visits into that loop, as in Sub-Zero's port. */
void tanya_script_victory(Fighter *fighter) {
  fighter->body.physics = victory_watch;
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 9);
}

static void second_victory(Fighter *fighter) {
  /* 490c00 faces the peer and flips the facing bit before 4712e0. */
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? -1 : 1;
  tanya_script_victory(fighter);
}

/* 465130: stance pose, face the peer, then 120 visits before victory. */
void tanya_second_finish(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  mk4_mmove_bind(fighter, owner_pose.clip, owner_pose.last + 1, 0);
  native_hold(fighter, 120, second_victory);
}

void tanya_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, TANYA_EVENT_FATALITY_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(victim, S_KNOCKDOWN, &victim_pose);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(victim, 0x4e5b38, fighter_ready, 9);
  /* 475530 places the victim 1.0 ahead; 4906c0 grounds the owner. */
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), Q16_ONE);
  }
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(fighter, 0x4e5798, fighter_ready, 9);
}
