static void tanya_lp_throw(Fighter *fighter) {
  tanya_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

void tanya_lk_throw(Fighter *fighter) {
  tanya_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

void tanya_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, retail_script_004e5e28, NULL);
}
