#include "../character.h"

static void tanya_low_kick(Fighter *fighter);

/* 4e6c90 selects B[1] 4ebae0 (13%, 16/13/6). */
static const AttackRecord high_kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};

static void kick_frame(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &high_kick_record);
  }
}

/* 46a760 -> 470c60 block 4e5d30: B[1] queries at pose 17, then 46a770 ->
 * 4e5c70. The former tail skipped this contact phase. */
void tanya_script_kick_finish(Fighter *fighter) {
  native_kick_contact_phase(fighter, kick_frame, tanya_entry_retail_script_004e5c70);
}

void tanya_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &tanya_s_kick);
  tanya_entry_retail_script_004e5ca0(fighter);
}

static void tanya_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &tanya_s_kick);
  fighter->action = ACTION_RETAIL_0102;
  tanya_entry_retail_script_004e5d90(fighter);
}

void tanya_press_high_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, tanya_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, tanya_close_hk);
    return;
  }

  fighter_interrupt(fighter, tanya_high_kick);
}

void tanya_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, tanya_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? tanya_sweep : tanya_low_kick);
}
