#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef reptile_s_lp_thrown = {.clip = 153, .last = 26, .flags = SF_LOCK | SF_REACTION};

static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {121, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Reptile's retail inventory descriptor at 0x4ece78, preserved as encoded words. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t reptile_weapon_descriptor[] = {
  0x67,
  0x141b64,
  0x142dc1,
  0x13b35a,
  0x13eeb9,
  0x13ee9e,
  0x13eea1,
  0xffff999a,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,

  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
  [S_THROW] = retail_lp_throw,
};
// clang-format on

const CharacterDef mk4_reptile = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &retail_stances,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void reptile_entry_retail_script_004e8128(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8128, fighter_ready, 10);
}

void reptile_entry_retail_script_004e8178(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8178, fighter_ready, 10);
}

void reptile_entry_retail_script_004e9620(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9620, fighter_ready, 10);
}

void reptile_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, fighter_ready, 10);
}

void reptile_entry_retail_script_004ed1e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed1e8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 10);
}

void reptile_entry_retail_script_004e97f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e97f8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e5bb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5bb0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e95f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e95f0, fighter_ready, 10);
}

void reptile_entry_retail_script_004dff08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dff08, fighter_ready, 10);
}

void reptile_entry_retail_script_004ea290(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea290, fighter_ready, 10);
}

void reptile_entry_retail_script_004ea2a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea2a8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e60(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e60, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e70(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e70, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2e98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2e98, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ea8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ea8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2eb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2eb0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ec0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ec0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e2ed0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e2ed0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 10);
}

void reptile_entry_retail_script_004ec640(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec640, fighter_ready, 10);
}

void reptile_entry_retail_script_004e8bc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 10);
}

void reptile_entry_retail_script_004e8c98(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8c98, mk4_mmove_thrown_finish, 10);
}

void reptile_entry_retail_script_004e8cc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 10);
}
