#include "retail_native.h"
#ifndef MK4_GORO_PRIVATE_H
#define MK4_GORO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */

void goro_script_current_reaction(Fighter *fighter, int reaction);
void goro_script_lp_phase_repeat(Fighter *fighter);
void goro_script_opponent_reaction(Fighter *fighter, int reaction);

void goro_begin_special(Fighter *fighter, StateId state, const MoveDef *move, int token);
void goro_fireball(Fighter *fighter);
void goro_fireball_spawn(Fighter *fighter, int task_type);
void goro_kick_victim(Fighter *victim);
void goro_leap_stomp(Fighter *fighter);
void goro_punch_victim(Fighter *victim);
void goro_quake_resting_weapons(void);
void goro_quake_stomp(Fighter *fighter);
void goro_quake_victim(Fighter *victim);
void goro_script_kick_tail(Fighter *fighter);
void goro_script_leap_continue(Fighter *fighter);
void goro_script_punch_tail(Fighter *fighter);
void goro_script_quake_land(Fighter *fighter);
void goro_script_stomp_return(Fighter *fighter);
void goro_script_stomp_victim_hold(Fighter *victim);
void goro_script_uppercut_victim_slide(Fighter *victim);
void goro_special_kick(Fighter *fighter);
void goro_special_punch(Fighter *fighter);
void goro_special_uppercut(Fighter *fighter);
void goro_stomp_hold_separation(Fighter *fighter);
void goro_stomp_victim(Fighter *victim);
void goro_uppercut_victim(Fighter *victim);
void goro_victim_finish_getup(Fighter *victim);
void goro_victim_getup(Fighter *victim);

#endif
