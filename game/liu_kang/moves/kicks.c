#include "../character.h"

static void liu_kang_kick_contact(Fighter *fighter);
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void liu_kang_kick_contact(Fighter *fighter) {
  if (!fighter->connected && liu_kang_victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void liu_kang_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &liu_kang_s_kick);
  liu_kang_entry_retail_script_004e6c90(fighter);
}

void liu_kang_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? liu_kang_roundhouse : liu_kang_hk);
}

void liu_kang_liu_script_kick_tail(Fighter *fighter) {
  native_kick_contact_phase(fighter, liu_kang_kick_contact, liu_kang_entry_retail_script_004e5c70);
}
