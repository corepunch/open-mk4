#include "../character.h"

static void reiko_ready(Fighter *fighter);
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

/* Native controller return used by generated VM programs and native runners. */
static void reiko_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void reiko_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void reiko_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    reiko_entry_retail_script_004de6b8(fighter);
  } else {
    reiko_entry_retail_script_004de640(fighter);
  }
}

void reiko_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  reiko_entry_retail_script_004de664(victim);
  reiko_entry_retail_script_004de668(fighter);
}

void reiko_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, reiko_ready);
}

void reiko_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

void reiko_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }

  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Reiko opponent reaction");
}

void reiko_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 107, "Unexpected reiko LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  if (reaction == 102) {
    reiko_entry_retail_script_004e9bd0(victim);
  } else {
    reiko_entry_retail_script_004e9b40(victim);
  }
}
