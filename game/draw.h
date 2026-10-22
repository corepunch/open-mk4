#ifndef MK4_DRAW_H
#define MK4_DRAW_H
#include "game.h"
#include "renderer.h"

/* Borrows battle and assets for this call on the host thread. Rendering never
 * changes simulation or animation history. This is not a detached snapshot
 * suitable for concurrent simulation. */
void draw_battle(Canvas *canvas, const Battle *battle, const Assets *assets, bool cpu, bool paused, double interpolation);
/* Renders only the loaded arena, using the same hierarchy, materials and light
 * path as battle drawing. The caller owns the preview canvas. */
void draw_arena_preview(Canvas *canvas, const Arena *arena);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
