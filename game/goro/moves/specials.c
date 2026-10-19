#include "../character.h"
#include "math_util.h"

static void special_query(Fighter *fighter);
static void special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries);
static const MoveDef s_special_punch = {.clip = 599, .last = 56, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef s_special_kick = {.clip = 598, .last = 30, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const MoveDef s_special_uppercut = {.clip = 600, .last = 36, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
static const AttackRecord punch_record = {0x13810, -0xe106, 0xfd2f, 0x11be7, 189, 189, 7, 0, 0x2666};
static const AttackRecord kick_record = {0x1b2b0, -0x6b43, 0x16872, 0xa624, 190, 190, 7, 0, 0x2666};
static const AttackRecord uppercut_record = {0x16b43, -0xcf5c, 0x109fb, 0x1424d, 192, 192, 6, 0, 0x23d7};

/* Goro's command handlers open with 489ef0's sampled voice token and
 * 45f870, which stops the grounded body and refaces the pair before the
 * selected clip starts. */
void goro_begin_special(Fighter *fighter, StateId state, const MoveDef *move, int token) {
  battle_sound(SOUND_TOKEN, token);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
}

static void special_query(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

/* 43b940 advances one pose and queries D[n] per visit for its row count;
 * a contact (43ba30) keeps advancing without further queries. 428cf0 then
 * finishes the bound clip, 429670 holds its last pose one visit and 43aa50
 * returns a human controller to its stance. */
static void special_tail(Fighter *fighter, const AttackRecord *record, int first, int queries) {
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && first + queries - 1 <= last, "Goro special contact rows exceed the bound clip");
  fighter->native_record = record;
  native_begin(fighter, 1, last - first + 2, fighter_ready);
  for (int frame = 1; frame <= last - first + 2; ++frame) {
    native_frame(fighter, frame, mini(last, first + frame - 1), frame <= queries ? special_query : NULL);
  }
  native_play(fighter, false);
}

/* 43af90 -> 4dfb60 clip 599 to 14, then 43afc0: three D[32] rows. */
void goro_special_punch(Fighter *fighter) {
  goro_begin_special(fighter, S_PUNCH, &s_special_punch, 0x5fa);
  goro_entry_retail_script_004dfb60(fighter);
}

void goro_script_punch_tail(Fighter *fighter) {
  special_tail(fighter, &punch_record, fighter_sample(fighter) + 1, 3);
}

/* 43b170 -> 4dfb70 clip 598 to 8, then 43b1a0: eight D[33] rows. */
void goro_special_kick(Fighter *fighter) {
  goro_begin_special(fighter, S_KICK, &s_special_kick, 0x5fa);
  goro_entry_retail_script_004dfb70(fighter);
}

void goro_script_kick_tail(Fighter *fighter) {
  special_tail(fighter, &kick_record, fighter_sample(fighter) + 1, 8);
}

/* 43b640 binds slot 53c6bc (fixup -> descriptor 4fc99c, clip 600) at
 * pose 0 through 428750, advances nine poses with 429490, then queries
 * D[35] for five rows through 43b940 before finishing the clip. */
void goro_special_uppercut(Fighter *fighter) {
  goro_begin_special(fighter, S_UPPERCUT, &s_special_uppercut, 0x5fa);
  mk4_mmove_bind(fighter, 600, 37, 0);
  fighter->native_record = &uppercut_record;
  native_begin(fighter, 0, 37, fighter_ready);
  for (int frame = 0; frame <= 37; ++frame) {
    native_frame(fighter, frame, mini(36, frame), frame >= 9 && frame < 14 ? special_query : NULL);
  }
  native_play(fighter, false);
}
