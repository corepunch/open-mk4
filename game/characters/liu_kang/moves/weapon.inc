static void sword_travel(Fighter *fighter, int delay, int duration) {
  q16 speed = fighter->weapon_strike == NULL ? 0x11eb : fighter->weapon_strike->speed;
  fighter->saved_motion[0] = speed;
  native_schedule_speed(fighter, speed, delay, duration);
}

static void sword_cleanup(Fighter *fighter) {
  if (fighter->body.physics == native_motion_step) {
    native_motion_stop(fighter);
    fighter_set_speed(fighter, 0);
  }
}

static bool sword_repeat(Fighter *fighter) {
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) != 0;
  fighter->buffered &= ~BUTTON_BIT(BUTTON_HIGH_PUNCH);
  return repeat;
}

static void sword_blend_weight(Fighter *fighter, float weight) {
  fighter->blend_ticks = (int)weight;
}

static void sword_blend(Fighter *fighter, int ticks) {
  fighter->blend_clip = fighter_move(fighter)->clip;
  fighter->blend_sample = fighter_sample(fighter);
  fighter->blend_ticks = 0;
  fighter->blend_duration = ticks;
  native_begin(fighter, 1, ticks, NULL);
  for (int frame = 1; frame <= ticks; ++frame) {
    native_frame(fighter, frame, fighter->blend_sample, NULL);
    fighter->native.frames[frame - 1].aifunc = sword_blend_weight;
    fighter->native.frames[frame - 1].dist = (float)frame;
  }
  native_wait(fighter);
}

static void sword_strike_script(Fighter *fighter) {
  int index = fighter->script_phase;
  for (;;) {
    const WeaponStrike *strike = &sword_attacks[index];
    int opening = strike->windup - strike->first;
    native_advance(fighter, opening, 1);
    battle_sound(SOUND_C, strike->sound);
    native_play_to(fighter, strike->target, 1);
    native_queries(fighter, &strike->record, strike->queries, 1);
    bool hit = fighter->connected;
    fighter->action = ACTION_RETAIL_1006;
    if (index == 0 && hit && sword_repeat(fighter)) {
      index = 1; /* 496500 -> 4ec720/730, delayed travel then second strike. */
      fighter_fiber_move(fighter, S_PUNCH, &sword_attacks[index].move);
      fighter->weapon_strike = &sword_attacks[index];
      fighter->sample_override = sword_attacks[index].first;
      sword_travel(fighter, sword_attacks[index].delay, sword_attacks[index].travel);
      continue;
    }
    fighter_set_speed(fighter, 0);
    fighter->body.physics = NULL;
    native_play_to(fighter, strike->move.last, 1);
    if (index == 2 && hit) {
      sword_blend(fighter, 10); /* 494e10: fixed armed stance, step 0x1999. */
    }
    native_delay(fighter, 1);
    fighter_fiber_finish(fighter_ready);
  }
}

static void equip_dragon_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &dragon_sword);
}

static void sword_attack(Fighter *fighter, int index) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, index == 2, (index == 2) ? retail_script_004e66d0 : retail_script_004e66f0);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &sword_attacks[index].move);
  fighter->script_phase = index;
  fighter->weapon_strike = &sword_attacks[index];
  fighter->sample_override = sword_attacks[index].first;
  if (sword_attacks[index].travel != 0) {
    sword_travel(fighter, sword_attacks[index].delay, sword_attacks[index].travel);
  }
  fighter_start_fiber(fighter, index == 3 ? retail_script_004ec7f0 : sword_strike_script, sword_cleanup);
}

static void sword_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    weapon_air_attack(fighter, true, retail_script_004e66d0);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_sword_hp);
  fighter->weapon_strike = NULL;
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, retail_script_004ec680, sword_cleanup);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, 0);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, 2);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, 3);
}

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void liu_kang_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_dragon_sword(fighter);
    weapon_followup_victim(fighter);
    sword_lp(fighter); /* 4ecb40[type 66 - 60] -> 4964d0. */
  } else {
    weapon_draw(fighter, &dragon_sword);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void liu_kang_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

static void liu_script_draw_equip(Fighter *fighter) {
  equip_dragon_sword(fighter);
  retail_script_004e62d8(fighter);
}

static unsigned liu_script_sword_gate(Fighter *fighter) {
  return sword_repeat(fighter) ? 0u : 4u;
}

static void liu_script_sword_blend(Fighter *fighter) {
  sword_blend(fighter, 5);
  fighter_fiber_finish(fighter_ready);
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
