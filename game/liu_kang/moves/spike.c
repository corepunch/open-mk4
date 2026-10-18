#include "../character.h"

static void liu_spike_finish(Fighter *fighter);

bool liu_kang_can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && liu_kang_can_close_fatality(fighter);
}

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
