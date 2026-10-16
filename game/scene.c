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
