#include "../character.h"

static void throw_getup_land(Fighter *victim);
static void lk_victim_land(Fighter *victim);
static void ground_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 43, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 163, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_air_owner = {.clip = 77, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_air_victim = {.clip = 161, .last = 39, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* 485b50: after landing, a living victim turns and plays getup 328 through
 * its last pose, holding that pose for one more visit before the controller. */
static void throw_getup_land(Fighter *victim) {
  if (victim->health <= 0) {
    mk4_mmove_thrown_finish(victim);
    return;
  }
  victim->facing = -victim->facing;
  mk4_mmove_bind(victim, 328, 15, 0);
  native_begin(victim, 1, 16, mk4_mmove_thrown_finish);
  for (int frame = 1; frame <= 16; ++frame) {
    native_frame(victim, frame, mini(14, frame), NULL);
  }
  native_play(victim, false);
}

void kitana_script_throw_getup(Fighter *victim) {
  native_await(victim, native_grounded, throw_getup_land);
}

static void ground_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, true)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &s_lk_throw, &s_lk_thrown, kitana_entry_retail_script_004de360);
}

void kitana_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter);
}

static void lk_victim_land(Fighter *victim) {
  native_await(victim, native_grounded, mk4_mmove_thrown_finish);
}

/* The victim advances seven poses and then completes clip 203 in one run. */
void kitana_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 203, 112, 75);
  native_finish_clip(victim, lk_victim_land);
}

void kitana_air_throw(Fighter *fighter) {
  if (!mk4_can_air_throw(fighter)) {
    return;
  }
  fighter_upright(fighter);
  fighter_upright(fighter_peer(fighter));
  mk4_mmove_throw_start(fighter, &s_air_owner, &s_air_victim, kitana_entry_retail_script_004e5ed0);
}
