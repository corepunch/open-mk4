#include "../character.h"

void goro_lk_throw(Fighter *fighter) {
  goro_start_script(fighter, S_THROW, &goro_s_throw, goro_entry_retail_script_004de2f8);
}

void goro_lp_throw(Fighter *fighter) {
  fighter->script_phase = 0;
  goro_start_script(fighter, S_THROW, &goro_s_throw, goro_entry_retail_script_004de798);
}

void goro_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, goro_entry_retail_script_004e5e28, goro_entry_retail_script_004e9910);
}
