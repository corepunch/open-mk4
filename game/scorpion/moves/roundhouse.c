#include "../character.h"

void scorpion_roundhouse(Fighter *fighter) {
  scorpion_ground_attack(fighter, G_ROUNDHOUSE);
}

void scorpion_script_roundhouse_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  scorpion_ground_native_tail(fighter);
}

void scorpion_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    scorpion_entry_retail_script_004e6860(fighter);
  } else {
    scorpion_entry_retail_script_004e6878(fighter);
  }
}
