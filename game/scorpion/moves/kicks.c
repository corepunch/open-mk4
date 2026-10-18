#include "../character.h"

static void scorpion_kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void scorpion_kick_contact(Fighter *fighter) {
  if (!fighter->connected && scorpion_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void scorpion_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &scorpion_s_kick);
  fighter_start_fiber(fighter, scorpion_entry_retail_script_004e5ca0, NULL);
}

void scorpion_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_hk);
}

void scorpion_script_kick_tail(Fighter *fighter) {
  scorpion_kick_contact(fighter);
  native_calls(fighter, 2, 0, scorpion_kick_contact);
  scorpion_entry_retail_script_004e5c70(fighter);
}
