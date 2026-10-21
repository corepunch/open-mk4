#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebac8, B selector 0 */
/* 0x4ebb58, B selector 35 */

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef kitana_s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
const MoveDef kitana_s_kick = {.clip = 65, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
const MoveDef kitana_s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef kitana_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */

const MoveDef kitana_s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef kitana_s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
const MoveDef kitana_s_victory = {114, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &kitana_s_jab,
  [S_KICK] = &kitana_s_kick,
  [S_SWEEP] = &kitana_s_sweep,
  [S_UPPERCUT] = &kitana_s_uppercut,
  [S_THROW] = &kitana_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &kitana_s_knockdown,
  [S_VICTORY] = &kitana_s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_THROW] = kitana_enter_throw,
  [S_VICTORY] = kitana_enter_victory,
  [S_IDLE] = kitana_armed_idle,
  [S_WALK] = kitana_armed_walk,
  [S_CROUCH] = kitana_armed_crouch,
  [S_BLOCK] = kitana_armed_block,
  [S_PUNCH] = kitana_jab,
  [S_KICK] = kitana_kick,
  [S_SWEEP] = kitana_sweep,
  [S_UPPERCUT] = kitana_uppercut,
};
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

const MoveDef kitana_s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent frame programs after release; replacement cancels only a live pair. */

/* 49c300 spawns independently of the owner's generated recovery. */

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = kitana_press_jab,
    [BUTTON_LOW_PUNCH] = kitana_press_lp,
    [BUTTON_HIGH_KICK] = kitana_press_kick,
    [BUTTON_LOW_KICK] = kitana_press_lk}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = kitana_press_uppercut,
    [BUTTON_LOW_PUNCH] = kitana_press_crouch_lp,
    [BUTTON_HIGH_KICK] = kitana_press_crouch_hk,
    [BUTTON_LOW_KICK] = kitana_press_crouch_lk}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = kitana_press_air_hp,
    [BUTTON_LOW_PUNCH] = kitana_press_air_lp,
    [BUTTON_HIGH_KICK] = kitana_press_air_hk,
    [BUTTON_LOW_KICK] = kitana_press_air_lk}};
static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = kitana_jab,
            [CPU_LOW_KICK] = kitana_lk,
            [CPU_HIGH_KICK] = kitana_hk,
            [CPU_SWEEP] = kitana_enter_sweep,
            [CPU_UPPERCUT] = kitana_enter_uppercut,
            [CPU_THROW] = kitana_enter_throw,
            [CPU_CLOSE_HP] = kitana_cpu_close_hp,
            [CPU_CLOSE_HK] = kitana_cpu_close_hk,
            [CPU_CROUCH_LP] = kitana_crouch_lp,
            [CPU_AIR_HP] = kitana_air_hp,
            [CPU_AIR_HK] = kitana_cpu_air_hk,
            [CPU_AIR_LK] = kitana_cpu_air_lk,
            [CPU_DRAW] = kitana_weapon,
            [CPU_FINISH_1] = kitana_first_fatality,
            [CPU_FINISH_2] = kitana_second_fatality,
            [CPU_FINISH_FAN] = retail_fan_finish,
            [CPU_FINISH_SPIKE] = kitana_spike_fatality,
        },
    .finish_min = {0x10000, 0x1cccc},
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "RING TOSS", "D,F+LP"},
  {MOVE_LIST_SPECIAL, "AIR SHADOW", "D,F+LP (AIR)"},
  {MOVE_LIST_SPECIAL, "BICYCLE KICK", "F,B+HP"},
  {MOVE_LIST_SPECIAL, "SQUARE WAVE", "B,B,D+HK"},
  {MOVE_LIST_SPECIAL, "CARTWHEEL", "B,D,F+LK"},
  {MOVE_LIST_SPECIAL, "LEG GRAB", "D+LP+BL"},
  {MOVE_LIST_SPECIAL, "WEAPON", "F,F+LK"},
  {MOVE_LIST_FINISHER, "FATALITY 1", "D,D,D,U+RUN"},
  {MOVE_LIST_FINISHER, "FATALITY 2", "U,D,D,U+HK"},
  {MOVE_LIST_FINISHER, "FAN STAGE", "D,B,B+HK"},
  {MOVE_LIST_FINISHER, "SPIKE STAGE", "F,D,F+HP"},
};
// clang-format on

const CharacterDef mk4_kitana = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = &cpu_moves,
    .move_list = move_list,
    .move_list_count = COUNT(move_list),
};

void kitana_entry_retail_script_004de360(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de360, kitana_ready, 16);
}

void kitana_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, kitana_ready, 16);
}

void kitana_entry_retail_script_004e01c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e01c0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e01d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e01d0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5930(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5930, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5940(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5940, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5960(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5960, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5980(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5980, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5990(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5990, kitana_ready, 16);
}

void kitana_entry_retail_script_004e59a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e59a8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e59c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e59c8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5a20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5a20, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ca0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5d20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5d20, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5d90, kitana_ready, 16);
}

void kitana_entry_retail_script_004e5ed0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ed0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6020(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6020, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6158(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6158, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, kitana_ready, 16);
}

void kitana_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62f0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, kitana_ready, 16);
}

void kitana_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6798, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6860(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6860, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6878(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6878, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, kitana_ready, 16);
}

void kitana_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, kitana_ready, 16);
}

void kitana_entry_retail_script_004e81d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81d0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e8578(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8578, kitana_ready, 16);
}

void kitana_entry_retail_script_004e85a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e85a8, kitana_ready, 16);
}

void kitana_entry_retail_script_004e85c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e85c0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e8b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8b38, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004e8f50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8f50, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004e9110(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9110, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004e98a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e98a0, kitana_ready, 16);
}

void kitana_entry_retail_script_004e9a68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9a68, kitana_ready, 16);
}

void kitana_entry_retail_script_004e9b88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b88, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004e9de8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9de8, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004ea130(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea130, mk4_script_thrown_ready, 16);
}

void kitana_entry_retail_script_004ec7a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7a0, kitana_ready, 16);
}

void kitana_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7f0, kitana_ready, 16);
}

void kitana_entry_retail_script_004ed268(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed268, kitana_ready, 16);
}

void kitana_entry_retail_script_004ed2c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed2c0, kitana_ready, 16);
}

void kitana_entry_retail_script_004ed4c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed4c0, kitana_ready, 16);
}
