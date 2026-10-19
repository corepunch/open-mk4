#include "../character.h"

static void goro_event_counter_sweep(Fighter *fighter);

/* Native 43afc0/43b1a0 share 43b940: advance the pose and query D[32]
 * for three rows or D[33] for eight rows, then finish the remaining clip.
 * A contact consumes the query but preserves the remaining pose window. */
static const AttackRecord punch_contact = {0x13810, -0xe106, 0xfd2f, 0x11be7, 189, 189, 7, 0, 0x2666};
static const AttackRecord kick_contact = {0x1b2b0, -0x6b43, 0x16872, 0xa624, 190, 190, 7, 0, 0x2666};

static void contact_frame(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

static void contact_tail(Fighter *fighter, const AttackRecord *record, int frames) {
  fighter->native_record = record;
  native_calls(fighter, frames, 1, contact_frame);
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(fighter_ready);
}

void goro_script_punch_tail(Fighter *fighter) {
  contact_tail(fighter, &punch_contact, 3);
}

void goro_script_counter_kick_tail(Fighter *fighter) {
  contact_tail(fighter, &kick_contact, 8);
}

void goro_script_event_finish(Fighter *fighter) {
  fighter_fiber_finish(fighter_ready);
}

void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action) {
  require(fighter != NULL && script != NULL && count > 0, "Invalid Goro actor event");
  fighter->playback_move = (MoveDef){.clip = clip, .last = count - 1, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = action};
  fighter_start_move(fighter, clip == 599 ? S_PUNCH : S_KICK, &fighter->playback_move);
  fighter_start_fiber(fighter, script, NULL);
}

void goro_event_progress(Fighter *fighter) {
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfb60, 599, 57, 0);
}

void goro_event_counter_kick(Fighter *fighter) {
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfb70, 598, 31, 0);
}

static void goro_event_counter_sweep(Fighter *fighter) {
  /* Native 43b320 writes 0x0409 to the selected fighter before script entry. */
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfbb8, 601, 52, 0x0409);
}
