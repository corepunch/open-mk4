#include "../character.h"

static void raiden_kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {71368, -56950, 83820, 74645, 16, 13, 6, 0, 8519};

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void raiden_kick_contact(Fighter *fighter) {
  if (!fighter->connected && fighter_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void raiden_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &raiden_s_kick);
  raiden_entry_retail_script_004e5ca0(fighter);
}

void raiden_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? raiden_roundhouse : raiden_hk);
}

void raiden_script_kick_tail(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 3, raiden_entry_retail_script_004e6998);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, pose + frame, raiden_kick_contact);
  }
  native_play(fighter, false);
}
