#include "renderer.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
namespace mk4 {
Canvas::Canvas(int width,int height):w(width),h(height),pixels(size_t(w)*size_t(h)),depth(pixels.size()),stencil(pixels.size()) {}
void Canvas::clear() {
    for(int y=0;y<h;++y) {
        unsigned k=unsigned(15+10.f*(1.f-float(y)/float(h)));
        uint32_t c=0xff000000|(k<<16)|((k+3)<<8)|(k+8);
        std::fill_n(pixels.data()+size_t(y)*size_t(w),w,c);
    }
    std::fill(depth.begin(),depth.end(),-std::numeric_limits<float>::infinity());
    std::fill(stencil.begin(),stencil.end(),0);
}
void Canvas::rect(Rect r,uint32_t color) {
    int x0=std::max(0,r.x),x1=std::min(w,r.x+r.w);
    for(int y=std::max(0,r.y);y<std::min(h,r.y+r.h);++y)
        if(x1>x0)std::fill_n(pixels.data()+size_t(y)*size_t(w)+size_t(x0),x1-x0,color);
}
void Canvas::line(int x0,int y0,int x1,int y1,uint32_t color) {
    int dx=std::abs(x1-x0),sx=x0<x1?1:-1,dy=-std::abs(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
    for(;;) {
        if(x0>=0&&x0<w&&y0>=0&&y0<h)pixels[size_t(y0)*size_t(w)+size_t(x0)]=color;
        if(x0==x1&&y0==y1)break;
        int e=2*err;if(e>=dy){err+=dy;x0+=sx;}if(e<=dx){err+=dx;y0+=sy;}
    }
}
void Canvas::outline(Rect r,uint32_t color) {
    rect({r.x,r.y,r.w,1},color);rect({r.x,r.y+r.h-1,r.w,1},color);
    rect({r.x,r.y,1,r.h},color);rect({r.x+r.w-1,r.y,1,r.h},color);
}
void Canvas::text(int x,int y,const std::string& s,int scale,uint32_t color) {
    static const char* letters="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+./:[]";
    static const std::array<std::array<uint8_t,7>,42> glyphs={{
        {{14,17,17,31,17,17,17}},{{30,17,17,30,17,17,30}},{{14,17,16,16,16,17,14}},
        {{30,17,17,17,17,17,30}},{{31,16,16,30,16,16,31}},{{31,16,16,30,16,16,16}},
        {{14,17,16,23,17,17,15}},{{17,17,17,31,17,17,17}},{{14,4,4,4,4,4,14}},
        {{7,2,2,2,18,18,12}},{{17,18,20,24,20,18,17}},{{16,16,16,16,16,16,31}},
        {{17,27,21,21,17,17,17}},{{17,25,21,19,17,17,17}},{{14,17,17,17,17,17,14}},
        {{30,17,17,30,16,16,16}},{{14,17,17,17,21,18,13}},{{30,17,17,30,20,18,17}},
        {{15,16,16,14,1,1,30}},{{31,4,4,4,4,4,4}},{{17,17,17,17,17,17,14}},
        {{17,17,17,17,17,10,4}},{{17,17,17,21,21,21,10}},{{17,17,10,4,10,17,17}},
        {{17,17,10,4,4,4,4}},{{31,1,2,4,8,16,31}},{{14,17,19,21,25,17,14}},
        {{4,12,4,4,4,4,14}},{{14,17,1,2,4,8,31}},{{30,1,1,14,1,1,30}},
        {{2,6,10,18,31,2,2}},{{31,16,16,30,1,1,30}},{{14,16,16,30,17,17,14}},
        {{31,1,2,4,8,8,8}},{{14,17,17,14,17,17,14}},{{14,17,17,15,1,1,14}},
        {{0,0,0,31,0,0,0}},{{0,4,4,31,4,4,0}},{{0,0,0,0,0,12,12}},
        {{1,2,2,4,8,8,16}},{{0,12,12,0,12,12,0}},{{14,8,8,8,8,8,14}}
    }};
    for(char c:s) {
        const char* at=std::strchr(letters,c);
        if(at && c) {
            size_t idx=size_t(at-letters);
            if(idx<glyphs.size())for(int row=0;row<7;++row)for(int col=0;col<5;++col)
                if(glyphs[idx][size_t(row)]&(1<<(4-col)))rect({x+col*scale,y+row*scale,scale,scale},color);
        }
        x+=6*scale;
    }
}
void Canvas::triangle(const std::array<ScreenVertex,3>& p,const mk4::Texture& tex,Rect clip,bool wire,TrianglePass pass) {
    auto edge=[](const ScreenVertex& a,const ScreenVertex& b,float x,float y){return (x-a.x)*(b.y-a.y)-(y-a.y)*(b.x-a.x);};
    float area=edge(p[0],p[1],p[2].x,p[2].y);
    if(std::abs(area)<0.001f)return;
    int x0=std::max({clip.x,0,int(std::floor(std::min({p[0].x,p[1].x,p[2].x})))});
    int x1=std::min({clip.x+clip.w-1,w-1,int(std::ceil(std::max({p[0].x,p[1].x,p[2].x})))});
    int y0=std::max({clip.y,0,int(std::floor(std::min({p[0].y,p[1].y,p[2].y})))});
    int y1=std::min({clip.y+clip.h-1,h-1,int(std::ceil(std::max({p[0].y,p[1].y,p[2].y})))});
    float inv=1.f/area;
    for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x) {
        float a=edge(p[1],p[2],float(x)+.5f,float(y)+.5f)*inv;
        float b=edge(p[2],p[0],float(x)+.5f,float(y)+.5f)*inv;
        float c=1-a-b;
        if(a<-.00001f||b<-.00001f||c<-.00001f)continue;
        float z=a*p[0].z+b*p[1].z+c*p[2].z;size_t at=size_t(y)*size_t(w)+size_t(x);
        // A small reciprocal-depth bias keeps the projected plane from
        // fighting quantized arena floor vertices. The stencil still
        // restricts shadows to visible floor fragments, never props/walls.
        float bias=pass==TrianglePass::ShadowMask?.0002f:0.f;
        if(z+bias<depth[at])continue;
        float q=a*p[0].q+b*p[1].q+c*p[2].q;
        if(q<=0)continue;
        int u=std::clamp(int((a*p[0].u*p[0].q+b*p[1].u*p[1].q+c*p[2].u*p[2].q)/q),0,tex.width-1);
        int v=std::clamp(int((a*p[0].v*p[0].q+b*p[1].v*p[1].q+c*p[2].v*p[2].q)/q),0,tex.height-1);
        uint32_t color=tex.pixels[size_t(v)*size_t(tex.width)+size_t(u)];
        if(!(color>>24))continue;
        if(pass==TrianglePass::ShadowMask) {
            if(stencil[at]&receiver_bit)stencil[at]|=shadow_bit;
            continue; // Shadow coverage changes neither scene depth nor color.
        }
        stencil[at]=pass==TrianglePass::ShadowReceiver?receiver_bit:0;
        float light=std::clamp((a*p[0].light*p[0].q+b*p[1].light*p[1].q+c*p[2].light*p[2].q)/q,0.f,1.f);
        unsigned r=unsigned(float((color>>16)&255)*light),g=unsigned(float((color>>8)&255)*light),bl=unsigned(float(color&255)*light);
        pixels[at]=0xff000000|(r<<16)|(g<<8)|bl;depth[at]=z;
    }
    if(wire)for(int i=0;i<3;++i)line(int(p[size_t(i)].x),int(p[size_t(i)].y),int(p[size_t((i+1)%3)].x),int(p[size_t((i+1)%3)].y),0xff5fabb3);
}

void Canvas::resolve_shadows(uint8_t opacity) {
    // Composite the union once: internal mesh overlap and two overlapping
    // fighters' shadows must not multiply the darkening.
    for(size_t i=0;i<pixels.size();++i)if((stencil[i]&(receiver_bit|shadow_bit))==(receiver_bit|shadow_bit)) {
        uint32_t c=pixels[i],keep=255u-opacity;
        uint32_t r=((c>>16)&255)*keep/255,g=((c>>8)&255)*keep/255,b=(c&255)*keep/255;
        pixels[i]=(c&0xff000000u)|(r<<16)|(g<<8)|b;
        stencil[i]&=uint8_t(~shadow_bit);
    }
}

mk4::Mat3 yaw_matrix(float angle) {
    float c=std::cos(angle),s=std::sin(angle);return {{c,0,s,0,1,0,-s,0,c}};
}
Bounds bounds(const mk4::Character& ch,const mk4::Clip& clip) {
    Bounds b;auto front=yaw_matrix(ch.front_yaw);
    for(size_t f=0;f<clip.frames.size();f+=std::max(size_t(1),clip.frames.size()/16)) {
        auto pose=mk4::pose(ch,clip,double(f));
        for(size_t n=0;n<ch.bones.size();++n) {
            auto& bone=ch.bones[n];if(bone.flags&4)continue;
            for(auto& tri:ch.meshes[size_t(bone.mesh)].triangles)for(auto& v:tri.v) {
                auto p=front.apply(pose[n].position+pose[n].rotation.apply(v.p));
                b.lo.x=std::min(b.lo.x,p.x);b.lo.y=std::min(b.lo.y,p.y);b.lo.z=std::min(b.lo.z,p.z);
                b.hi.x=std::max(b.hi.x,p.x);b.hi.y=std::max(b.hi.y,p.y);b.hi.z=std::max(b.hi.z,p.z);
            }
        }
    }
    return b;
}
void draw_character(Canvas& cv,const mk4::Character& ch,const mk4::Clip& clip,double frame,Rect r,
                    Bounds b,float yaw,bool wire,bool skeleton) {
    auto pose=mk4::pose(ch,clip,frame);auto turn=yaw_matrix(yaw),front=yaw_matrix(ch.front_yaw);
    float height=std::max(.1f,b.hi.y-b.lo.y),width=std::max(.1f,b.hi.x-b.lo.x);
    float scale=std::min(float(r.h-76)/height,float(r.w-38)/std::max(width,height*.57f));
    mk4::Vec3 center{(b.lo.x+b.hi.x)*.5f,(b.lo.y+b.hi.y)*.5f,(b.lo.z+b.hi.z)*.5f};
    auto project=[&](mk4::Vec3 p){p=turn.apply(front.apply(p)-center);return mk4::Vec3{float(r.x+r.w/2)+p.x*scale,float(r.y+30+(r.h-76)/2)+p.y*scale,p.z};};
    for(size_t n=0;n<ch.bones.size();++n) {
        auto& bone=ch.bones[n];if(bone.flags&4)continue;
        for(auto& tri:ch.meshes[size_t(bone.mesh)].triangles) {
            std::array<ScreenVertex,3> p;
            for(size_t i=0;i<3;++i) {
                auto v=tri.v[i];auto world=pose[n].position+pose[n].rotation.apply(v.p);
                auto pos=project(world),normal=turn.apply(front.apply(pose[n].rotation.apply(v.normal)));
                // Broad, soft frontal lighting preserves the original painted atlas.
                float len=std::sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
                float light=.86f;if(len>.01f)light=.76f+.24f*std::abs((normal.z*.85f-normal.y*.4f+normal.x*.2f)/len);
                p[i]={pos.x,pos.y,pos.z,v.u,v.v,light};
            }
            cv.triangle(p,ch.texture,{r.x+8,r.y+23,r.w-16,r.h-72},wire);
        }
    }
    if(skeleton)for(size_t n=0;n<ch.bones.size();++n) {
        int parent=ch.bones[n].parent;if(parent<0)continue;
        auto a=project(pose[n].position),z=project(pose[size_t(parent)].position);
        cv.line(int(a.x),int(a.y),int(z.x),int(z.y),0xffe6bd5f);
        cv.rect({int(a.x)-2,int(a.y)-2,4,4},0xffe6bd5f);
    }
}
void save_bmp(const Canvas& cv,const std::string& path) {
    SDL_Surface* s=SDL_CreateRGBSurfaceWithFormatFrom(const_cast<uint32_t*>(cv.pixels.data()),cv.w,cv.h,32,cv.w*4,SDL_PIXELFORMAT_ARGB8888);
    if(!s)throw std::runtime_error(SDL_GetError());
    int rc=SDL_SaveBMP(s,path.c_str());SDL_FreeSurface(s);
    if(rc)throw std::runtime_error("Screenshot: "+std::string(SDL_GetError()));
}
void Canvas::blit(const Texture& t,Rect src,Rect dst,bool flip) {
    if(dst.w<=0||dst.h<=0||src.w<=0||src.h<=0)return;
    for(int y=std::max(0,dst.y);y<std::min(h,dst.y+dst.h);++y)
        for(int x=std::max(0,dst.x);x<std::min(w,dst.x+dst.w);++x) {
            int u=(x-dst.x)*src.w/dst.w;if(flip)u=src.w-1-u;
            u+=src.x;int v=src.y+(y-dst.y)*src.h/dst.h;
            if(u<0||u>=t.width||v<0||v>=t.height)continue;
            auto c=t.pixels[size_t(v)*size_t(t.width)+size_t(u)];
            if(c>>24)pixels[size_t(y)*size_t(w)+size_t(x)]=c;
        }
}

}
