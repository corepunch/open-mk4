#ifndef MK4_CHARACTERS_H
#define MK4_CHARACTERS_H
#include "game.h"

extern const Motion mk4_motion;

bool mk4_can_fan_finish(const Fighter *fighter);
void mk4_special_stamp(Fighter *fighter, int slot);
bool mk4_special_elapsed(const Fighter *fighter, int slot, uint64_t ticks);
bool mk4_can_spike_finish(const Fighter *fighter);
bool mk4_begin_stage_finish(Fighter *fighter, int id);
bool mk4_can_ground_throw(const Fighter *fighter, bool low_kick);
bool mk4_can_air_throw(const Fighter *fighter);
bool mk4_can_sidestep(const Fighter *fighter);
void mk4_sidestep(Fighter *fighter);
void mk4_script_air_throw_release(Fighter *fighter);
void mk4_mmove_air_getup(Fighter *fighter);
void scorpion_generated_reaction(Fighter *fighter);
void raiden_generated_reaction(Fighter *fighter);
void reiko_star_reaction(Fighter *fighter);
void sonya_generated_reaction(Fighter *fighter);
void kitana_generated_reaction(Fighter *fighter);
void mk4_walk(Fighter *fighter);
void mk4_jump(Fighter *fighter);
void mk4_jump_backward(Fighter *fighter);
void mk4_land(Fighter *fighter);
void mk4_return_to_stance(Fighter *fighter);
void mk4_flip_apex_return(Fighter *fighter);

void mk4_air_throw_frames(Fighter *fighter, uint32_t owner_script, FighterFn victim_script);
void mk4_throw_flip_tail(Fighter *fighter);
void mk4_air_pair_frames(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, uint32_t owner_script, FighterFn victim_script);
void mk4_mmove_air_throw_release(Fighter *fighter);

void kai_handstand_reaction(Fighter *fighter);
void kai_weapon_reaction(Fighter *fighter);
void cage_weapon_reaction(Fighter *fighter);
void tanya_weapon_reaction(Fighter *fighter);
void cage_shadow_victim(Fighter *fighter);
void cage_power_victim(Fighter *fighter);
void jax_dash_victim_hit(Fighter *fighter);
void jax_dash_victim_throw(Fighter *fighter);
void jax_fatality_2_head(Fighter *fighter);
void jax_slam_victim(Fighter *fighter);
void cage_second_victim_hit(Fighter *fighter);
void shinnok_second_victim(Fighter *fighter);
void jarek_weapon_reaction(Fighter *fighter);
void jarek_ball_victim(Fighter *fighter);
void shinnok_weapon_reaction(Fighter *fighter);
void quan_chi_skull_reaction(Fighter *fighter);
void quan_chi_flip_reaction(Fighter *fighter);
void quan_chi_stomp_reaction(Fighter *fighter);
void quan_chi_steal_reaction(Fighter *fighter);
#endif
