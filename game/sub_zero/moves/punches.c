#include "../character.h"

void sub_zero_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
    return;
  }
  retail_high_punch(fighter);
}

void sub_zero_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sub_zero_hp);
}

/* Shared CPU selections use these native entries directly, as 45f5f0 does.
 * Human command patterns and held-button predicates do not gate drone actions. */
void sub_zero_cpu_close_hp(Fighter *fighter) {
  retail_close_high_punch(fighter);
}
