static void reptile_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void reptile_roundhouse(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e6800);
}
