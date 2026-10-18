#include "blood_atlas.h"
#include "math_util.h"
#include <math.h>

/* One 512x192 page: two rows of 64x32 drops, a row of 64x64 mist frames
 * beside them, then a row of 128x128 floor splats. Each cell keeps a
 * transparent border so bilinear sampling never reaches its neighbour. */
#define ATLAS_W 512
#define ATLAS_H 192
#define DROP_W 64
#define DROP_H 32
#define MIST_SIZE 64
#define SPLAT_SIZE 128

static uint32_t s_pixels[ATLAS_W * ATLAS_H];
static Texture s_atlas;

static float atlas_random(unsigned *seed) {
  *seed = *seed * 1664525u + 1013904223u;
  return (float)(*seed >> 8) / 16777216.f;
}

static float smoothstep(float lo, float hi, float x) {
  float t = clampf((x - lo) / (hi - lo), 0.f, 1.f);
  return t * t * (3 - 2 * t);
}

static float smooth_min(float a, float b, float k) {
  float h = clampf(.5f + .5f * (b - a) / k, 0.f, 1.f);
  return b + (a - b) * h - k * h * (1 - h);
}

/* Distance to a segment-swept circle whose radius runs from ra to rb. */
static float capsule(float x, float y, float ax, float ay, float bx, float by, float ra, float rb) {
  float dx = bx - ax, dy = by - ay;
  float t = clampf(((x - ax) * dx + (y - ay) * dy) / (dx * dx + dy * dy), 0.f, 1.f);
  return hypotf(x - ax - dx * t, y - ay - dy * t) - (ra + (rb - ra) * t);
}

static uint32_t pack(float r, float g, float b, float a) {
  unsigned ia = (unsigned)(clampf(a, 0.f, 1.f) * 255.f + .5f);
  unsigned ir = (unsigned)(clampf(r, 0.f, 1.f) * 255.f + .5f);
  unsigned ig = (unsigned)(clampf(g, 0.f, 1.f) * 255.f + .5f);
  unsigned ib = (unsigned)(clampf(b, 0.f, 1.f) * 255.f + .5f);
  return ia << 24 | ir << 16 | ig << 8 | ib;
}

static void put(Rect cell, int x, int y, uint32_t color) {
  s_pixels[(size_t)(cell.y + y) * ATLAS_W + (size_t)(cell.x + x)] = color;
}

/* A glossy sphere-shaded head with a tapering, fading tail behind it. */
static void generate_drop(Rect cell, int variant) {
  unsigned seed = 0x9e3779b9u * (unsigned)(variant + 1);
  float radius = 7.5f + atlas_random(&seed) * 4.5f;
  float hx = (float)cell.w - 2.5f - radius, cy = (float)cell.h * .5f;
  float tail = 3.f + atlas_random(&seed) * 10.f, bend = (atlas_random(&seed) - .5f) * 4.f;
  for (int y = 1; y < cell.h - 1; ++y) {
    for (int x = 1; x < cell.w - 1; ++x) {
      float px = (float)x + .5f, py = (float)y + .5f;
      float head = hypotf(px - hx, py - cy) - radius;
      float body = capsule(px, py, hx, cy, tail, cy + bend, radius * .85f, .8f);
      float d = smooth_min(head, body, 2.f);
      float cover = clampf(.5f - d, 0.f, 1.f);
      if (cover <= 0) {
        continue;
      }
      float along = clampf((px - tail) / (hx - tail), 0.f, 1.f);
      float nx = (px - hx) / radius, ny = (py - cy) / radius;
      float nz = sqrtf(fmaxf(0.f, 1 - nx * nx - ny * ny));
      float shade = .42f + .58f * powf(nz, .7f) * along;
      float spec = powf(fmaxf(0.f, 1 - hypotf(nx + .1f, ny + .38f) * 3.2f), 2.f) * .9f;
      put(cell, x, y, pack(.80f * shade + spec, .02f * shade + spec * .62f, .03f * shade + spec * .62f, cover * (.55f + .45f * powf(along, .6f))));
    }
  }
}

/* A soft core that swells and thins while its speckles fly outward. */
static void generate_mist(Rect cell, int frame) {
  unsigned seed = 0x51ed2701u;
  float t = (float)(frame + 1) / BLOOD_MIST_FRAMES, c = MIST_SIZE * .5f;
  float core = 7.f + 17.f * t, fade = powf(1.f - t * .78f, 1.3f);
  float speck_x[24], speck_y[24], speck_r[24];
  for (int i = 0; i < 24; ++i) {
    float angle = atlas_random(&seed) * 6.2831853f, distance = (.35f + atlas_random(&seed) * .65f) * (10.f + 18.f * t);
    speck_x[i] = c + cosf(angle) * distance;
    speck_y[i] = c + sinf(angle) * distance;
    speck_r[i] = (.8f + atlas_random(&seed) * 2.2f) * (1.15f - t * .5f);
  }
  for (int y = 1; y < cell.h - 1; ++y) {
    for (int x = 1; x < cell.w - 1; ++x) {
      float px = (float)x + .5f, py = (float)y + .5f;
      float r = hypotf(px - c, py - c) / core;
      float alpha = r < 1 ? powf(1 - r, 1.6f) * .8f * fade : 0.f;
      for (int i = 0; i < 24; ++i) {
        alpha = fmaxf(alpha, clampf(.5f - (hypotf(px - speck_x[i], py - speck_y[i]) - speck_r[i]), 0.f, 1.f) * fade);
      }
      if (alpha > 0) {
        float shade = .62f + .38f * (1 - clampf(r, 0.f, 1.f));
        put(cell, x, y, pack(.72f * shade, .015f, .02f, alpha));
      }
    }
  }
}

/* An irregular wet pool with radiating streaks and scattered satellites. */
static void generate_splat(Rect cell, int variant) {
  unsigned seed = 0x2545f491u * (unsigned)(variant + 3);
  float c = SPLAT_SIZE * .5f, radius = 22.f + atlas_random(&seed) * 8.f;
  float phase[3] = {atlas_random(&seed) * 6.28f, atlas_random(&seed) * 6.28f, atlas_random(&seed) * 6.28f};
  int streaks = 3 + (int)(atlas_random(&seed) * 4), satellites = 9 + (int)(atlas_random(&seed) * 7);
  float streak[7][4], satellite[16][3];
  for (int i = 0; i < streaks; ++i) {
    float angle = atlas_random(&seed) * 6.2831853f, reach = radius * (1.35f + atlas_random(&seed) * .55f);
    streak[i][0] = cosf(angle);
    streak[i][1] = sinf(angle);
    streak[i][2] = fminf(reach, c - 6.f);
    streak[i][3] = 2.2f + atlas_random(&seed) * 2.f;
  }
  for (int i = 0; i < satellites; ++i) {
    float angle = atlas_random(&seed) * 6.2831853f, distance = radius * (1.2f + atlas_random(&seed) * .75f);
    distance = fminf(distance, c - 8.f);
    satellite[i][0] = c + cosf(angle) * distance;
    satellite[i][1] = c + sinf(angle) * distance;
    satellite[i][2] = (1.4f + atlas_random(&seed) * 3.6f) * (1.6f - distance / (c * 1.4f));
  }
  for (int y = 1; y < cell.h - 1; ++y) {
    for (int x = 1; x < cell.w - 1; ++x) {
      float px = (float)x + .5f, py = (float)y + .5f, dx = px - c, dy = py - c;
      float angle = atan2f(dy, dx);
      float edge = radius * (1 + .16f * sinf(3 * angle + phase[0]) + .09f * sinf(5 * angle + phase[1]) + .05f * sinf(11 * angle + phase[2]));
      float pool = hypotf(dx, dy) - edge, d = pool;
      for (int i = 0; i < streaks; ++i) {
        float sx = streak[i][0], sy = streak[i][1];
        d = smooth_min(d, capsule(px, py, c + sx * radius * .6f, c + sy * radius * .6f, c + sx * streak[i][2], c + sy * streak[i][2], streak[i][3], .8f), 3.f);
      }
      for (int i = 0; i < satellites; ++i) {
        d = fminf(d, hypotf(px - satellite[i][0], py - satellite[i][1]) - satellite[i][2]);
      }
      float cover = clampf(.5f - d, 0.f, 1.f);
      if (cover <= 0) {
        continue;
      }
      /* Pools darken toward their thick centre and catch light on the rim
       * facing the camera's upper left. */
      float depth = smoothstep(0.f, radius * .8f, -d);
      float rim = smoothstep(-6.f, -2.f, pool) * (1 - smoothstep(-2.f, 0.f, pool)) * clampf(-(dx * .6f + dy * .8f) / (hypotf(dx, dy) + .001f), 0.f, 1.f);
      float shade = .8f - .28f * depth;
      put(cell, x, y, pack(.72f * shade + rim * .3f, .01f + rim * .12f, .015f + rim * .12f, cover * .94f));
    }
  }
}

const Texture *blood_atlas(void) {
  if (s_atlas.width == 0) {
    for (int i = 0; i < BLOOD_DROP_VARIANTS; ++i) {
      generate_drop(blood_drop_cell(i), i);
    }
    for (int i = 0; i < BLOOD_MIST_FRAMES; ++i) {
      generate_mist(blood_mist_cell(i), i);
    }
    for (int i = 0; i < BLOOD_SPLAT_VARIANTS; ++i) {
      generate_splat(blood_splat_cell(i), i);
    }
    s_atlas = (Texture){ATLAS_W, ATLAS_H, {s_pixels, ATLAS_W * ATLAS_H, ATLAS_W * ATLAS_H}};
  }
  return &s_atlas;
}

Rect blood_drop_cell(int variant) {
  variant = mini(BLOOD_DROP_VARIANTS - 1, maxi(0, variant));
  return (Rect){(variant % 4) * DROP_W, (variant / 4) * DROP_H, DROP_W, DROP_H};
}

Rect blood_mist_cell(int frame) {
  frame = mini(BLOOD_MIST_FRAMES - 1, maxi(0, frame));
  return (Rect){256 + frame * MIST_SIZE, 0, MIST_SIZE, MIST_SIZE};
}

Rect blood_splat_cell(int variant) {
  variant = mini(BLOOD_SPLAT_VARIANTS - 1, maxi(0, variant));
  return (Rect){variant * SPLAT_SIZE, 64, SPLAT_SIZE, SPLAT_SIZE};
}
