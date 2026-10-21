#include "retail_vm.h"
#include "character.h"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */

/* 0x4ebd50, A selector 2 */
/* 0x4ebd68, A selector 6 */
/* 0x4ebaf8, B selector 2 */
/* 0x4ebb70, B selector 36 */

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

void raiden_entry_retail_script_004de668(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4de668, fighter_ready, 1);
}

void raiden_entry_retail_script_004e0210(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0210, fighter_ready, 1);
}

void raiden_entry_retail_script_004e0220(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e0220, fighter_ready, 1);
}

void raiden_entry_retail_script_004e56c8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56c8, fighter_ready, 1);
}

void raiden_entry_retail_script_004e56d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56d8, fighter_ready, 1);
}

void raiden_entry_retail_script_004e56f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e56f0, fighter_ready, 1);
}

void raiden_entry_retail_script_004e5740(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5740, fighter_ready, 1);
}

void raiden_entry_retail_script_004e5760(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5760, fighter_ready, 1);
}

void raiden_entry_retail_script_004e5770(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5770, fighter_ready, 1);
}

void raiden_entry_retail_script_004e5788(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e5788, fighter_ready, 1);
}

void raiden_entry_retail_script_004e62c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 1);
}

void raiden_entry_retail_script_004e62d8(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 1);
}

void raiden_entry_retail_script_004e6a40(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e6a40, fighter_ready, 1);
}

void raiden_entry_retail_script_004e8440(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8440, fighter_ready, 1);
}

void raiden_entry_retail_script_004e8458(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8458, fighter_ready, 1);
}

void raiden_entry_retail_script_004e8e18(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e8e18, mk4_script_thrown_ready, 1);
}

void raiden_entry_retail_script_004e99c0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e99c0, fighter_ready, 1);
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
  retail_vm_enter_owned(fighter, 0x4ea108, fighter_ready, 1);
}

void raiden_entry_retail_script_004ed5f0(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4ed5f0, fighter_ready, 1);
}
