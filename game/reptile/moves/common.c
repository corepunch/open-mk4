#include "../character.h"

static bool reptile_visibility_event(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static BattleEvent *reptile_find_event(Fighter *fighter, BattleEventFn step, int kind);
static bool reptile_task_76_event(BattleEvent *event, Fighter *fighter, Fighter *opponent);
static void reptile_ready(Fighter *fighter);
static void reptile_script_47ab50(Fighter *fighter);
static void reptile_script_47b260(Fighter *fighter);
static void reptile_loop_frame(Fighter *fighter);
void reptile_input_47a690(Fighter *fighter);
void reptile_input_47afe0(Fighter *fighter);
static void reptile_input_4988d0(Fighter *fighter);
static void reptile_entry_block(Fighter *fighter);
static const MoveDef s_dash = {.clip = 302, .last = 7, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x216};
static const MoveDef s_roll = {.clip = 106, .last = 21, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x20d};
static const MoveDef s_reaction_40 = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const AttackRecord reptile_loop_contact = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 40, 10, 9, 0, 0x170a};

static bool reptile_visibility_event(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  (void)opponent;
  fighter->appearance = event->data[1];
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  return false;
}

static BattleEvent *reptile_find_event(Fighter *fighter, BattleEventFn step, int kind) {
  int owner = fighter == &g_battle.fighters[0] ? 0 : 1;
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (event->active && event->owner == owner && event->step == step && event->data[7] == kind) {
      return event;
    }
  }
  return NULL;
}

void reptile_script_visibility_start(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
    return;
  }
  event = battle_event_start(fighter, reptile_visibility_event, false, true);
  event->data[1] = fighter->appearance;
  battle_event_program(event, 104);
  event->data[7] = REPTILE_EVENT_VISIBILITY;
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter->flags |= FF_TELEPORTING;
}

void reptile_script_visibility_extend(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
    battle_event_program(event, 104);
  }
}

void reptile_script_visibility_end(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    fighter->appearance = event->data[1];
    fighter->flags &= ~(unsigned)FF_TELEPORTING;
    battle_event_cancel(event);
  }
}

void reptile_script_visibility_phase(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
  }
}

static bool reptile_task_76_event(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  (void)fighter;
  (void)opponent;
  g_battle.projectiles[event->data[1]].kind = 0;
  return false;
}

void reptile_task_76_spawn(Fighter *fighter, int task_type) {
  require(task_type == 76, "Unexpected Reptile cooperative task");
  Projectile *clone = battle_spawn_projectile(fighter, PROJECTILE_CLONE, 20, reptile_clone_step);
  if (clone == NULL) {
    return;
  }
  clone->clip = 26;
  clone->sample = 0;
  memcpy(clone->pos, fighter->body.pos, sizeof clone->pos);
  BattleEvent *event = battle_event_start(fighter, reptile_task_76_event, false, true);
  event->data[1] = (int)(clone - g_battle.projectiles);
  battle_event_program(event, 14);
  for (int frame = 1; frame <= 14; ++frame) {
    battle_event_frame(event, frame - 1, frame < 13 ? frame : 13, frame >= 11 && frame <= 13 ? reptile_clone_contact : reptile_clone_pose);
  }
  battle_sound(SOUND_C, 0x21);
}

static void reptile_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

void reptile_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void reptile_script_47a6d0(Fighter *fighter) {
  native_queries(fighter, &reptile_script_contact, 16, 1);
  if (!fighter->connected) {
    reptile_entry_retail_script_004e8148(fighter);
    return;
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_47ab50(Fighter *fighter) {
  reptile_entry_retail_script_004e8174(fighter);
}

static void reptile_script_47b260(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &reptile_loop_contact);
  }
}

static void reptile_loop_frame(Fighter *fighter) {
  reptile_script_47b260(fighter);
  if (fighter->connected) {
    fighter_stop_mmove(fighter);
  }
}

void reptile_script_47b010(Fighter *fighter) {
  reptile_script_47b260(fighter);
  if (!fighter->connected) {
    native_begin(fighter, 1, 6, NULL);
    for (int frame = 1; frame <= 6; ++frame) {
      native_frame(fighter, frame, fighter_sample(fighter), frame < 6 ? reptile_loop_frame : NULL);
    }
    native_wait(fighter);
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reptile_ready);
}

void reptile_script_reaction_side_getup(Fighter *fighter) {
  if (fighter->health <= 0) {
    mk4_script_thrown_finish(fighter);
  }
  mk4_script_bind(fighter, 185, 16, 0);
  mk4_script_finish_clip(fighter, 1);
  mk4_script_thrown_finish(fighter);
}

void reptile_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, reptile_ready);
}

void reptile_script_current_reaction(Fighter *fighter, int reaction) {

  battle_react(fighter, reaction);
}

void reptile_reaction_40(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_reaction_40);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, reptile_entry_retail_script_004e9620, NULL);
}

void reptile_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Reptile opponent reaction");
}

void reptile_script_finish(Fighter *fighter) {
  native_standard_sweep(fighter);
}

void reptile_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  if (state == S_SWEEP) {
    script(fighter);
  } else {
    fighter_start_fiber(fighter, script, NULL);
  }
}

bool reptile_can_dash(const Fighter *fighter) {
  /* 45e950: action family 11 must be at least 70 ticks old and the peer
   * combo count (+7c, queried by 45f5b0) must not exceed four. */
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->slide_ready && fighter_opponent(fighter)->hits <= 4;
}

void reptile_input_47a690(Fighter *fighter) {
  fighter->slide_ready = g_battle.ticks + 70;
  mk4_script_stop_body(fighter);
  reptile_start(fighter, S_KICK, &s_dash, reptile_entry_retail_script_004e8128);
}

void reptile_input_47afe0(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  reptile_start(fighter, S_KICK, &s_roll, reptile_entry_retail_script_004e8178);
}

static void reptile_input_4988d0(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &reptile_s_jab, reptile_entry_retail_script_004ed1e8);
}

static void reptile_entry_block(Fighter *fighter) {
  fighter_enter_state(fighter, S_BLOCK);
}

void reptile_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102, "Unexpected reptile LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &reptile_s_lp_thrown);
  reptile_entry_retail_script_004e9bd0(victim);
}
