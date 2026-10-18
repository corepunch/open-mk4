#ifndef MK4_BLOOD_ATLAS_H
#define MK4_BLOOD_ATLAS_H
#include "renderer.h"

/* Procedurally generated blood sprites. Drops point toward +U: their head
 * sits at the cell's right edge and their tail fades toward the left. */
#define BLOOD_DROP_VARIANTS 8
#define BLOOD_MIST_FRAMES 4
#define BLOOD_SPLAT_VARIANTS 4

const Texture *blood_atlas(void);
Rect blood_drop_cell(int variant);
Rect blood_mist_cell(int frame);
Rect blood_splat_cell(int variant);
#endif
