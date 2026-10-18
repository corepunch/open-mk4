static void slide_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
  stop_body(fighter);
}

static void slide_contact(Fighter *fighter) {
  fighter->sample_override = 9;
  ContactResult contact = battle_contact(fighter, &slide_record);
  if (contact == CONTACT_MISS) {
    return;
  }
  fighter->script_phase = 1;
  if (contact == CONTACT_BLOCK) {
    retail_script_004e8530(fighter);
  } else {
    fighter->slide_ready = g_battle.ticks + 64;
    retail_script_004e84f8(fighter);
  }
}

static void slide_script(Fighter *fighter) {
  fighter_mmove_phase(fighter, &slide_contact_phase);
}

static bool can_slide(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && (fighter->held & mask) == chord;
}

static void sub_zero_slide(Fighter *fighter) {
  stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_slide);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = slide_cleanup;
  retail_script_004e84d8(fighter);
}

static void script_breath_slide(Fighter *fighter) {
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &breath_slide_phase);
}

static void script_slide_return(Fighter *fighter) {
  stop_body(fighter);
  fighter->script_phase = 2;
  fighter_mmove_phase(fighter, &slide_return_phase);
}
