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
  {RETAIL_NATIVE_SONYA_DRAW_WEAPON_HP_SLASH, MK4_RETAIL_DATA_WEAPON_CLIP_235_QUERY_1_PARAMETERS, 0, 0x0, false, true},
  {RETAIL_NATIVE_SONYA_DRAW_WEAPON_LP_SLASH, MK4_RETAIL_DATA_WEAPON_CLIP_236_QUERY_0_PARAMETERS, 0, 0x0, true, true},
  {RETAIL_NATIVE_RAIDEN_STAFF_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_233_QUERY_4_PARAMETERS, 0, 0x112, false, true},
  {RETAIL_NATIVE_RAIDEN_STAFF_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_234_QUERY_5_PARAMETERS, 0, 0x112, true, true},
  {RETAIL_NATIVE_SONYA_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_230_QUERY_8_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_SONYA_WEAPON_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_9_PARAMETERS, 0, 0x111, true, true},
  {RETAIL_NATIVE_SUB_ZERO_SCEPTER_HP_ENTRY, MK4_RETAIL_DATA_SCEPTER_HIGH_PUNCH_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_SUB_ZERO_SCEPTER_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_250_QUERY_38_PARAMETERS, 0, 0x0, true, true},
  {RETAIL_NATIVE_SHINNOK_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_237_QUERY_28_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_237_QUERY_28_MOTION, 0x0, false, true},
  {RETAIL_NATIVE_SHINNOK_WEAPON_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_238_QUERY_29_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_238_QUERY_29_MOTION, 0x0, true, true},
  {RETAIL_NATIVE_JAX_AXE_HP_ENTRY, MK4_RETAIL_DATA_AXE_HIGH_PUNCH_PARAMETERS, 0, 0x0, false, true},
  {RETAIL_NATIVE_JAX_AXE_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_232_QUERY_24_PARAMETERS, 0, 0x0, true, true},
  {RETAIL_NATIVE_TANYA_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_42_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_KAI_WEAPON_HIGH_PUNCH_FRONTEND, MK4_RETAIL_DATA_KAI_WEAPON_HIGH_PUNCH_PARAMETERS, MK4_RETAIL_DATA_KAI_WEAPON_HIGH_PUNCH_MOTION, 0x0, false, true},
  {RETAIL_NATIVE_KAI_WEAPON_LOW_PUNCH_FRONTEND, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_32_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_32_MOTION, 0x0, true, true},
  {RETAIL_NATIVE_JAREK_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_239_QUERY_37_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_JAREK_WEAPON_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_36_PARAMETERS, 0, 0x111, true, true},
  {RETAIL_NATIVE_CAGE_WEAPON_LP_ENTRY, MK4_RETAIL_DATA_CAGE_WEAPON_LOW_PUNCH_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_REPTILE_WEAPON_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_240_QUERY_21_PARAMETERS, 0, 0x111, true, true},
  {RETAIL_NATIVE_REPTILE_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_REPTILE_WEAPON_HIGH_PUNCH_PARAMETERS, 0, 0x111, false, true},
  {RETAIL_NATIVE_SUB_ZERO_SCEPTER_BACK_HP_ENTRY, MK4_RETAIL_DATA_SCEPTER_BACK_HIGH_PUNCH_PARAMETERS, MK4_RETAIL_DATA_SCEPTER_BACK_HIGH_PUNCH_MOTION, 0x0, false, false},
  {RETAIL_NATIVE_SUB_ZERO_SCEPTER_BACK_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_228_QUERY_39_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_228_QUERY_39_MOTION, 0x113, false, false},
  {RETAIL_NATIVE_QUAN_CHI_WEAPON_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_238_QUERY_7_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_238_QUERY_7_MOTION, 0x112, false, false},
  {RETAIL_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY, MK4_RETAIL_DATA_STAFF_BACK_LOW_PUNCH_PARAMETERS, MK4_RETAIL_DATA_STAFF_BACK_LOW_PUNCH_MOTION, 0x112, false, false},
  {RETAIL_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_231_QUERY_43_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_231_QUERY_43_MOTION, 0x0, false, false},
  {RETAIL_NATIVE_JAREK_WEAPON_BACK_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_228_QUERY_35_PARAMETERS, MK4_RETAIL_DATA_WEAPON_CLIP_228_QUERY_35_MOTION, 0x0, false, false},
  {RETAIL_NATIVE_JAX_AXE_BACK_HP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_229_QUERY_23_PARAMETERS, 0, 0x0, false, false},
  {RETAIL_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY, MK4_RETAIL_DATA_WEAPON_CLIP_243_QUERY_30_PARAMETERS, 0, 0x0, false, false},
  {RETAIL_NATIVE_SUB_ZERO_DRAW_WEAPON_SCEPTER_FOLLOW_UP, MK4_RETAIL_DATA_SCEPTER_BACK_HIGH_PUNCH_PARAMETERS, 0, 0x0, false, false},
  {RETAIL_NATIVE_KAI_WEAPON_FOLLOWUP_ENTRY, MK4_RETAIL_DATA_KAI_WEAPON_FOLLOWUP_PARAMETERS, MK4_RETAIL_DATA_KAI_WEAPON_FOLLOWUP_MOTION, 0x0, false, false},
  {RETAIL_NATIVE_REPTILE_WEAPON_ALTERNATE_HIGH_PUNCH, MK4_RETAIL_DATA_REPTILE_WEAPON_BACK_HIGH_PUNCH_PARAMETERS, 0, 0x0, false, false},
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
    case RETAIL_NATIVE_LIU_WEAPON_HIGH_PUNCH:
      if (back) {
        retail_liu_kang_sword_back_high_punch(fighter);
      } else {
        retail_liu_kang_sword_high_punch(fighter);
      }
      break;
    case RETAIL_NATIVE_LIU_WEAPON_LOW_PUNCH:
      if (mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        weapon_native(fighter, RETAIL_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY, false);
      } else {
        retail_liu_kang_sword_low_punch(fighter);
      }
      break;
    case RETAIL_NATIVE_QUAN_CHI_WEAPON_BACK_HP_ENTRY:
      if (back) {
        weapon_native(fighter, RETAIL_NATIVE_QUAN_CHI_WEAPON_HP_ENTRY, false);
      } else {
        weapon_start_retail_strike(fighter, MK4_RETAIL_DATA_WEAPON_CLIP_231_PARAMETERS, 0, 0x112, false);
      }
      break;
    case RETAIL_NATIVE_CAGE_WEAPON_BACK_HP_ENTRY:
      if (back) {
        retail_cage_weapon_back_upper(fighter);
      } else {
        weapon_start_retail_strike(fighter, MK4_RETAIL_DATA_WEAPON_CLIP_239_PARAMETERS, 0, 0x111, false);
      }
      break;
    case RETAIL_NATIVE_FUJIN_CROSSBOW_HP_ENTRY:
      retail_bow_high_punch(fighter);
      break;
    case RETAIL_NATIVE_FUJIN_CROSSBOW_LP_ENTRY:
      if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
        retail_lp_throw(fighter);
      } else if (back) {
        retail_bow_back_strike(fighter);
      } else {
        retail_bow_low_punch(fighter);
      }
      break;
    case RETAIL_NATIVE_FUJIN_CROSSBOW_FOLLOWUP_ENTRY:
      retail_bow_followup_strike(fighter);
      break;
    case RETAIL_NATIVE_TANYA_WEAPON_LP_ENTRY:
      retail_boomerang_low_punch(fighter);
      break;
    case RETAIL_NATIVE_SWORD_BACK_LP:
      retail_sword_back_low_punch(fighter);
      break;
    case RETAIL_NATIVE_SWORD_BACK_HP:
      retail_sword_back_high_punch(fighter);
      break;
    case RETAIL_NATIVE_LIU_KANG_SWORD_BACK_HP_ENTRY:
      retail_liu_kang_sword_back_high_punch(fighter);
      break;
    case RETAIL_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY:
      retail_liu_kang_sword_back_low_punch(fighter);
      break;
    case RETAIL_NATIVE_SONYA_WEAPON_BACK_HP_ENTRY:
      retail_sonya_weapon_back_high_punch(fighter);
      break;
    case RETAIL_NATIVE_KAI_WEAPON_BACK_STRIKE_FRONTEND:
      retail_kai_weapon_back_strike(fighter);
      break;
    case RETAIL_NATIVE_CAGE_WEAPON_UPPER_ENTRY:
      retail_cage_weapon_back_upper(fighter);
      break;
    case RETAIL_NATIVE_REPTILE_WEAPON_BACK_HP_ENTRY:
      retail_reptile_weapon_flurry(fighter);
      break;
    case RETAIL_NATIVE_REPTILE_WEAPON_BACK_LP_ENTRY:
      retail_reptile_weapon_back_low_punch(fighter);
      break;
    case RETAIL_NATIVE_JAX_AXE_BACK_LP_ENTRY:
      retail_axe_back_low_punch(fighter);
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
  retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_SHINNOK_DRAW_WEAPON_EQUIP, fighter_ready);
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
    uint32_t target = mk4_image_u32(retail_vm_image(), MK4_RETAIL_DATA_WEAPON_DRAW_FOLLOWUP_ENTRIES + (unsigned)(fighter->weapon->type - 0x60) * 4);
    weapon_native(fighter, target, false);
  } else {
    weapon_draw(fighter, weapon_definition((unsigned)fighter->roster));
    retail_vm_enter(fighter, MK4_RETAIL_SCRIPT_CAGE_DRAW_WEAPON_DRAW, fighter_ready);
  }
}

/* 43a5c0/43a610 visits resting object descriptors of retail type6c. */
void retail_disturb_resting_weapons(void) {
  weapon_hop_resting(0x6c);
}
