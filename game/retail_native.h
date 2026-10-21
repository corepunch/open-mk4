#ifndef MK4_RETAIL_NATIVE_H
#define MK4_RETAIL_NATIVE_H
#include "game.h"

FighterFn retail_native_entry(unsigned character, uint32_t address);
CommandPredicate retail_native_predicate(unsigned character, uint32_t address);
bool retail_native_gate(const Fighter *fighter, uint32_t address);
#endif
