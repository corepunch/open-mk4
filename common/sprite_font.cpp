#include "sprite_font.h"
#include <algorithm>
#include <cmath>

namespace mk4 {
int SpriteFont::glyph(char c) {
    if(c>='a'&&c<='z')c=char(c-'a'+'A');
    if(c>='0'&&c<='9')return 28+c-'0';
    if(c>='A'&&c<='Z')return 38+c-'A';
    if(c>='!'&&c<='/')return 64+c-'!';
    if(c>=':'&&c<='@')return 79+c-':';
    if(c>='['&&c<='`')return 86+c-'[';
    if(c>='{'&&c<='~')return 92+c-'{';
    return c==' '?-1:84;
}
int SpriteFont::width(std::string_view text,float scale) const {
    float width=0;
    for(char c:text){int index=glyph(c);width+=float(index<0?8:atlas_.sprites.at(size_t(index)).width+1)*scale;}
    return int(std::lround(width));
}
void SpriteFont::draw(Canvas& cv,int x,int y,std::string_view text,float scale) const {
    if(scale<=0)return;
    float pen=float(x);
    for(char c:text) {
        int index=glyph(c);
        if(index<0){pen+=8*scale;continue;}
        const auto& s=atlas_.sprites.at(size_t(index));
        cv.blit(atlas_.textures.at(size_t(s.texture)),{s.u,s.v,s.w,s.h},
            {int(std::lround(pen+float(s.origin_x)*scale)),
             y+int(std::lround(float(13+s.origin_y)*scale)),
             std::max(1,int(std::lround(float(s.width)*scale))),
             std::max(1,int(std::lround(float(s.height)*scale)))});
        pen+=float(s.width+1)*scale;
    }
}
void SpriteFont::centered(Canvas& cv,int y,std::string_view text,float scale) const {
    draw(cv,(cv.w-width(text,scale))/2,y,text,scale);
}
}
