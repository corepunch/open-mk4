#include "native_services.h"
#include "retail_data.h"

/* Native482aa0: the impact voice depends on the captured peer, independently
 * of the selected script/model aliases. Script words supply subsequent sound. */
static void stagger_voice(Fighter *fighter) {
  if (fighter_peer(fighter)->roster == 13) {
    retail_native_sound_list(0x4ea8c8);
    retail_native_sound_list(0x4ea870);
  } else {
    mk4_script_indexed_sound(fighter, 1);
  }
}

void retail_stagger_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  int previous_action = fighter->action;
  bool crouch = previous_action == 0x2001 || previous_action == 0x2002;
  if (entry == 0x482e00) {
    crouch |= previous_action == 0x2010 || previous_action == 0x106 || previous_action == 0x107;
  }
  if (entry == 0x482e00 || entry == 0x482830 || entry == 0x482870 || entry == 0x4828d0) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4001};
  fighter_start_move(fighter, S_HIT, &move);
  uint32_t script;
  switch (entry) {
    case 0x482e00:
      mk4_script_disarm_self(fighter);
      if (crouch) {
        uint32_t physics = mk4_image_u32(retail_vm_image(), 0x4e9358);
        require(retail_native_vm(fighter, physics, RETAIL_PHYSICS, 0), "Unknown crouch reaction motion callback");
      }
      script = crouch ? 0x4e9360 : 0x4e9308;
      break;
    case 0x482830:
      script = 0x4e90e0;
      break;
    case 0x482870:
    case 0x4828d0:
      mk4_script_disarm_self(fighter);
      retail_native_sound_list(0x4ea938);
      script = crouch ? 0x4e9170 : entry == 0x482870 ? 0x4e9130 : 0x4e9150;
      break;
    case 0x482be0:
      fighter->damage_scale = 0x9999;
      script = 0x4e9260;
      break;
    case 0x482e80:
    case 0x482e90:
      fighter->damage_scale = 0x8000;
      mk4_script_disarm_self(fighter);
      stagger_voice(fighter);
      fighter->action = ACTION_RETAIL_4003;
      script = 0x4e9388;
      break;
    case 0x482b10:
      fighter->action = 0x4002;
      stagger_voice(fighter);
      fighter->damage_scale = 0x9999;
      script = 0x4e9238;
      break;
    default:
      require(false, "Unexpected shared stagger native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

FighterFn retail_reaction_entry(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case 0x482e00:
    case 0x482830:
    case 0x482870:
    case 0x4828d0:
    case 0x482be0:
    case 0x482e80:
    case 0x482e90:
    case 0x482b10:
      return retail_stagger_reaction;
    default:
      return retail_throw_reaction(image, reaction);
  }
}
