static void quan_chi_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void quan_chi_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void quan_chi_air_hp(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void quan_chi_air_lp(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void quan_chi_air_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e61f8);
}

static void quan_chi_air_diagonal_hk(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e6500);
}

static void quan_chi_air_diagonal_lk(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e63d8);
}

static void press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_lp);
}

static void press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? quan_chi_air_diagonal_hk : quan_chi_air_kick);
}

static void press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_diagonal_lk);
}
