#include "../character.h"

static void reptile_jab(Fighter *fighter);
static void reptile_entry_high_punch(Fighter *fighter);
static void reptile_entry_low_punch(Fighter *fighter);

static void reptile_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &reptile_s_jab);
  reptile_entry_retail_script_004e62f0(fighter);
}

static void reptile_entry_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    reptile_close_hp(fighter);
  } else {
    reptile_jab(fighter);
  }
}

static void reptile_entry_low_punch(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.7f) {
    reptile_lp_throw(fighter);
  } else {
    reptile_jab(fighter);
  }
}

void reptile_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_entry_high_punch);
}

void reptile_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_entry_low_punch);
}
