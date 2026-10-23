#ifndef MK4_SCRIPT_LUA_H
#define MK4_SCRIPT_LUA_H
#include "game.h"

/* Lua actor scripts.
 *
 * The host opens one Lua state next to the linked data package: the symbol
 * map written by tools/mk4_lua_symbols.py gives every script label its
 * linked address, and the modules listed by program.lua define the script
 * functions. Each actor script runs as one coroutine; the shared
 * script_vm_step resumes it after the engine-owned playback prologue. */
void script_lua_open(const char *symbols_path, const char *program_path);
/* Open with the symbol map that sits beside the data package. */
void script_lua_open_beside(const char *data_path, const char *program_path);
void script_lua_close(void);

/* Called by the script execution context when an entry starts and on each
 * actor tick that no playback owns. A start without a Lua function is fatal. */
void script_lua_start(Fighter *fighter, uint32_t entry);
void script_lua_step(Fighter *fighter);
const char *script_lua_entry_name(uint32_t address);

/* Evidence for tests: wrap every script function with an execution counter
 * (before open), heap after a full collection, entry starts and operation
 * and function counts. */
void script_lua_set_coverage(bool enabled);
unsigned script_lua_entries(void);
unsigned script_lua_memory_kb(void);
void script_lua_visit_entries(void (*visit)(uint32_t address, const char *name, unsigned count, bool owned));
void script_lua_visit_operations(void (*visit)(const char *name, unsigned count));
void script_lua_visit_functions(void (*visit)(const char *name, unsigned count));
#endif
