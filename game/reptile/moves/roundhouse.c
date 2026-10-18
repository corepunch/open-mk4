#include "../character.h"

static void reptile_roundhouse(Fighter *fighter);

void reptile_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    reptile_entry_retail_script_004e6860(fighter);
  } else {
    reptile_entry_retail_script_004e6878(fighter);
  }
}

static void reptile_roundhouse(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &reptile_s_kick, reptile_entry_retail_script_004e6800);
}
