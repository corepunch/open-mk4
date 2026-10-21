#include "retail_native.h"
#ifndef MK4_FUJIN_PRIVATE_H
#define MK4_FUJIN_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */

void fujin_enter_throw(Fighter *fighter);
void fujin_script_current_reaction(Fighter *fighter, int reaction);
void fujin_script_opponent_reaction(Fighter *fighter, int reaction);

extern const ButtonTable fujin_diagonal_air;
bool fujin_can_dive(const Fighter *fighter);
void fujin_dive(Fighter *fighter);
void fujin_dive_contact(Fighter *fighter);
void fujin_dive_land(Fighter *fighter);
void fujin_bow_finish(Fighter *fighter);
void fujin_bow_followup_next(Fighter *fighter);
void fujin_bolt_victim(Fighter *fighter);
bool fujin_can_wind(const Fighter *fighter);
bool fujin_can_wind_slam(const Fighter *fighter);
void fujin_wind(Fighter *fighter);
void fujin_wind_slam(Fighter *fighter);
void fujin_wind_victim(Fighter *fighter);
void fujin_wind_slam_victim(Fighter *fighter);
void fujin_wind_recovery(Fighter *fighter);
void fujin_triple_kick(Fighter *fighter);
void fujin_triple_contact(Fighter *fighter);
void fujin_triple_fast(Fighter *fighter);
void fujin_triple_victim(Fighter *fighter);
bool fujin_can_wind_grab(const Fighter *fighter);
void fujin_wind_grab(Fighter *fighter);
void fujin_grab_control_start(Fighter *fighter);
void fujin_grab_victim(Fighter *fighter);
void fujin_second_fatality(Fighter *fighter);
void fujin_second_victim(Fighter *fighter);
void fujin_second_burst(Fighter *fighter);
void fujin_second_finish(Fighter *fighter);
void fujin_second_event(Fighter *fighter, int selector);
void fujin_second_blood(Fighter *fighter, int task_type);
bool fujin_can_first_fatality(const Fighter *fighter);
void fujin_first_fatality(Fighter *fighter);
void fujin_first_loop_start(Fighter *fighter);
void fujin_first_cycle(Fighter *fighter);
void fujin_first_shoot(Fighter *fighter);
void fujin_first_fire_end(Fighter *fighter);
#endif
