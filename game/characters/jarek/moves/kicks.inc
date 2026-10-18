static void jarek_high_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void jarek_low_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void jarek_kick_frame(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &jarek_high_kick_record);
  }
}

static void jarek_script_kick_finish(Fighter *fighter) {
  native_calls(fighter, 2, 1, jarek_kick_frame);
  retail_script_004e5c70(fighter);
}

static void jarek_script_low_kick_avoid(Fighter *fighter) {
  fighter->avoids_high = true;
}

static void jarek_script_low_kick_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}

static void jarek_script_low_kick_finish(Fighter *fighter) {
  fighter->avoids_high = false;
  fighter_fiber_finish(jarek_ready);
}

static void jarek_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_high_kick);
}

static void jarek_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? jarek_sweep : jarek_low_kick);
}
