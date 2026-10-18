#ifndef MK4_JAX_PRIVATE_H
#define MK4_JAX_PRIVATE_H

#include "game.h"
#include "native.h"
#include "math_util.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_block jax_s_block
#define s_crouch jax_s_crouch
#define s_high_kick jax_s_high_kick
#define s_idle jax_s_idle
#define s_jab jax_s_jab
#define s_jax_jab_record jax_s_jax_jab_record
#define s_sweep jax_s_sweep
#define s_throw jax_s_throw
#define s_thrown jax_s_thrown
#define s_uppercut jax_s_uppercut
#define s_walk jax_s_walk

enum {
  JAX_EVENT_FATALITY_1_SIGNAL = 1,
  JAX_EVENT_FATALITY_1_FINISH,
  JAX_EVENT_FATALITY_2_TASK,
  JAX_EVENT_FATALITY_2_FINISH,
  JAX_EVENT_JAB_CONTACT,
  JAX_EVENT_JAB_RECOVERY
};

void jax_block(Fighter *fighter);
bool jax_contact_event(BattleEvent *event, Fighter *owner, Fighter *opponent);
void jax_crouch(Fighter *fighter);
void jax_fan_tail(Fighter *fighter);
void jax_high_kick(Fighter *fighter);
void jax_idle(Fighter *fighter);
void jax_jab(Fighter *fighter);
void jax_low_kick(Fighter *fighter);
void jax_lp_throw(Fighter *fighter);
void jax_press_air_hp(Fighter *fighter);
void jax_press_air_kick(Fighter *fighter);
void jax_press_air_lp(Fighter *fighter);
void jax_press_hk(Fighter *fighter);
void jax_press_hp(Fighter *fighter);
void jax_press_lk(Fighter *fighter);
void jax_press_lp(Fighter *fighter);
void jax_press_sweep(Fighter *fighter);
void jax_press_uppercut(Fighter *fighter);
void jax_ready(Fighter *fighter);
void jax_script_air_finish(Fighter *fighter);
void jax_script_air_throw_ground_release(Fighter *fighter);
void jax_script_air_throw_release(Fighter *fighter);
void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type);
void jax_script_current_reaction(Fighter *fighter, int reaction);
void jax_script_fatality_1_finish(Fighter *fighter);
void jax_script_fatality_1_scene_signal(Fighter *fighter);
void jax_script_fatality_2_scene_finish(Fighter *fighter);
void jax_script_fatality_2_tail(Fighter *fighter);
void jax_script_fatality_2_task(Fighter *fighter, int task_type);
void jax_script_finish(Fighter *fighter);
void jax_script_jab_contact(Fighter *fighter);
void jax_script_jab_followup(Fighter *fighter);
void jax_script_lk_finish(Fighter *fighter);
void jax_script_lk_return(Fighter *fighter);
void jax_script_opponent_reaction(Fighter *fighter, int reaction);
void jax_script_roundhouse_start(Fighter *fighter);
void jax_script_roundhouse_tail(Fighter *fighter);
void jax_start(Fighter *fighter, int state, const MoveDef *move, FighterFn script);
void jax_sweep(Fighter *fighter);
void jax_uppercut(Fighter *fighter);
void jax_walk(Fighter *fighter);
void jax_weapon_action(Fighter *fighter);
void jax_entry_retail_script_004de2f8(Fighter *fighter);
void jax_entry_retail_script_004de358(Fighter *fighter);
void jax_entry_retail_script_004de798(Fighter *fighter);
void jax_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *jax_retail_script_004e0138_repeat_phase(void);
void jax_entry_retail_script_004e0148(Fighter *fighter);
void jax_entry_retail_script_004e0188(Fighter *fighter);
void jax_entry_retail_script_004e01a0(Fighter *fighter);
void jax_entry_retail_script_004e1390(Fighter *fighter);
void jax_entry_retail_script_004e13a0(Fighter *fighter);
void jax_entry_retail_script_004e13b0(Fighter *fighter);
void jax_entry_retail_script_004e13c0(Fighter *fighter);
void jax_entry_retail_script_004e13d0(Fighter *fighter);
void jax_entry_retail_script_004e13e0(Fighter *fighter);
void jax_entry_retail_script_004e18f0(Fighter *fighter);
void jax_entry_retail_script_004e1958(Fighter *fighter);
void jax_entry_retail_script_004e5a80(Fighter *fighter);
void jax_entry_retail_script_004e5a98(Fighter *fighter);
void jax_entry_retail_script_004e5b10(Fighter *fighter);
void jax_entry_retail_script_004e5b38(Fighter *fighter);
void jax_entry_retail_script_004e5ca0(Fighter *fighter);
void jax_entry_retail_script_004e5d90(Fighter *fighter);
void jax_entry_retail_script_004e5dd8(Fighter *fighter);
void jax_entry_retail_script_004e5df8(Fighter *fighter);
void jax_entry_retail_script_004e5e28(Fighter *fighter);
void jax_entry_retail_script_004e5e50(Fighter *fighter);
void jax_entry_retail_script_004e61f8(Fighter *fighter);
void jax_entry_retail_script_004e62a0(Fighter *fighter);
void jax_entry_retail_script_004e62c0(Fighter *fighter);
void jax_entry_retail_script_004e62d8(Fighter *fighter);
void jax_entry_retail_script_004e62f0(Fighter *fighter);
void jax_entry_retail_script_004e63d8(Fighter *fighter);
void jax_entry_retail_script_004e6500(Fighter *fighter);
void jax_entry_retail_script_004e6658(Fighter *fighter);
void jax_entry_retail_script_004e6710(Fighter *fighter);
void jax_entry_retail_script_004e6798(Fighter *fighter);
void jax_entry_retail_script_004e6800(Fighter *fighter);
void jax_entry_retail_script_004e6890(Fighter *fighter);
void jax_entry_retail_script_004e68a8(Fighter *fighter);
void jax_entry_retail_script_004e6950(Fighter *fighter);
void jax_entry_retail_script_004e8150(Fighter *fighter);
void jax_entry_retail_script_004e8174(Fighter *fighter);
void jax_entry_retail_script_004e9910(Fighter *fighter);
void jax_entry_retail_script_004ed090(Fighter *fighter);
extern const MoveDef jax_s_block;
extern const MoveDef jax_s_crouch;
extern const MoveDef jax_s_high_kick;
extern const MoveDef jax_s_idle;
extern const MoveDef jax_s_jab;
extern const AttackRecord jax_s_jax_jab_record;
extern const MoveDef jax_s_sweep;
extern const MoveDef jax_s_throw;
extern const MoveDef jax_s_thrown;
extern const MoveDef jax_s_uppercut;
extern const MoveDef jax_s_walk;

extern const ButtonTable jax_diagonal_air;
void jax_press_air_hk(Fighter *fighter);
void jax_press_air_lk(Fighter *fighter);
void jax_close_hp(Fighter *fighter);
void jax_combo_hp(Fighter *fighter);
void jax_close_hk(Fighter *fighter);
void jax_press_air_ground_hp(Fighter *fighter);
void jax_script_close_hp_tail(Fighter *fighter);
void jax_script_combo_hp_tail(Fighter *fighter);
void jax_script_close_hk_tail(Fighter *fighter);
void jax_entry_retail_script_004e61e0(Fighter *fighter);
void jax_entry_retail_script_004e69b0(Fighter *fighter);
void jax_entry_retail_script_004e6190(Fighter *fighter);
void jax_lk_throw(Fighter *fighter);
#endif
