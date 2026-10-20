#ifndef MK4_RETAIL_VM_H
#define MK4_RETAIL_VM_H
#include "game.h"

/* First prototype: caller supplies a reviewed contiguous script range and
 * typed native adapter. Nothing in the image is executed as host code. */
void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn finish, RetailNativeFn native);
void retail_vm_step(Fighter *fighter);
void retail_vm_cancel(Fighter *fighter);
void retail_vm_reverse_return(Fighter *fighter);
#endif
