#include "../character.h"

static void reiko_high_punch(Fighter *fighter);
static void reiko_jab(Fighter *fighter);

static void reiko_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    reiko_close_hp(fighter);
  } else {
    reiko_jab(fighter);
  }
}

static void reiko_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &reiko_s_jab);
  reiko_entry_retail_script_004e62f0(fighter);
}

void reiko_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : reiko_high_punch);
}

void reiko_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? reiko_lp_throw : reiko_jab);
}

void reiko_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_uppercut);
}
