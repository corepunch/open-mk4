#include "game.h"
#include "retail_vm.h"

/* Cleanup is detached before calling user code, so replacement/reset cannot
 * run it twice. A phase change deliberately retains the action's cleanup. */
void fighter_execution_reset(Fighter *fighter) {
  retail_vm_reset(fighter);
  fighter->execution.native_frames.script_end = (RetailScriptContinuation){0};
  fighter->execution.native_frames.native_end = (RetailNativeContinuation){0};
  fighter->execution.frames = NULL;
  fighter->execution.hold = false;
  fighter->execution.nextframe = -1;
  fighter->execution.generation++;
}

void fighter_execution_cancel(Fighter *fighter) {
  FighterFn cleanup = fighter->execution.cleanup;
  fighter->execution.cleanup = NULL;
  fighter_execution_reset(fighter);
  if (cleanup != NULL) {
    cleanup(fighter);
  }
}

bool fighter_execution_step(Fighter *fighter) {
  if (fighter->execution.script.active) {
    require(fighter->execution.frames == NULL, "Script and native playback cannot both own an actor");
    retail_vm_step(fighter);
    return true;
  }
  if (fighter->execution.frames != NULL) {
    fighter_step_mmove(fighter);
    return true;
  }
  return false;
}

void fighter_execution_cancel_all(void) {
  for (int i = 0; i < 2; ++i) {
    fighter_execution_cancel(&g_battle.fighters[i]);
  }
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    SceneActor *slot = &g_battle.scene_actors[i];
    fighter_execution_cancel(&slot->actor);
    slot->active = false;
  }
}

void fighter_execution_finish(Fighter *fighter, FighterFn next) {
  fighter_execution_cancel(fighter);
  if (next != NULL) {
    next(fighter);
  }
}

/* This changes only playback, retaining combat identity, contact, buttons,
 * paired ownership and action-local state. New phases start next actor tick. */
void fighter_mmove_phase(Fighter *fighter, const mmove_t *phase) {
  retail_vm_reset(fighter);
  require(phase != NULL && phase->frame != NULL && phase->lastframe >= phase->firstframe, "Invalid mmove phase");
  for (int frame = phase->firstframe; frame <= phase->lastframe; ++frame) {
    if (phase->samples != NULL) {
      require(phase->samples[frame - phase->firstframe] >= 0, "Invalid mframe sample map");
    }
  }
  fighter->execution.frames = phase;
  fighter->execution.frame = phase->firstframe;
  fighter->execution.nextframe = -1;
  fighter->execution.hold = false;
  fighter->execution.last_tick = g_battle.ticks;
  fighter->sample_override = -1;
  fighter->execution.generation++;
}

void fighter_start_mmove(Fighter *fighter, int state, const MoveDef *move, const mmove_t *mmove) {
  fighter_start_move(fighter, state, move);
  fighter_mmove_phase(fighter, mmove);
  fighter->execution.cleanup = mmove->cleanup;
}

static bool mmove_changed(const Fighter *fighter, const mmove_t *phase, uint64_t continuation) {
  return fighter->execution.generation != continuation || fighter->execution.frames != phase || fighter->pending != NULL;
}

/* Frame jumps are consumed before the next callback, including a jump to zero.
 * Callbacks can hold any row and release it on a later visit. */
void fighter_mmove_nextframe(Fighter *fighter, int frame) {
  const mmove_t *move = fighter->execution.frames;
  require(move != NULL && frame >= move->firstframe && frame <= move->lastframe, "Invalid mmove frame jump");
  fighter->execution.nextframe = frame;
}

void fighter_execution_stop(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  fighter_execution_cancel(fighter);
  fighter->sample_override = sample;
}

void fighter_step_mmove(Fighter *fighter) {
  const mmove_t *move = fighter->execution.frames;
  require(move != NULL, "Fighter has no mmove");
  if (fighter->execution.last_tick == g_battle.ticks) {
    return;
  }
  fighter->execution.last_tick = g_battle.ticks;
  bool jumped = fighter->execution.nextframe >= 0;
  if (jumped) {
    fighter->execution.frame = fighter->execution.nextframe;
    fighter->execution.nextframe = -1;
    fighter->sample_override = -1;
  }
  const mframe_t *frame = &move->frame[fighter->execution.frame - move->firstframe];
  uint64_t continuation = fighter->execution.generation;
  if (frame->aifunc != NULL) {
    frame->aifunc(fighter, fighter->execution.hold ? 0.0f : frame->dist);
    if (mmove_changed(fighter, move, continuation)) {
      return;
    }
  }
  if (frame->thinkfunc != NULL) {
    frame->thinkfunc(fighter);
    if (mmove_changed(fighter, move, continuation)) {
      return;
    }
  }
  if (fighter->execution.hold || fighter->execution.nextframe >= 0) {
    return;
  }
  if (fighter->execution.frame == move->lastframe) {
    if (move->endfunc != NULL) {
      move->endfunc(fighter);
      if (mmove_changed(fighter, move, continuation)) {
        return;
      }
      if (fighter->execution.hold || fighter->execution.nextframe >= 0) {
        return;
      }
    }
    fighter->execution.frame = move->firstframe;
  } else {
    fighter->execution.frame++;
  }
  fighter->sample_override = -1;
}
