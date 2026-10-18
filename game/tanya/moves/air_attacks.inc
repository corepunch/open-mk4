static void tanya_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void tanya_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void tanya_air_hp(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void tanya_air_lp(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void tanya_air_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

void tanya_air_diagonal_hk(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void tanya_air_diagonal_lk(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void tanya_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_hp);
}

static void tanya_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_lp);
}

static void tanya_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? tanya_air_diagonal_hk : tanya_air_kick);
}

static void tanya_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_diagonal_lk);
}
