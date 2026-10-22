#include "retail_native.h"
#include "retail_vm.h"
#include "native_services.h"
#include "script.h"
#include "native.h"
#include "retail_data.h"
#include <string.h>

/* 43d4b0 selects the cycle from the captured peer, not the active fighter.
 * The native routines own the counter and scene; retail words own playback. */
static void fan_cycle_enter(Fighter *fighter) {
  int roster = fighter_opponent(fighter)->roster;
  roster = roster == 16 ? 2 : roster == 17 ? 7 : roster;
  if (roster == 2 || roster == 9) {
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_FAN_FATALITY_OWNER_CYCLE, fighter_ready);
  } else {
    retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_414, 0);
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAREK_FAN_FATALITY_MALE_VICTIM_OWNER_CYCLE, fighter_ready);
  }
}

void retail_fan_start(Fighter *fighter) {
  retail_vm_enter((Fighter *)fighter_opponent(fighter), MK4_RETAIL_SCRIPT_JAX_FAN_FATALITY_VICTIM, fighter_ready);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_FAN_FATALITY_PERFORMER, fighter_ready);
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
  retail_vm_enter(victim, MK4_RETAIL_SCRIPT_JAX_FAN_FATALITY_VICTIM_RELEASE, fighter_ready);
  mk4_native_fan_rise(fighter);
  /* CountdownStoreCallChain rebinds the release descriptor at sample 69. */
  retail_vm_bind_animation(fighter, MK4_RETAIL_ANIMATION_DESCRIPTOR_CLIP_415, 69);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_JAX_FAN_FATALITY_OWNER_RELEASE, fighter_ready);
}

void retail_fan_victory(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_FAN_VICTORY, fighter_ready);
}

void retail_fan_victim_end(Fighter *fighter) {
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter_finish_mmove(fighter, NULL);
}

static void spike_script_enter(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SPIKE_ENTER, fighter_ready);
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
    if (owner->retail_vm.active && owner->retail_vm.entry == MK4_RETAIL_SCRIPT_RAIDEN_VICTORY) {
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

/*475690/4756f0 share facing, orientation and horizontal origin with the
 * captured peer. Retail words bind the scene's victim animations. */
void retail_scene_reaction(Fighter *fighter) {
  Fighter *owner = fighter_peer(fighter);
  uint32_t entry = retail_reaction_target(battle_data(), (unsigned)fighter->reaction);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_0700};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  fighter->facing = owner->facing;
  memcpy(fighter->body.angle, owner->body.angle, sizeof fighter->body.angle);
  fighter->body.pos[AXIS_X] = owner->body.pos[AXIS_X];
  fighter->body.pos[AXIS_Z] = owner->body.pos[AXIS_Z];
  owner->flags |= FF_PAIRED;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE | FF_PAIRED;
  uint32_t script;
  switch (entry) {
    case RETAIL_NATIVE_RAIDEN_FATALITY_2_SCENE_VICTIM:
      script = MK4_RETAIL_SCRIPT_RAIDEN_FATALITY_2_VICTIM;
      break;
    case RETAIL_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186:
      script = MK4_RETAIL_SCRIPT_TANYA_FATALITY_2_VICTIM_SCENE;
      break;
    case RETAIL_NATIVE_SONYA_FATALITY_2_SCENE_VICTIM:
      script = MK4_RETAIL_SCRIPT_SONYA_FATALITY_2_VICTIM;
      break;
    default:
      require(false, "Unexpected native scene reaction");
      return;
  }
  retail_vm_enter(fighter, script, fighter_ready);
}

/* Native467fc0 owns only the attacker's small rewind/advance cycle. */
static void stage_owner_forward(Fighter *fighter);
static void stage_owner_cycle(Fighter *fighter);

static void stage_owner_park(Fighter *fighter) {
  native_hold(fighter, 30, retail_task_pause);
}

static void stage_owner_reverse(Fighter *fighter) {
  native_play_rows(fighter, mini(6, fighter_move(fighter)->last), -1, stage_owner_forward);
}

static void stage_owner_forward(Fighter *fighter) {
  native_play_rows(fighter, mini(8, fighter_move(fighter)->last), 1, stage_owner_cycle);
}

static void stage_owner_cycle(Fighter *fighter) {
  if (--fighter->native_variant > 0) {
    stage_owner_reverse(fighter);
  } else {
    native_finish_clip(fighter, stage_owner_park);
  }
}

void retail_stage_owner(Fighter *fighter) {
  fighter->native_variant = 3;
  stage_owner_reverse(fighter);
}

/*4836e0 hands the victim to original4ea300. Actor motion and animation
 * remain script-owned, independently of the attacker's continuation. */
void retail_stage_victim(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_STAGE_VICTIM, retail_task_pause);
}

static bool stage_ceiling_reached(const Fighter *fighter) {
  return fighter->body.pos[AXIS_Y] <= -0x59999;
}

static void stage_impaled(Fighter *fighter) {
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_STAGE_IMPALED, retail_task_pause);
}

/*483720/48f5f0 waits for the original absolute ceiling height. */
void retail_stage_impale(Fighter *fighter) {
  native_await(fighter, stage_ceiling_reached, stage_impaled);
}

static void stage_complete(Fighter *fighter) {
  (void)fighter;
  fighter_cancel_all_mmoves();
  battle_end_finisher();
}

/*4837c0's result/input director is the portable battle coordinator. */
void retail_stage_complete(Fighter *fighter) {
  native_hold(fighter, 60, stage_complete);
}

/*481360: flip and bind the loaded held-victim descriptor. The portable
 * scene director already owns camera tracking and the later impact task. */
void retail_scene_victim_hold(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_0700};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  fighter->facing = -fighter->facing;
  retail_vm_bind_animation(fighter, mk4_image_u32(battle_data(), MK4_RETAIL_ANIMATION_DESCRIPTOR_SCENE_VICTIM_HOLD) * 4, 0);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
}

/*4891c0/489110 uses the shared bounded fragment pools and160-visit join. */
void retail_scene_explode(Fighter *fighter) {
  fighter->script_phase = 2;
  battle_shatter(fighter);
  battle_particles(fighter, 3, 20, false);
  native_hold(fighter, 160, stage_complete);
}

/*488d10's renderer-tree attachment is implicit in the engine actor registry. */
void retail_scene_park(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_task_pause(fighter);
}
