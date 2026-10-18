static void shinnok_high_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void shinnok_low_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void shinnok_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void shinnok_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_high_kick);
}

static void shinnok_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? shinnok_sweep : shinnok_low_kick);
}
