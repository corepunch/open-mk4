#include "../character.h"

static void fujin_sweep(Fighter *fighter);
static void fujin_entry_sweep(Fighter *fighter);

static void fujin_sweep(Fighter *fighter) {
  fujin_start(fighter, S_SWEEP, &fujin_s_sweep, fujin_entry_retail_script_004e6798);
}

static void fujin_entry_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

void fujin_enter_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

void fujin_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_sweep);
}
