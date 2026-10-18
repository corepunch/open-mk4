#include "../character.h"

static void noob_saibot_roundhouse(Fighter *fighter);

void noob_saibot_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    noob_saibot_entry_retail_script_004e6860(fighter);
  } else {
    noob_saibot_entry_retail_script_004e6878(fighter);
  }
}

static void noob_saibot_roundhouse(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &noob_saibot_s_kick, noob_saibot_entry_retail_script_004e6800);
}
