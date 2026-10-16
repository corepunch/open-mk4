#pragma once
#include "renderer.h"
#include <string_view>

namespace mk4 {
// Original MK4FONT glyphs, with the authored bearings and baseline.
class SpriteFont {
    const Geometry& atlas_;
public:
    explicit SpriteFont(const Geometry& atlas):atlas_(atlas) {}
    static int glyph(char character);
    int width(std::string_view text,float scale=1) const;
    void draw(Canvas&,int x,int y,std::string_view text,float scale=1) const;
    void centered(Canvas&,int y,std::string_view text,float scale=1) const;
};
}
