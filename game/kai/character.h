#include "retail_native.h"
#ifndef MK4_KAI_PRIVATE_H
#define MK4_KAI_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include <string.h>
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_air_attack kai_s_air_attack
#define s_low_kick kai_s_low_kick

enum {
  KAI_STAGE_EVENT_PLAY_CLIP = 1,
  KAI_STAGE_EVENT_SETTLE
};

enum {
  KAI_EVENT_FATALITY_1 = 23,
  KAI_EVENT_FATALITY_2
};

void kai_input_478df0(Fighter *fighter);
void kai_input_47bd10(Fighter *fighter);
void kai_input_47c520(Fighter *fighter);
void kai_aerial_punch_contact(Fighter *fighter);
void kai_lunge_kick_contact(Fighter *fighter);
void kai_script_contact_b10_continuation(Fighter *fighter);
void kai_script_current_reaction(Fighter *fighter, int reaction);
void kai_script_lp_extended_finish(Fighter *fighter);
void kai_script_lp_short_finish(Fighter *fighter);
void kai_script_opponent_reaction(Fighter *fighter, int reaction);
void kai_script_stage_event(Fighter *fighter);

extern const ButtonTable kai_diagonal_air;
bool kai_can_handstand(const Fighter *fighter);
void kai_handstand_spin_begin(Fighter *fighter);
void kai_handstand_rear_contact(Fighter *fighter);
void kai_handstand_victim_tail(Fighter *fighter);
void kai_handstand_crouch_return(Fighter *fighter);
void kai_handstand_reaction(Fighter *fighter);
void kai_weapon_back_contact(Fighter *fighter);
void kai_weapon_victim_getup(Fighter *fighter);
void kai_weapon_reaction(Fighter *fighter);
bool kai_can_rising_skull(const Fighter *fighter);
bool kai_can_ground_skull(const Fighter *fighter);
void kai_rising_skull(Fighter *fighter);
void kai_rising_skull_spawn(Fighter *fighter);
void kai_rising_skull_finish(Fighter *fighter);
void kai_ground_skull(Fighter *fighter);
void kai_ground_skull_spawn(Fighter *fighter);
bool kai_can_first_fatality(const Fighter *fighter);
void kai_fatality_1(Fighter *fighter);
void kai_first_owner_grab(Fighter *fighter);
void kai_first_owner_spin(Fighter *fighter);
void kai_first_owner_tear(Fighter *fighter);
void kai_first_victim_71(Fighter *fighter);
void kai_first_victim_turn(Fighter *fighter);
void kai_first_victim_sever(Fighter *fighter);
void kai_first_torso_203(Fighter *fighter);
void kai_first_torso_impact(Fighter *fighter);
void kai_first_task_end(Fighter *fighter);
void kai_fatality_2(Fighter *fighter);
void kai_second_event(Fighter *fighter, int selector);
void kai_second_child_walk(Fighter *fighter);
void kai_second_hover(Fighter *fighter, int task_type);
void kai_second_skull(Fighter *fighter, int task_type);
void kai_second_victim_fall(Fighter *fighter);
void kai_second_victim(Fighter *fighter);
#endif
