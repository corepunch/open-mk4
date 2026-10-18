#include "../character.h"

static void reiko_roundhouse(Fighter *fighter);

void reiko_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    reiko_entry_retail_script_004e6860(fighter);
  } else {
    reiko_entry_retail_script_004e6878(fighter);
  }
}

static void reiko_roundhouse(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e6800);
}
