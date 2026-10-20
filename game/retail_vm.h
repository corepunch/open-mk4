#ifndef MK4_RETAIL_VM_H
#define MK4_RETAIL_VM_H
#include "game.h"

/* Caller supplies a reviewed contiguous script range and
 * typed native adapter. Nothing in the image is executed as host code. */
void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn finish, RetailNativeFn native);
void retail_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, uint32_t begin, uint32_t end, FighterFn finish, RetailNativeFn native);
void retail_vm_step(Fighter *fighter);
void retail_vm_cancel(Fighter *fighter);
const Mk4Image *retail_vm_image(void);
AttackRecord retail_vm_attack_record(const Mk4Image *image, uint32_t address);
void retail_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next);
void retail_vm_reverse_return(Fighter *fighter);
#endif
