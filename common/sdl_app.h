#ifndef MK4_SDL_APP_H
#define MK4_SDL_APP_H
#include "renderer.h"
#include <SDL.h>
typedef struct SdlApp { SDL_Window *window; SDL_Renderer *renderer; SDL_Texture *texture; } SdlApp;
void sdl_app_init(SdlApp *app,const char *title,int width,int height,bool headless);
void sdl_app_free(SdlApp *app);
void sdl_app_present(const SdlApp *app,const Canvas *canvas);
void sdl_app_logical(const SdlApp *app,int x,int y,float *lx,float *ly);
#endif
