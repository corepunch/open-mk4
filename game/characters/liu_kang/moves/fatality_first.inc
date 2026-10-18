static bool can_first_fatality(const Fighter *fighter) {
  /* 45e410 / 4e4158: word A requires BLK+HK, word B requires LK;
   * reject Up/HP/LP/RUN. Directions other than Up are outside the mask. */
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned mask = chord | BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_RUN);
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f && (fighter->held & mask) == chord;
}

static void liu_kang_first_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_LIU_KANG_1);
}
