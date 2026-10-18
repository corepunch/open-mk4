#include "character.h"

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

#include "generated/scripts.inc"
#include "generated/frames.inc"

static void shinnok_fatality_contact_hold(Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord shinnok_sweep_record = {95682, 31391, 68812, 35389, 18, 18, 4, ATTACK_LOW, 9175};
static const AttackRecord shinnok_roundhouse_record = {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796};
static const AttackRecord shinnok_uppercut_record = {58327, -64880, 38010, 66840, 34, 34, 6, 0, 10485};
static const AttackRecord shinnok_uppercut_followup_record = {73990, -70057, 64159, 96927, 34, 34, 6, 0, 2621};
// clang-format on

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

static const WeaponDef shinnok_weapon = {
    0x64, shinnok_weapon_hp, shinnok_weapon_lp, shinnok_weapon_back_hp, shinnok_weapon_back_lp, NULL, weapon_throw, shinnok_weapon_equip, SCENE_MODEL_SWORD};

const CharacterDef mk4_shinnok = {GAME_MK4, 6, "SHINNOK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
