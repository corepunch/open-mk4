#ifndef MK4_NATIVE_H
#define MK4_NATIVE_H
#include "game.h"

/* Assemble a bounded native move once on entry. Rows run through the same
 * fixed-frame runner as generated scripts; callbacks never count elapsed time. */
void native_begin(Fighter *fighter, int first, int last, FighterFn end);
void native_frame(Fighter *fighter, int frame, int pose, FighterFn action);
void native_play(Fighter *fighter, bool immediate);
void native_hold(Fighter *fighter, int frames, FighterFn end);
void native_motion_begin(Fighter *fighter, int frames, FighterFn end);
void native_motion_frame(Fighter *fighter, int frame, FighterAIFn movement, float distance, FighterFn action);
void native_motion_play(Fighter *fighter);
void native_motion_stop(Fighter *fighter);
void native_motion_speed(Fighter *fighter, float speed);
void native_schedule_speed(Fighter *fighter, q16 speed, int delay, int frames);
#endif
