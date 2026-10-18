#include "../character.h"

static void fujin_sweep(Fighter *fighter);
static void fujin_entry_sweep(Fighter *fighter);

static void fujin_sweep(Fighter *fighter) {
  fujin_start(fighter, S_SWEEP, fujin_entry_retail_script_004e6798);
}

static void fujin_entry_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

void fujin_enter_sweep(Fighter *fighter) {
  fujin_entry_sweep(fighter);
}

void fujin_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_sweep);
}

static const AttackRecord sweep_record = {0x170a3, 0x8518, 0xdeb8, 0x8000, 19, 19, 4, ATTACK_LOW, 0x23d7};

static void sweep_recover(Fighter *fighter) {
  /* 46f130 sets action1006, then428cf0 advances to the clip end. */
  fighter->action = 0x1006;
  int start = fighter_sample(fighter);
  native_begin(fighter, start + 1, 43, fighter_ready);
  for (int pose = start + 1; pose <= 43; ++pose) {
    native_frame(fighter, pose, pose, NULL);
  }
  native_play(fighter, false);
}

static void sweep_contact(Fighter *fighter) {
  if (battle_contact(fighter, &sweep_record) != CONTACT_MISS) {
    sweep_recover(fighter);
  }
}

void fujin_sweep_tail(Fighter *fighter) {
  /* 46efd0 submits23/24 and queries B36; contact exits immediately. */
  native_begin(fighter, 23, 24, sweep_recover);
  native_frame(fighter, 23, 23, sweep_contact);
  native_frame(fighter, 24, 24, sweep_contact);
  native_play(fighter, false);
}
