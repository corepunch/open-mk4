static void jax_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void jax_air_hp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void jax_air_lp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void jax_air_kick(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void jax_air_diagonal_hk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void jax_air_diagonal_lk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void jax_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_hp);
}

static void jax_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_lp);
}

static void jax_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_kick);
}
