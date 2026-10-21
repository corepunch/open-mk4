#include "../character.h"

static void throw_getup_land(Fighter *victim);
static void ground_throw(Fighter *fighter);
static const MoveDef s_lk_throw = {.clip = 33, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0501};
static const MoveDef s_lk_thrown = {.clip = 149, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* 485ad0: after landing, a living victim turns and plays getup 328 through
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

void raiden_script_throw_getup(Fighter *victim) {
  native_await(victim, native_grounded, throw_getup_land);
}

static void ground_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, true)) {
    fighter_ready(fighter);
    return;
  }
  mk4_mmove_throw_start(fighter, &s_lk_throw, &s_lk_thrown, raiden_entry_retail_script_004de218);
}

void raiden_enter_throw(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    mk4_air_throw_frames(fighter, raiden_entry_retail_script_004e5e28, raiden_entry_retail_script_004e9910);
    return;
  }
  retail_lp_throw(fighter);
}

void raiden_enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter);
}

void raiden_script_thrown_return(Fighter *fighter) {
  if (fighter->reaction == 102 && fighter_move(fighter)->clip == 143) {
    raiden_script_throw_getup(fighter);
    return;
  }
  fighter_finish_mmove(fighter, fighter_ready);
}

void raiden_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 169, 43, 0);
  fighter_set_speed(victim, 0xf5c); /* 490ab0 consumes 4e99bc's speed. */
  raiden_entry_retail_script_004e99c0(victim);
}
