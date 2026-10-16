#ifndef MK4_SPRITE_FONT_H
#define MK4_SPRITE_FONT_H
#include "renderer.h"
int font_glyph(char character);
int font_width(const Geometry *atlas,const char *text,float scale);
void font_draw(const Geometry *atlas,Canvas *canvas,int x,int y,const char *text,float scale);
void font_centered(const Geometry *atlas,Canvas *canvas,int y,const char *text,float scale);
#endif
