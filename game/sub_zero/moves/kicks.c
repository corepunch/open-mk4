#include "../character.h"

static void sub_zero_kick_contact(Fighter *fighter);
static void kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {0x116c8, -0xde76, 0x1476c, 0x12395, 16, 13, 6, 0, 0x2147};
// clang-format off: Keep table entries on one row, including long rows.
static const mframe_t kick_contact_frames[3] = {
  [0] = {.thinkfunc = kick_contact},
  [1] = {.thinkfunc = kick_contact},
  [2] = {.thinkfunc = kick_contact},
};
// clang-format on
static const mmove_t kick_contact_phase = {.firstframe = 10, .lastframe = 12, .frame = kick_contact_frames, .endfunc = sub_zero_entry_retail_script_004e6998};

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void sub_zero_kick_contact(Fighter *fighter) {
  if (!fighter->connected && sub_zero_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void sub_zero_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &sub_zero_s_kick);
  sub_zero_entry_retail_script_004e5c80(fighter);
}

void sub_zero_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sub_zero_roundhouse : sub_zero_hk);
}

static void kick_contact(Fighter *fighter) {
  fighter->sample_override = fighter_sample(fighter) + 1;
  sub_zero_kick_contact(fighter);
}

void sub_zero_script_kick_tail(Fighter *fighter) {
  sub_zero_hold_phase(fighter, &kick_contact_phase);
}
