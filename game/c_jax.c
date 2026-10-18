#include "game.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 265, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_low_kick = {.clip = 70, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};
static const MoveDef s_sweep = {.clip = 62, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};
static const MoveDef s_roundhouse = {.clip = 64, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010D};
static const AttackRecord s_roundhouse_record = {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14};
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};
static const MoveDef s_weapon_draw = {.clip = 244, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_1_opening = {.clip = 384, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_1_victim = {.clip = 204, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_1_victim_alt = {.clip = 400, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_fatality_2_opening = {.clip = 284, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_fatality_2_victim = {.clip = 556, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_throw = {.clip = 83, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_thrown = {.clip = 145, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_lk_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_lk_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 412, .last = MOVE_END, .flags = SF_HOLD};
static const WeaponDef s_jax_weapon;
static void jax_weapon_action(Fighter *fighter);
static void jax_script_current_reaction(Fighter *fighter, int reaction);
static void jax_script_opponent_reaction(Fighter *fighter, int reaction);
static void jax_script_roundhouse_tail(Fighter *fighter);
static void jax_script_fatality_1_scene_signal(Fighter *fighter);
static void jax_script_fatality_1_finish(Fighter *fighter);
static void jax_script_fatality_2_task(Fighter *fighter, int task_type);
static void jax_script_fatality_2_scene_finish(Fighter *fighter);
static void jax_script_air_throw_ground_release(Fighter *fighter);
static void jax_script_jab_contact(Fighter *fighter);
static void jax_script_jab_followup(Fighter *fighter);
static void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type);

static void jax_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

#include "generated/jax.inc"

enum {
  JAX_EVENT_FATALITY_1_SIGNAL = 1,
  JAX_EVENT_FATALITY_1_FINISH,
  JAX_EVENT_FATALITY_2_TASK,
  JAX_EVENT_FATALITY_2_FINISH,
  JAX_EVENT_JAB_CONTACT,
  JAX_EVENT_JAB_RECOVERY
};

// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord s_jax_jab_record = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 10, 10, 40, 0, 0x170a};
// clang-format on

static bool jax_scene_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == JAX_EVENT_FATALITY_1_SIGNAL) {
    event->phase = JAX_EVENT_FATALITY_1_FINISH;
    event->data[0] = owner->sample_override;
    battle_event_delay(event, 1);
    return true;
  }
  if (event->phase == JAX_EVENT_FATALITY_2_TASK) {
    event->phase = JAX_EVENT_FATALITY_2_FINISH;
    event->data[0] = event->data[1];
    event->data[1] = owner->sample_override;
    battle_event_delay(event, event->data[0] == 0x26b ? 15 : 1);
    return true;
  }
  if (event->phase == JAX_EVENT_FATALITY_1_FINISH) {
    if (opponent->sample_override < 128) {
      battle_event_delay(event, 1);
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
      battle_event_delay(event, 1);
      return true;
    }
    int last_sample = move->clip == 556 ? 33 : 99;
    if (owner->sample_override < last_sample) {
      battle_event_delay(event, 1);
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

static bool jax_contact_event(BattleEvent *event, Fighter *owner, Fighter *opponent) {
  (void)opponent;
  if (event->phase == JAX_EVENT_JAB_CONTACT) {
    ContactResult result = battle_contact(owner, &s_jax_jab_record);
    event->data[0] = result;
    event->phase = JAX_EVENT_JAB_RECOVERY;
    event->data[1] = result == CONTACT_MISS ? 8 : 17;
    battle_event_delay(event, 1);
    return true;
  }
  if (--event->data[1] <= 0) {
    fighter_enter_state(owner, S_IDLE);
    return false;
  }
  battle_event_delay(event, 1);
  return true;
}

static void jax_script_fatality_1_scene_signal(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_1_SIGNAL;
}

static void jax_script_fatality_1_finish(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_1_FINISH;
}

static void jax_script_fatality_2_task(Fighter *fighter, int task_type) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_2_TASK;
  event->data[1] = task_type;
}

static void jax_script_fatality_2_scene_finish(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_scene_event, false, false);
  event->phase = JAX_EVENT_FATALITY_2_FINISH;
  event->data[0] = 1;
}

static void jax_script_air_throw_ground_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_react(victim, 78);
  battle_damage(victim, 0x2147);
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(jax_ready);
}

static void jax_script_jab_contact(Fighter *fighter) {
  BattleEvent *event = battle_event_start(fighter, jax_contact_event, false, true);
  event->phase = JAX_EVENT_JAB_CONTACT;
}

static void jax_script_jab_followup(Fighter *fighter) {
  (void)battle_event_start(fighter, jax_contact_event, false, true);
}

static void jax_script_cpu_projectile_step(Projectile *projectile) {
  Fighter *owner = &g_battle.fighters[projectile->owner];
  if (projectile->ticks >= 36 || g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  projectile->pos[0] += projectile->vel[0];
  projectile->pos[1] += projectile->vel[1];
  if (battle_projectile_overlaps(projectile, &s_jax_jab_record)) {
    projectile->kind = 0;
    battle_damage((Fighter *)fighter_opponent(owner), 0x170a);
  }
}

static void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type) {
  (void)task_type;
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 36, jax_script_cpu_projectile_step);
  if (projectile != NULL) {
    projectile->vel[0] = (q16)(fighter->facing * 0x1800);
    projectile->vel[1] = 0;
  }
}

static void jax_script_opponent_reaction(Fighter *fighter, int reaction) {
  battle_react((Fighter *)fighter_opponent(fighter), reaction);
}

static void jax_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 183) {
    fighter_start_fiber(fighter, retail_script_004e5b10, NULL);
    return;
  }
  battle_react(fighter, reaction);
}

static void jax_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void jax_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void jax_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void jax_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &s_crouch);
}

static void jax_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void jax_jab(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void jax_high_punch(Fighter *fighter) {
  jax_jab(fighter);
}

static void jax_high_kick(Fighter *fighter) {
  jax_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void jax_low_kick(Fighter *fighter) {
  jax_start(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void jax_sweep(Fighter *fighter) {
  jax_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void jax_uppercut(Fighter *fighter) {
  jax_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void jax_roundhouse(Fighter *fighter) {
  jax_start(fighter, S_KICK, &s_roundhouse, retail_script_004e6800);
}

static void jax_script_roundhouse_start(Fighter *fighter) {
  fighter->script_timer = fighter_fast_start(fighter);
  if (fighter->script_timer) {
    retail_script_004e6890(fighter);
  } else {
    retail_script_004e68a8(fighter);
  }
}

static void jax_script_roundhouse_tail(Fighter *fighter) {
  const int fast = fighter->script_timer != 0 ? 4 : 0;
  const int active = 14 - fast;
  for (;;) {
    int tick = fighter->ticks;
    if (!fighter->connected && tick >= active && tick <= active + 2) {
      battle_contact(fighter, &s_roundhouse_record);
    }
    if (tick >= active) {
      fighter->action = ACTION_RETAIL_1006;
    }
    int end = (fighter->connected ? 67 : 39) - fast;
    if (fighter->contact == CONTACT_BLOCK) {
      end = 39 - fast;
    }
    if (tick <= 6) {
      fighter->sample_override = tick * 2;
    } else if (tick < active) {
      fighter->sample_override = mini(19, 12 + (tick - 6) * (fast != 0 ? 2 : 1));
    } else {
      fighter->sample_override = mini(44, 20 + tick - active);
    }
    if (tick >= end) {
      fighter_fiber_finish(jax_ready);
    }
    fighter_wait_ticks(1);
  }
}

static void jax_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(jax_ready);
}

static void jax_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void jax_air_hp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void jax_air_lp(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void jax_air_kick(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void jax_air_diagonal_hk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void jax_air_diagonal_lk(Fighter *fighter) {
  jax_start(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void jax_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004de798);
}

static void jax_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void jax_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void jax_script_air_throw_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void jax_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void jax_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void jax_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void jax_script_input_47ab10(Fighter *fighter) {
  jax_start(fighter, S_PUNCH, &s_jab, retail_script_004e8150);
}

static void jax_script_input_47ab50(Fighter *fighter) {
  retail_script_004e8174(fighter);
}

static void jax_script_cpu_496f80(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004ed090, NULL);
}

static void jax_script_input_46a990(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, retail_script_004e5dd8);
}

static void jax_script_input_46a9f0(Fighter *fighter) {
  retail_script_004e5df8(fighter);
}

static void jax_script_weapon_draw_finish(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

static void jax_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, jax_fan_start, NULL);
}

static void jax_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    jax_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void jax_script_fatality_1_tail(Fighter *fighter) {
  retail_script_004e13a0(fighter);
}

static void jax_script_fatality_1_actor_tail(Fighter *fighter) {
  retail_script_004e13d0(fighter);
}

static void jax_script_fatality_1_final_tail(Fighter *fighter) {
  retail_script_004e13e0(fighter);
}

static void jax_script_fatality_1_actor_variant_a(Fighter *fighter) {
  retail_script_004e13b0(fighter);
}

static void jax_script_fatality_1_actor_variant_b(Fighter *fighter) {
  retail_script_004e13c0(fighter);
}

static void jax_script_fatality_1_scene_task_a(Fighter *fighter) {
  retail_script_004e18f0(fighter);
}

static void jax_script_fatality_1_scene_task_b(Fighter *fighter) {
  retail_script_004e1958(fighter);
}

static void jax_script_lk_return(Fighter *fighter) {
  retail_script_004de358(fighter);
}

static void jax_script_lk_finish(Fighter *fighter) {
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(jax_ready);
}

static void jax_script_fatality_1_victim(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_fatality_1_victim);
  fighter_start_fiber(fighter, retail_script_004e13b0, NULL);
}

static void jax_script_fatality_1_victim_alt(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_fatality_1_victim_alt);
  fighter_start_fiber(fighter, retail_script_004e13c0, NULL);
}

static void jax_script_fatality_2_tail(Fighter *fighter) {
  retail_script_004e5a98(fighter);
}

static void jax_script_fatality_2_victim(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e5b38, NULL);
}

static void jax_fatality_1(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_1)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_VICTORY, &s_fatality_1_opening);
  jax_script_fatality_1_victim(victim);
  fighter_start_fiber(fighter, retail_script_004e1390, NULL);
}

static void jax_fatality_2(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_JAX_2)) {
    jax_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_VICTORY, &s_fatality_2_opening);
  fighter_start_move(victim, S_KNOCKDOWN, &s_fatality_2_victim);
  fighter_start_fiber(victim, retail_script_004e5b38, NULL);
  fighter_start_fiber(fighter, retail_script_004e5a80, NULL);
}

static void jax_cpu_medium(Fighter *fighter) {
  fighter->action = 0x101;
  fighter->script_phase = 0;
  battle_cpu_step(fighter);
}

static void jax_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jax_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jax_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jax_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jax_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &s_jax_weapon);
}

static const WeaponDef s_jax_weapon = {
    0x69, jax_weapon_hp, jax_weapon_lp, jax_weapon_back_hp, jax_weapon_back_lp, NULL, weapon_throw, jax_weapon_equip, SCENE_MODEL_SWORD};

static void jax_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
    return;
  }
  if (fighter->flags & FF_FOLLOW_UP) {
    jax_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    jax_weapon_back_hp(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_weapon_draw);
  weapon_draw(fighter, &s_jax_weapon);
  fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
}

static void jax_press_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_weapon_action);
}

static void jax_press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? fighter->weapon->lp : (fighter_distance(fighter) <= 0.7f ? jax_lp_throw : jax_jab);
  fighter_interrupt(fighter, entry);
}

static void jax_press_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_high_kick);
}

static void jax_press_lk(Fighter *fighter) {
  FighterFn entry = fighter_holds_back(fighter) ? jax_sweep : jax_low_kick;
  fighter_interrupt(fighter, entry);
}

static void jax_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jax_uppercut);
}

static void jax_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jax_sweep);
}

static void jax_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_hp);
}

static void jax_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_lp);
}

static void jax_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_kick);
}

static void jax_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_throw);
}

static const ButtonTable s_standing = {
    {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const ButtonTable s_crouching = {{[BUTTON_HIGH_PUNCH] = jax_press_uppercut,
    [BUTTON_LOW_PUNCH] = jax_press_uppercut,
    [BUTTON_HIGH_KICK] = jax_press_sweep,
    [BUTTON_LOW_KICK] = jax_press_sweep}};
static const ButtonTable s_jumping = {{[BUTTON_HIGH_PUNCH] = jax_press_air_hp,
    [BUTTON_LOW_PUNCH] = jax_press_air_lp,
    [BUTTON_HIGH_KICK] = jax_press_air_kick,
    [BUTTON_LOW_KICK] = jax_press_air_kick}};
static const Controls s_controls = {NULL, 0, {&s_standing, &s_crouching, &s_jumping}};

static void jax_command_weapon(Fighter *fighter) {
  jax_weapon_action(fighter);
}

static void jax_command_spike(Fighter *fighter) {
  jax_spike_fatality(fighter);
}

static void jax_command_fan(Fighter *fighter) {
  jax_fan_fatality(fighter);
}

static void jax_command_run(Fighter *fighter) {
  mk4_walk(fighter);
}

static void jax_command_sidestep(Fighter *fighter) {
  mk4_sidestep(fighter);
}

static void jax_command_medium(Fighter *fighter) {
  jax_cpu_medium(fighter);
}

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = jax_idle,
    [S_WALK] = jax_walk,
    [S_CROUCH] = jax_crouch,
    [S_JUMP] = mk4_jump,
    [S_PUNCH] = jax_jab,
    [S_KICK] = jax_high_kick,
    [S_SWEEP] = jax_sweep,
    [S_UPPERCUT] = jax_uppercut,
    [S_THROW] = jax_lp_throw,
    [S_BLOCK] = jax_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = NULL,
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_high_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_jax = {GAME_MK4, 13, "JAX", &mk4_motion, moves, S_COUNT, &s_controls, enter, -75169, mk4_walk, mk4_jump, NULL};
