#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef reptile_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef reptile_s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef reptile_s_sweep = {63, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef reptile_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef reptile_s_throw = {76, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

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

static const ButtonTable reptile_standing = {{[BUTTON_HIGH_PUNCH] = reptile_press_punch,
    [BUTTON_LOW_PUNCH] = reptile_press_low_punch,
    [BUTTON_HIGH_KICK] = reptile_press_kick,
    [BUTTON_LOW_KICK] = reptile_press_low_kick}};
static const ButtonTable reptile_crouching = {{[BUTTON_HIGH_PUNCH] = reptile_press_uppercut,
    [BUTTON_LOW_PUNCH] = reptile_press_uppercut,
    [BUTTON_HIGH_KICK] = reptile_press_sweep,
    [BUTTON_LOW_KICK] = reptile_press_sweep}};
static const ButtonTable reptile_jumping = {{[BUTTON_HIGH_PUNCH] = reptile_press_air_hp,
    [BUTTON_LOW_PUNCH] = reptile_press_air_lp,
    [BUTTON_HIGH_KICK] = reptile_press_air_hk,
    [BUTTON_LOW_KICK] = reptile_press_air_lk}};

#include "generated/controls.inc"

static const Controls reptile_controls = {NULL, 0,
    {[STANCE_STANDING] = &reptile_standing,
        [STANCE_CROUCHING] = &reptile_crouching,
        [STANCE_JUMP] = &reptile_jumping,
        [STANCE_DIAGONAL_AIR] = &reptile_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &reptile_s_jab,
  [S_KICK] = &reptile_s_kick,
  [S_SWEEP] = &reptile_s_sweep,
  [S_UPPERCUT] = &reptile_s_uppercut,
  [S_THROW] = &reptile_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = reptile_armed_idle,
  [S_WALK] = reptile_armed_walk,
  [S_CROUCH] = reptile_armed_crouch,
  [S_BLOCK] = reptile_armed_block,
  [S_THROW] = reptile_lp_throw,
};
// clang-format on

const CharacterDef mk4_reptile = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &reptile_controls,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = NULL,
    .move_list_count = 0,
};

void reptile_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 10);
}

void reptile_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6b8, fighter_ready, 10);
}

void reptile_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 10);
}

void reptile_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ca0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5d90, fighter_ready, 10);
}

void reptile_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62f0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6798, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6860(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6860, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6878(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6878, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 10);
}

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

void reptile_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 10);
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

void reptile_entry_retail_script_004e5c70(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5c70, fighter_ready, 10);
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

void reptile_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 10);
}

void reptile_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 10);
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
