#include "retail_vm.h"
#include "../character.h"

static const MoveDef s_lk_thrown_end = {.clip = 586, .first = 65, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

void scorpion_special_ready(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  fighter_ready(fighter);
}

void scorpion_script_lk_rebase(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  retail_vm_enter_owned(fighter, 0x4de318, fighter_ready, 0);
}

void scorpion_script_lk_damage(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de328, fighter_ready, 0);
}

void scorpion_script_lp_return(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void scorpion_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Scorpion pair release");
  Fighter *victim = fighter_peer(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown_end);
  victim->sample_override = 65;
  scorpion_thrown_end_script(victim);
}

void scorpion_spin_script(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, NULL);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, frame % 12, frame == 12 ? scorpion_sword_spin_query : scorpion_sword_spin_control);
  }
  fighter->native.poses[0] = 0;
  native_play(fighter, false);
  fighter->sample_override = pose;
}

void scorpion_script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = fighter_peer(fighter);
  if (reaction == 175) {
    return; /* Native arena continuation owns the impalement and completion. */
  }
  if (reaction == 71) {
    if (victim->action == ACTION_RETAIL_1008) {
      victim->reaction = 71;
    }
    return; /* Pull arrival already started the independently interruptible stun. */
  }
  require(reaction == 100, "Unexpected generated Scorpion reaction");
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  battle_react(victim, reaction);
}

void scorpion_generated_reaction(Fighter *victim) {
  (victim->reaction == 173 ? scorpion_entry_retail_script_004e97f8 : scorpion_entry_retail_script_004e9938)(victim);
}

void scorpion_native_blend(Fighter *fighter, const SceneClip *blend, FighterFn end) {
  native_scene_blends(fighter, blend, 1, end);
}

void scorpion_script_transform_effect(Fighter *fighter, int type) {
  require(type == 0, "Unexpected transform effect");
  mk4_script_effect(fighter, 18);
}
