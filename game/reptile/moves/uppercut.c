#include "../character.h"

static void reptile_uppercut(Fighter *fighter);

static void reptile_uppercut(Fighter *fighter) {
  reptile_start(fighter, S_UPPERCUT, &reptile_s_uppercut, reptile_entry_retail_script_004e62a0);
}

void reptile_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_uppercut);
}
