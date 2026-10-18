#include "character.h"

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

#include "generated/scripts.inc"
#include "generated/frames.inc"

static void jarek_fatality_contact_hold(Fighter *fighter);

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

static const WeaponDef jarek_weapon = {
    0x6a, jarek_weapon_hp, jarek_weapon_lp, jarek_weapon_back_hp, jarek_weapon_back_lp, NULL, weapon_throw, jarek_weapon_equip, SCENE_MODEL_SWORD};

const CharacterDef mk4_jarek = {GAME_MK4, 12, "JAREK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, NULL};

/* Private move implementations share this character's data and declarations. */
#include "moves/air_attacks.inc"
#include "moves/common.inc"
#include "moves/controls.inc"
#include "moves/fan.inc"
#include "moves/fatality.inc"
#include "moves/ground_attacks.inc"
#include "moves/kicks.inc"
#include "moves/projectile.inc"
#include "moves/punches.inc"
#include "moves/roundhouse.inc"
#include "moves/spike.inc"
#include "moves/sweep.inc"
#include "moves/throws.inc"
#include "moves/uppercut.inc"
#include "moves/weapon.inc"
