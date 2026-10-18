#include "../character.h"

void tanya_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    tanya_entry_retail_script_004e6860(fighter);
  } else {
    tanya_entry_retail_script_004e6878(fighter);
  }
}

void tanya_roundhouse(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &tanya_s_kick, tanya_entry_retail_script_004e6800);
}
