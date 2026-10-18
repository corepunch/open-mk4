#include "../character.h"

void shinnok_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    shinnok_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, shinnok_entry_retail_script_004e6950, NULL);
}
