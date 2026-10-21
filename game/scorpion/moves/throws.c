#include "../character.h"

static void thrown_end_done(Fighter *victim);
static void ground_thrown_frame(Fighter *victim);
static void ground_thrown_script(Fighter *victim);
static void ground_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

static void thrown_end_done(Fighter *victim) {
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void scorpion_thrown_end_script(Fighter *victim) {
  native_finish_clip(victim, thrown_end_done);
}

static void ground_thrown_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    thrown_end_done(victim);
    return;
  }
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->mmove_hold = true;
}

static void ground_thrown_script(Fighter *victim) {
  native_begin(victim, 0, 0, NULL);
  victim->native.frames[0].thinkfunc = ground_thrown_frame;
  native_play(victim, false);
}

static void ground_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, true)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, &s_lk_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  (scorpion_entry_retail_script_004de2f8)(fighter);
  ground_thrown_script(victim);
}

void scorpion_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter);
}

void scorpion_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, scorpion_entry_retail_script_004e5e28, scorpion_entry_retail_script_004e9910);
}
