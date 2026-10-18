static void liu_kang_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void liu_script_roundhouse_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void liu_script_roundhouse_start(Fighter *fighter) {
  fighter->native_followup = fighter_fast_start(fighter);
  if (fighter->native_followup) {
    retail_script_004e6890(fighter);
  } else {
    retail_script_004e68a8(fighter);
  }
}
