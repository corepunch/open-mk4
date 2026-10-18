#include "../character.h"

void jarek_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    jarek_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, jarek_entry_retail_script_004e6950, NULL);
}
