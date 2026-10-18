#include "game.h"
#include "mk4_characters.h"
#include "script.h"

/* Native controller return used by generated VM programs and native runners. */
static void noob_saibot_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void noob_saibot_script_air_throw_continue(Fighter *fighter);
static void noob_saibot_script_air_throw_end(Fighter *fighter);

#include "generated/noob_saibot.inc"
#include "generated/frames_noob_saibot.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {78, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_noob_air_throw = {.clip = 74, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_noob_air_thrown = {.clip = 150, .last = 27, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_noob_shadow_low = {.clip = 26, .last = 13, .flags = SF_LOCK, .action = 0x309};
static const MoveDef s_noob_shadow_up = {.clip = 24, .last = 9, .flags = SF_LOCK, .action = 0x30a};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {113, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
const uint32_t noob_saibot_weapon_descriptor[] = {0x69, 0x141b95, 0x142dca, 0x13b33c, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffb334};

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

static void noob_saibot_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_lp_select(Fighter *fighter) {
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

static void noob_saibot_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004de664, NULL);
  retail_script_004de668(fighter);
}

static void noob_saibot_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void noob_saibot_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void noob_saibot_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void noob_saibot_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void noob_saibot_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void noob_saibot_script_air_throw_continue(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x147a;
  victim->body.gravity = 0x147a;
  fighter->body.vel[AXIS_Y] = -0x1999;
  victim->body.vel[AXIS_Y] = -0x1999;
  retail_script_004e5ea0(fighter);
}

static void noob_saibot_script_air_throw_end(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_damage(victim, 0x30a3);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void noob_saibot_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void noob_saibot_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void noob_saibot_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 102 || reaction == 107) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
    fighter_start_fiber(victim, reaction == 102 ? retail_script_004e9bd0 : retail_script_004e9b40, NULL);
    fighter_fiber_finish(noob_saibot_ready);
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

static void noob_saibot_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void noob_saibot_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void noob_saibot_lp_throw(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void noob_saibot_lk_throw(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void noob_saibot_jab(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void noob_saibot_uppercut(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void noob_saibot_sweep(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void noob_saibot_air_hp(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void noob_saibot_air_lp(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void noob_saibot_air_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void noob_saibot_air_diagonal_hk(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void noob_saibot_air_diagonal_lk(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void noob_saibot_air_throw(Fighter *fighter) {
  mk4_air_throw_generated_with_moves(fighter, &s_noob_air_throw, &s_noob_air_thrown, retail_script_004e5e78, retail_script_004e9910);
}

static void noob_saibot_shadow_low(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_low, retail_script_004ed268);
}

static void noob_saibot_shadow_up(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_PUNCH, &s_noob_shadow_up, retail_script_004ed2c0);
}

void noob_saibot_command_498b70(Fighter *fighter);
void noob_saibot_command_498e70(Fighter *fighter);

static bool noob_saibot_shadow_up_enabled(const Fighter *fighter) {
  /* 45ef20 returns true exactly when current-group +0x4c (gravity) is nonzero. */
  return fighter->body.gravity != 0;
}

static const CommandDef noob_saibot_commands[] = {
    {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0, NULL, noob_saibot_command_498b70},
    {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, CMD_AIR, noob_saibot_shadow_up_enabled, noob_saibot_command_498e70},
};

void noob_saibot_command_498b70(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_shadow_low);
}

void noob_saibot_command_498e70(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_shadow_up);
}

static void noob_saibot_shadow_task_low(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_LOW);
}

static void noob_saibot_shadow_task_up(Fighter *fighter, int task_type) {
  require(task_type == 75, "Unexpected Noob Saibot shadow task type");
  battle_shadow_actor_start(fighter, SHADOW_ACTOR_UP);
}

static void noob_saibot_shadow_install_physics(Fighter *fighter) {
  battle_shadow_actor_install_physics(fighter);
  fighter_fiber_finish(noob_saibot_ready);
}

static void noob_saibot_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void noob_saibot_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void noob_saibot_high_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void noob_saibot_low_kick(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void noob_saibot_roundhouse(Fighter *fighter) {
  noob_saibot_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void noob_saibot_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_jab);
}

static void noob_saibot_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? noob_saibot_lp_throw : noob_saibot_jab);
}

static void noob_saibot_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_high_kick);
}

static void noob_saibot_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? noob_saibot_sweep : noob_saibot_low_kick);
}

static void noob_saibot_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_uppercut);
}

static void noob_saibot_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_sweep);
}

static void noob_saibot_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_air_hp);
}

static void noob_saibot_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, noob_saibot_air_lp);
}

static void noob_saibot_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? noob_saibot_air_diagonal_hk : noob_saibot_air_kick);
}

static void noob_saibot_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? noob_saibot_air_diagonal_lk : noob_saibot_air_kick);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable noob_saibot_standing = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_high_punch, [BUTTON_LOW_PUNCH] = noob_saibot_press_low_punch, [BUTTON_HIGH_KICK] = noob_saibot_press_high_kick, [BUTTON_LOW_KICK] = noob_saibot_press_low_kick}};
static const ButtonTable noob_saibot_crouching = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_crouching_hp, [BUTTON_LOW_PUNCH] = noob_saibot_press_crouching_hp, [BUTTON_HIGH_KICK] = noob_saibot_press_crouching_kick, [BUTTON_LOW_KICK] = noob_saibot_press_crouching_kick}};
static const ButtonTable noob_saibot_air = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_air_hp, [BUTTON_LOW_PUNCH] = noob_saibot_press_air_lp, [BUTTON_HIGH_KICK] = noob_saibot_press_air_hk, [BUTTON_LOW_KICK] = noob_saibot_press_air_lk}};
// clang-format on

static const Controls noob_saibot_controls = {noob_saibot_commands, (int)(sizeof noob_saibot_commands / sizeof noob_saibot_commands[0]),
    {[STANCE_STANDING] = &noob_saibot_standing, [STANCE_CROUCHING] = &noob_saibot_crouching, [STANCE_JUMP] = &noob_saibot_air}};

const CharacterDef mk4_noob_saibot = {GAME_MK4, 17, "NOOB SAIBOT", &mk4_motion, moves, S_COUNT, &noob_saibot_controls, NULL, -77660, mk4_walk, mk4_jump, NULL};
