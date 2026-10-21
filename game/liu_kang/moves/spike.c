#include "../character.h"

static void liu_spike_finish(Fighter *fighter);

static void liu_spike_finish(Fighter *fighter) {
  liu_kang_fatal_pose(fighter, 119, 0, true);
  liu_kang_finish_scene(fighter);
}

void liu_kang_liu_script_spike_tail(Fighter *fighter) {
  mk4_native_spike_tail(fighter, liu_spike_finish);
}

void liu_kang_spike_fatality(Fighter *fighter) {
  liu_kang_start_fatality(fighter, FINISH_SPIKE);
}
