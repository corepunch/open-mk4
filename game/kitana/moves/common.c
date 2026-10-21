#include "retail_vm.h"
#include "../character.h"

static const MoveDef s_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};

void kitana_shadow_low(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_low);
  retail_vm_enter_owned(fighter, 0x4ed268, fighter_ready, 16);
}

void kitana_shadow_up(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_shadow_up);
  retail_vm_enter_owned(fighter, 0x4ed2c0, fighter_ready, 16);
}

void kitana_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 184) {
    Fighter *victim = fighter_peer(fighter);
    mk4_script_scene_pose(victim, 561, 0);
    fighter_cancel_mmove(victim);
    retail_vm_enter_owned(victim, 0x4e59c8, fighter_ready, 16);
    return;
  }
  if (reaction == 175) {
    return;
  }
  require(reaction == 78 || reaction == 79 || reaction == 103, "Unexpected Kitana paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_cancel_mmove(victim);
  if (reaction == 78) {
    kitana_lk_victim(victim);
  } else {
    RetailScriptContinuation script =
        reaction == 79 ? (RetailScriptContinuation){0x4e9a68, fighter_ready, 16} : (RetailScriptContinuation){0x4e9b88, mk4_script_thrown_ready, 16};
    retail_vm_enter_owned(victim, script.entry, script.finish, script.native_owner);
  }
}

void kitana_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

void kitana_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Kitana shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

void kitana_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

void kitana_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

void kitana_generated_reaction(Fighter *victim) {
  RetailScriptContinuation script = {0};
  FighterFn native = NULL;
  switch (victim->reaction) {
    case 24:
      script = (RetailScriptContinuation){0x4e9110, mk4_script_thrown_ready, 16};
      break;
    case 55:
      script = (RetailScriptContinuation){0x4e8b38, mk4_script_thrown_ready, 16};
      break;
    case 56:
      script = (RetailScriptContinuation){0x4e8f50, mk4_script_thrown_ready, 16};
      break;
    case 68:
      script = (RetailScriptContinuation){0x4e9de8, mk4_script_thrown_ready, 16};
      break;
    case 86:
      script = (RetailScriptContinuation){0x4ea130, mk4_script_thrown_ready, 16};
      break;
    case 87:
      native = kitana_square_victim;
      break;
    case 121:
      script = (RetailScriptContinuation){0x4ea0b8, mk4_script_thrown_ready, 16};
      battle_wall_recoil(victim);
      break;
    default:
      require(false, "Unexpected Kitana record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, roster_def(victim->roster)->moves[S_KNOCKDOWN]);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  if (native != NULL) {
    native(victim);
  } else {
    retail_vm_enter_owned(victim, script.entry, script.finish, script.native_owner);
  }
}
