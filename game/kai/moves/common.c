#include "../character.h"

void kai_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void kai_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void kai_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void kai_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

void kai_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 enters the follow-up VM root after its global task checks. */
  kai_entry_retail_script_004de358(fighter);
}

void kai_script_lk_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
}

void kai_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    kai_entry_retail_script_004de6b8(fighter);
  } else {
    kai_entry_retail_script_004de610(fighter);
  }
}

void kai_script_current_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 109, "Unexpected Kai LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void kai_script_lp_extended_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
}

void kai_script_lp_short_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, kai_ready);
}

void kai_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Kai paired reaction");
}

void kai_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}
