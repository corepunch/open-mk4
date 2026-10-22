#include "native.h"
#include "retail_vm.h"
#include "retail_native.h"
#include "math_util.h"
#include <string.h>

static void frame_pose(Fighter *fighter, float pose) {
  if (!fighter->mmove_hold) {
    fighter->sample_override = (int)pose;
  }
}

void native_begin(Fighter *fighter, int first, int last, FighterFn end) {
  require(first >= 0 && last >= first && last - first < NATIVE_FRAME_CAPACITY, "Native move exceeds frame capacity");
  NativeFrames *program = &fighter->native;
  program->script_end = (RetailScriptContinuation){0};
  program->native_end = (RetailNativeContinuation){0};
  int pose = fighter_sample(fighter);
  program->move = (mmove_t){.firstframe = first, .lastframe = last, .frame = program->frames, .endfunc = end, .samples = program->poses};
  memset(program->frames, 0, (size_t)(last - first + 1) * sizeof program->frames[0]);
  for (int i = 0; i <= last - first; ++i) {
    program->poses[i] = pose;
  }
}

static void native_script_resume(Fighter *fighter) {
  RetailScriptContinuation next = fighter->native.script_end;
  fighter->native.script_end = (RetailScriptContinuation){0};
  require(next.entry != 0, "Missing native script continuation");
  retail_vm_enter(fighter, next.entry, next.finish);
}

/* Install after native_begin (or a non-immediate native helper). The phase
 * owns the original address as data; no fighter-specific C thunk is needed. */
void native_script_end(Fighter *fighter, uint32_t entry, FighterFn finish) {
  require(entry >= RETAIL_SCRIPT_BEGIN && entry < RETAIL_SCRIPT_END && entry % 4 == 0 && finish != NULL, "Invalid native script continuation");
  fighter->native.native_end = (RetailNativeContinuation){0};
  fighter->native.script_end = (RetailScriptContinuation){entry, finish};
  fighter->native.move.endfunc = native_script_resume;
}

static void native_address_resume(Fighter *fighter) {
  RetailNativeContinuation next = fighter->native.native_end;
  fighter->native.native_end = (RetailNativeContinuation){0};
  require(next.address != 0, "Missing native address continuation");
  uint64_t continuation = fighter->continuation;
  require(retail_native_vm(fighter, next.address, RETAIL_TAIL, 0), "Unsupported native address continuation");
  require(fighter->continuation != continuation || fighter->current_mmove != &fighter->native.move, "Native continuation did not transfer ownership");
}

/* Install after native_begin or a non-immediate native helper. Native callback
 * addresses remain distinct from script entries and are never host pointers. */
void native_address_end(Fighter *fighter, uint32_t address) {
  require(address >= 0x400000 && address < RETAIL_SCRIPT_BEGIN, "Invalid native address continuation");
  fighter->native.script_end = (RetailScriptContinuation){0};
  fighter->native.native_end = (RetailNativeContinuation){address};
  fighter->native.move.endfunc = native_address_resume;
}

/* Native4287a0 reads a packed descriptor and a byte-address callback cell,
 * then starts ordinary forward native playback at pose zero. */
void native_animation_table(Fighter *fighter, uint32_t table) {
  const Mk4Image *image = retail_vm_image();
  uint32_t descriptor = mk4_image_u32(image, table) * 4;
  uint32_t callback = mk4_image_u32(image, table + 4);
  retail_vm_bind_animation(fighter, descriptor, 0);
  native_finish_clip(fighter, NULL);
  native_address_end(fighter, callback);
}

void native_frame(Fighter *fighter, int frame, int pose, FighterFn action) {
  NativeFrames *program = &fighter->native;
  int index = frame - program->move.firstframe;
  require(index >= 0 && frame <= program->move.lastframe && pose >= 0, "Invalid native action frame");
  /* The source pose changes at this row's boundary, before its action. The
   * next row displays that pose while waiting for its own fixed-frame visit. */
  program->frames[index] = (mframe_t){frame_pose, (float)pose, action};
  if (frame < program->move.lastframe) {
    program->poses[index + 1] = pose;
  }
}

static void native_install(Fighter *fighter) {
  fighter_mmove_phase(fighter, &fighter->native.move);
  fighter->sample_override = fighter->native.poses[0];
  if (fighter->native_immediate) {
    fighter->mmove_last_tick--;
    fighter_step_mmove(fighter);
  }
}

void native_play(Fighter *fighter, bool immediate) {
  fighter->native_immediate = immediate;
  native_install(fighter);
}

void native_hold(Fighter *fighter, int frames, FighterFn end) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 0, frames - 1, end);
  for (int frame = 0; frame < frames; ++frame) {
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}

void native_motion_step(Fighter *fighter) {
  NativeMotion *motion = &fighter->native_motion;
  if (motion->count == 0 || motion->last_tick == g_battle.ticks) {
    return;
  }
  motion->last_tick = g_battle.ticks;
  uint64_t generation = motion->generation, continuation = fighter->continuation;
  const mframe_t *frame = &motion->frames[motion->frame];
  if (frame->aifunc != NULL) {
    frame->aifunc(fighter, frame->dist);
  }
  if (generation != motion->generation || continuation != fighter->continuation || fighter->pending != NULL) {
    return;
  }
  if (frame->thinkfunc != NULL) {
    frame->thinkfunc(fighter);
  }
  if (generation != motion->generation || continuation != fighter->continuation || fighter->pending != NULL) {
    return;
  }
  if (++motion->frame == motion->count) {
    if (motion->loop) {
      motion->frame = 0;
      return;
    }
    FighterFn end = motion->end;
    native_motion_stop(fighter);
    if (end != NULL) {
      end(fighter);
    }
  }
}

void native_motion_begin(Fighter *fighter, int frames, FighterFn end) {
  require(frames > 0 && frames <= NATIVE_FRAME_CAPACITY, "Native motion exceeds frame capacity");
  NativeMotion *motion = &fighter->native_motion;
  motion->generation++;
  motion->count = frames;
  motion->frame = 0;
  motion->end = end;
  motion->binding = NULL;
  motion->loop = false;
  memset(motion->frames, 0, (size_t)frames * sizeof motion->frames[0]);
}

void native_motion_frame(Fighter *fighter, int frame, FighterAIFn movement, float distance, FighterFn action) {
  require(frame >= 0 && frame < fighter->native_motion.count, "Invalid native motion frame");
  fighter->native_motion.frames[frame] = (mframe_t){movement, distance, action};
}

void native_motion_play(Fighter *fighter) {
  fighter->native_motion.last_tick = UINT64_MAX;
  fighter->body.physics = native_motion_step;
}

void native_motion_stop(Fighter *fighter) {
  fighter->native_motion.generation++;
  fighter->native_motion.count = 0;
  if (fighter->body.physics == native_motion_step || fighter->body.physics == fighter->native_motion.binding) {
    fighter->body.physics = NULL;
  }
  fighter->native_motion.binding = NULL;
  fighter->native_motion.loop = false;
}

void native_motion_bind(Fighter *fighter, FighterFn binding, const mframe_t *frames, int count) {
  if (fighter->native_motion.binding != binding || fighter->native_motion.count == 0) {
    native_motion_begin(fighter, count, NULL);
    memcpy(fighter->native_motion.frames, frames, (size_t)count * sizeof *frames);
    fighter->native_motion.binding = binding;
    fighter->native_motion.loop = true;
    native_motion_play(fighter);
    fighter->body.physics = binding;
  }
}

void native_motion_speed(Fighter *fighter, float speed) {
  fighter_set_speed(fighter, (q16)speed);
}

static void motion_stop_speed(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
}

void native_schedule_speed(Fighter *fighter, q16 speed, int delay, int frames) {
  require(delay >= 0 && frames >= 0, "Invalid native motion range");
  fighter_set_speed(fighter, 0);
  native_motion_begin(fighter, delay + frames + 1, NULL);
  if (frames > 0) {
    native_motion_frame(fighter, delay, native_motion_speed, (float)speed, NULL);
  }
  native_motion_frame(fighter, delay + frames, NULL, 0, motion_stop_speed);
  native_motion_play(fighter);
}

/* One row per pose step toward sample, then end. */
void native_play_rows(Fighter *fighter, int sample, int step, FighterFn end) {
  int start = fighter_sample(fighter);
  require(step != 0, "Native playback requires a pose step");
  int distance = step > 0 ? sample - start : start - sample;
  if (distance <= 0) {
    end(fighter);
    return;
  }
  int stride = step > 0 ? step : -step;
  int count = (distance + stride - 1) / stride;
  native_begin(fighter, 1, count, end);
  for (int frame = 1; frame <= count; ++frame) {
    int pose = start + frame * step;
    native_frame(fighter, frame, step > 0 ? mini(sample, pose) : maxi(sample, pose), NULL);
  }
  native_play(fighter, false);
}

/* Contact leaves early through the continuation, which replaces the rows. */
static void query_continue(Fighter *fighter) {
  if (battle_contact(fighter, fighter->native_record) != CONTACT_MISS) {
    fighter->native.move.endfunc(fighter);
  }
}

/* Up to frames contact rows, then end; contact leaves early through end. */
void native_query_rows(Fighter *fighter, const AttackRecord *record, int frames, int pose_step, FighterFn end) {
  if (frames <= 0) {
    end(fighter);
    return;
  }
  int pose = fighter_sample(fighter);
  require(record != NULL, "Native contact playback needs a record");
  fighter->native_record_value = *record;
  fighter->native_record = &fighter->native_record_value;
  native_begin(fighter, 1, frames, end);
  for (int frame = 1; frame <= frames; ++frame) {
    native_frame(fighter, frame, maxi(0, pose + frame * pose_step), query_continue);
  }
  native_play(fighter, false);
}

static void condition_frame(Fighter *fighter) {
  fighter->mmove_hold = !fighter->native_condition(fighter);
}

/* Holds the current pose until condition holds; the continuation is the end of that row. */
void native_await(Fighter *fighter, FighterTest condition, FighterFn end) {
  if (condition(fighter)) {
    end(fighter);
    return;
  }
  fighter->native_condition = condition;
  native_begin(fighter, 0, 0, end);
  fighter->native.frames[0].thinkfunc = condition_frame;
  native_play(fighter, false);
}

/* Nonblocking 45a7b0 completion: advance to the bound clip's last pose and
 * hold it for one visit before the continuation. */
void native_finish_clip(Fighter *fighter, FighterFn end) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  require(last != MOVE_END && first >= 0 && first <= last, "Clip completion needs a bounded cursor");
  native_begin(fighter, 1, last - first + 1, end);
  for (int frame = 1; frame <= last - first + 1; ++frame) {
    native_frame(fighter, frame, mini(last, first + frame), NULL);
  }
  native_play(fighter, false);
}

static void scene_weight_frame(Fighter *fighter, float weight) {
  fighter->blend_weight = (q16)weight;
}

void native_scene_blends(Fighter *fighter, const SceneClip *clips, int count, FighterFn end) {
  scene_begin(fighter, clips, count, -1);
  native_begin(fighter, 0, scene_frame_count(clips, count) - 1, end);
  int row = 0;
  for (int clip = 0; clip < count; ++clip) {
    require(clips[clip].blend_step > 0 && clips[clip].repeats == 1, "Native blend range needs single blend clips");
    int frames = (Q16_ONE - clips[clip].blend_first) / clips[clip].blend_step + 1;
    for (int frame = 1; frame <= frames; ++frame) {
      q16 weight = clips[clip].blend_first + mini(frame, frames - 1) * clips[clip].blend_step;
      fighter->native.poses[row] = clips[clip].first;
      fighter->native.frames[row++] = (mframe_t){scene_weight_frame, (float)weight, frame == frames ? scene_blend_advance : NULL};
    }
  }
  native_play(fighter, false);
}
