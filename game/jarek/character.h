#ifndef MK4_JAREK_PRIVATE_H
#define MK4_JAREK_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block jarek_s_block
#define s_crouch jarek_s_crouch
#define s_high_kick jarek_s_high_kick
#define s_idle jarek_s_idle
#define s_jab jarek_s_jab
#define s_low_kick jarek_s_low_kick
#define s_sweep jarek_s_sweep
#define s_throw jarek_s_throw
#define s_uppercut jarek_s_uppercut
#define s_walk jarek_s_walk

enum {
  JAREK_CONTACT_RECOVERY = 1,
  JAREK_FAN_SCENE = 16,
  JAREK_FAN_FINISH
};

void jarek_ai_projectile_spawn(Fighter *fighter, int task_type);
void jarek_block(Fighter *fighter);
void jarek_crouch(Fighter *fighter);
void jarek_fan_fatality(Fighter *fighter);
void jarek_fan_tail(Fighter *fighter);
void jarek_fan_victory(Fighter *fighter);
void jarek_fatality_contact_hold(Fighter *fighter);
void jarek_high_kick(Fighter *fighter);
void jarek_idle(Fighter *fighter);
void jarek_input_target(Fighter *fighter, Button button);
void jarek_jab(Fighter *fighter);
void jarek_low_kick(Fighter *fighter);
void jarek_lp_throw(Fighter *fighter);
void jarek_press_air_hp(Fighter *fighter);
void jarek_press_air_kick(Fighter *fighter);
void jarek_press_air_hk(Fighter *fighter);
void jarek_press_air_lk(Fighter *fighter);
void jarek_press_air_lp(Fighter *fighter);
void jarek_press_kick(Fighter *fighter);
void jarek_press_low_kick(Fighter *fighter);
void jarek_press_low_punch(Fighter *fighter);
void jarek_press_punch(Fighter *fighter);
void jarek_press_sweep(Fighter *fighter);
void jarek_press_uppercut(Fighter *fighter);
void jarek_ready(Fighter *fighter);
void jarek_script_47b600(Fighter *fighter);
void jarek_script_air_finish(Fighter *fighter);
void jarek_script_contact_24(Fighter *fighter);
void jarek_script_contact_25(Fighter *fighter);
void jarek_script_contact_26(Fighter *fighter);
void jarek_script_current_reaction(Fighter *fighter, int reaction);
void jarek_script_fan_finish(Fighter *fighter);
void jarek_script_fan_scene(Fighter *fighter);
void jarek_script_fan_victim_scene(Fighter *fighter);
void jarek_script_kick_finish(Fighter *fighter);
void jarek_script_lk_return(Fighter *fighter);
void jarek_script_low_kick_avoid(Fighter *fighter);
void jarek_script_low_kick_clear(Fighter *fighter);
void jarek_script_lp_end(Fighter *fighter);
void jarek_script_lp_return(Fighter *fighter);
void jarek_script_lp_select(Fighter *fighter);
void jarek_script_opponent_reaction(Fighter *fighter, int reaction);
void jarek_script_roundhouse_finish(Fighter *fighter);
void jarek_script_roundhouse_start(Fighter *fighter);
void jarek_script_sweep_finish(Fighter *fighter);
void jarek_script_uppercut_finish(Fighter *fighter);
void jarek_script_weapon_draw_finish(Fighter *fighter);
extern const AttackRecord jarek_selector_25_record;
void jarek_spike_fatality(Fighter *fighter);
void jarek_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
void jarek_sweep(Fighter *fighter);
void jarek_uppercut(Fighter *fighter);
void jarek_walk(Fighter *fighter);
void jarek_weapon_action(Fighter *fighter);
void jarek_entry_retail_script_004de2f8(Fighter *fighter);
void jarek_entry_retail_script_004de640(Fighter *fighter);
void jarek_entry_retail_script_004de664(Fighter *fighter);
void jarek_entry_retail_script_004de668(Fighter *fighter);
void jarek_entry_retail_script_004de6b8(Fighter *fighter);
void jarek_entry_retail_script_004de798(Fighter *fighter);
void jarek_entry_retail_script_004e0118(Fighter *fighter);
void jarek_entry_retail_script_004e0138(Fighter *fighter);
const mmove_t *jarek_retail_script_004e0138_repeat_phase(void);
void jarek_entry_retail_script_004e0148(Fighter *fighter);
void jarek_entry_retail_script_004e0158(Fighter *fighter);
void jarek_entry_retail_script_004e0188(Fighter *fighter);
void jarek_entry_retail_script_004e01a0(Fighter *fighter);
void jarek_entry_retail_script_004e01b0(Fighter *fighter);
void jarek_entry_retail_script_004e5d20(Fighter *fighter);
void jarek_entry_retail_script_004e5ca0(Fighter *fighter);
void jarek_entry_retail_script_004e5d90(Fighter *fighter);
void jarek_entry_retail_script_004e61f8(Fighter *fighter);
void jarek_entry_retail_script_004e62a0(Fighter *fighter);
void jarek_entry_retail_script_004e62c0(Fighter *fighter);
void jarek_entry_retail_script_004e62d8(Fighter *fighter);
void jarek_entry_retail_script_004e62f0(Fighter *fighter);
void jarek_entry_retail_script_004e63d8(Fighter *fighter);
void jarek_entry_retail_script_004e6500(Fighter *fighter);
void jarek_entry_retail_script_004e6658(Fighter *fighter);
void jarek_entry_retail_script_004e6710(Fighter *fighter);
void jarek_entry_retail_script_004e6798(Fighter *fighter);
void jarek_entry_retail_script_004e6800(Fighter *fighter);
void jarek_entry_retail_script_004e6860(Fighter *fighter);
void jarek_entry_retail_script_004e6878(Fighter *fighter);
void jarek_entry_retail_script_004e6950(Fighter *fighter);
void jarek_entry_retail_script_004e80a0(Fighter *fighter);
void jarek_entry_retail_script_004e80c8(Fighter *fighter);
void jarek_entry_retail_script_004e80f4(Fighter *fighter);
void jarek_entry_retail_script_004e81a8(Fighter *fighter);
void jarek_entry_retail_script_004ed118(Fighter *fighter);
void jarek_entry_retail_script_004ed190(Fighter *fighter);
extern const MoveDef jarek_s_block;
extern const MoveDef jarek_s_crouch;
extern const MoveDef jarek_s_high_kick;
extern const MoveDef jarek_s_idle;
extern const MoveDef jarek_s_jab;
extern const MoveDef jarek_s_low_kick;
extern const MoveDef jarek_s_sweep;
extern const MoveDef jarek_s_throw;
extern const MoveDef jarek_s_uppercut;
extern const MoveDef jarek_s_walk;

void jarek_close_hp(Fighter *fighter);
void jarek_combo_hp(Fighter *fighter);
void jarek_close_hk(Fighter *fighter);
void jarek_press_air_ground_hp(Fighter *fighter);
void jarek_script_close_hp_tail(Fighter *fighter);
void jarek_script_combo_hp_tail(Fighter *fighter);
void jarek_script_close_hk_tail(Fighter *fighter);
void jarek_entry_retail_script_004e61e0(Fighter *fighter);
void jarek_entry_retail_script_004e69b0(Fighter *fighter);
void jarek_entry_retail_script_004e6190(Fighter *fighter);
void jarek_lk_throw(Fighter *fighter);
void jarek_fan_fatality(Fighter *fighter);
void jarek_fan_tail(Fighter *fighter);
void jarek_spike_fatality(Fighter *fighter);
void jarek_roundhouse(Fighter *fighter);
void jarek_script_roundhouse_finish(Fighter *fighter);
void jarek_entry_retail_script_004e6270(Fighter *fighter);
#endif
