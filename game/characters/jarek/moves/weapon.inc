static void jarek_script_weapon_draw_finish(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

static void jarek_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &jarek_weapon);
}

static void jarek_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
    return;
  }
  if (fighter->flags & FF_FOLLOW_UP) {
    jarek_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    jarek_weapon_back_hp(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_weapon_draw);
  weapon_draw(fighter, &jarek_weapon);
  fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
}
