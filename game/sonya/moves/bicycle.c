#include "../character.h"

static bool bicycle_height(const Fighter *fighter);
static void bicycle_contact(Fighter *fighter);
static void bicycle_done(Fighter *fighter);
static void bicycle_fall(Fighter *fighter);
static void bicycle_spin(Fighter *fighter);
static void bicycle_script(Fighter *fighter);
static const MoveDef s_bicycle = {.clip = 310, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x205};
static const AttackRecord bicycle_record = {80609, -1310, 62259, 20971, 86, 86, 0, 0, 9830};

static bool bicycle_height(const Fighter *fighter) {
  return fighter_height(fighter) >= 2.5f;
}

static void bicycle_contact(Fighter *fighter) {
  if (battle_contact(fighter, &bicycle_record) != CONTACT_MISS) {
    bicycle_fall(fighter);
  }
}

static void bicycle_done(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void bicycle_fall(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->body.gravity = 0x28f;
  native_await(fighter, native_grounded, bicycle_done);
}

static void bicycle_spin(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_mmove_bind(fighter, 101, 9, 0);
  fighter_set_speed(fighter, 0x3333);
  native_begin(fighter, 0, 8, bicycle_fall);
  for (int frame = 0; frame < 9; ++frame) {
    native_frame(fighter, frame, frame, bicycle_contact);
  }
  native_play(fighter, false);
}

static void bicycle_script(Fighter *fighter) {
  fighter->body.gravity = 0;
  fighter->body.vel[AXIS_Y] = -0x2666;
  battle_sound(SOUND_A, 6);
  battle_sound(SOUND_INDEXED, 12);
  native_await(fighter, bicycle_height, bicycle_spin);
}

void sonya_bicycle(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_JUMP, &s_bicycle);
  fighter->mmove_cleanup = sonya_special_cleanup;
  bicycle_script(fighter);
}
