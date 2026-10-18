#include "game.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 267, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};
static const MoveDef s_high_punch = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_high_kick = {.clip = 65, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};
static const MoveDef s_roundhouse = {.clip = 313, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010D};
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};
static const MoveDef s_weapon_draw = {.clip = 244, .last = MOVE_END, .flags = SF_LOCK};
static const MoveDef s_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};
static const WeaponDef jarek_weapon;
static const AttackRecord jarek_roundhouse_record;
static const AttackRecord jarek_selector_25_record;
static void jarek_weapon_action(Fighter *fighter);

static void jarek_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

#include "generated/jarek.inc"
#include "generated/frames_jarek.inc"

static void jarek_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void jarek_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void jarek_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void jarek_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &s_crouch);
}

static void jarek_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void jarek_jab(Fighter *fighter) {
  jarek_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void jarek_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
}

static void jarek_high_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void jarek_low_kick(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void jarek_sweep(Fighter *fighter) {
  jarek_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void jarek_uppercut(Fighter *fighter) {
  jarek_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void jarek_roundhouse(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_roundhouse, retail_script_004e6800);
}

static void jarek_air_hp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void jarek_air_lp(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void jarek_air_kick(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void jarek_air_diagonal_hk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void jarek_air_diagonal_lk(Fighter *fighter) {
  jarek_start(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void jarek_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, script);
}

static void jarek_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(jarek_ready);
}

static void jarek_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    retail_script_004de6b8(fighter);
  } else {
    retail_script_004de640(fighter);
  }
}

static void jarek_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004de664, NULL);
  retail_script_004de668(fighter);
}

static void jarek_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(jarek_ready);
}

static void jarek_lp_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, retail_script_004de798);
}

static void jarek_lk_throw(Fighter *fighter) {
  jarek_ground_throw(fighter, retail_script_004de2f8);
}

static void jarek_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void jarek_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void jarek_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void jarek_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void jarek_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void jarek_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    jarek_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, jarek_fan_start, NULL);
}

static void jarek_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    jarek_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void jarek_throw_reaction(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static void jarek_fatality_contact_hold(Fighter *fighter);

static void jarek_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    jarek_fatality_contact_hold(fighter);
    return;
  }
  battle_react(fighter, reaction);
}

static void jarek_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77 || reaction == 102 || reaction == 107, "Unexpected Jarek paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_fiber_finish(jarek_ready);
}

/* The Jarek fatality contact uses reaction 175 with zero damage. The stage
 * coordinator owns the victim pose and recovery for that scene. */
static void jarek_fatality_contact_hold(Fighter *fighter) {
  require(g_battle.phase == PHASE_FATALITY, "Jarek reaction 175 outside fatality");
  fighter->script = NULL;
  fighter->pending = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
}

// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord jarek_high_kick_record = {89718, -55705, 86441, 70123, 16, 13, 6, 0, 8519};
static const AttackRecord jarek_low_kick_record = {97648, -17039, 64225, 28835, 17, 10, 6, 0, 7208};
static const AttackRecord jarek_sweep_record = {95682, 31391, 68812, 35389, 18, 18, 4, ATTACK_LOW, 9175};
static const AttackRecord jarek_roundhouse_record = {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796};
static const AttackRecord jarek_uppercut_record = {58327, -64880, 38010, 66840, 34, 34, 6, 0, 10485};
static const AttackRecord jarek_uppercut_followup_record = {73990, -70057, 64159, 96927, 34, 34, 6, 0, 2621};
static const AttackRecord jarek_selector_24_record = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 160, 160, 8, 0, 0x2666};
static const AttackRecord jarek_selector_25_record = {0x17fbe, -0x51a9, 0x13020, 0x2b43, 162, 162, 165, 0, 0x23d7};
static const AttackRecord jarek_selector_26_record = {0xc7ae, -0x16624, 0xe872, 0x13020, 163, 163, 3, 0, 0x1eb8};
// clang-format on

enum {
  JAREK_CONTACT_RECOVERY = 1,
  JAREK_FAN_SCENE = 16,
  JAREK_FAN_FINISH
};

static void jarek_start_contact_recovery(BattleEvent *event, Fighter *fighter, int ticks) {
  event->phase = JAREK_CONTACT_RECOVERY;
  event->data[0] = ticks;
  fighter->flags |= FF_BUSY;
  battle_event_delay(event, 1);
}

static bool jarek_contact_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  ContactResult result;
  (void)victim;

  if (event->phase == JAREK_CONTACT_RECOVERY) {
    if (--event->data[0] <= 0) {
      fighter->flags &= ~(unsigned)FF_BUSY;
      jarek_ready(fighter);
      return false;
    }
    battle_event_delay(event, 1);
    return true;
  }

  if (event->data[0] == 0) {
    result = battle_contact(fighter, &jarek_selector_25_record);
    if (result == CONTACT_MISS) {
      event->data[0] = 1;
      fighter_start_move(fighter, S_PUNCH, &s_jab);
      fighter_start_fiber(fighter, retail_script_004e80c8, NULL);
      return false;
    }
    jarek_start_contact_recovery(event, fighter, s_low_kick.recovery);
    return true;
  }

  if (event->data[0] == 1) {
    result = battle_contact(fighter, &jarek_selector_26_record);
    if (result != CONTACT_MISS) {
      event->data[0] = 2;
      fighter_start_move(fighter, S_KICK, &s_high_kick);
      fighter_start_fiber(fighter, retail_script_004e80f4, NULL);
      return false;
    }
    jarek_start_contact_recovery(event, fighter, 6);
    return true;
  }

  result = battle_contact(fighter, &jarek_selector_24_record);
  jarek_start_contact_recovery(event, fighter, result == CONTACT_MISS ? 5 : s_high_kick.recovery);
  return true;
}

static void jarek_contact_dispatch(Fighter *fighter, int stage) {
  BattleEvent *event = battle_event_start(fighter, jarek_contact_event, false, true);
  event->data[0] = stage;
}

static void jarek_script_contact_25(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 0);
}

static void jarek_script_contact_26(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 1);
}

static void jarek_script_contact_24(Fighter *fighter) {
  jarek_contact_dispatch(fighter, 2);
}

static bool jarek_fan_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  (void)victim;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->phase == 0) {
    event->phase = JAREK_FAN_SCENE;
    event->data[0] = 0;
    battle_event_delay(event, 1);
    return true;
  }
  if (event->phase == JAREK_FAN_SCENE) {
    if (event->data[0]++ == 0) {
      fighter_start_fiber(fighter, retail_script_004e0138, NULL);
      battle_event_delay(event, 78);
      return true;
    }
    fighter_start_fiber(fighter, retail_script_004e0148, NULL);
    fighter_start_fiber((Fighter *)fighter_opponent(fighter), retail_script_004e01a0, NULL);
    event->phase = JAREK_FAN_FINISH;
    battle_event_delay(event, 29);
    return true;
  }
  return false;
}

static void jarek_script_fan_scene(Fighter *fighter) {
  (void)battle_event_start(fighter, jarek_fan_event, false, true);
}

static void jarek_script_fan_victim_scene(Fighter *fighter) {
  Fighter *owner = (Fighter *)fighter_opponent(fighter);
  Fighter *victim = (Fighter *)fighter_opponent(owner);
  mk4_script_scene_pose(victim, 416, 108, 69);
  fighter_start_fiber(owner, retail_script_004e01b0, NULL);
}

static bool jarek_fan_finish_event(BattleEvent *event, Fighter *fighter, Fighter *victim) {
  (void)victim;
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->data[0]++ >= 102) {
    battle_end_finisher();
    return false;
  }
  if (event->data[0] == 1) {
    mk4_script_scene_pose((Fighter *)fighter_opponent(fighter), 416, 108, 69);
  }
  battle_event_delay(event, 1);
  return true;
}

static void jarek_script_fan_finish(Fighter *fighter) {
  (void)battle_event_start(fighter, jarek_fan_finish_event, false, true);
}

static void jarek_ai_projectile_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  if (battle_projectile_overlaps(projectile, &jarek_selector_25_record)) {
    Fighter *owner = &g_battle.fighters[projectile->owner];
    projectile->kind = 0;
    fighter_start_fiber(owner, retail_script_004ed190, NULL);
  }
}

static void jarek_ai_projectile_spawn(Fighter *fighter, int task_type) {
  require(task_type == 78, "Unexpected Jarek cooperative task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, jarek_ai_projectile_step);
  if (projectile == NULL) {
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void jarek_script_kick_finish(Fighter *fighter) {
  for (int sample = 9; sample <= 10; ++sample) {
    fighter_wait_ticks(1);
    fighter->sample_override = sample;
    if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
      battle_contact(fighter, &jarek_high_kick_record);
    }
  }
  retail_script_004e5c70(fighter);
}

static void jarek_script_low_kick_avoid(Fighter *fighter) {
  fighter->avoids_high = true;
}

static void jarek_script_low_kick_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}

static void jarek_script_low_kick_finish(Fighter *fighter) {
  fighter->avoids_high = false;
  fighter_fiber_finish(jarek_ready);
}

static void jarek_script_air_finish(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(mk4_land);
}

static void jarek_script_uppercut_finish(Fighter *fighter) {
  const AttackRecord *record = (fighter->flags & FF_FOLLOW_UP) ? &jarek_uppercut_followup_record : &jarek_uppercut_record;
  for (int sample = 6; sample <= 8; ++sample) {
    fighter_wait_ticks(sample == 6 ? 2 : 1);
    fighter->sample_override = sample;
    if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001 && battle_contact(fighter, record) != CONTACT_MISS) {
      fighter->action = ACTION_RETAIL_1006;
    }
  }
  fighter_fiber_finish(jarek_ready);
}

static void jarek_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_sweep_record);
  }
  fighter_fiber_finish(jarek_ready);
}

static void jarek_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer != 0) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void jarek_script_roundhouse_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_roundhouse_record);
  }
  fighter_fiber_finish(jarek_ready);
}

static void jarek_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_high_punch);
}

static void jarek_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? jarek_lp_throw : jarek_jab);
}

static void jarek_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_high_kick);
}

static void jarek_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? jarek_sweep : jarek_low_kick);
}

static void jarek_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_uppercut);
}

static void jarek_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_sweep);
}

static void jarek_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_hp);
}

static void jarek_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_lp);
}

static void jarek_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_kick);
}

static void jarek_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_throw);
}

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = jarek_press_punch,
    [BUTTON_LOW_PUNCH] = jarek_press_low_punch,
    [BUTTON_HIGH_KICK] = jarek_press_kick,
    [BUTTON_LOW_KICK] = jarek_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = jarek_press_uppercut,
    [BUTTON_LOW_PUNCH] = jarek_press_uppercut,
    [BUTTON_HIGH_KICK] = jarek_press_sweep,
    [BUTTON_LOW_KICK] = jarek_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = jarek_press_air_hp,
    [BUTTON_LOW_PUNCH] = jarek_press_air_lp,
    [BUTTON_HIGH_KICK] = jarek_press_air_kick,
    [BUTTON_LOW_KICK] = jarek_press_air_kick}};
static const ButtonTable air_punch_followup = {{[BUTTON_LOW_PUNCH] = jarek_press_air_lp}};
static const ButtonTable air_followup = {{[BUTTON_HIGH_KICK] = jarek_press_air_kick, [BUTTON_LOW_KICK] = jarek_press_air_kick}};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = jarek_press_punch}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = jarek_press_punch}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_KICK] = jarek_press_kick}};

static void jarek_input_target(Fighter *fighter, Button button) {
  fighter_press(fighter, button);
}

static void jarek_command_469a00(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_command_469a10(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469a20(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469a30(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469a40(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_command_469a50(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469a60(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469a70(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_469a80(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_469a90(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469aa0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_BLOCK);
}

static void jarek_command_469ab0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_KICK);
}

static void jarek_command_469ac0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_command_469ad0(Fighter *fighter) {
  jarek_input_target(fighter, BUTTON_LOW_KICK);
}

static void jarek_command_467cd0(Fighter *fighter) {
  jarek_fan_fatality(fighter);
}

static void jarek_command_467ce0(Fighter *fighter) {
  jarek_spike_fatality(fighter);
}

static void jarek_command_46d1d0(Fighter *fighter) {
  jarek_weapon_action(fighter);
}

static void jarek_command_4797f0(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_low_kick, retail_script_004e80a0);
}

static void jarek_command_47b310(Fighter *fighter) {
  jarek_low_kick(fighter);
}

static void jarek_command_47b5d0(Fighter *fighter) {
  jarek_start(fighter, S_KICK, &s_high_kick, retail_script_004e81a8);
}

static void jarek_command_497540(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004ed118, NULL);
}

static void jarek_script_weapon_draw_finish(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

static void jarek_script_47b600(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_damage(victim, 0x23d7);
  battle_react(victim, 136);
  fighter_fiber_finish(jarek_ready);
}

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = jarek_idle,
    [S_WALK] = jarek_walk,
    [S_CROUCH] = jarek_crouch,
    [S_JUMP] = NULL,
    [S_PUNCH] = jarek_jab,
    [S_KICK] = jarek_high_kick,
    [S_SWEEP] = jarek_sweep,
    [S_UPPERCUT] = jarek_uppercut,
    [S_THROW] = jarek_lp_throw,
    [S_BLOCK] = jarek_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = jarek_fan_victory,
};

static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping}};

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

static void jarek_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void jarek_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void jarek_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &jarek_weapon);
}

static const WeaponDef jarek_weapon = {
    0x6a, jarek_weapon_hp, jarek_weapon_lp, jarek_weapon_back_hp, jarek_weapon_back_lp, NULL, weapon_throw, jarek_weapon_equip, SCENE_MODEL_SWORD};

static void jarek_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
    return;
  }
  if (fighter->flags & FF_FOLLOW_UP) {
    jarek_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    jarek_weapon_back_hp(fighter);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_weapon_draw);
  weapon_draw(fighter, &jarek_weapon);
  fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
}

const CharacterDef mk4_jarek = {GAME_MK4, 12, "JAREK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, NULL};
