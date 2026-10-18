static void shinnok_sweep(Fighter *fighter) {
  shinnok_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void shinnok_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &shinnok_sweep_record);
  }
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_sweep);
}
