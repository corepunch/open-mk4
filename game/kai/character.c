#include "retail_vm.h"
#include "character.h"

/* Kai's move clips and contact timing come from the slot-11 retail tables.
 * Script bodies are emitted from game/kai/port.json. */
const MoveDef kai_s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef kai_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef kai_s_high_kick = {71, 0, MOVE_END, 10, 5, 18, 12, 28, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef kai_s_low_kick = {70, 0, MOVE_END, 9, 5, 18, 12, 28, 1.55f, 0.27f, SF_ATTACK | SF_LOCK | SF_LOW, NULL, 0};
const MoveDef kai_s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 44, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef kai_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 20, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef kai_s_throw = {37, 0, MOVE_END, 6, 2, 25, 14, 66, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

const MoveDef kai_s_air_attack = {61, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef kai_s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {122, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

static const ButtonTable kai_standing = {{[BUTTON_HIGH_PUNCH] = kai_press_high_punch,
    [BUTTON_LOW_PUNCH] = kai_press_low_punch,
    [BUTTON_HIGH_KICK] = kai_press_high_kick,
    [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const ButtonTable kai_crouching = {{[BUTTON_HIGH_PUNCH] = kai_press_uppercut,
    [BUTTON_LOW_PUNCH] = kai_press_uppercut,
    [BUTTON_HIGH_KICK] = kai_press_sweep,
    [BUTTON_LOW_KICK] = kai_press_sweep}};
static const ButtonTable kai_jumping = {{[BUTTON_HIGH_PUNCH] = kai_press_air_hp,
    [BUTTON_LOW_PUNCH] = kai_press_air_lp,
    [BUTTON_HIGH_KICK] = kai_press_air_hk,
    [BUTTON_LOW_KICK] = kai_press_air_lk}};

#include "generated/controls.inc"

static const Controls kai_controls = {
    NULL, 0, {[STANCE_STANDING] = &kai_standing, [STANCE_CROUCHING] = &kai_crouching, [STANCE_JUMP] = &kai_jumping, [STANCE_DIAGONAL_AIR] = &kai_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = kai_idle,
  [S_WALK] = kai_walk,
  [S_CROUCH] = kai_crouch,
  [S_JUMP] = NULL,
  [S_PUNCH] = kai_jab,
  [S_KICK] = kai_high_kick,
  [S_SWEEP] = kai_sweep,
  [S_UPPERCUT] = kai_uppercut,
  [S_THROW] = kai_lp_throw,
  [S_BLOCK] = kai_block,
  [S_HIT] = NULL,
  [S_KNOCKDOWN] = NULL,
  [S_VICTORY] = kai_ready,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &kai_s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &kai_s_jab,
  [S_KICK] = &kai_s_high_kick,
  [S_SWEEP] = &kai_s_sweep,
  [S_UPPERCUT] = &kai_s_uppercut,
  [S_THROW] = &kai_s_throw,
  [S_BLOCK] = &kai_s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

/* Encoded Kai weapon descriptor 0x4ecef8; attack moves remain routed through
 * the selected weapon's native inventory entry until their strike records
 * are ported. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t kai_weapon_descriptor[] = {
  0x63,
  0x141b33,
  0x142daf,
  0x13b330,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffc000,
};
// clang-format on

const CharacterDef mk4_kai = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &kai_controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = NULL,
    .move_list_count = 0,
};

void kai_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 11);
}

void kai_entry_retail_script_004de358(Fighter *fighter) {
  /* 4de358 is speed data consumed by490ab0, not a VM program. */
  mk4_mmove_lk_owner_return(fighter, kai_script_lk_finish);
}

void kai_entry_retail_script_004de610(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de610, fighter_ready, 11);
}

void kai_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6b8, fighter_ready, 11);
}

void kai_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 11);
}

void kai_entry_retail_script_004e6998(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6998, fighter_ready, 11);
}

void kai_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ca0, fighter_ready, 11);
}

void kai_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5d90, fighter_ready, 11);
}

void kai_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 11);
}

void kai_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, fighter_ready, 11);
}

void kai_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 11);
}

void kai_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62f0, fighter_ready, 11);
}

void kai_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 11);
}

void kai_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 11);
}

void kai_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 11);
}

void kai_entry_retail_script_004e6798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6798, fighter_ready, 11);
}

void kai_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, fighter_ready, 11);
}

void kai_entry_retail_script_004e6890(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6890, fighter_ready, 11);
}

void kai_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e68a8, fighter_ready, 11);
}

void kai_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 11);
}

void kai_entry_retail_script_004e8210(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8210, fighter_ready, 11);
}

void kai_entry_retail_script_004e8320(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8320, fighter_ready, 11);
}

void kai_entry_retail_script_004e8358(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8358, fighter_ready, 11);
}

void kai_entry_retail_script_004e8390(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8390, fighter_ready, 11);
}

void kai_entry_retail_script_004e8870(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8870, fighter_ready, 11);
}

void kai_entry_retail_script_004e88c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e88c8, fighter_ready, 11);
}

void kai_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 11);
}

void kai_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 11);
}

void kai_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 11);
}

void kai_entry_retail_script_004e8338(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8338, fighter_ready, 11);
}

void kai_entry_retail_script_004e83a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83a0, fighter_ready, 11);
}

void kai_entry_retail_script_004e83d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e83e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e83e8, fighter_ready, 11);
}

void kai_entry_retail_script_004e9308(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9308, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e9360(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9360, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ea170(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea170, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ea1c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea1c0, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 11);
}

void kai_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 11);
}

void kai_entry_retail_script_004ec5a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec5a8, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8b78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8b78, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8bc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8bc0, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004e8f50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8f50, mk4_mmove_thrown_finish, 11);
}

void kai_entry_retail_script_004ed330(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed330, fighter_ready, 11);
}

void kai_entry_retail_script_004ed348(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed348, fighter_ready, 11);
}

void kai_entry_retail_script_004ed2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2f8, fighter_ready, 11);
}

void kai_entry_retail_script_004e1430(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1430, fighter_ready, 11);
}

void kai_entry_retail_script_004e1440(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1440, fighter_ready, 11);
}

void kai_entry_retail_script_004e1450(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1450, fighter_ready, 11);
}

void kai_entry_retail_script_004e1468(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1468, fighter_ready, 11);
}

void kai_entry_retail_script_004e1480(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1480, fighter_ready, 11);
}

void kai_entry_retail_script_004e1490(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1490, fighter_ready, 11);
}

void kai_entry_retail_script_004e14a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14a8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14b8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14c8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14d8, fighter_ready, 11);
}

void kai_entry_retail_script_004e14e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e14e8, fighter_ready, 11);
}

void kai_entry_retail_script_004dff88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dff88, fighter_ready, 11);
}

void kai_entry_retail_script_004ea240(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea240, fighter_ready, 11);
}

void kai_entry_retail_script_004e5be0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5be0, fighter_ready, 11);
}
