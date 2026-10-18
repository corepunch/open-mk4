#include "menu.h"
#include "game.h"
#include "sprite_font.h"
#include <SDL.h>
#include <stdio.h>
#include <string.h>

static const uint32_t background = 0xff10151d;
static const uint32_t panel = 0xff1a2430;
static const uint32_t edge = 0xff485b6c;
static const uint32_t gold = 0xffffd56b;
static const uint32_t blue = 0xff63c9ff;
static const uint32_t red = 0xffff756b;

static void centered(const Geometry *font, Canvas *canvas, int y, const char *text, float scale) {
  int x = (canvas->w - font_width(font, text, scale)) / 2;
  font_draw(font, canvas, x, y, text, scale);
}

static void blit_name(Canvas *canvas, const Geometry *geometry, int index, Rect destination) {
  if (index < 0 || (size_t)index >= geometry->sprites.count) {
    return;
  }
  const Sprite *sprite = &geometry->sprites.data[index];
  if (sprite->texture < 0 || (size_t)sprite->texture >= geometry->textures.count) {
    return;
  }
  canvas_blit(canvas, &geometry->textures.data[sprite->texture], (Rect){sprite->u, sprite->v, sprite->w, sprite->h}, destination, false);
}

static void title_frame(Canvas *canvas, const Assets *assets, const char *eyebrow, const char *title) {
  canvas_rect(canvas, (Rect){0, 0, canvas->w, canvas->h}, background);
  canvas_rect(canvas, (Rect){0, 0, canvas->w, 7}, gold);
  canvas_rect(canvas, (Rect){0, canvas->h - 7, canvas->w, 7}, gold);
  centered(&assets->hud.font, canvas, 64, eyebrow, .8f);
  centered(&assets->hud.font, canvas, 109, title, 1.8f);
  canvas_line(canvas, 260, 166, canvas->w - 260, 166, edge);
}

void menu_init(FrontendMenu *menu) {
  memset(menu, 0, sizeof *menu);
  menu->screen = MENU_TITLE;
  menu->p1 = 0;
  menu->p2 = 4;
  menu->arena = 8;
}

static void move_character(FrontendMenu *menu, int dx, int dy) {
  int column = menu->cursor % 6;
  int row = menu->cursor / 6;
  column = (column + dx + 6) % 6;
  row = (row + dy + 3) % 3;
  menu->cursor = row * 6 + column;
  if (menu->cursor >= roster_count()) {
    menu->cursor = (menu->cursor + (dy > 0 ? 6 : -6) + roster_count()) % roster_count();
  }
}

bool menu_key(FrontendMenu *menu, int keycode, bool *quit) {
  bool confirm = keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER || keycode == SDLK_SPACE;
  bool back = keycode == SDLK_ESCAPE || keycode == SDLK_BACKSPACE;
  if (keycode == SDLK_ESCAPE && menu->screen == MENU_TITLE) {
    *quit = true;
    return false;
  }
  if (menu->screen == MENU_TITLE) {
    if (keycode != SDLK_ESCAPE) {
      menu->screen = MENU_MODE;
      menu->cursor = 0;
    }
    return false;
  }
  if (back) {
    if (menu->screen == MENU_MODE) {
      menu->screen = MENU_TITLE;
    } else if (menu->screen == MENU_SELECT) {
      if (menu->selecting_player == 1 && menu->mode == MENU_VERSUS) {
        menu->selecting_player = 0;
        menu->cursor = menu->p1;
      } else {
        menu->screen = MENU_MODE;
      }
    } else if (menu->screen == MENU_ARENA) {
      menu->screen = MENU_SELECT;
      menu->selecting_player = menu->mode == MENU_VERSUS ? 1 : 0;
      menu->cursor = menu->selecting_player ? menu->p2 : menu->p1;
    }
    return false;
  }
  if (menu->screen == MENU_MODE) {
    if (keycode == SDLK_UP || keycode == SDLK_w) {
      menu->cursor = (menu->cursor + 2) % 3;
    } else if (keycode == SDLK_DOWN || keycode == SDLK_s) {
      menu->cursor = (menu->cursor + 1) % 3;
    } else if (confirm) {
      if (menu->cursor == 2) {
        *quit = true;
      } else {
        menu->mode = menu->cursor == 0 ? MENU_ARCADE : MENU_VERSUS;
        menu->screen = MENU_SELECT;
        menu->selecting_player = 0;
        menu->cursor = menu->p1;
      }
    }
    return false;
  }
  if (menu->screen == MENU_SELECT) {
    if (keycode == SDLK_LEFT || keycode == SDLK_a) {
      move_character(menu, -1, 0);
    } else if (keycode == SDLK_RIGHT || keycode == SDLK_d) {
      move_character(menu, 1, 0);
    } else if (keycode == SDLK_UP || keycode == SDLK_w) {
      move_character(menu, 0, -1);
    } else if (keycode == SDLK_DOWN || keycode == SDLK_s) {
      move_character(menu, 0, 1);
    } else if (confirm) {
      if (menu->selecting_player == 0) {
        menu->p1 = menu->cursor;
        if (menu->mode == MENU_VERSUS) {
          menu->selecting_player = 1;
          menu->cursor = menu->p2;
        } else {
          menu->p2 = (menu->p1 + 1) % roster_count();
          menu->screen = MENU_ARENA;
        }
      } else {
        menu->p2 = menu->cursor;
        menu->screen = MENU_ARENA;
      }
    }
    return false;
  }
  if (menu->screen == MENU_ARENA) {
    if (keycode == SDLK_LEFT || keycode == SDLK_a) {
      menu->arena = (menu->arena + MK4_ARENA_COUNT - 1) % MK4_ARENA_COUNT;
    } else if (keycode == SDLK_RIGHT || keycode == SDLK_d) {
      menu->arena = (menu->arena + 1) % MK4_ARENA_COUNT;
    } else if (confirm) {
      return true;
    }
  }
  return false;
}

static void draw_mode(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  const char *rows[] = {"ARCADE", "VERSUS", "EXIT GAME"};
  title_frame(canvas, assets, "MORTAL KOMBAT 4", "CHOOSE GAME MODE");
  for (int i = 0; i < 3; ++i) {
    Rect row = {390, 242 + i * 82, 500, 58};
    canvas_rect(canvas, row, i == menu->cursor ? 0xff273b4c : panel);
    canvas_outline(canvas, row, i == menu->cursor ? gold : edge);
    centered(&assets->hud.font, canvas, row.y + 13, rows[i], 1.0f);
  }
  centered(&assets->hud.font, canvas, 586, "UP DOWN TO CHOOSE   ENTER TO SELECT", .68f);
  centered(&assets->hud.font, canvas, 618, "ARCADE: PLAY AGAINST THE CPU    VERSUS: TWO PLAYERS", .56f);
}

static void draw_preview(Canvas *canvas, const Assets *assets, int roster, Rect bounds, float yaw) {
  static Bounds bounds_cache[MK4_ROSTER_COUNT];
  static bool cached[MK4_ROSTER_COUNT];
  const Character *character = &assets->characters.data[roster];
  const Clip *clip = &assets->clips.data[character->idle];
  if (!cached[roster]) {
    bounds_cache[roster] = character_bounds(character, clip);
    cached[roster] = true;
  }
  double frame = fmod((double)SDL_GetTicks() * 0.024, (double)clip->frames.count);
  draw_character(canvas, character, clip, frame, bounds, bounds_cache[roster], yaw, false, false);
  blit_name(canvas, &assets->hud.effects, assets->hud.names[roster], (Rect){bounds.x + 14, bounds.y + bounds.h - 46, bounds.w - 28, 29});
}

static void draw_select(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  title_frame(canvas, assets, "MORTAL KOMBAT 4", "CHOOSE YOUR FIGHTER");
  int active = menu->selecting_player == 0 ? menu->p1 : menu->p2;
  uint32_t active_color = menu->selecting_player == 0 ? blue : red;
  centered(&assets->hud.font, canvas, 181, menu->selecting_player == 0 ? "PLAYER 1: CHOOSE A FIGHTER" : "PLAYER 2: CHOOSE A FIGHTER", .75f);
  canvas_rect(canvas, (Rect){26, 246, 208, 302}, panel);
  canvas_outline(canvas, (Rect){26, 246, 208, 302}, menu->selecting_player == 0 ? blue : edge);
  canvas_rect(canvas, (Rect){1046, 246, 208, 302}, panel);
  canvas_outline(canvas, (Rect){1046, 246, 208, 302}, menu->selecting_player == 1 ? red : edge);
  draw_preview(canvas, assets, menu->p1, (Rect){33, 264, 194, 276}, -.15f);
  draw_preview(canvas, assets, menu->p2, (Rect){1053, 264, 194, 276}, .15f);
  for (int i = 0; i < roster_count(); ++i) {
    int column = i % 6;
    int row = i / 6;
    Rect tile = {258 + column * 128, 260 + row * 91, 116, 72};
    bool cursor = i == menu->cursor;
    bool chosen = i == active;
    canvas_rect(canvas, tile, cursor ? 0xff354454 : panel);
    canvas_outline(canvas, tile, cursor ? active_color : (chosen ? gold : edge));
    int name = assets->hud.names[i];
    const Sprite *sprite = &assets->hud.effects.sprites.data[name];
    int width = sprite->width;
    int height = sprite->height;
    float scale = fminf(1.5f, fminf(98.f / (float)width, 33.f / (float)height));
    int draw_width = (int)((float)width * scale);
    int draw_height = (int)((float)height * scale);
    blit_name(canvas, &assets->hud.effects, name, (Rect){tile.x + (tile.w - draw_width) / 2, tile.y + (tile.h - draw_height) / 2, draw_width, draw_height});
    if (chosen) {
      canvas_rect(canvas, (Rect){tile.x + 3, tile.y + tile.h - 7, tile.w - 6, 3}, i == menu->p1 ? blue : red);
    }
  }
  centered(&assets->hud.font, canvas, 592, "ARROWS / WASD TO MOVE   ENTER TO SELECT   ESC TO GO BACK", .62f);
}

static void draw_arena(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  title_frame(canvas, assets, "MORTAL KOMBAT 4", "CHOOSE THE ARENA");
  Rect stage = {272, 220, 736, 316};
  canvas_rect(canvas, stage, panel);
  canvas_outline(canvas, stage, gold);
  canvas_rect(canvas, (Rect){stage.x + 20, stage.y + 20, stage.w - 40, stage.h - 40}, 0xff202d38);
  for (int i = 0; i < 8; ++i) {
    int x = stage.x + 54 + i * 91;
    canvas_rect(canvas, (Rect){x, stage.y + 105 + (i % 3) * 18, 31, 110 - (i % 3) * 8}, 0xff293b49);
    canvas_rect(canvas, (Rect){x - 7, stage.y + 213, 45, 3}, 0xff55626a);
  }
  const Arena *arena = &assets->arenas.data[menu->arena];
  centered(&assets->hud.font, canvas, 562, arena->name, 1.25f);
  char choice[48];
  snprintf(choice, sizeof choice, "ARENA %02d / %02d", menu->arena + 1, MK4_ARENA_COUNT);
  centered(&assets->hud.font, canvas, 603, choice, .7f);
  centered(&assets->hud.font, canvas, 646, "LEFT / RIGHT TO CHOOSE   ENTER TO FIGHT   ESC TO GO BACK", .58f);
  draw_preview(canvas, assets, menu->p1, (Rect){78, 273, 172, 238}, -.18f);
  draw_preview(canvas, assets, menu->p2, (Rect){1030, 273, 172, 238}, .18f);
}

void menu_draw(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  if (menu->screen == MENU_TITLE) {
    canvas_rect(canvas, (Rect){0, 0, canvas->w, canvas->h}, background);
    canvas_rect(canvas, (Rect){0, 0, canvas->w, 8}, gold);
    canvas_rect(canvas, (Rect){0, canvas->h - 8, canvas->w, 8}, gold);
    centered(&assets->hud.font, canvas, 170, "MIDWAY PRESENTS", .9f);
    centered(&assets->hud.font, canvas, 239, "MORTAL KOMBAT 4", 2.3f);
    centered(&assets->hud.font, canvas, 321, "CHOOSE YOUR DESTINY", .95f);
    canvas_line(canvas, 250, 374, canvas->w - 250, 374, edge);
    centered(&assets->hud.font, canvas, 447, "PRESS START", 1.3f);
    centered(&assets->hud.font, canvas, 506, "ENTER OR SPACE", .72f);
    centered(&assets->hud.font, canvas, 682, "ESC TO EXIT", .6f);
  } else if (menu->screen == MENU_MODE) {
    draw_mode(canvas, assets, menu);
  } else if (menu->screen == MENU_SELECT) {
    draw_select(canvas, assets, menu);
  } else {
    draw_arena(canvas, assets, menu);
  }
}
