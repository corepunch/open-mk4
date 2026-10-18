#include "../character.h"

static bool jax_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
static void jax_script_fatality_1_tail(Fighter *fighter);
static void jax_script_fatality_1_actor_tail(Fighter *fighter);
static void jax_script_fatality_1_final_tail(Fighter *fighter);
static void jax_script_fatality_1_actor_variant_a(Fighter *fighter);
static void jax_script_fatality_1_actor_variant_b(Fighter *fighter);
static void jax_script_fatality_1_scene_task_a(Fighter *fighter);
static void jax_script_fatality_1_scene_task_b(Fighter *fighter);
static void jax_script_fatality_1_victim(Fighter *fighter);
static void jax_script_fatality_1_victim_alt(Fighter *fighter);
static void jax_script_fatality_2_victim(Fighter *fighter);
static void jax_fatality_1(Fighter *fighter);
static void jax_fatality_2(Fighter *fighter);
static const MoveDef s_fatality_1_opening = {.clip = 384, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_1_victim = {.clip = 204, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_1_victim_alt = {.clip = 400, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_2_opening = {.clip = 284, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_2_victim = {.clip = 556, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};

static bool jax_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == JAX_EVENT_FATALITY_1_SIGNAL) {
    event->phase = JAX_EVENT_FATALITY_1_FINISH;
    event->data[0] = owner->sample_override;
    return true;
  }
  if (event->phase == JAX_EVENT_FATALITY_2_TASK) {
    event->phase = JAX_EVENT_FATALITY_2_FINISH;
    event->data[0] = event->data[1];
    event->data[1] = owner->sample_override;
    battle_event_program(event, event->data[0] == 0x26b ? 15 : 1);
    return true;
  }
  if (event->phase == JAX_EVENT_FATALITY_1_FINISH) {
    if (opponent->sample_override < 128) {
      return true;
    }
    if (opponent->health > 0) {
      battle_damage(opponent, opponent->health);
    }
    battle_end_finisher();
    return false;
  }
  if (event->phase == JAX_EVENT_FATALITY_2_FINISH) {
    const MoveDef *move = owner->current_move;
    if (move == NULL || (move->clip != 556 && move->clip != 114)) {
      return true;
    }
    int last_sample = move->clip == 556 ? 33 : 99;
    if (owner->sample_override < last_sample) {
      return true;
    }
    if (opponent->health > 0) {
      battle_damage(opponent, opponent->health);
    }
    battle_end_finisher();
    return false;
  }
  return false;
}

void jax_script_fatality_1_scene_signal(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_1_SIGNAL;
}

void jax_script_fatality_1_finish(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_1_FINISH;
}

void jax_script_fatality_2_task(Fighter *fighter, int task_type) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_2_TASK;
  event->data[1] = task_type;
}

void jax_script_fatality_2_scene_finish(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_2_FINISH;
  event->data[0] = 1;
}

static void jax_script_fatality_1_tail(Fighter *fighter) {
  jax_entry_retail_script_004e13a0(fighter);
}

static void jax_script_fatality_1_actor_tail(Fighter *fighter) {
  jax_entry_retail_script_004e13d0(fighter);
}

static void jax_script_fatality_1_final_tail(Fighter *fighter) {
  jax_entry_retail_script_004e13e0(fighter);
}

static void jax_script_fatality_1_actor_variant_a(Fighter *fighter) {
  jax_entry_retail_script_004e13b0(fighter);
}

static void jax_script_fatality_1_actor_variant_b(Fighter *fighter) {
  jax_entry_retail_script_004e13c0(fighter);
}

static void jax_script_fatality_1_scene_task_a(Fighter *fighter) {
  jax_entry_retail_script_004e18f0(fighter);
}

static void jax_script_fatality_1_scene_task_b(Fighter *fighter) {
  jax_entry_retail_script_004e1958(fighter);
}

static void jax_script_fatality_1_victim(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_fatality_1_victim);
  fighter_start_fiber(fighter, jax_entry_retail_script_004e13b0, NULL);
}

static void jax_script_fatality_1_victim_alt(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_fatality_1_victim_alt);
  fighter_start_fiber(fighter, jax_entry_retail_script_004e13c0, NULL);
}

void jax_script_fatality_2_tail(Fighter *fighter) {
  jax_entry_retail_script_004e5a98(fighter);
}

static void jax_script_fatality_2_victim(Fighter *fighter) {
  fighter_start_fiber(fighter, jax_entry_retail_script_004e5b38, NULL);
}

static void jax_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_1)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_VICTORY, &s_fatality_1_opening);
  jax_script_fatality_1_victim(victim);
  fighter_start_fiber(fighter, jax_entry_retail_script_004e1390, NULL);
}

static void jax_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_2)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_VICTORY, &s_fatality_2_opening);
  fighter_start_move(victim, S_KNOCKDOWN, &s_fatality_2_victim);
  fighter_start_fiber(victim, jax_entry_retail_script_004e5b38, NULL);
  fighter_start_fiber(fighter, jax_entry_retail_script_004e5a80, NULL);
}
