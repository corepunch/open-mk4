#include "../character.h"

static void noob_saibot_high_kick(Fighter *fighter);
static void noob_saibot_low_kick(Fighter *fighter);

/* 46a760 -> 470c60 with block 4e5d30: B[1] is queried while poses 17, 17
 * and 16 are submitted. A contact latches and the victim's crouch action
 * 2001 skips the query. 46a770 then stores action 1006 (470e80) and enters
 * recovery 4e5c70 from pose 16. */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};

static void kick_contact(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

static void kick_recovery(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  noob_saibot_entry_retail_script_004e5c70(fighter);
}

void noob_saibot_script_kick_finish(Fighter *fighter) {
  static const int poses[] = {17, 17, 16};
  native_begin(fighter, 1, 3, kick_recovery);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, poses[frame - 1], kick_contact);
  }
  native_play(fighter, false);
}

static void noob_saibot_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &noob_saibot_s_kick);
  noob_saibot_entry_retail_script_004e5ca0(fighter);
}

static void noob_saibot_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &noob_saibot_s_kick);
  fighter->action = ACTION_RETAIL_0102;
  noob_saibot_entry_retail_script_004e5d90(fighter);
}

void noob_saibot_press_high_kick(Fighter *fighter) {
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, noob_saibot_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, noob_saibot_close_hk);
    return;
  }

  fighter_interrupt(fighter, noob_saibot_high_kick);
}

void noob_saibot_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, noob_saibot_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? noob_saibot_sweep : noob_saibot_low_kick);
}

void noob_saibot_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_sweep);
}
