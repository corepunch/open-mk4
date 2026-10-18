static void jarek_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, script);
}

static void jarek_lp_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, retail_script_004de798);
}

static void jarek_lk_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, retail_script_004de2f8);
}

static void jarek_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void jarek_throw_reaction(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static void jarek_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_throw);
}
