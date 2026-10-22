#ifndef MK4_SCRIPT_BINDINGS_H
#define MK4_SCRIPT_BINDINGS_H
#include "game.h"

/* Original native addresses are typed IDs, never executable host pointers. */
bool script_native_dispatch(Fighter *fighter, uint32_t address, ScriptNativeKind kind, unsigned argument);
FighterFn script_native_entry(uint32_t address);
CommandPredicate script_native_predicate(uint32_t address);
bool script_native_gate(const Fighter *fighter, uint32_t address);
#endif
