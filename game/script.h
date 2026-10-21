#ifndef MK4_SCRIPT_H
#define MK4_SCRIPT_H

#include "game.h"

/* Primitives for generated frame programs. No bytecode or address dispatch. */
bool mk4_mmove_pair_step(Fighter *fighter);
void mk4_mmove_bind(Fighter *fighter, int clip, int count, int first);
void mk4_script_inherit(Fighter *fighter, int clip, int count, int sample);
void mk4_mmove_inherit_current(Fighter *fighter, int count, int sample);
void mk4_mmove_blend_bind(Fighter *fighter, int clip, int count);
void mk4_mmove_blend_to(Fighter *fighter, int clip, int count, int target, q16 step, q16 first, FighterFn end);
void mk4_script_stop(Fighter *fighter);
void mk4_script_avoid_clear(Fighter *fighter);
void mk4_script_avoid_set(Fighter *fighter);
void mk4_script_attack_return(Fighter *fighter);
void mk4_script_indexed_sound(Fighter *fighter, int selector);
void mk4_script_guard_block_sound_26(Fighter *fighter);
void mk4_script_guard_block_sound_28(Fighter *fighter);

void mk4_mmove_land(Fighter *fighter);
void mk4_script_thrown_ready(Fighter *fighter);
void mk4_script_impact(Fighter *fighter);
void mk4_script_impact_callback(Fighter *fighter);
void mk4_script_disarm(Fighter *fighter);
void mk4_script_disarm_self(Fighter *fighter);
void mk4_script_touchable(Fighter *fighter);
void mk4_script_damage(Fighter *fighter, int damage);
void mk4_script_pair_adjust(Fighter *fighter, int amount);
void mk4_script_context(Fighter *fighter, int selector);
void mk4_script_roster_sound(Fighter *fighter, int selector);
void mk4_script_draw_voice(Fighter *fighter);
void mk4_script_scene_active(Fighter *fighter);
void mk4_mmove_pair_back_step(Fighter *owner);
void mk4_mmove_throw_start(Fighter *fighter, const MoveDef *owner_move, const MoveDef *victim_move, FighterFn script);
Fighter *mk4_script_release_pair(Fighter *fighter);
void mk4_mmove_pair_victim(Fighter *fighter, const MoveDef *move);
void mk4_mmove_carry_victim(Fighter *fighter, const MoveDef *move);
void mk4_script_pair_cleanup(Fighter *fighter);
void mk4_mmove_thrown_finish(Fighter *fighter);
void mk4_mmove_lk_release(Fighter *fighter, int reaction);
void mk4_mmove_lk_owner_return(Fighter *fighter, FighterFn end);
void mk4_mmove_lk_owner_ready(Fighter *fighter);
void mk4_mmove_side_getup(Fighter *fighter);
void mk4_script_save_motion(Fighter *fighter);
void mk4_script_restore_motion(Fighter *fighter);
unsigned mk4_script_jab_gate(Fighter *fighter);
void mk4_script_jab_contact(Fighter *fighter);
void mk4_script_stop_body(Fighter *fighter);

void mk4_script_context_sound(Fighter *fighter, int selector);
void mk4_script_reaction_motion(Fighter *fighter, int left, int right, int up, int gravity);
void mk4_script_landing_motion(Fighter *fighter);
void mk4_script_scene_pose(Fighter *fighter, int clip, int sample);
void mk4_script_fan_cycle(Fighter *fighter);
void mk4_mmove_fan_victim_end(Fighter *fighter);
void mk4_native_fan_rise(Fighter *fighter);
void mk4_native_spike_tail(Fighter *fighter, FighterFn end);
void mk4_script_spike_tail(Fighter *fighter);
void mk4_script_shake(Fighter *fighter, int selector);
void mk4_script_effect(Fighter *fighter, int selector);
void mk4_script_projectile_step(Projectile *object, const AttackRecord *record, bool hits_crouching);
Projectile *mk4_script_projectile_spawn(Fighter *fighter, ProjectileFn step, const MeshAnimation *mesh, q16 forward, q16 y, int lifetime);
void mk4_script_schedule_motion(Fighter *fighter, int speed, int delay, int duration);
void mk4_mmove_weapon_tail(Fighter *fighter, const AttackRecord *record, int queries);
void mk4_script_scene_task(Fighter *fighter, int task_type);

void mk4_mmove_hold(Fighter *fighter);
void mk4_mmove_wait_grounded(Fighter *fighter);
void mk4_mmove_wait_land(Fighter *fighter, FighterFn next);

void mk4_mmove_lk_blend_return(Fighter *fighter);
#endif
