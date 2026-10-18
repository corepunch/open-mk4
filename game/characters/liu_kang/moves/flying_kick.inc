static void flying_query(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter) && battle_contact(fighter, &flying_record) == CONTACT_MISS;
}

static void flying_kick_script(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  if (fighter_airborne(fighter) && battle_contact(fighter, &flying_record) == CONTACT_MISS) {
    native_begin(fighter, 0, 0, NULL);
    fighter->native.frames[0].thinkfunc = flying_query;
    native_wait(fighter);
  }
  if (!fighter_airborne(fighter)) {
    retail_script_004e86d0(fighter);
  } else if (fighter->contact == CONTACT_BLOCK) {
    retail_script_004e8718(fighter);
  } else {
    retail_script_004e86e8(fighter);
  }
}

static void liu_kang_flying_kick(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_flying_kick);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, 0x23d7);
  fighter->body.vel[AXIS_Y] = -0x170a;
  fighter->body.gravity = 0x106;
  battle_sound(SOUND_INDEXED, 0);
  fighter_start_fiber(fighter, retail_script_004e86a8, NULL);
}
