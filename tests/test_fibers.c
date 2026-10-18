#include "game.h"
#include "native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(bool yes, const char *message) {
  if (!yes) {
    fprintf(stderr, "FAIL fibers: %s\n", message);
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

static int nested_wait(Fighter *fighter, int value) {
  int local = value;
  fighter_wait_ticks(2);
  local += 7;
  record(fighter, local);
  fighter_wait_ticks(1);
  return local + 1;
}

static void nested_script(Fighter *fighter) {
  int local = 10;
  record(fighter, local);
  local = nested_wait(fighter, local);
  record(fighter, local);
  fighter_wait_ticks(3);
  record(fighter, local + 1);
  fighter_fiber_finish(finish);
}

static void short_script(Fighter *fighter) {
  fighter_wait_ticks(1);
  record(fighter, 99);
  /* Returning normally retires the stack and enters fighter_ready. */
}

static void setup(void) {
  battle_init(0, 4);
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
  fighter->mmove_hold = ++fighter->script_timer < 4;
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
  check(fighter->current_mmove == &held_phase && fighter->script_timer == 0 && cleaned[0] == 0, "phase branch advanced immediately or cleaned the action");
  check(fighter->contact == CONTACT_BLOCK && fighter->buffered == BUTTON_BIT(BUTTON_HIGH_PUNCH) && (fighter->flags & FF_PAIRED) && fighter->action == action,
      "phase branch reset combat state");
  advance(3);
  fighter_step_mmove(fighter);
  check(fighter->current_mmove == &held_phase && fighter->script_timer == 3 && frame_think_calls == 3,
      "held frame skipped its action callback or advanced twice in a tick");
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
      battle_init(4, 0);
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

static void native_handoff(Fighter *fighter) {
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
    fighter_start_fiber(fighter, native_handoff, cleanup);
    check(!fighter_fiber_active(fighter) && fighter->current_mmove != NULL && cleaned[0] == 0,
        "native handoff must retire the fiber while retaining action cleanup");
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
}

static void advance(int ticks) {
  for (int i = 0; i < ticks; ++i) {
    battle_step((Input[2]){0});
  }
}

static void deterministic_nested_waits(void) {
  int expected[8][3] = {{0, 0, 10}, {0, 1, 10}, {2, 0, 17}, {2, 1, 17}, {3, 0, 18}, {3, 1, 18}, {6, 0, 19}, {6, 1, 19}};
  for (int repeat = 0; repeat < 20; ++repeat) {
    setup();
    fighter_start_fiber(&g_battle.fighters[0], nested_script, cleanup);
    fighter_start_fiber(&g_battle.fighters[1], nested_script, cleanup);
    fighter_step_fiber(&g_battle.fighters[0]);
    check(event_count == 2, "a same-tick pump must not resume a fiber twice");
    advance(5);
    check(event_count == 6 && fighter_fiber_active(&g_battle.fighters[0]), "nested waits resumed before their deadlines");
    advance(1);
    check(event_count == 8 && memcmp(events, expected, sizeof expected) == 0, "fiber order/local variables/deadlines must be reproducible");
    for (int i = 0; i < 2; ++i) {
      check(!fighter_fiber_active(&g_battle.fighters[i]) && cleaned[i] == 1 && completed[i] == 1 && g_battle.fighters[i].state == S_IDLE,
          "completion must retire the stack and transition exactly once on the scheduler");
    }
    advance(10);
    check(event_count == 8 && completed[0] == 1 && completed[1] == 1, "completed fibers must never resume again");
  }
}

static void interruptions_and_reset(void) {
  for (int kind = 0; kind < 5; ++kind) {
    setup();
    Fighter *fighter = &g_battle.fighters[0];
    fighter_start_fiber(fighter, nested_script, cleanup);
    advance(1);
    if (kind == 0) {
      fighter_interrupt(fighter, fighter_ready);
    } else if (kind == 1) {
      fighter_start_move(fighter, S_HIT, &test_move);
    } else if (kind == 2) {
      battle_next_round();
    } else if (kind == 3) {
      battle_init(4, 0);
    } else {
      fighter_start_fiber(fighter, short_script, cleanup);
    }
    check(cleaned[0] == 1, "interruption/replacement/reset must retire the old stack exactly once");
    advance(10);
    check(event_count == (kind == 4 ? 2 : 1) && completed[0] == 0, "an interrupted nested wait ran stale code");
    check(!fighter_fiber_active(fighter), "interruption or normal return leaked an active fiber");
    if (kind == 4) {
      check(events[1][2] == 99 && cleaned[0] == 2 && fighter->state == S_IDLE, "replacement must get a fresh deadline and return safely");
    }
  }
  setup();
  fighter_start_fiber(&g_battle.fighters[0], nested_script, cleanup);
  fighter_start_fiber(&g_battle.fighters[1], nested_script, cleanup);
  battle_reset_match();
  check(cleaned[0] == 1 && cleaned[1] == 1, "rematch must retire both fighter stacks");
  advance(100);
  check(event_count == 2 && completed[0] == 0 && completed[1] == 0, "rematch resumed an old fiber");
}

static void sequence_event(Fighter *fighter) {
  record(fighter, fighter->ticks);
}

static void forbidden_end(Fighter *fighter) {
  (void)fighter;
  check(false, "blocking sequence must return to its C caller, not invoke the old end callback");
}

static bool hold_done(const Fighter *fighter) {
  return fighter->ticks >= 5;
}

static const MoveSegment phase_segments[] = {{0, 6, -1, 1}};
static const MoveEvent phase_events[] = {{0, sequence_event}, {2, sequence_event}};
static const MoveSequence phase_sequence = {phase_segments, 1, phase_events, 2, 3, forbidden_end, hold_done};
static const MoveDef phase_move = {.clip = 14, .flags = SF_LOCK, .sequence = &phase_sequence};

static void phase_script(Fighter *fighter) {
  int local = 73;
  fighter_wait_ticks(2);
  fighter_fiber_move(fighter, S_PUNCH, &phase_move);
  fighter_fiber_sequence(fighter, &phase_sequence);
  check(fighter_sample(fighter) == 4 && fighter->ticks == 5, "sequence reverse/hold timing changed");
  fighter->sample_override = fighter_sample(fighter);
  fighter_fiber_play_to(fighter, 0, -2);
  record(fighter, local);
  fighter_fiber_finish(NULL);
}

static void paired_script(Fighter *fighter) {
  record(fighter, 20);
  fighter_wait_ticks(1);
  record(fighter, 21);
  fighter_fiber_finish(NULL);
}

static void pair_owner_script(Fighter *fighter) {
  fighter_wait_ticks(1);
  Fighter *victim = &g_battle.fighters[1 - player(fighter)];
  fighter_start_move(victim, S_HIT, &test_move);
  fighter_start_fiber(victim, paired_script, cleanup);
  record(fighter, 10);
  fighter_wait_ticks(2);
  fighter_fiber_finish(NULL);
}

static void phase_and_paired_playback(void) {
  setup();
  Fighter *fighter = &g_battle.fighters[0];
  fighter_start_fiber(fighter, phase_script, cleanup);
  advance(8);
  check(fighter_fiber_active(fighter) && event_count == 2, "phase playback completed early or duplicated events");
  advance(1);
  check(!fighter_fiber_active(fighter) && fighter->script == NULL && cleaned[0] == 1, "explicit parked completion must release its stack");
  check(events[0][0] == 2 && events[0][2] == 0 && events[1][0] == 4 && events[1][2] == 2 && events[2][0] == 9 && events[2][2] == 73,
      "phase rebinding must preserve locals and run each event exactly once");
  for (int owner = 0; owner < 2; ++owner) {
    setup();
    fighter_start_fiber(&g_battle.fighters[1 - owner], nested_script, cleanup);
    fighter_start_fiber(&g_battle.fighters[owner], pair_owner_script, cleanup);
    advance(4);
    check(event_count == 4 && events[1][0] == 1 && events[1][1] == owner && events[1][2] == 10 && events[2][0] == 1 && events[2][1] == 1 - owner &&
              events[2][2] == 20 && events[3][0] == 2 && events[3][2] == 21,
        "paired entry must start on the host after owner yields and suspend a full tick in either player slot");
    check(cleaned[owner] == 1 && cleaned[1 - owner] == 2 && !fighter_fiber_active(&g_battle.fighters[0]) && !fighter_fiber_active(&g_battle.fighters[1]),
        "paired replacement/completion must release both stacks exactly once");
  }
}

int main(void) {
  mmove_frames_and_callbacks();
  mmove_phases_and_cleanup();
  mmove_hold_jump_and_loop();
  native_frame_schedules();
  deterministic_nested_waits();
  interruptions_and_reset();
  phase_and_paired_playback();
  fighter_cancel_all_fibers();
  puts(
      "PASS: fixed frame callbacks, holds, jumps and loops; deterministic P1/P2 fiber scheduling; nested waits and locals; exact deadlines; completion; interruption/replacement/round/rematch/reset cleanup");
  return 0;
}
