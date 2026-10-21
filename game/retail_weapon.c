#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "native.h"
#include "native_services.h"

/* These are native call operands, not fighter configuration. Each original
 * frontend supplies one parameter row to494990 (or494950 without back testing). */
typedef struct WeaponNative {
  uint32_t address, parameters, motion;
  int action;
  bool close_throw, back_test;
} WeaponNative;

// clang-format off: Keep table entries on one row, including long rows.
static const WeaponNative weapon_natives[] = {
  {0x496c20, 0x4ec880, 0, 0x0, false, true},
  {0x496c30, 0x4ec898, 0, 0x0, true, true},
  {0x496a40, 0x4ec810, 0, 0x112, false, true},
  {0x496ba0, 0x4ec848, 0, 0x112, true, true},
  {0x4966b0, 0x4ec760, 0, 0x111, false, true},
  {0x4966e0, 0x4ec778, 0, 0x111, true, true},
  {0x4956c0, 0x4ec458, 0, 0x111, false, true},
  {0x495720, 0x4ec490, 0, 0x0, true, true},
  {0x495bb0, 0x4ec5d8, 0x4ec5c8, 0x0, false, true},
  {0x495be0, 0x4ec600, 0x4ec5f0, 0x0, true, true},
  {0x496cd0, 0x4ec988, 0, 0x0, false, true},
  {0x496cf0, 0x4ec9b0, 0, 0x0, true, true},
  {0x4955b0, 0x4ec420, 0, 0x111, false, true},
  {0x495a20, 0x4ec558, 0x4ec548, 0x0, false, true},
  {0x495a50, 0x4ec580, 0x4ec570, 0x0, true, true},
  {0x495830, 0x4ec4c8, 0, 0x111, false, true},
  {0x495980, 0x4ec510, 0, 0x111, true, true},
  {0x495800, 0x4ec9f8, 0, 0x111, false, true},
  {0x496100, 0x4ec658, 0, 0x111, true, true},
  {0x495d40, 0x4ec628, 0, 0x111, false, true},
  {0x4956f0, 0x4ec480, 0x4ec470, 0x0, false, false},
  {0x495740, 0x4ec4b8, 0x4ec4a8, 0x113, false, false},
  {0x496a70, 0x4ec838, 0x4ec828, 0x112, false, false},
  {0x496be0, 0x4ec870, 0x4ec860, 0x112, false, false},
  {0x4955e0, 0x4ec448, 0x4ec438, 0x0, false, false},
  {0x4959c0, 0x4ec538, 0x4ec528, 0x0, false, false},
  {0x496ce0, 0x4ec9a0, 0, 0x0, false, false},
  {0x495c20, 0x4ec618, 0, 0x0, false, false},
  {0x495710, 0x4ec480, 0, 0x0, false, false},
  {0x4959f0, 0x4eca18, 0x4eca08, 0x0, false, false},
  {0x495d20, 0x4eca28, 0, 0x0, false, false},
};
// clang-format on

static void weapon_native(Fighter *fighter, uint32_t address, bool back) {
  for (size_t i = 0; i < sizeof weapon_natives / sizeof *weapon_natives; ++i) {
    const WeaponNative *native = &weapon_natives[i];
    if (address != native->address) {
      continue;
    }
    if (native->close_throw && mk4_can_ground_throw(fighter, false)) {
      retail_lp_throw(fighter);
      return;
    }
    if (native->action != 0) {
      fighter->action = native->action;
    }
    if (native->back_test && back) {
      uint32_t alternative = mk4_image_u32(retail_vm_image(), native->parameters + 12);
      if (alternative != 0) {
        require(alternative != address, "Recursive native weapon alternative");
        weapon_native(fighter, alternative, false);
        return;
      }
    }
    int action = fighter->action;
    if (action != 0x111 && action != 0x112 && action != 0x113 && action != 0x212) {
      action = 0x10e;
    }
    weapon_start_retail_strike(fighter, native->parameters, native->motion, action, native->motion != 0);
    return;
  }
  switch (address) {
    case 0x4963e0:
      if (back) {
        retail_weapon_native_496670(fighter);
      } else {
        retail_weapon_native_4963e0(fighter);
      }
      break;
    case 0x4964d0:
      if (mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        weapon_native(fighter, 0x496920, false);
      } else {
        retail_weapon_native_4964d0(fighter);
      }
      break;
    case 0x495780:
      if (back) {
        weapon_native(fighter, 0x496a70, false);
      } else {
        weapon_start_retail_strike(fighter, 0x4ec9d8, 0, 0x112, false);
      }
      break;
    case 0x4957b0:
      if (back) {
        retail_weapon_native_495860(fighter);
      } else {
        weapon_start_retail_strike(fighter, 0x4ec9e8, 0, 0x111, false);
      }
      break;
    case 0x495c30:
      retail_weapon_native_495c30(fighter);
      break;
    case 0x495c60:
      if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        retail_weapon_native_495cc0(fighter);
      } else {
        retail_weapon_native_495c60(fighter);
      }
      break;
    case 0x495cf0:
      retail_weapon_native_495cf0(fighter);
      break;
    case 0x495610:
      retail_weapon_native_495610(fighter);
      break;
    case 0x496c50:
      retail_weapon_native_496c50(fighter);
      break;
    case 0x496c90:
      retail_weapon_native_496c90(fighter);
      break;
    case 0x496670:
      retail_weapon_native_496670(fighter);
      break;
    case 0x496920:
      retail_weapon_native_496920(fighter);
      break;
    case 0x496720:
      retail_weapon_native_496720(fighter);
      break;
    case 0x495a90:
      retail_weapon_native_495a90(fighter);
      break;
    case 0x495860:
      retail_weapon_native_495860(fighter);
      break;
    case 0x495d70:
      retail_weapon_native_495d70(fighter);
      break;
    case 0x496140:
      retail_weapon_native_496140(fighter);
      break;
    case 0x496d10:
      retail_weapon_native_496d10(fighter);
      break;
    default:
      require(false, "Unimplemented native weapon entry");
      break;
  }
}

static void weapon_input(Fighter *fighter, unsigned button, bool back) {
  require(fighter->armed && fighter->weapon != NULL && button < 2, "Native weapon input needs a held inventory");
  if (fighter_airborne(fighter)) {
    retail_air_attack(fighter, button);
    return;
  }
  uint32_t address = mk4_image_u32(retail_vm_image(), fighter->weapon->buttons + button * 4);
  weapon_native(fighter, address, back || fighter_holds_back(fighter));
}

void retail_weapon_high_punch(Fighter *fighter) {
  weapon_input(fighter, 0, false);
}

void retail_weapon_low_punch(Fighter *fighter) {
  weapon_input(fighter, 1, false);
}

void retail_weapon_back_high_punch(Fighter *fighter) {
  weapon_input(fighter, 0, true);
}

void retail_weapon_back_low_punch(Fighter *fighter) {
  weapon_input(fighter, 1, true);
}

static const WeaponDef *weapon_definition(unsigned roster) {
  static WeaponDef definitions[MK4_ROSTER_COUNT];
  require(roster < MK4_ROSTER_COUNT, "Invalid native inventory slot");
  RetailWeaponInventory inventory;
  retail_weapon_read(retail_vm_image(), roster, &inventory);
  definitions[roster] = (WeaponDef){.type = (int)inventory.type,
      .model = mk4_weapon_model_index(retail_vm_image(), inventory.address),
      .inventory = inventory.address,
      .buttons = inventory.buttons,
      .idle = inventory.idle,
      .forward = inventory.forward,
      .backward = inventory.backward};
  return &definitions[roster];
}

void retail_weapon_equip(Fighter *fighter) {
  /* Original493080 refuses weapon creation for roster15. */
  if (fighter->roster != 15) {
    fighter_equip_weapon(fighter, weapon_definition((unsigned)fighter->roster));
  }
}

void retail_weapon_draw_equip(Fighter *fighter) {
  retail_weapon_equip(fighter);
  retail_vm_enter(fighter, 0x4e62d8, fighter_ready);
}

void retail_weapon_draw(Fighter *fighter) {
  if (fighter->armed) {
    weapon_throw(fighter);
    return;
  }
  if (fighter->roster == 15) {
    fighter_ready(fighter);
    return;
  }
  mk4_special_stamp(fighter, 14);
  if (fighter->flags & FF_FOLLOW_UP) {
    retail_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    require(fighter->weapon->type >= 0x60 && fighter->weapon->type <= 0x6f, "Invalid weapon followup type");
    uint32_t target = mk4_image_u32(retail_vm_image(), 0x4ecb40 + (unsigned)(fighter->weapon->type - 0x60) * 4);
    weapon_native(fighter, target, false);
  } else {
    weapon_draw(fighter, weapon_definition((unsigned)fighter->roster));
    retail_vm_enter(fighter, 0x4e62c0, fighter_ready);
  }
}

/* 43a5c0/43a610 visits resting object descriptors of retail type6c. */
void retail_disturb_resting_weapons(void) {
  weapon_hop_resting(0x6c);
}
