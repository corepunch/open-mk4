#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebaf8, B selector 2 */
/* 0x4ebb70, B selector 36 */

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef raiden_s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
const MoveDef raiden_s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
const MoveDef raiden_s_sweep = {.clip = 62, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef raiden_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */

const MoveDef raiden_s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef raiden_s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
const MoveDef raiden_s_victory = {123, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &raiden_s_jab,
  [S_KICK] = &raiden_s_kick,
  [S_SWEEP] = &raiden_s_sweep,
  [S_UPPERCUT] = &raiden_s_uppercut,
  [S_THROW] = &raiden_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &raiden_s_knockdown,
  [S_VICTORY] = &raiden_s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_THROW] = raiden_enter_throw,
  [S_VICTORY] = raiden_enter_victory,
  [S_IDLE] = raiden_armed_idle,
  [S_WALK] = raiden_armed_walk,
  [S_CROUCH] = raiden_armed_crouch,
  [S_BLOCK] = raiden_armed_block,
  [S_PUNCH] = raiden_jab,
  [S_KICK] = raiden_kick,
  [S_SWEEP] = raiden_sweep,
  [S_UPPERCUT] = raiden_uppercut,
};
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

const MoveDef raiden_s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent frame programs after release; replacement cancels only a live pair. */

/* 49c300 spawns independently of the owner's generated recovery. */

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = raiden_press_jab,
    [BUTTON_LOW_PUNCH] = raiden_press_lp,
    [BUTTON_HIGH_KICK] = raiden_press_kick,
    [BUTTON_LOW_KICK] = raiden_press_lk}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = raiden_press_uppercut,
    [BUTTON_LOW_PUNCH] = raiden_press_crouch_lp,
    [BUTTON_HIGH_KICK] = raiden_press_crouch_hk,
    [BUTTON_LOW_KICK] = raiden_press_crouch_lk}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = raiden_press_air_hp,
    [BUTTON_LOW_PUNCH] = raiden_press_air_lp,
    [BUTTON_HIGH_KICK] = raiden_press_air_hk,
    [BUTTON_LOW_KICK] = raiden_press_air_lk}};
static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = raiden_jab,
            [CPU_LOW_KICK] = raiden_lk,
            [CPU_HIGH_KICK] = raiden_hk,
            [CPU_SWEEP] = raiden_enter_sweep,
            [CPU_UPPERCUT] = raiden_enter_uppercut,
            [CPU_THROW] = raiden_enter_throw,
            [CPU_CLOSE_HP] = raiden_cpu_close_hp,
            [CPU_CLOSE_HK] = raiden_cpu_close_hk,
            [CPU_CROUCH_LP] = raiden_crouch_lp,
            [CPU_AIR_HP] = raiden_air_hp,
            [CPU_AIR_HK] = raiden_cpu_air_hk,
            [CPU_AIR_LK] = raiden_cpu_air_lk,
            [CPU_DRAW] = raiden_weapon,
            [CPU_FINISH_1] = raiden_first_fatality,
            [CPU_FINISH_2] = raiden_second_fatality,
            [CPU_FINISH_FAN] = retail_fan_finish,
            [CPU_FINISH_SPIKE] = raiden_spike_fatality,
        },
    .finish_min = {0, 0},
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "LIGHTNING", "D,B+LP"},
  {MOVE_LIST_SPECIAL, "TORPEDO", "F,F+LK (AIR OK)"},
  {MOVE_LIST_SPECIAL, "TELEPORT", "D,U"},
  {MOVE_LIST_SPECIAL, "WEAPON", "F,B+HP"},
  {MOVE_LIST_FINISHER, "FATALITY 1", "F,B,U,U+HK"},
  {MOVE_LIST_FINISHER, "FATALITY 2", "D,U,U,U+HP"},
  {MOVE_LIST_FINISHER, "FAN STAGE", "D,F,B+BL"},
  {MOVE_LIST_FINISHER, "SPIKE STAGE", "F,F,D+LP"},
};
// clang-format on

const CharacterDef mk4_raiden = {
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

void raiden_entry_retail_script_004de218(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de218, raiden_ready, 1);
}

void raiden_entry_retail_script_004de640(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de640, raiden_ready, 1);
}

void raiden_entry_retail_script_004de668(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de668, raiden_ready, 1);
}

void raiden_entry_retail_script_004de6b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de6b8, raiden_ready, 1);
}

void raiden_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, raiden_ready, 1);
}

void raiden_entry_retail_script_004e0210(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0210, raiden_ready, 1);
}

void raiden_entry_retail_script_004e0220(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0220, raiden_ready, 1);
}

void raiden_entry_retail_script_004e56c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56c8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e56d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56d8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e56f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56f0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5740(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5740, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5760(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5760, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5770(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5770, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5788(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5788, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5ca0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5d90, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e28, raiden_ready, 1);
}

void raiden_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e50, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6020(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6020, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6158(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6158, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, raiden_ready, 1);
}

void raiden_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62f0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, raiden_ready, 1);
}

void raiden_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6798, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6800(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6800, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6890(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6890, raiden_ready, 1);
}

void raiden_entry_retail_script_004e68a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e68a8, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6998(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6998, raiden_ready, 1);
}

void raiden_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, raiden_ready, 1);
}

void raiden_entry_retail_script_004e8440(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8440, raiden_ready, 1);
}

void raiden_entry_retail_script_004e8458(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8458, raiden_ready, 1);
}

void raiden_entry_retail_script_004e8e18(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e18, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e9910(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9910, raiden_ready, 1);
}

void raiden_entry_retail_script_004e99c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e99c0, raiden_ready, 1);
}

void raiden_entry_retail_script_004e9ad0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ad0, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e9ae8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ae8, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e9b40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b40, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e9d50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9d50, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004ea068(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea068, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004ea108(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea108, raiden_ready, 1);
}

void raiden_entry_retail_script_004ed5f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed5f0, raiden_ready, 1);
}
