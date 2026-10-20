#include "../character.h"
#include "retail_vm.h"

static bool native(Fighter *fighter, uint32_t address, bool tail) {
  if (!tail && address == 0x46a950) {
    mk4_script_avoid_clear(fighter);
  } else if (!tail && address == 0x46a970) {
    mk4_script_avoid_set(fighter);
  } else if (tail && address == 0x46a8d0) {
    /* Checked native tail: 429380/429600 reverses to zero, then 46f650
     * returns to the controller. Keep it on the VM's animation clock. */
    retail_vm_reverse_return(fighter);
  } else {
    return false;
  }
  return true;
}

bool scorpion_try_retail_vm(Fighter *fighter, uint32_t entry) {
  const Mk4Image *image = g_battle.retail_scripts;
  if (image == NULL) {
    return false;
  }
  require(fighter->roster == 0, "Scorpion VM adapter used by another roster");
  uint32_t end;
  switch (entry) {
    case 0x4e6d08:
      end = 0x4e6d9c;
      break;
    case 0x4e60b8:
      end = 0x4e610c;
      break;
    default:
      return false;
  }
  retail_vm_start(fighter, image, entry, end, scorpion_ready, native);
  return true;
}
