#ifndef MK4_RETAIL_NATIVE_H
#define MK4_RETAIL_NATIVE_H
#include "game.h"

void retail_close_high_punch(Fighter *fighter);
void retail_crouch_high_kick(Fighter *fighter);
void retail_press_crouch_high_kick(Fighter *fighter);
void retail_press_crouch_low_kick(Fighter *fighter);
void retail_crouch_low_punch(Fighter *fighter);
void retail_press_crouch_low_punch(Fighter *fighter);
void retail_crouch_low_kick(Fighter *fighter);
void retail_crouch_low_kick_tail(Fighter *fighter);
FighterFn retail_throw_reaction(const Mk4Image *image, unsigned reaction);
void retail_throw_victim_continue(Fighter *fighter);
void retail_throw_victim_impact(Fighter *fighter);
void retail_impact_voice(Fighter *fighter);
void retail_lk_continue_840(Fighter *fighter);
void retail_lk_throw(Fighter *fighter);
void retail_throw_pair_start(Fighter *fighter);
void retail_lk_return(Fighter *fighter);
void retail_lk_pause_long(Fighter *fighter);
void retail_lk_pause_short(Fighter *fighter);
void retail_lk_jump_back(Fighter *fighter);
void retail_lk_recoil(Fighter *fighter);
void retail_lk_branch_continue(Fighter *fighter);
void retail_lk_victim_return(Fighter *fighter);
void retail_lk_script_170(Fighter *fighter);
void retail_lk_script_180(Fighter *fighter);
void retail_lk_script_190(Fighter *fighter);
void retail_lp_throw(Fighter *fighter);
void retail_face_peer(Fighter *fighter);
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
void retail_lp_select_short(Fighter *fighter);
void retail_lp_select_long(Fighter *fighter);
#endif
