#ifndef MK4_SCRIPT_BINDINGS_H
#define MK4_SCRIPT_BINDINGS_H
#include "game.h"

/* Linked native references resolve to typed host function pointers. */
bool script_native_dispatch(Fighter *fighter, uint32_t address, ScriptNativeKind kind, unsigned argument);
FighterFn script_native_entry(uint32_t address);
CommandPredicate script_native_predicate(uint32_t address);
bool script_native_gate(const Fighter *fighter, uint32_t address);
const char *script_native_name(uint32_t reference);
FighterFn script_native_engine_reaction(uint32_t reference);
FighterFn script_native_throw_reaction(uint32_t reference);
FighterFn script_native_reaction(uint32_t reference);
uint32_t script_native_script(uint32_t reference);
void script_native_continue(Fighter *fighter, uint32_t reference, bool alternate, FighterFn finish);
FighterFn script_native_button(Fighter *fighter, uint32_t reference);
int script_native_air_input(uint32_t reference);
void script_native_weapon(Fighter *fighter, uint32_t reference, bool back);
#endif
