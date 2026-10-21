#include "native_services.h"

/* Native scenes retain independent victims, blends, particles and teardown.
 * Every VM playback/repeat below is a generated function, including result
 * poses that continue until the ordinary round/reset lifetime ends. */
void sonya_scene_end_watch(Fighter *fighter) {
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
