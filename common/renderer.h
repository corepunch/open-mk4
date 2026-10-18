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
/* Maps source onto a projected quad (corners clockwise from top-left) with
 * bilinear filtering. tint multiplies each channel and its alpha blends the
 * result; a NULL texture fills the quad with tint. Depth is ignored. */
void canvas_quad(Canvas *canvas, const Texture *texture, Rect source, const float corners[4][2], uint32_t tint);
/* Draws a perspective-correct, bilinear-filtered triangle that blends by
 * texture alpha. It tests against scene depth without writing it, so draw
 * soft effects after the opaque geometry they overlap. tint multiplies
 * each channel, including alpha. */
void canvas_blend_triangle(Canvas *canvas, const ScreenVertex points[3], const Texture *texture, uint32_t tint);
Mat3 yaw_matrix(float angle);
Bounds character_bounds(const Character *ch, const Clip *clip);
void draw_character(Canvas *canvas, const Character *ch, const Clip *clip, double frame, Rect rect, Bounds bounds, float yaw, bool wire, bool skeleton);
/* Draws the character reflected across the rectangle's vertical axis, as a
 * fighter facing left. */
void draw_character_mirrored(Canvas *canvas, const Character *ch, const Clip *clip, double frame, Rect rect, Bounds bounds, float yaw);
void save_bmp(const Canvas *canvas, const char *path);
#endif
