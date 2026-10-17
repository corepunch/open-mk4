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
  bool finished;
} MoveFiber;

/* Runtime stacks are not simulation snapshots. A slot belongs to one of the
 * two stable singleton fighter addresses, never to a copied Fighter value. */
static MoveFiber fibers[2];
static MoveFiber *running;

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

void fighter_cancel_fiber(Fighter *fighter) {
  MoveFiber *slot = fiber_slot(fighter);
  if (slot->coroutine == NULL) {
    return;
  }
  require(running != slot, "A running fiber must use fighter_fiber_finish for its own move transition");
  mco_coro *coroutine = slot->coroutine;
  FighterFn cleanup = slot->cleanup;
  *slot = (MoveFiber){0};
  if (fighter->script == fighter_step_fiber) {
    fighter->script = NULL;
  }
  require(mco_destroy(coroutine) == MCO_SUCCESS, "Cannot destroy suspended move fiber");
  if (cleanup != NULL) {
    cleanup(fighter);
  }
}

void fighter_cancel_all_fibers(void) {
  require(running == NULL, "Cannot reset the simulation inside a move fiber");
  for (int i = 0; i < 2; ++i) {
    fighter_cancel_fiber(&g_battle.fighters[i]);
  }
}

static void fiber_entry(mco_coro *coroutine) {
  MoveFiber *slot = mco_get_user_data(coroutine);
  slot->entry(slot->fighter);
}

static void fiber_resume(MoveFiber *slot) {
  require(running == NULL, "Move fibers must be scheduled by the simulation thread");
  running = slot;
  mco_result result = mco_resume(slot->coroutine);
  running = NULL;
  require(result == MCO_SUCCESS, "Cannot resume move fiber");
  if (slot->finished || mco_status(slot->coroutine) == MCO_DEAD) {
    Fighter *fighter = slot->fighter;
    FighterFn next = slot->next != NULL ? slot->next : fighter_ready;
    fighter_cancel_fiber(fighter);
    if (next != NULL) {
      next(fighter);
    }
  }
}

void fighter_start_fiber(Fighter *fighter, FighterFn entry, FighterFn cleanup) {
  require(running == NULL && entry != NULL, "Move fiber must start from the scheduler with an entry");
  fighter_cancel_fiber(fighter);
  MoveFiber *slot = fiber_slot(fighter);
  *slot = (MoveFiber){.fighter = fighter, .entry = entry, .cleanup = cleanup, .last_tick = g_battle.ticks};
  mco_desc desc = mco_desc_init(fiber_entry, 64 * 1024);
  desc.user_data = slot;
  desc.storage_size = 0;
  require(mco_create(&slot->coroutine, &desc) == MCO_SUCCESS, "Cannot allocate move fiber stack");
  fighter->script = fighter_step_fiber;
  fighter->sequence = NULL;
  fighter->continuation++;
  fiber_resume(slot);
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
  require(running != NULL && next != NULL, "Fiber finish requires a running fiber and continuation");
  running->next = next;
  running->finished = true;
  require(mco_yield(running->coroutine) == MCO_SUCCESS, "Cannot finish move fiber");
  require(false, "Finished move fiber was resumed");
  abort();
}
