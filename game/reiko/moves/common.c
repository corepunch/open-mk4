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
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(reiko_ready);
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
  fighter_start_fiber(victim, reiko_entry_retail_script_004de664, NULL);
  reiko_entry_retail_script_004de668(fighter);
}

void reiko_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(reiko_ready);
}

void reiko_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reiko_ready);
}

void reiko_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 102 || reaction == 107) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
    fighter_start_fiber(victim, reaction == 102 ? reiko_entry_retail_script_004e9bd0 : reiko_entry_retail_script_004e9b40, NULL);
    fighter_fiber_finish(reiko_ready);
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(reiko_ready);
    return;
  }
  require(false, "Unexpected Reiko opponent reaction");
}

void reiko_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}
