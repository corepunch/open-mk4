#include "../character.h"

static void kitana_kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {83230, -51118, 65536, 51118, 16, 13, 6, 0, 8519};

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void kitana_kick_contact(Fighter *fighter) {
  if (!fighter->connected && kitana_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void kitana_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &kitana_s_kick);
  fighter_start_fiber(fighter, kitana_entry_retail_script_004e5ca0, NULL);
}

void kitana_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? kitana_roundhouse : kitana_hk);
}

void kitana_script_kick_tail(Fighter *fighter) {
  native_calls(fighter, 3, 0, kitana_kick_contact);
  kitana_entry_retail_script_004e5d20(fighter);
}
