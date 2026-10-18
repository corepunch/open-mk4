#include "../character.h"

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
    cage_jab(fighter);
  }
}

void cage_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, cage_high_punch);
}

void cage_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? cage_lp_throw : cage_jab);
}

void cage_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, cage_uppercut);
}
