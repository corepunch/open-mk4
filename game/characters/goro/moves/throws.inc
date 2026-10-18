void goro_lk_throw(Fighter *fighter) {
  goro_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void goro_lp_throw(Fighter *fighter) {
  fighter->script_phase = 0;
  goro_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void goro_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}
