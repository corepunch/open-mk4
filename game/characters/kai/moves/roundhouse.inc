static void kai_roundhouse(Fighter *fighter) {
  kai_start_script(fighter, S_KICK, &s_high_kick, retail_script_004e6800);
}

static void kai_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}
