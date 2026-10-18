static void jax_roundhouse(Fighter *fighter) {
  jax_start(fighter, S_KICK, &s_roundhouse, retail_script_004e6800);
}

static void jax_script_roundhouse_start(Fighter *fighter) {
  fighter->native_followup = fighter_fast_start(fighter);
  if (fighter->native_followup) {
    retail_script_004e6890(fighter);
  } else {
    retail_script_004e68a8(fighter);
  }
}

static void jax_script_roundhouse_tail(Fighter *fighter) {
  fighter->script_phase = 7;
  native_ground_tail(fighter, &jax_ground_profile);
}
