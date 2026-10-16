#include "renderer.h"
#include "math_util.h"
#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
static int clampi(int x,int lo,int hi) {return mini(hi,maxi(lo,x));}
static float edge(const ScreenVertex *a,const ScreenVertex *b,float x,float y) {return (x-a->x)*(b->y-a->y)-(y-a->y)*(b->x-a->x);}
void canvas_init(Canvas *cv,int w,int h) {
    require(w>0 && h>0 && w<=16384 && h<=16384,"Invalid canvas size");cv->w=w;cv->h=h;
    size_t n=(size_t)w*(size_t)h;cv->pixels=checked_alloc(n,sizeof *cv->pixels);cv->depth=checked_alloc(n,sizeof *cv->depth);cv->stencil=checked_alloc(n,sizeof *cv->stencil);
}
void canvas_free(Canvas *cv) {free(cv->pixels);free(cv->depth);free(cv->stencil);memset(cv,0,sizeof *cv);}
void canvas_clear(Canvas *cv) {
    for(int y=0;y<cv->h;++y) {
        unsigned k=(unsigned)(15+10.f*(1.f-(float)(y)/(float)(cv->h)));
        uint32_t c=0xff000000|(k<<16)|((k+3)<<8)|(k+8);
        for(int x=0;x<cv->w;++x)cv->pixels[(size_t)y*(size_t)cv->w+(size_t)x]=c;
    }
    for(size_t i=0;i<(size_t)cv->w*(size_t)cv->h;++i)cv->depth[i]=-INFINITY;
    memset(cv->stencil,0,(size_t)cv->w*(size_t)cv->h);
}
void canvas_rect(Canvas *cv,Rect r,uint32_t color) {
    int x0=maxi(0,r.x),x1=mini(cv->w,r.x+r.w);
    for(int y=maxi(0,r.y);y<mini(cv->h,r.y+r.h);++y)
        for(int x=x0;x<x1;++x)cv->pixels[(size_t)y*(size_t)cv->w+(size_t)x]=color;
}
void canvas_line(Canvas *cv,int x0,int y0,int x1,int y1,uint32_t color) {
    int dx=abs(x1-x0),sx=x0<x1?1:-1,dy=-abs(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
    for(;;) {
        if(x0>=0&&x0<cv->w&&y0>=0&&y0<cv->h)cv->pixels[(size_t)(y0)*(size_t)(cv->w)+(size_t)(x0)]=color;
        if(x0==x1&&y0==y1)break;
        int e=2*err;if(e>=dy){err+=dy;x0+=sx;}if(e<=dx){err+=dx;y0+=sy;}
    }
}
void canvas_outline(Canvas *cv,Rect r,uint32_t color) {
    canvas_rect(cv,(Rect){r.x,r.y,r.w,1},color);canvas_rect(cv,(Rect){r.x,r.y+r.h-1,r.w,1},color);
    canvas_rect(cv,(Rect){r.x,r.y,1,r.h},color);canvas_rect(cv,(Rect){r.x+r.w-1,r.y,1,r.h},color);
}
void canvas_text(Canvas *cv,int x,int y,const char *s,int scale,uint32_t color) {
    static const char* letters="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+./:[]";
    static const uint8_t glyphs[42][7]={
        {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
        {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
        {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
        {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
        {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
        {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
        {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
        {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
        {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},{14,17,19,21,25,17,14},
        {4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
        {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},{14,16,16,30,17,17,14},
        {31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14},
        {0,0,0,31,0,0,0},{0,4,4,31,4,4,0},{0,0,0,0,0,12,12},
        {1,2,2,4,8,8,16},{0,12,12,0,12,12,0},{14,8,8,8,8,8,14}
    };
    for(;*s;++s) {
        char c=*s;
        const char* at=strchr(letters,c);
        if(at && c) {
            size_t idx=(size_t)(at-letters);
            if(idx<42)for(int row=0;row<7;++row)for(int col=0;col<5;++col)
                if(glyphs[idx][(size_t)(row)]&(1<<(4-col)))canvas_rect(cv,(Rect){x+col*scale,y+row*scale,scale,scale},color);
        }
        x+=6*scale;
    }
}
void canvas_triangle(Canvas *cv,const ScreenVertex p[3],const Texture *tex,Rect clip,bool wire,TrianglePass pass) {
    float area=edge(&p[0],&p[1],p[2].x,p[2].y);
    if(fabsf(area)<0.001f)return;
    int x0=maxi(maxi(clip.x,0),(int)floorf(fminf(p[0].x,fminf(p[1].x,p[2].x))));
    int x1=mini(mini(clip.x+clip.w-1,cv->w-1),(int)ceilf(fmaxf(p[0].x,fmaxf(p[1].x,p[2].x))));
    int y0=maxi(maxi(clip.y,0),(int)floorf(fminf(p[0].y,fminf(p[1].y,p[2].y))));
    int y1=mini(mini(clip.y+clip.h-1,cv->h-1),(int)ceilf(fmaxf(p[0].y,fmaxf(p[1].y,p[2].y))));
    float inv=1.f/area;
    for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x) {
        float a=edge(&p[1],&p[2],(float)(x)+.5f,(float)(y)+.5f)*inv;
        float b=edge(&p[2],&p[0],(float)(x)+.5f,(float)(y)+.5f)*inv;
        float c=1-a-b;
        if(a<-.00001f||b<-.00001f||c<-.00001f)continue;
        float z=a*p[0].z+b*p[1].z+c*p[2].z;size_t at=(size_t)(y)*(size_t)(cv->w)+(size_t)(x);
        // A small reciprocal-depth bias keeps the projected plane from
        // fighting quantized arena floor vertices. The cv->stencil still
        // restricts shadows to visible floor fragments, never props/walls.
        float bias=pass==PASS_SHADOW_MASK?.0002f:0.f;
        if(z+bias<cv->depth[at])continue;
        float q=a*p[0].q+b*p[1].q+c*p[2].q;
        if(q<=0)continue;
        int u=clampi((int)((a*p[0].u*p[0].q+b*p[1].u*p[1].q+c*p[2].u*p[2].q)/q),0,tex->width-1);
        int v=clampi((int)((a*p[0].v*p[0].q+b*p[1].v*p[1].q+c*p[2].v*p[2].q)/q),0,tex->height-1);
        uint32_t color=tex->pixels.data[(size_t)(v)*(size_t)(tex->width)+(size_t)(u)];
        if(!(color>>24))continue;
        if(pass==PASS_SHADOW_MASK) {
            if(cv->stencil[at]&CANVAS_RECEIVER_BIT)cv->stencil[at]|=CANVAS_SHADOW_BIT;
            continue; // Shadow coverage changes neither scene depth nor color.
        }
        cv->stencil[at]=pass==PASS_SHADOW_RECEIVER?CANVAS_RECEIVER_BIT:0;
        float light=clampf((a*p[0].light*p[0].q+b*p[1].light*p[1].q+c*p[2].light*p[2].q)/q,0.f,1.f);
        unsigned r=(unsigned)((float)((color>>16)&255)*light),g=(unsigned)((float)((color>>8)&255)*light),bl=(unsigned)((float)(color&255)*light);
        cv->pixels[at]=0xff000000|(r<<16)|(g<<8)|bl;cv->depth[at]=z;
    }
    if(wire)for(int i=0;i<3;++i)canvas_line(cv,(int)(p[(size_t)(i)].x),(int)(p[(size_t)(i)].y),(int)(p[(size_t)((i+1)%3)].x),(int)(p[(size_t)((i+1)%3)].y),0xff5fabb3);
}

void canvas_resolve_shadows(Canvas *cv,uint8_t opacity) {
    // Composite the union once: internal mesh overlap and two overlapping
    // fighters' shadows must not multiply the darkening.
    for(size_t i=0;i<(size_t)cv->w*(size_t)cv->h;++i)if((cv->stencil[i]&(CANVAS_RECEIVER_BIT|CANVAS_SHADOW_BIT))==(CANVAS_RECEIVER_BIT|CANVAS_SHADOW_BIT)) {
        uint32_t c=cv->pixels[i],keep=255u-opacity;
        uint32_t r=((c>>16)&255)*keep/255,g=((c>>8)&255)*keep/255,b=(c&255)*keep/255;
        cv->pixels[i]=(c&0xff000000u)|(r<<16)|(g<<8)|b;
        cv->stencil[i]&=(uint8_t)(~CANVAS_SHADOW_BIT);
    }
}

Mat3 yaw_matrix(float angle) {float c=cosf(angle),s=sinf(angle);return (Mat3){{c,0,s,0,1,0,-s,0,c}};}
Bounds character_bounds(const Character *ch,const Clip *clip) {
    Bounds b={{1e9f,1e9f,1e9f},{-1e9f,-1e9f,-1e9f}};Mat3 front=yaw_matrix(ch->front_yaw);
    for(size_t f=0;f<clip->frames.count;f+=maxz(1,clip->frames.count/16)) {
        Pose nodes[MK4_MAX_BONES];pose(ch,clip,(double)f,nodes);
        for(size_t n=0;n<ch->bones.count;++n) {
            const Bone *bone=&ch->bones.data[n];if(bone->flags&4)continue;
            const Triangles *triangles=&ch->meshes.data[bone->mesh].triangles;
            for(size_t j=0;j<triangles->count;++j)for(size_t k=0;k<3;++k) {
                Vec3 p=mat3_apply(front,vec_add(nodes[n].position,mat3_apply(nodes[n].rotation,triangles->data[j].v[k].p)));
                b.lo.x=fminf(b.lo.x,p.x);b.lo.y=fminf(b.lo.y,p.y);b.lo.z=fminf(b.lo.z,p.z);
                b.hi.x=fmaxf(b.hi.x,p.x);b.hi.y=fmaxf(b.hi.y,p.y);b.hi.z=fmaxf(b.hi.z,p.z);
            }
        }
    }
    return b;
}
static Vec3 gallery_project(Vec3 p,Mat3 turn,Mat3 front,Vec3 center,Rect r,float scale) {
    p=mat3_apply(turn,vec_sub(mat3_apply(front,p),center));return (Vec3){(float)(r.x+r.w/2)+p.x*scale,(float)(r.y+30+(r.h-76)/2)+p.y*scale,p.z};
}
void draw_character(Canvas *cv,const Character *ch,const Clip *clip,double frame,Rect r,Bounds b,float yaw,bool wire,bool skeleton) {
    Pose nodes[MK4_MAX_BONES];pose(ch,clip,frame,nodes);Mat3 turn=yaw_matrix(yaw),front=yaw_matrix(ch->front_yaw);
    float height=fmaxf(.1f,b.hi.y-b.lo.y),width=fmaxf(.1f,b.hi.x-b.lo.x);
    float scale=fminf((float)(r.h-76)/height,(float)(r.w-38)/fmaxf(width,height*.57f));
    Vec3 center={(b.lo.x+b.hi.x)*.5f,(b.lo.y+b.hi.y)*.5f,(b.lo.z+b.hi.z)*.5f};
    for(size_t n=0;n<ch->bones.count;++n) {
        const Bone *bone=&ch->bones.data[n];if(bone->flags&4)continue;
        const Triangles *triangles=&ch->meshes.data[bone->mesh].triangles;
        for(size_t j=0;j<triangles->count;++j) {
            ScreenVertex p[3];
            for(size_t i=0;i<3;++i) {
                Vertex v=triangles->data[j].v[i];Vec3 world=vec_add(nodes[n].position,mat3_apply(nodes[n].rotation,v.p));
                Vec3 pos=gallery_project(world,turn,front,center,r,scale),normal=mat3_apply(turn,mat3_apply(front,mat3_apply(nodes[n].rotation,v.normal)));
                float len=sqrtf(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
                float light=.86f;if(len>.01f)light=.76f+.24f*fabsf((normal.z*.85f-normal.y*.4f+normal.x*.2f)/len);
                p[i]=(ScreenVertex){pos.x,pos.y,pos.z,v.u,v.v,light,1};
            }
            canvas_triangle(cv,p,&ch->texture,(Rect){r.x+8,r.y+23,r.w-16,r.h-72},wire,PASS_COLOR);
        }
    }
    if(skeleton)for(size_t n=0;n<ch->bones.count;++n) {
        int parent=ch->bones.data[n].parent;if(parent<0)continue;
        Vec3 a=gallery_project(nodes[n].position,turn,front,center,r,scale),z=gallery_project(nodes[parent].position,turn,front,center,r,scale);
        canvas_line(cv,(int)a.x,(int)a.y,(int)z.x,(int)z.y,0xffe6bd5f);canvas_rect(cv,(Rect){(int)a.x-2,(int)a.y-2,4,4},0xffe6bd5f);
    }
}
void save_bmp(const Canvas *cv,const char *path) {
    SDL_Surface *s=SDL_CreateRGBSurfaceWithFormatFrom(cv->pixels,cv->w,cv->h,32,cv->w*4,SDL_PIXELFORMAT_ARGB8888);
    if(!s)fatal("%s",SDL_GetError());int rc=SDL_SaveBMP(s,path);SDL_FreeSurface(s);if(rc)fatal("Screenshot: %s",SDL_GetError());
}
void canvas_blit(Canvas *cv,const Texture *t,Rect src,Rect dst,bool flip) {
    if(dst.w<=0||dst.h<=0||src.w<=0||src.h<=0)return;
    for(int y=maxi(0,dst.y);y<mini(cv->h,dst.y+dst.h);++y)for(int x=maxi(0,dst.x);x<mini(cv->w,dst.x+dst.w);++x) {
        int u=(x-dst.x)*src.w/dst.w;if(flip)u=src.w-1-u;u+=src.x;int v=src.y+(y-dst.y)*src.h/dst.h;
        if(u<0||u>=t->width||v<0||v>=t->height)continue;
        uint32_t c=t->pixels.data[(size_t)v*(size_t)t->width+(size_t)u];if(c>>24)cv->pixels[(size_t)y*(size_t)cv->w+(size_t)x]=c;
    }
}
