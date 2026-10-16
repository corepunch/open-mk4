#pragma once
#include "mk4_assets.h"
#include <string>
namespace mk4 {
struct Rect { int x,y,w,h; };
struct Bounds { Vec3 lo{1e9f,1e9f,1e9f},hi{-1e9f,-1e9f,-1e9f}; };
struct ScreenVertex { float x,y,z,u,v,light,q=1; };
enum class TrianglePass { Color, ShadowReceiver, ShadowMask };
struct Canvas {
    static constexpr uint8_t receiver_bit=1,shadow_bit=2;
    int w,h;
    std::vector<uint32_t> pixels;
    std::vector<float> depth;
    std::vector<uint8_t> stencil;
    Canvas(int width,int height);
    void clear();
    void rect(Rect,uint32_t);
    void line(int,int,int,int,uint32_t);
    void outline(Rect,uint32_t);
    void text(int,int,const std::string&,int,uint32_t);
    void triangle(const std::array<ScreenVertex,3>&,const Texture&,Rect,bool wire=false,TrianglePass pass=TrianglePass::Color);
    void resolve_shadows(uint8_t opacity=96);
    void blit(const Texture&,Rect source,Rect dest,bool flip=false);
};
Mat3 yaw_matrix(float angle);
Bounds bounds(const Character&,const Clip&);
void draw_character(Canvas&,const Character&,const Clip&,double,Rect,Bounds,float,bool,bool);
void save_bmp(const Canvas&,const std::string&);
}
