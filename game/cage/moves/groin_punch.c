#include "../character.h"

static void groin_choose(Fighter *fighter);
static void groin_query(Fighter *fighter);
static void groin_recover(Fighter *fighter);
static void groin_victim(Fighter *victim);
static const MoveDef groin_move = {.clip = 438, .last = 14, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x110};
static const MoveDef groin_stun = {.clip = 126, .last = 14, .flags = SF_LOCK | SF_REACTION, .action = 0x1014};
static const AttackRecord groin_record = {0xd6c8, 0x28f5, 0xba5e, 0xd3f7, 164, 164, 165, 0, 0x28f5};

bool cage_can_groin_punch(const Fighter *fighter) {
  unsigned chord = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_BLOCK);
  unsigned excluded = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) |
                      BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && !fighter->armed && !fighter_airborne(fighter) && (fighter->held & chord) == chord && !(fighter->held & excluded) &&
         g_battle.ticks - fighter->latch[BUTTON_LOW_PUNCH] <= 4 && g_battle.ticks - fighter->latch[BUTTON_BLOCK] <= 4;
}

static void groin_cleanup(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_NO_GUARD;
}

void cage_groin_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &groin_move);
  fighter->mmove_cleanup = groin_cleanup;
  battle_sound(SOUND_C, 9);
  native_begin(fighter, 1, 5, groin_choose);
  for (int sample = 1; sample <= 5; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}

static void groin_victim(Fighter *victim) {
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(victim);
  }
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_HIT, &groin_stun);
  victim->flags |= FF_REACTING;
  victim->body.physics = fighter_friction;
  cage_entry_retail_script_004e9f78(victim);
}

static void groin_contact(Fighter *fighter) {
  ContactResult result = battle_contact(fighter, &groin_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result != CONTACT_BLOCK) {
    fighter_interrupt((Fighter *)fighter_opponent(fighter), groin_victim);
  }
  groin_recover(fighter);
}

static void groin_query(Fighter *fighter) {
  native_begin(fighter, 1, 3, groin_recover);
  native_frame(fighter, 1, 12, groin_contact);
  native_frame(fighter, 2, 13, groin_contact);
  native_frame(fighter, 3, 14, NULL);
  native_play(fighter, false);
  /* Native468b8a queries D[27] at11 before advancing the next two poses. */
  groin_contact(fighter);
}

static void groin_recover(Fighter *fighter) {
  int repetitions = fighter->contact == CONTACT_BLOCK ? 12 : 5;
  int pose = fighter_sample(fighter), frame = 0;
  fighter->action = 0x100e;
  fighter->flags |= FF_NO_GUARD;
  /* Native4292f0 always steps once before testing its target. A contact at
   * pose11 therefore retreats to10 before the first extension to14. */
  int first_rewind = pose > 12 ? pose - 12 : 1;
  int first_low = pose - first_rewind;
  int frames = first_rewind + 14 - first_low + (repetitions - 1) * 4;
  native_begin(fighter, 1, frames, cage_entry_retail_script_004e5ba8);
  for (int repeat = 0; repeat < repetitions; ++repeat) {
    do {
      native_frame(fighter, ++frame, --pose, NULL);
    } while (pose > 12);
    while (pose < 14) {
      native_frame(fighter, ++frame, ++pose, NULL);
    }
  }
  native_play(fighter, false);
}

static void groin_avoid_reverse(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, pose, fighter_ready);
  for (int frame = 1; frame <= pose; ++frame) {
    native_frame(fighter, frame, pose - frame, NULL);
  }
  native_play(fighter, false);
}

static void groin_choose(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int roster = victim->roster == 16 ? 2 : victim->roster == 17 ? 7 : victim->roster;
  if (roster == 2 || roster == 9 || victim->action == 0x1014) {
    native_hold(fighter, 20, groin_avoid_reverse);
    return;
  }
  native_begin(fighter, 6, 11, groin_query);
  for (int sample = 6; sample <= 11; ++sample) {
    native_frame(fighter, sample, sample, NULL);
  }
  native_play(fighter, false);
}
