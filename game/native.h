#ifndef MK4_NATIVE_H
#define MK4_NATIVE_H
#include "game.h"

/* Assemble a bounded native move once on entry. Rows run through the same
 * fixed-frame runner as generated scripts; callbacks never count elapsed time. */
void native_begin(Fighter *fighter, int first, int last, FighterFn end);
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

typedef struct NativeGetupDef {
  int clip, count, crouch_clip, crouch_count;
  q16 speed, crouch_speed;
} NativeGetupDef;

void mk4_mmove_getup(Fighter *fighter, const NativeGetupDef *profile);
void mk4_mmove_finish_getup(Fighter *fighter, const NativeGetupDef *profile);
void weapon_idle(Fighter *fighter);
void weapon_walk(Fighter *fighter);
void weapon_crouch(Fighter *fighter);
void weapon_block(Fighter *fighter);
void weapon_script_contact_tail(Fighter *fighter, const AttackRecord *record, int queries);
bool weapon_can_draw(const Fighter *fighter);
void weapon_axe_strike(Fighter *fighter, int index, const NativeAxeProfile *profile);
void weapon_axe_reaction(Fighter *victim, bool followup, FighterFn script);

typedef struct GroundAttack {
  MoveDef move;
  AttackRecord record;
  int active, end_miss, end_contact;
} GroundAttack;

typedef struct NativeGroundProfile {
  const GroundAttack *attacks;
  const ButtonTable *followups[3];
  FighterFn ready;
  int combo_offset, combo_last;
} NativeGroundProfile;

typedef struct AirAttack {
  MoveDef move;
  AttackRecord record;
  int extend, hold, retract;
} AirAttack;

typedef struct NativeAirProfile {
  const AirAttack *attacks;
  const ButtonTable *followup, *ground_followup;
  FighterFn land, straight_miss;
} NativeAirProfile;

bool native_grounded(const Fighter *fighter);
void native_uppercut_tail(Fighter *fighter, FighterFn contact);
void native_uppercut_default(Fighter *fighter);
void native_ground_tail(Fighter *fighter, const NativeGroundProfile *profile);
void native_ground_next(Fighter *fighter, const NativeGroundProfile *profile);
void native_air_tail(Fighter *fighter, const NativeAirProfile *profile, int index, int elapsed);
void native_air_cleanup(Fighter *fighter);
void native_air_straight_land(Fighter *fighter);
void native_air_straight_tail(Fighter *fighter, const NativeAirProfile *profile);
#endif
