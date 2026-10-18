#include "menu.h"
#include "game.h"
#include "sprite_font.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t panel = 0xff1a2430;
static const uint32_t edge = 0xff485b6c;
static const uint32_t red = 0xffff1818;

static const int retail_roster[15] = {3, 7, 5, 12, 10, 0, 13, 8, 14, 11, 9, 1, 4, 6, 2};

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

static void blit_page(Canvas *canvas, const Geometry *geometry, int page, Rect source, Rect destination) {
  if (page < 0 || (size_t)page >= geometry->textures.count) {
    return;
  }
  canvas_blit(canvas, &geometry->textures.data[page], source, destination, false);
}

static int retail_cursor_for_roster(int roster) {
  for (int i = 0; i < 15; ++i) {
    if (retail_roster[i] == roster) {
      return i;
    }
  }
  return 0;
}

static int roster_for_retail_cursor(int cursor, int current) {
  if (cursor < 15) {
    return retail_roster[cursor];
  }
  if (cursor == 15) {
    return rand() % 15;
  }
  if (cursor == 17) {
    return 15 + rand() % 3;
  }
  return current;
}

void menu_init(FrontendMenu *menu) {
  memset(menu, 0, sizeof *menu);
  srand(SDL_GetTicks());
  menu->screen = MENU_MODE;
  menu->p1 = 0;
  menu->p2 = 4;
  menu->arena = 8;
  menu->cursor = 0;
}

static void move_character(FrontendMenu *menu, int dx, int dy) {
  if (menu->cursor >= 15) {
    if (dy < 0) {
      menu->cursor = 10 + menu->cursor - 15;
    } else if (dx != 0) {
      menu->cursor = 15 + (menu->cursor - 15 + dx + 3) % 3;
    }
    return;
  }
  int column = menu->cursor % 5;
  int row = menu->cursor / 5;
  if (dy != 0 && row == 2 && dy > 0) {
    menu->cursor = 15 + (column * 3) / 5;
    return;
  }
  if (dy != 0 && row == 0 && dy < 0) {
    menu->cursor = 15 + (column * 3) / 5;
    return;
  }
  column = (column + dx + 5) % 5;
  row = (row + dy + 3) % 3;
  menu->cursor = row * 5 + column;
}

bool menu_key(FrontendMenu *menu, int keycode, bool *quit) {
  bool confirm = keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER || keycode == SDLK_SPACE;
  bool back = keycode == SDLK_ESCAPE || keycode == SDLK_BACKSPACE;
  if (back) {
    if (menu->screen == MENU_MODE) {
      *quit = true;
    } else if (menu->screen == MENU_SELECT) {
      if (menu->selecting_player == 1 && menu->mode == MENU_TEAM) {
        menu->selecting_player = 0;
        menu->cursor = retail_cursor_for_roster(menu->p1);
      } else {
        menu->screen = MENU_MODE;
      }
    } else if (menu->screen == MENU_ARENA) {
      menu->screen = MENU_SELECT;
      menu->selecting_player = menu->mode == MENU_TEAM ? 1 : 0;
      menu->cursor = retail_cursor_for_roster(menu->selecting_player ? menu->p2 : menu->p1);
    }
    return false;
  }
  if (menu->screen == MENU_MODE) {
    if (keycode == SDLK_UP || keycode == SDLK_w) {
      menu->cursor = (menu->cursor + 5) % 6;
    } else if (keycode == SDLK_DOWN || keycode == SDLK_s) {
      menu->cursor = (menu->cursor + 1) % 6;
    } else if (confirm) {
      if (menu->cursor == MENU_EXIT) {
        *quit = true;
      } else {
        menu->mode = (MenuMode)menu->cursor;
        menu->screen = MENU_SELECT;
        menu->selecting_player = 0;
        menu->cursor = retail_cursor_for_roster(menu->p1);
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
        menu->p1 = roster_for_retail_cursor(menu->cursor, menu->p1);
        if (menu->mode == MENU_TEAM) {
          menu->selecting_player = 1;
          menu->cursor = retail_cursor_for_roster(menu->p2);
        } else {
          menu->p2 = (menu->p1 + 1) % roster_count();
          menu->screen = MENU_ARENA;
        }
      } else {
        menu->p2 = roster_for_retail_cursor(menu->cursor, menu->p2);
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
  static const char *rows[] = {"ARCADE", "TEAM", "ENDURANCE", "TOURNAMENT", "PRACTICE", "EXIT GAME"};
  int width = canvas->w;
  int height = canvas->h;
  canvas_rect(canvas, (Rect){0, 0, width, height}, 0xff071728);
  blit_page(canvas, &assets->hud.title_scene, 0, (Rect){0, 0, 203, 210}, (Rect){0, 0, width / 2, height});
  blit_page(canvas, &assets->hud.title_scene, 1, (Rect){100, 0, 103, 210}, (Rect){width / 2, 0, width / 2, height});
  int menu_width = width * 46 / 100;
  int menu_x = (width - menu_width) / 2;
  int row_height = height * 8 / 100;
  int gap = 0;
  int block_height = row_height * 6;
  int top = height * 49 / 100;
  if (top + block_height > height - 40) {
    top = height - 40 - block_height;
  }
  for (int i = 0; i < 6; ++i) {
    Rect row = {menu_x, top + i * (row_height + gap), menu_width, row_height - 2};
    canvas_rect(canvas, row, 0xe8001424);
    if (i == menu->cursor) {
      canvas_outline(canvas, row, red);
    }
    float text_scale = 1.1f;
    int text_width = font_width(&assets->hud.font, rows[i], text_scale);
    int text_x = row.x + (row.w - text_width) / 2;
    font_draw(&assets->hud.font, canvas, text_x, row.y + (row.h - (int)(30 * text_scale)) / 2, rows[i], text_scale);
  }
  font_draw(&assets->hud.font, canvas, width * 7 / 100, height - 80, "UP/DOWN", 1.f);
  font_draw(&assets->hud.font, canvas, width * 7 / 100, height - 49, "TO CHOOSE", 1.f);
  font_draw(&assets->hud.font, canvas, width * 74 / 100, height - 80, "ACTION KEYS", 1.f);
  font_draw(&assets->hud.font, canvas, width * 79 / 100, height - 49, "SELECT", 1.f);
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
  int width = canvas->w;
  int height = canvas->h;
  canvas_rect(canvas, (Rect){0, 0, width, height}, 0xff6e6e65);
  // The full-size portraits occupy page 3; page 2 contains only the tiny
  // GROUP SELECT thumbnail, whose frame and labels must not enter the grid.
  static const int portrait_cells[15] = {10, 0, 9, 2, 11, -1, 5, 6, 7, 13, 12, 8, 3, 4, 1};
  // Use the clean marble regions, excluding the player-number/color swatches.
  for (int row = 0; row < 2; ++row) {
    int y = row * height / 2;
    int h = (row + 1) * height / 2 - y;
    blit_page(canvas, &assets->hud.select_scene, 0, (Rect){0, 0, 212, 112}, (Rect){0, y, width / 2, h});
    blit_page(canvas, &assets->hud.select_scene, 1, (Rect){0, 0, 168, 112}, (Rect){width / 2, y, width - width / 2, h});
  }
  int grid_x = width * 5 / 100;
  int grid_y = height * 8 / 100;
  int grid_width = width * 90 / 100;
  int grid_height = height * 84 / 100;
  int gap = 3;
  blit_page(canvas, &assets->hud.select_scene, 2, (Rect){66, 229, 180, 16}, (Rect){width * 28 / 100, height / 100, width * 44 / 100, height * 6 / 100});
  for (int i = 0; i < 15; ++i) {
    int col = i % 5;
    int row = i / 5;
    int x = grid_x + col * grid_width / 5;
    int y = grid_y + row * grid_height / 3;
    Rect dst = {x, y, grid_x + (col + 1) * grid_width / 5 - x - gap, grid_y + (row + 1) * grid_height / 3 - y - gap};
    int cell = portrait_cells[i];
    Rect src = cell < 0 ? (Rect){168, 112, 64, 64} : (Rect){(cell % 4) * 64, (cell / 4) * 64, 64, 64};
    blit_page(canvas, &assets->hud.select_scene, cell < 0 ? 1 : 3, src, dst);
    canvas_outline(canvas, dst, edge);
    if (i == menu->cursor) {
      canvas_outline(canvas, (Rect){dst.x - 1, dst.y - 1, dst.w + 2, dst.h + 2}, red);
    }
  }
  int footer_y = height * 93 / 100;
  blit_page(canvas, &assets->hud.select_scene, 0, (Rect){84, 225, 128, 30}, (Rect){width * 23 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  blit_page(canvas, &assets->hud.select_scene, 1, (Rect){0, 225, 128, 30}, (Rect){width * 41 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  blit_page(canvas, &assets->hud.select_scene, 2, (Rect){64, 197, 128, 30}, (Rect){width * 59 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  if (menu->cursor >= 15) {
    int button_x = width * (23 + (menu->cursor - 15) * 18) / 100;
    canvas_outline(canvas, (Rect){button_x - 4, footer_y - 4, width * 18 / 100 + 8, height * 6 / 100 + 8}, red);
  }
}

static void draw_arena(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  canvas_rect(canvas, (Rect){0, 0, canvas->w, canvas->h}, 0xff10151d);
  centered(&assets->hud.font, canvas, 90, "CHOOSE THE ARENA", 1.4f);
  Rect stage = {272, 220, 736, 316};
  canvas_rect(canvas, stage, panel);
  canvas_outline(canvas, stage, 0xffffd56b);
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
  if (menu->screen == MENU_MODE) {
    draw_mode(canvas, assets, menu);
  } else if (menu->screen == MENU_SELECT) {
    draw_select(canvas, assets, menu);
  } else {
    draw_arena(canvas, assets, menu);
  }
}
