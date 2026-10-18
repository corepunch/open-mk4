#include "../character.h"

static void fujin_entry_high_punch(Fighter *fighter);
static void fujin_jab(Fighter *fighter);
static void fujin_enter_punch(Fighter *fighter);
static const MoveDef s_retail_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010B};

static void fujin_entry_high_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

static void fujin_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_retail_jab);
  fighter_start_fiber(fighter, fujin_entry_retail_script_004e62f0, NULL);
}

static void fujin_enter_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

void fujin_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_punch);
}

void fujin_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? fujin_enter_throw : fujin_enter_punch);
}
