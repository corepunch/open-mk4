#include "../character.h"

void liu_kang_roundhouse(Fighter *fighter) {
  liu_kang_ground_attack(fighter, G_ROUNDHOUSE);
}

void liu_kang_liu_script_roundhouse_tail(Fighter *fighter) {
  liu_kang_ground_native_tail(fighter);
}

void liu_kang_liu_script_roundhouse_start(Fighter *fighter) {
  fighter->native_followup = fighter_fast_start(fighter);
  if (fighter->native_followup) {
    liu_kang_entry_retail_script_004e6890(fighter);
  } else {
    liu_kang_entry_retail_script_004e68a8(fighter);
  }
}
