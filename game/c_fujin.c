#include "game.h"
#include "mk4_characters.h"
#include "script.h"

#include "generated/fujin.inc"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_retail_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {65, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
static const MoveDef s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD | SF_LOW};
static const MoveDef s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
static const MoveDef s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
static const MoveDef s_throw = {81, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_lk_throw = {.clip = 81, .last = 79, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_lk_thrown = {.clip = 158, .last = 47, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_air_throw = {.clip = 82, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_UNBLOCKABLE};
static const MoveDef s_air_thrown = {.clip = 202, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {120, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Retail inventory descriptor 0x4ece98, preserved as encoded EXE words. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t fujin_weapon_descriptor[] = {0x68, 0x141b6b, 0x142dc4, 0x13b360, 0x13edd5, 0x13eea4, 0x13eea7, 0xffffcccd};
// clang-format on

static void fujin_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void fujin_script_lk_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_lp_select(Fighter *fighter) {
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
    retail_script_004de610(fighter);
  }
}

static void fujin_script_current_reaction(Fighter *fighter, int reaction) {
  battle_react(fighter, reaction);
}

static void fujin_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }
  if (reaction == 77) {
    Fighter *victim = mk4_script_release_pair(fighter);
    victim->reaction = reaction;
    victim->flags |= FF_REACTING;
    fighter_fiber_finish(fujin_ready);
    return;
  }
  require(false, "Unexpected Fujin opponent reaction");
}

static void fujin_script_lp_extended_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 109;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  battle_react(victim, 109);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_lp_short_finish(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 102;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_knockdown);
  battle_react(victim, 102);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void fujin_script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void fujin_script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void fujin_script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void fujin_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void fujin_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void fujin_script_air_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(mk4_land);
}

static void fujin_script_finish(Fighter *fighter) {
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(fujin_ready);
}

static void fujin_script_roundhouse_start(Fighter *fighter) {
  if (fighter->script_timer) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void fujin_script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

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

static void fujin_low_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_low_kick);
  fighter_start_fiber(fighter, retail_script_004e5d90, NULL);
}

static void fujin_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void fujin_lk_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_lk_throw, &s_lk_thrown, retail_script_004de2f8);
}

static void fujin_lp_throw(Fighter *fighter) {
  mk4_script_throw_start(fighter, &s_throw, &s_lp_thrown, retail_script_004de798);
}

static void fujin_high_kick(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e5ca0);
}

static void fujin_sweep(Fighter *fighter) {
  fujin_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void fujin_uppercut(Fighter *fighter) {
  fujin_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void fujin_roundhouse(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e6800);
}

static void fujin_air_hp(Fighter *fighter) {
  fujin_start(fighter, S_PUNCH, &s_jab, retail_script_004e6658);
}

static void fujin_air_lp(Fighter *fighter) {
  fujin_start(fighter, S_PUNCH, &s_jab, retail_script_004e6710);
}

static void fujin_air_kick(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e61f8);
}

static void fujin_air_diagonal_hk(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e6500);
}

static void fujin_air_diagonal_lk(Fighter *fighter) {
  fujin_start(fighter, S_KICK, &s_kick, retail_script_004e63d8);
}

static void fujin_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void fujin_fan_fatality(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  fighter_start_fiber(fighter, retail_script_004e0118, NULL);
}

static void fujin_spike_fatality(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void fujin_jab(Fighter *fighter);

static void fujin_entry_roundhouse(Fighter *fighter) {
  fujin_roundhouse(fighter);
}

static void fujin_entry_low_kick(Fighter *fighter) {
  fujin_low_kick(fighter);
}

static void fujin_entry_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

static void fujin_entry_high_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

static void fujin_entry_fan(Fighter *fighter) {
  fujin_fan_fatality(fighter);
}

static void fujin_entry_spike(Fighter *fighter) {
  fujin_spike_fatality(fighter);
}

static void fujin_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_retail_jab);
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void fujin_state_move(Fighter *fighter, int state) {
  fighter_start_move(fighter, state, moves[state]);
}

static void fujin_enter_punch(Fighter *fighter) {
  fujin_jab(fighter);
}

static void fujin_enter_air_punch(Fighter *fighter) {
  fujin_air_hp(fighter);
}

static void fujin_enter_kick(Fighter *fighter) {
  fujin_high_kick(fighter);
}

static void fujin_enter_sweep(Fighter *fighter) {
  fujin_sweep(fighter);
}

static void fujin_enter_uppercut(Fighter *fighter) {
  fujin_uppercut(fighter);
}

static void fujin_enter_throw(Fighter *fighter) {
  fujin_lp_throw(fighter);
}

static void fujin_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_punch);
}

static void fujin_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? fujin_enter_throw : fujin_enter_punch);
}

static void fujin_press_air_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_air_punch);
}

static void fujin_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_kick);
}

static void fujin_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? fujin_enter_sweep : fujin_low_kick);
}

static void fujin_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_uppercut);
}

static void fujin_press_roundhouse(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_roundhouse);
}

static void fujin_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_air_lp);
}

static void fujin_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? fujin_air_diagonal_hk : fujin_air_kick);
}

static void fujin_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? fujin_air_diagonal_lk : fujin_air_kick);
}

static void fujin_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, fujin_enter_sweep);
}

// clang-format off: Keep table entries on one row, including long rows.
static const ButtonTable fujin_standing = {{[BUTTON_HIGH_PUNCH] = fujin_press_punch, [BUTTON_LOW_PUNCH] = fujin_press_low_punch, [BUTTON_HIGH_KICK] = fujin_press_kick, [BUTTON_LOW_KICK] = fujin_press_low_kick}};
static const ButtonTable fujin_crouching = {{[BUTTON_HIGH_PUNCH] = fujin_press_uppercut, [BUTTON_LOW_PUNCH] = fujin_press_uppercut, [BUTTON_HIGH_KICK] = fujin_press_sweep, [BUTTON_LOW_KICK] = fujin_press_sweep}};
static const ButtonTable fujin_jumping = {{[BUTTON_HIGH_PUNCH] = fujin_press_air_punch, [BUTTON_LOW_PUNCH] = fujin_press_air_lp, [BUTTON_HIGH_KICK] = fujin_press_air_hk, [BUTTON_LOW_KICK] = fujin_press_air_lk}};
// clang-format on

static const Controls fujin_controls = {NULL, 0, {[STANCE_STANDING] = &fujin_standing, [STANCE_CROUCHING] = &fujin_crouching, [STANCE_JUMP] = &fujin_jumping}};

const CharacterDef mk4_fujin = {GAME_MK4, 5, "FUJIN", &mk4_motion, moves, S_COUNT, &fujin_controls, NULL, -73531, mk4_walk, mk4_jump, NULL};
