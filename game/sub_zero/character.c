#include "character.h"

static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;

#include "generated/scripts.inc"
#include "generated/frames.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebaf8, B selector 2 */
/* 0x4ebb58, B selector 35 */

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {267, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef sub_zero_s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
const MoveDef sub_zero_s_kick = {.clip = 71, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
const MoveDef sub_zero_s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef sub_zero_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
const MoveDef sub_zero_s_throw = {.clip = 76, .last = 48, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &sub_zero_s_jab,
  [S_KICK] = &sub_zero_s_kick,
  [S_SWEEP] = &sub_zero_s_sweep,
  [S_UPPERCUT] = &sub_zero_s_uppercut,
  [S_THROW] = &sub_zero_s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = sub_zero_armed_idle,
  [S_WALK] = sub_zero_armed_walk,
  [S_CROUCH] = sub_zero_armed_crouch,
  [S_BLOCK] = sub_zero_armed_block,
  [S_PUNCH] = sub_zero_jab,
  [S_KICK] = sub_zero_kick,
  [S_SWEEP] = sub_zero_sweep,
  [S_UPPERCUT] = sub_zero_uppercut,
};
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

// clang-format off: Keep table entries on one row, including long rows.
const GroundAttack sub_zero_ground_attacks[] = {
  {{.clip = 70, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {0x15c28, -0x35c2, 0x104df, 0x5c28, 17, 10, 6, 0, 0x1c28}, 11, 30, 32},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 52, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {0xa8f5, -0xd70a, 0xa3d7, 0xb851, 26, 26, 7, 0, 0x1999}, 7, 31, 31},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 532, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 11, 29, 32},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on

const MoveDef sub_zero_s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

const NativeGroundProfile sub_zero_ground_profile = {
    sub_zero_ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, sub_zero_ready, 5, 0, 28, 21, 22, 38, NULL};

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

static const ButtonTable close_hp_followup = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_combo_hp,
    [BUTTON_LOW_PUNCH] = sub_zero_press_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_kick,
    [BUTTON_LOW_KICK] = sub_zero_press_lk}};
static const ButtonTable combo_hp_followup = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_jab,
    [BUTTON_LOW_PUNCH] = sub_zero_press_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_kick,
    [BUTTON_LOW_KICK] = sub_zero_press_lk}};
static const ButtonTable close_hk_followup = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_knee_hp,
    [BUTTON_LOW_PUNCH] = sub_zero_press_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_combo_hk,
    [BUTTON_LOW_KICK] = sub_zero_press_lk}};

/* LP: roster script 4de798[4] -> 4de6b8/4de898; reaction 102 is
 * 485b70 -> 4e9bd0, then reverse getup 488b80/4fbcdc.
 * LK: 4ddfb8[4] -> 42c180 -> 4de490/4de4b8/4de4e0;
 * 42c300 releases victim through 4871e0/4e9ee0/487220.
 * Pair alignment uses our shared world origin, not retail task/group pointers. */

const MoveDef sub_zero_s_lp_thrown = {.clip = 153, .last = 25, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

/* Native 488b80: reversed getup clip 328, final sample held for one tick. */

/* Freeze: 49bc60 -> 4ed588/4ed818 -> 49bcb0. Flight is an independent
 * task, so it survives owner interruption; bounds and contact own teardown. */

/* 49c300 creates rig 506a48 (mesh 74); 49bcfe installs 4ed898:
 * shared meshes 75..82, one tick each, looping without the spawn rig.
 * The high bit invokes 40d720 for the separately managed ice trail. */

/* 40d720/40d920, 4d1b18/28/34: ten independent trail nodes,
 * ten-tick track, quarter scale shrinking by .02 each tick.
 * The supplied EXE's 405430 gate always succeeds. */

/* 47f4d0 copies the current pose before the retreat. Both ice predicates
 * inspect clone task types 45/46, not the projectile task type 42. */

/* Slide: 47dc00/4e84d8 -> 47dc40; hit 4e84f8, block 4e8530.
 * Only an unblocked contact stamps cooldown slot 2 (47de8e). */

/* Native 47dc40 queries contact on simulation frames 9 through 25. */

/* Shared weapon draw 4e62c0/4e62d8, air 4e66d0/4e66f0 and
 * throw 498290. Scepter ground strikes use character-4 descriptor 4ecdb8. */
/* Scepter descriptor 4ecdb8. Parameter blocks 4ec458/490/480/4b8
 * select the shared native attack runner and its recovery continuations. */

/* Generated VM finishers use shared scene, blend and object helpers. */

/* These records are native blend operations. VM playback is generated. */

/* 445920: camera records 4e0f00/4e0f58, reordered from X,Z,Y. */

/* 445740/446620 and 4e0440: independent head/spine playback. */

/* 445c40: eleven double sprays, eleven singles, thirteen 2/1-tick
 * pairs, then seventy-one singles two ticks apart. 445e20 aims from
 * joint 2 away from joint 3; 474330 selects the shared blood mesh. */

/* 46f080/46f130: two native contact visits, then remaining clip playback.
 * Keep the established portable timing; source-script startup is generated. */

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, sub_zero_can_first_fatality, sub_zero_first_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_BACK, TOKEN_DOWN, TOKEN_BACK}, 4, 50, 0, sub_zero_can_second_fatality, sub_zero_second_fatality},
  {BUTTON_LOW_PUNCH, {0}, 0, 0, 0, sub_zero_can_slide, sub_zero_slide},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0, sub_zero_can_ice, sub_zero_ice},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 18, CMD_AIR, sub_zero_can_clone, sub_zero_clone},
  {BUTTON_BLOCK, {0}, 0, 0, 0, sub_zero_can_slide, sub_zero_slide},
  {BUTTON_BLOCK, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, sub_zero_can_first_fatality, sub_zero_first_fatality}, /* Requested extension: retail Sub-Zero has no 4e4a04 air-throw record. * Keep his slide/fatality priority, then reuse the shared retail throw. */
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, sub_zero_air_throw},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_UP, TOKEN_UP, TOKEN_UP}, 4, 48, 0, sub_zero_can_fan_fatality, sub_zero_fan_fatality},
  {BUTTON_LOW_KICK, {0}, 0, 0, 0, sub_zero_can_slide, sub_zero_slide},
  {BUTTON_LOW_KICK, {TOKEN_DOWN, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, sub_zero_can_spike_fatality, sub_zero_spike_fatality},
  {BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 24, 0, sub_zero_can_weapon, sub_zero_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, sub_zero_can_sidestep, sub_zero_sidestep},
  {BUTTON_RUN, {0}, 0, 0, 0, sub_zero_can_pickup, sub_zero_pickup},
  {BUTTON_RUN, {TOKEN_FORWARD, TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN}, 4, 60, CMD_BLOCK_OK, sub_zero_can_first_fatality, sub_zero_first_fatality},
};
// clang-format on

static const ButtonTable standing = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_jab,
    [BUTTON_LOW_PUNCH] = sub_zero_press_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_kick,
    [BUTTON_LOW_KICK] = sub_zero_press_lk}};
static const ButtonTable crouching = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_uppercut,
    [BUTTON_LOW_PUNCH] = sub_zero_press_crouch_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_crouch_hk,
    [BUTTON_LOW_KICK] = sub_zero_press_crouch_lk}};
static const ButtonTable jumping = {{[BUTTON_HIGH_PUNCH] = sub_zero_press_air_hp,
    [BUTTON_LOW_PUNCH] = sub_zero_press_air_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_air_hk,
    [BUTTON_LOW_KICK] = sub_zero_press_air_lk}};
static const Controls controls = {commands, COUNT(commands), {&standing, &crouching, &jumping}};

static const CpuMoves cpu_moves = {.enter = {
                                       [CPU_JAB] = sub_zero_jab,
                                       [CPU_LOW_KICK] = sub_zero_lk,
                                       [CPU_HIGH_KICK] = sub_zero_hk,
                                       [CPU_SWEEP] = sub_zero_enter_sweep,
                                       [CPU_UPPERCUT] = sub_zero_enter_uppercut,
                                       [CPU_THROW] = sub_zero_enter_throw,
                                       [CPU_CLOSE_HP] = sub_zero_cpu_close_hp,
                                       [CPU_CLOSE_HK] = sub_zero_cpu_close_hk,
                                       [CPU_CROUCH_LP] = sub_zero_crouch_lp,
                                       [CPU_AIR_HP] = sub_zero_air_hp,
                                       [CPU_AIR_HK] = sub_zero_air_hk_diagonal,
                                       [CPU_AIR_LK] = sub_zero_air_lk_diagonal,
                                       [CPU_PROJECTILE] = sub_zero_ice,
                                       [CPU_SPECIAL_1] = sub_zero_clone,
                                       [CPU_SPECIAL_2] = sub_zero_slide,
                                       [CPU_DRAW] = sub_zero_weapon,
                                       [CPU_FINISH_1] = sub_zero_first_fatality,
                                       [CPU_FINISH_2] = sub_zero_second_fatality,
                                       [CPU_FINISH_FAN] = sub_zero_fan_fatality,
                                       [CPU_FINISH_SPIKE] = sub_zero_spike_fatality,
                                   }};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "ICE BLAST", "D,F+LP"},
  {MOVE_LIST_SPECIAL, "ICE CLONE", "D,B+LP (AIR OK)"},
  {MOVE_LIST_SPECIAL, "SLIDE", "LP+BL+LK"},
  {MOVE_LIST_SPECIAL, "WEAPON", "D,F+HK"},
  {MOVE_LIST_FINISHER, "FATALITY 1", "F,B,F,D+HP"},
  {MOVE_LIST_FINISHER, "FATALITY 2", "B,B,D,B+HP"},
  {MOVE_LIST_FINISHER, "FAN STAGE", "D,U,U,U+HK"},
  {MOVE_LIST_FINISHER, "SPIKE STAGE", "D,D,D+LK"},
};
// clang-format on

const CharacterDef mk4_sub_zero = {
    GAME_MK4, 4, "SUB ZERO", &mk4_motion, moves, S_COUNT, &controls, enter, -77660, mk4_walk, mk4_jump, &cpu_moves, move_list, COUNT(move_list)};

void sub_zero_entry_retail_script_004d2bf0(Fighter *fighter) {
  retail_script_004d2bf0(fighter);
}

void sub_zero_entry_retail_script_004d2c10(Fighter *fighter) {
  retail_script_004d2c10(fighter);
}

void sub_zero_entry_retail_script_004d2c78(Fighter *fighter) {
  retail_script_004d2c78(fighter);
}

void sub_zero_entry_retail_script_004d2ca0(Fighter *fighter) {
  retail_script_004d2ca0(fighter);
}

void sub_zero_entry_retail_script_004d2cc8(Fighter *fighter) {
  retail_script_004d2cc8(fighter);
}

void sub_zero_entry_retail_script_004de490(Fighter *fighter) {
  retail_script_004de490(fighter);
}

void sub_zero_entry_retail_script_004de4b8(Fighter *fighter) {
  retail_script_004de4b8(fighter);
}

void sub_zero_entry_retail_script_004de4e0(Fighter *fighter) {
  retail_script_004de4e0(fighter);
}

void sub_zero_entry_retail_script_004de798(Fighter *fighter) {
  retail_script_004de798(fighter);
}

void sub_zero_entry_retail_script_004e0118(Fighter *fighter) {
  retail_script_004e0118(fighter);
}

void sub_zero_entry_retail_script_004e0148(Fighter *fighter) {
  retail_script_004e0148(fighter);
}

void sub_zero_entry_retail_script_004e0158(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

void sub_zero_entry_retail_script_004e0188(Fighter *fighter) {
  retail_script_004e0188(fighter);
}

void sub_zero_entry_retail_script_004e01a0(Fighter *fighter) {
  retail_script_004e01a0(fighter);
}

void sub_zero_entry_retail_script_004e0368(Fighter *fighter) {
  retail_script_004e0368(fighter);
}

void sub_zero_entry_retail_script_004e0378(Fighter *fighter) {
  retail_script_004e0378(fighter);
}

void sub_zero_entry_retail_script_004e0388(Fighter *fighter) {
  retail_script_004e0388(fighter);
}

void sub_zero_entry_retail_script_004e0398(Fighter *fighter) {
  retail_script_004e0398(fighter);
}

void sub_zero_entry_retail_script_004e03a8(Fighter *fighter) {
  retail_script_004e03a8(fighter);
}

void sub_zero_entry_retail_script_004e03b8(Fighter *fighter) {
  retail_script_004e03b8(fighter);
}

void sub_zero_entry_retail_script_004e03c8(Fighter *fighter) {
  retail_script_004e03c8(fighter);
}

void sub_zero_entry_retail_script_004e03d8(Fighter *fighter) {
  retail_script_004e03d8(fighter);
}

void sub_zero_entry_retail_script_004e03f0(Fighter *fighter) {
  retail_script_004e03f0(fighter);
}

void sub_zero_entry_retail_script_004e0420(Fighter *fighter) {
  retail_script_004e0420(fighter);
}

void sub_zero_entry_retail_script_004e0430(Fighter *fighter) {
  retail_script_004e0430(fighter);
}

void sub_zero_entry_retail_script_004e5c80(Fighter *fighter) {
  retail_script_004e5c80(fighter);
}

void sub_zero_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_script_004e5e28(fighter);
}

void sub_zero_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_script_004e5e50(fighter);
}

void sub_zero_entry_retail_script_004e5fd8(Fighter *fighter) {
  retail_script_004e5fd8(fighter);
}

void sub_zero_entry_retail_script_004e6158(Fighter *fighter) {
  retail_script_004e6158(fighter);
}

void sub_zero_entry_retail_script_004e6190(Fighter *fighter) {
  retail_script_004e6190(fighter);
}

void sub_zero_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_script_004e61e0(fighter);
}

void sub_zero_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_script_004e61f8(fighter);
}

void sub_zero_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_script_004e62a0(fighter);
}

void sub_zero_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_script_004e62c0(fighter);
}

void sub_zero_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_script_004e62d8(fighter);
}

void sub_zero_entry_retail_script_004e62f0(Fighter *fighter) {
  retail_script_004e62f0(fighter);
}

void sub_zero_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_script_004e63d8(fighter);
}

void sub_zero_entry_retail_script_004e6500(Fighter *fighter) {
  retail_script_004e6500(fighter);
}

void sub_zero_entry_retail_script_004e6658(Fighter *fighter) {
  retail_script_004e6658(fighter);
}

void sub_zero_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_script_004e66d0(fighter);
}

void sub_zero_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_script_004e66f0(fighter);
}

void sub_zero_entry_retail_script_004e6710(Fighter *fighter) {
  retail_script_004e6710(fighter);
}

void sub_zero_entry_retail_script_004e6798(Fighter *fighter) {
  retail_script_004e6798(fighter);
}

void sub_zero_entry_retail_script_004e6800(Fighter *fighter) {
  retail_script_004e6800(fighter);
}

void sub_zero_entry_retail_script_004e6860(Fighter *fighter) {
  retail_script_004e6860(fighter);
}

void sub_zero_entry_retail_script_004e6878(Fighter *fighter) {
  retail_script_004e6878(fighter);
}

void sub_zero_entry_retail_script_004e6950(Fighter *fighter) {
  retail_script_004e6950(fighter);
}

void sub_zero_entry_retail_script_004e6998(Fighter *fighter) {
  retail_script_004e6998(fighter);
}

void sub_zero_entry_retail_script_004e69c8(Fighter *fighter) {
  retail_script_004e69c8(fighter);
}

void sub_zero_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_script_004e6a40(fighter);
}

void sub_zero_entry_retail_script_004e6da0(Fighter *fighter) {
  retail_script_004e6da0(fighter);
}

void sub_zero_entry_retail_script_004e84d8(Fighter *fighter) {
  retail_script_004e84d8(fighter);
}

void sub_zero_entry_retail_script_004e84f8(Fighter *fighter) {
  retail_script_004e84f8(fighter);
}

void sub_zero_entry_retail_script_004e8530(Fighter *fighter) {
  retail_script_004e8530(fighter);
}

void sub_zero_entry_retail_script_004e9910(Fighter *fighter) {
  retail_script_004e9910(fighter);
}

void sub_zero_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_script_004e9bd0(fighter);
}

void sub_zero_entry_retail_script_004e9ee0(Fighter *fighter) {
  retail_script_004e9ee0(fighter);
}

void sub_zero_entry_retail_script_004ed588(Fighter *fighter) {
  retail_script_004ed588(fighter);
}

const mmove_t *sub_zero_retail_script_004e0138_phase(void) {
  return &retail_script_004e0138_phase;
}
