static void noob_saibot_script_air_throw_continue(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  retail_script_004e5ea0(fighter);
}

static void noob_saibot_script_air_throw_end(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_damage(victim, 0x30a3);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_lp_throw(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void noob_saibot_lk_throw(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void noob_saibot_air_throw(Fighter *fighter) {
  mk4_air_throw_generated_with_moves(fighter, &s_noob_air_throw, &s_noob_air_thrown, retail_script_004e5e78, retail_script_004e9910);
}
