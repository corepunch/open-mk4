#include "../character.h"

void sub_zero_roundhouse(Fighter *fighter) {
  sub_zero_ground_attack(fighter, G_ROUNDHOUSE);
}

void sub_zero_script_roundhouse_tail(Fighter *fighter) {
  native_ground_next(fighter, &sub_zero_ground_profile);
}

void sub_zero_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    sub_zero_entry_retail_script_004e6860(fighter);
  } else {
    sub_zero_entry_retail_script_004e6878(fighter);
  }
}
