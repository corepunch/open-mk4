#include "../character.h"

static void shinnok_high_punch(Fighter *fighter);
static const MoveDef s_high_punch = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};

void shinnok_jab(Fighter *fighter) {
  shinnok_start(fighter, S_PUNCH, &shinnok_s_jab, shinnok_entry_retail_script_004e62f0);
}

static void shinnok_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
}

void shinnok_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_jab);
}

void shinnok_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? shinnok_lp_throw : shinnok_jab);
}
