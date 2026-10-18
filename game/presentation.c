#include "game.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

void battle_sound(int bank, int selector) {
  require(bank >= 0 && bank < SOUND_BANK_COUNT && selector >= 0 && selector < SOUND_EVENT_COUNT, "Invalid sound event");
  if (g_battle.sound_count < MAX_SOUND_EVENTS) {
    g_battle.sounds[g_battle.sound_count++] = (SoundEvent){bank, selector};
  }
}

/* 457cb0: voice row 1 of the impacted fighter, shake 4e3638[0], A[19]. */
void battle_scene_impact(const Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
  battle_camera_shake(3, 4, 0x2666);
  battle_sound(SOUND_A, 0x13);
}

void fighter_visual_offset(Fighter *fighter, q16 along, q16 y) {
  fighter->visual_offset[AXIS_X] = q16_mul(fighter->heading[0], along);
  fighter->visual_offset[AXIS_Y] = y;
  fighter->visual_offset[AXIS_Z] = q16_mul(fighter->heading[1], along);
}

static void orbit_light_step(OrbitLight *light) {
  const OrbitLightDef *def = light->def;
  light->phase = (light->phase + def->speed) % 0x6487e;
  light->pos[AXIS_X] = (q16)(cosf(q16_float(light->phase)) * def->radius);
  light->pos[AXIS_Y] = def->height;
  light->pos[AXIS_Z] = (q16)(sinf(q16_float(light->phase)) * def->radius);
  light->strength -= def->fade;
}

/* 47f9d0/fa90/fb10: the optional decoration is a lighting reference
 * substituted for the camera light, with its own orbit and strength. */
void battle_object_light(Projectile *object, const OrbitLightDef *def) {
  require(def != NULL && def->radius >= 0, "Invalid object light");
  object->light = (OrbitLight){.def = def, .strength = Q16_ONE};
  orbit_light_step(&object->light);
}

const Geometry *mesh_animation_geometry(const Assets *assets, const MeshAnimation *animation) {
  require(assets != NULL && animation != NULL && (animation->source == MESH_SHARED || animation->source == MESH_SHARED2), "Invalid effect geometry bank");
  return animation->source == MESH_SHARED2 ? &assets->hud.effects2 : &assets->hud.effects;
}

int projectile_mesh_frame(const Projectile *object) {
  const MeshAnimation *animation = object->mesh_animation;
  require(animation != NULL && animation->count > 0 && animation->period > 0, "Invalid object mesh animation");
  if (object->ticks == 0) {
    return object->initial_mesh;
  }
  return animation->first + (maxi(0, object->ticks - 1) / animation->period) % animation->count;
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

static void camera_shot(void) {
  SceneCamera *camera = &g_battle.camera;
  const CameraShot *shot = &camera->shots[camera->shot];
  require(shot->count >= 4 && shot->step > 0, "Invalid camera shot");
  camera->parameter = Q16_ONE;
  camera->hold = 0;
  camera->path_start = camera->eye;
  if (shot->aim_ticks > 0) {
    Mat3 unused;
    fighter_joint_position(&g_battle.fighters[camera->target_player], shot->aim_joint, &camera->to_target, &unused);
    camera->to_target.y += q16_float(shot->aim_y);
    camera->from_target = camera->target;
    camera->ticks = 0;
    camera->duration = shot->aim_ticks;
  }
}

/* 445920/44ca70/44cb20: retain a scene reference along the pair axis.
 * Camera tracks run independently from actor animation continuations. */
void battle_camera_sequence(const Fighter *owner, const Fighter *target, const CameraShot *shots, int count) {
  require(shots != NULL && count > 0, "Invalid camera sequence");
  SceneCamera *camera = &g_battle.camera;
  Vec3 initial = camera->active ? camera->eye : battle_follow_eye(NULL);
  *camera = (SceneCamera){.active = true, .eye = initial, .shots = shots, .shot_count = count, .target_player = target == &g_battle.fighters[0] ? 0 : 1};
  camera->axis = (Vec3){q16_float(owner->heading[0]), 0, q16_float(owner->heading[1])};
  camera->origin = (Vec3){fighter_x(target) - camera->axis.x * q16_float(0xe666), -1.25f, fighter_z(target) - camera->axis.z * q16_float(0xe666)};
  camera->target = camera->origin;
  camera_shot();
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

static q16 visual_random(unsigned *seed, q16 range) {
  *seed = *seed * 1664525u + 1013904223u;
  return (q16)(((uint64_t)*seed * (unsigned)range) >> 32);
}

void battle_spray(const ParticleSpray *spray, Vec3 origin, Vec3 direction, q16 speed, q16 variation) {
  require(spray != NULL && spray->visual != NULL && spray->lifetime > 0 && variation >= 0, "Invalid particle spray");
  int count = spray->count;
  for (int i = 0; i < MAX_PARTICLES && count > 0; ++i) {
    Particle *p = &g_battle.particles[i];
    if (p->ticks > 0) {
      continue;
    }
    unsigned seed = (unsigned)g_battle.ticks * 1664525u + (unsigned)i * 1013904223u;
    q16 velocity = speed + visual_random(&seed, variation);
    *p = (Particle){.pos = {(q16)(origin.x * Q16_ONE), (q16)(origin.y * Q16_ONE), (q16)(origin.z * Q16_ONE)},
        .vel = {(q16)(direction.x * velocity), (q16)(direction.y * velocity), (q16)(direction.z * velocity)},
        .gravity = spray->gravity,
        .ticks = spray->lifetime,
        .visual = spray->visual,
        .scale = spray->visual->animation.scale,
        .splash = spray->splash};
    for (int k = 0; k < 3; ++k) {
      p->pos[k] += spray->offset[k] + visual_random(&seed, spray->position_spread[k] * 2) - spray->position_spread[k];
      p->vel[k] += spray->velocity[k] + visual_random(&seed, spray->spread[k] * 2) - spray->spread[k];
    }
    --count;
  }
}

static void joint_spray_step(JointSpray *emitter) {
  const JointSprayDef *def = emitter->def;
  int age = emitter->ticks++;
  for (int phase = 0; phase < def->count; ++phase) {
    const EmissionPhase *timing = &def->phases[phase];
    if (age >= timing->duration) {
      age -= timing->duration;
      continue;
    }
    if (age % timing->period == 0 || age % timing->period == timing->extra) {
      Fighter *fighter = &g_battle.fighters[emitter->fighter];
      Vec3 origin, target;
      Mat3 unused;
      fighter_joint_position(fighter, def->origin_joint, &origin, &unused);
      fighter_joint_position(fighter, def->base_joint, &target, &unused);
      Vec3 direction = vec_sub(origin, target);
      float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
      direction = length > .0001f ? vec_scale(direction, 1.f / length) : (Vec3){0, -1, 0};
      ParticleSpray spray = *def->spray;
      spray.count = timing->count;
      battle_spray(&spray, origin, direction, def->speed, def->variation);
    }
    return;
  }
  emitter->def = NULL;
}

void battle_joint_spray(const Fighter *fighter, const JointSprayDef *def) {
  require(def != NULL && def->phases != NULL && def->count > 0, "Invalid joint spray");
  for (int i = 0; i < MAX_JOINT_SPRAYS; ++i) {
    if (g_battle.sprays[i].def == NULL) {
      g_battle.sprays[i] = (JointSpray){.def = def, .fighter = fighter == &g_battle.fighters[0] ? 0 : 1};
      joint_spray_step(&g_battle.sprays[i]);
      return;
    }
  }
}

/* Our fight framing, not the retail camera director: look across the pair
 * axis at the fighters' midpoint and pull back with their separation. */
Vec3 battle_follow_eye(Vec3 *heading) {
  const Fighter *a = &g_battle.fighters[0], *b = &g_battle.fighters[1];
  Vec3 view = g_battle.view;
  float length = hypotf(view.x, view.z);
  view = length > .001f ? (Vec3){view.x / length, 0, view.z / length} : (Vec3){0, 0, -1};
  float distance = hypotf(fighter_x(b) - fighter_x(a), fighter_z(b) - fighter_z(a));
  float back = fmaxf(7.5f, distance * .85f + 3.f);
  if (heading != NULL) {
    *heading = view;
  }
  return (Vec3){(fighter_x(a) + fighter_x(b)) * .5f - view.x * back, -2.8f, (fighter_z(a) + fighter_z(b)) * .5f - view.z * back};
}

/* The heading stays perpendicular to the pair axis on the side it already
 * views from, so a crossover keeps the camera put, and eases toward each
 * new axis so sidesteps swing the view rather than snap it. */
static void step_follow_view(void) {
  const Fighter *a = &g_battle.fighters[0], *b = &g_battle.fighters[1];
  float dx = fighter_x(b) - fighter_x(a), dz = fighter_z(b) - fighter_z(a);
  float distance = hypotf(dx, dz);
  Vec3 view;
  battle_follow_eye(&view);
  if (distance > .05f) {
    Vec3 goal = {dz / distance, 0, -dx / distance};
    if (goal.x * view.x + goal.z * view.z < 0) {
      goal = vec_scale(goal, -1);
    }
    view = vec_add(view, vec_scale(vec_sub(goal, view), .1f));
  }
  g_battle.view = view;
}

/* 457b50/457b90: alternate a vertical camera offset and its removal,
 * independently of the active shot and without accumulating drift. */
void battle_camera_shake(int pulses, int interval, q16 amount) {
  require(pulses > 0 && interval > 0, "Invalid camera shake");
  SceneCamera *camera = &g_battle.camera;
  camera->shake_y = camera->shake_amount = amount;
  camera->shake_pulses = pulses;
  camera->shake_interval = camera->shake_ticks = interval;
}

void battle_particles(const Fighter *fighter, int joint, int count, bool ice) {
  static const ParticleVisual blood = {.animation = {.first = 0, .count = 1, .period = 1, .scale = Q16_ONE}};
  static const ParticleVisual chip = {.animation = {.first = 73, .count = 1, .period = 1, .scale = Q16_ONE}};
  Vec3 origin;
  Mat3 unused;
  fighter_joint_position(fighter, joint, &origin, &unused);
  for (int i = 0; i < MAX_PARTICLES && count > 0; ++i) {
    Particle *p = &g_battle.particles[i];
    if (p->ticks > 0) {
      continue;
    }
    unsigned seed = (unsigned)g_battle.ticks * 1664525u + (unsigned)i * 1013904223u;
    q16 side = (q16)((seed >> 16) & 0x3fff) - 0x2000;
    *p = (Particle){.pos = {(q16)(origin.x * Q16_ONE), (q16)(origin.y * Q16_ONE), (q16)(origin.z * Q16_ONE)},
        .vel = {q16_mul(fighter->heading[0], ice ? 0x51e0 : side), ice ? (side / 8) : -0x170a, q16_mul(fighter->heading[1], ice ? 0x51e0 : side)},
        .gravity = ice ? 0 : 0x147,
        .ticks = ice ? 16 : 45,
        .visual = ice ? &chip : &blood,
        .scale = Q16_ONE,
        .splash = !ice};
    p->vel[AXIS_X] -= q16_mul(fighter->heading[1], side);
    p->vel[AXIS_Z] += q16_mul(fighter->heading[0], side);
    --count;
  }
}

static BloodDrop *blood_slot(void) {
  for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
    if (g_battle.blood[i].kind == BLOOD_NONE) {
      return &g_battle.blood[i];
    }
  }
  return NULL;
}

/* Hit blood replaces retail's single hit sprite with MK3-style droplets
 * that arc away from the attacker and splash on the floor. It reuses the
 * retail blood drop, crown splash and spatter art from shared_g.geo.
 * Larger hits throw more and faster drops. */
void battle_blood_hit(const Fighter *attacker, const Fighter *victim, Vec3 point, q16 damage) {
  if (damage <= 0 || victim->appearance == APPEARANCE_FROZEN || victim->appearance == APPEARANCE_HIDDEN) {
    return;
  }
  float dx = fighter_x(victim) - fighter_x(attacker), dz = fighter_z(victim) - fighter_z(attacker);
  float length = hypotf(dx, dz);
  if (length < .001f) {
    dx = (float)attacker->facing;
    dz = 0;
    length = 1;
  }
  dx /= length;
  dz /= length;
  /* Bleed from the victim's front, at the attack's height but no farther
   * from the body than its root (0) and head (2) joints, however posed. */
  Vec3 root, head;
  Mat3 unused;
  fighter_joint_position(victim, 0, &root, &unused);
  fighter_joint_position(victim, 2, &head, &unused);
  point.x = fighter_x(victim) - dx * .2f;
  point.z = fighter_z(victim) - dz * .2f;
  point.y = clampf(point.y, fminf(root.y, head.y), fmaxf(root.y, head.y));
  int percent = (int)((int64_t)damage * 100 / Q16_ONE);
  int count = mini(24, maxi(6, 4 + percent * 3 / 2));
  float power = fminf(1.6f, .8f + (float)percent * .04f);
  unsigned seed = (unsigned)g_battle.ticks * 2654435761u ^ (victim == &g_battle.fighters[0] ? 0u : 40503u);
  BloodDrop *spatter = blood_slot();
  if (spatter != NULL) {
    *spatter = (BloodDrop){.kind = BLOOD_SPATTER,
        .pos = {(q16)(point.x * Q16_ONE), (q16)(point.y * Q16_ONE), (q16)(point.z * Q16_ONE)},
        .vel = {(q16)(dx * .01f * Q16_ONE), 0, (q16)(dz * .01f * Q16_ONE)},
        .scale = (q16)((.7f + .04f * (float)mini(percent, 12)) * Q16_ONE),
        .ticks = 12};
  }
  for (int i = 0; i < count; ++i) {
    BloodDrop *drop = blood_slot();
    if (drop == NULL) {
      return;
    }
    /* Most drops follow the blow; a few spatter back toward the attacker. */
    float along = (visual_random(&seed, 0x10000) < 0x3000 ? -.35f : 1.f) * (.025f + q16_float(visual_random(&seed, 0x10000)) * .05f) * power;
    float side = (q16_float(visual_random(&seed, 0x10000)) - .5f) * .045f;
    float rise = (.015f + q16_float(visual_random(&seed, 0x10000)) * .06f) * power;
    float jitter[3];
    for (int k = 0; k < 3; ++k) {
      jitter[k] = (q16_float(visual_random(&seed, 0x10000)) - .5f) * .14f;
    }
    *drop = (BloodDrop){.kind = BLOOD_DROP,
        .pos = {(q16)((point.x + jitter[0]) * Q16_ONE), (q16)((point.y + jitter[1]) * Q16_ONE), (q16)((point.z + jitter[2]) * Q16_ONE)},
        .vel = {(q16)((dx * along - dz * side) * Q16_ONE), (q16)(-rise * Q16_ONE), (q16)((dz * along + dx * side) * Q16_ONE)},
        .scale = (q16)((.5f + q16_float(visual_random(&seed, 0x10000)) * .55f) * Q16_ONE),
        .ticks = 150};
  }
}

/* Larger landed drops stain the floor with one of the retail red splat
 * sprites; their green twins and neighbours are other effects. */
static void stain_floor(const BloodDrop *drop, int index) {
  static const int sprites[] = {163, 164, 166, 167, 168};
  if (drop->scale < 0xd800) {
    return;
  }
  BloodSplat *splat = &g_battle.splats[g_battle.splat_next];
  g_battle.splat_next = (g_battle.splat_next + 1) % MAX_BLOOD_SPLATS;
  *splat = (BloodSplat){.pos = {drop->pos[AXIS_X], drop->pos[AXIS_Z]},
      .axis = {g_battle.axis[0], g_battle.axis[1]},
      .half = 0x1800 + q16_mul(drop->scale, 0x2000),
      .sprite = sprites[(index + (int)g_battle.ticks) % (int)(sizeof sprites / sizeof sprites[0])],
      .ticks = BLOOD_SPLAT_TICKS};
}

static void step_blood(void) {
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    if (g_battle.splats[i].ticks > 0) {
      g_battle.splats[i].ticks--;
    }
  }
  for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
    BloodDrop *drop = &g_battle.blood[i];
    if (drop->kind == BLOOD_NONE) {
      continue;
    }
    drop->age++;
    if (--drop->ticks <= 0) {
      drop->kind = BLOOD_NONE;
      continue;
    }
    if (drop->kind == BLOOD_DROP) {
      drop->vel[AXIS_Y] += 0x1c0;
    }
    for (int k = 0; k < 3; ++k) {
      drop->pos[k] += drop->vel[k];
    }
    if (drop->kind == BLOOD_DROP && drop->pos[AXIS_Y] >= 0) {
      /* As 4745d0 does for retail drops: nine two-tick crown meshes. */
      stain_floor(drop, i);
      drop->kind = BLOOD_SPLASH;
      drop->pos[AXIS_Y] = -0x7ae;
      memset(drop->vel, 0, sizeof drop->vel);
      drop->age = 0;
      drop->ticks = 18;
    }
  }
}

static ScenePiece *detach(Fighter *victim, int joint, const q16 velocity[3], const q16 spin[3]) {
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
      ScenePiece *piece = &g_battle.pieces[i];
      if (piece->active) {
        continue;
      }
      *piece = (ScenePiece){
          .active = true, .frozen = victim->appearance == APPEARANCE_FROZEN, .roster = victim->roster, .bone = (int)n, .attached_to = -1, .model = ch};
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

ScenePiece *battle_detach_piece(Fighter *victim, int joint, const q16 velocity[3], const q16 spin[3]) {
  return detach(victim, joint, velocity, spin);
}

ScenePiece *battle_attach_head(Fighter *victim, Fighter *owner, int joint) {
  ScenePiece *head = detach(victim, 2, (q16[3]){0}, (q16[3]){0});
  if (head != NULL) {
    head->attached_to = owner == &g_battle.fighters[0] ? 0 : 1;
    head->joint = joint;
  }
  return head;
}

static void attachment_pose(SceneAttachment *attachment) {
  const AttachmentDef *def = attachment->def;
  const Frame *frame = &g_battle.assets->clips.data[def->clip].frames.data[attachment->sample];
  ScenePiece *piece = &g_battle.pieces[attachment->piece];
  Vec3 position = vec_add(attachment->origin, mat3_apply(attachment->orientation, vec_add(frame->translation, def->piece_offset)));
  for (int k = 0; k < 3; ++k) {
    piece->pos[k] = (q16)((k == 0 ? position.x : k == 1 ? position.y : position.z) * Q16_ONE);
  }
  piece->orientation = mat3_mul(attachment->orientation, rotation(def->piece_kind, frame->angles.data[0]));
}

void battle_attach_piece_model(ScenePiece *piece, const AttachmentDef *def, const Fighter *actor) {
  if (piece == NULL) {
    return;
  }
  require(def != NULL && def->first <= def->last && def->loop_from >= def->first && def->loop_from <= def->last, "Invalid animated attachment");
  for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
    if (g_battle.attachments[i].def == NULL) {
      g_battle.attachments[i] = (SceneAttachment){.def = def, .piece = (int)(piece - g_battle.pieces), .sample = def->first};
      SceneAttachment *attachment = &g_battle.attachments[i];
      attachment->origin = vec_add((Vec3){fighter_x(actor), q16_float(actor->body.pos[AXIS_Y]), fighter_z(actor)}, def->group_offset);
      attachment->orientation = fighter_turn(actor->assets, actor);
      piece->animated = true;
      piece->attached_to = -1;
      attachment_pose(attachment);
      return;
    }
  }
}

/* 41b610 -> 419720/7e0/8a0/900/9e0/aa0/b60.
 * First six words of each 4d28e8..4d2b58 record: linear/angular speed.
 * Bodies use our bounded piece pool; the native's task tree is unnecessary. */
void battle_shatter(Fighter *victim) {
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
    detach(victim, launches[i].joint, launches[i].velocity, launches[i].spin);
  }
  if (victim->assets != NULL) {
    const Character *ch = fighter_model(victim->assets, victim);
    for (size_t i = 0; i < ch->bones.count; ++i) {
      /* Remaining hierarchy follows 419470's final body launch. */
      detach(victim, ch->bones.data[i].id, (q16[3]){0x666, -0x1c28, 0xccc}, (q16[3]){0, 0x3333, -0x51e});
    }
  }
  victim->appearance = APPEARANCE_HIDDEN;
}

void battle_step_presentation(void) {
  for (int i = 0; i < MAX_JOINT_SPRAYS; ++i) {
    if (g_battle.sprays[i].def != NULL) {
      joint_spray_step(&g_battle.sprays[i]);
    }
  }
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *object = &g_battle.projectiles[i];
    if (object->kind != 0 && object->light.def != NULL) {
      orbit_light_step(&object->light);
    }
  }
  for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
    SceneAttachment *attachment = &g_battle.attachments[i];
    if (attachment->def != NULL) {
      if (!g_battle.pieces[attachment->piece].active) {
        attachment->def = NULL;
      } else {
        attachment->sample = attachment->sample < attachment->def->last ? attachment->sample + 1 : attachment->def->loop_from;
        attachment_pose(attachment);
      }
    }
  }
  step_follow_view();
  SceneCamera *camera = &g_battle.camera;
  if (camera->shake_pulses > 0 && --camera->shake_ticks == 0) {
    if (camera->shake_y != 0) {
      camera->shake_y = 0;
    } else if (--camera->shake_pulses > 0) {
      camera->shake_y = camera->shake_amount;
    }
    camera->shake_ticks = camera->shake_interval;
  }
  if (camera->active && camera->shots != NULL) {
    const CameraShot *shot = &camera->shots[camera->shot];
    q16 end = (shot->count - 1) * Q16_ONE;
    if (camera->parameter < end) {
      camera->parameter = mini(end, camera->parameter + shot->step);
      camera->eye = camera_sample(camera);
    } else if (camera->shot + 1 < camera->shot_count && ++camera->hold >= shot->hold) {
      camera->shot++;
      camera_shot();
    }
  }
  if (camera->active && camera->ticks < camera->duration) {
    float t = (float)++camera->ticks / camera->duration;
    camera->target = vec_add(camera->from_target, vec_scale(vec_sub(camera->to_target, camera->from_target), t));
  }
  step_blood();
  bool splash_sound = false;
  for (int i = 0; i < MAX_PARTICLES; ++i) {
    Particle *p = &g_battle.particles[i];
    if (p->ticks > 0) {
      --p->ticks;
      p->age++;
      if (p->visual != NULL) {
        p->scale = maxi(0, p->scale - p->visual->shrink);
      }
      p->vel[AXIS_Y] += p->gravity;
      for (int k = 0; k < 3; ++k) {
        p->pos[k] += p->vel[k];
      }
      if (p->splash && p->pos[AXIS_Y] >= 0 && p->vel[AXIS_Y] > 0) {
        /* 4745d0: floor contact replaces the drop with 4e7ab0's
         * nine two-tick meshes and a final one-tick tail. */
        static const ParticleVisual splash = {.animation = {.first = 4, .count = 9, .period = 2, .scale = Q16_ONE}};
        p->visual = &splash;
        p->pos[AXIS_Y] = -0x7ae;
        memset(p->vel, 0, sizeof p->vel);
        p->gravity = p->age = 0;
        p->ticks = 19;
        p->scale = Q16_ONE;
        p->splash = false;
        if (!splash_sound) {
          battle_sound(SOUND_B, 5 + (i % 4));
          splash_sound = true;
        }
      }
    }
  }
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    ScenePiece *piece = &g_battle.pieces[i];
    if (!piece->active || piece->animated) {
      continue;
    }
    if (piece->lifetime > 0 && --piece->lifetime == 0) {
      piece->active = false;
      continue;
    }
    if (piece->attached_to >= 0) {
      Vec3 position;
      fighter_joint_position(&g_battle.fighters[piece->attached_to], piece->joint, &position, &piece->orientation);
      piece->pos[0] = (q16)(position.x * Q16_ONE);
      piece->pos[1] = (q16)(position.y * Q16_ONE);
      piece->pos[2] = (q16)(position.z * Q16_ONE);
      continue;
    }
    if (piece->pos[AXIS_Y] < -0x1999 || piece->vel[AXIS_Y] < 0) {
      piece->vel[AXIS_Y] += piece->gravity != 0 ? piece->gravity : 0x147; /* 4d0364 default, used by419040 */
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
