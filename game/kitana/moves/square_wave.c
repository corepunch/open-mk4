#include "../character.h"

static void square_query(Fighter *fighter);
static void square_done(Fighter *fighter);
static void square_land(Fighter *fighter);
static void square_fall(Fighter *fighter);
static void square_victim_drop(Fighter *victim);
static void square_victim_guard(Fighter *victim);
static const MoveDef s_square_wave = {.clip = 72, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x201};
static const AttackRecord square_record = {19005, -9830, 39321, 57671, 87, 87, 0, 0, 13107};

static void square_query(Fighter *fighter) {
  battle_contact(fighter, &square_record);
  fighter->mmove_hold = fighter_height(fighter) < 2.4f && !fighter->connected;
}

static void square_done(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter_finish_mmove(fighter, kitana_ready);
}

static void square_land(Fighter *fighter) {
  native_await(fighter, native_grounded, square_done);
}

static void square_fall(Fighter *fighter) {
  fighter->body.gravity = 0x28f;
  fighter_set_speed(fighter, fighter->connected ? -0x1999 : 0);
  if (fighter->connected) {
    native_hold(fighter, 32, square_land);
  } else {
    square_land(fighter);
  }
}

void kitana_script_square_wave_tail(Fighter *fighter) {
  if (fighter_height(fighter) < 2.4f && !fighter->connected) {
    native_begin(fighter, 0, 0, square_fall);
    native_frame(fighter, 0, fighter_sample(fighter), square_query);
    native_play(fighter, false);
  } else {
    square_fall(fighter);
  }
}

void kitana_square_wave(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_square_wave);
  fighter->mmove_cleanup = kitana_special_cleanup;
  kitana_entry_retail_script_004e85c0(fighter);
}

static void square_victim_drop(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->body.gravity = 0x28f;
  victim->body.vel[AXIS_Y] = 0x666;
  kitana_script_throw_getup(victim);
}

static void square_victim_guard(Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  if (owner->action != 0x201 || owner->pending != NULL) {
    square_victim_drop(victim);
  }
}

/* The carried pose cycle repeats until the owner leaves the square wave. */
void kitana_square_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 136, 24, 0);
  victim->flags |= FF_PAIRED;
  const Fighter *owner = fighter_opponent(victim);
  if (owner->action != 0x201 || owner->pending != NULL) {
    square_victim_drop(victim);
    return;
  }
  native_begin(victim, 0, 19, NULL);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(victim, frame, 4 + (frame + 1) % 20, square_victim_guard);
  }
  native_play(victim, false);
}
