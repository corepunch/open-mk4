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

static void equip_scepter(Fighter *fighter) {
  fighter_equip_weapon(fighter, &scepter);
}

static void scepter_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, index == 0 || index == 2, (index == 0 || index == 2) ? retail_script_004e66d0 : retail_script_004e66f0);
  } else {
    weapon_start_strike(fighter, &scepter_attacks[index], true);
  }
}

static void scepter_hp(Fighter *fighter) {
  scepter_attack(fighter, 0);
}

static void scepter_lp(Fighter *fighter) {
  scepter_attack(fighter, 1);
}

static void scepter_back_hp(Fighter *fighter) {
  scepter_attack(fighter, 2);
}

static void scepter_back_lp(Fighter *fighter) {
  scepter_attack(fighter, 3);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void sub_zero_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_scepter(fighter);
    weapon_followup_victim(fighter);
    weapon_start_strike(fighter, &scepter_attacks[2], false);
    fighter->action = ACTION_RETAIL_010E;
  } else {
    weapon_draw(fighter, &scepter);
    retail_script_004e62c0(fighter);
  }
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void sub_zero_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

static void script_draw_equip(Fighter *fighter) {
  equip_scepter(fighter);
  retail_script_004e62d8(fighter);
}
