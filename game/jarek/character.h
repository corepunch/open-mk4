#include "retail_native.h"
#ifndef MK4_JAREK_PRIVATE_H
#define MK4_JAREK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_low_kick jarek_s_low_kick

/* Character-owned finisher ids, outside the shared FINISH_* range. */
enum {
  JAREK_EVENT_FATALITY_1 = 25,
  JAREK_EVENT_FATALITY_2
};

void jarek_fatality_contact_hold(Fighter *fighter);
void jarek_script_current_reaction(Fighter *fighter, int reaction);
void jarek_script_low_kick_avoid(Fighter *fighter);
void jarek_script_low_kick_clear(Fighter *fighter);
void jarek_script_lp_end(Fighter *fighter);
void jarek_script_lp_return(Fighter *fighter);
void jarek_script_opponent_reaction(Fighter *fighter, int reaction);
void jarek_script_weapon_draw_finish(Fighter *fighter);
void jarek_cannonball(Fighter *fighter);
void jarek_fatality_1(Fighter *fighter);
void jarek_first_heart(Fighter *fighter);
void jarek_first_victim_rip(Fighter *fighter);
void jarek_first_victim_impact(Fighter *fighter);
void jarek_first_victim_end(Fighter *fighter);
void jarek_first_victim_done(Fighter *fighter);
void jarek_fatality_2(Fighter *fighter);
void jarek_second_glow(Fighter *fighter);
void jarek_second_handoff(Fighter *fighter);
void jarek_second_head_fall(Fighter *fighter, int task_type);
void jarek_second_victim_hold(Fighter *fighter);
void jarek_quake(Fighter *fighter);
void jarek_blade(Fighter *fighter);
void jarek_blade_spawn(Fighter *fighter, int task_type);
void jarek_quake_tail(Fighter *fighter);
void jarek_upward_ball(Fighter *fighter);
void jarek_ball_land(Fighter *fighter);
void jarek_weapon_action(Fighter *fighter);
void jarek_entry_retail_script_004de664(Fighter *fighter);
extern const MoveDef jarek_s_low_kick;

void jarek_weapon_upper_contact(Fighter *fighter);
void jarek_weapon_reaction(Fighter *fighter);
#endif
