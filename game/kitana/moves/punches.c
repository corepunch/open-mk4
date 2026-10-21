#include "../character.h"

void kitana_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
    return;
  }
  retail_high_punch(fighter);
}

void kitana_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : kitana_hp);
}

void kitana_cpu_close_hp(Fighter *fighter) {
  retail_close_high_punch(fighter);
}
