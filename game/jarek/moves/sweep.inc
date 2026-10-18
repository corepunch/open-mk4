static void jarek_sweep(Fighter *fighter) {
  jarek_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void jarek_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_sweep_record);
  }
  fighter_fiber_finish(jarek_ready);
}

static void jarek_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_sweep);
}
