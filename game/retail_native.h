#ifndef MK4_RETAIL_NATIVE_H
#define MK4_RETAIL_NATIVE_H
#include "game.h"

void retail_native_begin(Fighter *fighter, StateId state, int action, bool commands);
void retail_sweep(Fighter *fighter);
void retail_sweep_state(Fighter *fighter);
void retail_sweep_standard_tail(Fighter *fighter);
void retail_sweep_advancing_tail(Fighter *fighter);
void retail_jab(Fighter *fighter);
void retail_high_punch(Fighter *fighter);
void retail_high_kick(Fighter *fighter);
void retail_low_kick(Fighter *fighter);
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

void retail_press_sweep(Fighter *fighter);
void retail_roundhouse(Fighter *fighter);
void retail_press_roundhouse(Fighter *fighter);
void retail_roundhouse_start(Fighter *fighter);
void retail_roundhouse_wide_start(Fighter *fighter);
void retail_roundhouse_tail(Fighter *fighter);
void retail_roundhouse_wide_tail(Fighter *fighter);
void retail_uppercut(Fighter *fighter);
void retail_uppercut_state(Fighter *fighter);
void retail_press_uppercut(Fighter *fighter);

void retail_finish_first(Fighter *fighter);
void retail_finish_second(Fighter *fighter);
void retail_spike_finish(Fighter *fighter);
void retail_scene_pose(Fighter *fighter, int clip, int sample, bool owner);
#endif
