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
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_496C20, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC880, 0, 0x0, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_496C30, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC898, 0, 0x0, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_496A40, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC810, 0, 0x112, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_496BA0, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC848, 0, 0x112, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_4966B0, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC760, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_4966E0, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC778, 0, 0x111, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_4956C0, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC458, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_495720, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC490, 0, 0x0, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_495BB0, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC5D8, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC5C8, 0x0, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_495BE0, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC600, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC5F0, 0x0, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_496CD0, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC988, 0, 0x0, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_496CF0, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC9B0, 0, 0x0, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_4955B0, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC420, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_495A20, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC558, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC548, 0x0, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_495A50, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC580, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC570, 0x0, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_495830, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC4C8, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_495980, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC510, 0, 0x111, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_495800, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC9F8, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_LP_THROW_OR_STRIKE_496100, MK4_RETAIL_DATA_WEAPON_LP_THROW_OR_STRIKE_PARAMETERS_4EC658, 0, 0x111, true, true},
  {RETAIL_NATIVE_WEAPON_HP_STRIKE_495D40, MK4_RETAIL_DATA_WEAPON_HP_STRIKE_PARAMETERS_4EC628, 0, 0x111, false, true},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_4956F0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC480, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC470, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_495740, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC4B8, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC4A8, 0x113, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_496A70, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC838, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC828, 0x112, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_496BE0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC870, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC860, 0x112, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_4955E0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC448, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC438, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_4959C0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC538, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC528, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_496CE0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC9A0, 0, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_495C20, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC618, 0, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_495710, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4EC480, 0, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_4959F0, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4ECA18, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4ECA08, 0x0, false, false},
  {RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_495D20, MK4_RETAIL_DATA_WEAPON_ALTERNATE_STRIKE_PARAMETERS_4ECA28, 0, 0x0, false, false},
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
    case RETAIL_NATIVE_LIU_WEAPON_HIGH_PUNCH_4963E0:
      if (back) {
        retail_weapon_native_496670(fighter);
      } else {
        retail_weapon_native_4963e0(fighter);
      }
      break;
    case RETAIL_NATIVE_LIU_WEAPON_LOW_PUNCH_4964D0:
      if (mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        weapon_native(fighter, RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496920, false);
      } else {
        retail_weapon_native_4964d0(fighter);
      }
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495780:
      if (back) {
        weapon_native(fighter, RETAIL_NATIVE_WEAPON_ALTERNATE_STRIKE_496A70, false);
      } else {
        weapon_start_retail_strike(fighter, MK4_RETAIL_DATA_WEAPON_NATIVE_4EC9D8, 0, 0x112, false);
      }
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_4957B0:
      if (back) {
        retail_weapon_native_495860(fighter);
      } else {
        weapon_start_retail_strike(fighter, MK4_RETAIL_DATA_WEAPON_NATIVE_4EC9E8, 0, 0x111, false);
      }
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495C30:
      retail_weapon_native_495c30(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495C60:
      if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        retail_weapon_native_495cc0(fighter);
      } else {
        retail_weapon_native_495c60(fighter);
      }
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495CF0:
      retail_weapon_native_495cf0(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495610:
      retail_weapon_native_495610(fighter);
      break;
    case RETAIL_NATIVE_SWORD_BACK_LP_496C50:
      retail_weapon_native_496c50(fighter);
      break;
    case RETAIL_NATIVE_SWORD_BACK_HP_496C90:
      retail_weapon_native_496c90(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496670:
      retail_weapon_native_496670(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496920:
      retail_weapon_native_496920(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496720:
      retail_weapon_native_496720(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495A90:
      retail_weapon_native_495a90(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495860:
      retail_weapon_native_495860(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_495D70:
      retail_weapon_native_495d70(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496140:
      retail_weapon_native_496140(fighter);
      break;
    case RETAIL_NATIVE_WEAPON_PARAMETER_ENTRY_496D10:
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
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_WEAPON_DRAW_EQUIP_4E62D8, fighter_ready);
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
    uint32_t target = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_WEAPON_DRAW_4ECB40 + (unsigned)(fighter->weapon->type - 0x60) * 4);
    weapon_native(fighter, target, false);
  } else {
    weapon_draw(fighter, weapon_definition((unsigned)fighter->roster));
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_WEAPON_DRAW_4E62C0, fighter_ready);
  }
}

/* 43a5c0/43a610 visits resting object descriptors of retail type6c. */
void retail_disturb_resting_weapons(void) {
  weapon_hop_resting(0x6c);
}
