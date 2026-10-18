static void kai_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de798);
}

static void kai_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de2f8);
}

static void kai_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void kai_script_thrown_return(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}
