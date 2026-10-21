#include "retail_vm.h"
#include "../character.h"

void kitana_script_weapon_victim_getup(Fighter *victim) {
  native_finish_clip(victim, retail_getup_roll);
}
