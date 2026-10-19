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
#define s_block kai_s_block
#define s_crouch kai_s_crouch
#define s_high_kick kai_s_high_kick
#define s_idle kai_s_idle
#define s_jab kai_s_jab
#define s_low_kick kai_s_low_kick
#define s_sweep kai_s_sweep
#define s_throw kai_s_throw
#define s_uppercut kai_s_uppercut
#define s_walk kai_s_walk

enum {
  KAI_STAGE_EVENT_PLAY_CLIP = 1,
  KAI_STAGE_EVENT_SETTLE
};

void kai_block(Fighter *fighter);
void kai_crouch(Fighter *fighter);
void kai_high_kick(Fighter *fighter);
void kai_high_punch(Fighter *fighter);
void kai_idle(Fighter *fighter);
void kai_input_478df0(Fighter *fighter);
void kai_input_47bd10(Fighter *fighter);
void kai_input_47c520(Fighter *fighter);
void kai_aerial_punch_contact(Fighter *fighter);
void kai_lunge_kick_contact(Fighter *fighter);
void kai_jab(Fighter *fighter);
void kai_lp_throw(Fighter *fighter);
void kai_press_air_hk(Fighter *fighter);
void kai_press_air_hp(Fighter *fighter);
void kai_press_air_lk(Fighter *fighter);
void kai_press_air_lp(Fighter *fighter);
void kai_press_high_kick(Fighter *fighter);
void kai_press_high_punch(Fighter *fighter);
void kai_press_low_kick(Fighter *fighter);
void kai_press_low_punch(Fighter *fighter);
void kai_press_sweep(Fighter *fighter);
void kai_press_uppercut(Fighter *fighter);
void kai_ready(Fighter *fighter);
void kai_script_air_finish(Fighter *fighter);
void kai_script_contact_b10_continuation(Fighter *fighter);
void kai_script_current_reaction(Fighter *fighter, int reaction);
void kai_script_draw_equip(Fighter *fighter);
void kai_script_fan_cycle(Fighter *fighter);
void kai_script_fan_tail(Fighter *fighter);
void kai_script_fan_victim_end(Fighter *fighter);
void kai_script_fan_victory(Fighter *fighter);
void kai_script_finish(Fighter *fighter);
void kai_script_kick_finish(Fighter *fighter);
void kai_script_lk_finish(Fighter *fighter);
void kai_script_lk_return(Fighter *fighter);
void kai_script_lp_extended_finish(Fighter *fighter);
void kai_script_lp_select(Fighter *fighter);
void kai_script_lp_short_finish(Fighter *fighter);
void kai_script_opponent_reaction(Fighter *fighter, int reaction);
void kai_script_roundhouse_start(Fighter *fighter);
void kai_script_spike_tail(Fighter *fighter);
void kai_script_stage_event(Fighter *fighter);
void kai_sweep(Fighter *fighter);
void kai_uppercut(Fighter *fighter);
void kai_walk(Fighter *fighter);
void kai_entry_retail_script_004de2f8(Fighter *fighter);
void kai_entry_retail_script_004de358(Fighter *fighter);
void kai_entry_retail_script_004de610(Fighter *fighter);
void kai_entry_retail_script_004de6b8(Fighter *fighter);
void kai_entry_retail_script_004de798(Fighter *fighter);
void kai_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *kai_retail_script_004e0138_repeat_phase(void);
void kai_entry_retail_script_004e0148(Fighter *fighter);
void kai_entry_retail_script_004e0158(Fighter *fighter);
void kai_entry_retail_script_004e0188(Fighter *fighter);
void kai_entry_retail_script_004e01a0(Fighter *fighter);
void kai_entry_retail_script_004e6998(Fighter *fighter);
void kai_entry_retail_script_004e5ca0(Fighter *fighter);
void kai_entry_retail_script_004e5d90(Fighter *fighter);
void kai_entry_retail_script_004e61f8(Fighter *fighter);
void kai_entry_retail_script_004e62a0(Fighter *fighter);
void kai_entry_retail_script_004e62c0(Fighter *fighter);
void kai_entry_retail_script_004e62d8(Fighter *fighter);
void kai_entry_retail_script_004e62f0(Fighter *fighter);
void kai_entry_retail_script_004e63d8(Fighter *fighter);
void kai_entry_retail_script_004e6500(Fighter *fighter);
void kai_entry_retail_script_004e6658(Fighter *fighter);
void kai_entry_retail_script_004e6710(Fighter *fighter);
void kai_entry_retail_script_004e6798(Fighter *fighter);
void kai_entry_retail_script_004e6800(Fighter *fighter);
void kai_entry_retail_script_004e6890(Fighter *fighter);
void kai_entry_retail_script_004e68a8(Fighter *fighter);
void kai_entry_retail_script_004e6950(Fighter *fighter);
void kai_entry_retail_script_004e8210(Fighter *fighter);
void kai_entry_retail_script_004e8320(Fighter *fighter);
void kai_entry_retail_script_004e8358(Fighter *fighter);
void kai_entry_retail_script_004e8390(Fighter *fighter);
void kai_entry_retail_script_004e8870(Fighter *fighter);
void kai_entry_retail_script_004e88c8(Fighter *fighter);
extern const MoveDef kai_s_air_attack;
extern const MoveDef kai_s_block;
extern const MoveDef kai_s_crouch;
extern const MoveDef kai_s_high_kick;
extern const MoveDef kai_s_idle;
extern const MoveDef kai_s_jab;
extern const MoveDef kai_s_low_kick;
extern const MoveDef kai_s_sweep;
extern const MoveDef kai_s_throw;
extern const MoveDef kai_s_uppercut;
extern const MoveDef kai_s_walk;

extern const ButtonTable kai_diagonal_air;
void kai_press_air_hk(Fighter *fighter);
void kai_press_air_lk(Fighter *fighter);
void kai_close_hp(Fighter *fighter);
void kai_combo_hp(Fighter *fighter);
void kai_close_hk(Fighter *fighter);
void kai_press_air_ground_hp(Fighter *fighter);
void kai_script_close_hp_tail(Fighter *fighter);
void kai_script_combo_hp_tail(Fighter *fighter);
void kai_script_close_hk_tail(Fighter *fighter);
void kai_entry_retail_script_004e61e0(Fighter *fighter);
void kai_entry_retail_script_004e69b0(Fighter *fighter);
void kai_entry_retail_script_004e6190(Fighter *fighter);
void kai_lk_throw(Fighter *fighter);
void kai_fan_fatality(Fighter *fighter);
void kai_spike_fatality(Fighter *fighter);
void kai_roundhouse(Fighter *fighter);
void kai_script_roundhouse_finish(Fighter *fighter);
bool kai_can_handstand(const Fighter *fighter);
void kai_handstand_spin_begin(Fighter *fighter);
void kai_handstand_rear_contact(Fighter *fighter);
void kai_handstand_victim_tail(Fighter *fighter);
void kai_handstand_crouch_return(Fighter *fighter);
void kai_handstand_reaction(Fighter *fighter);
void kai_entry_retail_script_004e8338(Fighter *fighter);
void kai_entry_retail_script_004e83a0(Fighter *fighter);
void kai_entry_retail_script_004e83d8(Fighter *fighter);
void kai_entry_retail_script_004e83e8(Fighter *fighter);
void kai_entry_retail_script_004e9308(Fighter *fighter);
void kai_entry_retail_script_004e9360(Fighter *fighter);
void kai_entry_retail_script_004ea170(Fighter *fighter);
void kai_entry_retail_script_004ea1c0(Fighter *fighter);
extern const uint32_t kai_weapon_descriptor[];
void kai_draw_weapon(Fighter *fighter);
void kai_weapon_back_contact(Fighter *fighter);
void kai_weapon_face_peer(Fighter *fighter);
void kai_weapon_victim_getup(Fighter *fighter);
void kai_weapon_reaction(Fighter *fighter);
void kai_entry_retail_script_004e66d0(Fighter *fighter);
void kai_entry_retail_script_004e66f0(Fighter *fighter);
void kai_entry_retail_script_004ec5a8(Fighter *fighter);
void kai_entry_retail_script_004e8b78(Fighter *fighter);
void kai_entry_retail_script_004e8bc0(Fighter *fighter);
void kai_entry_retail_script_004e8f50(Fighter *fighter);
void kai_entry_retail_script_004e0138(Fighter *fighter);
#endif
