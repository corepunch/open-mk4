#include "sdl_app.h"
#include <stdexcept>
namespace mk4 {
SdlApp::SdlApp(const char* title,int width,int height,bool headless) {
    if(headless)SDL_setenv("SDL_VIDEODRIVER","dummy",1);
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))throw std::runtime_error(SDL_GetError());
    try {
        if(headless)return;
        window_=SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
        if(!window_)throw std::runtime_error(SDL_GetError());
        renderer_=SDL_CreateRenderer(window_,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        if(!renderer_)renderer_=SDL_CreateRenderer(window_,-1,SDL_RENDERER_SOFTWARE);
        if(!renderer_)throw std::runtime_error(SDL_GetError());
        SDL_RenderSetLogicalSize(renderer_,width,height);
        texture_=SDL_CreateTexture(renderer_,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width,height);
        if(!texture_)throw std::runtime_error(SDL_GetError());
    } catch(...) {SDL_DestroyTexture(texture_);SDL_DestroyRenderer(renderer_);SDL_DestroyWindow(window_);SDL_Quit();throw;}
}
SdlApp::~SdlApp(){SDL_DestroyTexture(texture_);SDL_DestroyRenderer(renderer_);SDL_DestroyWindow(window_);SDL_Quit();}
void SdlApp::present(const Canvas& cv) {
    if(!renderer_)return;
    if(SDL_UpdateTexture(texture_,nullptr,cv.pixels.data(),cv.w*4))throw std::runtime_error(SDL_GetError());
    SDL_RenderClear(renderer_);SDL_RenderCopy(renderer_,texture_,nullptr,nullptr);SDL_RenderPresent(renderer_);
}
void SdlApp::logical(int x,int y,float& lx,float& ly) const {
    lx=float(x);ly=float(y);if(renderer_)SDL_RenderWindowToLogical(renderer_,x,y,&lx,&ly);
}
}
