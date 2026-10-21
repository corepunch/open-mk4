#include "retail_vm.h"
#include "character.h"

/* Encoded Goro roster descriptor retained from the EXE inventory at 0x4ecf18. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t goro_retail_weapon_descriptor[] = {
  0x6c,
  0x141bf0,
  0x142dd3,
  0x13b330,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0x0,
};
// clang-format on

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {603, 18, 0, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {602, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

static const MoveDef s_block = {595, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {593, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef goro_s_knockdown = {581, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {594, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,

  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &goro_s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

/* Goro's roster slot selects the shared stance tables 4e5058/4e5070/4e6ec8
 * (typed inventory), so stance presses enter the same native selectors as
 * the humanoid roster with Goro's own script slots and clip choices. */
static const ButtonTable goro_standing = {{[BUTTON_HIGH_PUNCH] = goro_press_high_punch,
    [BUTTON_LOW_PUNCH] = goro_press_low_punch,
    [BUTTON_HIGH_KICK] = goro_press_high_kick,
    [BUTTON_LOW_KICK] = goro_press_low_kick}};
static const ButtonTable goro_crouching = {{[BUTTON_HIGH_PUNCH] = goro_press_crouching_punch,
    [BUTTON_LOW_PUNCH] = goro_press_crouching_punch,
    [BUTTON_HIGH_KICK] = goro_press_crouching_kick,
    [BUTTON_LOW_KICK] = goro_press_crouching_kick}};
static const ButtonTable goro_jumping = {{[BUTTON_HIGH_PUNCH] = goro_press_air_hp,
    [BUTTON_LOW_PUNCH] = goro_press_air_lp,
    [BUTTON_BLOCK] = goro_press_air_block,
    [BUTTON_HIGH_KICK] = goro_press_air_hk,
    [BUTTON_LOW_KICK] = goro_press_air_lk}};

static const Controls goro_controls = {NULL, 0, {[STANCE_STANDING] = &goro_standing, [STANCE_CROUCHING] = &goro_crouching, [STANCE_JUMP] = &goro_jumping}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_THROW] = retail_lp_throw,
};
// clang-format on

const CharacterDef mk4_goro = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &goro_controls,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = NULL,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void goro_entry_retail_script_004de6e4(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6e4, fighter_ready, 15);
}

void goro_entry_retail_script_004de7e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de7e8, fighter_ready, 15);
}

void goro_entry_retail_script_004dfb60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfb60, fighter_ready, 15);
}

void goro_entry_retail_script_004dfb70(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfb70, fighter_ready, 15);
}

void goro_entry_retail_script_004dfbb8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfbb8, fighter_ready, 15);
}

void goro_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e28, fighter_ready, 15);
}

void goro_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e50, fighter_ready, 15);
}

void goro_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 15);
}

void goro_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 15);
}

void goro_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 15);
}

void goro_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 15);
}

void goro_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 15);
}

void goro_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 15);
}

void goro_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 15);
}

void goro_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 15);
}

void goro_entry_retail_script_004e9910(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9910, fighter_ready, 15);
}

void goro_entry_retail_script_004ed790(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed790, fighter_ready, 15);
}

void goro_entry_retail_script_004dfbd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfbd0, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc38, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc50, fighter_ready, 15);
}

void goro_entry_retail_script_004dfc60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfc60, fighter_ready, 15);
}

void goro_entry_retail_script_004e8dd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8dd0, fighter_ready, 15);
}

void goro_entry_retail_script_004e8e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e50, fighter_ready, 15);
}

void goro_entry_retail_script_004e8e80(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e80, fighter_ready, 15);
}

void goro_entry_retail_script_004e95c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e95c8, fighter_ready, 15);
}

void goro_entry_retail_script_004ea3e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea3e0, fighter_ready, 15);
}

void goro_entry_retail_script_004ea400(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea400, fighter_ready, 15);
}
