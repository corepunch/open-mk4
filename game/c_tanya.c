#include "game.h"
#include "mk4_characters.h"
#include "script.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {79, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {124, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_air_throw = {82, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_air_thrown = {202, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

static void tanya_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/tanya.inc"

static void tanya_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(tanya_ready);
}

static void tanya_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void tanya_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void tanya_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void tanya_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void tanya_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(tanya_ready);
    return;
  }
  require(false, "Unexpected Tanya opponent reaction");
}

static void tanya_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void tanya_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void tanya_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(tanya_ready);
}

static void tanya_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void tanya_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void tanya_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void tanya_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter_start_fiber(fighter, script, NULL);
}

static void tanya_lp_throw(Fighter *fighter) {
  tanya_start_script(fighter, S_THROW, &s_throw, retail_script_004de798);
}

void tanya_lk_throw(Fighter *fighter) {
  tanya_start_script(fighter, S_THROW, &s_throw, retail_script_004de2f8);
}

static void tanya_jab(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void tanya_high_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void tanya_low_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e5d90);
}

static void tanya_sweep(Fighter *fighter) {
  tanya_start_script(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void tanya_uppercut(Fighter *fighter) {
  tanya_start_script(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

void tanya_roundhouse(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void tanya_air_hp(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void tanya_air_lp(Fighter *fighter) {
  tanya_start_script(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void tanya_air_kick(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

void tanya_air_diagonal_hk(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void tanya_air_diagonal_lk(Fighter *fighter) {
  tanya_start_script(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

void tanya_air_throw(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_THROW, &s_air_throw);
  fighter_start_move(victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter_start_fiber(fighter, retail_script_004e5e28, NULL);
}

void tanya_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

void tanya_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

void tanya_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void tanya_press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_jab);
}

static void tanya_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? tanya_lp_throw : tanya_jab);
}

static void tanya_press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_high_kick);
}

static void tanya_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? tanya_sweep : tanya_low_kick);
}

static void tanya_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_uppercut);
}

static void tanya_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_sweep);
}

static void tanya_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_hp);
}

static void tanya_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_lp);
}

static void tanya_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? tanya_air_diagonal_hk : tanya_air_kick);
}

static void tanya_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_diagonal_lk);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = tanya_press_high_punch, [BUTTON_LOW_PUNCH] = tanya_press_low_punch, [BUTTON_HIGH_KICK] = tanya_press_high_kick, [BUTTON_LOW_KICK] = tanya_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = tanya_press_uppercut, [BUTTON_LOW_PUNCH] = tanya_press_uppercut, [BUTTON_HIGH_KICK] = tanya_press_sweep, [BUTTON_LOW_KICK] = tanya_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = tanya_press_air_hp, [BUTTON_LOW_PUNCH] = tanya_press_air_lp, [BUTTON_HIGH_KICK] = tanya_press_air_hk, [BUTTON_LOW_KICK] = tanya_press_air_lk}};
// clang-format on

static const Controls tanya_controls = {NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping}};

static void tanya_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static const FighterFn enter[S_COUNT] = {
    [S_PUNCH] = tanya_jab,
    [S_KICK] = tanya_high_kick,
    [S_SWEEP] = tanya_sweep,
    [S_UPPERCUT] = tanya_uppercut,
    [S_THROW] = tanya_lp_throw,
};

/* Retail Tanya uses the shared standing/crouching button tables. Air attacks
 * and LK throw remain explicit entries for command and CPU callers. */

/* Retail descriptor 4ecd98: preserve the encoded eight-word data block. */
const uint32_t tanya_weapon_descriptor[] = {0x6d, 0x141bfe, 0x142dd9, 0x13b31e, 0x13eeb6, 0x13ee98, 0x13ee9b, 0xffffcccd};

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

const CharacterDef mk4_tanya = {GAME_MK4, 9, "TANYA", &mk4_motion, moves, S_COUNT, &tanya_controls, enter, -71100, mk4_walk, mk4_jump, NULL};
