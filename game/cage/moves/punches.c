#include "../character.h"

static void cage_jab(Fighter *fighter);
void cage_powered_punch(Fighter *fighter);
static void cage_high_punch(Fighter *fighter);

static void cage_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &cage_s_jab);
  cage_entry_retail_script_004e62f0(fighter);
}

static void cage_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    cage_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

void cage_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : cage_high_punch);
}

void cage_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? cage_lp_throw : cage_jab);
}

void cage_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_uppercut);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */
