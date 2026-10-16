#pragma once
#include "renderer.h"
#include <SDL.h>
namespace mk4 {
class SdlApp {
    SDL_Window* window_=nullptr;
    SDL_Renderer* renderer_=nullptr;
    SDL_Texture* texture_=nullptr;
public:
    SdlApp(const char* title,int width,int height,bool headless);
    ~SdlApp();
    SdlApp(const SdlApp&)=delete;
    SdlApp& operator=(const SdlApp&)=delete;
    void present(const Canvas&);
    void logical(int x,int y,float& lx,float& ly) const;
};
}
