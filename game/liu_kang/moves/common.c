#include "native_services.h"

/* Original script words own VM control flow; the remaining native adapters
 * implement engine boundaries. See docs/liu-kang-scripts.md for evidence. */

void liu_kang_liu_script_lk_continue(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de410, fighter_ready, 3);
}

void liu_kang_liu_script_lk_damage(Fighter *fighter) {
  Fighter *victim = fighter_peer(fighter);
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    victim->body.pos[axis] = fighter->body.pos[axis] + q16_mul(fighter_axis(fighter, axis), 0xb333);
  }
  victim->script_phase = 1;
  victim->flags |= FF_PAIR_RELEASE;
  fighter_cancel_mmove(victim);
  liu_kang_lk_thrown_script(victim);
  battle_damage(victim, 0x2e14);
  /* 4de428 returns to this native caller, which then enters 4de468. */
  retail_vm_enter_then_owned(fighter, 0x4de428, 0x4de468, fighter_ready, 3);
}

void liu_kang_liu_script_spawn_low(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang low-fireball task type");
  liu_kang_fireball_spawn(fighter, 1);
}

void liu_kang_liu_script_spawn_high(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang high-fireball task type");
  liu_kang_fireball_spawn(fighter, 0);
}

void liu_kang_liu_script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = fighter_peer(fighter);
  victim->reaction = reaction;
  if (reaction == 104) {
    battle_react(mk4_script_release_pair(fighter), reaction);
  } else if (reaction == 80) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
  } else if (reaction == 176) {
    victim->facing = -victim->facing;
  }
}
