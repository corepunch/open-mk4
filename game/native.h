#ifndef MK4_NATIVE_H
#define MK4_NATIVE_H
#include "game.h"

/* Assemble a bounded native move once on entry. Rows run through the same
 * fixed-frame runner as generated scripts; callbacks never count elapsed time. */
void native_kick_contact_phase(Fighter *fighter, FighterFn query, FighterFn recovery);
void native_narrow_kick_contact(Fighter *fighter, FighterFn recovery);
void native_begin(Fighter *fighter, int first, int last, FighterFn end);
void native_frame(Fighter *fighter, int frame, int pose, FighterFn action);
void native_play(Fighter *fighter, bool immediate);
/* Returning bindings retain their VM continuation, but playback and actions
 * are dispatched by mframe rows rather than a native tick loop. */
void native_wait(Fighter *fighter);
void native_wait_loop(Fighter *fighter);
void native_delay(Fighter *fighter, int frames);
void native_advance(Fighter *fighter, int count, int step);
void native_play_to(Fighter *fighter, int sample, int step);
void native_calls(Fighter *fighter, int frames, int pose_step, FighterFn action);
void native_queries(Fighter *fighter, const AttackRecord *record, int frames, int pose_step);
void native_until(Fighter *fighter, FighterTest condition);
void native_repeat(Fighter *fighter, const mmove_t *move, int repetitions, FighterFn end);
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
  int fast, combo_offset, combo_last, round_pre, round_first, round_last;
  const AttackRecord *roundhouse_record;
} NativeGroundProfile;

typedef struct NativeHPProfile {
  const AttackRecord *record;
  const MoveDef *first, *second;
  FighterFn ready;
} NativeHPProfile;

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
void native_hp_start(Fighter *fighter, const NativeHPProfile *profile);
void native_air_tail(Fighter *fighter, const NativeAirProfile *profile, int index, int elapsed);
void native_air_cleanup(Fighter *fighter);
void native_air_straight_land(Fighter *fighter);
void native_air_straight_tail(Fighter *fighter, const NativeAirProfile *profile);
#endif
