static void jax_script_air_throw_ground_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_react(victim, 78);
  battle_damage(victim, 0x2147);
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(jax_ready);
}

static void jax_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de798);
}

static void jax_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void jax_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void jax_script_air_throw_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void jax_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_throw);
}
