#include "../character.h"

void kitana_roundhouse(Fighter *fighter) {
  kitana_ground_attack(fighter, G_ROUNDHOUSE);
}

void kitana_script_roundhouse_tail(Fighter *fighter) {
  kitana_ground_native_next(fighter);
}

void kitana_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    kitana_entry_retail_script_004e6860(fighter);
  } else {
    kitana_entry_retail_script_004e6878(fighter);
  }
}
