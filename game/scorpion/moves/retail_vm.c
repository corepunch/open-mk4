#include "../character.h"
#include "retail_vm.h"

static bool native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  if (kind == RETAIL_CALL && address == 0x46a950) {
    mk4_script_avoid_clear(fighter);
  } else if (kind == RETAIL_CALL && address == 0x46a970) {
    mk4_script_avoid_set(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46a8d0) {
    /* Checked native tail: 429380/429600 reverses to zero, then 46f650
     * returns to the controller. Keep it on the VM's animation clock. */
    retail_vm_reverse_return(fighter);
  } else if (kind == RETAIL_SPAWN && address == 0x49a6a0 && argument == 67) {
    scorpion_script_spawn_spear(fighter, (int)argument);
  } else if (kind == RETAIL_TAIL && address == 0x486000) {
    scorpion_script_spear_pull(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x487420) {
    scorpion_spear_stun_script(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x49a680) {
    scorpion_script_spear_return(fighter);
  } else if (kind == RETAIL_HANDLER && address == 0x45b1f0 && argument == 71) {
    scorpion_script_reaction(fighter, (int)argument);
  } else if (kind == RETAIL_TAIL && address == 0x46a760) {
    scorpion_script_kick_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f080) {
    scorpion_script_sweep_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46cde0) {
    scorpion_uppercut_script(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46b740) {
    scorpion_script_crouch_lk_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46bb60) {
    scorpion_script_knee_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46bf10) {
    scorpion_script_close_hp_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46c2c0) {
    scorpion_script_combo_hp_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f220) {
    scorpion_script_roundhouse_start(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f270) {
    scorpion_script_roundhouse_tail(fighter);
  } else {
    return false;
  }
  return true;
}

bool scorpion_try_retail_vm(Fighter *fighter, uint32_t entry) {
  const Mk4Image *image = retail_vm_image();
  FighterFn finish = scorpion_ready;
  uint32_t end;
  switch (entry) {
    case 0x4e6d08:
      end = 0x4e6d9c;
      break;
    case 0x4e60b8:
      end = 0x4e610c;
      break;
    case 0x4ed448:
      end = 0x4ed4a4;
      finish = scorpion_script_spear_done;
      break;
    case 0x4ed4a8:
      end = 0x4ed4c0;
      break;
    case 0x4e9d00:
      end = 0x4e9d1c;
      break;
    case 0x4e9f20:
      end = 0x4e9f30;
      break;
    case 0x4e5ca0:
      end = 0x4e6cb0;
      break;
    case 0x4e5c70:
      end = 0x4e5c80;
      break;
    case 0x4e6798:
      end = 0x4e6e8c;
      break;
    case 0x4e62a0:
      end = 0x4e62bc;
      break;
    case 0x4e6158:
      end = 0x4e617c;
      break;
    case 0x4e6190:
      end = 0x4e61c8;
      break;
    case 0x4e61e0:
      end = 0x4e61f4;
      break;
    case 0x4e69b0:
      end = 0x4e69c4;
      break;
    case 0x4e6844:
      end = 0x4e685c;
      break;
    case 0x4e6860:
      end = 0x4e6874;
      break;
    case 0x4e6878:
      end = 0x4e688c;
      break;
    default:
      return false;
  }
  retail_vm_start(fighter, image, entry, end, finish, native);
  return true;
}
