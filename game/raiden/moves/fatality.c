#include "native_services.h"

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
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 1);
}

void raiden_script_victory_effect(Fighter *fighter) {
  battle_particles(fighter, 10, 4, true);
  battle_particles(fighter, 13, 4, true);
}
