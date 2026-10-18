static void ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

static void ground_attack(Fighter *fighter, int index) {
  bool followup = fighter_fast_start(fighter);
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {retail_script_004e5d90, ground_native_tail, retail_script_004e6158, retail_script_004e6020, retail_script_004e61e0,
      retail_script_004e69b0, retail_script_004e6190, retail_script_004e6800};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static void raiden_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void raiden_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void raiden_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : raiden_crouch_lp);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, raiden_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, raiden_crouch_lk);
}

static void script_crouch_lk_tail(Fighter *fighter) {
  native_delay(fighter, 1);
  ground_native_tail(fighter);
}

static void script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}
