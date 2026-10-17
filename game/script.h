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

#endif
