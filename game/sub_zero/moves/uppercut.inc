/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void sub_zero_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

/* Phase branches are ordinary C; blocking playback owns only samples/events. */

static void uppercut_branch(Fighter *fighter) {
  static const int cycle[] = {8, 7, 6, 7, 8};
  static const int late[] = {9, 9, 8, 7, 6, 7, 8};
  int repeats = fighter->contact == CONTACT_MISS ? 3 : fighter->contact == CONTACT_BLOCK ? 2 : 8;
  int recovery = fighter->contact == CONTACT_MISS ? 8 : 9;
  bool delayed = fighter->contact_tick == 9;
  int count = repeats * 5 + (delayed ? 2 : 0) + 20 - recovery;
  native_begin(fighter, 0, count - 1, sub_zero_ready);
  int row = 0;
  for (int repeat = 0; repeat < repeats; ++repeat) {
    const int *poses = delayed && repeat == 0 ? late : cycle;
    int length = delayed && repeat == 0 ? 7 : 5;
    for (int frame = 0; frame < length; ++frame) {
      fighter->native.poses[row++] = poses[frame];
    }
  }
  for (int pose = recovery; pose < 20; ++pose) {
    fighter->native.poses[row++] = pose;
  }
  native_play(fighter, false);
}

static void uppercut_contact(Fighter *fighter) {
  fighter->sample_override = fighter_sample(fighter) + 1;
  sub_zero_uppercut_contact(fighter);
}

static void uppercut_script(Fighter *fighter) {
  hold_phase(fighter, &uppercut_contact_phase);
}

static void sub_zero_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  retail_script_004e62a0(fighter);
}

static void enter_uppercut(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}
