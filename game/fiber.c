#include "game.h"
#include <stdlib.h>

/* Keep the platform context switch in the unmodified, pinned dependency.
 * This scheduler uses one host thread; no TLS, worker threads or wall clock. */
#define MCO_NO_MULTITHREAD
#define MINICORO_IMPL
#include "../third_party/minicoro/minicoro.h"

typedef struct MoveFiber {
  mco_coro *coroutine;
  Fighter *fighter;
  FighterFn entry, cleanup, next;
  uint64_t clock, wake, last_tick;
  bool finished, started, rebinding, frame_wait;
} MoveFiber;

/* Runtime stacks are not simulation snapshots. A slot belongs to one of the
 * two stable singleton fighter addresses, never to a copied Fighter value. */
static MoveFiber fibers[2];
static MoveFiber *running;
static uint64_t pumped_ticks[2];

static MoveFiber *fiber_slot(const Fighter *fighter) {
  for (int i = 0; i < 2; ++i) {
    if (fighter == &g_battle.fighters[i]) {
      return &fibers[i];
    }
  }
  require(false, "Fiber owner must be an active fighter");
  return NULL;
}

bool fighter_fiber_active(const Fighter *fighter) {
  return fiber_slot(fighter)->coroutine != NULL;
}

bool fighter_fiber_running(const Fighter *fighter) {
  return running != NULL && running->fighter == fighter;
}

void fighter_fiber_pump_begin(Fighter *fighter) {
  pumped_ticks[fiber_slot(fighter) - fibers] = g_battle.ticks;
}

void fighter_cancel_fiber(Fighter *fighter) {
  MoveFiber *slot = fiber_slot(fighter);
  if (slot->coroutine == NULL) {
    return;
  }
  if (running == slot && slot->rebinding) {
    return;
  }
  require(running != slot, "A running fiber must use fighter_fiber_move or fighter_fiber_finish for its own move transition");
  mco_coro *coroutine = slot->coroutine;
  FighterFn cleanup = slot->cleanup;
  bool frame_wait = slot->frame_wait;
  *slot = (MoveFiber){0};
  if (fighter->script == fighter_step_fiber) {
    fighter->script = NULL;
  }
  require(mco_destroy(coroutine) == MCO_SUCCESS, "Cannot destroy suspended move fiber");
  /* An awaited native phase belongs to this stack. Leaving it installed
   * would let a released pair or scene keep running orphaned callbacks. */
  if (frame_wait && fighter->current_mmove != NULL) {
    fighter_stop_mmove(fighter);
  }
  if (cleanup != NULL) {
    cleanup(fighter);
  }
}

void fighter_cancel_all_fibers(void) {
  require(running == NULL, "Cannot reset the simulation inside a move fiber");
  for (int i = 0; i < 2; ++i) {
    fighter_cancel_fiber(&g_battle.fighters[i]);
    pumped_ticks[i] = UINT64_MAX;
  }
}

static void fiber_entry(mco_coro *coroutine) {
  MoveFiber *slot = mco_get_user_data(coroutine);
  slot->entry(slot->fighter);
}

static void fiber_resume(MoveFiber *slot) {
  require(running == NULL, "Move fibers must be scheduled by the simulation thread");
  if (!slot->started) {
    slot->last_tick = g_battle.ticks;
  }
  slot->started = true;
  running = slot;
  mco_result result = mco_resume(slot->coroutine);
  running = NULL;
  require(result == MCO_SUCCESS, "Cannot resume move fiber");
  if (slot->finished || mco_status(slot->coroutine) == MCO_DEAD) {
    Fighter *fighter = slot->fighter;
    FighterFn next = slot->finished ? slot->next : fighter_ready;
    fighter_cancel_fiber(fighter);
    if (next != NULL) {
      next(fighter);
    }
  }
  /* A paired actor can be initialized by the running script, but its entry
   * is resumed only after we have returned to the scheduler stack. */
  for (int i = 0; i < 2; ++i) {
    if (fibers[i].coroutine != NULL && !fibers[i].started) {
      fiber_resume(&fibers[i]);
    }
  }
}

void fighter_start_fiber(Fighter *fighter, FighterFn entry, FighterFn cleanup) {
  require(entry != NULL && !fighter_fiber_running(fighter), "Move fiber requires an entry and cannot replace its running owner");
  fighter_cancel_fiber(fighter);
  fighter_cancel_mmove(fighter);
  MoveFiber *slot = fiber_slot(fighter);
  *slot = (MoveFiber){.fighter = fighter, .entry = entry, .cleanup = cleanup, .last_tick = g_battle.ticks};
  if (running != NULL && pumped_ticks[slot - fibers] != g_battle.ticks) {
    slot->last_tick = g_battle.ticks - 1;
  }
  mco_desc desc = mco_desc_init(fiber_entry, 64 * 1024);
  desc.user_data = slot;
  desc.storage_size = 0;
  require(mco_create(&slot->coroutine, &desc) == MCO_SUCCESS, "Cannot allocate move fiber stack");
  fighter->script = fighter_step_fiber;
  fighter->continuation++;
  if (running == NULL) {
    fiber_resume(slot);
  }
}

/* Rebinding a phase is an explicit operation on the running actor. Locals
 * and the stack survive; ordinary external move replacement still cancels. */
void fighter_fiber_move(Fighter *fighter, int state, const MoveDef *move) {
  require(fighter_fiber_running(fighter), "Fiber phase change requires its running actor");
  running->rebinding = true;
  fighter_start_move(fighter, state, move);
  running->rebinding = false;
  fighter->script = fighter_step_fiber;
}

void fighter_fiber_sequence(Fighter *fighter, const MoveSequence *sequence) {
  require(fighter_fiber_running(fighter), "Fiber playback requires its running actor");
  if (fighter->sequence != sequence || fighter->ticks != 0) {
    running->rebinding = true;
    fighter_start_sequence(fighter, sequence);
    running->rebinding = false;
    fighter->script = fighter_step_fiber;
  }
  while (fighter->ticks < sequence->count) {
    fighter_wait_ticks(1);
    if (fighter->ticks < sequence->count) {
      fighter_step_sequence(fighter);
    }
  }
  while (sequence->until != NULL && !sequence->until(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter->sequence_finished = true;
}

void fighter_fiber_play_to(Fighter *fighter, int sample, int step) {
  require(fighter_fiber_running(fighter) && sample >= 0 && step != 0 && fighter->sample_override >= 0, "Invalid fiber sample playback");
  while (step > 0 ? fighter->sample_override < sample : fighter->sample_override > sample) {
    fighter_wait_ticks(1);
    int next = fighter->sample_override + step;
    fighter->sample_override = step > 0 ? (next < sample ? next : sample) : (next > sample ? next : sample);
  }
}

/* Called only in the existing fighter pump, P1 then P2. A repeated call in
 * the same battle tick cannot advance the clock or resume a second time. */
void fighter_step_fiber(Fighter *fighter) {
  MoveFiber *slot = fiber_slot(fighter);
  require(slot->coroutine != NULL, "Missing move fiber");
  if (slot->last_tick == g_battle.ticks) {
    return;
  }
  slot->last_tick = g_battle.ticks;
  if (slot->frame_wait) {
    fighter_step_mmove(fighter);
    if (slot->coroutine != NULL && slot->frame_wait && fighter->current_mmove == NULL) {
      slot->frame_wait = false;
      fighter->script = fighter_step_fiber;
      fiber_resume(slot);
    }
    return;
  }
  slot->clock++;
  if (slot->clock >= slot->wake) {
    fiber_resume(slot);
  }
}

void fighter_wait_ticks(int ticks) {
  require(running != NULL && ticks > 0, "Fiber wait requires a running fiber and positive simulation ticks");
  MoveFiber *slot = running;
  slot->wake = slot->clock + (uint64_t)ticks;
  require(mco_yield(slot->coroutine) == MCO_SUCCESS, "Cannot yield move fiber");
}

/* Finish on the scheduler stack so entry can replace this move safely.
 * This call never returns; no stack locals may escape into next/cleanup. */
_Noreturn void fighter_fiber_finish(FighterFn next) {
  require(running != NULL, "Fiber finish requires a running fiber");
  running->next = next;
  running->finished = true;
  require(mco_yield(running->coroutine) == MCO_SUCCESS, "Cannot finish move fiber");
  require(false, "Finished move fiber was resumed");
  abort();
}

/* Native tails retire their stack and transfer action cleanup to the frame
 * runner. The successor installs actor-owned rows on the scheduler stack. */
_Noreturn void fighter_fiber_handoff(FighterFn next) {
  require(running != NULL, "Native frame handoff requires a running fiber");
  if (running->cleanup != NULL) {
    running->fighter->mmove_cleanup = running->cleanup;
  }
  running->cleanup = NULL;
  fighter_fiber_finish(next);
}

/* A returning native binding suspends its VM stack while ordinary frame
 * callbacks own playback. Completion resumes that stack on the same boundary. */
void fighter_fiber_await_frames(Fighter *fighter) {
  require(fighter_fiber_running(fighter) && fighter->current_mmove != NULL, "Native frame wait needs its running owner");
  running->frame_wait = true;
  fighter->script = fighter_step_fiber;
  require(mco_yield(running->coroutine) == MCO_SUCCESS, "Cannot await native frames");
}

void fighter_fiber_release_to_mmove(Fighter *fighter) {
  MoveFiber *slot = fiber_slot(fighter);
  require(running != slot, "Running native handoff must yield");
  if (slot->cleanup != NULL) {
    fighter->mmove_cleanup = slot->cleanup;
    slot->cleanup = NULL;
  }
  fighter_cancel_fiber(fighter);
}
