#ifndef MK4_DRAW_H
#define MK4_DRAW_H
#include "game.h"
#include "renderer.h"

/* Borrows battle and assets for this call on the host thread. Rendering never
 * changes simulation or animation history. This is not a detached snapshot
 * suitable for concurrent simulation. */
void draw_battle(Canvas *canvas, const Battle *battle, const Assets *assets, bool cpu, bool paused, double interpolation);
void draw_sprite(Canvas *canvas, const Geometry *geometry, int sprite, Rect dest, bool flip);
#endif
