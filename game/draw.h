#ifndef MK4_GAME_DRAW_H
#define MK4_GAME_DRAW_H
#include "battle.h"
#include "renderer.h"
void draw_battle(Canvas *canvas,const Assets *assets,const Battle *battle,int arena,bool cpu,bool paused);
int animation(const Character *character,Action action);
void draw_sprite(Canvas *canvas,const Geometry *geometry,int sprite,Rect dest,bool flip);
#endif
