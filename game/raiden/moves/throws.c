#include "retail_vm.h"
#include "../character.h"

static void throw_getup_land(Fighter *victim);

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
  retail_vm_enter_owned(victim, 0x4e99c0, fighter_ready, 1);
}
