#include "retail_native.h"
#ifndef MK4_JAX_PRIVATE_H
#define MK4_JAX_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_thrown jax_s_thrown

void jax_lp_slam_first(Fighter *fighter);
void jax_lp_slam_second(Fighter *fighter);
void jax_lp_slam_third(Fighter *fighter);
void jax_lp_throw_return(Fighter *fighter);
void jax_throw_grunt(void);
bool jax_can_fatality_1(const Fighter *fighter);
void jax_fatality_1(Fighter *fighter);
void jax_fatality_1_first_arm(Fighter *fighter);
void jax_fatality_1_grab(Fighter *fighter);
void jax_fatality_1_second_arm(Fighter *fighter);
void jax_fatality_1_victory(Fighter *fighter);
void jax_fatality_2(Fighter *fighter);
void jax_fatality_2_approach(Fighter *fighter);
void jax_fatality_2_finish(Fighter *fighter);
void jax_fatality_2_flash(Fighter *fighter, int task_type);
void jax_fatality_2_sparks(Fighter *fighter, int task_type);
void jax_fatality_2_victim_pause(Fighter *fighter);
void jax_fatality_task_end(Fighter *fighter);
void jax_script_peer_reaction(Fighter *fighter, int reaction);
void jax_ground_pound(Fighter *fighter);
bool jax_can_dash_punch(const Fighter *fighter);
void jax_dash_punch(Fighter *fighter);
void jax_dash_punch_contact(Fighter *fighter);
bool jax_can_missile(const Fighter *fighter);
void jax_missile(Fighter *fighter);
void jax_missile_spawn(Fighter *fighter, int task_type);
void jax_fan_tail(Fighter *fighter);
void jax_script_opponent_reaction(Fighter *fighter, int reaction);

extern const ButtonTable jax_diagonal_air;
#endif
