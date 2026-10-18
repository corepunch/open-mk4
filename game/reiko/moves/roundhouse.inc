static void reiko_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void reiko_roundhouse(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}
