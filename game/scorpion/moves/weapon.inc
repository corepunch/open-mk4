static void equip_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &sword);
}

static void script_weapon_ready(Fighter *fighter) {
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_finish(fighter_ready);
  }
  fighter_finish_mmove(fighter, fighter_ready);
}

static void sword_slash_recover(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = fighter->body.vel[AXIS_Z] = 0;
  fighter->action = ACTION_RETAIL_1006;
}

static void sword_slash_query(Fighter *fighter) {
  if (!fighter->connected && battle_contact(fighter, &sword_records[0]) != CONTACT_MISS) {
    sword_slash_recover(fighter);
    sword_slash_build(fighter, fighter->mmove_frame, true);
  }
}

static void sword_slash_build(Fighter *fighter, int first, bool hit) {
  int last = hit ? 42 : 43;
  native_begin(fighter, first, last, special_ready);
  for (int frame = first; frame <= last; ++frame) {
    FighterFn action = !hit && frame >= 17 && frame < 21 ? sword_slash_query : !hit && frame == 21 ? sword_slash_recover : NULL;
    native_frame(fighter, frame, frame + hit, action);
  }
  native_play(fighter, hit);
}

static void sword_attack(Fighter *fighter, bool hp, bool back) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, hp, (hp) ? retail_script_004e66d0 : retail_script_004e66f0);
    return;
  }
  const MoveDef *move = back ? (hp ? &s_sword_back_hp : &s_sword_back_lp) : (hp ? &s_sword_hp : &s_sword_lp);
  fighter_start_move(fighter, S_PUNCH, move);
  fighter->sample_override = 0;
  if (back) {
    (hp ? retail_script_004ec918 : retail_script_004ec8b0)(fighter);
  } else {
    sword_slash_build(fighter, 1, false);
  }
}

static void sword_hp(Fighter *fighter) {
  sword_attack(fighter, true, false);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, false, false);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, true, true);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, false, true);
}

static void sword_spin_recover(Fighter *fighter) {
  stop_body(fighter);
  fighter->script_phase = 1;
  native_hold(fighter, 12, special_ready);
}

static void sword_spin_control(Fighter *fighter) {
  if (fighter_opponent(fighter)->hits >= 3) {
    sword_spin_recover(fighter);
    return;
  }
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    fighter_finish_mmove(fighter, special_ready);
    return;
  }
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  fighter->body.vel[AXIS_X] = direction * 0xf5c;
}

static void sword_spin_query(Fighter *fighter) {
  if (battle_contact(fighter, &sword_records[3]) == CONTACT_BLOCK) {
    sword_spin_recover(fighter);
  } else {
    sword_spin_control(fighter);
  }
}

static void sword_spin(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_spin_start);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = stop_body;
  retail_script_004e8850(fighter);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void scorpion_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_sword(fighter);
    weapon_followup_victim(fighter);
    fighter_start_move(fighter, S_PUNCH, &s_sword_back_lp);
    fighter->sample_override = 0;
    retail_script_004ec8b0(fighter);
  } else {
    weapon_draw(fighter, &sword);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
  fighter->sample_override = 0;
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void scorpion_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

static void script_draw_equip(Fighter *fighter) {
  equip_sword(fighter);
  retail_script_004e62d8(fighter);
}
