#include "retail_vm.h"
#include "character.h"

/* Combat values remain the native approximation until measured against retail. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {111, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

const MoveDef cage_s_cpu_kick_script = {.clip = 308, .last = 24, .flags = SF_LOCK | SF_RECORD};
const MoveDef cage_s_powered_punch_script = {.clip = 85, .last = 54, .flags = SF_LOCK | SF_RECORD};

/* Contact records from Cage's 53d2b8/53d268 tables, selected by the
 * generated controller callbacks479b70/479f00. */

// clang-format off: Keep table entries on one row, including long rows.
const uint32_t cage_weapon_descriptor[] = {
  0x6e,
  0x1430cb,
  0x144359,
  0x13b318,
  0x13eeb9,
  0x13ee9e,
  0x13eea1,
  0xffffb334,
};
// clang-format on

static const ButtonTable cage_straight_air = {.press = {[BUTTON_HIGH_PUNCH] = cage_press_air_hp,
                                                  [BUTTON_LOW_PUNCH] = cage_press_air_lp,
                                                  [BUTTON_HIGH_KICK] = cage_press_air_hk,
                                                  [BUTTON_LOW_KICK] = cage_press_air_lk}};

static const Controls cage_controls = {NULL, 0,
    {[STANCE_STANDING] = &retail_ground_buttons[0],
        [STANCE_CROUCHING] = &retail_ground_buttons[1],
        [STANCE_JUMP] = &cage_straight_air,
        [STANCE_DIAGONAL_AIR] = &cage_diagonal_air}};

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

const CharacterDef mk4_cage = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &cage_controls,
    .enter = state_entries,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = NULL,
    .move_list_count = 0,
};

void cage_entry_retail_script_004e12a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12a0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12b0, fighter_ready, 14);
}

void cage_entry_retail_script_004e5870(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5870, fighter_ready, 14);
}

void cage_entry_retail_script_004e5ba8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ba8, fighter_ready, 14);
}

void cage_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 14);
}

void cage_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 14);
}

void cage_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 14);
}

void cage_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 14);
}

void cage_entry_retail_script_004e80a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e80a8, fighter_ready, 14);
}

void cage_entry_retail_script_004e80d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e80d0, fighter_ready, 14);
}

void cage_entry_retail_script_004e9f78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9f78, mk4_script_thrown_ready, 14);
}

void cage_entry_retail_script_004e6270(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6270, fighter_ready, 14);
}

void cage_entry_retail_script_004ed0d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed0d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 14);
}

void cage_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 14);
}

void cage_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 14);
}

void cage_entry_retail_script_004ec4f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec4f0, fighter_ready, 14);
}

void cage_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7f0, fighter_ready, 14);
}

void cage_entry_retail_script_004e8cc0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8cc0, mk4_mmove_thrown_finish, 14);
}

void cage_entry_retail_script_004e8d38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8d38, mk4_mmove_thrown_finish, 14);
}

void cage_entry_retail_script_004e5880(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5880, fighter_ready, 14);
}

void cage_entry_retail_script_004e58d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e58d8, fighter_ready, 14);
}

void cage_entry_retail_script_004e5900(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5900, fighter_stop_mmove, 14);
}

void cage_entry_retail_script_004e5b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b38, fighter_ready, 14);
}

void cage_entry_retail_script_004e12c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12c0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12d0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12e0, fighter_ready, 14);
}

void cage_entry_retail_script_004e12f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e12f0, fighter_ready, 14);
}

void cage_entry_retail_script_004e1300(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1300, fighter_ready, 14);
}

void cage_entry_retail_script_004e1310(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1310, fighter_ready, 14);
}

void cage_entry_retail_script_004e1320(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1320, fighter_ready, 14);
}

void cage_entry_retail_script_004e1330(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1330, fighter_ready, 14);
}

void cage_entry_retail_script_004e1340(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1340, fighter_ready, 14);
}

void cage_entry_retail_script_004e1350(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1350, fighter_ready, 14);
}

void cage_entry_retail_script_004e1360(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1360, fighter_ready, 14);
}

void cage_entry_retail_script_004e1370(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1370, fighter_ready, 14);
}

void cage_entry_retail_script_004e1380(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e1380, fighter_ready, 14);
}

void cage_entry_retail_script_004e9718(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9718, fighter_ready, 14);
}

void cage_entry_retail_script_004ea040(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea040, fighter_ready, 14);
}

void cage_entry_retail_script_004e84b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84b8, fighter_ready, 14);
}
