#include "retail_native.h"
#ifndef MK4_NOOB_SAIBOT_PRIVATE_H
#define MK4_NOOB_SAIBOT_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */

void noob_saibot_command_498b70(Fighter *fighter);
void noob_saibot_command_498e70(Fighter *fighter);
void noob_saibot_press_air_hk(Fighter *fighter);
void noob_saibot_press_air_hp(Fighter *fighter);
void noob_saibot_press_air_lk(Fighter *fighter);
void noob_saibot_press_air_lp(Fighter *fighter);
void noob_saibot_script_air_finish(Fighter *fighter);
void noob_saibot_script_lp_end(Fighter *fighter);
void noob_saibot_script_lp_return(Fighter *fighter);
void noob_saibot_script_opponent_reaction(Fighter *fighter, int reaction);
void noob_saibot_shadow_install_physics(Fighter *fighter);
void noob_saibot_shadow_low(Fighter *fighter);
void noob_saibot_shadow_task_low(Fighter *fighter, int task_type);
void noob_saibot_shadow_task_up(Fighter *fighter, int task_type);
void noob_saibot_shadow_up(Fighter *fighter);
void noob_saibot_entry_retail_script_004de664(Fighter *fighter);
void noob_saibot_entry_retail_script_004de668(Fighter *fighter);
void noob_saibot_entry_retail_script_004e9b40(Fighter *fighter);
void noob_saibot_entry_retail_script_004e9bd0(Fighter *fighter);
void noob_saibot_entry_retail_script_004ed268(Fighter *fighter);
void noob_saibot_entry_retail_script_004ed2c0(Fighter *fighter);

extern const ButtonTable noob_saibot_diagonal_air;
void noob_saibot_press_air_hk(Fighter *fighter);
void noob_saibot_press_air_lk(Fighter *fighter);
void noob_saibot_weapon_action(Fighter *fighter);
void noob_saibot_script_weapon_draw_finish(Fighter *fighter);
void noob_saibot_entry_retail_script_004e62c0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e62d8(Fighter *fighter);
void noob_saibot_entry_retail_script_004ea040(Fighter *fighter);
void noob_saibot_entry_retail_script_004ea0b8(Fighter *fighter);
void noob_saibot_entry_retail_script_004e82b8(Fighter *fighter);
void noob_saibot_entry_retail_script_004e82e0(Fighter *fighter);
void noob_saibot_entry_retail_script_004e8310(Fighter *fighter);
void noob_saibot_entry_retail_script_004e8818(Fighter *fighter);
void noob_saibot_teleport(Fighter *fighter);
void noob_saibot_script_teleport_vanish(Fighter *fighter);
void noob_saibot_script_teleport_slam_fall(Fighter *fighter);
void noob_saibot_script_teleport_slam_drop(Fighter *fighter);
void noob_saibot_script_teleport_land(Fighter *fighter);
void noob_saibot_flip_kick(Fighter *fighter);
void noob_saibot_entry_retail_script_004ed038(Fighter *fighter);
void noob_saibot_star(Fighter *fighter);
void noob_saibot_star_spawn(Fighter *fighter, int task_type);
#endif
