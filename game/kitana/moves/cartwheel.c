#include "../character.h"

static void cartwheel_done(Fighter *fighter);
static void cartwheel_stop(Fighter *fighter);
static void cartwheel_recover(Fighter *fighter);
static const MoveDef s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};
static const AttackRecord cartwheel_record = {75300, -45088, 72083, 89063, 121, 121, 9, 0, 4587};

static void cartwheel_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void cartwheel_stop(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    native_hold(fighter, 20, cartwheel_done);
  } else {
    cartwheel_done(fighter);
  }
}

static void cartwheel_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_finish_clip(fighter, cartwheel_stop);
}

void kitana_script_cartwheel_tail(Fighter *fighter) {
  fighter->cartwheel_ready = g_battle.ticks + 48;
  native_query_rows(fighter, &cartwheel_record, 9, 1, cartwheel_recover);
}

void kitana_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &s_cartwheel);
  fighter->mmove_cleanup = kitana_special_cleanup;
  kitana_entry_retail_script_004e81d0(fighter);
}
