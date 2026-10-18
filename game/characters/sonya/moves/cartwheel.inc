static void script_cartwheel_tail(Fighter *fighter) {
  fighter->cartwheel_ready = g_battle.ticks + 48;
  native_queries(fighter, &cartwheel_record, 9, 1);
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    native_delay(fighter, 20);
  }
  fighter_fiber_finish(sonya_ready);
}

static void sonya_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &s_cartwheel);
  fighter_start_fiber(fighter, retail_script_004e81d0, special_cleanup);
}

static bool can_cartwheel(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->cartwheel_ready;
}
