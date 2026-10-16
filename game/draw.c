#include "game.h"
#include "sprite_font.h"
#include "shadow.h"
#include "lighting.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
static const float pi = 3.14159265358979323846f;

typedef struct Camera {
  float x, z, focal;
  int w, h;
} Camera;

static Vec3 camera_position(Camera c) {
  return (Vec3){c.x, -2.8f, c.z};
}

static Vec3 camera_point(Camera c, Vec3 p) {
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
  const Character *ch = roster_character(assets, fighter->roster);
  const MoveDef *move = fighter_move(fighter);
  int id = move->clip < 0 ? ch->idle : move->clip;
  const Clip *clip = &assets->clips.data[id];
  int channels = 0;
  double frame;
  for (size_t i = 0; i < ch->bones.count; ++i)
    channels = maxi(channels, ch->bones.data[i].channel + 1);
  if ((int)clip->frames.data[0].angles.count != channels)
    clip = &assets->clips.data[ch->idle];
  frame = fighter_frame(fighter, (int)clip->frames.count);
  Pose bones[MK4_MAX_BONES], initial[MK4_MAX_BONES];
  pose(ch, clip, frame, bones);
  pose(ch, clip, 0, initial);
  float front = ch->front_yaw;
  for (size_t n = 0; n < ch->bones.count; ++n)
    if (ch->bones.data[n].channel == 0) {
      Vec3 face = mat3_apply(initial[n].rotation, (Vec3){0, 0, -1});
      front = atan2f(-face.x, face.z);
      break;
    }
  Mat3 turn = yaw_matrix(front + (float)fighter->facing * pi * .5f);
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
    const Triangles *mesh = &ch->meshes.data[b->mesh].triangles;
    for (size_t j = 0; j < mesh->count; ++j) {
      Triangle t = mesh->data[j];
      for (size_t k = 0; k < 3; ++k) {
        Vertex *v = &t.v[k];
        Vec3 p = mat3_apply(turn, vec_sub(vec_add(bones[n].position, mat3_apply(bones[n].rotation, v->p)), root));
        v->p = (Vec3){p.x + fighter->x, p.y - fighter->height, p.z};
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

static void bar(Canvas *cv, const Geometry *font, int health, Rect r, bool right) {
  const Sprite *s = &font->sprites.data[25];
  canvas_blit(cv, &font->textures.data[s->texture], (Rect){s->u, s->v, s->w, 8}, r, right);
  int inner = r.w - 6, missing = inner * (100 - health) / 100;
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

void draw_battle(Canvas *cv, const Assets *assets, const Battle *battle, int arena, bool cpu, bool paused) {
  canvas_clear(cv);
  const Fighter *a = &battle->fighters[0], *b = &battle->fighters[1];
  Camera camera = {(a->x + b->x) * .25f, fmaxf(7.5f, fabsf(a->x - b->x) * .85f + 3.f), (float)cv->w * .95f, cv->w, cv->h};
  SceneLight light = above_camera_light(camera_position(camera));
  float floor_y = draw_arena(cv, camera, &assets->arenas.data[arena], light);
  Triangles fighters[] = {fighter_geometry(assets, a), fighter_geometry(assets, b)};
  Mat4 shadow;
  require(planar_shadow_matrix((Vec4){0, 1, 0, -floor_y}, light_homogeneous(light), &shadow), "Shadow light must intersect the receiver plane");
  for (size_t i = 0; i < 2; ++i) {
    const Texture *texture = &roster_character(assets, battle->fighters[i].roster)->texture;
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
  for (size_t i = 0; i < 2; ++i) {
    const Fighter *fighter = &battle->fighters[i];
    const Texture *texture = &roster_character(assets, fighter->roster)->texture;
    for (size_t j = 0; j < fighters[i].count; ++j)
      render_triangle(cv, camera, fighters[i].data[j], texture, fighter->state == S_HIT ? .72f : 1.f, PASS_COLOR, &light);
    FREE_ARRAY(fighters[i]);
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
  if (battle->phase == PHASE_ROUND_OVER || battle->phase == PHASE_MATCH_OVER) {
    char message[1100];
    if (battle->winner < 0)
      snprintf(message, sizeof message, "DRAW");
    else
      snprintf(message, sizeof message, "%s WINS", roster_character(assets, battle->fighters[battle->winner].roster)->name);
    canvas_rect(cv, (Rect){cv->w / 2 - 350, 205, 700, 100}, 0xff141517);
    font_centered(font, cv, 224, message, 1.5f);
    font_centered(font, cv, 267, battle->phase == PHASE_MATCH_OVER ? "MATCH COMPLETE / ENTER TO REMATCH" : "NEXT ROUND", .75f);
  }
  if (paused) {
    canvas_rect(cv, (Rect){cv->w / 2 - 130, cv->h / 2 - 35, 260, 66}, 0xff141517);
    font_centered(font, cv, cv->h / 2 - 15, "PAUSED", 1.5f);
  }
}
