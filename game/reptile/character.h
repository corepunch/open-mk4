#ifndef MK4_REPTILE_PRIVATE_H
#define MK4_REPTILE_PRIVATE_H

#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "script.h"
#include "script.h"

/* Internal interfaces; helpers and tables stay local to their owning move. */
#define s_jab reptile_s_jab
#define s_kick reptile_s_kick
#define s_lp_thrown reptile_s_lp_thrown
#define s_sweep reptile_s_sweep
#define s_throw reptile_s_throw
#define s_uppercut reptile_s_uppercut

enum {
  REPTILE_EVENT_VISIBILITY = 1,
  REPTILE_EVENT_TASK_76
};

bool reptile_clone_contact(BattleEvent *event, Fighter *fighter, Fighter *opponent);
bool reptile_clone_pose(BattleEvent *event, Fighter *fighter, Fighter *opponent);
void reptile_clone_step(Projectile *projectile);
void reptile_lp_throw(Fighter *fighter);
void reptile_press_air_hk(Fighter *fighter);
void reptile_press_air_hp(Fighter *fighter);
void reptile_press_air_lk(Fighter *fighter);
void reptile_press_air_lp(Fighter *fighter);
void reptile_press_kick(Fighter *fighter);
void reptile_press_low_kick(Fighter *fighter);
void reptile_press_low_punch(Fighter *fighter);
void reptile_press_punch(Fighter *fighter);
void reptile_press_sweep(Fighter *fighter);
void reptile_press_uppercut(Fighter *fighter);
void reptile_script_47a6d0(Fighter *fighter);
void reptile_script_47b010(Fighter *fighter);
void reptile_script_air_finish(Fighter *fighter);
void reptile_script_air_release(Fighter *fighter);
extern const AttackRecord reptile_script_contact;
void reptile_script_current_reaction(Fighter *fighter, int reaction);
void reptile_script_fan_cycle(Fighter *fighter);
void reptile_script_fan_tail(Fighter *fighter);
void reptile_script_fan_victim_end(Fighter *fighter);
void reptile_script_finish(Fighter *fighter);
void reptile_script_lk_return(Fighter *fighter);
void reptile_script_lp_end(Fighter *fighter);
void reptile_script_lp_return(Fighter *fighter);
void reptile_script_opponent_reaction(Fighter *fighter, int reaction);
void reptile_script_reaction_side_getup(Fighter *fighter);
void reptile_script_roundhouse_start(Fighter *fighter);
void reptile_script_spike_tail(Fighter *fighter);
void reptile_script_visibility_end(Fighter *fighter);
void reptile_script_visibility_extend(Fighter *fighter);
void reptile_script_visibility_phase(Fighter *fighter);
void reptile_script_visibility_start(Fighter *fighter);
void reptile_start(Fighter *fighter, StateId state, const MoveDef *move, FighterFn script);
void reptile_sweep(Fighter *fighter);
void reptile_task_76_spawn(Fighter *fighter, int task_type);
void reptile_entry_retail_script_004de2f8(Fighter *fighter);
void reptile_entry_retail_script_004de640(Fighter *fighter);
void reptile_entry_retail_script_004de664(Fighter *fighter);
void reptile_entry_retail_script_004de668(Fighter *fighter);
void reptile_entry_retail_script_004de6b8(Fighter *fighter);
void reptile_entry_retail_script_004de798(Fighter *fighter);
void reptile_entry_retail_script_004e0118(Fighter *fighter);
const mmove_t *reptile_retail_script_004e0138_repeat_phase(void);
void reptile_entry_retail_script_004e0148(Fighter *fighter);
void reptile_entry_retail_script_004e0188(Fighter *fighter);
void reptile_entry_retail_script_004e01a0(Fighter *fighter);
void reptile_entry_retail_script_004e5ca0(Fighter *fighter);
void reptile_entry_retail_script_004e5d90(Fighter *fighter);
void reptile_entry_retail_script_004e5e28(Fighter *fighter);
void reptile_entry_retail_script_004e5e50(Fighter *fighter);
void reptile_entry_retail_script_004e61f8(Fighter *fighter);
void reptile_entry_retail_script_004e62a0(Fighter *fighter);
void reptile_entry_retail_script_004e62f0(Fighter *fighter);
void reptile_entry_retail_script_004e63d8(Fighter *fighter);
void reptile_entry_retail_script_004e6500(Fighter *fighter);
void reptile_entry_retail_script_004e6658(Fighter *fighter);
void reptile_entry_retail_script_004e6710(Fighter *fighter);
void reptile_entry_retail_script_004e6798(Fighter *fighter);
void reptile_entry_retail_script_004e6800(Fighter *fighter);
void reptile_entry_retail_script_004e6860(Fighter *fighter);
void reptile_entry_retail_script_004e6878(Fighter *fighter);
void reptile_entry_retail_script_004e6950(Fighter *fighter);
void reptile_entry_retail_script_004e8128(Fighter *fighter);
void reptile_entry_retail_script_004e8148(Fighter *fighter);
void reptile_entry_retail_script_004e8174(Fighter *fighter);
void reptile_entry_retail_script_004e8178(Fighter *fighter);
void reptile_entry_retail_script_004e9620(Fighter *fighter);
void reptile_entry_retail_script_004e9910(Fighter *fighter);
void reptile_entry_retail_script_004e9b40(Fighter *fighter);
void reptile_entry_retail_script_004e9bd0(Fighter *fighter);
void reptile_entry_retail_script_004ed1e8(Fighter *fighter);
extern const MoveDef reptile_s_jab;
extern const MoveDef reptile_s_kick;
extern const MoveDef reptile_s_lp_thrown;
extern const MoveDef reptile_s_sweep;
extern const MoveDef reptile_s_throw;
extern const MoveDef reptile_s_uppercut;

extern const ButtonTable reptile_diagonal_air;
void reptile_press_air_hk(Fighter *fighter);
void reptile_press_air_lk(Fighter *fighter);
void reptile_close_hp(Fighter *fighter);
void reptile_combo_hp(Fighter *fighter);
void reptile_close_hk(Fighter *fighter);
void reptile_press_air_ground_hp(Fighter *fighter);
void reptile_script_close_hp_tail(Fighter *fighter);
void reptile_script_combo_hp_tail(Fighter *fighter);
void reptile_script_close_hk_tail(Fighter *fighter);
void reptile_entry_retail_script_004e61e0(Fighter *fighter);
void reptile_entry_retail_script_004e69b0(Fighter *fighter);
void reptile_entry_retail_script_004e6190(Fighter *fighter);
#endif
