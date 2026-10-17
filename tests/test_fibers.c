#include "game.h"
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
static const MoveDef test_move = {.clip = 14, .flags = SF_LOCK};

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
  for (int i = 0; i < 2; ++i) {
    fighter_start_move(&g_battle.fighters[i], S_PUNCH, &test_move);
  }
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
  deterministic_nested_waits();
  interruptions_and_reset();
  phase_and_paired_playback();
  fighter_cancel_all_fibers();
  puts(
      "PASS: deterministic P1/P2 fiber scheduling; nested waits and locals; exact deadlines; completion; interruption/replacement/round/rematch/reset cleanup");
  return 0;
}
