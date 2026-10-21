#include "retail_vm.h"
#include "../character.h"

static void finish_scene(Fighter *fighter);
static void burn_start(Fighter *fighter);
static void burn_tail_done(Fighter *victim);
static void beast_start(Fighter *fighter);
static void beast_watch(Fighter *fighter);
static void beast_gore_object(Projectile *object);
static const SceneClip burn_repeat_blend[] = {BLEND(390, 105)};
static const SceneClip burn_victory_blend[] = {BLEND(115, 0)};
static const SceneClip burn_victim_blend[] = {BLEND(206, 0)};

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void burn_start(Fighter *fighter) {
  int frames = 8 - g_battle.phase_ticks;
  if (frames > 0) {
    native_hold(fighter, frames, NULL);
    native_script_end(fighter, 0x4e0278, fighter_ready, 0);
  } else {
    retail_vm_enter_owned(fighter, 0x4e0278, fighter_ready, 0);
  }
}

static void beast_start(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2e58, fighter_ready, 0);
}

void scorpion_start_fatality(Fighter *fighter, int id) {
  require(id == FINISH_BURN || id == FINISH_BEAST, "Invalid native scene entry");
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  retail_scene_pose(victim, 13, 0, false);
  retail_scene_pose(fighter, id == FINISH_BEAST ? 12 : 390, 0, true);
  FighterFn start = id == FINISH_BURN ? burn_start : beast_start;
  start(fighter);
}

void scorpion_burn_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BURN);
}

void scorpion_beast_fatality(Fighter *fighter) {
  scorpion_start_fatality(fighter, FINISH_BEAST);
}

static void burn_tail_done(Fighter *victim) {
  fighter_finish_mmove(victim, mk4_script_thrown_ready);
}

void scorpion_script_burn_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_finish_clip(victim, burn_tail_done);
}

void scorpion_script_burn_begin(Fighter *fighter) {
  battle_sound(SOUND_B, 14);
  battle_sound(SOUND_B, 46);
  retail_vm_enter_owned(fighter, 0x4e0288, fighter_ready, 0);
}

void scorpion_script_burn_victim_start(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  victim->appearance = APPEARANCE_BURNED;
  fighter_cancel_mmove(victim);
  retail_vm_enter_owned(victim, 0x4e0300, fighter_ready, 0);
  battle_sound(SOUND_B, 15);
  retail_vm_enter_owned(fighter, 0x4e0298, fighter_ready, 0);
}

void scorpion_script_burn_repeat1(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_repeat_blend, NULL);
  native_script_end(fighter, 0x4e02a8, fighter_ready, 0);
}

void scorpion_script_burn_repeat2(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_repeat_blend, NULL);
  native_script_end(fighter, 0x4e02b8, fighter_ready, 0);
}

void scorpion_script_burn_victory(Fighter *fighter) {
  scorpion_native_blend(fighter, burn_victory_blend, NULL);
  native_script_end(fighter, 0x4e02d0, fighter_ready, 0);
}

void scorpion_script_burn_victim_cue(Fighter *victim) {
  battle_sound(SOUND_B, 16);
  retail_vm_enter_owned(victim, 0x4e0310, fighter_ready, 0);
}

void scorpion_script_burn_victim_repeat(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4e0320, fighter_ready, 0);
}

void scorpion_script_burn_victim_blend(Fighter *victim) {
  scorpion_native_blend(victim, burn_victim_blend, NULL);
  native_script_end(victim, 0x4e0338, fighter_ready, 0);
}

void scorpion_script_burn_victim_impact(Fighter *victim) {
  mk4_script_shake(victim, 0);
  battle_particles(victim, 0, 4, true);
  retail_vm_enter_owned(victim, 0x4e0348, fighter_ready, 0);
}

void scorpion_script_burn_victim_settle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  retail_vm_enter_owned(victim, 0x4e0358, fighter_ready, 0);
}

void scorpion_script_burn_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, finish_scene);
}

/* Both scene actors have parked their frame programs; the parked poses
 * remain while the finisher completes. */
static void beast_watch(Fighter *fighter) {
  if (fighter->script_phase == 9 && fighter_peer(fighter)->script_phase == 9) {
    battle_end_finisher();
  }
}

void scorpion_script_beast_begin(Fighter *fighter) {
  fighter->appearance = APPEARANCE_BEAST;
  fighter->body.physics = beast_watch;
  retail_vm_enter_owned(fighter, 0x4d2e88, fighter_ready, 0);
}

void scorpion_script_beast_attack(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  fighter_cancel_mmove(victim);
  retail_vm_enter_owned(victim, 0x4d2ef8, fighter_ready, 0);
  retail_vm_enter_owned(fighter, 0x4d2eb0, fighter_ready, 0);
}

void scorpion_script_beast_victim_pieces(Fighter *victim) {
  battle_particles(victim, 0, 12, true);
  retail_vm_enter_owned(victim, 0x4d2f10, fighter_ready, 0);
}

void scorpion_script_beast_voice(Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

void scorpion_script_beast_park(Fighter *fighter) {
  fighter->script_phase = 9;
  native_begin(fighter, 0, 0, NULL);
  fighter->native.frames[0].thinkfunc = mk4_mmove_hold;
  native_play(fighter, false);
}

static void beast_gore_object(Projectile *object) {
  Fighter *victim = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 4 == 0) {
    battle_particles(victim, 0, 2, true);
  }
}

void scorpion_script_beast_gore(Fighter *victim, int type) {
  require(type == 0, "Unexpected beast gore task");
  battle_spawn_projectile(victim, PROJECTILE_EFFECT, 60, beast_gore_object);
}
