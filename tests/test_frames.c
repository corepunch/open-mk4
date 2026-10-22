#include "game.h"
#include "mk4_program.h"

static Mk4Image image;
#include "native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(bool yes, const char *message) {
  if (!yes) {
    fprintf(stderr, "FAIL frames: %s\n", message);
    exit(1);
  }
}

static int events[32][3];
static int event_count, cleaned[2], completed[2];
static int frame_ai_calls, frame_think_calls, mmove_finished;
static const MoveDef test_move = {.clip = 14, .flags = SF_LOCK};
static void advance(int ticks);

static int player(const Fighter *fighter) {
  return fighter == &g_battle.fighters[0] ? 0 : 1;
}

static void record(const Fighter *fighter, int value) {
  check(event_count < 32, "bounded event buffer overflow");
  events[event_count][0] = (int)g_battle.ticks;
  events[event_count][1] = player(fighter);
  events[event_count++][2] = value;
}

static void cleanup(Fighter *fighter) {
  cleaned[player(fighter)]++;
}

static void finish(Fighter *fighter) {
  completed[player(fighter)]++;
  fighter_ready(fighter);
}

static void frame_ai(Fighter *fighter, float distance) {
  check(fighter == &g_battle.fighters[0] && distance == 1.5f, "mframe ai callback or distance changed");
  frame_ai_calls++;
}

static void frame_think(Fighter *fighter) {
  check(fighter_sample(fighter) == 3 || fighter_sample(fighter) == 4 || fighter_sample(fighter) == 5, "mframe think ran outside its displayed sample");
  frame_think_calls++;
}

static void mmove_end(Fighter *fighter) {
  mmove_finished++;
  fighter_ready(fighter);
}

static void setup(void) {
  battle_init(&image, 0, 4);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = -5 * Q16_ONE;
  g_battle.fighters[1].body.pos[AXIS_X] = 5 * Q16_ONE;
  event_count = 0;
  memset(events, 0, sizeof events);
  memset(cleaned, 0, sizeof cleaned);
  memset(completed, 0, sizeof completed);
  frame_ai_calls = 0;
  frame_think_calls = 0;
  mmove_finished = 0;
  for (int i = 0; i < 2; ++i) {
    fighter_start_move(&g_battle.fighters[i], S_PUNCH, &test_move);
  }
}

static const mframe_t test_mframes[] = {
    {frame_ai, 1.5f, frame_think},
    {NULL, 0.0f, NULL},
    {NULL, 0.0f, frame_think},
};
static const mmove_t test_mmove = {.firstframe = 3, .lastframe = 5, .frame = test_mframes, .endfunc = mmove_end};

static void mmove_frames_and_callbacks(void) {
  setup();
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_mmove(fighter, S_PUNCH, &test_move, &test_mmove);
  check(fighter_sample(fighter) == 3, "mmove did not select its first frame");
  advance(1);
  check(fighter_sample(fighter) == 4 && frame_ai_calls == 1 && frame_think_calls == 1, "fixed frame movement/action callback timing changed");
  advance(1);
  check(fighter_sample(fighter) == 5 && frame_think_calls == 1, "frame with optional callbacks did not advance");
  advance(1);
  check(mmove_finished == 1 && fighter->state == S_IDLE && fighter->current_mmove == NULL && frame_think_calls == 2,
      "mmove end callback did not run exactly once after the final frame");
}

static void held_think(Fighter *fighter) {
  frame_think(fighter);
  fighter->mmove_hold = frame_think_calls < 4;
}

static const mframe_t held_frames[] = {{.thinkfunc = held_think}};
static const mmove_t held_phase = {.firstframe = 4, .lastframe = 4, .frame = held_frames, .endfunc = finish};

static void branch_phase(Fighter *fighter, float distance) {
  (void)distance;
  fighter_mmove_phase(fighter, &held_phase);
}

static void stale_think(Fighter *fighter) {
  (void)fighter;
  check(false, "replaced frame ran its action callback");
}

static const mframe_t branch_frames[] = {{.aifunc = branch_phase, .thinkfunc = stale_think}};
static const mmove_t branch_move = {.firstframe = 3, .lastframe = 3, .frame = branch_frames, .cleanup = cleanup};

static void mmove_phases_and_cleanup(void) {
  setup();
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_mmove(fighter, S_PUNCH, &test_move, &branch_move);
  fighter->contact = CONTACT_BLOCK;
  fighter->buffered = BUTTON_BIT(BUTTON_HIGH_PUNCH);
  fighter->flags |= FF_PAIRED;
  int action = fighter->action;
  advance(1);
  check(fighter->current_mmove == &held_phase && frame_think_calls == 0 && cleaned[0] == 0, "phase branch advanced immediately or cleaned the action");
  check(fighter->contact == CONTACT_BLOCK && fighter->buffered == BUTTON_BIT(BUTTON_HIGH_PUNCH) && (fighter->flags & FF_PAIRED) && fighter->action == action,
      "phase branch reset combat state");
  advance(3);
  fighter_step_mmove(fighter);
  check(fighter->current_mmove == &held_phase && frame_think_calls == 3, "held frame skipped its action callback or advanced twice in a tick");
  advance(1);
  check(fighter->state == S_IDLE && cleaned[0] == 1 && completed[0] == 1, "held phase did not finish and clean exactly once");
  for (int kind = 0; kind < 5; ++kind) {
    setup();
    fighter_start_mmove(fighter, S_PUNCH, &test_move, &branch_move);
    advance(1);
    if (kind == 0) {
      fighter_interrupt(fighter, fighter_ready);
    } else if (kind == 1) {
      fighter_start_move(fighter, S_HIT, &test_move);
    } else if (kind == 2) {
      battle_next_round();
    } else if (kind == 3) {
      battle_init(&image, 4, 0);
    } else {
      battle_reset_match();
    }
    advance(8);
    check(cleaned[0] == 1 && completed[0] == 0 && fighter->current_mmove == NULL, "cancelled mmove resumed or missed cleanup");
  }
}

static float held_distances[4];
static int held_visits, jump_visits;

static void held_ai(Fighter *fighter, float distance) {
  (void)fighter;
  check(held_visits < 4, "held frame did not jump away");
  held_distances[held_visits++] = distance;
}

static void hold_then_jump(Fighter *fighter) {
  fighter->mmove_hold = held_visits < 3;
  if (!fighter->mmove_hold) {
    fighter_mmove_nextframe(fighter, 2);
  }
}

static void jump_to_zero(Fighter *fighter) {
  check(fighter_sample(fighter) == 2, "jump did not select target before callback");
  jump_visits++;
  fighter_mmove_nextframe(fighter, 0);
}

static const mframe_t jump_frames[] = {
    {.aifunc = held_ai, .dist = 1.5f, .thinkfunc = hold_then_jump},
    {.thinkfunc = stale_think},
    {.thinkfunc = jump_to_zero},
};
static const mmove_t jump_move = {.firstframe = 0, .lastframe = 2, .frame = jump_frames};
static const mframe_t loop_frames[2] = {{0}};
static const mmove_t loop_move = {.firstframe = 0, .lastframe = 1, .frame = loop_frames, .cleanup = cleanup};

static void mmove_hold_jump_and_loop(void) {
  setup();
  Fighter *fighter = &g_battle.fighters[0];
  held_visits = jump_visits = 0;
  fighter_start_mmove(fighter, S_PUNCH, &test_move, &jump_move);
  advance(4);
  check(held_visits == 3 && jump_visits == 1 && held_distances[0] == 1.5f && held_distances[1] == 0 && held_distances[2] == 0,
      "held frames must repeat callbacks with zero movement and honor frame jumps");
  advance(1);
  check(held_visits == 4 && held_distances[3] == 1.5f, "jump to frame zero was lost or treated as a held visit");
  fighter_start_mmove(fighter, S_PUNCH, &test_move, &loop_move);
  advance(5);
  check(fighter->current_mmove == &loop_move && fighter_sample(fighter) == 1 && cleaned[0] == 0, "unchanged move must loop without cleanup");
  fighter_stop_mmove(fighter);
  check(fighter->current_mmove == NULL && fighter_sample(fighter) == 1 && cleaned[0] == 1, "explicit stop must retain final pose and clean once");
}

static void native_action(Fighter *fighter) {
  record(fighter, 41);
}

static void native_motion_action(Fighter *fighter) {
  record(fighter, 42);
}

static void native_rows(Fighter *fighter) {
  native_begin(fighter, 1, 3, finish);
  native_frame(fighter, 1, 3, NULL);
  native_frame(fighter, 2, 4, native_action);
  native_frame(fighter, 3, 5, NULL);
  native_play(fighter, false);
}

static void native_frame_schedules(void) {
  for (int interrupted = 0; interrupted < 2; ++interrupted) {
    setup();
    Fighter *fighter = &g_battle.fighters[0];
    fighter->mmove_cleanup = cleanup;
    native_rows(fighter);
    check(fighter->current_mmove != NULL && cleaned[0] == 0, "native playback must retain its action cleanup");
    advance(1);
    check(event_count == 0 && fighter_sample(fighter) == 3, "native schedule ran its boundary action early");
    if (interrupted) {
      fighter_interrupt(fighter, fighter_ready);
    }
    advance(4);
    check(event_count == (interrupted ? 0 : 1) && cleaned[0] == 1 && completed[0] == (interrupted ? 0 : 1),
        "native frame action/cleanup survived replacement or ran twice");
  }
  setup();
  Fighter *fighter = &g_battle.fighters[0];
  native_motion_begin(fighter, 4, NULL);
  native_motion_frame(fighter, 1, NULL, 0, native_motion_action);
  native_motion_play(fighter);
  advance(6);
  check(event_count == 1 && events[0][2] == 42 && fighter->body.physics == NULL, "independent motion frames must execute scheduled actions once and stop");
  for (int player = 0; player < 2; ++player) {
    for (int replace = 0; replace < 3; ++replace) {
      setup();
      fighter = &g_battle.fighters[player];
      native_motion_begin(fighter, 4, finish);
      native_motion_frame(fighter, 1, NULL, 0, native_motion_action);
      native_motion_play(fighter);
      advance(1);
      if (replace == 2) {
        g_battle.phase = PHASE_FATALITY;
        g_battle.finisher = FINISH_LIU_KANG_1;
        battle_end_finisher();
      } else if (replace == 1) {
        fighter_start_move(fighter, S_HIT, &test_move);
      } else {
        fighter_interrupt(fighter, fighter_ready);
      }
      check(fighter->native_motion.count == 0 && fighter->body.physics == NULL, "action replacement must cancel its auxiliary frame program");
      advance(6);
      check(event_count == 0 && completed[player] == 0, "interrupted auxiliary frames ran a stale action or completion");
    }
  }
}

static bool event_frame_action(BattleEvent *event, Fighter *owner, Fighter *other) {
  (void)other;
  record(owner, event->frame_value);
  return true;
}

static bool event_frame_end(BattleEvent *event, Fighter *owner, Fighter *other) {
  (void)event;
  (void)other;
  record(owner, 53);
  return false;
}

static void event_frame_schedules(void) {
  for (int interrupted = 0; interrupted < 2; ++interrupted) {
    setup();
    Fighter *fighter = &g_battle.fighters[0];
    BattleEvent *event = battle_event_start(fighter, event_frame_end, true, true);
    battle_event_program(event, 4);
    battle_event_frame(event, 1, 51, event_frame_action);
    advance(1);
    battle_event_pump();
    check(event_count == 0, "event action ran before its frame or advanced twice in one pump");
    advance(1);
    check(event_count == 1 && events[0][2] == 51, "event frame lost its authored action value");
    if (interrupted) {
      fighter_ready(fighter);
    }
    advance(2);
    check(event_count == (interrupted ? 1 : 2) && !event->active, "event frame completion survived cancellation or failed to stop");
  }
}

static void advance(int ticks) {
  for (int i = 0; i < ticks; ++i) {
    battle_step((InputFrame[2]){0});
  }
}

int main(void) {
  image = mk4_image_load("MK4/MK-4.EXE");
  mk4_program_load(&image, "build/mk4.scripts");
  mmove_frames_and_callbacks();
  mmove_phases_and_cleanup();
  mmove_hold_jump_and_loop();
  native_frame_schedules();
  event_frame_schedules();
  puts("PASS: fixed frame callbacks, holds, jumps and loops; native rows, motion and event schedules; completion; interruption/replacement/round/rematch/reset "
       "cleanup");
  return 0;
}
