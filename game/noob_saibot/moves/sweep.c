#include "../character.h"

void noob_saibot_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &noob_saibot_s_sweep);
  noob_saibot_entry_retail_script_004e6798(fighter);
}
