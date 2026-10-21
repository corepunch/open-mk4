#include "retail_vm.h"
#include "character.h"

/* Retail attack records; source addresses identify the supplied EXE. */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebae0, B selector 1 */
/* 0x4ebb70, B selector 36 */

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */

/* Attacks query retail records from their events (SF_RECORD). Generic
 * S_HIT/S_KNOCKDOWN entries remain shared recovery fallbacks. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef liu_kang_s_throw = {.clip = 79, .last = 60, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {119, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_THROW] = &liu_kang_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
};
// clang-format on

/* Selected Liu Kang ground scripts and native tails: 4e6d08, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6e90/46f390. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* Slot 3: LP 4de678, LK 42c040/4de3b0/410/428/468/478.
 * The victim owns its released recovery, even after owner interruption/KO. */

/* LK release 4e49a8 -> 45eab0/47eaf0, scripts 4e8610/678/688.
 * Native 47eb50 holds a hit pair for 58 ticks; damage is D[6] once. */

/* Fireballs: 49bf50/90/49bfe0 -> 4ed678/6c0/6f8.
 * Projectile task 64 has a 48-tick lifetime independent of its owner. */

const MeshAnimation liu_kang_fireball_mesh = {.first = 50, .count = 1, .period = 1, .scale = Q16_ONE};

/* 47edc0/4e86a8 -> 47ee00/30; D[12], six opening visits.
 * Hit and blocked exits are 4e86e8 and 4e8718 respectively. */

/* Default weapon descriptor 53d5c0[3] -> 4ece58: type 66,
 * rig 506cb0. The four weapon entries select distinct native programs. */

// clang-format off: Keep table entries on one row, including long rows.
const WeaponStrike liu_kang_sword_attacks[] = {
  {{.clip = 240, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {106168, -54394, 77987, 80609, 59, 59, 119, 0, 10485}, 0, 8, 13, 3, 4, 0, 0, 0, 34},
  {{.clip = 237, .last = 44, .flags = RETAIL, .action = ACTION_RETAIL_010E}, {99614, -49152, 64880, 55050, 61, 61, 119, 0, 9830}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34},
  {{.clip = 242, .last = 30, .flags = RETAIL, .action = ACTION_RETAIL_0212}, {111411, -25552, 89784, 11796, 62, 62, 118, 0, 9830}, 8, 9, 11, 3, 8, 2, 18, 0x28f5, 34},
  {{.clip = 227, .last = 59, .flags = RETAIL, .action = ACTION_RETAIL_0112}, {98959, -28835, 67502, 36700, 57, 57, 119, 0, 9175}, 0, 16, 29, 2, 0, 8, 13, 0x1999, 31},
};
// clang-format on

/* First scene: 4e0498[3] -> 450930. Second: 4dfe90[3] ->
 * 43c3d0/4dfff8/4e0020, victim reactions 176/177. */

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = liu_kang_press_jab,
    [BUTTON_LOW_PUNCH] = liu_kang_press_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_kick,
    [BUTTON_LOW_KICK] = liu_kang_press_lk}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = retail_press_uppercut,
    [BUTTON_LOW_PUNCH] = liu_kang_press_crouch_lp,
    [BUTTON_HIGH_KICK] = retail_press_crouch_high_kick,
    [BUTTON_LOW_KICK] = retail_press_crouch_low_kick}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = liu_kang_press_air_hp,
    [BUTTON_LOW_PUNCH] = liu_kang_press_air_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_air_hk,
    [BUTTON_LOW_KICK] = liu_kang_press_air_lk}};
static const Controls controls = {NULL, 0, {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = retail_jab,
            [CPU_LOW_KICK] = retail_low_kick,
            [CPU_HIGH_KICK] = liu_kang_hk,
            [CPU_SWEEP] = retail_sweep_state,
            [CPU_UPPERCUT] = retail_uppercut_state,
            [CPU_THROW] = retail_lp_throw,
            [CPU_CLOSE_HP] = liu_kang_cpu_close_hp,
            [CPU_CLOSE_HK] = liu_kang_cpu_close_hk,
            [CPU_CROUCH_LP] = liu_kang_crouch_lp,
            [CPU_AIR_HP] = liu_kang_cpu_air_hp,
            [CPU_AIR_HK] = liu_kang_cpu_air_hk,
            [CPU_AIR_LK] = liu_kang_cpu_air_lk,
            [CPU_DRAW] = liu_kang_weapon,
        }, /* 4dfe10/4dfe50[3]. */
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "HIGH FIREBALL", "F,F+HP (AIR OK)"},
  {MOVE_LIST_SPECIAL, "LOW FIREBALL", "F,F+LP"},
  {MOVE_LIST_SPECIAL, "FLYING KICK", "F,F+HK"},
  {MOVE_LIST_SPECIAL, "BICYCLE KICK", "HOLD LK 2S, RELEASE"},
  {MOVE_LIST_SPECIAL, "WEAPON", "B,F+LK"},
  {MOVE_LIST_FINISHER, "FATALITY 1", "F,F,F,D+BL"},
  {MOVE_LIST_FINISHER, "FATALITY 2", "F,D,D,U+HP"},
  {MOVE_LIST_FINISHER, "FAN STAGE", "F,F,B+LP"},
  {MOVE_LIST_FINISHER, "SPIKE STAGE", "F,F,B+HK"},
};
// clang-format on

const CharacterDef mk4_liu_kang = {
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

void liu_kang_entry_retail_script_004de3b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de3b0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004de410(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de410, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004de428(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de428, liu_kang_entry_retail_script_004de468, 3);
}

void liu_kang_entry_retail_script_004de468(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de468, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004dfff8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4dfff8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e0020(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0020, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e60b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e60b8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6158(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6158, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6d08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6d08, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e6df0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6df0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e8610(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8610, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e8678(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8678, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e8688(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8688, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e86a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e86a8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e86d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e86d0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e86e8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e86e8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e8718(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8718, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e9a90(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9a90, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e9ad0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ad0, mk4_script_thrown_ready, 3);
}

void liu_kang_entry_retail_script_004e9ae8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ae8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e9b68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b68, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004ec680(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec680, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7f0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004ed678(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed678, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004ed6c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed6c0, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004ed6f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed6f8, fighter_ready, 3);
}

void liu_kang_entry_retail_script_004e9de8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9de8, mk4_mmove_thrown_finish, 3);
}
