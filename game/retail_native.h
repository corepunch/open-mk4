#ifndef MK4_RETAIL_NATIVE_H
#define MK4_RETAIL_NATIVE_H
#include "game.h"

void retail_kick_contact_rows(Fighter *fighter, uint32_t table);
void retail_kick_contact_default(Fighter *fighter);
void retail_kick_contact_wide(Fighter *fighter);
void retail_kick_contact_narrow(Fighter *fighter);
void retail_fan_start(Fighter *fighter);
void retail_fan_finish(Fighter *fighter);
void retail_fan_tail(Fighter *fighter);
void retail_fan_cycle(Fighter *fighter);
void retail_fan_victory(Fighter *fighter);
void retail_fan_victim_end(Fighter *fighter);
bool retail_native_vm(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument);
FighterFn retail_native_entry(unsigned character, uint32_t address);
bool retail_can_finish_close(const Fighter *fighter);
bool retail_can_finish_range(const Fighter *fighter);
bool retail_has_gravity(const Fighter *fighter);
CommandPredicate retail_native_predicate(uint32_t address);
bool retail_native_gate(const Fighter *fighter, uint32_t address);
#endif
