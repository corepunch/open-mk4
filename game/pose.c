#include "game.h"
#include "math_util.h"
#include <math.h>

void battle_bind_assets(const Assets *assets) {
  g_battle.assets = assets;
  g_battle.retail_scripts = assets != NULL && assets->retail_image.bytes.count != 0 ? &assets->retail_image : NULL;
  for (int i = 0; i < 2; ++i) {
    g_battle.fighters[i].assets = assets;
  }
  battle_capture_render_snapshot();
}

const Character *fighter_model(const Assets *assets, const Fighter *fighter) {
  if (fighter->scene_model != NULL) {
    return fighter->scene_model;
  }
  if (fighter->appearance == APPEARANCE_DRAGON) {
    return &assets->liu_dragon;
  }
  if (fighter->appearance == APPEARANCE_BEAST) {
    return &assets->scorpion_beast;
  }
  if (fighter->appearance == APPEARANCE_FROZEN) {
    return &assets->cut_characters.data[roster_def(fighter->roster)->asset];
  }
  return roster_character(assets, fighter->roster);
}

const Clip *fighter_clip(const Assets *assets, const Fighter *fighter) {
  const Character *ch = fighter_model(assets, fighter);
  int id = fighter->action == 0x1003 ? fighter->frozen_clip : fighter_move(fighter)->clip;
  if (id < 0) {
    id = ch->idle;
  }
  require((size_t)id < assets->clips.count, "Invalid fighter clip");
  const Clip *clip = &assets->clips.data[id];
  int channels = 0;
  for (size_t i = 0; i < ch->bones.count; ++i) {
    channels = maxi(channels, ch->bones.data[i].channel + 1);
  }
  return (int)clip->frames.data[0].angles.count == channels ? clip : &assets->clips.data[ch->idle];
}

double fighter_pose_frame(const Fighter *fighter) {
  if (fighter->assets == NULL) {
    return fighter_frame(fighter, 1 << 16);
  }
  const Clip *clip = fighter_clip(fighter->assets, fighter);
  int requested = fighter->action == 0x1003 ? fighter->frozen_clip : fighter_move(fighter)->clip;
  /* A rig-incompatible stream already selects the model's idle fallback.
   * Its presentation pose has no relation to the requested move's clock. */
  if (requested >= 0 && clip != &fighter->assets->clips.data[requested]) {
    return 0;
  }
  return fighter_frame(fighter, (int)clip->frames.count);
}

int fighter_pose_sample(const Fighter *fighter) {
  return (int)fighter_pose_frame(fighter);
}

Mat3 fighter_turn(const Assets *assets, const Fighter *fighter) {
  const Character *ch = fighter_model(assets, fighter);
  const Clip *clip = fighter_clip(assets, fighter);
  Pose initial[MK4_MAX_BONES];
  pose(ch, clip, 0, initial);
  float front = ch->front_yaw;
  for (size_t i = 0; i < ch->bones.count; ++i) {
    if (ch->bones.data[i].channel == 0) {
      Vec3 face = mat3_apply(initial[i].rotation, (Vec3){0, 0, -1});
      front = atan2f(-face.x, face.z);
      break;
    }
  }
  float yaw = front + atan2f((float)fighter->heading[0], (float)fighter->heading[1]);
  float c = cosf(yaw), s = sinf(yaw);
  Mat3 heading = {{c, 0, s, 0, 1, 0, -s, 0, c}};
  float factor = 4096.f / (6.283185307179586f * Q16_ONE);
  Mat3 angular = rotation(0, (Vec3){fighter->body.angle[0] * factor, fighter->body.angle[1] * factor, fighter->body.angle[2] * factor});
  return mat3_mul(heading, angular);
}

void fighter_pose(const Assets *assets, const Fighter *fighter, Pose output[MK4_MAX_BONES]) {
  const Character *ch = fighter_model(assets, fighter);
  const Clip *clip = fighter_clip(assets, fighter);
  if (fighter->blend_duration > 0) {
    int id = fighter->blend_clip < 0 ? ch->idle : fighter->blend_clip;
    float fraction = fighter->blend_weight >= 0 ? q16_float(fighter->blend_weight) : fminf(1.f, (float)fighter->blend_ticks / fighter->blend_duration);
    pose_blend(ch, &assets->clips.data[id], fighter->blend_sample, clip, fighter_pose_sample(fighter), fraction, output);
  } else {
    pose(ch, clip, fighter_pose_frame(fighter), output);
  }
}

void fighter_render_pose(const Battle *battle, const Assets *assets, const Fighter *fighter, double interpolation, Pose output[MK4_MAX_BONES]) {
  const Character *ch = fighter_model(assets, fighter);
  const Clip *clip = fighter_clip(assets, fighter);
  if (fighter->blend_duration > 0) {
    int id = fighter->blend_clip < 0 ? ch->idle : fighter->blend_clip;
    float fraction = fighter->blend_weight >= 0 ? q16_float(fighter->blend_weight) : fminf(1.f, (float)fighter->blend_ticks / fighter->blend_duration);
    int sample = fighter_pose_sample(fighter);
    pose_blend(ch, &assets->clips.data[id], fighter->blend_sample, clip, sample, fraction, output);
    return;
  }
  int fighter_index = fighter == &battle->fighters[0] ? 0 : fighter == &battle->fighters[1] ? 1 : -1;
  const AnimationSnapshot *previous = &battle->previous_animation;
  const AnimationSnapshot *current = &battle->current_animation;
  if (fighter_index < 0 || !previous->valid || !current->valid || !previous->fighters[fighter_index].valid || !current->fighters[fighter_index].valid ||
      previous->fighters[fighter_index].model != ch || current->fighters[fighter_index].model != ch) {
    double sample = fighter_pose_sample(fighter);
    pose(ch, clip, sample, output);
    return;
  }
  const FighterAnimationFrame *from = &previous->fighters[fighter_index];
  const FighterAnimationFrame *to = &current->fighters[fighter_index];
  require(from->clip >= 0 && (size_t)from->clip < assets->clips.count, "Invalid previous render clip");
  require(to->clip >= 0 && (size_t)to->clip < assets->clips.count, "Invalid current render clip");
  const Clip *from_clip = &assets->clips.data[from->clip];
  const Clip *to_clip = &assets->clips.data[to->clip];
  float fraction = (float)fmax(0., fmin(1., interpolation));
  if (from->clip == to->clip) {
    double sample = from->sample + (to->sample - from->sample) * fraction;
    pose(ch, to_clip, sample, output);
  } else {
    pose_blend_frames(ch, from_clip, from->sample, to_clip, to->sample, fraction, output);
  }
}

/* Joint positions include animation root translation, as in 4b8140.
 * Group Y is the bind-root height plus the fighter's displacement. */
bool fighter_joint_bounds(const Fighter *fighter, const q16 origin[3], const q16 axis[2], q16 bounds[4]) {
  if (fighter->assets == NULL) {
    return false;
  }
  const Character *ch = fighter_model(fighter->assets, fighter);
  Pose bones[MK4_MAX_BONES];
  fighter_pose(fighter->assets, fighter, bones);
  Mat3 turn = fighter_turn(fighter->assets, fighter);
  Vec3 bind = ch->bones.data[0].translation;
  bounds[0] = bounds[2] = INT32_MAX;
  bounds[1] = bounds[3] = INT32_MIN;
  for (size_t i = 0; i < ch->bones.count; ++i) {
    if (ch->bones.data[i].id <= 0) {
      continue;
    }
    Vec3 local = vec_sub(bones[i].position, bind);
    local.y += q16_float(fighter->body.root_y);
    Vec3 joint = mat3_apply(turn, local);
    q16 x = fighter->body.pos[AXIS_X] - origin[AXIS_X] + (q16)floorf(joint.x * Q16_ONE);
    q16 z = fighter->body.pos[AXIS_Z] - origin[AXIS_Z] + (q16)floorf(joint.z * Q16_ONE);
    q16 along = q16_mul(x, axis[0]) + q16_mul(z, axis[1]);
    q16 y = fighter->body.pos[AXIS_Y] + (q16)floorf(joint.y * Q16_ONE);
    bounds[0] = mini(bounds[0], along);
    bounds[1] = maxi(bounds[1], along);
    bounds[2] = mini(bounds[2], y);
    bounds[3] = maxi(bounds[3], y);
  }
  return bounds[0] <= bounds[1];
}
