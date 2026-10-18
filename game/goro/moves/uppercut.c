#include "../character.h"

static void goro_uppercut(Fighter *fighter);

static void goro_uppercut(Fighter *fighter) {
  goro_start_script(fighter, S_UPPERCUT, &goro_s_uppercut, goro_entry_retail_script_004e62a0);
}

void goro_enter_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, goro_uppercut);
}
