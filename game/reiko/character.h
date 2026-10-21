#include "retail_native.h"
#ifndef MK4_REIKO_PRIVATE_H
#define MK4_REIKO_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Finisher IDs are unique across the roster (coordinator allocation). */
enum {
  REIKO_FINISH_1 = 27,
  REIKO_FINISH_2
};

/* Internal interfaces; helpers and tables stay local to their owning move. */

void reiko_script_lp_end(Fighter *fighter);
void reiko_script_lp_return(Fighter *fighter);
void reiko_script_opponent_reaction(Fighter *fighter, int reaction);

extern const ButtonTable reiko_diagonal_air;
void reiko_weapon_action(Fighter *fighter);
void reiko_script_weapon_draw_finish(Fighter *fighter);
void reiko_flip_kick(Fighter *fighter);
bool reiko_can_flip_kick(const Fighter *fighter);
void reiko_star(Fighter *fighter);
void reiko_star_spawn(Fighter *fighter, int task_type);
bool reiko_can_star(const Fighter *fighter);
void reiko_script_star_victim_tail(Fighter *fighter);
void reiko_orbit(Fighter *fighter);
bool reiko_can_orbit(const Fighter *fighter);
void reiko_first_head_59(Fighter *fighter);
void reiko_first_head_83(Fighter *fighter);
void reiko_first_left_arm_56(Fighter *fighter);
void reiko_first_left_arm_land(Fighter *fighter);
void reiko_first_piece_end(Fighter *fighter);
void reiko_first_right_arm_44(Fighter *fighter);
void reiko_first_right_arm_land(Fighter *fighter);
void reiko_first_tear(Fighter *fighter);
void reiko_first_torso_29(Fighter *fighter);
void reiko_first_torso_end(Fighter *fighter);
void reiko_first_victim_118(Fighter *fighter);
void reiko_first_victim_165(Fighter *fighter);
void reiko_first_victory(Fighter *fighter);
void reiko_first_fatality(Fighter *fighter);
bool reiko_can_first_fatality(const Fighter *fighter);
void reiko_second_fatality(Fighter *fighter);
void reiko_second_stars(Fighter *fighter, int task_type);
void reiko_second_owner_park(Fighter *fighter);
void reiko_second_victim_fall(Fighter *fighter);
void reiko_second_victim_end(Fighter *fighter);
#endif
