#include "../character.h"

void noob_saibot_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &noob_saibot_s_uppercut);
  noob_saibot_entry_retail_script_004e62a0(fighter);
}
