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

void shinnok_fan_tail(Fighter *fighter);
void shinnok_fan_victory(Fighter *fighter);
void shinnok_fatality_contact_hold(Fighter *fighter);
void shinnok_script_current_reaction(Fighter *fighter, int reaction);
void shinnok_script_opponent_reaction(Fighter *fighter, int reaction);
void shinnok_weapon_back_hp(Fighter *fighter);
void shinnok_weapon_back_lp(Fighter *fighter);
void shinnok_weapon_hp(Fighter *fighter);
void shinnok_weapon_lp(Fighter *fighter);

extern const ButtonTable shinnok_diagonal_air;
void shinnok_weapon_crouch_return(Fighter *fighter);
void shinnok_weapon_sweep_getup(Fighter *fighter);
void shinnok_weapon_sweep_finish(Fighter *fighter);
void shinnok_weapon_reaction(Fighter *fighter);
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
#endif
