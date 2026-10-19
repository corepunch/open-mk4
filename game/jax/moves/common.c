#include "../character.h"

static void jax_script_input_47ab10(Fighter *fighter);
static void jax_script_input_47ab50(Fighter *fighter);

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
  mk4_mmove_lk_release(fighter, reaction);
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
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

void jax_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void jax_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void jax_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

void jax_script_finish(Fighter *fighter) {
  native_advancing_sweep(fighter);
}

static void jax_script_input_47ab10(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &jax_s_jab, jax_entry_retail_script_004e8150);
}

static void jax_script_input_47ab50(Fighter *fighter) {
  jax_entry_retail_script_004e8174(fighter);
}

void jax_script_lk_return(Fighter *fighter) {
  jax_entry_retail_script_004de358(fighter);
}

void jax_script_lk_finish(Fighter *fighter) {
  fighter_finish_mmove(fighter, jax_ready);
}
