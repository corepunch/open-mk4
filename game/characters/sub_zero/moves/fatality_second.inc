static bool can_second_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static void second_fatality_script(Fighter *fighter) {
  retail_script_004d2bf0(opponent(fighter));
  retail_script_004d2c10(fighter);
}

static void sub_zero_second_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SUB_ZERO_2);
}
