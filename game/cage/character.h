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
#define s_jab cage_s_jab
#define s_kick cage_s_kick
#define s_powered_punch_script cage_s_powered_punch_script
#define s_sweep cage_s_sweep
#define s_throw cage_s_throw
#define s_uppercut cage_s_uppercut

enum {
  CAGE_EVENT_CPU_KICK_RECOVERY = 1,
  CAGE_EVENT_POWERED_RECOVERY,
  CAGE_EVENT_FINISH_RECOVERY,
  CAGE_EVENT_SCENE_LOOP,
  CAGE_EVENT_FATALITY_1 = 17,
  CAGE_EVENT_FATALITY_2
};

void cage_lp_throw(Fighter *fighter);
void cage_press_air_hk(Fighter *fighter);
void cage_press_air_hp(Fighter *fighter);
void cage_press_air_lk(Fighter *fighter);
void cage_press_air_lp(Fighter *fighter);
void cage_press_crouching_hp(Fighter *fighter);
void cage_press_crouching_kick(Fighter *fighter);
void cage_press_high_kick(Fighter *fighter);
void cage_press_high_punch(Fighter *fighter);
void cage_press_low_kick(Fighter *fighter);
void cage_press_low_punch(Fighter *fighter);
void cage_script_air_finish(Fighter *fighter);
void cage_script_cpu_kick_tail(Fighter *fighter);
void cage_script_fatality_1_signal(Fighter *fighter);
void cage_script_fatality_2_continue(Fighter *fighter);
void cage_script_lk_throw_return(Fighter *fighter);
void cage_script_lp_return(Fighter *fighter);
void cage_script_opponent_reaction(Fighter *fighter, int reaction);
void cage_script_powered_punch_tail(Fighter *fighter);
void cage_script_roundhouse_start(Fighter *fighter);
void cage_script_spike_tail(Fighter *fighter);
void cage_uppercut(Fighter *fighter);
void cage_entry_retail_script_004de2f8(Fighter *fighter);
void cage_entry_retail_script_004de798(Fighter *fighter);
void cage_entry_retail_script_004e12a0(Fighter *fighter);
void cage_entry_retail_script_004e12b0(Fighter *fighter);
void cage_entry_retail_script_004e5870(Fighter *fighter);
void cage_entry_retail_script_004e5ba8(Fighter *fighter);
void cage_entry_retail_script_004e61f8(Fighter *fighter);
void cage_entry_retail_script_004e62a0(Fighter *fighter);
void cage_entry_retail_script_004e63d8(Fighter *fighter);
void cage_entry_retail_script_004e6500(Fighter *fighter);
void cage_entry_retail_script_004e6658(Fighter *fighter);
void cage_entry_retail_script_004e6710(Fighter *fighter);
void cage_entry_retail_script_004e6800(Fighter *fighter);
void cage_entry_retail_script_004e6860(Fighter *fighter);
void cage_entry_retail_script_004e6878(Fighter *fighter);
void cage_entry_retail_script_004e6950(Fighter *fighter);
void cage_entry_retail_script_004e80a8(Fighter *fighter);
void cage_entry_retail_script_004e9718(Fighter *fighter);
void cage_shadow_victim(Fighter *fighter);
void cage_entry_retail_script_004e80d0(Fighter *fighter);
void cage_entry_retail_script_004ea040(Fighter *fighter);
void cage_entry_retail_script_004e84b8(Fighter *fighter);
void cage_power_land(Fighter *fighter);
extern const MoveDef cage_s_cpu_kick_script;
extern const MoveDef cage_s_jab;
extern const MoveDef cage_s_kick;
extern const MoveDef cage_s_powered_punch_script;
extern const MoveDef cage_s_sweep;
extern const MoveDef cage_s_throw;
extern const MoveDef cage_s_uppercut;

extern const ButtonTable cage_diagonal_air;
void cage_press_air_hk(Fighter *fighter);
void cage_press_air_lk(Fighter *fighter);
void cage_close_hp(Fighter *fighter);
void cage_combo_hp(Fighter *fighter);
void cage_close_hk(Fighter *fighter);
void cage_press_air_ground_hp(Fighter *fighter);
void cage_script_close_hp_tail(Fighter *fighter);
void cage_script_combo_hp_tail(Fighter *fighter);
void cage_script_close_hk_tail(Fighter *fighter);
void cage_entry_retail_script_004e61e0(Fighter *fighter);
void cage_entry_retail_script_004e69b0(Fighter *fighter);
void cage_entry_retail_script_004e6190(Fighter *fighter);
void cage_lk_throw(Fighter *fighter);
void cage_spike_fatality(Fighter *fighter);
void cage_roundhouse(Fighter *fighter);
void cage_script_roundhouse_finish(Fighter *fighter);
void cage_powered_punch(Fighter *fighter);
void cage_cpu_kick(Fighter *fighter);
void cage_groin_punch(Fighter *fighter);
bool cage_can_groin_punch(const Fighter *fighter);
void cage_entry_retail_script_004e9f78(Fighter *fighter);
void cage_entry_retail_script_004e6270(Fighter *fighter);
void cage_high_orb(Fighter *fighter);
void cage_low_orb(Fighter *fighter);
void cage_orb_spawn(Fighter *fighter, int task_type);
void cage_entry_retail_script_004ed0d8(Fighter *fighter);
extern const uint32_t cage_weapon_descriptor[];
void cage_weapon_action(Fighter *fighter);
void cage_weapon_upper_contact(Fighter *fighter);
void cage_weapon_back_lp_contact(Fighter *fighter);
void cage_script_draw_equip(Fighter *fighter);
void cage_weapon_getup_back(Fighter *fighter);
void cage_weapon_getup_roll(Fighter *fighter);
void cage_weapon_reaction(Fighter *fighter);
void cage_entry_retail_script_004e62c0(Fighter *fighter);
void cage_entry_retail_script_004e62d8(Fighter *fighter);
void cage_entry_retail_script_004e66d0(Fighter *fighter);
void cage_entry_retail_script_004e66f0(Fighter *fighter);
void cage_entry_retail_script_004ec4f0(Fighter *fighter);
void cage_entry_retail_script_004ec7f0(Fighter *fighter);
void cage_entry_retail_script_004e8cc0(Fighter *fighter);
void cage_entry_retail_script_004e8d38(Fighter *fighter);
void cage_fatality_2(Fighter *fighter);
void cage_second_charge(Fighter *fighter, int task_type);
void cage_second_flash(Fighter *fighter, int task_type);
void cage_second_repeated_uppercuts(Fighter *fighter);
void cage_second_victim_pause(Fighter *fighter);
void cage_second_victim_finish(Fighter *fighter);
void cage_second_victim_hit(Fighter *fighter);
void cage_entry_retail_script_004e5880(Fighter *fighter);
void cage_entry_retail_script_004e58d8(Fighter *fighter);
void cage_entry_retail_script_004e5900(Fighter *fighter);
void cage_entry_retail_script_004e5b38(Fighter *fighter);
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
void cage_entry_retail_script_004e12c0(Fighter *fighter);
void cage_entry_retail_script_004e12d0(Fighter *fighter);
void cage_entry_retail_script_004e12e0(Fighter *fighter);
void cage_entry_retail_script_004e12f0(Fighter *fighter);
void cage_entry_retail_script_004e1300(Fighter *fighter);
void cage_entry_retail_script_004e1310(Fighter *fighter);
void cage_entry_retail_script_004e1320(Fighter *fighter);
void cage_entry_retail_script_004e1330(Fighter *fighter);
void cage_entry_retail_script_004e1340(Fighter *fighter);
void cage_entry_retail_script_004e1350(Fighter *fighter);
void cage_entry_retail_script_004e1360(Fighter *fighter);
void cage_entry_retail_script_004e1370(Fighter *fighter);
void cage_entry_retail_script_004e1380(Fighter *fighter);
#endif
