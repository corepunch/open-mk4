#include "character.h"
#include "retail_vm.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebae0, B selector 1 */
/* 0x4ebb58, B selector 35 */

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */

/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Specials translated from spear_entry/return, teleport_entry/landing and
 * breathe_fire in build/retail-disassembly/scripts. Native loops are ordinary
 * C continuations. The camera-dependent teleport exit uses a fixed ten-tick
 * vanish in our engine; it retains the distance/wall exits and contact branches. */

/* Ground scripts and their native tails: 4e6d08/46a8d0, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* Paired ground throws: 0x4de6e4 (LP), 0x4de2f8/318/328 (LK).
 * Native fighter_peer-root alignment is represented by a shared world origin. */

/* Generated VM finishers use shared scene, blend and object helpers. */

/* Weapon scripts 0x4e62c0/0x4e62d8, 0x4ec8b0/0x4ec918 and 0x4e8850;
 * slash parameter blocks 0x4ec880/0x4ec898; native throw 0x498290. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

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

void scorpion_entry_retail_script_004de318(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de318, fighter_ready, 0);
}

void scorpion_entry_retail_script_004de328(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de328, fighter_ready, 0);
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

void scorpion_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 0);
}

void scorpion_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 0);
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
