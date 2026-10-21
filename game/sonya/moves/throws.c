#include "../character.h"

static void throw_getup_land(Fighter *victim);
static void lk_victim_land(Fighter *victim);
static void ground_throw(Fighter *fighter, bool lk);
static const MoveDef s_lk_throw = {.clip = 43, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 163, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lp_thrown = {.clip = 156, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
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

void sonya_script_throw_getup(Fighter *victim) {
  native_await(victim, native_grounded, throw_getup_land);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!mk4_can_ground_throw(fighter, lk)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, lk ? &s_lk_throw : &sonya_s_throw, lk ? &s_lk_thrown : &s_lp_thrown,
      lk ? sonya_entry_retail_script_004de360 : sonya_entry_retail_script_004de798);
}

void sonya_enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

void sonya_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

static void lk_victim_land(Fighter *victim) {
  native_await(victim, native_grounded, mk4_mmove_thrown_finish);
}

/* The victim advances seven poses and then completes clip 203 in one run. */
void sonya_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 203, 112, 75);
  native_finish_clip(victim, lk_victim_land);
}

void sonya_air_throw(Fighter *fighter) {
  if (!mk4_can_air_throw(fighter)) {
    return;
  }
  fighter_upright(fighter);
  fighter_upright(fighter_peer(fighter));
  mk4_mmove_throw_start(fighter, &s_air_owner, &s_air_victim, sonya_entry_retail_script_004e5ed0);
}
