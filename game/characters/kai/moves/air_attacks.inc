static void kai_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_hp);
}

static void kai_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_lp);
}

static void kai_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_kick);
}

static void kai_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_diagonal_lk);
}

static void kai_air_hp(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void kai_air_lp(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void kai_air_kick(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void kai_air_diagonal_hk(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void kai_air_diagonal_lk(Fighter *fighter) {
  kai_start_script(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void kai_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void kai_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}
