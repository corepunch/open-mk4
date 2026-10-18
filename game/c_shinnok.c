#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"

static const MoveDef s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
static const MoveDef s_walk = {.clip = 267, .last = MOVE_END};
static const MoveDef s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};
static const MoveDef s_high_punch = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_high_kick = {.clip = 66, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
static const MoveDef s_low_kick = {.clip = 69, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};
static const MoveDef s_roundhouse = {.clip = 313, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010D};
static const MoveDef s_air_attack = {.clip = 0, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0601};
static const MoveDef s_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_thrown = {.clip = 157, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};
static const WeaponDef shinnok_weapon;

static void shinnok_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

#include "generated/shinnok.inc"
#include "generated/frames_shinnok.inc"

static void shinnok_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script) {
  fighter_start_move(fighter, state, move);
  fighter->sample_override = 0;
  fighter_start_fiber(fighter, script, NULL);
}

static void shinnok_idle(Fighter *fighter) {
  fighter_start_move(fighter, S_IDLE, &s_idle);
}

static void shinnok_walk(Fighter *fighter) {
  fighter_start_move(fighter, S_WALK, &s_walk);
}

static void shinnok_crouch(Fighter *fighter) {
  fighter_start_move(fighter, S_CROUCH, &s_crouch);
}

static void shinnok_block(Fighter *fighter) {
  fighter_start_move(fighter, S_BLOCK, &s_block);
}

static void shinnok_jab(Fighter *fighter) {
  shinnok_start(fighter, S_PUNCH, &s_jab, retail_script_004e62f0);
}

static void shinnok_high_punch(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_high_punch);
}

static void shinnok_high_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_high_kick, retail_script_004e5ca0);
}

static void shinnok_low_kick(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_low_kick, retail_script_004e5d90);
}

static void shinnok_sweep(Fighter *fighter) {
  shinnok_start(fighter, S_SWEEP, &s_sweep, retail_script_004e6798);
}

static void shinnok_uppercut(Fighter *fighter) {
  shinnok_start(fighter, S_UPPERCUT, &s_uppercut, retail_script_004e62a0);
}

static void shinnok_roundhouse(Fighter *fighter) {
  shinnok_start(fighter, S_KICK, &s_roundhouse, retail_script_004e6800);
}

static void shinnok_air_hp(Fighter *fighter) {
  shinnok_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6658);
}

static void shinnok_air_lp(Fighter *fighter) {
  shinnok_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6710);
}

static void shinnok_air_kick(Fighter *fighter) {
  shinnok_start(fighter, S_JUMP, &s_air_attack, retail_script_004e61f8);
}

static void shinnok_air_diagonal_hk(Fighter *fighter) {
  shinnok_start(fighter, S_JUMP, &s_air_attack, retail_script_004e6500);
}

static void shinnok_air_diagonal_lk(Fighter *fighter) {
  shinnok_start(fighter, S_JUMP, &s_air_attack, retail_script_004e63d8);
}

static void shinnok_ground_throw(Fighter *fighter, FighterFn script) {
  mk4_script_throw_start(fighter, &s_throw, &s_thrown, script);
}

static void shinnok_lp_throw(Fighter *fighter) {
  shinnok_ground_throw(fighter, retail_script_004de798);
}

static void shinnok_lk_throw(Fighter *fighter) {
  shinnok_ground_throw(fighter, retail_script_004de2f8);
}

static void shinnok_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void shinnok_script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void shinnok_fan_start(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void shinnok_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void shinnok_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void shinnok_fan_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_FAN)) {
    shinnok_ready(fighter);
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  mk4_script_scene_pose(fighter, 415, 130, 0);
  mk4_script_scene_pose(victim, 416, 108, 0);
  fighter_start_fiber(fighter, shinnok_fan_start, NULL);
}

static void shinnok_spike_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, FINISH_SPIKE)) {
    shinnok_ready(fighter);
    return;
  }
  fighter_start_fiber(fighter, retail_script_004e6950, NULL);
}

static void shinnok_throw_reaction(Fighter *fighter) {
  mk4_script_thrown_finish(fighter);
}

static void shinnok_fatality_contact_hold(Fighter *fighter);

static void shinnok_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    shinnok_fatality_contact_hold(fighter);
    return;
  }
  battle_react(fighter, reaction);
}

static void shinnok_script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Shinnok paired reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_fiber_finish(shinnok_ready);
}

/* Spike-fatality contact 0x4ebd80 uses reaction 175 with zero damage. The
 * stage coordinator owns the victim pose and recovery for that scene. */
static void shinnok_fatality_contact_hold(Fighter *fighter) {
  require(g_battle.phase == PHASE_FATALITY, "Shinnok reaction 175 outside fatality");
  fighter->script = NULL;
  fighter->pending = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
}

// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord shinnok_sweep_record = {95682, 31391, 68812, 35389, 18, 18, 4, ATTACK_LOW, 9175};
static const AttackRecord shinnok_roundhouse_record = {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796};
static const AttackRecord shinnok_uppercut_record = {58327, -64880, 38010, 66840, 34, 34, 6, 0, 10485};
static const AttackRecord shinnok_uppercut_followup_record = {73990, -70057, 64159, 96927, 34, 34, 6, 0, 2621};
// clang-format on

static void shinnok_script_kick_finish(Fighter *fighter) {
  retail_script_004e5c70(fighter);
}

static void shinnok_script_air_finish(Fighter *fighter) {
  (void)fighter;
  fighter_fiber_finish(mk4_land);
}

static void shinnok_uppercut_frame(Fighter *fighter) {
  const AttackRecord *record = (fighter->flags & FF_FOLLOW_UP) ? &shinnok_uppercut_followup_record : &shinnok_uppercut_record;
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001 && battle_contact(fighter, record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

static void shinnok_script_uppercut_finish(Fighter *fighter) {
  native_begin(fighter, 1, 4, NULL);
  for (int frame = 1; frame <= 4; ++frame) {
    native_frame(fighter, frame, frame == 1 ? fighter_sample(fighter) : frame + 4, frame >= 2 ? shinnok_uppercut_frame : NULL);
  }
  native_wait(fighter);
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &shinnok_sweep_record);
  }
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_script_roundhouse_start(Fighter *fighter) {
  if (fighter_fast_start(fighter)) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void shinnok_script_roundhouse_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &shinnok_roundhouse_record);
  }
  fighter_fiber_finish(shinnok_ready);
}

static void shinnok_press_punch(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_jab);
}

static void shinnok_press_low_punch(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(fighter) <= 0.7f ? shinnok_lp_throw : shinnok_jab);
}

static void shinnok_press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_high_kick);
}

static void shinnok_press_low_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? shinnok_sweep : shinnok_low_kick);
}

static void shinnok_press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_uppercut);
}

static void shinnok_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_sweep);
}

static void shinnok_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_air_hp);
}

static void shinnok_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_air_lp);
}

static void shinnok_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_air_kick);
}

static void shinnok_press_air_throw(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_air_throw);
}

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = shinnok_press_punch,
    [BUTTON_LOW_PUNCH] = shinnok_press_low_punch,
    [BUTTON_HIGH_KICK] = shinnok_press_kick,
    [BUTTON_LOW_KICK] = shinnok_press_low_kick}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = shinnok_press_uppercut,
    [BUTTON_LOW_PUNCH] = shinnok_press_uppercut,
    [BUTTON_HIGH_KICK] = shinnok_press_sweep,
    [BUTTON_LOW_KICK] = shinnok_press_sweep}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = shinnok_press_air_hp,
    [BUTTON_LOW_PUNCH] = shinnok_press_air_lp,
    [BUTTON_HIGH_KICK] = shinnok_press_air_kick,
    [BUTTON_LOW_KICK] = shinnok_press_air_kick}};
static const ButtonTable air_punch_followup = {{[BUTTON_LOW_PUNCH] = shinnok_press_air_lp}};
static const ButtonTable air_followup = {{[BUTTON_HIGH_KICK] = shinnok_press_air_kick, [BUTTON_LOW_KICK] = shinnok_press_air_kick}};
static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = shinnok_press_punch}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = shinnok_press_punch}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_KICK] = shinnok_press_kick}};

static void shinnok_input_target(Fighter *fighter, Button button) {
  fighter_press(fighter, button);
}

static void shinnok_command_469a00(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_command_469a10(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469a20(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469a30(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469a40(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_command_469a50(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469a60(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469a70(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_469a80(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_469a90(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469aa0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_BLOCK);
}

static void shinnok_command_469ab0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_KICK);
}

static void shinnok_command_469ac0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_command_469ad0(Fighter *fighter) {
  shinnok_input_target(fighter, BUTTON_LOW_KICK);
}

static void shinnok_command_467cd0(Fighter *fighter) {
  shinnok_fan_fatality(fighter);
}

static void shinnok_command_467ce0(Fighter *fighter) {
  shinnok_spike_fatality(fighter);
}

static void shinnok_command_46d1d0(Fighter *fighter) {
  weapon_draw(fighter, &shinnok_weapon);
}

static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = shinnok_idle,
    [S_WALK] = shinnok_walk,
    [S_CROUCH] = shinnok_crouch,
    [S_JUMP] = NULL,
    [S_PUNCH] = shinnok_jab,
    [S_KICK] = shinnok_high_kick,
    [S_SWEEP] = shinnok_sweep,
    [S_UPPERCUT] = shinnok_uppercut,
    [S_THROW] = shinnok_lp_throw,
    [S_BLOCK] = shinnok_block,
    [S_HIT] = NULL,
    [S_KNOCKDOWN] = NULL,
    [S_VICTORY] = shinnok_fan_victory,
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

static void shinnok_weapon_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_weapon_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_weapon_back_hp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_HIGH_PUNCH);
}

static void shinnok_weapon_back_lp(Fighter *fighter) {
  fighter_press(fighter, BUTTON_LOW_PUNCH);
}

static void shinnok_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &shinnok_weapon);
}

static const WeaponDef shinnok_weapon = {
    0x64, shinnok_weapon_hp, shinnok_weapon_lp, shinnok_weapon_back_hp, shinnok_weapon_back_lp, NULL, weapon_throw, shinnok_weapon_equip, SCENE_MODEL_SWORD};

const CharacterDef mk4_shinnok = {GAME_MK4, 6, "SHINNOK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, NULL};
