#include "../character.h"

static void thrown_end_done(Fighter *victim);
static void ground_thrown_frame(Fighter *victim);
static void ground_thrown_script(Fighter *victim);
static void ground_throw(Fighter *fighter, bool lk);
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 152, .last = 41, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

bool scorpion_can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

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

static void ground_throw(Fighter *fighter, bool lk) {
  if (!scorpion_can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  Fighter *victim = fighter_peer(fighter);
  mk4_script_stop_body(fighter);
  mk4_script_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &scorpion_s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  (lk ? scorpion_entry_retail_script_004de2f8 : scorpion_entry_retail_script_004de798)(fighter);
  ground_thrown_script(victim);
}

void scorpion_enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

void scorpion_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

void scorpion_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, scorpion_entry_retail_script_004e5e28, scorpion_entry_retail_script_004e9910);
}
