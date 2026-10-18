#include "../character.h"

static void reiko_high_kick(Fighter *fighter);
static void reiko_low_kick(Fighter *fighter);

void reiko_script_kick_finish(Fighter *fighter) {
  reiko_entry_retail_script_004e5c70(fighter);
}

static void reiko_high_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e5ca0);
}

static void reiko_low_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &reiko_s_kick, reiko_entry_retail_script_004e5d90);
}

void reiko_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_high_kick);
}

void reiko_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reiko_sweep : reiko_low_kick);
}

void reiko_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_sweep);
}
