#include "game.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

void battle_sound(Battle *battle, int bank, int selector) {
  require(bank >= 0 && bank < SOUND_BANK_COUNT && selector >= 0 && selector < SOUND_EVENT_COUNT, "Invalid sound event");
  if (battle->sound_count < MAX_SOUND_EVENTS) {
    battle->sounds[battle->sound_count++] = (SoundEvent){bank, selector};
  }
}

void fighter_visual_offset(Fighter *fighter, q16 along, q16 y) {
  fighter->visual_offset[AXIS_X] = q16_mul(fighter->heading[0], along);
  fighter->visual_offset[AXIS_Y] = y;
  fighter->visual_offset[AXIS_Z] = q16_mul(fighter->heading[1], along);
}

bool fighter_joint_position(const Fighter *fighter, int joint, Vec3 *position, Mat3 *orientation) {
  *position = (Vec3){fighter_x(fighter), q16_float(fighter->body.pos[AXIS_Y]), fighter_z(fighter)};
  Vec3 offset = {q16_float(fighter->visual_offset[0]), q16_float(fighter->visual_offset[1]), q16_float(fighter->visual_offset[2])};
  *position = vec_add(*position, offset);
  *orientation = mat3_identity();
  if (fighter->assets == NULL) {
    return false;
  }
  const Character *ch = fighter_model(fighter->assets, fighter);
  Pose bones[MK4_MAX_BONES];
  fighter_pose(fighter->assets, fighter, bones);
  Mat3 turn = fighter_turn(fighter->assets, fighter);
  for (size_t i = 0; i < ch->bones.count; ++i) {
    if (ch->bones.data[i].id == joint) {
      Vec3 local = vec_sub(bones[i].position, ch->bones.data[0].translation);
      local.y += q16_float(fighter->body.root_y);
      *position = vec_add(mat3_apply(turn, local), (Vec3){fighter_x(fighter), q16_float(fighter->body.pos[AXIS_Y]), fighter_z(fighter)});
      *position = vec_add(*position, offset);
      *orientation = mat3_mul(turn, bones[i].rotation);
      return true;
    }
  }
  return false;
}

static void camera_shot(Battle *battle) {
  SceneCamera *camera = &battle->camera;
  const CameraShot *shot = &camera->shots[camera->shot];
  require(shot->count >= 4 && shot->step > 0, "Invalid camera shot");
  camera->parameter = Q16_ONE;
  camera->hold = 0;
  camera->path_start = camera->eye;
  if (shot->aim_ticks > 0) {
    Mat3 unused;
    fighter_joint_position(&battle->fighters[camera->target_player], shot->aim_joint, &camera->to_target, &unused);
    camera->to_target.y += q16_float(shot->aim_y);
    camera->from_target = camera->target;
    camera->ticks = 0;
    camera->duration = shot->aim_ticks;
  }
}

/* 445920/44ca70/44cb20: retain a scene reference along the pair axis.
 * Camera tracks run independently from actor animation continuations. */
void battle_camera_sequence(Battle *battle, const Fighter *owner, const Fighter *target, const CameraShot *shots, int count) {
  require(shots != NULL && count > 0, "Invalid camera sequence");
  SceneCamera *camera = &battle->camera;
  float ax = fighter_x(&battle->fighters[0]), bx = fighter_x(&battle->fighters[1]);
  Vec3 initial = camera->active ? camera->eye : (Vec3){(ax + bx) * .25f, -2.8f, fmaxf(7.5f, fabsf(ax - bx) * .85f + 3.f)};
  *camera = (SceneCamera){.active = true, .eye = initial, .shots = shots, .shot_count = count, .target_player = target == &battle->fighters[0] ? 0 : 1};
  camera->axis = (Vec3){q16_float(owner->heading[0]), 0, q16_float(owner->heading[1])};
  camera->origin = (Vec3){fighter_x(target) - camera->axis.x * q16_float(0xe666), -1.25f, fighter_z(target) - camera->axis.z * q16_float(0xe666)};
  camera->target = camera->origin;
  camera_shot(battle);
}

/* 48b030/4a0: four control points, with the retail compact-support
 * cubic kernel .5*(1 - .75*d*d + .25*abs(d*d*d)). Indices <= 2
 * repeat the camera's starting position; later controls are local to
 * the scene reference. Retail records store X,Z,Y; CameraShot uses X,Y,Z. */
static Vec3 camera_sample(const SceneCamera *camera) {
  const CameraShot *shot = &camera->shots[camera->shot];
  float parameter = q16_float(camera->parameter);
  int first = (int)parameter - 1;
  Vec3 result = {0};
  for (int i = first; i < first + 4; ++i) {
    float d = fabsf(parameter - i);
    float weight = .5f * (1.f - .75f * d * d + .25f * d * d * d);
    Vec3 point = camera->path_start;
    if (i > 2) {
      int n = mini(shot->count - 1, i - 3);
      float x = q16_float(shot->points[n][0]), z = q16_float(shot->points[n][2]);
      point = (Vec3){camera->origin.x + camera->axis.x * x - camera->axis.z * z, q16_float(shot->points[n][1]),
          camera->origin.z + camera->axis.z * x + camera->axis.x * z};
    }
    result = vec_add(result, vec_scale(point, weight));
  }
  return result;
}

void battle_particles(Battle *battle, const Fighter *fighter, int joint, uint32_t color, int count, bool ice) {
  Vec3 origin;
  Mat3 unused;
  fighter_joint_position(fighter, joint, &origin, &unused);
  for (int i = 0; i < MAX_PARTICLES && count > 0; ++i) {
    Particle *p = &battle->particles[i];
    if (p->ticks > 0) {
      continue;
    }
    unsigned seed = (unsigned)battle->ticks * 1664525u + (unsigned)i * 1013904223u;
    q16 side = (q16)((seed >> 16) & 0x3fff) - 0x2000;
    *p = (Particle){.pos = {(q16)(origin.x * Q16_ONE), (q16)(origin.y * Q16_ONE), (q16)(origin.z * Q16_ONE)},
        .vel = {q16_mul(fighter->heading[0], ice ? 0x51e0 : side), ice ? (side / 8) : -0x170a, q16_mul(fighter->heading[1], ice ? 0x51e0 : side)},
        .gravity = ice ? 0 : 0x147,
        .ticks = ice ? 16 : 45,
        .color = color};
    p->vel[AXIS_X] -= q16_mul(fighter->heading[1], side);
    p->vel[AXIS_Z] += q16_mul(fighter->heading[0], side);
    --count;
  }
}

static ScenePiece *detach(Battle *battle, Fighter *victim, int joint, const q16 velocity[3], const q16 spin[3]) {
  if (joint < -128 || joint >= 128 || (joint >= 0 && (victim->hidden_joints[joint / 64] & (UINT64_C(1) << (joint % 64))))) {
    return NULL;
  }
  if (joint >= 0) {
    victim->hidden_joints[joint / 64] |= UINT64_C(1) << (joint % 64);
  }
  if (victim->assets == NULL) {
    return NULL;
  }
  const Character *ch = fighter_model(victim->assets, victim);
  for (size_t n = 0; n < ch->bones.count; ++n) {
    if (ch->bones.data[n].id != joint || (ch->bones.data[n].flags & 4)) {
      continue;
    }
    for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
      ScenePiece *piece = &battle->pieces[i];
      if (piece->active) {
        continue;
      }
      *piece = (ScenePiece){.active = true, .frozen = victim->appearance == APPEARANCE_FROZEN, .roster = victim->roster, .bone = (int)n, .attached_to = -1};
      Vec3 origin;
      fighter_joint_position(victim, joint, &origin, &piece->orientation);
      piece->pos[0] = (q16)(origin.x * Q16_ONE);
      piece->pos[1] = (q16)(origin.y * Q16_ONE);
      piece->pos[2] = (q16)(origin.z * Q16_ONE);
      memcpy(piece->vel, velocity, sizeof piece->vel);
      memcpy(piece->spin, spin, sizeof piece->spin);
      piece->vel[AXIS_X] *= victim->facing;
      return piece;
    }
  }
  return NULL;
}

ScenePiece *battle_attach_head(Battle *battle, Fighter *victim, Fighter *owner, int joint) {
  ScenePiece *head = detach(battle, victim, 2, (q16[3]){0}, (q16[3]){0});
  if (head != NULL) {
    head->attached_to = owner == &battle->fighters[0] ? 0 : 1;
    head->joint = joint;
  }
  return head;
}

static void attachment_pose(Battle *battle, SceneAttachment *attachment) {
  const AttachmentDef *def = attachment->def;
  const Frame *frame = &battle->assets->clips.data[def->clip].frames.data[attachment->sample];
  ScenePiece *piece = &battle->pieces[attachment->piece];
  Vec3 position = vec_add(attachment->origin, mat3_apply(attachment->orientation, vec_add(frame->translation, def->piece_offset)));
  for (int k = 0; k < 3; ++k) {
    piece->pos[k] = (q16)((k == 0 ? position.x : k == 1 ? position.y : position.z) * Q16_ONE);
  }
  piece->orientation = mat3_mul(attachment->orientation, rotation(def->piece_kind, frame->angles.data[0]));
}

void battle_attach_piece_model(Battle *battle, ScenePiece *piece, const AttachmentDef *def, const Fighter *actor) {
  if (piece == NULL) {
    return;
  }
  require(def != NULL && def->first <= def->last && def->loop_from >= def->first && def->loop_from <= def->last, "Invalid animated attachment");
  for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
    if (battle->attachments[i].def == NULL) {
      battle->attachments[i] = (SceneAttachment){.def = def, .piece = (int)(piece - battle->pieces), .sample = def->first};
      SceneAttachment *attachment = &battle->attachments[i];
      attachment->origin = vec_add((Vec3){fighter_x(actor), q16_float(actor->body.pos[AXIS_Y]), fighter_z(actor)}, def->group_offset);
      attachment->orientation = fighter_turn(actor->assets, actor);
      piece->animated = true;
      piece->attached_to = -1;
      attachment_pose(battle, attachment);
      return;
    }
  }
}

/* 41b610 -> 419720/7e0/8a0/900/9e0/aa0/b60.
 * First six words of each 4d28e8..4d2b58 record: linear/angular speed.
 * Bodies use our bounded piece pool; the native's task tree is unnecessary. */
void battle_shatter(Battle *battle, Fighter *victim) {
  typedef struct Launch {
    int joint;
    q16 velocity[3], spin[3];
  } Launch;

  // clang-format off: Keep table entries on one row, including long rows.
  static const Launch launches[] = {
    {10, {-26214, -78643, -52428}, {0, 0, 13107}},
    {9, {26214, -78643, 52428}, {0, 3276, -13107}},
    {16, {-13107, -98304, 78643}, {0, -26214, 0}},
    {15, {13107, -98304, 13107}, {0, 26214, 0}},
    {8, {-32768, -58982, 65536}, {0, 3276, 13107}},
    {7, {32768, -58982, -65536}, {0, 0, -13107}},
    {14, {-32768, -58982, 32768}, {0, 0, 9830}},
    {13, {32768, -58982, -65536}, {0, 3276, -9830}},
    {6, {-19660, -39321, -45875}, {0, 0, 13107}},
    {5, {19660, -39321, 45875}, {0, 3276, -13107}},
    {12, {-26214, -72089, -65536}, {0, 3276, 6553}},
    {11, {26214, -72089, 65536}, {0, 0, -6553}},
    {3, {-16384, -45875, -32768}, {0, -13107, 1310}},
  };
  // clang-format on
  for (size_t i = 0; i < sizeof launches / sizeof launches[0]; ++i) {
    detach(battle, victim, launches[i].joint, launches[i].velocity, launches[i].spin);
  }
  if (victim->assets != NULL) {
    const Character *ch = fighter_model(victim->assets, victim);
    for (size_t i = 0; i < ch->bones.count; ++i) {
      /* Remaining hierarchy follows 419470's final body launch. */
      detach(battle, victim, ch->bones.data[i].id, (q16[3]){0x666, -0x1c28, 0xccc}, (q16[3]){0, 0x3333, -0x51e});
    }
  }
  victim->appearance = APPEARANCE_HIDDEN;
}

void battle_step_presentation(Battle *battle) {
  for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
    SceneAttachment *attachment = &battle->attachments[i];
    if (attachment->def != NULL) {
      if (!battle->pieces[attachment->piece].active) {
        attachment->def = NULL;
      } else {
        attachment->sample = attachment->sample < attachment->def->last ? attachment->sample + 1 : attachment->def->loop_from;
        attachment_pose(battle, attachment);
      }
    }
  }
  SceneCamera *camera = &battle->camera;
  if (camera->active && camera->shots != NULL) {
    const CameraShot *shot = &camera->shots[camera->shot];
    q16 end = (shot->count - 1) * Q16_ONE;
    if (camera->parameter < end) {
      camera->parameter = mini(end, camera->parameter + shot->step);
      camera->eye = camera_sample(camera);
    } else if (camera->shot + 1 < camera->shot_count && ++camera->hold >= shot->hold) {
      camera->shot++;
      camera_shot(battle);
    }
  }
  if (camera->active && camera->ticks < camera->duration) {
    float t = (float)++camera->ticks / camera->duration;
    camera->target = vec_add(camera->from_target, vec_scale(vec_sub(camera->to_target, camera->from_target), t));
  }
  for (int i = 0; i < MAX_PARTICLES; ++i) {
    Particle *p = &battle->particles[i];
    if (p->ticks > 0) {
      --p->ticks;
      p->vel[AXIS_Y] += p->gravity;
      for (int k = 0; k < 3; ++k) {
        p->pos[k] += p->vel[k];
      }
    }
  }
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    ScenePiece *piece = &battle->pieces[i];
    if (!piece->active || piece->animated) {
      continue;
    }
    if (piece->attached_to >= 0) {
      Vec3 position;
      fighter_joint_position(&battle->fighters[piece->attached_to], piece->joint, &position, &piece->orientation);
      piece->pos[0] = (q16)(position.x * Q16_ONE);
      piece->pos[1] = (q16)(position.y * Q16_ONE);
      piece->pos[2] = (q16)(position.z * Q16_ONE);
      continue;
    }
    if (piece->pos[AXIS_Y] < -0x1999 || piece->vel[AXIS_Y] < 0) {
      piece->vel[AXIS_Y] += 0x147; /* 4d0364, used by 419040 */
      for (int k = 0; k < 3; ++k) {
        piece->pos[k] += piece->vel[k];
        piece->angle[k] = (piece->angle[k] + piece->spin[k]) % 0x6487e;
        piece->spin[k] = q16_mul(piece->spin[k], 0xfd70);
      }
    } else {
      piece->pos[AXIS_Y] = -0x1999;
      memset(piece->vel, 0, sizeof piece->vel);
      memset(piece->spin, 0, sizeof piece->spin);
    }
  }
}
