#include "game.h"

/* Small C playback records for translated scene continuations. There are
 * no bytecodes, task stacks, virtual addresses or implicit global operands. */
static void begin_clip(Fighter *fighter) {
  const SceneClip *clip = &fighter->scene.clips[fighter->scene.index];
  int source = fighter->assets != NULL ? fighter_clip(fighter->assets, fighter)->id : fighter_move(fighter)->clip;
  int sample = fighter_pose_sample(fighter);
  fighter->current_move = &clip->move;
  fighter->sample_override = clip->first;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  if (clip->blend_step > 0) {
    fighter->blend_clip = source;
    fighter->blend_sample = sample;
    fighter->blend_duration = 1;
    fighter->blend_weight = clip->blend_first;
  }
}

void scene_begin(Fighter *fighter, const SceneClip *clips, int count, int loop_from) {
  require(clips != NULL && count > 0 && loop_from < count, "Invalid scene playback");
  fighter->scene = (ScenePlayback){.clips = clips, .count = count, .loop_from = loop_from};
  begin_clip(fighter);
}

bool scene_step(Fighter *fighter) {
  ScenePlayback *scene = &fighter->scene;
  if (scene->done || scene->clips == NULL) {
    return true;
  }
  const SceneClip *clip = &scene->clips[scene->index];
  if (clip->blend_step > 0) {
    fighter->blend_weight += clip->blend_step;
    if (fighter->blend_weight <= Q16_ONE) {
      return false;
    }
    fighter->sample_override = clip->last;
    fighter->blend_duration = 0;
    fighter->blend_weight = -1;
  } else if (fighter->sample_override != clip->last) {
    fighter->sample_override += clip->last >= clip->first ? 1 : -1;
    return false;
  }
  if (++scene->repeat < clip->repeats) {
    fighter->sample_override = clip->first;
    return false;
  }
  scene->repeat = 0;
  if (++scene->index == scene->count) {
    if (scene->loop_from < 0) {
      scene->index--;
      scene->done = true;
      return true;
    }
    scene->index = scene->loop_from;
  }
  begin_clip(fighter);
  return false;
}

int scene_frame_count(const SceneClip *clips, int count) {
  int frames = 0;
  for (int index = 0; index < count; ++index) {
    const SceneClip *clip = &clips[index];
    int span = clip->last >= clip->first ? clip->last - clip->first : clip->first - clip->last;
    int visits = clip->blend_step > 0 ? (Q16_ONE - clip->blend_first) / clip->blend_step + 1 : span + 1;
    frames += visits * clip->repeats;
  }
  return frames;
}

/* Blend boundaries are selected by authored mframe rows, never by testing
 * a live weight or an elapsed counter. */
void scene_blend_advance(Fighter *fighter) {
  ScenePlayback *scene = &fighter->scene;
  fighter->sample_override = scene->clips[scene->index].last;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  if (++scene->index == scene->count) {
    scene->index--;
    scene->done = true;
    scene->clips = NULL;
  } else {
    begin_clip(fighter);
  }
}
