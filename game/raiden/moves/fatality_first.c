#include "../character.h"

static void first_end_shake(Fighter *fighter);
static void first_end_particles(Fighter *fighter);

void raiden_script_first_release(Fighter *fighter) {
  mk4_script_scene_pose(raiden_opponent(fighter), 393, 25, 0);
  battle_particles(raiden_opponent(fighter), 2, 12, true);
  raiden_entry_retail_script_004e0220(fighter);
}

static void first_end_shake(Fighter *fighter) {
  mk4_script_shake(fighter, 8);
}

static void first_end_particles(Fighter *fighter) {
  battle_particles(raiden_opponent(fighter), 2, 24, true);
}

void raiden_script_first_end(Fighter *fighter) {
  first_end_shake(fighter);
  native_begin(fighter, 1, 60, NULL);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), frame == 30 ? first_end_shake : frame == 28 || frame == 58 ? first_end_particles : NULL);
  }
  native_wait(fighter);
  battle_shatter(raiden_opponent(fighter));
  battle_end_finisher();
  raiden_script_victory(fighter);
}

void raiden_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 392, 25, 0);
  mk4_script_scene_pose(raiden_opponent(fighter), 393, 25, 0);
  fighter_start_fiber(fighter, raiden_entry_retail_script_004e0210, NULL);
}
