#ifndef MK4_SCRIPT_VM_H
#define MK4_SCRIPT_VM_H
#include "game.h"

/* Actor script execution: Lua functions decide the operations, this module
 * owns playback, waits, contact results, pairing and native continuations. */
void script_vm_bind_animation(Fighter *fighter, uint32_t descriptor, int first);
void script_vm_repeat(Fighter *fighter, uint32_t entry, unsigned count, uint32_t after);
void script_vm_repeat_next(Fighter *fighter);
void script_vm_enter_then(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish);
void script_vm_enter(Fighter *fighter, uint32_t entry, FighterFn finish);

/* The entry is a linked script label; the Lua runtime must own it. */
void script_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, FighterFn finish);
void script_vm_step(Fighter *fighter);
/* Internal playback reset; action cleanup belongs to the execution context. */
void script_vm_reset(Fighter *fighter);
AttackRecord script_vm_attack_record(const Mk4Image *image, uint32_t address);
void script_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next);
void script_vm_return(Fighter *fighter);
void script_vm_forward_return(Fighter *fighter);
void script_vm_reverse_return(Fighter *fighter);

/* Playback primitives shared with the Lua operations. */
void script_vm_play(Fighter *fighter, int target, bool inclusive);
void script_vm_sample(Fighter *fighter, int pose);
void script_vm_contact(Fighter *fighter, uint32_t table, unsigned selector);
void script_vm_slow_play(Fighter *fighter, int target, bool inclusive);
void script_vm_interpolated_play(Fighter *fighter, unsigned limit);
void script_vm_blend(Fighter *fighter, uint32_t descriptor);
#endif
