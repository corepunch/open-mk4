#include "../character.h"

static void scorpion_sweep_contact(Fighter *fighter);
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7};

/* 0x46f080: no gate; queries each visit until a contact. */
static void scorpion_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

void scorpion_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &scorpion_s_sweep);
  fighter_start_fiber(fighter, scorpion_entry_retail_script_004e6798, NULL);
}

void scorpion_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void scorpion_script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, scorpion_sweep_contact);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(scorpion_ready);
}
