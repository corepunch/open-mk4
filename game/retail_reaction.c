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

void retail_launch_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry != 0x4835e0) {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x481320) {
    battle_wall_recoil(fighter);
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  switch (entry) {
    case 0x481320:
      script = 0x4e89a0;
      break;
    case 0x482560:
      script = 0x4e8ea8;
      break;
    case 0x4825c0:
      script = 0x4e8f10;
      break;
    case 0x4835e0:
      script = 0x4e96c8;
      break;
    default:
      require(false, "Unexpected shared launch native");
      return;
  }
  retail_vm_enter(fighter, script, mk4_script_thrown_ready);
}

/* 482790 and483610 both finish the inherited descriptor; their native
 * completion differs. Engine continuations replace the task84 resume flag. */
void retail_reaction_getup_back(Fighter *fighter) {
  mk4_mmove_finish_getup(fighter, 0x4ea210);
}

void retail_reaction_finish(Fighter *fighter) {
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

void retail_reaction_fall(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  retail_vm_enter(fighter, 0x4e96e0, mk4_script_thrown_ready);
}

void retail_tumble_reaction(Fighter *fighter) {
  uint32_t entry = retail_reaction_target(retail_vm_image(), (unsigned)fighter->reaction);
  if (entry == 0x482730) {
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  } else {
    mk4_script_disarm_self(fighter);
  }
  if (entry == 0x483450 || entry == 0x483490) {
    battle_wall_recoil(fighter);
  }
  if (entry == 0x4826a0) {
    const Fighter *peer = fighter_peer(fighter);
    RetailWeaponInventory inventory;
    retail_weapon_read(retail_vm_image(), (unsigned)peer->roster, &inventory);
    int type = peer->weapon != NULL ? peer->weapon->type : (int)inventory.type;
    if (type == 0x61) {
      mk4_script_indexed_sound(fighter, 2);
    } else if (type != 0x69) {
      retail_native_sound_list(0x4ea880);
    }
  }
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  uint32_t script;
  FighterFn finish = mk4_script_thrown_ready;
  switch (entry) {
    case 0x483450:
      script = 0x4e9508;
      finish = retail_getup_back;
      break;
    case 0x483490:
      script = 0x4e9568;
      finish = retail_getup_back;
      break;
    case 0x482750:
      script = 0x4e9048;
      break;
    case 0x482730:
      script = 0x4e8fe8;
      break;
    case 0x4826a0:
      script = 0x4e8f80;
      finish = retail_getup_back;
      break;
    default:
      require(false, "Unexpected shared tumble native");
      return;
  }
  retail_vm_enter(fighter, script, finish);
}

static void stagger_tail_finish(Fighter *fighter) {
  /*483fa0 resets the controller context without rebinding its descriptor. */
  fighter_neutral(fighter);
  native_finish_clip(fighter, mk4_mmove_thrown_finish);
}

/*483df0/483ed0 run20 then ten4-sample blocks through429490. Its
 * 4297a0 cursor saturates at the inherited descriptor end, never a fixed
 * fighter clip/count. The CPU controller owns the intervening input hook. */
void retail_stagger_tail(Fighter *fighter) {
  int sample = fighter_sample(fighter);
  int last = fighter_move(fighter)->last;
  require(last != MOVE_END && sample >= 0 && sample <= last, "Stagger tail needs a bounded inherited descriptor");
  native_play_rows(fighter, mini(last, sample + 60), 1, stagger_tail_finish);
}

FighterFn retail_reaction_entry(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case 0x483450:
    case 0x483490:
    case 0x482750:
    case 0x482730:
    case 0x4826a0:
      return retail_tumble_reaction;
    case 0x481320:
    case 0x482560:
    case 0x4825c0:
    case 0x4835e0:
      return retail_launch_reaction;
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
