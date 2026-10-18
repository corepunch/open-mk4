#include "../character.h"

static void cage_roundhouse(Fighter *fighter);

void cage_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    cage_entry_retail_script_004e6860(fighter);
  } else {
    cage_entry_retail_script_004e6878(fighter);
  }
}

static void cage_roundhouse(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &cage_s_kick, cage_entry_retail_script_004e6800);
}
