#include "../character.h"

static void noob_saibot_high_punch(Fighter *fighter);
static void noob_saibot_jab(Fighter *fighter);

static void noob_saibot_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    noob_saibot_close_hp(fighter);
  } else {
    noob_saibot_jab(fighter);
  }
}

static void noob_saibot_jab(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &noob_saibot_s_jab, noob_saibot_entry_retail_script_004e62f0);
}

void noob_saibot_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_high_punch);
}

void noob_saibot_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? noob_saibot_lp_throw : noob_saibot_jab);
}

void noob_saibot_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_uppercut);
}
