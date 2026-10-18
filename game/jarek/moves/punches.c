#include "../character.h"

static void jarek_high_punch(Fighter *fighter);
static const MoveDef s_high_punch = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

void jarek_jab(Fighter *fighter) {
  jarek_start(fighter, S_PUNCH, &jarek_s_jab, jarek_entry_retail_script_004e62f0);
}

static void jarek_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    jarek_close_hp(fighter);
  } else {
    jarek_jab(fighter);
  }
}

void jarek_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_high_punch);
}

void jarek_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? jarek_lp_throw : jarek_jab);
}
