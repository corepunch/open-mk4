static void jarek_uppercut(Fighter *fighter) {
  jarek_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void jarek_uppercut_frame(Fighter *fighter) {
  const AttackRecord *record = (fighter->flags & FF_FOLLOW_UP) ? &jarek_uppercut_followup_record : &jarek_uppercut_record;
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001 && battle_contact(fighter, record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

static void jarek_script_uppercut_finish(Fighter *fighter) {
  native_begin(fighter, 1, 4, NULL);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, frame == 1 ? fighter_sample(fighter) : frame + 4, frame >= 2 ? jarek_uppercut_frame : NULL);
  }
  native_wait(fighter);
  fighter_fiber_finish(jarek_ready);
}

static void jarek_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_uppercut);
}
