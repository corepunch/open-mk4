#include "game.h"
#include "sprite_font.h"
#include "shadow.h"
#include "lighting.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Camera {
  float x, z, focal;
  int w, h;
  bool scene;
  Vec3 eye, right, up, forward;
} Camera;

static Vec3 camera_position(Camera c) {
  return c.scene ? c.eye : (Vec3){c.x, -2.8f, c.z};
}

static float dot(Vec3 a, Vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 camera_point(Camera c, Vec3 p) {
  if (c.scene) {
    Vec3 relative = vec_sub(p, c.eye);
    return (Vec3){dot(relative, c.right), dot(relative, c.up), dot(relative, c.forward)};
  }
  float y = p.y + 2.8f, d = c.z - p.z;
  return (Vec3){p.x - c.x, y * .978f - d * .208f, y * .208f + d * .978f};
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

static Triangles fighter_geometry(const Assets *assets, const Fighter *fighter) {
  Triangles triangles = {0};
  const Character *ch = fighter_model(assets, fighter);
  if (fighter->appearance == APPEARANCE_HIDDEN) {
    return triangles;
  }
  const Clip *clip = fighter_clip(assets, fighter);
  double frame = fighter_frame(fighter, (int)clip->frames.count);
  Pose bones[MK4_MAX_BONES];
  fighter_pose(assets, fighter, bones);
  Mat3 turn = fighter_turn(assets, fighter);
  double wrapped = fmod(fmax(0., frame), (double)clip->frames.count);
  Vec3 current = clip->frames.data[(size_t)wrapped].translation;
  /* Root-motion Y plants the sole on the floor. Dropping that channel lifts
   * the pose by the frame's Y. Uppercut 293 opens near +0.41, crouch 259
   * settles near +0.44, and knockdown 186 settles near +0.97. X/Z stay with
   * the fighter record. */
  Vec3 root = {current.x, 0.f, current.z};
  for (size_t n = 0; n < ch->bones.count; ++n) {
    const Bone *b = &ch->bones.data[n];
    if (b->flags & 4)
      continue;
    if (b->id >= 0 && b->id < 128 && (fighter->hidden_joints[b->id / 64] & (UINT64_C(1) << (b->id % 64)))) {
      continue;
    }
    const Triangles *mesh = &ch->meshes.data[b->mesh].triangles;
    for (size_t j = 0; j < mesh->count; ++j) {
      Triangle t = mesh->data[j];
      for (size_t k = 0; k < 3; ++k) {
        Vertex *v = &t.v[k];
        Vec3 local = vec_sub(vec_add(bones[n].position, mat3_apply(bones[n].rotation, v->p)), root);
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

static void draw_blade(Canvas *cv, Camera camera, Vec3 base, Vec3 tip) {
  ScreenVertex a = camera_project(camera, (Vertex){.p = camera_point(camera, base)}, 1);
  ScreenVertex b = camera_project(camera, (Vertex){.p = camera_point(camera, tip)}, 1);
  for (int width = -2; width <= 2; ++width) {
    canvas_line(cv, (int)a.x + width, (int)a.y, (int)b.x, (int)b.y, width == 0 ? 0xfff0f0ec : 0xff8c959a);
  }
  canvas_line(cv, (int)a.x - 5, (int)a.y, (int)a.x + 5, (int)a.y, 0xffb7a46c);
}

static void draw_weapon(Canvas *cv, Camera camera, Vec3 base, Vec3 tip, const WeaponDef *weapon) {
  if (weapon == NULL || weapon->type != 0x6b) {
    draw_blade(cv, camera, base, tip);
    return;
  }
  ScreenVertex a = camera_project(camera, (Vertex){.p = camera_point(camera, base)}, 1);
  ScreenVertex b = camera_project(camera, (Vertex){.p = camera_point(camera, tip)}, 1);
  for (int width = -2; width <= 2; ++width) {
    canvas_line(cv, (int)a.x + width, (int)a.y, (int)b.x + width, (int)b.y, width == 0 ? 0xffdef7ff : 0xff3189bc);
  }
  canvas_rect(cv, (Rect){(int)b.x - 4, (int)b.y - 4, 9, 9}, 0xff8ee8ff);
}

void draw_battle(Canvas *cv, const Assets *assets, const Battle *battle, int arena, bool cpu, bool paused) {
  canvas_clear(cv);
  const Fighter *a = &battle->fighters[0], *b = &battle->fighters[1];
  float ax = fighter_x(a), bx = fighter_x(b);
  Camera camera = {.x = (ax + bx) * .25f, .z = fmaxf(7.5f, fabsf(ax - bx) * .85f + 3.f), .focal = (float)cv->w * .95f, .w = cv->w, .h = cv->h};
  if (battle->camera.active) {
    camera.scene = true;
    camera.eye = battle->camera.eye;
    Vec3 direction = vec_sub(battle->camera.target, camera.eye);
    float length = sqrtf(dot(direction, direction));
    camera.forward = vec_scale(direction, 1.f / fmaxf(.001f, length));
    float horizontal = hypotf(direction.x, direction.z);
    camera.right = (Vec3){-direction.z / fmaxf(.001f, horizontal), 0, direction.x / fmaxf(.001f, horizontal)};
    camera.up =
        (Vec3){-camera.forward.y * camera.right.z, camera.forward.x * camera.right.z - camera.forward.z * camera.right.x, camera.forward.y * camera.right.x};
  }
  SceneLight light = above_camera_light(camera_position(camera));
  float floor_y = draw_arena(cv, camera, &assets->arenas.data[arena], light);
  Triangles fighters[] = {fighter_geometry(assets, a), fighter_geometry(assets, b)};
  Mat4 shadow;
  require(planar_shadow_matrix((Vec4){0, 1, 0, -floor_y}, light_homogeneous(light), &shadow), "Shadow light must intersect the receiver plane");
  for (size_t i = 0; i < 2; ++i) {
    const Texture *texture = &fighter_model(assets, &battle->fighters[i])->texture;
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
  uint32_t ice_pixel = 0xff89dfff;
  const Texture ice_texture = {1, 1, {&ice_pixel, 1, 1}};
  for (int i = 0; i < MAX_SCENE_PIECES; ++i) {
    const ScenePiece *piece = &battle->pieces[i];
    if (!piece->active) {
      continue;
    }
    const Character *ch = roster_character(assets, piece->roster);
    const Bone *bone = &ch->bones.data[piece->bone];
    float factor = 4096.f / (6.283185307179586f * Q16_ONE);
    Mat3 spin = rotation(0, (Vec3){piece->angle[0] * factor, piece->angle[1] * factor, piece->angle[2] * factor});
    Mat3 transform = mat3_mul(spin, piece->orientation);
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
  for (size_t i = 0; i < 2; ++i) {
    const Fighter *fighter = &battle->fighters[i];
    const Texture *texture = fighter->appearance == APPEARANCE_FROZEN ? &ice_texture : &fighter_model(assets, fighter)->texture;
    for (size_t j = 0; j < fighters[i].count; ++j)
      render_triangle(cv, camera, fighters[i].data[j], texture,
          fighter->appearance == APPEARANCE_BURNED ? .18f
          : fighter->state == S_HIT                ? .72f
                                                   : 1.f,
          PASS_COLOR, &light);
    FREE_ARRAY(fighters[i]);
  }
  /* Script objects are rendered from simulation state; rendering never advances them. */
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &battle->projectiles[i];
    if (p->kind == PROJECTILE_CLONE) {
      Fighter clone = battle->fighters[p->owner];
      MoveDef captured = {.clip = p->clip, .last = MOVE_END};
      clone.current_move = &captured;
      clone.appearance = APPEARANCE_NORMAL;
      clone.action = 0;
      clone.sample_override = p->sample;
      clone.facing = p->facing;
      memcpy(clone.heading, p->heading, sizeof clone.heading);
      memcpy(clone.body.angle, p->angle, sizeof clone.body.angle);
      clone.body.root_y = p->root_y;
      memcpy(clone.body.pos, p->pos, sizeof clone.body.pos);
      Triangles geometry = fighter_geometry(assets, &clone);
      for (size_t j = 0; j < geometry.count; ++j) {
        render_triangle(cv, camera, geometry.data[j], &ice_texture, .9f, PASS_COLOR, &light);
      }
      FREE_ARRAY(geometry);
    }
    if (p->kind == PROJECTILE_ICE) {
      Vec3 tip = {q16_float(p->pos[AXIS_X]), q16_float(p->pos[AXIS_Y]), q16_float(p->pos[AXIS_Z])};
      ScreenVertex at = camera_project(camera, (Vertex){.p = camera_point(camera, tip)}, 1);
      canvas_rect(cv, (Rect){(int)at.x - 7, (int)at.y - 3, 15, 7}, 0xff78d9ff);
      canvas_line(cv, (int)at.x - 9, (int)at.y, (int)at.x + 9, (int)at.y, 0xffe0faff);
    }
    if (p->kind == PROJECTILE_SWORD) {
      Vec3 base = {q16_float(p->pos[AXIS_X]), q16_float(p->pos[AXIS_Y]), q16_float(p->pos[AXIS_Z])};
      float angle = p->phase == 2 ? 0.f : (float)p->ticks * .3f;
      draw_weapon(cv, camera, base, vec_add(base, (Vec3){cosf(angle), sinf(angle), 0}), p->weapon);
    }
    if (p->kind != PROJECTILE_SPEAR) {
      continue;
    }
    const Fighter *owner = &battle->fighters[p->owner];
    Vec3 hand = {fighter_x(owner), -1.4f - fighter_height(owner), fighter_z(owner)};
    Vec3 tip = {q16_float(p->pos[AXIS_X]), q16_float(p->pos[AXIS_Y]), q16_float(p->pos[AXIS_Z])};
    ScreenVertex from = camera_project(camera, (Vertex){.p = camera_point(camera, hand)}, 1);
    ScreenVertex to = camera_project(camera, (Vertex){.p = camera_point(camera, tip)}, 1);
    canvas_line(cv, (int)from.x, (int)from.y, (int)to.x, (int)to.y, 0xffc9a85c);
    canvas_rect(cv, (Rect){(int)to.x - 3, (int)to.y - 2, 7, 4}, 0xffdedede);
  }
  for (int i = 0; i < 2; ++i) {
    const Fighter *f = &battle->fighters[i];
    if (f->armed) {
      Vec3 hand = {fighter_x(f) + f->facing * .3f, -1.2f - fighter_height(f), fighter_z(f)};
      float angle = fighter_move(f)->clip == 224 ? f->ticks * .5f : -1.1f;
      draw_weapon(cv, camera, hand, vec_add(hand, (Vec3){f->facing * cosf(angle), sinf(angle), 0}), f->weapon);
    }
    if ((fighter_move(f)->clip == 448 && f->script != NULL && f->ticks >= 13 && f->ticks < (f->script_phase ? 32 : 48)) ||
        (battle->phase == PHASE_FATALITY && battle->finisher == FINISH_BURN && i == battle->winner && battle->phase_ticks >= 51 && battle->phase_ticks < 191)) {
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
    const Particle *p = &battle->particles[i];
    if (p->ticks <= 0) {
      continue;
    }
    Vec3 point = camera_point(camera, (Vec3){q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])});
    if (point.z > .4f) {
      ScreenVertex screen = camera_project(camera, (Vertex){.p = point}, 1);
      int size = maxi(1, mini(8, (int)(camera.focal * .025f / point.z)));
      canvas_rect(cv, (Rect){(int)screen.x - size / 2, (int)screen.y - size / 2, size, size}, p->color);
    }
  }
  for (size_t i = 0; i < 2; ++i) {
    const HitEffect *e = &battle->effects[i];
    if (e->ticks <= 0)
      continue;
    ScreenVertex p = camera_project(camera, (Vertex){.p = camera_point(camera, (Vec3){e->x, -e->height, .2f})}, 1);
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
    const Fighter *f = &battle->fighters[i];
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
  snprintf(timer, sizeof timer, "%02d", maxi(0, (battle->time_ticks + 59) / 60));
  snprintf(round, sizeof round, "ROUND %d", battle->round);
  font_centered(font, cv, 43, timer, 2);
  font_centered(font, cv, 96, round, .65f);
  canvas_rect(cv, (Rect){0, cv->h - 96, cv->w, 96}, 0xff141517);
  font_draw(font, cv, 28, cv->h - 79, assets->arenas.data[arena].name, 1);
  const char *mode = cpu ? "P1 VS CPU / F3 TWO PLAYERS" : "LOCAL TWO PLAYERS / F3 CPU";
  font_draw(font, cv, cv->w - 28 - font_width(font, mode, .6f), cv->h - 78, mode, .6f);
  font_draw(font, cv, 28, cv->h - 49, "P1  A/D MOVE  W JUMP  S DUCK  U HP  J LP  I HK  K LK  L BLOCK  H RUN", .6f);
  font_draw(font, cv, 28, cv->h - 29, "P2  ARROWS  NUM 1/2/3/4/5  RSHIFT RUN    F1/F2 FIGHTERS  F4 ARENA  R RESET  SPACE PAUSE", .6f);
  if (battle->phase == PHASE_READY) {
    round_intro(cv, &assets->hud, battle->round);
    if (battle->phase_ticks > 40)
      draw_sprite(cv, &assets->hud.effects, 137, (Rect){cv->w / 2 - 181, 285, 362, 126}, false);
  }
  if (battle->phase == PHASE_FINISH || battle->phase == PHASE_FATALITY) {
    font_centered(font, cv, 165, battle->phase == PHASE_FINISH ? "FINISH HIM" : "FATALITY", 1.5f);
  }
  if (battle->phase == PHASE_ROUND_OVER || battle->phase == PHASE_MATCH_OVER) {
    char message[1100];
    if (battle->winner < 0)
      snprintf(message, sizeof message, "DRAW");
    else
      snprintf(message, sizeof message, battle->finisher ? "%s WINS / FATALITY" : "%s WINS",
          roster_character(assets, battle->fighters[battle->winner].roster)->name);
    canvas_rect(cv, (Rect){cv->w / 2 - 350, 205, 700, 100}, 0xff141517);
    font_centered(font, cv, 224, message, 1.5f);
    font_centered(font, cv, 267, battle->phase == PHASE_MATCH_OVER ? "MATCH COMPLETE / ENTER TO REMATCH" : "NEXT ROUND", .75f);
  }
  if (paused) {
    canvas_rect(cv, (Rect){cv->w / 2 - 130, cv->h / 2 - 35, 260, 66}, 0xff141517);
    font_centered(font, cv, cv->h / 2 - 15, "PAUSED", 1.5f);
  }
}
