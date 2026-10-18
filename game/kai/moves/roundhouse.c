#include "../character.h"

static void kai_roundhouse(Fighter *fighter);

static void kai_roundhouse(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &kai_s_high_kick, kai_entry_retail_script_004e6800);
}

void kai_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    kai_entry_retail_script_004e6860(fighter);
  } else {
    kai_entry_retail_script_004e6878(fighter);
  }
}
