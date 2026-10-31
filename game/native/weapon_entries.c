#include "script/native_ids.h"
#include "native/actions.h"
#include "data/tables.h"
#include "script/vm.h"
#include "runtime/playback.h"
#include "native/capabilities.h"

/* These are native call operands, not fighter configuration. Each original
 * frontend supplies one parameter row to494990 (or494950 without back testing). */
typedef struct WeaponNative {
  uint32_t address, parameters, motion;
  int action;
  bool close_throw, back_test;
} WeaponNative;

// clang-format off: Keep table entries on one row, including long rows.
static const WeaponNative weapon_natives[] = {
  {native_sonya_draw_weapon_hp_slash, dat_weapon_clip_235_query_1_parameters, 0, 0x0, false, true},
  {native_sonya_draw_weapon_lp_slash, dat_weapon_clip_236_query_0_parameters, 0, 0x0, true, true},
  {native_raiden_staff_hp_entry, dat_weapon_clip_233_query_4_parameters, 0, 0x112, false, true},
  {native_raiden_staff_lp_entry, dat_weapon_clip_234_query_5_parameters, 0, 0x112, true, true},
  {native_sonya_weapon_hp_entry, dat_weapon_clip_230_query_8_parameters, 0, 0x111, false, true},
  {native_sonya_weapon_lp_entry, dat_weapon_clip_229_query_9_parameters, 0, 0x111, true, true},
  {native_sub_zero_scepter_hp_entry, dat_scepter_high_punch_parameters, 0, 0x111, false, true},
  {native_sub_zero_scepter_lp_entry, dat_weapon_clip_250_query_38_parameters, 0, 0x0, true, true},
  {native_shinnok_weapon_hp_entry, dat_weapon_clip_237_query_28_parameters, dat_weapon_clip_237_query_28_motion, 0x0, false, true},
  {native_shinnok_weapon_lp_entry, dat_weapon_clip_238_query_29_parameters, dat_weapon_clip_238_query_29_motion, 0x0, true, true},
  {native_jax_axe_hp_entry, dat_axe_high_punch_parameters, 0, 0x0, false, true},
  {native_jax_axe_lp_entry, dat_weapon_clip_232_query_24_parameters, 0, 0x0, true, true},
  {native_tanya_weapon_hp_entry, dat_weapon_clip_229_query_42_parameters, 0, 0x111, false, true},
  {native_kai_weapon_high_punch_frontend, dat_kai_weapon_high_punch_parameters, dat_kai_weapon_high_punch_motion, 0x0, false, true},
  {native_kai_weapon_low_punch_frontend, dat_weapon_clip_229_query_32_parameters, dat_weapon_clip_229_query_32_motion, 0x0, true, true},
  {native_jarek_weapon_hp_entry, dat_weapon_clip_239_query_37_parameters, 0, 0x111, false, true},
  {native_jarek_weapon_lp_entry, dat_weapon_clip_229_query_36_parameters, 0, 0x111, true, true},
  {native_cage_weapon_lp_entry, dat_cage_weapon_low_punch_parameters, 0, 0x111, false, true},
  {native_reptile_weapon_lp_entry, dat_weapon_clip_240_query_21_parameters, 0, 0x111, true, true},
  {native_reptile_weapon_hp_entry, dat_reptile_weapon_high_punch_parameters, 0, 0x111, false, true},
  {native_sub_zero_scepter_back_hp_entry, dat_scepter_back_high_punch_parameters, dat_scepter_back_high_punch_motion, 0x0, false, false},
  {native_sub_zero_scepter_back_lp_entry, dat_weapon_clip_228_query_39_parameters, dat_weapon_clip_228_query_39_motion, 0x113, false, false},
  {native_quan_chi_weapon_hp_entry, dat_weapon_clip_238_query_7_parameters, dat_weapon_clip_238_query_7_motion, 0x112, false, false},
  {native_raiden_staff_back_lp_entry, dat_staff_back_low_punch_parameters, dat_staff_back_low_punch_motion, 0x112, false, false},
  {native_tanya_weapon_back_hp_entry, dat_weapon_clip_231_query_43_parameters, dat_weapon_clip_231_query_43_motion, 0x0, false, false},
  {native_jarek_weapon_back_lp_entry, dat_weapon_clip_228_query_35_parameters, dat_weapon_clip_228_query_35_motion, 0x0, false, false},
  {native_jax_axe_back_hp_entry, dat_weapon_clip_229_query_23_parameters, 0, 0x0, false, false},
  {native_shinnok_weapon_back_lp_entry, dat_weapon_clip_243_query_30_parameters, 0, 0x0, false, false},
  {native_sub_zero_draw_weapon_scepter_follow_up, dat_scepter_back_high_punch_parameters, 0, 0x0, false, false},
  {native_kai_weapon_followup_entry, dat_kai_weapon_followup_parameters, dat_kai_weapon_followup_motion, 0x0, false, false},
  {native_reptile_weapon_alternate_high_punch, dat_reptile_weapon_back_high_punch_parameters, 0, 0x0, false, false},
};
// clang-format on

static void weapon_native(Fighter *fighter, uint32_t address, bool back) {
  for (size_t i = 0; i < sizeof weapon_natives / sizeof *weapon_natives; ++i) {
    const WeaponNative *native = &weapon_natives[i];
    if (address != native->address) {
      continue;
    }
    if (native->close_throw && mk4_can_ground_throw(fighter, false)) {
      action_lp_throw(fighter);
      return;
    }
    if (native->action != 0) {
      fighter->action = native->action;
    }
    if (native->back_test && back) {
      uint32_t alternative = mk4_image_u32(battle_data(), native->parameters + 12);
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
    weapon_start_encoded_strike(fighter, native->parameters, native->motion, action, native->motion != 0);
    return;
  }
  switch (address) {
    case native_liu_weapon_high_punch:
      if (back) {
        action_liu_kang_sword_back_high_punch(fighter);
      } else {
        action_liu_kang_sword_high_punch(fighter);
      }
      break;
    case native_liu_weapon_low_punch:
      if (mk4_can_ground_throw(fighter, false)) {
        action_lp_throw(fighter);
      } else if (back) {
        weapon_native(fighter, native_cage_weapon_back_lp_entry, false);
      } else {
        action_liu_kang_sword_low_punch(fighter);
      }
      break;
    case native_quan_chi_weapon_back_hp_entry:
      if (back) {
        weapon_native(fighter, native_quan_chi_weapon_hp_entry, false);
      } else {
        weapon_start_encoded_strike(fighter, dat_weapon_clip_231_parameters, 0, 0x112, false);
      }
      break;
    case native_cage_weapon_back_hp_entry:
      if (back) {
        action_cage_weapon_back_upper(fighter);
      } else {
        weapon_start_encoded_strike(fighter, dat_weapon_clip_239_parameters, 0, 0x111, false);
      }
      break;
    case native_fujin_crossbow_hp_entry:
      action_bow_high_punch(fighter);
      break;
    case native_fujin_crossbow_lp_entry:
      if (!(fighter->flags & FF_FOLLOW_UP) && mk4_can_ground_throw(fighter, false)) {
        action_lp_throw(fighter);
      } else if (back) {
        action_bow_back_strike(fighter);
      } else {
        action_bow_low_punch(fighter);
      }
      break;
    case native_fujin_crossbow_followup_entry:
      action_bow_followup_strike(fighter);
      break;
    case native_tanya_weapon_lp_entry:
      action_boomerang_low_punch(fighter);
      break;
    case native_sword_back_lp:
      action_sword_back_low_punch(fighter);
      break;
    case native_sword_back_hp:
      action_sword_back_high_punch(fighter);
      break;
    case native_liu_kang_sword_back_hp_entry:
      action_liu_kang_sword_back_high_punch(fighter);
      break;
    case native_cage_weapon_back_lp_entry:
      action_liu_kang_sword_back_low_punch(fighter);
      break;
    case native_sonya_weapon_back_hp_entry:
      action_sonya_weapon_back_high_punch(fighter);
      break;
    case native_kai_weapon_back_strike_frontend:
      action_kai_weapon_back_strike(fighter);
      break;
    case native_cage_weapon_upper_entry:
      action_cage_weapon_back_upper(fighter);
      break;
    case native_reptile_weapon_back_hp_entry:
      action_reptile_weapon_flurry(fighter);
      break;
    case native_reptile_weapon_back_lp_entry:
      action_reptile_weapon_back_low_punch(fighter);
      break;
    case native_jax_axe_back_lp_entry:
      action_axe_back_low_punch(fighter);
      break;
    default:
      require(false, "Unimplemented native weapon entry");
      break;
  }
}

static void weapon_input(Fighter *fighter, unsigned button, bool back) {
  require(fighter->armed && fighter->weapon != NULL && button < 2, "Native weapon input needs a held inventory");
  if (fighter_airborne(fighter)) {
    action_air_attack(fighter, button);
    return;
  }
  uint32_t address = mk4_image_u32(battle_data(), fighter->weapon->buttons + button * 4);
  weapon_native(fighter, address, back || fighter_holds_back(fighter));
}

void action_weapon_high_punch(Fighter *fighter) {
  weapon_input(fighter, 0, false);
}

void action_weapon_low_punch(Fighter *fighter) {
  weapon_input(fighter, 1, false);
}

void action_weapon_back_high_punch(Fighter *fighter) {
  weapon_input(fighter, 0, true);
}

void action_weapon_back_low_punch(Fighter *fighter) {
  weapon_input(fighter, 1, true);
}

static const WeaponDef *weapon_definition(unsigned roster) {
  static WeaponDef definitions[MK4_ROSTER_COUNT];
  require(roster < MK4_ROSTER_COUNT, "Invalid native inventory slot");
  Mk4WeaponInventory inventory;
  mk4_weapon_read(battle_data(), roster, &inventory);
  definitions[roster] = (WeaponDef){.type = (int)inventory.type,
      .model = mk4_weapon_model_index(battle_data(), inventory.address),
      .inventory = inventory.address,
      .buttons = inventory.buttons,
      .idle = inventory.idle,
      .forward = inventory.forward,
      .backward = inventory.backward};
  return &definitions[roster];
}

void action_weapon_equip(Fighter *fighter) {
  /* Original493080 refuses weapon creation for roster15. */
  if (fighter->roster != 15) {
    fighter_equip_weapon(fighter, weapon_definition((unsigned)fighter->roster));
  }
}

void action_weapon_draw_equip(Fighter *fighter) {
  action_weapon_equip(fighter);
  script_vm_enter(fighter, fn_shinnok_draw_weapon_equip, fighter_ready);
}

void action_weapon_draw(Fighter *fighter) {
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
    action_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    require(fighter->weapon->type >= 0x60 && fighter->weapon->type <= 0x6f, "Invalid weapon followup type");
    uint32_t target = mk4_image_u32(battle_data(), dat_weapon_draw_followup_entries + (unsigned)(fighter->weapon->type - 0x60) * 4);
    weapon_native(fighter, target, false);
  } else {
    weapon_draw(fighter, weapon_definition((unsigned)fighter->roster));
    script_vm_enter(fighter, fn_cage_draw_weapon_draw, fighter_ready);
  }
}

/* 43a5c0/43a610 visits resting object descriptors of retail type6c. */
void action_disturb_resting_weapons(void) {
  weapon_hop_resting(0x6c);
}
