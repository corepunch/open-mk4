#include "../character.h"

static void raiden_sweep_contact(Fighter *fighter);
static const AttackRecord sweep_record = {94371, 34072, 57016, 32768, 19, 19, 4, 1, 9175};

/* 0x46f080: no gate; queries each visit until a contact. */
static void raiden_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

void raiden_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &raiden_s_sweep);
  fighter_start_fiber(fighter, raiden_entry_retail_script_004e6798, NULL);
}

void raiden_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void raiden_script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, raiden_sweep_contact);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(raiden_ready);
}
