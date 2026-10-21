#include "../character.h"

void scorpion_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
    return;
  }
  retail_high_punch(fighter);
}

void scorpion_press_jab(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : scorpion_hp;
  fighter_interrupt(fighter, fighter->armed ? entry : scorpion_hp);
}
