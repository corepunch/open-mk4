static void quan_chi_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void quan_chi_roundhouse(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e6800);
}
