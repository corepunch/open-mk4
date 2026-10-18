#include "../character.h"

static void standing_high_punch(Fighter *fighter);
static const MoveDef s_high_punch = {.clip = 296, .last = 20, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010A};
static const MoveDef s_high_punch_second = {.clip = 297, .last = 18, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010A};
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};

static void cage_jab(Fighter *fighter);
void cage_powered_punch(Fighter *fighter);
static void cage_high_punch(Fighter *fighter);

void cage_script_powered_punch_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 1;
}

void cage_script_powered_punch_finish_tail(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, cage_combo_event, false, true);
  event->data[0] = 2;
}

static void cage_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &cage_s_jab);
  cage_entry_retail_script_004e62f0(fighter);
}

void cage_powered_punch(Fighter *fighter) {
  cage_start_script(fighter, S_PUNCH, &cage_s_powered_punch_script, cage_entry_retail_script_004e80d0);
}

static void cage_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    cage_close_hp(fighter);
  } else {
    standing_high_punch(fighter);
  }
}

void cage_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : cage_high_punch);
}

void cage_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? cage_lp_throw : cage_jab);
}

void cage_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_uppercut);
}

/* 46d290/46d9b3: select the native HP record and 296/297 repeat phases. */
static void standing_high_punch(Fighter *fighter) {
  static const NativeHPProfile profile = {&hp_record, &s_high_punch, &s_high_punch_second, fighter_ready};
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  native_hp_start(fighter, &profile);
}
