static void fujin_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void fujin_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_lp_thrown, retail_script_004de798);
}

static void fujin_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void fujin_enter_throw(Fighter *fighter) {
  fujin_lp_throw(fighter);
}
