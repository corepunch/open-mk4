#include "character.h"

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebac8, B selector 0 */
/* 0x4ebb58, B selector 35 */

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef sonya_s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
const MoveDef sonya_s_kick = {.clip = 65, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
const MoveDef sonya_s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef sonya_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* Native uppercut recovery uses contact-selected pose sequences. */

const MoveDef sonya_s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
const MoveDef sonya_s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
const MoveDef sonya_s_victory = {114, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &sonya_s_jab,
  [S_KICK] = &sonya_s_kick,
  [S_SWEEP] = &sonya_s_sweep,
  [S_UPPERCUT] = &sonya_s_uppercut,
  [S_THROW] = &sonya_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &sonya_s_knockdown,
  [S_VICTORY] = &sonya_s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_THROW] = sonya_enter_throw,
  [S_VICTORY] = sonya_enter_victory,
  [S_IDLE] = sonya_armed_idle,
  [S_WALK] = sonya_armed_walk,
  [S_CROUCH] = sonya_armed_crouch,
  [S_BLOCK] = sonya_armed_block,
  [S_PUNCH] = sonya_jab,
  [S_KICK] = sonya_kick,
  [S_SWEEP] = sonya_sweep,
  [S_UPPERCUT] = sonya_uppercut,
};
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

const MoveDef sonya_s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent frame programs after release; replacement cancels only a live pair. */

/* 49c300 spawns independently of the owner's generated recovery. */

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK}, 2, 24, 0x0, NULL, sonya_bicycle},
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_FORWARD}, 3, 40, 0x0, sonya_can_spike_fatality, sonya_spike_fatality},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 16, 0x0, NULL, sonya_ring},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0x40, sonya_can_leg_grab, sonya_leg_grab},
  {BUTTON_BLOCK, {0}, 0, 0, 0x10, mk4_can_air_throw, sonya_air_throw},
  {BUTTON_BLOCK, {0}, 0, 0, 0x40, sonya_can_leg_grab, sonya_leg_grab},
  {BUTTON_HIGH_KICK, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN}, 3, 40, 0x0, NULL, sonya_square_wave},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_BACK, TOKEN_BACK}, 3, 40, 0x0, sonya_can_fan_fatality, sonya_fan_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_UP, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x0, sonya_can_second_fatality, sonya_second_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_DOWN, TOKEN_FORWARD}, 3, 29, 0x0, sonya_can_cartwheel, sonya_cartwheel},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0x0, sonya_can_weapon, sonya_weapon},
  {BUTTON_RUN, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0x40, sonya_can_first_fatality, sonya_first_fatality},
  {BUTTON_RUN, {0}, 0, 0, 0x0, mk4_can_sidestep, sonya_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0x0, weapon_can_pickup, weapon_pickup},
};
// clang-format on

static const ButtonTable standing = {
    {[BUTTON_HIGH_PUNCH] = sonya_press_jab, [BUTTON_LOW_PUNCH] = sonya_press_lp, [BUTTON_HIGH_KICK] = sonya_press_kick, [BUTTON_LOW_KICK] = sonya_press_lk}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = sonya_press_uppercut,
    [BUTTON_LOW_PUNCH] = sonya_press_crouch_lp,
    [BUTTON_HIGH_KICK] = sonya_press_crouch_hk,
    [BUTTON_LOW_KICK] = sonya_press_crouch_lk}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = sonya_press_air_hp,
    [BUTTON_LOW_PUNCH] = sonya_press_air_lp,
    [BUTTON_HIGH_KICK] = sonya_press_air_hk,
    [BUTTON_LOW_KICK] = sonya_press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {
    .enter =
        {
            [CPU_JAB] = sonya_jab,
            [CPU_LOW_KICK] = sonya_lk,
            [CPU_HIGH_KICK] = sonya_hk,
            [CPU_SWEEP] = sonya_enter_sweep,
            [CPU_UPPERCUT] = sonya_enter_uppercut,
            [CPU_THROW] = sonya_enter_throw,
            [CPU_CLOSE_HP] = sonya_cpu_close_hp,
            [CPU_CLOSE_HK] = sonya_cpu_close_hk,
            [CPU_CROUCH_LP] = sonya_crouch_lp,
            [CPU_AIR_HP] = sonya_air_hp,
            [CPU_AIR_HK] = sonya_cpu_air_hk,
            [CPU_AIR_LK] = sonya_cpu_air_lk,
            [CPU_DRAW] = sonya_weapon,
            [CPU_FINISH_1] = sonya_first_fatality,
            [CPU_FINISH_2] = sonya_second_fatality,
            [CPU_FINISH_FAN] = sonya_fan_fatality,
            [CPU_FINISH_SPIKE] = sonya_spike_fatality,
            [CPU_PROJECTILE] = sonya_ring,
            [CPU_SPECIAL_NEAR] = sonya_leg_grab,
            [CPU_SPECIAL_1] = sonya_bicycle,
            [CPU_SPECIAL_2] = sonya_cartwheel,
        },
    .finish_min = {0x10000, 0x1cccc},
    .finish_distance = {0x30000, 0x30000},
};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "RING TOSS", "D,F+LP"},
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

const CharacterDef mk4_sonya = {
    GAME_MK4, 2, "SONYA", &mk4_motion, moves, S_COUNT, &controls, enter, -73629, mk4_walk, mk4_jump, &cpu_moves, move_list, COUNT(move_list)};

void sonya_entry_retail_script_004de360(Fighter *fighter) {
  retail_script_004de360(fighter);
}

void sonya_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void sonya_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void sonya_entry_retail_script_004e0138(Fighter *fighter) {
  retail_script_004e0138(fighter);
}

void sonya_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void sonya_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void sonya_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void sonya_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void sonya_entry_retail_script_004e01c0(Fighter *fighter) {
  retail_script_004e01c0(fighter);
}

void sonya_entry_retail_script_004e01d0(Fighter *fighter) {
  retail_script_004e01d0(fighter);
}

void sonya_entry_retail_script_004e5930(Fighter *fighter) {
  retail_script_004e5930(fighter);
}

void sonya_entry_retail_script_004e5940(Fighter *fighter) {
  retail_script_004e5940(fighter);
}

void sonya_entry_retail_script_004e5960(Fighter *fighter) {
  retail_script_004e5960(fighter);
}

void sonya_entry_retail_script_004e5980(Fighter *fighter) {
  retail_script_004e5980(fighter);
}

void sonya_entry_retail_script_004e5990(Fighter *fighter) {
  retail_script_004e5990(fighter);
}

void sonya_entry_retail_script_004e59a8(Fighter *fighter) {
  retail_script_004e59a8(fighter);
}

void sonya_entry_retail_script_004e59c8(Fighter *fighter) {
  retail_script_004e59c8(fighter);
}

void sonya_entry_retail_script_004e5a20(Fighter *fighter) {
  retail_script_004e5a20(fighter);
}

void sonya_entry_retail_script_004e5ca0(Fighter *fighter) {
  retail_script_004e5ca0(fighter);
}

void sonya_entry_retail_script_004e5d20(Fighter *fighter) {
  retail_script_004e5d20(fighter);
}

void sonya_entry_retail_script_004e5d90(Fighter *fighter) {
  retail_script_004e5d90(fighter);
}

void sonya_entry_retail_script_004e5ed0(Fighter *fighter) {
  retail_script_004e5ed0(fighter);
}

void sonya_entry_retail_script_004e6020(Fighter *fighter) {
  retail_script_004e6020(fighter);
}

void sonya_entry_retail_script_004e6158(Fighter *fighter) {
  retail_script_004e6158(fighter);
}

void sonya_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void sonya_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void sonya_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void sonya_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void sonya_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void sonya_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void sonya_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void sonya_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void sonya_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void sonya_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void sonya_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void sonya_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void sonya_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void sonya_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void sonya_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void sonya_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void sonya_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void sonya_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void sonya_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_script_004e69b0(fighter);
}

void sonya_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_script_004e6a40(fighter);
}

void sonya_entry_retail_script_004e81d0(Fighter *fighter) {
  retail_script_004e81d0(fighter);
}

void sonya_entry_retail_script_004e8578(Fighter *fighter) {
  retail_script_004e8578(fighter);
}

void sonya_entry_retail_script_004e85a8(Fighter *fighter) {
  retail_script_004e85a8(fighter);
}

void sonya_entry_retail_script_004e85c0(Fighter *fighter) {
  retail_script_004e85c0(fighter);
}

void sonya_entry_retail_script_004e8b38(Fighter *fighter) {
  retail_script_004e8b38(fighter);
}

void sonya_entry_retail_script_004e8f50(Fighter *fighter) {
  retail_script_004e8f50(fighter);
}

void sonya_entry_retail_script_004e9110(Fighter *fighter) {
  retail_script_004e9110(fighter);
}

void sonya_entry_retail_script_004e98a0(Fighter *fighter) {
  retail_script_004e98a0(fighter);
}

void sonya_entry_retail_script_004e9a68(Fighter *fighter) {
  retail_script_004e9a68(fighter);
}

void sonya_entry_retail_script_004e9b88(Fighter *fighter) {
  retail_script_004e9b88(fighter);
}

void sonya_entry_retail_script_004e9de8(Fighter *fighter) {
  retail_script_004e9de8(fighter);
}

void sonya_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_script_004ea0b8(fighter);
}

void sonya_entry_retail_script_004ea130(Fighter *fighter) {
  retail_script_004ea130(fighter);
}

void sonya_entry_retail_script_004ec7a0(Fighter *fighter) {
  retail_script_004ec7a0(fighter);
}

void sonya_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_script_004ec7f0(fighter);
}

void sonya_entry_retail_script_004ed4c0(Fighter *fighter) {
  retail_script_004ed4c0(fighter);
}

const mmove_t *sonya_retail_script_004e0138_repeat_phase(void) {
  return &retail_script_004e0138_phase;
}
