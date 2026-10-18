#include "game.h"
#include "mk4_characters.h"
#include "script.h"

/* Native controller return used by generated VM programs and native runners. */
static void reiko_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/reiko.inc"

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
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {113, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};
static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
const uint32_t reiko_weapon_descriptor[] = {0x69, 0x141b95, 0x142dca, 0x13b33c, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffb334};

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

static void reiko_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(reiko_ready);
}

static void reiko_script_lp_select(Fighter *fighter) {
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

static void reiko_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004de664, NULL);
  retail_script_004de668(fighter);
}

static void reiko_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_fiber_finish(reiko_ready);
}

static void reiko_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void reiko_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void reiko_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void reiko_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void reiko_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void reiko_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void reiko_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(reiko_ready);
}

static void reiko_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void reiko_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void reiko_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void reiko_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 102 || reaction == 107) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
    fighter_start_fiber(victim, reaction == 102 ? retail_script_004e9bd0 : retail_script_004e9b40, NULL);
    fighter_fiber_finish(reiko_ready);
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(reiko_ready);
    return;
  }
  require(false, "Unexpected Reiko opponent reaction");
}

static void reiko_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void reiko_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void reiko_lp_throw(Fighter *fighter) {
  reiko_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

static void reiko_lk_throw(Fighter *fighter) {
  reiko_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void reiko_jab(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void reiko_uppercut(Fighter *fighter) {
  reiko_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void reiko_sweep(Fighter *fighter) {
  reiko_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void reiko_air_hp(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void reiko_air_lp(Fighter *fighter) {
  reiko_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void reiko_air_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void reiko_air_diagonal_hk(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void reiko_air_diagonal_lk(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void reiko_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, retail_script_004e5e28, NULL);
}

static void reiko_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void reiko_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void reiko_high_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void reiko_low_kick(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void reiko_roundhouse(Fighter *fighter) {
  reiko_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void reiko_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_jab);
}

static void reiko_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? reiko_lp_throw : reiko_jab);
}

static void reiko_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_high_kick);
}

static void reiko_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? reiko_sweep : reiko_low_kick);
}

static void reiko_press_crouching_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_uppercut);
}

static void reiko_press_crouching_kick(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_sweep);
}

static void reiko_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_air_hp);
}

static void reiko_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, reiko_air_lp);
}

static void reiko_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reiko_air_diagonal_hk : reiko_air_kick);
}

static void reiko_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? reiko_air_diagonal_lk : reiko_air_kick);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable reiko_standing = {{[BUTTON_HIGH_PUNCH] = reiko_press_high_punch, [BUTTON_LOW_PUNCH] = reiko_press_low_punch, [BUTTON_HIGH_KICK] = reiko_press_high_kick, [BUTTON_LOW_KICK] = reiko_press_low_kick}};
static const ButtonTable reiko_crouching = {{[BUTTON_HIGH_PUNCH] = reiko_press_crouching_hp, [BUTTON_LOW_PUNCH] = reiko_press_crouching_hp, [BUTTON_HIGH_KICK] = reiko_press_crouching_kick, [BUTTON_LOW_KICK] = reiko_press_crouching_kick}};
static const ButtonTable reiko_air = {{[BUTTON_HIGH_PUNCH] = reiko_press_air_hp, [BUTTON_LOW_PUNCH] = reiko_press_air_lp, [BUTTON_HIGH_KICK] = reiko_press_air_hk, [BUTTON_LOW_KICK] = reiko_press_air_lk}};
// clang-format on

static const Controls reiko_controls = {NULL, 0, {[STANCE_STANDING] = &reiko_standing, [STANCE_CROUCHING] = &reiko_crouching, [STANCE_JUMP] = &reiko_air}};

const CharacterDef mk4_reiko = {GAME_MK4, 7, "REIKO", &mk4_motion, moves, S_COUNT, &reiko_controls, NULL, -77660, mk4_walk, mk4_jump, NULL};
