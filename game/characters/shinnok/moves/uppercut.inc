static void shinnok_uppercut(Fighter *fighter) {
  shinnok_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void shinnok_uppercut_frame(Fighter *fighter) {
  const AttackRecord *record = (fighter->flags & FF_FOLLOW_UP) ? &shinnok_uppercut_followup_record : &shinnok_uppercut_record;
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001 && battle_contact(fighter, record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

static void shinnok_script_uppercut_finish(Fighter *fighter) {
  native_begin(fighter, 1, 4, NULL);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, frame == 1 ? fighter_sample(fighter) : frame + 4, frame >= 2 ? shinnok_uppercut_frame : NULL);
  }
  native_wait(fighter);
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_uppercut);
}
