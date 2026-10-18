#include "../character.h"

static void quan_chi_roundhouse(Fighter *fighter);

void quan_chi_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    quan_chi_entry_retail_script_004e6860(fighter);
  } else {
    quan_chi_entry_retail_script_004e6878(fighter);
  }
}

static void quan_chi_roundhouse(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &quan_chi_s_high_kick, quan_chi_entry_retail_script_004e6800);
}
