static void fujin_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void fujin_roundhouse(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void fujin_entry_roundhouse(Fighter *fighter) {
  fujin_roundhouse(fighter);
}

static void fujin_press_roundhouse(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_roundhouse);
}
