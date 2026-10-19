#include "../character.h"

static void standing_high_punch(Fighter *fighter);
static const MoveDef s_high_punch = {.clip = 296, .last = 20, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010A};
static const MoveDef s_high_punch_second = {.clip = 297, .last = 18, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010A};
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};

static void quan_chi_high_punch(Fighter *fighter);

void quan_chi_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &quan_chi_s_jab);
  quan_chi_entry_retail_script_004e62f0(fighter);
}

static void quan_chi_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    quan_chi_close_hp(fighter);
  } else {
    standing_high_punch(fighter);
  }
}

void quan_chi_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : quan_chi_high_punch);
}

void quan_chi_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? quan_chi_lp_throw : quan_chi_jab);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */
static void standing_high_punch(Fighter *fighter) {
  static const NativeHPProfile profile = {&hp_record, &s_high_punch, &s_high_punch_second, fighter_ready};
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  native_hp_start(fighter, &profile);
}
