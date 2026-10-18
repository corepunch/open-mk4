#include "../character.h"

static void goro_jab(Fighter *fighter);
static void goro_high_punch(Fighter *fighter);

static void goro_jab(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &goro_s_jab, goro_entry_retail_script_004e62f0);
}

static void goro_high_punch(Fighter *fighter) {
  goro_jab(fighter);
}

void goro_enter_jab(Fighter *fighter) {
  fighter_interrupt(fighter, goro_jab);
}

void goro_enter_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? goro_lp_throw : goro_jab);
}
