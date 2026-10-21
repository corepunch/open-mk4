#include "retail_vm.h"
#include "../character.h"

void raiden_script_thrown_return(Fighter *fighter) {
  if (fighter->reaction == 102 && fighter_move(fighter)->clip == 143) {
    retail_getup_flip(fighter);
    return;
  }
  fighter_finish_mmove(fighter, fighter_ready);
}

void raiden_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 169, 43, 0);
  fighter_set_speed(victim, 0xf5c); /* 490ab0 consumes 4e99bc's speed. */
  retail_vm_enter_owned(victim, 0x4e99c0, fighter_ready, 1);
}
