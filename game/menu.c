#include "menu.h"
#include "game.h"
#include "sprite_font.h"
#include "math_util.h"
#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t panel = 0xff1a2430;

static const int retail_roster[15] = {3, 7, 5, 12, 10, 0, 13, 8, 14, 11, 9, 1, 4, 6, 2};

enum {
  SELECT_COLUMNS = 3,
  SELECT_ROWS = 5
};

static void centered(const Geometry *font, Canvas *canvas, int y, const char *text, float scale) {
  int x = (canvas->w - font_width(font, text, scale)) / 2;
  font_draw(font, canvas, x, y, text, scale);
}

/* Fighter name plates are sprites in the HUD font atlas; center one at y. */
static void blit_name(Canvas *canvas, const Geometry *font, int index, int center_x, int y, float scale) {
  if (index < 0 || (size_t)index >= font->sprites.count) {
    return;
  }
  const Sprite *sprite = &font->sprites.data[index];
  if (sprite->texture < 0 || (size_t)sprite->texture >= font->textures.count) {
    return;
  }
  int w = (int)((float)sprite->w * scale);
  int h = (int)((float)sprite->h * scale);
  canvas_blit(canvas, &font->textures.data[sprite->texture], (Rect){sprite->u, sprite->v, sprite->w, sprite->h}, (Rect){center_x - w / 2, y, w, h}, false);
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
  // The custom wall stacks the retail order in three columns; the three
  // footer buttons sit directly below them.
  if (menu->cursor >= 15) {
    int column = menu->cursor - 15;
    if (dy < 0) {
      menu->cursor = (SELECT_ROWS - 1) * SELECT_COLUMNS + column;
    } else if (dy > 0) {
      menu->cursor = column;
    } else if (dx != 0) {
      menu->cursor = 15 + (column + dx + SELECT_COLUMNS) % SELECT_COLUMNS;
    }
    return;
  }
  int column = menu->cursor % SELECT_COLUMNS;
  int row = menu->cursor / SELECT_COLUMNS;
  if ((dy > 0 && row == SELECT_ROWS - 1) || (dy < 0 && row == 0)) {
    menu->cursor = 15 + column;
    return;
  }
  column = (column + dx + SELECT_COLUMNS) % SELECT_COLUMNS;
  row += dy;
  menu->cursor = row * SELECT_COLUMNS + column;
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
  const Geometry *font = &assets->hud.font;
  int width = canvas->w;
  int height = canvas->h;
  // mklogo_g.geo stores the 408x256 title backdrop as four 102-pixel strips:
  // the left and right halves of page 0 and page 1, interleaved.
  static const int strips[4][2] = {{0, 0}, {1, 0}, {0, 102}, {1, 102}};
  for (int i = 0; i < 4; ++i) {
    int x = i * width / 4;
    blit_page(canvas, &assets->hud.title_scene, strips[i][0], (Rect){strips[i][1], 0, 102, 256}, (Rect){x, 0, (i + 1) * width / 4 - x, height});
  }
  // Retail proportions: six stacked bars across the middle 45% of the
  // screen, from 42% of its height, with a lit top edge, a darker right
  // bevel and a dark separator below each bar.
  int bar_x = width * 275 / 1000;
  int bar_w = width * 450 / 1000;
  int pitch = height * 84 / 1000;
  int top = height * 42 / 100;
  int separator = maxi(2, pitch / 10);
  int bevel = bar_w * 2 / 100;
  for (int i = 0; i < 6; ++i) {
    int y = top + i * pitch;
    int h = pitch - separator;
    canvas_rect(canvas, (Rect){bar_x, y, bar_w, h}, 0xff0f2d51);
    canvas_rect(canvas, (Rect){bar_x + bar_w - bevel, y, bevel, h}, 0xff0c2041);
    canvas_rect(canvas, (Rect){bar_x, y, bar_w, maxi(2, pitch / 22)}, 0xff2d5075);
    canvas_rect(canvas, (Rect){bar_x, y + h, bar_w, separator}, 0xff02060f);
    // Capital glyphs stand about 15.5 font units tall from the pen.
    float text_scale = 1.9f;
    int text_width = font_width(font, rows[i], text_scale);
    font_draw(font, canvas, bar_x + (bar_w - text_width) / 2, y + (h - (int)(15.5f * text_scale)) / 2, rows[i], text_scale);
    if (i == menu->cursor) {
      for (int ring = 0; ring < 3; ++ring) {
        canvas_outline(canvas, (Rect){bar_x - 1 - ring, y - 1 - ring, bar_w + 2 + 2 * ring, h + 2 + 2 * ring}, 0xffd03c2d);
      }
    }
  }
  static const char *hints[2][2] = {{"UP/DOWN", "TO CHOOSE"}, {"ACTION KEYS", "TO SELECT"}};
  for (int side = 0; side < 2; ++side) {
    int center = width * (side == 0 ? 155 : 845) / 1000;
    for (int line = 0; line < 2; ++line) {
      const char *text = hints[side][line];
      font_draw(font, canvas, center - font_width(font, text, 1.15f) / 2, height * (84 + line * 5) / 100, text, 1.15f);
    }
  }
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
  blit_name(canvas, &assets->hud.font, assets->hud.names[roster], bounds.x + bounds.w / 2, bounds.y + bounds.h - 46, 1.5f);
}

typedef struct SelectTile {
  int cursor;
  float depth;
  float corners[4][2];
  float frame[4][2];
} SelectTile;

typedef struct SelectCamera {
  float cx, cy, focal;
} SelectCamera;

static void select_project(const SelectCamera *camera, Vec3 p, float out[2]) {
  out[0] = camera->cx + camera->focal * p.x / p.z;
  out[1] = camera->cy - camera->focal * p.y / p.z;
}

static void select_quad(const SelectCamera *camera, Vec3 center, Vec3 right, Vec3 up, float half, float out[4][2]) {
  static const float signs[4][2] = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
  for (int i = 0; i < 4; ++i) {
    Vec3 corner = vec_add(center, vec_add(vec_scale(right, signs[i][0] * half), vec_scale(up, signs[i][1] * half)));
    select_project(camera, corner, out[i]);
  }
}

/* Lays the portraits on the inside of a shallow dish facing the camera: the
 * outer columns and rows turn inward, so their outer edges come closer and
 * the wall opens toward the two fighters. */
static SelectTile select_tile(const SelectCamera *camera, int cursor, bool focused) {
  const float distance = 4.f;
  const float column_step = .58f;
  const float row_step = .08f;
  const float column_radius = 1.1f / column_step;
  const float row_radius = 1.08f / row_step;
  int column = cursor % SELECT_COLUMNS;
  int row = cursor / SELECT_COLUMNS;
  float yaw = (float)(column - 1) * column_step;
  float pitch = (float)(2 - row) * row_step;
  Vec3 center = {column_radius * sinf(yaw), row_radius * sinf(pitch), distance - column_radius * (1 - cosf(yaw)) - row_radius * (1 - cosf(pitch))};
  Vec3 right = {cosf(yaw), 0, -sinf(yaw)};
  Vec3 up = {0, cosf(pitch), -sinf(pitch)};
  float half = .5f;
  if (focused) {
    center.z -= .12f;
    half = .53f;
  }
  // The focused tile always paints last, over any nearer outer neighbor.
  SelectTile tile = {cursor, focused ? -INFINITY : center.z, {{0}}, {{0}}};
  select_quad(camera, center, right, up, half, tile.corners);
  select_quad(camera, center, right, up, half + (focused ? .08f : .035f), tile.frame);
  return tile;
}

static void select_shade(Canvas *canvas, Rect area, uint32_t color) {
  float corners[4][2] = {{(float)area.x, (float)area.y}, {(float)(area.x + area.w), (float)area.y}, {(float)(area.x + area.w), (float)(area.y + area.h)},
      {(float)area.x, (float)(area.y + area.h)}};
  canvas_quad(canvas, NULL, (Rect){0, 0, 0, 0}, corners, color);
}

static void select_floor(Canvas *canvas, int cx, int cy, int rx, int ry) {
  // A soft dark pool grounds the fighter on the marble backdrop.
  for (int y = cy - ry; y <= cy + ry; ++y) {
    for (int x = cx - rx; x <= cx + rx; ++x) {
      if (x < 0 || y < 0 || x >= canvas->w || y >= canvas->h) {
        continue;
      }
      float dx = (float)(x - cx) / (float)rx;
      float dy = (float)(y - cy) / (float)ry;
      float d = dx * dx + dy * dy;
      if (d >= 1) {
        continue;
      }
      unsigned keep = (unsigned)(255.f * (.35f + .65f * d));
      uint32_t *at = &canvas->pixels[(size_t)y * (size_t)canvas->w + (size_t)x];
      uint32_t c = *at;
      *at = 0xff000000 | ((((c >> 16) & 255) * keep / 255) << 16) | ((((c >> 8) & 255) * keep / 255) << 8) | ((c & 255) * keep / 255);
    }
  }
}

static void select_silhouette(Canvas *canvas, Rect area) {
  for (int y = maxi(0, area.y); y < mini(canvas->h, area.y + area.h); ++y) {
    for (int x = maxi(0, area.x); x < mini(canvas->w, area.x + area.w); ++x) {
      size_t at = (size_t)y * (size_t)canvas->w + (size_t)x;
      if (canvas->depth[at] != -INFINITY) {
        canvas->pixels[at] = 0xff2b3038;
      }
    }
  }
}

static int select_hovered(const FrontendMenu *menu, int player) {
  int current = player == 0 ? menu->p1 : menu->p2;
  if (menu->cursor < 15) {
    return retail_roster[menu->cursor];
  }
  return menu->cursor == 16 ? current : -1;
}

static void draw_move_list(Canvas *canvas, const Assets *assets, const CharacterDef *def, Rect area) {
  const Geometry *font = &assets->hud.font;
  const float text = .55f;
  const float heading = .6f;
  int rows = def->move_list_count == 0 ? 2 : 0;
  for (int i = 0; i < def->move_list_count; ++i) {
    rows += i == 0 || def->move_list[i].kind != def->move_list[i - 1].kind ? 2 : 1;
  }
  int line = mini(22, (area.h - 16) / rows);
  select_shade(canvas, (Rect){area.x, area.y, area.w, rows * line + 16}, 0xb0080606);
  canvas_rect(canvas, (Rect){area.x, area.y, area.w, 2}, 0xffb01810);
  int x = area.x + 12;
  int y = area.y + 8;
  int right = area.x + area.w - 12;
  if (def->move_list_count == 0) {
    font_draw(font, canvas, x, y, "SPECIAL MOVES", heading);
    font_draw(font, canvas, x + 6, y + line, "BASIC ATTACKS ONLY", text);
    return;
  }
  for (int i = 0; i < def->move_list_count; ++i) {
    const MoveListEntry *entry = &def->move_list[i];
    if (i == 0 || entry->kind != def->move_list[i - 1].kind) {
      font_draw(font, canvas, x, y, entry->kind == MOVE_LIST_SPECIAL ? "SPECIAL MOVES" : "FINISHERS", heading);
      y += line;
    }
    font_draw(font, canvas, x + 6, y, entry->name, text);
    font_draw(font, canvas, right - font_width(font, entry->input, text), y, entry->input, text);
    y += line;
  }
}

static void draw_select_side(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, int player, Rect side, float seconds) {
  static Bounds bounds_cache[MK4_ROSTER_COUNT];
  static bool cached[MK4_ROSTER_COUNT];
  const Geometry *font = &assets->hud.font;
  bool team = menu->mode == MENU_TEAM;
  bool active = menu->selecting_player == player;
  bool cpu = player == 1 && !team;
  int roster = -1;
  if (active) {
    roster = select_hovered(menu, player);
  } else if (player == 0) {
    roster = menu->p1;
  } else if (cpu) {
    // Single-player modes face the next roster slot; see menu_key.
    int hovered = select_hovered(menu, 0);
    roster = hovered < 0 ? -1 : (hovered + 1) % roster_count();
  } else {
    roster = menu->p2;
  }
  const char *label = cpu ? "CPU" : player == 0 ? "PLAYER 1" : "PLAYER 2";
  int label_x = player == 0 ? side.x + 24 : side.x + side.w - 24 - font_width(font, label, .75f);
  font_draw(font, canvas, label_x, side.y + 6, label, .75f);

  // draw_character centers the idle bounds, leaving the feet 46 pixels
  // above the bottom of the stage rectangle.
  Rect stage = {side.x + 10, side.y + 24, side.w - 20, 452};
  int floor_y = stage.y + stage.h - 46;
  select_floor(canvas, stage.x + stage.w / 2, floor_y - 4, stage.w * 36 / 100, 20);
  if (roster < 0) {
    const char *mystery = "?";
    const char *caption = menu->cursor == 15 ? "RANDOM FIGHTER" : "HIDDEN FIGHTER";
    font_draw(font, canvas, stage.x + (stage.w - font_width(font, mystery, 7.f)) / 2, stage.y + 90, mystery, 7.f);
    font_draw(font, canvas, stage.x + (stage.w - font_width(font, caption, .9f)) / 2, floor_y + 8, caption, .9f);
  } else {
    const Character *character = roster_character(assets, roster);
    const Clip *clip = &assets->clips.data[character->idle];
    if (!cached[roster]) {
      bounds_cache[roster] = character_bounds(character, clip);
      cached[roster] = true;
    }
    double frame = fmod((double)seconds * 24.0, (double)clip->frames.count);
    // Both previews share one turn toward the wall; player two is mirrored
    // the way a fighter facing left is in battle.
    if (player == 0) {
      draw_character(canvas, character, clip, frame, stage, bounds_cache[roster], -.55f, false, false);
    } else {
      draw_character_mirrored(canvas, character, clip, frame, stage, bounds_cache[roster], -.55f);
    }
    if (cpu || (!active && player == 1)) {
      // Team mode shows player two's previous pick until their turn.
      select_silhouette(canvas, stage);
    } else {
      blit_name(canvas, font, assets->hud.names[roster], stage.x + stage.w / 2, floor_y + 8, 2.f);
    }
  }
  Rect list = {side.x + 16, side.y + 482, side.w - 32, side.h - 482};
  if (roster >= 0 && !cpu && (active || player == 0)) {
    draw_move_list(canvas, assets, roster_def(roster), list);
  }
}

static void draw_select(Canvas *canvas, const Assets *assets, const FrontendMenu *menu) {
  int width = canvas->w;
  int height = canvas->h;
  float seconds = (float)SDL_GetTicks() / 1000.f;
  // Use the clean marble regions, excluding the player-number/color swatches.
  for (int row = 0; row < 2; ++row) {
    int y = row * height / 2;
    int h = (row + 1) * height / 2 - y;
    blit_page(canvas, &assets->hud.select_scene, 0, (Rect){0, 0, 212, 112}, (Rect){0, y, width / 2, h});
    blit_page(canvas, &assets->hud.select_scene, 1, (Rect){0, 0, 168, 112}, (Rect){width / 2, y, width - width / 2, h});
  }
  select_shade(canvas, (Rect){0, 0, width, height}, 0x8c000000);
  select_shade(canvas, (Rect){0, 0, width * 31 / 100, height}, 0x50000000);
  select_shade(canvas, (Rect){width - width * 31 / 100, 0, width * 31 / 100, height}, 0x50000000);
  blit_page(canvas, &assets->hud.select_scene, 2, (Rect){66, 229, 180, 16}, (Rect){width * 30 / 100, height / 100, width * 40 / 100, height * 55 / 1000});

  // The full-size portraits occupy page 3, indexed here by roster slot and
  // matched against each fighter's model; Scorpion's is on page one. Page 2
  // contains only the tiny GROUP SELECT thumbnail.
  static const int portrait_cells[15] = {-1, 0, 1, 2, 3, 8, 9, 6, 4, 12, 11, 10, 13, 5, 7};
  SelectCamera camera = {(float)width * .5f, (float)height * .48f, (float)height * .475f};
  SelectTile tiles[15];
  for (int i = 0; i < 15; ++i) {
    tiles[i] = select_tile(&camera, i, i == menu->cursor);
  }
  // Paint far tiles first, then the focused tile.
  for (int i = 1; i < 15; ++i) {
    SelectTile tile = tiles[i];
    int j = i - 1;
    for (; j >= 0 && tiles[j].depth < tile.depth; --j) {
      tiles[j + 1] = tiles[j];
    }
    tiles[j + 1] = tile;
  }
  float pulse = .5f + .5f * sinf(seconds * 6.f);
  uint32_t player_color = menu->selecting_player == 0 ? 0xffe02010 : 0xff3080ff;
  for (int i = 0; i < 15; ++i) {
    const SelectTile *tile = &tiles[i];
    bool focused = tile->cursor == menu->cursor;
    int cell = portrait_cells[retail_roster[tile->cursor]];
    Rect source = cell < 0 ? (Rect){168, 112, 64, 64} : (Rect){(cell % 4) * 64, (cell / 4) * 64, 64, 64};
    const Texture *page = &assets->hud.select_scene.textures.data[cell < 0 ? 1 : 3];
    uint32_t frame = 0xff14100c;
    if (focused) {
      unsigned glow = (unsigned)(200 + 55 * pulse);
      frame = (player_color & 0xff000000) | ((((player_color >> 16) & 255) * glow / 255) << 16) | ((((player_color >> 8) & 255) * glow / 255) << 8) |
              ((player_color & 255) * glow / 255);
    }
    canvas_quad(canvas, NULL, (Rect){0, 0, 0, 0}, tile->frame, frame);
    canvas_quad(canvas, page, source, tile->corners, focused ? 0xffffffff : 0xff9c948c);
  }

  int side_width = width * 31 / 100;
  int footer_y = height * 93 / 100;
  draw_select_side(canvas, assets, menu, 0, (Rect){0, 0, side_width, footer_y - 8}, seconds);
  draw_select_side(canvas, assets, menu, 1, (Rect){width - side_width, 0, side_width, footer_y - 8}, seconds);

  blit_page(canvas, &assets->hud.select_scene, 0, (Rect){84, 225, 128, 30}, (Rect){width * 23 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  blit_page(canvas, &assets->hud.select_scene, 1, (Rect){0, 225, 128, 30}, (Rect){width * 41 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  blit_page(canvas, &assets->hud.select_scene, 2, (Rect){64, 197, 128, 30}, (Rect){width * 59 / 100, footer_y, width * 18 / 100, height * 6 / 100});
  if (menu->cursor >= 15) {
    int button_x = width * (23 + (menu->cursor - 15) * 18) / 100;
    for (int ring = 0; ring < 3; ++ring) {
      canvas_outline(canvas, (Rect){button_x - 3 - ring, footer_y - 3 - ring, width * 18 / 100 + 6 + 2 * ring, height * 6 / 100 + 6 + 2 * ring}, player_color);
    }
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
  // Resets depth for the fighter previews; each screen repaints every pixel.
  canvas_clear(canvas);
  if (menu->screen == MENU_MODE) {
    draw_mode(canvas, assets, menu);
  } else if (menu->screen == MENU_SELECT) {
    draw_select(canvas, assets, menu);
  } else {
    draw_arena(canvas, assets, menu);
  }
}
