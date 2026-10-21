#include "retail_vm.h"
#include "../character.h"

void sub_zero_second_fatality_script(Fighter *fighter) {
  retail_vm_enter_owned(fighter_peer(fighter), 0x4d2bf0, fighter_ready, 4);
  retail_vm_enter_owned(fighter, 0x4d2c10, fighter_ready, 4);
}

void sub_zero_second_fatality(Fighter *fighter) {
  sub_zero_start_fatality(fighter, FINISH_SUB_ZERO_2);
}
