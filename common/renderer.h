#ifndef MK4_RENDERER_H
#define MK4_RENDERER_H
#include "mk4_assets.h"

typedef struct Rect {
  int x, y, w, h;
} Rect;

typedef struct Bounds {
  Vec3 lo, hi;
} Bounds;

typedef struct ScreenVertex {
  float x, y, z, u, v, light, q;
} ScreenVertex;

typedef enum TrianglePass {
  PASS_COLOR,
  PASS_ADDITIVE,
  PASS_SHADOW_RECEIVER,
  PASS_SHADOW_MASK
} TrianglePass;

#define CANVAS_RECEIVER_BIT 1
#define CANVAS_SHADOW_BIT 2

typedef struct Canvas {
  int w, h;
  uint32_t *pixels;
  float *depth;
  uint8_t *stencil;
} Canvas;

void canvas_init(Canvas *canvas, int width, int height);
void canvas_free(Canvas *canvas);
void canvas_clear(Canvas *canvas);
void canvas_rect(Canvas *canvas, Rect rect, uint32_t color);
void canvas_line(Canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
void canvas_outline(Canvas *canvas, Rect rect, uint32_t color);
void canvas_text(Canvas *canvas, int x, int y, const char *text, int scale, uint32_t color);
void canvas_triangle(Canvas *canvas, const ScreenVertex points[3], const Texture *texture, Rect clip, bool wire, TrianglePass pass);
void canvas_resolve_shadows(Canvas *canvas, uint8_t opacity);
void canvas_blit(Canvas *canvas, const Texture *texture, Rect source, Rect dest, bool flip);
Mat3 yaw_matrix(float angle);
Bounds character_bounds(const Character *ch, const Clip *clip);
void draw_character(Canvas *canvas, const Character *ch, const Clip *clip, double frame, Rect rect, Bounds bounds, float yaw, bool wire, bool skeleton);
void save_bmp(const Canvas *canvas, const char *path);
#endif
