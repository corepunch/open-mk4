#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
const MoveDef noob_saibot_s_jab = {294, 0, MOVE_END, 5, 4, 12, 7, 19, 1.25f, 0.13f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef noob_saibot_s_kick = {66, 0, MOVE_END, 10, 5, 18, 12, 25, 1.85f, 0.32f, SF_ATTACK | SF_LOCK, NULL, 0};
const MoveDef noob_saibot_s_sweep = {62, 0, MOVE_END, 11, 5, 23, 10, 45, 1.65f, 0.25f, SF_ATTACK | SF_LOCK | SF_LOW | SF_LAUNCH, NULL, 0};
const MoveDef noob_saibot_s_uppercut = {293, 0, MOVE_END, 9, 5, 30, 18, 60, 1.35f, 0.5f, SF_ATTACK | SF_LOCK | SF_LAUNCH, NULL, 0};
const MoveDef noob_saibot_s_throw = {78, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_RECORD | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};

static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {113, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

/* Retail inventory descriptor 4eced8, kept as encoded evidence until the
 * weapon's native attack callbacks have a reviewed portable implementation. */
// clang-format off: Keep table entries on one row, including long rows.
const uint32_t noob_saibot_weapon_descriptor[] = {
  0x69,
  0x141b95,
  0x142dca,
  0x13b33c,
  0x13eeb6,
  0x13ee98,
  0x13ee9b,
  0xffffb334,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &noob_saibot_s_jab,
  [S_KICK] = &noob_saibot_s_kick,
  [S_SWEEP] = &noob_saibot_s_sweep,
  [S_UPPERCUT] = &noob_saibot_s_uppercut,
  [S_THROW] = &noob_saibot_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static const ButtonTable noob_saibot_standing = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_high_punch,
    [BUTTON_LOW_PUNCH] = noob_saibot_press_low_punch,
    [BUTTON_HIGH_KICK] = noob_saibot_press_high_kick,
    [BUTTON_LOW_KICK] = noob_saibot_press_low_kick}};
static const ButtonTable noob_saibot_crouching = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_crouching_hp,
    [BUTTON_LOW_PUNCH] = noob_saibot_press_crouching_hp,
    [BUTTON_HIGH_KICK] = noob_saibot_press_crouching_kick,
    [BUTTON_LOW_KICK] = noob_saibot_press_crouching_kick}};
static const ButtonTable noob_saibot_air = {{[BUTTON_HIGH_PUNCH] = noob_saibot_press_air_hp,
    [BUTTON_LOW_PUNCH] = noob_saibot_press_air_lp,
    [BUTTON_HIGH_KICK] = noob_saibot_press_air_hk,
    [BUTTON_LOW_KICK] = noob_saibot_press_air_lk}};

#include "generated/controls.inc"

static const Controls noob_saibot_controls = {NULL, 0,
    {[STANCE_STANDING] = &noob_saibot_standing,
        [STANCE_CROUCHING] = &noob_saibot_crouching,
        [STANCE_JUMP] = &noob_saibot_air,
        [STANCE_DIAGONAL_AIR] = &noob_saibot_diagonal_air}};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "SHADOW CHARGE", "D,F+LP"},
  {MOVE_LIST_SPECIAL, "AIR SHADOW", "D,F+LP (AIR)"},
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn state_entries[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,

  [S_THROW] = noob_saibot_lp_throw,
};
// clang-format on

const CharacterDef mk4_noob_saibot = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &noob_saibot_controls,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &retail_cpu_moves,
    .move_list = move_list,
    .move_list_count = (int)(sizeof move_list / sizeof move_list[0]),
};

void noob_saibot_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004de640(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de640, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004de664(Fighter *fighter) {
  /* 4de664 is a490350 callback table, not a VM body. */
  fighter->facing = -fighter->facing;
}

void noob_saibot_entry_retail_script_004de668(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de668, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6b8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e5e78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e78, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e5ea0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ea0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e9880(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9880, mk4_script_thrown_ready, 17);
}

void noob_saibot_entry_retail_script_004e9b40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b40, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004ed268(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed268, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004ed2c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2c0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, mk4_script_thrown_ready, 17);
}

void noob_saibot_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 17);
}

void noob_saibot_entry_retail_script_004e82b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e82b8, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e82e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e82e0, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e8310(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8310, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004e8818(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8818, fighter_ready, 17);
}

void noob_saibot_entry_retail_script_004ed038(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed038, fighter_ready, 17);
}
