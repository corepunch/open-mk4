#include "../character.h"

void sonya_script_reaction(Fighter *fighter, int reaction) {
  if (reaction == 184) {
    Fighter *victim = fighter_peer(fighter);
    mk4_script_scene_pose(victim, 561, 0);
    fighter_cancel_mmove(victim);
    sonya_entry_retail_script_004e59c8(victim);
    return;
  }
  if (reaction == 175) {
    return;
  }
  require(reaction == 78 || reaction == 79 || reaction == 103, "Unexpected Sonya paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_cancel_mmove(victim);
  FighterFn script = reaction == 78 ? sonya_lk_victim : reaction == 79 ? sonya_entry_retail_script_004e9a68 : sonya_entry_retail_script_004e9b88;
  script(victim);
}

void sonya_special_cleanup(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter->body.physics = NULL;
  fighter_set_speed(fighter, 0);
  if (fighter_airborne(fighter)) {
    fighter->body.gravity = 0x28f;
  }
}

void sonya_generated_reaction(Fighter *victim) {
  FighterFn script;
  switch (victim->reaction) {
    case 24:
      script = sonya_entry_retail_script_004e9110;
      break;
    case 55:
      script = sonya_entry_retail_script_004e8b38;
      break;
    case 56:
      script = sonya_entry_retail_script_004e8f50;
      break;
    case 68:
      script = sonya_entry_retail_script_004e9de8;
      break;
    case 86:
      script = sonya_entry_retail_script_004ea130;
      break;
    case 87:
      script = sonya_square_victim;
      break;
    case 121:
      script = sonya_entry_retail_script_004ea0b8;
      battle_wall_recoil(victim);
      break;
    default:
      require(false, "Unexpected Sonya record reaction");
      return;
  }
  mk4_script_disarm_self(victim);
  fighter_start_move(victim, S_KNOCKDOWN, roster_def(victim->roster)->moves[S_KNOCKDOWN]);
  victim->flags |= FF_REACTING;
  victim->sample_override = 0;
  if (fighter_airborne(victim) && victim->body.gravity == 0) {
    victim->body.gravity = 0x28f;
  }
  script(victim);
}
