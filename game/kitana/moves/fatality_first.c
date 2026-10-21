#include "retail_vm.h"
#include "../character.h"

void kitana_script_first_release(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  fighter->body.physics = kitana_scene_end_watch;
  retail_vm_enter_owned(fighter, 0x4e01d0, fighter_ready, 16);
}
