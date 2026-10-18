static void equip_weapon(Fighter *fighter) {
  fighter_equip_weapon(fighter, &native_weapon);
}

static void weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, true, retail_script_004e66d0);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[0], true);
  }
}

static void weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, false, retail_script_004e66f0);
  } else {
    weapon_start_strike(fighter, &weapon_attacks[1], true);
  }
}

static void weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[2], true);
}

static void weapon_back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &weapon_attacks[3], true);
}

static void script_draw_equip(Fighter *fighter) {
  equip_weapon(fighter);
  retail_script_004e62d8(fighter);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void raiden_weapon(Fighter *fighter) {
  if (fighter->armed) {
    weapon_throw(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_weapon(fighter);
    weapon_followup_victim(fighter);
    weapon_back_hp(fighter);
  } else {
    weapon_draw(fighter, &native_weapon);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
}

static void armed_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

static void armed_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

static void armed_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

static void armed_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

static void script_weapon_victim_getup(Fighter *victim) {
  mk4_script_finish_clip(victim, 1);
  script_throw_getup(victim);
}

static void script_staff_victim_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  native_delay(victim, 20);
  mk4_script_finish_clip(victim, 1);
  script_throw_getup(victim);
}
