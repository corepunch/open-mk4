static void jarek_air_hp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void jarek_air_lp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void jarek_air_kick(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void jarek_air_diagonal_hk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void jarek_air_diagonal_lk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void jarek_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void jarek_script_air_finish(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(mk4_land);
}

static void jarek_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_hp);
}

static void jarek_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_lp);
}

static void jarek_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_kick);
}
