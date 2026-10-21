#include "retail_native.h"
#ifndef MK4_QUAN_CHI_PRIVATE_H
#define MK4_QUAN_CHI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define press_sweep retail_press_sweep

void quan_chi_script_current_reaction(Fighter *fighter, int reaction);
void quan_chi_script_opponent_reaction(Fighter *fighter, int reaction);

extern const ButtonTable quan_chi_diagonal_air;
void quan_chi_skull(Fighter *fighter);
void quan_chi_script_skull_release(Fighter *fighter);
void quan_chi_flip_kick(Fighter *fighter);
void quan_chi_script_flip_kick_tail(Fighter *fighter);
void quan_chi_teleport_stomp(Fighter *fighter);
void quan_chi_script_stomp_victim(Fighter *fighter);
void quan_chi_weapon_steal(Fighter *fighter);
void quan_chi_first_fatality(Fighter *fighter);
void quan_chi_second_fatality(Fighter *fighter);
bool quan_chi_can_first_fatality(const Fighter *fighter);
void quan_chi_fatality_cue_40(Fighter *fighter);
void quan_chi_fatality_leg(Fighter *fighter);
void quan_chi_fatality_hit_179(Fighter *fighter);
void quan_chi_fatality_hit_241(Fighter *fighter);
void quan_chi_fatality_beat(Fighter *fighter);

/* Roster-unique finisher identity for the first fatality scene (44bcd0).
 * The second fatality (43be30) runs the victim roster's own scene and id. */
enum {
  QUAN_CHI_FINISH_LEG = 31
};
#endif
