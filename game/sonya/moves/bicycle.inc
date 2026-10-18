static bool bicycle_height(const Fighter *fighter) {
  return fighter_height(fighter) >= 2.5f;
}

static void bicycle_contact(Fighter *fighter) {
  if (battle_contact(fighter, &bicycle_record) != CONTACT_MISS) {
    fighter_stop_mmove(fighter);
  }
}

static void bicycle_script(Fighter *fighter) {
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = -0x2666;
  battle_sound(SOUND_A, 6);
  battle_sound(SOUND_INDEXED, 12);
  native_until(fighter, bicycle_height);
  stop_body(fighter);
  mk4_script_bind(fighter, 101, 9, 0);
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 0, 8, NULL);
  for (int frame = 0; frame < 9; ++frame) {
    native_frame(fighter, frame, frame, bicycle_contact);
  }
  native_wait(fighter);
  fighter_set_speed(fighter, 0);
  fighter->body.gravity = 0x28f;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(sonya_ready);
}

static void sonya_bicycle(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_bicycle);
  fighter_start_fiber(fighter, bicycle_script, special_cleanup);
}
