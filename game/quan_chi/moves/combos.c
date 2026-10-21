#include "../character.h"

void quan_chi_press_air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, retail_close_high_punch);
  } else {
    quan_chi_press_high_punch(fighter);
  }
}
