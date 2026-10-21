#include "../character.h"

void liu_kang_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    retail_close_high_punch(fighter);
    return;
  }
  retail_high_punch(fighter);
}

void liu_kang_press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : liu_kang_hp);
}

/* 438e50/60/70/80 select 4df8e8/928/968/9a8[3]:
 * bicycle, low fireball, flying kick, low fireball. */
void liu_kang_cpu_close_hp(Fighter *fighter) {
  retail_close_high_punch(fighter);
}
