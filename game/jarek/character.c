#include "character.h"

const MoveDef jarek_s_idle = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END};
const MoveDef jarek_s_walk = {.clip = 267, .last = MOVE_END};
const MoveDef jarek_s_crouch = {.clip = 259, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_jump = {.clip = 304, .last = MOVE_END};
const MoveDef jarek_s_jab = {.clip = 294, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_010B};

const MoveDef jarek_s_high_kick = {.clip = 65, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK, .action = ACTION_RETAIL_0100};
const MoveDef jarek_s_low_kick = {.clip = 308, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_0106};
const MoveDef jarek_s_sweep = {.clip = 63, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef jarek_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_LAUNCH, .action = ACTION_RETAIL_0109};

const MoveDef jarek_s_throw = {.clip = 37, .last = MOVE_END, .flags = SF_LOCK | SF_UNBLOCKABLE | SF_RECORD, .action = ACTION_RETAIL_0500};

const MoveDef jarek_s_block = {.clip = 322, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_hit = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_HOLD};
static const MoveDef s_knockdown = {.clip = 186, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION | SF_WAIT_LAND};
static const MoveDef s_victory = {.clip = 111, .last = MOVE_END, .flags = SF_HOLD};

#include "generated/scripts.inc"
#include "generated/frames.inc"

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
    [BUTTON_HIGH_KICK] = jarek_press_air_hk,
    [BUTTON_LOW_KICK] = jarek_press_air_lk}};

// clang-format off: Keep table entries on one row, including long rows.
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
  [S_VICTORY] = NULL,
};
// clang-format on

#include "generated/controls.inc"

static const Controls controls = {retail_commands, (int)(sizeof retail_commands / sizeof retail_commands[0]), {&standing, &crouching, &jumping}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &jarek_s_idle,
  [S_WALK] = &jarek_s_walk,
  [S_CROUCH] = &jarek_s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &jarek_s_jab,
  [S_KICK] = &jarek_s_high_kick,
  [S_SWEEP] = &jarek_s_sweep,
  [S_UPPERCUT] = &jarek_s_uppercut,
  [S_THROW] = &jarek_s_throw,
  [S_BLOCK] = &jarek_s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

const CharacterDef mk4_jarek = {GAME_MK4, 12, "JAREK", &mk4_motion, moves, S_COUNT, &controls, enter, -73531, mk4_walk, mk4_jump, &retail_cpu_moves, NULL, 0};

void jarek_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_script_004de2f8(fighter);
}

void jarek_entry_retail_script_004de640(Fighter *fighter) {
  retail_script_004de640(fighter);
}

void jarek_entry_retail_script_004de664(Fighter *fighter) {
  /* 4de664 is callback data consumed by48fcd0, not a VM stream.
   * Its native490350 toggles the captured peer facing. */
  fighter->facing = -fighter->facing;
}

void jarek_entry_retail_script_004de668(Fighter *fighter) {
  retail_script_004de668(fighter);
}

void jarek_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_script_004de6b8(fighter);
}

void jarek_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void jarek_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void jarek_entry_retail_script_004e0138(Fighter *fighter) {
  retail_script_004e0138(fighter);
}

void jarek_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void jarek_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void jarek_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void jarek_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void jarek_entry_retail_script_004e5d20(Fighter *fighter) {
  retail_script_004e5d20(fighter);
}

void jarek_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void jarek_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void jarek_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void jarek_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void jarek_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void jarek_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void jarek_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void jarek_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void jarek_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void jarek_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void jarek_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void jarek_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void jarek_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void jarek_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void jarek_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void jarek_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void jarek_entry_retail_script_004e81a8(Fighter *fighter) {
  retail_script_004e81a8(fighter);
}

void jarek_entry_retail_script_004ed118(Fighter *fighter) {
  retail_script_004ed118(fighter);
}

const mmove_t *jarek_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_phase;
}

void jarek_entry_retail_script_004e0128(Fighter *fighter) {
  retail_script_004e0128(fighter);
}

const mmove_t *jarek_retail_script_004e0128_repeat_phase(void) {
  return &retail_script_004e0128_phase;
}

void jarek_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void jarek_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void jarek_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void jarek_entry_retail_script_004e6270(Fighter *fighter) {
  retail_script_004e6270(fighter);
}

void jarek_entry_retail_script_004ec4f0(Fighter *fighter) {
  retail_script_004ec4f0(fighter);
}

void jarek_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void jarek_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void jarek_entry_retail_script_004e8c08(Fighter *fighter) {
  retail_script_004e8c08(fighter);
}

void jarek_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_script_004e84b8(fighter);
}

void jarek_entry_retail_script_004e9658(Fighter *fighter) {
  retail_script_004e9658(fighter);
}

void jarek_entry_retail_script_004e81c8(Fighter *fighter) {
  retail_script_004e81c8(fighter);
}

void jarek_entry_retail_script_004e95c8(Fighter *fighter) {
  retail_script_004e95c8(fighter);
}

void jarek_entry_retail_script_004d2db0(Fighter *fighter) {
  retail_script_004d2db0(fighter);
}

void jarek_entry_retail_script_004d2de0(Fighter *fighter) {
  retail_script_004d2de0(fighter);
}

void jarek_entry_retail_script_004d2e10(Fighter *fighter) {
  retail_script_004d2e10(fighter);
}

void jarek_entry_retail_script_004e1220(Fighter *fighter) {
  retail_script_004e1220(fighter);
}

void jarek_entry_retail_script_004e1230(Fighter *fighter) {
  retail_script_004e1230(fighter);
}

void jarek_entry_retail_script_004e1248(Fighter *fighter) {
  retail_script_004e1248(fighter);
}

void jarek_entry_retail_script_004e1260(Fighter *fighter) {
  retail_script_004e1260(fighter);
}

void jarek_entry_retail_script_004e1270(Fighter *fighter) {
  retail_script_004e1270(fighter);
}

void jarek_entry_retail_script_004e1280(Fighter *fighter) {
  retail_script_004e1280(fighter);
}

void jarek_entry_retail_script_004e1290(Fighter *fighter) {
  retail_script_004e1290(fighter);
}
