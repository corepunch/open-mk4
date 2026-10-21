#include "retail_native.h"
#include "retail_vm.h"
#include "mk4_characters.h"
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
