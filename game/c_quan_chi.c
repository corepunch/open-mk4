#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

static void quan_chi_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

#include "generated/quan_chi.inc"
#include "generated/frames_quan_chi.inc"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 267, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH};
static const MoveDef s_throw = {.clip = 78, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 120, .last = MOVE_END, .flags = SF_HOLD};

static void quan_chi_script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void quan_chi_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void quan_chi_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void quan_chi_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void quan_chi_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(quan_chi_ready);
    return;
  }
  require(false, "Unexpected Quan Chi opponent reaction");
}

static void quan_chi_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void quan_chi_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void quan_chi_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void quan_chi_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_script_sweep_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(quan_chi_ready);
}

static void quan_chi_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void quan_chi_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void quan_chi_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void quan_chi_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_lk_thrown, retail_script_004de798);
}

static void quan_chi_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void quan_chi_jab(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void quan_chi_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
}

static void quan_chi_high_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void quan_chi_low_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e5d90);
}

static void quan_chi_sweep(Fighter *fighter) {
  quan_chi_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void quan_chi_uppercut(Fighter *fighter) {
  quan_chi_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void quan_chi_roundhouse(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e6800);
}

static void quan_chi_air_hp(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void quan_chi_air_lp(Fighter *fighter) {
  quan_chi_start(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void quan_chi_air_kick(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e61f8);
}

static void quan_chi_air_diagonal_hk(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e6500);
}

static void quan_chi_air_diagonal_lk(Fighter *fighter) {
  quan_chi_start(fighter, S_KICK, &s_high_kick, retail_script_004e63d8);
}

static void quan_chi_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void quan_chi_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void quan_chi_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void quan_chi_entry_469c80(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void quan_chi_entry_46a020(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_weapon_draw(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void quan_chi_entry_47a370(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_47cf30(Fighter *fighter) {
  fighter_enter_state(fighter, S_KICK);
}

static void quan_chi_entry_499740(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}

static void press_high_punch(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_jab);
}

static void press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? quan_chi_lp_throw : quan_chi_jab);
}

static void press_high_kick(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_high_kick);
}

static void press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? quan_chi_sweep : quan_chi_low_kick);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_uppercut);
}

static void press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_sweep);
}

static void press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_lp);
}

static void press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? quan_chi_air_diagonal_hk : quan_chi_air_kick);
}

static void press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_diagonal_lk);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_uppercut, [BUTTON_HIGH_KICK] = press_sweep, [BUTTON_LOW_KICK] = press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable straight_air = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable air_punch_followup = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp}};
static const ButtonTable diagonal_air = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = press_high_punch, [BUTTON_LOW_PUNCH] = press_low_punch, [BUTTON_HIGH_KICK] = press_high_kick, [BUTTON_LOW_KICK] = press_low_kick}};
static const ButtonTable air_followup = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
// clang-format on

static const Controls quan_chi_controls = {NULL, 0, {[STANCE_STANDING] = &standing, [STANCE_CROUCHING] = &crouching, [STANCE_JUMP] = &jumping}};

static const FighterFn enter[S_COUNT] = {
    [S_PUNCH] = quan_chi_jab,
    [S_KICK] = quan_chi_high_kick,
    [S_SWEEP] = quan_chi_sweep,
    [S_UPPERCUT] = quan_chi_uppercut,
    [S_THROW] = quan_chi_lp_throw,
};

static const uint32_t quan_chi_weapon_descriptor[] = {0x6f, 0x1430c4, 0x144356, 0x13b312, 0x13eeb9, 0x13ee9e, 0x13eea1, 0xffffb334};

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

const CharacterDef mk4_quan_chi = {GAME_MK4, 8, "QUAN CHI", &mk4_motion, moves, S_COUNT, &quan_chi_controls, enter, -78643, mk4_walk, mk4_jump, NULL};
