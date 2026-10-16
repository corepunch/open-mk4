#include "sdl_app.h"
#include <string.h>
static void sdl_check(bool ok) {if(!ok)fatal("%s",SDL_GetError());}
void sdl_app_init(SdlApp *app,const char *title,int width,int height,bool headless) {
    memset(app,0,sizeof *app);if(headless)SDL_setenv("SDL_VIDEODRIVER","dummy",1);
    sdl_check(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==0);if(headless)return;
    app->window=SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
    if(app->window) {
        app->renderer=SDL_CreateRenderer(app->window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        if(!app->renderer)app->renderer=SDL_CreateRenderer(app->window,-1,SDL_RENDERER_SOFTWARE);
    }
    if(app->renderer) {
        SDL_RenderSetLogicalSize(app->renderer,width,height);
        app->texture=SDL_CreateTexture(app->renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width,height);
    }
    if(!app->texture) {char error[512];SDL_strlcpy(error,SDL_GetError(),sizeof error);sdl_app_free(app);fatal("%s",error);}
}
void sdl_app_free(SdlApp *app) {
    SDL_DestroyTexture(app->texture);SDL_DestroyRenderer(app->renderer);SDL_DestroyWindow(app->window);SDL_Quit();memset(app,0,sizeof *app);
}
void sdl_app_present(const SdlApp *app,const Canvas *cv) {
    if(!app->renderer)return;
    sdl_check(SDL_UpdateTexture(app->texture,NULL,cv->pixels,cv->w*4)==0);
    SDL_RenderClear(app->renderer);SDL_RenderCopy(app->renderer,app->texture,NULL,NULL);SDL_RenderPresent(app->renderer);
}
void sdl_app_logical(const SdlApp *app,int x,int y,float *lx,float *ly) {
    *lx=(float)x;*ly=(float)y;if(app->renderer)SDL_RenderWindowToLogical(app->renderer,x,y,lx,ly);
}
