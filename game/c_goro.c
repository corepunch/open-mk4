#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

static void goro_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void goro_script_lk_return(Fighter *fighter);
static void goro_script_lk_finish(Fighter *fighter);
static void goro_script_lp_pair_start(Fighter *fighter);
static void goro_script_lp_phase_repeat(Fighter *fighter);
static void goro_script_event_finish(Fighter *fighter);
static void goro_event_progress(Fighter *fighter);
static void goro_event_counter_kick(Fighter *fighter);
static void goro_event_counter_sweep(Fighter *fighter);
static void goro_cpu_projectile_spawn(Fighter *fighter, int task_type);
static const MoveDef s_knockdown;

#include "generated/goro.inc"
#include "generated/frames_goro.inc"

/* Encoded Goro roster descriptor retained from the EXE inventory at 0x4ecf18. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t goro_retail_weapon_descriptor[] = {0x6c, 0x141bf0, 0x142dd3, 0x13b330, 0x13eeb6, 0x13ee98, 0x13ee9b, 0x0};
// clang-format on

static void goro_script_current_reaction(Fighter *fighter, int reaction) {
  /* Goro's roster entry makes battle_react select the roster-15 fallback. */
  battle_react(fighter, reaction);
}

static void goro_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(goro_ready);
    return;
  }
  require(false, "Unexpected Goro opponent reaction");
}

static void goro_script_lk_return(Fighter *fighter) {
  /* Retail 42be10 selects the follow-up stream after its task-state checks. */
  retail_script_004de358(fighter);
}

static void goro_script_lk_finish(Fighter *fighter) {
  /* The portable fiber owns the paired throw lifetime and actor return. */
  mk4_script_pair_cleanup(fighter);
  fighter_fiber_finish(goro_ready);
}

static void goro_script_lp_pair_start(Fighter *fighter) {
  mk4_script_pair_victim(fighter, &s_knockdown);
}

static void goro_script_lp_phase_repeat(Fighter *fighter) {
  fighter->script_phase++;
  if (fighter->script_phase < 5) {
    retail_script_004de7e8(fighter);
    return;
  }
  retail_script_004de6e4(fighter);
}

static void goro_script_event_finish(Fighter *fighter) {
  fighter_fiber_finish(goro_ready);
}

static void goro_start_actor_event(Fighter *fighter, FighterFn script, int clip, int count, int action) {
  require(fighter != NULL && script != NULL && count > 0, "Invalid Goro actor event");
  fighter->playback_move = *fighter_move(fighter);
  fighter->playback_move.clip = clip;
  fighter->playback_move.first = 0;
  fighter->playback_move.last = count - 1;
  fighter->playback_move.flags |= SF_LOCK;
  fighter->playback_move.sequence = NULL;
  fighter->playback_move.action = action != 0 ? action : fighter->action;
  fighter_start_move(fighter, fighter->state, &fighter->playback_move);
  fighter_start_fiber(fighter, script, NULL);
}

static void goro_event_progress(Fighter *fighter) {
  goro_start_actor_event(fighter, retail_script_004dfb60, 599, 57, 0);
}

static void goro_event_counter_kick(Fighter *fighter) {
  goro_start_actor_event(fighter, retail_script_004dfb70, 598, 31, 0);
}

static void goro_event_counter_sweep(Fighter *fighter) {
  /* Native 43b320 writes 0x0409 to the selected fighter before script entry. */
  goro_start_actor_event(fighter, retail_script_004dfbb8, 601, 52, 0x0409);
}

static void goro_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void goro_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void goro_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void goro_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, &retail_script_004e0138_repeat_phase, retail_script_004e0148, retail_script_004e01a0);
}

static void goro_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void goro_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void goro_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(goro_ready);
}

static void goro_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void goro_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void goro_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

void goro_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

void goro_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {590, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {603, 18, 0, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {602, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {597, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {598, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {601, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {600, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {604, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_cpu_projectile = {.clip = 592, .last = 24, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_block = {595, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {593, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {581, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {594, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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

static void goro_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

/* Portable task83 fallback uses a known Goro box while its retail payload remains unavailable. */
static const AttackRecord goro_cpu_projectile_record = {0x12147, 0x51e, 0x9999, 0x4ccc, 23, 23, 4, ATTACK_MARK, 0x1999};

static void goro_cpu_projectile_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  projectile->sample = projectile->ticks < 24 ? projectile->ticks : 24;
  if (battle_projectile_overlaps(projectile, &goro_cpu_projectile_record) &&
      battle_projectile_contact(projectile, &goro_cpu_projectile_record) != CONTACT_MISS) {
    projectile->kind = 0;
  }
}

static void goro_cpu_projectile_spawn(Fighter *fighter, int task_type) {
  require(task_type == 83, "Unexpected Goro cooperative task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, goro_cpu_projectile_step);
  if (projectile == NULL) {
    return;
  }
  projectile->clip = 592;
  projectile->sample = 14;
  memcpy(projectile->pos, fighter->body.pos, sizeof projectile->pos);
  memcpy(projectile->heading, fighter->heading, sizeof projectile->heading);
  projectile->facing = fighter->facing;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void goro_cpu_projectile(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_cpu_projectile, retail_script_004ed790);
}

void goro_roundhouse(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

void goro_lk_throw(Fighter *fighter) {
  goro_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void goro_jab(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void goro_lp_throw(Fighter *fighter) {
  fighter->script_phase = 0;
  goro_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void goro_high_punch(Fighter *fighter) {
  goro_jab(fighter);
}

static void goro_uppercut(Fighter *fighter) {
  goro_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void goro_sweep(Fighter *fighter) {
  goro_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void goro_high_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void goro_low_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void goro_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void goro_air_hp(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void goro_air_lp(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void goro_air_kick(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void goro_air_diagonal_hk(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void goro_air_diagonal_lk(Fighter *fighter) {
  goro_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void goro_enter_jab(Fighter *fighter) {
  fighter_interrupt(fighter, goro_jab);
}

static void goro_enter_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? goro_lp_throw : goro_jab);
}

static void goro_enter_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, goro_uppercut);
}

static void goro_enter_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, goro_sweep);
}

static void goro_enter_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_high_kick);
}

static void goro_enter_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_low_kick);
}

static void goro_enter_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_hp);
}

static void goro_enter_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_lp);
}

static void goro_enter_air_hk(Fighter *fighter) {
  bool diagonal = fighter->body.vel[AXIS_X] != 0 || fighter->body.vel[AXIS_Z] != 0;
  fighter_interrupt(fighter, diagonal ? goro_air_diagonal_hk : goro_air_kick);
}

static void goro_enter_air_lk(Fighter *fighter) {
  bool diagonal = fighter->body.vel[AXIS_X] != 0 || fighter->body.vel[AXIS_Z] != 0;
  fighter_interrupt(fighter, diagonal ? goro_air_diagonal_lk : goro_air_kick);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable goro_standing = {{[BUTTON_HIGH_PUNCH] = goro_enter_jab, [BUTTON_LOW_PUNCH] = goro_enter_low_punch, [BUTTON_HIGH_KICK] = goro_enter_high_kick, [BUTTON_LOW_KICK] = goro_enter_low_kick}};
static const ButtonTable goro_crouching = {{[BUTTON_HIGH_PUNCH] = goro_enter_uppercut, [BUTTON_LOW_PUNCH] = goro_enter_uppercut, [BUTTON_HIGH_KICK] = goro_enter_sweep, [BUTTON_LOW_KICK] = goro_enter_sweep}};
static const ButtonTable goro_jumping = {{[BUTTON_HIGH_PUNCH] = goro_enter_air_hp, [BUTTON_LOW_PUNCH] = goro_enter_air_lp, [BUTTON_HIGH_KICK] = goro_enter_air_hk, [BUTTON_LOW_KICK] = goro_enter_air_lk}};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef goro_commands[] = {
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, goro_air_throw},
};
// clang-format on

static const Controls goro_controls = {
    goro_commands, 1, {[STANCE_STANDING] = &goro_standing, [STANCE_CROUCHING] = &goro_crouching, [STANCE_JUMP] = &goro_jumping}};

static const CpuMoves goro_cpu_moves = {.enter = {[CPU_PROJECTILE] = goro_cpu_projectile}};

const CharacterDef mk4_goro = {GAME_MK4, 15, "GORO", &mk4_motion, moves, S_COUNT, &goro_controls, NULL, -85458, mk4_walk, NULL, &goro_cpu_moves};
