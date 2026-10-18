static void air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  mk4_land(fighter);
}

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

static void start_air_attack(Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  if (combo) {
    fighter->body.gravity = 0;
  }
  fighter_upright(fighter);
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {retail_script_004e6658, retail_script_004e6710, retail_script_004e61f8, retail_script_004e6500, retail_script_004e63d8};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static void air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void air_hk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void air_lk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

static void press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

static void press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

static void press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    kitana_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

static void script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

static void script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

static void script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

static void script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

static void script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void script_air_release(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 85;
  fighter_start_fiber(victim, retail_script_004e98a0, NULL);
  battle_damage(victim, 0x2666);
  mk4_script_finish_clip(fighter, 1);
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = -0x51e;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(mk4_land);
}

static void script_air_getup(Fighter *victim) {
  mk4_script_finish_clip(victim, 1);
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    fighter_fiber_finish(NULL);
  }
  mk4_script_bind(victim, 197, 29, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

static void cpu_air_hk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 3 : 2);
}

static void cpu_air_lk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 4 : 2);
}
