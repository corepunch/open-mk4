#include "../character.h"

static void reptile_high_kick(Fighter *fighter);
static void reptile_low_kick(Fighter *fighter);

/* 4e5ca0 selects 4e6c90: clip 66 opens at 3 and advances twice per visit
 * to 17, then 46a760 runs 470c60 with block 4e5d30. */
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
/* 4e5d30 selector 1: B[1], 13%, reactions 16/13/6. */
static const AttackRecord high_kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};

static void high_kick_query(Fighter *fighter) {
  /* 470cb0 skips the query after contact or against action 2001. */
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &high_kick_record);
  }
}

static void high_kick_recover(Fighter *fighter) {
  /* 46a770: action 1006 (470e80), then VM 4e5c70. */
  fighter->action = ACTION_RETAIL_1006;
  reptile_entry_retail_script_004e5c70(fighter);
}

/* 4e5d30 lists poses 17, 17 and 16 with one B[1] query per visit; the
 * terminator calls 46a770 on the following visit. */
void reptile_script_kick_tail(Fighter *fighter) {
  static const int poses[] = {17, 17, 16};
  require(fighter_move(fighter)->clip == 66 && fighter_sample(fighter) == 17, "Reptile high kick needs its selected clip-66 startup");
  native_begin(fighter, 1, 3, high_kick_recover);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, poses[frame - 1], high_kick_query);
  }
  native_play(fighter, false);
}

static void reptile_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_high_kick);
  fighter->sample_override = 0;
  reptile_entry_retail_script_004e5ca0(fighter);
}

static void reptile_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &reptile_s_kick);
  fighter->action = ACTION_RETAIL_0102;
  reptile_entry_retail_script_004e5d90(fighter);
}

void reptile_press_kick(Fighter *fighter) {
  if (!fighter->armed && fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, reptile_roundhouse);
    return;
  }

  if (!fighter_holds_back(fighter) && !fighter->armed && fighter_distance(fighter) <= .8f && !fighter_airborne(fighter_opponent(fighter))) {
    fighter_interrupt(fighter, reptile_close_hk);
    return;
  }

  fighter_interrupt(fighter, reptile_high_kick);
}

void reptile_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, reptile_lk_throw);
    return;
  }

  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reptile_sweep : reptile_low_kick);
}
