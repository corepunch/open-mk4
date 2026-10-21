#include "retail_native.h"
#ifndef MK4_SHINNOK_PRIVATE_H
#define MK4_SHINNOK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block shinnok_s_block
#define s_crouch shinnok_s_crouch
#define s_throw shinnok_s_throw

void shinnok_entry_retail_script_004de2f8(Fighter *fighter);
void shinnok_entry_retail_script_004de798(Fighter *fighter);
void shinnok_entry_retail_script_004e61f8(Fighter *fighter);
void shinnok_entry_retail_script_004e63d8(Fighter *fighter);
void shinnok_entry_retail_script_004e6500(Fighter *fighter);
void shinnok_entry_retail_script_004e6658(Fighter *fighter);
void shinnok_entry_retail_script_004e6710(Fighter *fighter);
extern const MoveDef shinnok_s_block;
extern const MoveDef shinnok_s_crouch;
extern const MoveDef shinnok_s_throw;
void shinnok_fan_tail(Fighter *fighter);
void shinnok_fan_victory(Fighter *fighter);
void shinnok_fatality_contact_hold(Fighter *fighter);
void shinnok_lp_throw(Fighter *fighter);
void shinnok_press_air_hp(Fighter *fighter);
void shinnok_press_air_kick(Fighter *fighter);
void shinnok_press_air_lp(Fighter *fighter);
void shinnok_press_kick(Fighter *fighter);
void shinnok_press_low_kick(Fighter *fighter);
void shinnok_press_low_punch(Fighter *fighter);
void shinnok_press_punch(Fighter *fighter);
void shinnok_script_air_finish(Fighter *fighter);
void shinnok_script_current_reaction(Fighter *fighter, int reaction);
void shinnok_script_opponent_reaction(Fighter *fighter, int reaction);
void shinnok_weapon_back_hp(Fighter *fighter);
void shinnok_weapon_back_lp(Fighter *fighter);
void shinnok_weapon_equip(Fighter *fighter);
void shinnok_weapon_hp(Fighter *fighter);
void shinnok_weapon_lp(Fighter *fighter);

extern const ButtonTable shinnok_diagonal_air;
void shinnok_press_air_hk(Fighter *fighter);
void shinnok_press_air_lk(Fighter *fighter);
void shinnok_close_hp(Fighter *fighter);
void shinnok_combo_hp(Fighter *fighter);
void shinnok_close_hk(Fighter *fighter);
void shinnok_press_air_ground_hp(Fighter *fighter);
void shinnok_script_close_hp_tail(Fighter *fighter);
void shinnok_script_combo_hp_tail(Fighter *fighter);
void shinnok_script_close_hk_tail(Fighter *fighter);
void shinnok_entry_retail_script_004e61e0(Fighter *fighter);
void shinnok_entry_retail_script_004e69b0(Fighter *fighter);
void shinnok_entry_retail_script_004e6190(Fighter *fighter);
void shinnok_lk_throw(Fighter *fighter);
void shinnok_entry_retail_script_004e6270(Fighter *fighter);
void shinnok_weapon_action(Fighter *fighter);
void shinnok_script_draw_equip(Fighter *fighter);
void shinnok_weapon_back_getup(Fighter *fighter);
void shinnok_weapon_crouch_return(Fighter *fighter);
void shinnok_weapon_sweep_getup(Fighter *fighter);
void shinnok_weapon_sweep_finish(Fighter *fighter);
void shinnok_weapon_reaction(Fighter *fighter);
void shinnok_entry_retail_script_004e62c0(Fighter *fighter);
void shinnok_entry_retail_script_004e62d8(Fighter *fighter);
void shinnok_entry_retail_script_004e66d0(Fighter *fighter);
void shinnok_entry_retail_script_004e66f0(Fighter *fighter);
void shinnok_entry_retail_script_004e8d08(Fighter *fighter);
void shinnok_entry_retail_script_004e9460(Fighter *fighter);
void shinnok_entry_retail_script_004e9498(Fighter *fighter);
void shinnok_entry_retail_script_004ec8b0(Fighter *fighter);
bool shinnok_can_morph(const Fighter *fighter);
void shinnok_morph_scorpion(Fighter *fighter);
void shinnok_morph_raiden(Fighter *fighter);
void shinnok_morph_sonya(Fighter *fighter);
void shinnok_morph_liu_kang(Fighter *fighter);
void shinnok_morph_sub_zero(Fighter *fighter);
void shinnok_morph_fujin(Fighter *fighter);
void shinnok_morph_reiko(Fighter *fighter);
void shinnok_morph_quan_chi(Fighter *fighter);
void shinnok_morph_tanya(Fighter *fighter);
void shinnok_morph_reptile(Fighter *fighter);
void shinnok_morph_kai(Fighter *fighter);
void shinnok_morph_jarek(Fighter *fighter);
void shinnok_morph_jax(Fighter *fighter);
void shinnok_morph_cage(Fighter *fighter);

enum {
  SHINNOK_FINISH_1 = 21,
  SHINNOK_FINISH_2
};

enum {
  SHINNOK_FATALITY_OWNER_READY = 1,
  SHINNOK_FATALITY_VICTIM_DONE = 2,
  SHINNOK_FATALITY_HAND_DONE = 4,
  SHINNOK_FATALITY_DONE = 7
};

void shinnok_fatality_1(Fighter *fighter);
void shinnok_fatality_2(Fighter *fighter);
void shinnok_fatality_begin(Fighter *fighter, unsigned done);
void shinnok_fatality_vanish_cue(Fighter *fighter);
void shinnok_fatality_return(Fighter *fighter);
void shinnok_fatality_maybe_complete(Fighter *owner);
void shinnok_entry_retail_script_004e2ef0(Fighter *fighter);
void shinnok_entry_retail_script_004e2f00(Fighter *fighter);
void shinnok_entry_retail_script_004e2f10(Fighter *fighter);
void shinnok_entry_retail_script_004e2f20(Fighter *fighter);
void shinnok_entry_retail_script_004e2f30(Fighter *fighter);
void shinnok_entry_retail_script_004e2f40(Fighter *fighter);
void shinnok_entry_retail_script_004e2f78(Fighter *fighter);
void shinnok_entry_retail_script_004e2f88(Fighter *fighter);
void shinnok_entry_retail_script_004e2fb8(Fighter *fighter);
void shinnok_entry_retail_script_004e0088(Fighter *fighter);
void shinnok_entry_retail_script_004e2fc8(Fighter *fighter);
void shinnok_entry_retail_script_004e2fd8(Fighter *fighter);
void shinnok_entry_retail_script_004e00b8(Fighter *fighter);
void shinnok_entry_retail_script_004e00e0(Fighter *fighter);
void shinnok_entry_retail_script_004e00f8(Fighter *fighter);
void shinnok_entry_retail_script_004ea270(Fighter *fighter);
#endif
