#ifndef MK4_RETAIL_VM_H
#define MK4_RETAIL_VM_H
#include "game.h"

#define RETAIL_SCRIPT_BEGIN 0x4d0000u
#define RETAIL_SCRIPT_END 0x4ef000u

void retail_vm_bind_animation(Fighter *fighter, uint32_t descriptor, int first);
void retail_vm_repeat(Fighter *fighter, uint32_t entry, unsigned count, uint32_t after);
void retail_vm_repeat_next(Fighter *fighter);
void retail_vm_enter_then_owned(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish, unsigned native_owner);
void retail_vm_enter_owned(Fighter *fighter, uint32_t entry, FighterFn next, unsigned native_owner);
void retail_vm_enter(Fighter *fighter, uint32_t entry, FighterFn finish);

/* Caller supplies a reviewed contiguous script range and
 * typed native adapter. Nothing in the image is executed as host code. */
void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn finish, RetailNativeFn native);
void retail_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, uint32_t begin, uint32_t end, FighterFn finish, RetailNativeFn native);
void retail_vm_step(Fighter *fighter);
void retail_vm_cancel(Fighter *fighter);
const Mk4Image *retail_vm_image(void);
AttackRecord retail_vm_attack_record(const Mk4Image *image, uint32_t address);
void retail_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next);
void retail_vm_return(Fighter *fighter);
void retail_vm_forward_return(Fighter *fighter);
void retail_vm_reverse_return(Fighter *fighter);
#endif
