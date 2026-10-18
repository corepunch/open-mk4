static void cage_script_cpu_kick_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 0;
}

static void cage_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void cage_shadow_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter->sample_override = 27;
  fighter_start_fiber(fighter, retail_script_004e5ba8, NULL);
}

static void cage_cpu_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_cpu_kick_script, retail_script_004e80a8);
}

static void cage_high_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void cage_low_kick(Fighter *fighter) {
  cage_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void cage_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, cage_high_kick);
}

static void cage_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? cage_sweep : cage_low_kick);
}

static void cage_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, cage_sweep);
}
