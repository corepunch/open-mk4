static void cage_script_powered_punch_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 1;
}

static void cage_script_powered_punch_finish_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 2;
}

static void cage_jab(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void cage_powered_punch(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &s_powered_punch_script, retail_script_004e80d0);
}

static void cage_high_punch(Fighter *fighter) {
  cage_powered_punch(fighter);
}

static void cage_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, cage_jab);
}

static void cage_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? cage_lp_throw : cage_jab);
}

static void cage_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_uppercut);
}
