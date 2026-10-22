#ifndef MK4_NATIVE_H
#define MK4_NATIVE_H
#include "game.h"

/* Assemble bounded native playback once on entry. Original script words run
 * in the VM; only native routines build these engine playback rows. */
void native_begin(Fighter *fighter, int first, int last, FighterFn end);
void native_address_end(Fighter *fighter, uint32_t address);
void native_animation_table(Fighter *fighter, uint32_t table);
void native_script_end(Fighter *fighter, uint32_t entry, FighterFn finish);
void native_frame(Fighter *fighter, int frame, int pose, FighterFn action);
void native_play(Fighter *fighter, bool immediate);
void native_play_rows(Fighter *fighter, int sample, int step, FighterFn end);
void native_query_rows(Fighter *fighter, const AttackRecord *record, int frames, int pose_step, FighterFn end);
void native_await(Fighter *fighter, FighterTest condition, FighterFn end);
void native_finish_clip(Fighter *fighter, FighterFn end);
void native_scene_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end);
void native_hold(Fighter *fighter, int frames, FighterFn end);
void native_motion_begin(Fighter *fighter, int frames, FighterFn end);
void native_motion_frame(Fighter *fighter, int frame, FighterAIFn movement, float distance, FighterFn action);
void native_motion_play(Fighter *fighter);
void native_motion_step(Fighter *fighter);
void native_motion_bind(Fighter *fighter, FighterFn binding, const mframe_t *frames, int count);
void native_motion_stop(Fighter *fighter);
void native_motion_speed(Fighter *fighter, float speed);
void native_schedule_speed(Fighter *fighter, q16 speed, int delay, int frames);

typedef struct NativeAxeProfile {
  FighterFn first_hit, followup_hit;
} NativeAxeProfile;

void mk4_mmove_getup(Fighter *fighter, uint32_t record);
void mk4_mmove_finish_getup(Fighter *fighter, uint32_t record);
void weapon_idle(Fighter *fighter);
void weapon_walk(Fighter *fighter);
void weapon_crouch(Fighter *fighter);
void weapon_block(Fighter *fighter);
void weapon_script_contact_tail(Fighter *fighter, const AttackRecord *record, int queries);
bool weapon_can_draw(const Fighter *fighter);
void weapon_axe_strike(Fighter *fighter, int index, const NativeAxeProfile *profile);
void weapon_axe_reaction(Fighter *victim, bool followup, ScriptContinuation script);

bool native_grounded(const Fighter *fighter);
void native_uppercut_default(Fighter *fighter);
#endif
