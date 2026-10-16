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

void battle_scene_camera(Battle *battle, const Fighter *owner, const Fighter *target, q16 along, q16 y, q16 across, int ticks) {
  SceneCamera *camera = &battle->camera;
  float x = q16_float(owner->heading[0]), z = q16_float(owner->heading[1]);
  Vec3 eye = {fighter_x(owner) + x * q16_float(along) - z * q16_float(across), q16_float(y), fighter_z(owner) + z * q16_float(along) + x * q16_float(across)};
  Vec3 aim;
  Mat3 unused;
  fighter_joint_position(target, 3, &aim, &unused);
  if (!camera->active || ticks <= 0) {
    camera->eye = eye;
    camera->target = aim;
  }
  camera->active = true;
  camera->from_eye = camera->eye;
  camera->from_target = camera->target;
  camera->to_eye = eye;
  camera->to_target = aim;
  camera->ticks = 0;
  camera->duration = maxi(1, ticks);
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
  if (joint < 0 || joint >= 128 || (victim->hidden_joints[joint / 64] & (UINT64_C(1) << (joint % 64)))) {
    return NULL;
  }
  victim->hidden_joints[joint / 64] |= UINT64_C(1) << (joint % 64);
  if (victim->assets == NULL) {
    return NULL;
  }
  const Character *ch = fighter_model(victim->assets, victim);
  for (size_t n = 0; n < ch->bones.count; ++n) {
    if (ch->bones.data[n].id != joint) {
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

void battle_attach_head(Battle *battle, Fighter *victim, Fighter *owner, int joint) {
  ScenePiece *head = detach(battle, victim, 2, (q16[3]){0}, (q16[3]){0});
  if (head != NULL) {
    head->attached_to = owner == &battle->fighters[0] ? 0 : 1;
    head->joint = joint;
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
  SceneCamera *camera = &battle->camera;
  if (camera->active && camera->ticks < camera->duration) {
    float t = (float)++camera->ticks / camera->duration;
    camera->eye = vec_add(camera->from_eye, vec_scale(vec_sub(camera->to_eye, camera->from_eye), t));
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
    if (!piece->active) {
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
