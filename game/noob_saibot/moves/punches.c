#include "../character.h"

static void noob_saibot_high_punch(Fighter *fighter);
static void noob_saibot_jab(Fighter *fighter);

static void noob_saibot_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &noob_saibot_s_jab);
}

static void noob_saibot_jab(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &noob_saibot_s_jab, noob_saibot_entry_retail_script_004e62f0);
}

void noob_saibot_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_jab);
}

void noob_saibot_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? noob_saibot_lp_throw : noob_saibot_jab);
}

void noob_saibot_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_uppercut);
}
