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

bool cage_combo_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
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
void cage_ready(Fighter *fighter);
void cage_script_air_finish(Fighter *fighter);
void cage_script_air_release(Fighter *fighter);
void cage_script_cpu_kick_tail(Fighter *fighter);
void cage_script_fan_cycle(Fighter *fighter);
void cage_script_fan_tail(Fighter *fighter);
void cage_script_fan_victim_end(Fighter *fighter);
void cage_script_fan_victory(Fighter *fighter);
void cage_script_fatality_1_signal(Fighter *fighter);
void cage_script_fatality_2_continue(Fighter *fighter);
void cage_script_finish(Fighter *fighter);
void cage_script_kick_finish(Fighter *fighter);
void cage_script_lk_throw_return(Fighter *fighter);
void cage_script_lp_return(Fighter *fighter);
void cage_script_opponent_reaction(Fighter *fighter, int reaction);
void cage_script_powered_punch_finish_tail(Fighter *fighter);
void cage_script_powered_punch_tail(Fighter *fighter);
void cage_script_roundhouse_start(Fighter *fighter);
void cage_script_spike_tail(Fighter *fighter);
void cage_start_script(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void cage_sweep(Fighter *fighter);
void cage_uppercut(Fighter *fighter);
void cage_entry_retail_script_004de2f8(Fighter *fighter);
void cage_entry_retail_script_004de798(Fighter *fighter);
void cage_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *cage_retail_script_004e0138_repeat_phase(void);
void cage_entry_retail_script_004e0148(Fighter *fighter);
void cage_entry_retail_script_004e0158(Fighter *fighter);
void cage_entry_retail_script_004e0188(Fighter *fighter);
void cage_entry_retail_script_004e01a0(Fighter *fighter);
void cage_entry_retail_script_004e12a0(Fighter *fighter);
void cage_entry_retail_script_004e12b0(Fighter *fighter);
void cage_entry_retail_script_004e5870(Fighter *fighter);
void cage_entry_retail_script_004e5ba8(Fighter *fighter);
void cage_entry_retail_script_004e5c70(Fighter *fighter);
void cage_entry_retail_script_004e5ca0(Fighter *fighter);
void cage_entry_retail_script_004e5d90(Fighter *fighter);
void cage_entry_retail_script_004e5e28(Fighter *fighter);
void cage_entry_retail_script_004e5e50(Fighter *fighter);
void cage_entry_retail_script_004e61f8(Fighter *fighter);
void cage_entry_retail_script_004e62a0(Fighter *fighter);
void cage_entry_retail_script_004e62f0(Fighter *fighter);
void cage_entry_retail_script_004e63d8(Fighter *fighter);
void cage_entry_retail_script_004e6500(Fighter *fighter);
void cage_entry_retail_script_004e6658(Fighter *fighter);
void cage_entry_retail_script_004e6710(Fighter *fighter);
void cage_entry_retail_script_004e6798(Fighter *fighter);
void cage_entry_retail_script_004e6800(Fighter *fighter);
void cage_entry_retail_script_004e6860(Fighter *fighter);
void cage_entry_retail_script_004e6878(Fighter *fighter);
void cage_entry_retail_script_004e6950(Fighter *fighter);
void cage_entry_retail_script_004e80a8(Fighter *fighter);
void cage_entry_retail_script_004e80c8(Fighter *fighter);
void cage_entry_retail_script_004e80d0(Fighter *fighter);
void cage_entry_retail_script_004e80f4(Fighter *fighter);
void cage_entry_retail_script_004e9910(Fighter *fighter);
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
void cage_fan_fatality(Fighter *fighter);
void cage_spike_fatality(Fighter *fighter);
#endif
