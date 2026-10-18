#include "../character.h"

static void liu_kang_sweep_contact(Fighter *fighter);
static const AttackRecord sweep_record = {0x170a3, 0x8518, 0xdeb8, 0x8000, 19, 19, 4, ATTACK_LOW, 0x23d7};

/* 0x46f080: no gate; queries each visit until a contact. */
static void liu_kang_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

void liu_kang_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &liu_kang_s_sweep);
  fighter_start_fiber(fighter, liu_kang_entry_retail_script_004e67d8, NULL);
}

void liu_kang_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

void liu_kang_liu_script_sweep_tail(Fighter *fighter) {
  native_calls(fighter, 2, 0, liu_kang_sweep_contact);
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(liu_kang_ready);
}
