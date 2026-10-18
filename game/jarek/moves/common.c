#include "../character.h"

static void jarek_start_contact_recovery(BattleEvent *event, Fighter *fighter, int frames);
static bool jarek_contact_event(BattleEvent *event, Fighter *fighter, Fighter *victim);
static void jarek_contact_dispatch(Fighter *fighter, int stage);
static const AttackRecord jarek_selector_24_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 160, 160, 8, 0, 0x2666};
static const AttackRecord jarek_selector_26_record = {0xc7ae, -0x16624, 0xe872, 0x13020, 163, 163, 3, 0, 0x1eb8};

void jarek_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

void jarek_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

void jarek_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &jarek_s_idle);
}

void jarek_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &jarek_s_walk);
}

void jarek_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &jarek_s_block);
}

void jarek_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void jarek_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    jarek_entry_retail_script_004de6b8(fighter);
  } else {
    jarek_entry_retail_script_004de640(fighter);
  }
}

void jarek_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  jarek_entry_retail_script_004de664(victim);
  jarek_entry_retail_script_004de668(fighter);
}

void jarek_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, jarek_ready);
}

void jarek_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    jarek_fatality_contact_hold(fighter);
    return;
  }
  require(reaction == 102 || reaction == 107, "Unexpected Jarek LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void jarek_script_opponent_reaction(Fighter *fighter, int reaction) {
  mk4_mmove_lk_release(fighter, reaction);
}

static void jarek_start_contact_recovery(BattleEvent *event, Fighter *fighter, int frames) {
  (void)event;
  fighter->flags |= FF_BUSY;
  native_hold(fighter, frames, jarek_ready);
}

static bool jarek_contact_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  ContactResult result;
  (void)victim;

  if (event->data[0] == 0) {
    result = battle_contact(fighter, &jarek_selector_25_record);
    if (result == CONTACT_MISS) {
      event->data[0] = 1;
      fighter_start_move(fighter, S_PUNCH, &jarek_s_jab);
      fighter_start_fiber(fighter, jarek_entry_retail_script_004e80c8, NULL);
      return false;
    }
    jarek_start_contact_recovery(event, fighter, jarek_s_low_kick.recovery);
    return false;
  }

  if (event->data[0] == 1) {
    result = battle_contact(fighter, &jarek_selector_26_record);
    if (result != CONTACT_MISS) {
      event->data[0] = 2;
      fighter_start_move(fighter, S_KICK, &jarek_s_high_kick);
      fighter_start_fiber(fighter, jarek_entry_retail_script_004e80f4, NULL);
      return false;
    }
    jarek_start_contact_recovery(event, fighter, 6);
    return false;
  }

  result = battle_contact(fighter, &jarek_selector_24_record);
  jarek_start_contact_recovery(event, fighter, result == CONTACT_MISS ? 5 : jarek_s_high_kick.recovery);
  return false;
}

static void jarek_contact_dispatch(Fighter *fighter, int stage) {
  BattleEvent *event = battle_event_start(fighter, jarek_contact_event, false, true);
  event->data[0] = stage;
}

void jarek_script_contact_25(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 0);
}

void jarek_script_contact_26(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 1);
}

void jarek_script_contact_24(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 2);
}

void jarek_input_target(Fighter *fighter, Button button) {
  fighter_press(fighter, button);
}

void jarek_script_47b600(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_damage(victim, 0x23d7);
  battle_react(victim, 136);
  fighter_fiber_finish(jarek_ready);
}
