#include "../character.h"

static void scorpion_combo_hp(Fighter *fighter);

void scorpion_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &scorpion_s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  scorpion_entry_retail_script_004e62f0(fighter);
}

void scorpion_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    scorpion_ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &scorpion_s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  scorpion_hp_script(fighter);
}

static void scorpion_combo_hp(Fighter *fighter) {
  scorpion_ground_attack(fighter, G_COMBO_HP);
}

void scorpion_press_jab(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : scorpion_hp;
  fighter_interrupt(fighter, fighter->armed ? entry : scorpion_hp);
}

void scorpion_press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    scorpion_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, scorpion_combo_hp);
  }
}

void scorpion_press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, scorpion_enter_uppercut);
  } else {
    scorpion_press_jab(fighter);
  }
}

void scorpion_script_close_hp_tail(Fighter *fighter) {
  scorpion_ground_native_tail(fighter);
}

void scorpion_script_combo_hp_tail(Fighter *fighter) {
  scorpion_ground_native_next(fighter);
}
