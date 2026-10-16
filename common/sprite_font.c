#include "sprite_font.h"
#include "math_util.h"
int font_glyph(char c) {
    if(c>='a'&&c<='z')c=(char)(c-'a'+'A');
    if(c>='0'&&c<='9')return 28+c-'0';
    if(c>='A'&&c<='Z')return 38+c-'A';
    if(c>='!'&&c<='/')return 64+c-'!';
    if(c>=':'&&c<='@')return 79+c-':';
    if(c>='['&&c<='`')return 86+c-'[';
    if(c>='{'&&c<='~')return 92+c-'{';
    return c==' '?-1:84;
}
static const Sprite *font_sprite(const Geometry *atlas,int index) {
    require(index>=0 && (size_t)index<atlas->sprites.count,"Font glyph outside atlas");return &atlas->sprites.data[index];
}
int font_width(const Geometry *atlas,const char *text,float scale) {
    float width=0;for(;*text;++text) {int index=font_glyph(*text);width+=(float)(index<0?8:font_sprite(atlas,index)->width+1)*scale;}
    return (int)lroundf(width);
}
void font_draw(const Geometry *atlas,Canvas *cv,int x,int y,const char *text,float scale) {
    if(scale<=0)return;float pen=(float)x;
    for(;*text;++text) {
        int index=font_glyph(*text);if(index<0){pen+=8*scale;continue;}
        const Sprite *s=font_sprite(atlas,index);require(s->texture>=0 && (size_t)s->texture<atlas->textures.count,"Font page outside atlas");
        canvas_blit(cv,&atlas->textures.data[s->texture],(Rect){s->u,s->v,s->w,s->h},
            (Rect){(int)lroundf(pen+(float)s->origin_x*scale),y+(int)lroundf((float)(13+s->origin_y)*scale),
                maxi(1,(int)lroundf((float)s->width*scale)),maxi(1,(int)lroundf((float)s->height*scale))},false);
        pen+=(float)(s->width+1)*scale;
    }
}
void font_centered(const Geometry *atlas,Canvas *cv,int y,const char *text,float scale) {font_draw(atlas,cv,(cv->w-font_width(atlas,text,scale))/2,y,text,scale);}
