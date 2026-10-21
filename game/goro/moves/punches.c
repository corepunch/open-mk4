#include "../character.h"

static void goro_jab(Fighter *fighter);
static void goro_high_punch(Fighter *fighter);

static void goro_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &goro_s_jab);
  goro_entry_retail_script_004e62f0(fighter);
}

/* Goro shares stance table 4e5058: 460f40 selects close HP 46be40 within
 * 0.8 unless 48e6e0 sees a reacting victim or 48ee60 an airborne one. */
static void goro_high_punch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    goro_close_hp(fighter);
  } else {
    retail_high_punch(fighter);
  }
}

/* 46d290/46d9b3: clip 296/297, action 010a and A[0], the same native
 * repeat/recovery program as the humanoid roster. */

void goro_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : goro_high_punch);
}

/* 460fb0 selects LP throw 42c560 within 0.7, otherwise jab 46dca0. */
void goro_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter_distance(fighter) <= 0.7f ? goro_lp_throw : goro_jab);
}

void goro_press_crouching_punch(Fighter *fighter) {
  fighter_interrupt(fighter, goro_uppercut);
}
