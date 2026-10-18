static void jarek_roundhouse(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_roundhouse, retail_script_004e6800);
}

static void jarek_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void jarek_script_roundhouse_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_roundhouse_record);
  }
  fighter_fiber_finish(jarek_ready);
}
