#include "../character.h"

static void sub_zero_sweep_contact(Fighter *fighter);
static void sweep_recover(Fighter *fighter);
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7};
static const mframe_t sweep_recovery_frames[24] = {{0}};
static const mmove_t sweep_recovery = {.firstframe = 22, .lastframe = 45, .frame = sweep_recovery_frames, .endfunc = sub_zero_ready};
// clang-format off: Keep table entries on one row, including long rows.
static const int sweep_contact_frames_samples[] = {
  22,
  22,
};
// clang-format on
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t sweep_contact_frames[2] = {
  [0] = {.thinkfunc = sub_zero_sweep_contact},
  [1] = {.thinkfunc = sub_zero_sweep_contact},
};
// clang-format on
static const mmove_t sweep_contact = {
    .firstframe = 0, .lastframe = 1, .frame = sweep_contact_frames, .endfunc = sweep_recover, .samples = sweep_contact_frames_samples};

/* 0x46f080: no gate; queries each visit until a contact. */
static void sub_zero_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

void sub_zero_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &sub_zero_s_sweep);
  sub_zero_entry_retail_script_004e6798(fighter);
}

void sub_zero_enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void sweep_recover(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_recovery);
}

void sub_zero_script_sweep_tail(Fighter *fighter) {
  fighter_mmove_phase(fighter, &sweep_contact);
}
