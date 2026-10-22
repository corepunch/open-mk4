#include "retail_native.h"
#include "retail_vm.h"
#include "native_services.h"
#include "script.h"
#include "native.h"

/* 43d4b0 selects the cycle from the captured peer, not the active fighter.
 * The native routines own the counter and scene; retail words own playback. */
static void fan_cycle_enter(Fighter *fighter) {
  int roster = fighter_opponent(fighter)->roster;
  roster = roster == 16 ? 2 : roster == 17 ? 7 : roster;
  if (roster == 2 || roster == 9) {
    retail_vm_enter(fighter, 0x4e0138, fighter_ready);
  } else {
    retail_vm_bind_animation(fighter, 0x4fc0e4, 0);
    retail_vm_enter(fighter, 0x4e0128, fighter_ready);
  }
}

void retail_fan_start(Fighter *fighter) {
  retail_vm_enter((Fighter *)fighter_opponent(fighter), 0x4e0188, fighter_ready);
  retail_vm_enter(fighter, 0x4e0118, fighter_ready);
}

void retail_fan_finish(Fighter *fighter) {
  if (mk4_begin_stage_finish(fighter, FINISH_FAN)) {
    retail_fan_start(fighter);
  }
}

void retail_fan_tail(Fighter *fighter) {
  fighter->native_variant = 31;
  fan_cycle_enter(fighter);
}

void retail_fan_cycle(Fighter *fighter) {
  require(fighter->native_variant > 0, "Fan cycle without a native scene counter");
  if (--fighter->native_variant > 0) {
    fan_cycle_enter(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_cancel_mmove(victim);
  victim->sample_override = 69;
  retail_vm_enter(victim, 0x4e01a0, fighter_ready);
  mk4_native_fan_rise(fighter);
  /* CountdownStoreCallChain rebinds the release descriptor at sample 69. */
  retail_vm_bind_animation(fighter, 0x4fc0f0, 69);
  retail_vm_enter(fighter, 0x4e0148, fighter_ready);
}

void retail_fan_victory(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4e0158, fighter_ready);
}

void retail_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void spike_script_enter(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4e6950, fighter_ready);
}

static void retail_spike_delay(Fighter *fighter) {
  int wait = 30 - g_battle.phase_ticks;
  if (wait > 0) {
    native_hold(fighter, wait, spike_script_enter);
  } else {
    spike_script_enter(fighter);
  }
}

void retail_spike_finish(Fighter *fighter) {
  if (mk4_begin_stage_finish(fighter, FINISH_SPIKE)) {
    retail_spike_delay(fighter);
  }
}

/* Native scene setup owns actor state; clip lengths remain in the same
 * retail descriptors used by the interpreter. No fighter clip table exists. */
void retail_scene_pose(Fighter *fighter, int clip, int sample, bool owner) {
  static const MoveDef scene = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700};
  if (fighter_move(fighter)->clip != clip) {
    fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &scene);
  }
  mk4_script_scene_pose(fighter, clip, sample);
}

/* Native41f1b0/466df0 leaves the final rendered pose owned by the scene. */
void retail_task_pause(Fighter *fighter) {
  fighter_stop_mmove(fighter);
}

/* Native49cb60 retires the task. The portable caller owns its scene-join
 * callback; native identity never depends on a roster/context switch. */
void retail_task_retire(Fighter *fighter) {
  FighterFn completed = fighter->retail_vm.active ? fighter->retail_vm.finish : NULL;
  require(completed != fighter_ready && completed != retail_task_retire, "Task retirement needs a scene completion boundary");
  fighter_stop_mmove(fighter);
  if (completed != NULL) {
    completed(fighter);
  }
}

static void scene_body_complete(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  battle_end_finisher();
}

/* Native4817c0: the scene task lowers the remaining body once, then
 * yields60 before4314e0 completes orchestration. Raw audio controls remain
 * part of the engine scene boundary rather than indexed fighter samples. */
void retail_scene_body_finish(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] -= 0x147a;
  native_hold(fighter, 60, scene_body_complete);
}

enum {
  HOVER_EASE_START,
  HOVER_EASE,
  HOVER_WAIT_DOWN,
  HOVER_REVERSE_UP,
  HOVER_WAIT_UP,
  HOVER_REVERSE_DOWN
};

/*46a280/46a340/46a380: an independent task changes group gravity;
 * velocity predicates and ten-visit holds belong to this native task. */
static bool scene_hover_step(BattleEvent *event, Fighter *owner, Fighter *peer) {
  (void)peer;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  switch (event->phase) {
    case HOVER_EASE_START:
      event->data[0] = 10;
      event->phase = HOVER_EASE;
      break;
    case HOVER_EASE:
      owner->body.gravity += 0x41;
      if (--event->data[0] == 0) {
        owner->body.gravity = 0x83;
        event->phase = HOVER_WAIT_DOWN;
      }
      break;
    case HOVER_WAIT_DOWN:
      if (owner->body.vel[AXIS_Y] >= 0) {
        event->phase = HOVER_REVERSE_UP;
        battle_event_delay(event, 10);
      }
      break;
    case HOVER_REVERSE_UP:
      owner->body.gravity = -0x83;
      event->phase = HOVER_WAIT_UP;
      break;
    case HOVER_WAIT_UP:
      if (owner->body.vel[AXIS_Y] < 0) {
        event->phase = HOVER_REVERSE_DOWN;
        battle_event_delay(event, 10);
      }
      break;
    case HOVER_REVERSE_DOWN:
      owner->body.gravity = 0x83;
      event->phase = HOVER_WAIT_DOWN;
      break;
    default:
      require(false, "Invalid native hover phase");
  }
  return true;
}

void retail_scene_hover(Fighter *fighter, int task_type) {
  require(task_type == 38, "Unexpected native hover task");
  BattleEvent *event = battle_event_start(fighter, scene_hover_step, false, false);
  require(event != NULL, "Scene needs its independent hover task");
  fighter->body.gravity = -0x28f;
  event->phase = HOVER_EASE_START;
}

/* Portable scene orchestration waits for the original victory root. It
 * belongs to the scene caller, never to4712e0 or a particular model clip. */
static bool scene_victory_join(BattleEvent *event, Fighter *owner, Fighter *peer) {
  (void)peer;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    if (owner->retail_vm.active && owner->retail_vm.entry == 0x4e6a40) {
      event->phase = 1;
      battle_event_delay(event, 50);
    }
    return true;
  }
  fighter_cancel_all_mmoves();
  battle_end_finisher();
  return false;
}

void retail_scene_join_victory(Fighter *fighter) {
  require(battle_event_start(fighter, scene_victory_join, false, false) != NULL, "Scene needs a victory completion task");
}
