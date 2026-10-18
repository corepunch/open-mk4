#include "game.h"
#include "mk4_characters.h"
#include "script.h"

#include "generated/reptile.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {268, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {76, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_reaction_40 = {.clip = 183, .last = 33, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {121, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Reptile's retail inventory descriptor at 0x4ece78, preserved as encoded words. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reptile_weapon_descriptor[] = {0x67, 0x141b64, 0x142dc1, 0x13b35a, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffff999a};
// clang-format on

/* Retail D[23] at 0x4ec038; the 0x47a6d0 callback queries it during its loop. */
static const AttackRecord reptile_script_contact = {0xd1eb, 0x570a, 0x16872, 0xbfbe, 158, 158, 165, 0, 0x1eb8};
/* Retail D[21] at 0x4ec028; callback 0x47b260 probes it during recovery. */
static const AttackRecord reptile_loop_contact = {0xe872, -0x3f7c, 0xb84b, 0x56c8, 40, 10, 9, 0, 0x170a};

enum {
  REPTILE_EVENT_VISIBILITY = 1,
  REPTILE_EVENT_TASK_76
};

static void reptile_clone_step(Projectile *projectile) {
  (void)projectile;
}

static bool reptile_visibility_event(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  (void)opponent;
  event->data[0]++;
  if (event->data[0] >= event->data[2]) {
    fighter->appearance = event->data[1];
    fighter->flags &= ~(unsigned)FF_TELEPORTING;
    return false;
  }
  return true;
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

static void reptile_script_visibility_start(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
    return;
  }
  event = battle_event_start(fighter, reptile_visibility_event, false, true);
  event->data[1] = fighter->appearance;
  event->data[2] = 104;
  event->data[7] = REPTILE_EVENT_VISIBILITY;
  fighter->appearance = APPEARANCE_HIDDEN;
  fighter->flags |= FF_TELEPORTING;
}

static void reptile_script_visibility_extend(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
    event->data[2] = event->data[0] + 104;
  }
}

static void reptile_script_visibility_end(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    fighter->appearance = event->data[1];
    fighter->flags &= ~(unsigned)FF_TELEPORTING;
    battle_event_cancel(event);
  }
}

static void reptile_script_visibility_phase(Fighter *fighter) {
  BattleEvent *event = reptile_find_event(fighter, reptile_visibility_event, REPTILE_EVENT_VISIBILITY);
  if (event != NULL) {
    event->phase++;
  }
}

static bool reptile_task_76_event(BattleEvent *event, Fighter *fighter, Fighter *opponent) {
  int age = ++event->data[0];
  Projectile *clone = &g_battle.projectiles[event->data[1]];
  if (clone->kind == 0) {
    return false;
  }
  clone->sample = age < 13 ? age : 13;
  memcpy(clone->pos, fighter->body.pos, sizeof clone->pos);
  clone->facing = fighter->facing;
  memcpy(clone->heading, fighter->heading, sizeof clone->heading);
  if (clone->sample >= 11 && clone->sample <= 13 && battle_projectile_overlaps(clone, &reptile_script_contact)) {
    ContactResult result = battle_projectile_contact(clone, &reptile_script_contact);
    if (result != CONTACT_MISS) {
      fighter_note_contact(fighter, result);
      opponent->body_contact = true;
      clone->kind = 0;
      return false;
    }
  }
  if (age >= 14) {
    clone->kind = 0;
    return false;
  }
  return true;
}

static void reptile_task_76_spawn(Fighter *fighter, int task_type) {
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
  battle_sound(SOUND_C, 0x21);
}

static void reptile_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void reptile_script_lk_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_47a6d0(Fighter *fighter) {
  int last = fighter_move(fighter)->last;
  int remaining = 16;
  bool contacted = false;
  while (remaining > 0) {
    fighter_wait_ticks(1);
    if (fighter->sample_override < last) {
      fighter->sample_override++;
    }
    if (battle_contact(fighter, &reptile_script_contact) != CONTACT_MISS) {
      contacted = true;
      break;
    }
    remaining--;
  }
  if (!contacted) {
    retail_script_004e8148(fighter);
    return;
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_47ab50(Fighter *fighter) {
  retail_script_004e8174(fighter);
}

static void reptile_script_47b260(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &reptile_loop_contact);
  }
}

static void reptile_script_47b010(Fighter *fighter) {
  for (int remaining = 6; remaining > 0 && !fighter->connected; --remaining) {
    reptile_script_47b260(fighter);
    if (!fighter->connected) {
      fighter_wait_ticks(1);
    }
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_reaction_side_getup(Fighter *fighter) {
  if (fighter->health <= 0) {
    mk4_script_thrown_finish(fighter);
  }
  mk4_script_bind(fighter, 185, 16, 0);
  mk4_script_finish_clip(fighter, 1);
  mk4_script_thrown_finish(fighter);
}

static void reptile_script_lp_select(Fighter *fighter) {
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

static void reptile_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004de664, NULL);
  retail_script_004de668(fighter);
}

static void reptile_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 102 || reaction == 107) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
    fighter_start_fiber(victim, reaction == 102 ? retail_script_004e9bd0 : retail_script_004e9b40, NULL);
    fighter_fiber_finish(reptile_ready);
    return;
  }
  battle_react(fighter, reaction);
}

void reptile_reaction_40(Fighter *fighter) {
  fighter_start_move(fighter, S_KNOCKDOWN, &s_reaction_40);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, retail_script_004e9620, NULL);
}

static void reptile_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(reptile_ready);
    return;
  }
  require(false, "Unexpected Reptile opponent reaction");
}

static void reptile_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void reptile_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void reptile_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void reptile_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void reptile_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reptile_ready);
}

static void reptile_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void reptile_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void reptile_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void reptile_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void reptile_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_lp_thrown, retail_script_004de798);
}

static void reptile_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void reptile_jab(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void reptile_high_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void reptile_low_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void reptile_sweep(Fighter *fighter) {
  reptile_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void reptile_uppercut(Fighter *fighter) {
  reptile_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void reptile_roundhouse(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void reptile_input_47a690(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e8128);
}

static void reptile_input_47afe0(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e8178);
}

static void reptile_input_4988d0(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &s_jab, retail_script_004ed1e8);
}

static void reptile_air_hp(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void reptile_air_lp(Fighter *fighter) {
  reptile_start(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void reptile_air_kick(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void reptile_air_diagonal_hk(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void reptile_air_diagonal_lk(Fighter *fighter) {
  reptile_start(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void reptile_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void reptile_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void reptile_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void reptile_entry_fan(Fighter *fighter) {
  reptile_fan_fatality(fighter);
}

static void reptile_entry_spike(Fighter *fighter) {
  reptile_spike_fatality(fighter);
}

static void reptile_entry_block(Fighter *fighter) {
  fighter_enter_state(fighter, S_BLOCK);
}

static void reptile_entry_high_punch(Fighter *fighter) {
  reptile_jab(fighter);
}

static void reptile_entry_low_punch(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.7f) {
    reptile_lp_throw(fighter);
  } else {
    reptile_jab(fighter);
  }
}

static void reptile_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_jab);
}

static void reptile_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_entry_low_punch);
}

static void reptile_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_high_kick);
}

static void reptile_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reptile_sweep : reptile_low_kick);
}

static void reptile_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_uppercut);
}

static void reptile_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_sweep);
}

static void reptile_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_air_hp);
}

static void reptile_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, reptile_air_lp);
}

static void reptile_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reptile_air_diagonal_hk : reptile_air_kick);
}

static void reptile_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reptile_air_diagonal_lk : reptile_air_kick);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable reptile_standing = {{[BUTTON_HIGH_PUNCH] = reptile_press_punch, [BUTTON_LOW_PUNCH] = reptile_press_low_punch, [BUTTON_HIGH_KICK] = reptile_press_kick, [BUTTON_LOW_KICK] = reptile_press_low_kick}};
static const ButtonTable reptile_crouching = {{[BUTTON_HIGH_PUNCH] = reptile_press_uppercut, [BUTTON_LOW_PUNCH] = reptile_press_uppercut, [BUTTON_HIGH_KICK] = reptile_press_sweep, [BUTTON_LOW_KICK] = reptile_press_sweep}};
static const ButtonTable reptile_jumping = {{[BUTTON_HIGH_PUNCH] = reptile_press_air_hp, [BUTTON_LOW_PUNCH] = reptile_press_air_lp, [BUTTON_HIGH_KICK] = reptile_press_air_hk, [BUTTON_LOW_KICK] = reptile_press_air_lk}};
// clang-format on

static const Controls reptile_controls = {
    NULL, 0, {[STANCE_STANDING] = &reptile_standing, [STANCE_CROUCHING] = &reptile_crouching, [STANCE_JUMP] = &reptile_jumping}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_reptile = {GAME_MK4, 10, "REPTILE", &mk4_motion, moves, S_COUNT, &reptile_controls, NULL, -73531, mk4_walk, mk4_jump, NULL};
