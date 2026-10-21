#include "retail_native.h"
#ifndef MK4_CAGE_PRIVATE_H
#define MK4_CAGE_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_cpu_kick_script cage_s_cpu_kick_script
#define s_powered_punch_script cage_s_powered_punch_script

enum {
  CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  CAGE_EVENT_POWERED_RECOVERY,
  CAGE_EVENT_FINISH_RECOVERY,
  CAGE_EVENT_SCENE_LOOP,
  CAGE_EVENT_FATALITY_1 = 17,
  CAGE_EVENT_FATALITY_2
};

void cage_script_cpu_kick_tail(Fighter *fighter);
void cage_script_fatality_1_signal(Fighter *fighter);
void cage_script_fatality_2_continue(Fighter *fighter);
void cage_script_lp_return(Fighter *fighter);
void cage_script_opponent_reaction(Fighter *fighter, int reaction);
void cage_script_powered_punch_tail(Fighter *fighter);
void cage_shadow_victim(Fighter *fighter);
void cage_power_land(Fighter *fighter);
extern const MoveDef cage_s_cpu_kick_script;
extern const MoveDef cage_s_powered_punch_script;

extern const ButtonTable cage_diagonal_air;
void cage_powered_punch(Fighter *fighter);
void cage_cpu_kick(Fighter *fighter);
void cage_groin_punch(Fighter *fighter);
bool cage_can_groin_punch(const Fighter *fighter);
void cage_high_orb(Fighter *fighter);
void cage_low_orb(Fighter *fighter);
void cage_orb_spawn(Fighter *fighter, int task_type);
void cage_weapon_action(Fighter *fighter);
void cage_weapon_upper_contact(Fighter *fighter);
void cage_weapon_back_lp_contact(Fighter *fighter);
void cage_script_draw_equip(Fighter *fighter);
void cage_weapon_getup_back(Fighter *fighter);
void cage_weapon_getup_roll(Fighter *fighter);
void cage_weapon_reaction(Fighter *fighter);
void cage_fatality_2(Fighter *fighter);
void cage_second_charge(Fighter *fighter, int task_type);
void cage_second_flash(Fighter *fighter, int task_type);
void cage_second_repeated_uppercuts(Fighter *fighter);
void cage_second_victim_pause(Fighter *fighter);
void cage_second_victim_finish(Fighter *fighter);
void cage_second_victim_hit(Fighter *fighter);
void cage_fatality_1(Fighter *fighter);
void cage_first_victim_59(Fighter *fighter);
void cage_first_victim_131(Fighter *fighter);
void cage_first_victim_148(Fighter *fighter);
void cage_first_victim_166(Fighter *fighter);
void cage_first_victim_185(Fighter *fighter);
void cage_first_victim_206(Fighter *fighter);
void cage_first_sever(Fighter *fighter);
void cage_first_torso_half(Fighter *fighter);
void cage_first_torso_123(Fighter *fighter);
void cage_first_torso_finish(Fighter *fighter);
void cage_first_task_end(Fighter *fighter);
#endif
