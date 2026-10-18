static void sub_zero_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  retail_script_004e62f0(fighter);
}

static void sub_zero_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter->script_phase = 0;
  hp_script(fighter);
}

static void sub_zero_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void press_jab(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : sub_zero_hp);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, sub_zero_combo_hp);
  }
}

static void press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, enter_uppercut);
  } else {
    press_jab(fighter);
  }
}

static void script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_combo_hp_tail(Fighter *fighter) {
  native_ground_next(fighter, &ground_profile);
}

/* Shared CPU selections use these native entries directly, as 45f5f0 does.
 * Human command patterns and held-button predicates do not gate drone actions. */
static void cpu_close_hp(Fighter *fighter) {
  ground_attack(fighter, G_CLOSE_HP);
}
