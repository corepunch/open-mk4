#include "character.h"
#include "retail_vm.h"

static const ButtonTable scorpion_standing;

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebae0, B selector 1 */
/* 0x4ebb58, B selector 35 */

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};

const MoveDef scorpion_s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
const MoveDef scorpion_s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
const MoveDef scorpion_s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
const MoveDef scorpion_s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */

const MoveDef scorpion_s_throw = {.clip = 75, .last = 64, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &scorpion_s_jab,
  [S_KICK] = &scorpion_s_kick,
  [S_SWEEP] = &scorpion_s_sweep,
  [S_UPPERCUT] = &scorpion_s_uppercut,
  [S_THROW] = &scorpion_s_throw,
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
  [S_PUNCH] = retail_jab,
  [S_KICK] = retail_high_kick,
  [S_SWEEP] = retail_sweep,
  [S_UPPERCUT] = scorpion_uppercut,
};
// clang-format on

/* Specials translated from spear_entry/return, teleport_entry/landing and
 * breathe_fire in build/retail-disassembly/scripts. Native loops are ordinary
 * C continuations. The camera-dependent teleport exit uses a fixed ten-tick
 * vanish in our engine; it retains the distance/wall exits and contact branches. */

const MoveDef scorpion_s_teleport_land = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = ACTION_RETAIL_2008};

/* Ground scripts and their native tails: 4e6d08/46a8d0, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* Paired ground throws: 0x4de6e4 (LP), 0x4de2f8/318/328 (LK).
 * Native fighter_peer-root alignment is represented by a shared world origin. */

/* Generated VM finishers use shared scene, blend and object helpers. */

static const ButtonTable scorpion_standing = {{[BUTTON_HIGH_PUNCH] = scorpion_press_jab,
    [BUTTON_LOW_PUNCH] = scorpion_press_lp,
    [BUTTON_HIGH_KICK] = scorpion_press_kick,
    [BUTTON_LOW_KICK] = scorpion_press_lk}};
static const ButtonTable scorpion_crouching = {{[BUTTON_HIGH_PUNCH] = scorpion_press_uppercut,
    [BUTTON_LOW_PUNCH] = scorpion_press_crouch_lp,
    [BUTTON_HIGH_KICK] = scorpion_press_crouch_hk,
    [BUTTON_LOW_KICK] = scorpion_press_crouch_lk}};
static const ButtonTable scorpion_jumping = {{[BUTTON_HIGH_PUNCH] = scorpion_press_air_hp,
    [BUTTON_LOW_PUNCH] = scorpion_press_air_lp,
    [BUTTON_HIGH_KICK] = scorpion_press_air_hk,
    [BUTTON_LOW_KICK] = scorpion_press_air_lk}};
static const Controls scorpion_controls = {NULL, 0, {&scorpion_standing, &scorpion_crouching, &scorpion_jumping}};

/* Weapon scripts 0x4e62c0/0x4e62d8, 0x4ec8b0/0x4ec918 and 0x4e8850;
 * slash parameter blocks 0x4ec880/0x4ec898; native throw 0x498290. */

// clang-format off: Keep table entries on one row, including long rows.
static const MoveListEntry move_list[] = {
  {MOVE_LIST_SPECIAL, "SPEAR", "B,B+LP"},
  {MOVE_LIST_SPECIAL, "HELLFIRE", "D,F+LP"},
  {MOVE_LIST_SPECIAL, "TELEPORT", "D,B+HP (AIR OK)"},
  {MOVE_LIST_SPECIAL, "WEAPON", "F,F+HK"},
  {MOVE_LIST_FINISHER, "FATALITY 1", "B,F,F,B+BL"},
  {MOVE_LIST_FINISHER, "FATALITY 2", "B,F,D,U+HP"},
  {MOVE_LIST_FINISHER, "FAN STAGE", "F,D,D+LK"},
  {MOVE_LIST_FINISHER, "SPIKE STAGE", "B,F,F+LK"},
};
// clang-format on

const CharacterDef mk4_scorpion = {
    .motion = &mk4_motion,
    .moves = moves,
    .move_count = S_COUNT,
    .controls = &scorpion_controls,
    .enter = enter,
    .walk = mk4_walk,
    .jump = mk4_jump,
    .cpu = NULL,
    .move_list = move_list,
    .move_list_count = COUNT(move_list),
};

/* Native-only blends; no VM timing tables. */

void scorpion_entry_retail_script_004d2e58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2e58, fighter_ready, 0);
}

void scorpion_entry_retail_script_004d2e88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2e88, fighter_ready, 0);
}

void scorpion_entry_retail_script_004d2eb0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2eb0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004d2ef8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2ef8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004d2f10(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4d2f10, fighter_ready, 0);
}

void scorpion_entry_retail_script_004de2f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de2f8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004de318(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de318, fighter_ready, 0);
}

void scorpion_entry_retail_script_004de328(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de328, fighter_ready, 0);
}

void scorpion_entry_retail_script_004de798(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de798, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0278(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0278, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0288(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0288, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0298(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0298, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e02a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e02a8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e02b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e02b8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e02d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e02d0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0300(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0300, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0310(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0310, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0320(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0320, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0338(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0338, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0348(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0348, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e0358(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0358, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e5b58(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5b58, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e5e28(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e28, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e5e50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5e50, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e60b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e60b8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6158(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6158, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6190(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6190, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e61e0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61e0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e61f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e61f8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e62a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62a0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e63d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e63d8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6500(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6500, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6658(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6658, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e66d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66d0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e66f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e66f0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6710(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6710, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6844(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6844, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6860(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6860, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6878(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6878, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6950(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6950, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e69b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e69b0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e6d08(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6d08, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e87b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e87b0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e8818(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8818, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e8850(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8850, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e97f8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e97f8, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e9910(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9910, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e9938(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9938, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e9970(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9970, mk4_script_thrown_ready, 0);
}

void scorpion_entry_retail_script_004e9d00(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9d00, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e9f20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9f20, fighter_ready, 0);
}

void scorpion_entry_retail_script_004ec8b0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec8b0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004ec918(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec918, fighter_ready, 0);
}

void scorpion_entry_retail_script_004ed448(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed448, scorpion_script_spear_done, 0);
}

void scorpion_entry_retail_script_004ed4a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed4a8, fighter_ready, 0);
}
