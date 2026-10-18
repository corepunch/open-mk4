#include "../character.h"

static void jax_script_input_47ab10(Fighter *fighter);
static void jax_script_input_47ab50(Fighter *fighter);
void jax_script_input_46a990(Fighter *fighter);
void jax_script_input_46a9f0(Fighter *fighter);

void jax_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

bool jax_contact_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (event->phase == JAX_EVENT_JAB_CONTACT) {
    ContactResult result = battle_contact(owner, &jax_s_jax_jab_record);
    native_hold(owner, result == CONTACT_MISS ? 8 : 17, jax_ready);
  } else {
    native_hold(owner, 1, jax_ready);
  }
  return false;
}

void jax_script_opponent_reaction(Fighter *fighter, int reaction) {
  battle_react((Fighter *)fighter_opponent(fighter), reaction);
}

void jax_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 183) {
    fighter_start_fiber(fighter, jax_entry_retail_script_004e5b10, NULL);
    return;
  }
  battle_react(fighter, reaction);
}

void jax_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

void jax_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &jax_s_idle);
}

void jax_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &jax_s_walk);
}

void jax_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &jax_s_block);
}

void jax_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(jax_ready);
}

static void jax_script_input_47ab10(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &jax_s_jab, jax_entry_retail_script_004e8150);
}

static void jax_script_input_47ab50(Fighter *fighter) {
  jax_entry_retail_script_004e8174(fighter);
}

void jax_script_input_46a990(Fighter *fighter) {
  static const MoveDef owner = {.clip = 84, .last = 71, .flags = SF_ATTACK | SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0502};
  static const MoveDef victim = {.clip = 144, .last = 43, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
  mk4_script_throw_start(fighter, &owner, &victim, jax_entry_retail_script_004e5dd8);
}

void jax_script_input_46a9f0(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = victim->body.gravity = 0x3d7;
  jax_entry_retail_script_004e5df8(fighter);
}

void jax_script_lk_return(Fighter *fighter) {
  jax_entry_retail_script_004de358(fighter);
}

void jax_script_lk_finish(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(jax_ready);
}
