#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebac8, B selector 0 */
/* 0x4ebb58, B selector 35 */

/* Attack entries query source contact records through SF_RECORD. Locomotion
 * and generic reaction states retain the shared portable controllers. */

/* Native uppercut recovery uses contact-selected pose sequences. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

/* Ground scripts and their native tails: 4e6da0, 4e5fd8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69c8/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

/* 4ddfb8 roster dispatch and 4de798 LP selector. Owner and victim have
 * independent frame programs after release; replacement cancels only a live pair. */

/* 49c300 spawns independently of the owner's generated recovery. */

/* Native 494a00 parameter blocks; byte fields are decoded at authoring. */

// clang-format off: Keep table entries on one row, including long rows.
// clang-format on

void sonya_entry_retail_script_004e01c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e01c0, fighter_ready, 2);
}

void sonya_entry_retail_script_004e01d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e01d0, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5930(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5930, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5940(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5940, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5960(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5960, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5980(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5980, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5990(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5990, fighter_ready, 2);
}

void sonya_entry_retail_script_004e59a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e59a8, fighter_ready, 2);
}

void sonya_entry_retail_script_004e59c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e59c8, fighter_ready, 2);
}

void sonya_entry_retail_script_004e5a20(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5a20, fighter_ready, 2);
}

void sonya_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 2);
}

void sonya_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 2);
}

void sonya_entry_retail_script_004e81d0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e81d0, fighter_ready, 2);
}

void sonya_entry_retail_script_004e8578(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8578, fighter_ready, 2);
}

void sonya_entry_retail_script_004e85a8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e85a8, fighter_ready, 2);
}

void sonya_entry_retail_script_004e85c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e85c0, fighter_ready, 2);
}

void sonya_entry_retail_script_004e8b38(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8b38, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004e8f50(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8f50, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004e9110(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9110, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004e9a68(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9a68, fighter_ready, 2);
}

void sonya_entry_retail_script_004e9b88(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9b88, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004e9de8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9de8, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004ea0b8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea0b8, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004ea130(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ea130, mk4_script_thrown_ready, 2);
}

void sonya_entry_retail_script_004ec7a0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7a0, fighter_ready, 2);
}

void sonya_entry_retail_script_004ec7f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ec7f0, fighter_ready, 2);
}

void sonya_entry_retail_script_004ed4c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed4c0, fighter_ready, 2);
}
