#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebaf8, B selector 2 */
/* 0x4ebb58, B selector 35 */

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

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
static const FighterFn enter[S_COUNT] = {
  [S_IDLE] = weapon_idle,
  [S_WALK] = weapon_walk,
  [S_CROUCH] = weapon_crouch,
  [S_BLOCK] = weapon_block,
};
// clang-format on

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

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

static const ButtonTable jumping = {.press = {[BUTTON_HIGH_PUNCH] = sub_zero_press_air_hp,
                                        [BUTTON_LOW_PUNCH] = sub_zero_press_air_lp,
                                        [BUTTON_HIGH_KICK] = sub_zero_press_air_hk,
                                        [BUTTON_LOW_KICK] = sub_zero_press_air_lk}};
static const Controls controls = {NULL, 0, {&retail_ground_buttons[0], &retail_ground_buttons[1], &jumping}};

static const CpuMoves cpu_moves = {.enter = {
                                       [CPU_JAB] = retail_jab,
                                       [CPU_LOW_KICK] = retail_low_kick,
                                       [CPU_HIGH_KICK] = sub_zero_hk,
                                       [CPU_SWEEP] = retail_sweep_state,
                                       [CPU_UPPERCUT] = retail_uppercut_state,
                                       [CPU_THROW] = retail_lp_throw,
                                       [CPU_CLOSE_HP] = sub_zero_cpu_close_hp,
                                       [CPU_CLOSE_HK] = sub_zero_cpu_close_hk,
                                       [CPU_CROUCH_LP] = retail_crouch_low_punch,
                                       [CPU_AIR_HP] = retail_air_high_punch,
                                       [CPU_AIR_HK] = retail_air_diagonal_high_kick,
                                       [CPU_AIR_LK] = retail_air_diagonal_low_kick,
                                       [CPU_DRAW] = sub_zero_weapon,
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

void sub_zero_entry_retail_script_004d2bf0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2bf0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004d2c10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2c10, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004d2c78(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2c78, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004d2ca0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2ca0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004d2cc8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2cc8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004de4b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de4b8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004de4e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de4e0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0368(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0368, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0378(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0378, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0388(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0388, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0398(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0398, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e03a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e03a8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e03b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e03b8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e03c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e03c8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e03d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e03d8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e03f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e03f0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0420(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0420, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e0430(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0430, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e84d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84d8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e84f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e84f8, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e8530(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8530, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e9bd0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9bd0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004e9ee0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9ee0, fighter_ready, 4);
}

void sub_zero_entry_retail_script_004ed588(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed588, fighter_ready, 4);
}
