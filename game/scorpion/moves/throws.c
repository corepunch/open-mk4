#include "../character.h"

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

void scorpion_thrown_end_script(Fighter *victim) {
  native_play_to(victim, 130, 1);
  native_delay(victim, 1);
  fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
}

static void ground_thrown_frame(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    fighter_stop_mmove(victim);
    return;
  }
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->mmove_hold = true;
}

static void ground_thrown_script(Fighter *victim) {
  native_begin(victim, 0, 0, NULL);
  victim->native.frames[0].thinkfunc = ground_thrown_frame;
  native_wait(victim);
  fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!scorpion_can_ground_throw(fighter, lk)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = scorpion_opponent(fighter);
  scorpion_stop_body(fighter);
  scorpion_stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &scorpion_s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  fighter_start_fiber(fighter, lk ? scorpion_entry_retail_script_004de2f8 : scorpion_entry_retail_script_004de798, NULL);
  fighter_start_fiber(victim, ground_thrown_script, NULL);
}

void scorpion_enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

void scorpion_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

void scorpion_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, scorpion_entry_retail_script_004e5e28, scorpion_entry_retail_script_004e9910);
}
