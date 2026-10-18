#include "../character.h"

static void shinnok_high_punch(Fighter *fighter);
static const MoveDef s_high_punch = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

void shinnok_jab(Fighter *fighter) {
  shinnok_start(fighter, S_PUNCH, &shinnok_s_jab, shinnok_entry_retail_script_004e62f0);
}

static void shinnok_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    shinnok_close_hp(fighter);
  } else {
    shinnok_jab(fighter);
  }
}

void shinnok_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_high_punch);
}

void shinnok_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? shinnok_lp_throw : shinnok_jab);
}
