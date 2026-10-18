#include "../character.h"

void goro_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    goro_entry_retail_script_004e6860(fighter);
  } else {
    goro_entry_retail_script_004e6878(fighter);
  }
}

void goro_roundhouse(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &goro_s_kick, goro_entry_retail_script_004e6800);
}
