#pragma once
#include "battle.h"
#include "renderer.h"
namespace mk4::game {
void draw_battle(Canvas&,const Assets&,const Battle&,int arena,bool cpu,bool paused);
int animation(const Character&,Action);
void draw_sprite(Canvas&,const Geometry&,int sprite,Rect dest,bool flip=false);
}
