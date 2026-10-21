#include "../character.h"

void raiden_arena_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    fighter_ready(fighter);
    return;
  }
  mk4_script_scene_pose(fighter, id == FINISH_FAN ? 415 : 293, id == FINISH_FAN ? 130 : 20, 0);
  mk4_script_scene_pose(raiden_opponent(fighter), id == FINISH_FAN ? 416 : 13, id == FINISH_FAN ? 108 : 56, 0);
  (id == FINISH_FAN ? retail_fan_start : raiden_spike_start)(fighter);
}

/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
void raiden_scene_end_watch(Fighter *fighter) {
  int clip = fighter_move(fighter)->clip;
  if (g_battle.phase != PHASE_FATALITY) {
    fighter->body.physics = NULL;
    return;
  }
  if (fighter_sample(fighter) == fighter_move(fighter)->last && (clip == 573 || clip == 388 || clip == 114)) {
    fighter->body.physics = NULL;
    battle_end_finisher();
  }
}

void raiden_script_victory(Fighter *fighter) {
  raiden_entry_retail_script_004e6a40(fighter);
}

void raiden_script_victory_effect(Fighter *fighter) {
  battle_particles(fighter, 10, 4, true);
  battle_particles(fighter, 13, 4, true);
}

void raiden_enter_victory(Fighter *fighter) {
  fighter_start_move(fighter, S_VICTORY, &raiden_s_victory);
  raiden_script_victory(fighter);
}
