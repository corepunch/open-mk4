#include "../character.h"

static void sonya_kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {83230, -51118, 65536, 51118, 16, 13, 6, 0, 8519};

/* Runner 0x470c60 with block 0x4e6978: the first contact latches. */
static void sonya_kick_contact(Fighter *fighter) {
  if (!fighter->connected && fighter_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void sonya_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &sonya_s_kick);
  sonya_entry_retail_script_004e5ca0(fighter);
}

void sonya_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? sonya_roundhouse : sonya_hk);
}

void sonya_script_kick_tail(Fighter *fighter) {
  native_kick_contact_phase(fighter, sonya_kick_contact, sonya_entry_retail_script_004e5d20);
}
