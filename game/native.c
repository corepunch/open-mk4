#include "native.h"
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
  int pose = fighter_sample(fighter);
  program->move = (mmove_t){.firstframe = first, .lastframe = last, .frame = program->frames, .endfunc = end, .samples = program->poses};
  memset(program->frames, 0, (size_t)(last - first + 1) * sizeof program->frames[0]);
  for (int i = 0; i <= last - first; ++i) {
    program->poses[i] = pose;
  }
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
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_handoff(native_install);
  }
  if (fighter_fiber_active(fighter)) {
    fighter_fiber_release_to_mmove(fighter);
  }
  native_install(fighter);
}

void native_wait(Fighter *fighter) {
  fighter->native.move.endfunc = fighter_stop_mmove;
  fighter->native_immediate = false;
  native_install(fighter);
  fighter_fiber_await_frames(fighter);
}

void native_wait_loop(Fighter *fighter) {
  fighter->native.move.endfunc = NULL;
  fighter->native_immediate = false;
  native_install(fighter);
  fighter_fiber_await_frames(fighter);
}

void native_delay(Fighter *fighter, int frames) {
  if (frames <= 0) {
    return;
  }
  int pose = fighter_sample(fighter);
  native_begin(fighter, 0, frames - 1, NULL);
  for (int frame = 0; frame < frames; ++frame) {
    native_frame(fighter, frame, pose, NULL);
  }
  native_wait(fighter);
}

void native_advance(Fighter *fighter, int count, int step) {
  if (count <= 0) {
    return;
  }
  int pose = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, 1, count, NULL);
  for (int frame = 1; frame <= count; ++frame) {
    native_frame(fighter, frame, maxi(0, last == MOVE_END ? pose + step * frame : mini(last, pose + step * frame)), NULL);
  }
  native_wait(fighter);
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

void native_play_to(Fighter *fighter, int sample, int step) {
  int start = fighter_sample(fighter);
  require(step != 0, "Native playback requires a pose step");
  int distance = step > 0 ? sample - start : start - sample;
  if (distance <= 0) {
    return;
  }
  int stride = step > 0 ? step : -step;
  int count = (distance + stride - 1) / stride;
  native_begin(fighter, 1, count, NULL);
  for (int frame = 1; frame <= count; ++frame) {
    int pose = start + frame * step;
    native_frame(fighter, frame, step > 0 ? mini(sample, pose) : maxi(sample, pose), NULL);
  }
  native_wait(fighter);
}

void native_calls(Fighter *fighter, int frames, int pose_step, FighterFn action) {
  if (frames <= 0) {
    return;
  }
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, frames, NULL);
  for (int frame = 1; frame <= frames; ++frame) {
    native_frame(fighter, frame, maxi(0, pose + frame * pose_step), action);
  }
  native_wait(fighter);
}

static void query_frame(Fighter *fighter) {
  if (battle_contact(fighter, fighter->native_record) != CONTACT_MISS) {
    fighter_stop_mmove(fighter);
  }
}

void native_queries(Fighter *fighter, const AttackRecord *record, int frames, int pose_step) {
  fighter->native_record = record;
  native_calls(fighter, frames, pose_step, query_frame);
}

static void condition_frame(Fighter *fighter) {
  fighter->mmove_hold = !fighter->native_condition(fighter);
}

void native_until(Fighter *fighter, FighterTest condition) {
  if (condition(fighter)) {
    return;
  }
  fighter->native_condition = condition;
  native_begin(fighter, 0, 0, NULL);
  fighter->native.frames[0].thinkfunc = condition_frame;
  native_wait(fighter);
}
