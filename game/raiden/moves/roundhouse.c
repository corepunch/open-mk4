#include "../character.h"

void raiden_roundhouse(Fighter *fighter) {
  raiden_ground_attack(fighter, G_ROUNDHOUSE);
}

void raiden_script_roundhouse_tail(Fighter *fighter) {
  raiden_ground_native_next(fighter);
}

void raiden_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    raiden_entry_retail_script_004e6890(fighter);
  } else {
    raiden_entry_retail_script_004e68a8(fighter);
  }
}
