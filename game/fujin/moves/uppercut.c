#include "../character.h"

static void fujin_uppercut(Fighter *fighter);
static void fujin_enter_uppercut(Fighter *fighter);

static void fujin_uppercut(Fighter *fighter) {
  fujin_start(fighter, S_UPPERCUT, &fujin_s_uppercut, fujin_entry_retail_script_004e62a0);
}

static void fujin_enter_uppercut(Fighter *fighter) {
  fujin_uppercut(fighter);
}

void fujin_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_uppercut);
}
