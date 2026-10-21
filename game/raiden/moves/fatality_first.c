#include "../character.h"

static void first_end_shake(Fighter *fighter);
static void first_end_particles(Fighter *fighter);
static void first_end_finish(Fighter *fighter);

void raiden_script_first_release(Fighter *fighter) {
  mk4_script_scene_pose(fighter_peer(fighter), 393, 0);
  battle_particles(fighter_peer(fighter), 2, 12, true);
  raiden_entry_retail_script_004e0220(fighter);
}

static void first_end_shake(Fighter *fighter) {
  mk4_script_shake(fighter, 8);
}

static void first_end_particles(Fighter *fighter) {
  battle_particles(fighter_peer(fighter), 2, 24, true);
}

static void first_end_finish(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  battle_end_finisher();
  raiden_script_victory(fighter);
}

void raiden_script_first_end(Fighter *fighter) {
  first_end_shake(fighter);
  native_begin(fighter, 1, 60, first_end_finish);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, fighter_sample(fighter), frame == 30 ? first_end_shake : frame == 28 || frame == 58 ? first_end_particles : NULL);
  }
  native_play(fighter, false);
}

void raiden_first_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_RAIDEN_1)) {
    return;
  }
  mk4_script_scene_pose(fighter, 392, 0);
  mk4_script_scene_pose(fighter_peer(fighter), 393, 0);
  raiden_entry_retail_script_004e0210(fighter);
}
