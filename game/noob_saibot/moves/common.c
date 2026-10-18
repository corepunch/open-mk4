#include "../character.h"

static const MoveDef s_noob_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_noob_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

/* Native controller return used by generated VM programs and native runners. */
void noob_saibot_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void noob_saibot_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(noob_saibot_ready);
}

void noob_saibot_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    noob_saibot_entry_retail_script_004de6b8(fighter);
  } else {
    noob_saibot_entry_retail_script_004de640(fighter);
  }
}

void noob_saibot_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  noob_saibot_entry_retail_script_004de664(victim);
  noob_saibot_entry_retail_script_004de668(fighter);
}

void noob_saibot_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, noob_saibot_ready);
}

void noob_saibot_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(noob_saibot_ready);
}

void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }

  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(noob_saibot_ready);
    return;
  }
  require(false, "Unexpected Noob Saibot opponent reaction");
}

void noob_saibot_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

void noob_saibot_shadow_low(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_low, noob_saibot_entry_retail_script_004ed268);
}

void noob_saibot_shadow_up(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_up, noob_saibot_entry_retail_script_004ed2c0);
}

bool noob_saibot_shadow_up_enabled(const Fighter *fighter) {
  /* 45ef20 returns true exactly when current-group +0x4c (gravity) is nonzero. */
  return fighter->body.gravity != 0;
}

void noob_saibot_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

void noob_saibot_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

void noob_saibot_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_fiber_finish(noob_saibot_ready);
}

void noob_saibot_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 107, "Unexpected noob_saibot LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  if (reaction == 102) {
    noob_saibot_entry_retail_script_004e9bd0(victim);
  } else {
    noob_saibot_entry_retail_script_004e9b40(victim);
  }
}
