#include "../character.h"

static void throw_getup_land(Fighter *victim);
static void lk_victim_land(Fighter *victim);

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

static void lk_victim_land(Fighter *victim) {
  native_await(victim, native_grounded, mk4_mmove_thrown_finish);
}

/* The victim advances seven poses and then completes clip 203 in one run. */
void sonya_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 203, 112, 75);
  native_finish_clip(victim, lk_victim_land);
}
