#include "../character.h"

void fujin_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void fujin_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    fujin_entry_retail_script_004de6b8(fighter);
  } else {
    fujin_entry_retail_script_004de610(fighter);
  }
}

void fujin_script_current_reaction(Fighter *fighter, int reaction) {
  /* 480f80 -> 48f880/48fd00 installs this on the task's captured peer.
   * Replacing the owner here cancels the very stream issuing the reaction. */
  if (reaction == 97 || reaction == 181) {
    battle_react((Fighter *)fighter_opponent(fighter), reaction);
    return;
  }
  require(reaction == 102 || reaction == 109, "Unexpected Fujin LP victim release");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void fujin_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Fujin opponent reaction");
}

void fujin_script_lp_extended_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

void fujin_script_lp_short_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}
