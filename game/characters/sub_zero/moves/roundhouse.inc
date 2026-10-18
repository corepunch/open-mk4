static void sub_zero_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void script_roundhouse_tail(Fighter *fighter) {
  native_ground_next(fighter, &ground_profile);
}

static void script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}
