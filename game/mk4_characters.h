#ifndef MK4_CHARACTERS_H
#define MK4_CHARACTERS_H
#include "game.h"

extern const Motion mk4_motion;
extern const Controls mk4_controls;

extern const CharacterDef mk4_scorpion;
extern const CharacterDef mk4_raiden;
extern const CharacterDef mk4_sonya;
extern const CharacterDef mk4_liu_kang;
extern const CharacterDef mk4_sub_zero;
extern const CharacterDef mk4_fujin;
extern const CharacterDef mk4_shinnok;
extern const CharacterDef mk4_reiko;
extern const CharacterDef mk4_quan_chi;
extern const CharacterDef mk4_tanya;
extern const CharacterDef mk4_reptile;
extern const CharacterDef mk4_kai;
extern const CharacterDef mk4_jarek;
extern const CharacterDef mk4_jax;
extern const CharacterDef mk4_cage;
extern const CharacterDef mk4_goro;
extern const CharacterDef mk4_kitana;
extern const CharacterDef mk4_noob_saibot;
bool mk4_can_fan_finish(const Fighter *fighter);
bool mk4_can_spike_finish(const Fighter *fighter);
bool mk4_begin_stage_finish(Fighter *fighter, int id);
bool mk4_can_ground_throw(const Fighter *fighter, bool low_kick);
bool mk4_can_air_throw(const Fighter *fighter);
bool mk4_can_sidestep(const Fighter *fighter);
void mk4_sidestep(Fighter *fighter);
void mk4_air_throw_generated(Fighter *fighter, FighterFn owner_script, FighterFn victim_script);
void mk4_air_throw_generated_with_moves(
    Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn owner_script, FighterFn victim_script);
void mk4_script_air_throw_release(Fighter *fighter);
void mk4_script_air_getup(Fighter *fighter);
void scorpion_generated_reaction(Fighter *fighter);
void raiden_generated_reaction(Fighter *fighter);
void sonya_generated_reaction(Fighter *fighter);
void kitana_generated_reaction(Fighter *fighter);
void mk4_walk(Fighter *fighter);
void mk4_jump(Fighter *fighter);
void mk4_land(Fighter *fighter);
void mk4_return_to_stance(Fighter *fighter);

void mk4_air_throw_frames(Fighter *fighter, FighterFn owner_script, FighterFn victim_script);
void mk4_mmove_air_throw_release(Fighter *fighter);

void kai_handstand_reaction(Fighter *fighter);
void kai_weapon_reaction(Fighter *fighter);
void liu_kang_entry_retail_script_004e9de8(Fighter *fighter);
void cage_weapon_reaction(Fighter *fighter);
#endif
