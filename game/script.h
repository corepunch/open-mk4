#ifndef MK4_SCRIPT_H
#define MK4_SCRIPT_H

#include "game.h"

/* Primitives for generated ordinary C fibers. No bytecode or address dispatch. */
void mk4_script_bind(Fighter *fighter, int clip, int count, int first);
void mk4_script_bind_victim(Fighter *fighter, int clip, int count, int first);
void mk4_script_inherit(Fighter *fighter, int clip, int count, int sample);
void mk4_script_pair_advance(Fighter *fighter, int count);
void mk4_script_current(Fighter *fighter);
void mk4_script_advance(Fighter *fighter, int count, int step);
void mk4_script_finish_clip(Fighter *fighter, int step);
void mk4_script_stop(Fighter *fighter);
void mk4_script_avoid_clear(Fighter *fighter);
void mk4_script_avoid_set(Fighter *fighter);
void mk4_script_reverse_return(Fighter *fighter);
void mk4_script_attack_return(Fighter *fighter);
void mk4_script_indexed_sound(Fighter *fighter, int selector);

void mk4_script_ready(Fighter *fighter);
void mk4_script_land(Fighter *fighter);
void mk4_script_thrown_ready(Fighter *fighter);
void mk4_script_impact(Fighter *fighter);
void mk4_script_impact_callback(Fighter *fighter);
void mk4_script_disarm(Fighter *fighter);
void mk4_script_damage(Fighter *fighter, int damage);
void mk4_script_pair_adjust(Fighter *fighter, int amount);
void mk4_script_context(Fighter *fighter, int selector);
void mk4_script_roster_sound(Fighter *fighter, int selector);
void mk4_script_draw_voice(Fighter *fighter);
void mk4_script_scene_active(Fighter *fighter);
void mk4_script_pair_to(Fighter *owner, int sample);
void mk4_script_save_motion(Fighter *fighter);
void mk4_script_restore_motion(Fighter *fighter);
unsigned mk4_script_jab_gate(Fighter *fighter);
void mk4_script_jab_contact(Fighter *fighter);
void mk4_script_stop_body(Fighter *fighter);

#endif
