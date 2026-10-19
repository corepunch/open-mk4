#include "../character.h"

void sonya_roundhouse(Fighter *fighter) {
  sonya_ground_attack(fighter, G_ROUNDHOUSE);
}

void sonya_script_roundhouse_tail(Fighter *fighter) {
  sonya_ground_native_next(fighter);
}

void sonya_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    sonya_entry_retail_script_004e6860(fighter);
  } else {
    sonya_entry_retail_script_004e6878(fighter);
  }
}
