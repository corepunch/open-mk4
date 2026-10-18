#include "game.h"
#include "sprite_font.h"
#include "shadow.h"
#include "lighting.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The fight camera keeps a fixed downward tilt along its horizontal
 * heading; scene shots use their full eye/target basis. */
typedef struct Camera {
  float focal, shake_y;
  int w, h;
  bool scene;
  Vec3 eye, heading, right, up, forward;
} Camera;

static float dot(Vec3 a, Vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 camera_point(Camera c, Vec3 p) {
  if (c.scene) {
    Vec3 relative = vec_sub(p, c.eye);
    return (Vec3){dot(relative, c.right), dot(relative, c.up), dot(relative, c.forward)};
  }
  Vec3 relative = vec_sub(p, c.eye);
  float y = relative.y, d = dot(relative, c.heading);
  return (Vec3){dot(relative, c.right), y * .978f - d * .208f, y * .208f + d * .978f};
}

static ScreenVertex camera_project(Camera c, Vertex v, float light) {
  return (ScreenVertex){(float)c.w * .5f + v.p.x * c.focal / v.p.z, (float)c.h * .47f + v.p.y * c.focal / v.p.z, 1.f / v.p.z, v.u, v.v, light, 1.f / v.p.z};
}

static void render_triangle(Canvas *cv, Camera camera, Triangle t, const Texture *texture, float brightness, TrianglePass pass, const SceneLight *light) {
  typedef struct LitVertex {
    Vertex v;
    float light;
  } LitVertex;

  LitVertex in[3], out[6];
  size_t count = 0;
  for (size_t i = 0; i < 3; ++i) {
    Vertex v = t.v[i];
    float shade = brightness * (light ? light_shade(*light, v.p, v.normal) : 1.f);
    v.p = camera_point(camera, v.p);
    in[i] = (LitVertex){v, shade};
  }
  /* Clip position, UV and brightness together before projection. */
  for (size_t i = 0; i < 3; ++i) {
    LitVertex a = in[i], b = in[(i + 1) % 3];
    bool va = a.v.p.z >= .4f, vb = b.v.p.z >= .4f;
    if (va)
      out[count++] = a;
    if (va != vb) {
      float u = (.4f - a.v.p.z) / (b.v.p.z - a.v.p.z);
      LitVertex v = {0};
      v.v.p = vec_add(a.v.p, vec_scale(vec_sub(b.v.p, a.v.p), u));
      v.v.u = a.v.u + (b.v.u - a.v.u) * u;
      v.v.v = a.v.v + (b.v.v - a.v.v) * u;
      v.light = a.light + (b.light - a.light) * u;
      out[count++] = v;
    }
  }
  for (size_t i = 1; i + 1 < count; ++i) {
    ScreenVertex v[] = {
        camera_project(camera, out[0].v, out[0].light), camera_project(camera, out[i].v, out[i].light), camera_project(camera, out[i + 1].v, out[i + 1].light)};
    canvas_triangle(cv, v, texture, (Rect){0, 0, cv->w, cv->h}, false, pass);
  }
}

static float draw_arena(Canvas *cv, Camera camera, const Arena *arena, SceneLight light) {
  float floor_y = INFINITY;
  Frame frame = {0};
  Clip bind = {.frames = {&frame, 1, 1}};
  Character rig = {.bones = arena->nodes};
  Pose nodes[MK4_MAX_BONES];
  pose(&rig, &bind, 0, nodes);
  for (size_t i = 0; i < arena->nodes.count; ++i) {
    const Bone *n = &arena->nodes.data[i];
    if (n->flags & 4)
      continue;
    const Triangles *triangles = &arena->geometry.meshes.data[n->mesh].triangles;
    for (size_t j = 0; j < triangles->count; ++j) {
      Triangle t = triangles->data[j];
      bool floor = true;
      for (size_t k = 0; k < 3; ++k) {
        Vertex *v = &t.v[k];
        v->p = vec_add(nodes[i].position, mat3_apply(nodes[i].rotation, v->p));
        v->normal = mat3_apply(nodes[i].rotation, v->normal);
        floor &= fabsf(v->p.y) < .05f;
      }
      if (floor)
        for (size_t k = 0; k < 3; ++k)
          floor_y = fminf(floor_y, t.v[k].p.y);
      render_triangle(cv, camera, t, &arena->geometry.textures.data[t.texture], 1, floor ? PASS_SHADOW_RECEIVER : PASS_COLOR, &light);
    }
  }
  return isfinite(floor_y) ? floor_y : 0.f;
}

static Triangles fighter_geometry(const Assets *assets, const Fighter *fighter, const Character *model, double interpolation) {
  Triangles triangles = {0};
  const Character *ch = model != NULL ? model : fighter_model(assets, fighter);
  if (fighter->appearance == APPEARANCE_HIDDEN) {
    return triangles;
  }
  Pose bones[MK4_MAX_BONES];
  if (model != NULL) {
    pose(ch, fighter_clip(assets, fighter), fighter_pose_frame(fighter), bones);
  } else {
    fighter_render_pose(assets, fighter, interpolation, bones);
  }
  Mat3 turn = fighter_turn(assets, fighter);
  /* The pose keeps every root-motion channel, as 0x4b8140 does for joints.
   * Y plants the sole: uppercut 293 opens near +0.41, crouch 259 settles
   * near +0.44 and knockdown 186 near +0.97. X/Z keep planted feet still:
   * idle root X/Z swings up to 0.25 while the soles move under 0.06. */
  for (size_t n = 0; n < ch->bones.count; ++n) {
    const Bone *b = &ch->bones.data[n];
    if (b->flags & 4)
      continue;
    /* Retail subtree masking hides all meshes descending from the selected
     * joint, while their transforms still pose independent scene bodies. */
    bool hidden = false;
    for (int ancestor = (int)n; ancestor >= 0; ancestor = ch->bones.data[ancestor].parent) {
      int id = ch->bones.data[ancestor].id;
      if (id >= 0 && id < 128 && (fighter->hidden_joints[id / 64] & (UINT64_C(1) << (id % 64)))) {
        hidden = true;
        break;
      }
    }
    if (hidden) {
      continue;
    }
    const Triangles *mesh = &ch->meshes.data[b->mesh].triangles;
    for (size_t j = 0; j < mesh->count; ++j) {
      Triangle t = mesh->data[j];
      for (size_t k = 0; k < 3; ++k) {
        Vertex *v = &t.v[k];
        Vec3 local = vec_add(bones[n].position, mat3_apply(bones[n].rotation, v->p));
        Vec3 bind = ch->bones.data[0].translation;
        if (fighter->appearance != APPEARANCE_BEAST) {
          local = vec_sub(local, bind);
          local.y += q16_float(fighter->body.root_y);
        }
        Vec3 p = mat3_apply(turn, local);
        /* 0x41ea50 places the creature root at absolute Y=-0.9. Human
         * rigs carry their ground offset in their bind root instead. */
        if (fighter->appearance == APPEARANCE_BEAST) {
          p.y -= .9f;
        }
        float y = fighter->appearance == APPEARANCE_BEAST ? -fighter_height(fighter) : q16_float(fighter->body.pos[AXIS_Y]);
        v->p = (Vec3){p.x + fighter_x(fighter), p.y + y, p.z + fighter_z(fighter)};
        v->p = vec_add(v->p, (Vec3){q16_float(fighter->visual_offset[0]), q16_float(fighter->visual_offset[1]), q16_float(fighter->visual_offset[2])});
        v->normal = mat3_apply(turn, mat3_apply(bones[n].rotation, v->normal));
      }
      PUSH(triangles, t);
    }
  }
  return triangles;
}

static void round_intro(Canvas *cv, const Hud *hud, int round) {
  const Sprite *word = &hud->effects.sprites.data[138];
  const int scale = 2, gap = 18;
  char number[32];
  snprintf(number, sizeof number, "%d", round);
  int digit = round >= 1 && round <= 3 ? 138 + round : -1;
  int width = digit >= 0 ? hud->effects.sprites.data[digit].width * scale : font_width(&hud->font, number, scale);
  int x = (cv->w - word->width * scale - gap - width) / 2;
  draw_sprite(cv, &hud->effects, 138, (Rect){x, 205, word->width * scale, word->height * scale}, false);
  x += word->width * scale + gap;
  if (digit >= 0) {
    const Sprite *s = &hud->effects.sprites.data[digit];
    draw_sprite(cv, &hud->effects, digit, (Rect){x, 205, s->width * scale, s->height * scale}, false);
  } else
    font_draw(&hud->font, cv, x, 216, number, scale);
}

static void bar(Canvas *cv, const Geometry *font, q16 health, Rect r, bool right) {
  const Sprite *s = &font->sprites.data[25];
  canvas_blit(cv, &font->textures.data[s->texture], (Rect){s->u, s->v, s->w, 8}, r, right);
  int inner = r.w - 6, missing = (int)((int64_t)inner * (Q16_ONE - health) / Q16_ONE);
  canvas_rect(cv, (Rect){right ? r.x + 3 : r.x + r.w - 3 - missing, r.y + 3, missing, r.h - 6}, 0xff292320);
}

void draw_sprite(Canvas *cv, const Geometry *geo, int index, Rect dst, bool flip) {
  if (index < 0 || (size_t)index >= geo->sprites.count)
    return;
  const Sprite *s = &geo->sprites.data[index];
  if (!s->w || !s->h)
    return;
  canvas_blit(cv, &geo->textures.data[s->texture], (Rect){s->u, s->v, s->w, s->h}, dst, flip);
}

static void draw_shared_mesh(Canvas *cv, Camera camera, const Geometry *geometry, int index, Vec3 origin, Mat3 basis, float scale, const SceneLight *light) {
  require(index >= 0 && (size_t)index < geometry->meshes.count, "Effect mesh outside shared geometry");
  const Mesh *mesh = &geometry->meshes.data[index];
  TrianglePass pass = (mesh->flags & 0x180) == 0x100 ? PASS_ADDITIVE : PASS_COLOR;
  for (size_t j = 0; j < mesh->triangles.count; ++j) {
    Triangle triangle = mesh->triangles.data[j];
    for (int k = 0; k < 3; ++k) {
      triangle.v[k].p = vec_add(origin, mat3_apply(basis, vec_scale(triangle.v[k].p, scale)));
      triangle.v[k].normal = mat3_apply(basis, triangle.v[k].normal);
    }
    render_triangle(cv, camera, triangle, &geometry->textures.data[triangle.texture], 1, pass, light);
  }
}

static void draw_billboard(Canvas *cv, Camera camera, const Geometry *geometry, int index, Vec3 origin, float scale, bool additive) {
  require(index >= 0 && (size_t)index < geometry->sprites.count, "Effect sprite outside shared geometry");
  const Sprite *sprite = &geometry->sprites.data[index];
  Vec3 center = camera_point(camera, origin);
  if (center.z < .4f || sprite->w <= 0 || sprite->h <= 0) {
    return;
  }
  float x = center.x + sprite->origin_x * scale / 256.f, y = center.y + sprite->origin_y * scale / 256.f;
  float width = sprite->width * scale / 256.f, height = sprite->height * scale / 256.f;
  ScreenVertex corners[4];
  for (int i = 0; i < 4; ++i) {
    bool right = i & 1, down = i & 2;
    Vertex vertex = {.p = {x + (right ? width : 0), y + (down ? height : 0), center.z},
        .u = (float)(sprite->u + (right ? sprite->w - 1 : 0)),
        .v = (float)(sprite->v + (down ? sprite->h - 1 : 0))};
    corners[i] = camera_project(camera, vertex, 1);
  }
  ScreenVertex triangles[2][3] = {{corners[0], corners[1], corners[2]}, {corners[2], corners[1], corners[3]}};
  for (int i = 0; i < 2; ++i) {
    canvas_triangle(cv, triangles[i], &geometry->textures.data[sprite->texture], (Rect){0, 0, cv->w, cv->h}, false, additive ? PASS_ADDITIVE : PASS_COLOR);
  }
}

/* Points a mesh's local +Y along its velocity. The yaw matrix turns +Z
 * toward the heading, so the pitch about X is negative to tip +Y forward
 * rather than backward. */
static Mat3 velocity_basis(const q16 vel[3]) {
  float horizontal = hypotf(q16_float(vel[0]), q16_float(vel[2]));
  if (horizontal <= .0001f && vel[1] == 0) {
    return mat3_identity();
  }
  float pitch = atan2f(-horizontal, q16_float(vel[1]));
  float yaw = atan2f(q16_float(vel[0]), q16_float(vel[2]));
  return mat3_mul(yaw_matrix(yaw), rotation(0, (Vec3){pitch * 4096.f / 6.283185307179586f, 0, 0}));
}

/* Shared blood drop meshes 0-3 have a round head at -Y and their point at
 * +Y, so the point trails the drop's flight. */
static Mat3 drop_basis(const q16 vel[3]) {
  return velocity_basis((q16[3]){-vel[0], -vel[1], -vel[2]});
}

/* A filtered, alpha-blended camera-facing sprite, half wide in world units. */
static void draw_blend_billboard(Canvas *cv, Camera camera, const Geometry *geometry, int index, Vec3 origin, float half, uint32_t tint) {
  require(index >= 0 && (size_t)index < geometry->sprites.count, "Effect sprite outside shared geometry");
  const Sprite *sprite = &geometry->sprites.data[index];
  Vec3 center = camera_point(camera, origin);
  if (center.z < .4f || sprite->w <= 0 || sprite->h <= 0) {
    return;
  }
  float tall = half * (float)sprite->h / (float)sprite->w;
  ScreenVertex corners[4];
  for (int i = 0; i < 4; ++i) {
    bool right = i & 1, down = i & 2;
    Vertex vertex = {.p = {center.x + (right ? half : -half), center.y + (down ? tall : -tall), center.z},
        .u = (float)(sprite->u + (right ? sprite->w : 0)),
        .v = (float)(sprite->v + (down ? sprite->h : 0))};
    corners[i] = camera_project(camera, vertex, 1);
  }
  const Texture *texture = &geometry->textures.data[sprite->texture];
  canvas_blend_triangle(cv, (ScreenVertex[3]){corners[0], corners[1], corners[2]}, texture, tint);
  canvas_blend_triangle(cv, (ScreenVertex[3]){corners[2], corners[1], corners[3]}, texture, tint);
}

/* The retail splat sprites are drawn as seen from the fight camera, so
 * stretch their depth back out before laying them flat. The sprite's top
 * edge stays farthest from the camera, and stains fade over their last
 * second. */
static void draw_blood_splats(Canvas *cv, Camera camera, const Assets *assets, float floor_y) {
  const Geometry *effects = &assets->hud.effects;
  for (int i = 0; i < MAX_BLOOD_SPLATS; ++i) {
    const BloodSplat *splat = &g_battle.splats[i];
    if (splat->ticks <= 0) {
      continue;
    }
    const Sprite *sprite = &effects->sprites.data[splat->sprite];
    Vec3 across = {q16_float(splat->axis[0]), 0, q16_float(splat->axis[1])};
    if (dot(across, camera.right) < 0) {
      across = vec_scale(across, -1);
    }
    Vec3 toward = {-across.z, 0, across.x};
    float half = q16_float(splat->half), deep = half * (float)sprite->h / (float)sprite->w / .4f;
    Vec3 center = {q16_float(splat->pos[0]), floor_y - .005f, q16_float(splat->pos[1])};
    ScreenVertex corners[4];
    bool visible = true;
    for (int k = 0; k < 4; ++k) {
      bool right = k & 1, near = k & 2;
      Vec3 world = vec_add(center, vec_add(vec_scale(across, right ? half : -half), vec_scale(toward, near ? deep : -deep)));
      Vertex vertex = {.p = camera_point(camera, world), .u = (float)(sprite->u + (right ? sprite->w : 0)), .v = (float)(sprite->v + (near ? sprite->h : 0))};
      visible &= vertex.p.z >= .4f;
      corners[k] = camera_project(camera, vertex, 1);
    }
    if (!visible) {
      continue;
    }
    unsigned alpha = (unsigned)mini(224, splat->ticks * 224 / 60);
    const Texture *texture = &effects->textures.data[sprite->texture];
    canvas_blend_triangle(cv, (ScreenVertex[3]){corners[0], corners[1], corners[2]}, texture, alpha << 24 | 0xd8d8d8);
    canvas_blend_triangle(cv, (ScreenVertex[3]){corners[2], corners[1], corners[3]}, texture, alpha << 24 | 0xd8d8d8);
  }
}

/* Hit blood from battle_blood_hit: retail drop mesh 0 in flight, crown
 * meshes 4-12 on landing and spatter sprite 153 at the impact. */
static void draw_blood(Canvas *cv, Camera camera, const Assets *assets, const SceneLight *light) {
  const Geometry *effects = &assets->hud.effects;
  for (int i = 0; i < MAX_BLOOD_DROPS; ++i) {
    const BloodDrop *drop = &g_battle.blood[i];
    Vec3 origin = {q16_float(drop->pos[0]), q16_float(drop->pos[1]), q16_float(drop->pos[2])};
    float scale = q16_float(drop->scale);
    if (drop->kind == BLOOD_DROP) {
      draw_shared_mesh(cv, camera, effects, 0, origin, drop_basis(drop->vel), scale, light);
    } else if (drop->kind == BLOOD_SPLASH) {
      draw_shared_mesh(cv, camera, effects, 4 + mini(8, drop->age / 2), origin, mat3_identity(), scale, light);
    } else if (drop->kind == BLOOD_SPATTER) {
      float t = (float)drop->age / (float)(drop->age + drop->ticks);
      unsigned alpha = (unsigned)(255.f * (1.f - t * t));
      draw_blend_billboard(cv, camera, effects, 153, origin, scale * (.16f + .3f * t), alpha << 24 | 0xffffff);
    }
  }
}

static void draw_weapon(Canvas *cv, Camera camera, const Assets *assets, Vec3 origin, Mat3 basis, const WeaponDef *weapon, const SceneLight *light) {
  require(weapon != NULL && weapon->model >= 0 && (size_t)weapon->model < assets->scene_models.count, "Invalid weapon model");
  const Character *model = &assets->scene_models.data[weapon->model];
  Pose bones[MK4_MAX_BONES];
  pose_static(model, bones);
  for (size_t n = 0; n < model->bones.count; ++n) {
    const Bone *bone = &model->bones.data[n];
    const Triangles *mesh = &model->meshes.data[bone->mesh].triangles;
    for (size_t j = 0; j < mesh->count; ++j) {
      Triangle triangle = mesh->data[j];
      for (int k = 0; k < 3; ++k) {
        Vec3 local = vec_add(bones[n].position, mat3_apply(bones[n].rotation, triangle.v[k].p));
        triangle.v[k].p = vec_add(origin, mat3_apply(basis, local));
        triangle.v[k].normal = mat3_apply(basis, mat3_apply(bones[n].rotation, triangle.v[k].normal));
      }
      render_triangle(cv, camera, triangle, character_texture(model, triangle.texture), 1, PASS_COLOR, light);
    }
  }
}

/* Animated accessories reserve their first channel for the detached
 * parent piece; the model's remaining channels pose its own hierarchy. */
static Mat3 piece_basis(const Assets *assets, int index) {
  (void)assets;
  const ScenePiece *piece = &g_battle.pieces[index];
  float factor = 4096.f / (6.283185307179586f * Q16_ONE);
  Mat3 spin = rotation(0, (Vec3){piece->angle[0] * factor, piece->angle[1] * factor, piece->angle[2] * factor});
  return mat3_mul(spin, piece->orientation);
}

static void draw_attachments(Canvas *cv, Camera camera, const Assets *assets, const SceneLight *light) {
  for (int i = 0; i < MAX_SCENE_ATTACHMENTS; ++i) {
    const SceneAttachment *attachment = &g_battle.attachments[i];
    if (attachment->def == NULL) {
      continue;
    }
    const AttachmentDef *def = attachment->def;
    const ScenePiece *piece = &g_battle.pieces[attachment->piece];
    const Character *model = &assets->scene_models.data[def->model];
    const Clip *clip = &assets->clips.data[def->clip];
    Pose bones[MK4_MAX_BONES];
    pose(model, clip, attachment->sample, bones);
    /* 446730 installs the inverse-parent transform on the accessory
     * root. Its authored motion stays in the scene frame while the
     * detached head follows the first channel's own rotation. */
    Mat3 basis = def->scene_space ? attachment->orientation : piece_basis(assets, attachment->piece);
    Vec3 origin = {q16_float(piece->pos[0]), q16_float(piece->pos[1]), q16_float(piece->pos[2])};
    Vec3 root = clip->frames.data[attachment->sample].translation;
    for (size_t n = 0; n < model->bones.count; ++n) {
      const Bone *bone = &model->bones.data[n];
      const Triangles *mesh = &model->meshes.data[bone->mesh].triangles;
      for (size_t j = 0; j < mesh->count; ++j) {
        Triangle t = mesh->data[j];
        for (int k = 0; k < 3; ++k) {
          Vec3 local = vec_sub(vec_add(bones[n].position, mat3_apply(bones[n].rotation, t.v[k].p)), root);
          t.v[k].p = vec_add(origin, mat3_apply(basis, local));
          t.v[k].normal = mat3_apply(basis, mat3_apply(bones[n].rotation, t.v[k].normal));
        }
        render_triangle(cv, camera, t, character_texture(model, t.texture), 1, PASS_COLOR, light);
      }
    }
  }
}

void draw_battle(Canvas *cv, const Assets *assets, int arena, bool cpu, bool paused, double interpolation) {
  battle_capture_render_snapshot();
  double animation_fraction = 1.0;
  if (g_battle.current_animation.valid) {
    double ticks_since_snapshot = (double)(g_battle.ticks - g_battle.current_animation.tick);
    animation_fraction = fmax(0., fmin(1., (ticks_since_snapshot + fmax(0., fmin(1., interpolation))) / 2.0));
  }
  canvas_clear(cv);
  const Fighter *a = &g_battle.fighters[0], *b = &g_battle.fighters[1];
  Camera camera = {.focal = (float)cv->w * .95f, .w = cv->w, .h = cv->h};
  camera.eye = battle_follow_eye(&camera.heading);
  camera.right = (Vec3){-camera.heading.z, 0, camera.heading.x};
  if (g_battle.camera.active) {
    camera.scene = true;
    if (g_battle.camera.focal_scale > 0) {
      camera.focal *= q16_float(g_battle.camera.focal_scale);
    }
    camera.eye = g_battle.camera.eye;
    Vec3 direction = vec_sub(g_battle.camera.target, camera.eye);
    float length = sqrtf(dot(direction, direction));
    camera.forward = vec_scale(direction, 1.f / fmaxf(.001f, length));
    float horizontal = hypotf(direction.x, direction.z);
    camera.right = (Vec3){-direction.z / fmaxf(.001f, horizontal), 0, direction.x / fmaxf(.001f, horizontal)};
    camera.up =
        (Vec3){-camera.forward.y * camera.right.z, camera.forward.x * camera.right.z - camera.forward.z * camera.right.x, camera.forward.y * camera.right.x};
  }
  camera.shake_y = q16_float(g_battle.camera.shake_y);
  camera.eye.y += camera.shake_y;
  SceneLight light = above_camera_light(camera.eye);
  float floor_y = draw_arena(cv, camera, &assets->arenas.data[arena], light);
  Triangles fighters[] = {fighter_geometry(assets, a, NULL, animation_fraction), fighter_geometry(assets, b, NULL, animation_fraction)};
  Mat4 shadow;
  require(planar_shadow_matrix((Vec4){0, 1, 0, -floor_y}, light_homogeneous(light), &shadow), "Shadow light must intersect the receiver plane");
  for (size_t i = 0; i < 2; ++i) {
    const Texture *texture = &fighter_model(assets, &g_battle.fighters[i])->texture;
    for (size_t j = 0; j < fighters[i].count; ++j) {
      Triangle triangle = fighters[i].data[j];
      bool valid = true;
      for (size_t k = 0; k < 3; ++k)
        valid &= mat4_project(shadow, triangle.v[k].p, &triangle.v[k].p);
      if (valid)
        render_triangle(cv, camera, triangle, texture, 1, PASS_SHADOW_MASK, NULL);
    }
  }
  canvas_resolve_shadows(cv, 104);
  draw_blood_splats(cv, camera, assets, floor_y);
  uint32_t ice_pixel = 0xff89dfff;
  const Texture ice_texture = {1, 1, {&ice_pixel, 1, 1}};
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    const ScenePiece *piece = &g_battle.pieces[i];
    if (!piece->active) {
      continue;
    }
    const Character *ch = piece->frozen ? &assets->cut_characters.data[roster_def(piece->roster)->asset] : roster_character(assets, piece->roster);
    const Bone *bone = &ch->bones.data[piece->bone];
    Mat3 transform = piece_basis(assets, i);
    const Triangles *mesh = &ch->meshes.data[bone->mesh].triangles;
    for (size_t n = 0; n < mesh->count; ++n) {
      Triangle triangle = mesh->data[n];
      for (int k = 0; k < 3; ++k) {
        triangle.v[k].p = vec_add(mat3_apply(transform, triangle.v[k].p), (Vec3){q16_float(piece->pos[0]), q16_float(piece->pos[1]), q16_float(piece->pos[2])});
        triangle.v[k].normal = mat3_apply(transform, triangle.v[k].normal);
      }
      render_triangle(cv, camera, triangle, piece->frozen ? &ice_texture : &ch->texture, 1, PASS_COLOR, &light);
    }
  }
  draw_attachments(cv, camera, assets, &light);
  for (size_t i = 0; i < 2; ++i) {
    const Fighter *fighter = &g_battle.fighters[i];
    const Texture *texture = fighter->appearance == APPEARANCE_FROZEN ? &ice_texture : &fighter_model(assets, fighter)->texture;
    for (size_t j = 0; j < fighters[i].count; ++j)
      render_triangle(cv, camera, fighters[i].data[j], texture,
          fighter->appearance == APPEARANCE_BURNED ? .18f
          : fighter->state == S_HIT                ? .72f
                                                   : 1.f,
          PASS_COLOR, &light);
    FREE_ARRAY(fighters[i]);
  }
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    const BattleEvent *event = &g_battle.events[i];
    if (!battle_shadow_actor_event(event)) {
      continue;
    }
    const Fighter *owner = &g_battle.fighters[event->owner];
    Fighter actor = *owner;
    MoveDef move = {.clip = event->data[SHADOW_DATA_CLIP], .last = MOVE_END};
    actor.current_move = &move;
    actor.sample_override = event->data[SHADOW_DATA_SAMPLE];
    actor.blend_duration = 0;
    actor.action = 0;
    actor.facing = owner->facing;
    actor.body.pos[AXIS_X] = event->data[SHADOW_DATA_X];
    actor.body.pos[AXIS_Y] = event->data[SHADOW_DATA_Y];
    actor.body.pos[AXIS_Z] = event->data[SHADOW_DATA_Z];
    actor.visual_offset[1] = 0;
    const Character *model = fighter_model(assets, owner);
    Triangles geometry = fighter_geometry(assets, &actor, model, animation_fraction);
    for (size_t j = 0; j < geometry.count; ++j) {
      render_triangle(cv, camera, geometry.data[j], &model->texture, .38f, PASS_COLOR, &light);
    }
    FREE_ARRAY(geometry);
  }
  for (int i = 0; i < MAX_SCENE_ACTORS; ++i) {
    const SceneActor *slot = &g_battle.scene_actors[i];
    if (!slot->active) {
      continue;
    }
    const Fighter *actor = &slot->actor;
    const Character *model = fighter_model(assets, actor);
    Triangles geometry = fighter_geometry(assets, actor, model, animation_fraction);
    for (size_t j = 0; j < geometry.count; ++j) {
      render_triangle(cv, camera, geometry.data[j], &model->texture, 1.f, PASS_COLOR, &light);
    }
    FREE_ARRAY(geometry);
  }
  /* Script objects are rendered from simulation state; rendering never advances them. */
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_CLONE) {
      Fighter clone = g_battle.fighters[p->owner];
      MoveDef captured = {.clip = p->clip, .last = MOVE_END};
      clone.current_move = &captured;
      clone.appearance = APPEARANCE_NORMAL;
      clone.blend_duration = 0;
      clone.action = 0;
      clone.sample_override = p->sample;
      clone.facing = p->facing;
      memcpy(clone.heading, p->heading, sizeof clone.heading);
      memcpy(clone.body.angle, p->angle, sizeof clone.body.angle);
      clone.body.root_y = p->root_y;
      memcpy(clone.body.pos, p->pos, sizeof clone.body.pos);
      const Character *model = &assets->scene_models.data[SCENE_MODEL_ICE_CLONE];
      Triangles geometry = fighter_geometry(assets, &clone, model, animation_fraction);
      SceneLight clone_light = light;
      if (p->light.def != NULL) {
        clone_light.position = (Vec3){q16_float(p->light.pos[0]), q16_float(p->light.pos[1]), q16_float(p->light.pos[2])};
        clone_light.diffuse *= clampf(q16_float(p->light.strength), .3125f, 1.f);
      }
      for (size_t j = 0; j < geometry.count; ++j) {
        render_triangle(cv, camera, geometry.data[j], character_texture(model, geometry.data[j].texture), 1, PASS_COLOR, &clone_light);
      }
      FREE_ARRAY(geometry);
    }
    if (p->mesh_animation != NULL) {
      float x = q16_float(p->heading[0]), z = q16_float(p->heading[1]);
      Mat3 basis = {{x, 0, -z, 0, 1, 0, z, 0, x}};
      Vec3 origin = {q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])};
      float angle_factor = 4096.f / (6.283185307179586f * Q16_ONE);
      basis = mat3_mul(basis, rotation(0, (Vec3){p->angle[0] * angle_factor, p->angle[1] * angle_factor, p->angle[2] * angle_factor}));
      const MeshAnimation *animation = p->mesh_animation;
      Vec3 offset = {q16_float(animation->offset[0]), q16_float(animation->offset[1]), q16_float(animation->offset[2])};
      origin = vec_add(origin, mat3_apply(basis, vec_scale(offset, q16_float(animation->scale))));
      const Geometry *geometry = mesh_animation_geometry(assets, animation);
      int frame = projectile_mesh_frame(p);
      require(frame >= 0 && (size_t)frame < geometry->meshes.count, "Projectile frame outside geometry bank");
      if (geometry->sprites.data[frame].w > 0) {
        draw_billboard(cv, camera, geometry, frame, origin, q16_float(animation->scale), animation->additive);
      } else {
        draw_shared_mesh(cv, camera, geometry, frame, origin, basis, q16_float(animation->scale), NULL);
      }
    }
    if (p->kind == PROJECTILE_SWORD) {
      Vec3 base = {q16_float(p->pos[AXIS_X]), q16_float(p->pos[AXIS_Y]), q16_float(p->pos[AXIS_Z])};
      float factor = 4096.f / (6.283185307179586f * Q16_ONE);
      Mat3 turn = rotation(0, (Vec3){p->angle[0] * factor, p->angle[1] * factor, p->angle[2] * factor});
      float x = q16_float(p->heading[0]), z = q16_float(p->heading[1]);
      Mat3 heading = {{x, 0, -z, 0, 1, 0, z, 0, x}};
      draw_weapon(cv, camera, assets, base, mat3_mul(heading, turn), p->weapon, &light);
    }
    if (p->kind != PROJECTILE_SPEAR) {
      continue;
    }
    const Fighter *owner = &g_battle.fighters[p->owner];
    Vec3 hand = {fighter_x(owner), -1.4f - fighter_height(owner), fighter_z(owner)};
    Vec3 tip = {q16_float(p->pos[AXIS_X]), q16_float(p->pos[AXIS_Y]), q16_float(p->pos[AXIS_Z])};
    ScreenVertex from = camera_project(camera, (Vertex){.p = camera_point(camera, hand)}, 1);
    ScreenVertex to = camera_project(camera, (Vertex){.p = camera_point(camera, tip)}, 1);
    canvas_line(cv, (int)from.x, (int)from.y, (int)to.x, (int)to.y, 0xffc9a85c);
    canvas_rect(cv, (Rect){(int)to.x - 3, (int)to.y - 2, 7, 4}, 0xffdedede);
  }
  for (int i = 0; i < 2; ++i) {
    const Fighter *f = &g_battle.fighters[i];
    if (f->armed) {
      Vec3 hand;
      Mat3 grip;
      fighter_joint_position(f, 10, &hand, &grip);
      draw_weapon(cv, camera, assets, hand, grip, f->weapon, &light);
    }
    if ((fighter_move(f)->clip == 448 && f->script != NULL && f->ticks >= 13 && f->ticks < (f->script_phase ? 32 : 48)) ||
        (g_battle.phase == PHASE_FATALITY && g_battle.finisher == FINISH_BURN && i == g_battle.winner && g_battle.phase_ticks >= 51 &&
            g_battle.phase_ticks < 191)) {
      Vec3 mouth = {fighter_x(f), -1.65f - fighter_height(f), fighter_z(f)};
      Vec3 end = {mouth.x + f->facing * 2.3f, mouth.y + .25f, mouth.z};
      ScreenVertex from = camera_project(camera, (Vertex){.p = camera_point(camera, mouth)}, 1);
      ScreenVertex to = camera_project(camera, (Vertex){.p = camera_point(camera, end)}, 1);
      for (int spread = -8; spread <= 8; spread += 2) {
        canvas_line(cv, (int)from.x, (int)from.y, (int)to.x, (int)to.y + spread, spread % 4 ? 0xffff7b24 : 0xffffd253);
      }
    }
  }
  for (int i = 0; i < MAX_PARTICLES; ++i) {
    const Particle *p = &g_battle.particles[i];
    if (p->ticks <= 0) {
      continue;
    }
    if (p->visual != NULL) {
      const ParticleVisual *visual = p->visual;
      int frame = visual->animation.first + mini(visual->animation.count - 1, p->age / visual->animation.period);
      Vec3 origin = {q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])};
      const Geometry *geometry = mesh_animation_geometry(assets, &visual->animation);
      if (visual->billboard) {
        draw_billboard(cv, camera, geometry, frame, origin, q16_float(p->scale), visual->additive);
      } else {
        draw_shared_mesh(cv, camera, geometry, frame, origin, frame <= 3 ? drop_basis(p->vel) : velocity_basis(p->vel), q16_float(p->scale), &light);
      }
      continue;
    }
  }
  draw_blood(cv, camera, assets, &light);
  for (size_t i = 0; i < 2; ++i) {
    const HitEffect *e = &g_battle.effects[i];
    if (e->ticks <= 0)
      continue;
    /* Lift the flash slightly toward the camera, ahead of both bodies. */
    Vec3 center = camera_point(camera, (Vec3){e->x, -e->height, e->z});
    center.z -= .2f;
    ScreenVertex p = camera_project(camera, (Vertex){.p = center}, 1);
    canvas_blit(cv, &assets->hud.effects.textures.data[2], (Rect){1, 1, 23, 23}, (Rect){(int)p.x - 26, (int)p.y - 26, 52, 52}, false);
  }
  const uint32_t gold = 0xffdcc798;
  const Geometry *font = &assets->hud.font;
  canvas_rect(cv, (Rect){0, 0, cv->w, 122}, 0xff141517);
  canvas_line(cv, 32, 121, cv->w - 32, 121, 0xff66533a);
  int bw = (cv->w - 250) / 2;
  Rect left = {38, 60, bw, 18}, right = {cv->w - 38 - bw, 60, bw, 18};
  bar(cv, font, a->health, left, false);
  bar(cv, font, b->health, right, true);
  for (int i = 0; i < 2; ++i) {
    const Fighter *f = &g_battle.fighters[i];
    int idx = assets->hud.names[roster_def(f->roster)->asset];
    Sprite s = font->sprites.data[idx];
    int x = i ? cv->w - 38 - s.w * 2 : 38;
    draw_sprite(cv, font, idx, (Rect){x, 31, s.w * 2, s.h * 2}, false);
    for (int win = 0; win < 2; ++win)
      canvas_rect(cv, (Rect){i ? cv->w - 52 - win * 21 : 38 + win * 21, 92, 13, 8}, win < f->wins ? gold : 0xff3b3830);
    int meter_x = i ? cv->w - 218 : 38;
    canvas_rect(cv, (Rect){meter_x, 83, 180, 4}, 0xff3b3830);
    int energy = (int)((int64_t)f->run_energy * 180 / Q16_ONE);
    canvas_rect(cv, (Rect){i ? meter_x + 180 - energy : meter_x, 83, energy, 4}, gold);
  }
  char timer[32], round[32];
  snprintf(timer, sizeof timer, "%02d", maxi(0, (g_battle.time_ticks + 59) / 60));
  snprintf(round, sizeof round, "ROUND %d", g_battle.round);
  font_centered(font, cv, 43, timer, 2);
  font_centered(font, cv, 96, round, .65f);
  canvas_rect(cv, (Rect){0, cv->h - 96, cv->w, 96}, 0xff141517);
  font_draw(font, cv, 28, cv->h - 79, assets->arenas.data[arena].name, 1);
  const char *mode = cpu ? "P1 VS CPU / F3 TWO PLAYERS" : "LOCAL TWO PLAYERS / F3 CPU";
  font_draw(font, cv, cv->w - 28 - font_width(font, mode, .6f), cv->h - 78, mode, .6f);
  font_draw(font, cv, 28, cv->h - 49, "P1  A/D MOVE  W JUMP  S DUCK  U HP  J LP  I HK  K LK  L BLOCK  H RUN", .6f);
  font_draw(font, cv, 28, cv->h - 29, "P2  ARROWS  NUM 1/2/3/4/5  RSHIFT RUN    F1/F2 FIGHTERS  F4 ARENA  R RESET  SPACE PAUSE", .6f);
  if (g_battle.phase == PHASE_READY) {
    round_intro(cv, &assets->hud, g_battle.round);
    if (g_battle.phase_ticks > 40)
      draw_sprite(cv, &assets->hud.effects, 137, (Rect){cv->w / 2 - 181, 285, 362, 126}, false);
  }
  if (g_battle.phase == PHASE_FINISH || g_battle.phase == PHASE_FATALITY) {
    font_centered(font, cv, 165, g_battle.phase == PHASE_FINISH ? "FINISH HIM" : "FATALITY", 1.5f);
  }
  if (g_battle.phase == PHASE_ROUND_OVER || g_battle.phase == PHASE_MATCH_OVER) {
    char message[1100];
    if (g_battle.winner < 0)
      snprintf(message, sizeof message, "DRAW");
    else
      snprintf(message, sizeof message, g_battle.finisher ? "%s WINS / FATALITY" : "%s WINS",
          roster_character(assets, g_battle.fighters[g_battle.winner].roster)->name);
    canvas_rect(cv, (Rect){cv->w / 2 - 350, 205, 700, 100}, 0xff141517);
    font_centered(font, cv, 224, message, 1.5f);
    font_centered(font, cv, 267, g_battle.phase == PHASE_MATCH_OVER ? "MATCH COMPLETE / ENTER TO REMATCH" : "NEXT ROUND", .75f);
  }
  if (paused) {
    canvas_rect(cv, (Rect){cv->w / 2 - 130, cv->h / 2 - 35, 260, 66}, 0xff141517);
    font_centered(font, cv, cv->h / 2 - 15, "PAUSED", 1.5f);
  }
}
