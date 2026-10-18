#include "../character.h"

void goro_lk_throw(Fighter *fighter) {
  goro_start_script(fighter, S_THROW, &goro_s_throw, goro_entry_retail_script_004de2f8);
}

void goro_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  fighter->script_phase = 0;
  mk4_mmove_throw_start(fighter, &goro_s_throw, &goro_s_knockdown, goro_entry_retail_script_004de798);
}

void goro_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, goro_entry_retail_script_004e5e28, goro_entry_retail_script_004e9910);
}
