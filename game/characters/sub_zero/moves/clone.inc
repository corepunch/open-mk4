static void clone_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (g_battle.contacts_off || battle_contact_from(owner, &clone_record, p->pos, p->facing) != CONTACT_MISS) {
    p->kind = 0;
  }
}

static void clone_cleanup(Fighter *fighter) {
  fighter_upright(fighter);
}

static void clone_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, air_land);
}

static void clone_land_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void clone_land(Fighter *fighter) {
  fighter->sample_override = 0;
  if (fighter_airborne(fighter)) {
    fighter_mmove_phase(fighter, &clone_land_phase);
  } else {
    clone_done(fighter);
  }
}

static void clone_turn(Fighter *fighter) {
  memset(fighter->body.angle, 0, sizeof fighter->body.angle);
  memset(fighter->body.spin, 0, sizeof fighter->body.spin);
  fighter->script_phase = 1;
  fighter_mmove_phase(fighter, &clone_reverse);
}

static bool clone_falling(const Fighter *fighter) {
  return fighter->body.vel[AXIS_Y] >= 0;
}

static void clone_apex_frames_frame_0(Fighter *fighter) {
  fighter->mmove_hold = !clone_falling(fighter);
}

static void clone_apex_wait(Fighter *fighter) {
  fighter_mmove_phase(fighter, &clone_apex);
}

static void clone_spin(Fighter *fighter) {
  fighter->body.root_y += 0x3333;
  fighter->body.pos[AXIS_Y] -= 0x3333;
  fighter->body.spin[AXIS_X] = -0x6147;
  fighter_mmove_phase(fighter, &clone_extend);
}

static bool can_clone(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->clone_ready && fighter_opponent(fighter)->action != ACTION_RETAIL_1003 &&
         !owned_object(fighter, PROJECTILE_CLONE);
}

static void sub_zero_clone(Fighter *fighter) {
  bool near = fighter_distance(fighter) < 1.f;
  if (!near) {
    Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_CLONE, fighter_opponent(fighter)->human ? 74 : 53, clone_object);
    if (p != NULL) {
      p->clip = fighter_move(fighter)->clip;
      p->sample = fighter_pose_sample(fighter);
      battle_object_light(p, &clone_light);
    }
  }
  stop_body(fighter);
  fighter_start_move(fighter, S_JUMP, near ? &s_clone_near : &s_clone);
  fighter->sample_override = 0;
  fighter_set_speed(fighter, -0x170a);
  fighter->body.vel[AXIS_Y] = -0x28f5;
  fighter->body.gravity = 0x28f;
  fighter->clone_ready = g_battle.ticks + 180;
  battle_sound(SOUND_C, 0xa);
  fighter->mmove_cleanup = clone_cleanup;
  fighter_mmove_phase(fighter, &clone_start);
}
