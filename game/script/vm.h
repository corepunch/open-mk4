#ifndef MK4_SCRIPT_VM_H
#define MK4_SCRIPT_VM_H
#include "game.h"

#define SCRIPT_ADDRESS_BEGIN 0x4d0000u
#define SCRIPT_ADDRESS_END 0x4ef000u

void script_vm_bind_animation(Fighter *fighter, uint32_t descriptor, int first);
void script_vm_repeat(Fighter *fighter, uint32_t entry, unsigned count, uint32_t after);
void script_vm_repeat_next(Fighter *fighter);
void script_vm_enter_then(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish);
void script_vm_enter(Fighter *fighter, uint32_t entry, FighterFn finish);

/* Caller supplies a reviewed contiguous script range and
 * typed native adapter. Nothing in the image is executed as host code. */
void script_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn finish, ScriptNativeFn native);
void script_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, uint32_t begin, uint32_t end, FighterFn finish, ScriptNativeFn native);
void script_vm_step(Fighter *fighter);
/* Internal playback reset; action cleanup belongs to the execution context. */
void script_vm_reset(Fighter *fighter);
AttackRecord script_vm_attack_record(const Mk4Image *image, uint32_t address);
void script_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next);
void script_vm_return(Fighter *fighter);
void script_vm_forward_return(Fighter *fighter);
void script_vm_reverse_return(Fighter *fighter);
#endif
