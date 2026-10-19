#include "../character.h"

static void goro_high_kick(Fighter *fighter);
static void goro_low_kick(Fighter *fighter);
static void kick_contact(Fighter *fighter);

/* 46a760 -> 470c60 block 4e5d30: B[1] at poses 17, 17 and 16, skipping
 * a crouching (2001) victim or a consumed contact, then 46a770 -> 4e5c70. */
static const AttackRecord kick_record = {89718, -55705, 86441, 70123, 16, 13, 6, 0, 8519};

static void kick_contact(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

void goro_script_kick_finish(Fighter *fighter) {
  static const int poses[] = {17, 17, 16};
  native_begin(fighter, 1, 3, goro_entry_retail_script_004e5c70);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, poses[frame - 1], kick_contact);
  }
  native_play(fighter, false);
}

static void goro_high_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &goro_s_kick);
  goro_entry_retail_script_004e5ca0(fighter);
}

static void goro_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &goro_s_kick);
  fighter->action = ACTION_RETAIL_0102;
  goro_entry_retail_script_004e5d90(fighter);
}

/* 461030: back selects roundhouse 46f1f0; within 0.8 a grounded victim
 * outside reactions 4001/4005 selects close HK 46ba60; else 46a680. */
void goro_press_high_kick(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    fighter_interrupt(fighter, goro_roundhouse);
    return;
  }
  if (fighter_distance(fighter) <= .8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    fighter_interrupt(fighter, goro_close_hk);
    return;
  }
  fighter_interrupt(fighter, goro_high_kick);
}

/* 460fc0: back selects sweep 46ef90; LK throw 42b6b0 precedes low kick. */
void goro_press_low_kick(Fighter *fighter) {
  if (!fighter->armed && !fighter_holds_back(fighter) && mk4_can_ground_throw(fighter, true)) {
    fighter_interrupt(fighter, goro_lk_throw);
    return;
  }
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? goro_sweep : goro_low_kick);
}

void goro_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_sweep);
}
