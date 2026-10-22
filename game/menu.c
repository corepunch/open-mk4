#include "menu.h"
#include "game.h"
#include "draw.h"
#include "sprite_font.h"
#include "math_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void menu_init(FrontendMenu *menu, const Mk4Image *image) {
  memset(menu, 0, sizeof *menu);
  menu->screen = MENU_MODE;
  menu->p1 = 0;
  menu->p2 = 4;
  menu->arena = 8;
  menu->cursor = 0;
  /* Local EXE: five 16-byte column records, terminated packed opponent
   * lists, and five 12-byte label records. Pointers are word indices. */
  for (int i = 0; i < MENU_TOWERS; ++i) {
    MenuTower *tower = &menu->towers[i];
    uint32_t list = mk4_image_u32(image, MK4_RETAIL_DATA_TOWER_COLUMNS + i * 16) * 4;
    uint32_t name = mk4_image_u32(image, MK4_RETAIL_DATA_TOWER_LABELS + i * 12) * 4;
    require(name >= MK4_RETAIL_DATA_IMAGE_BASE, "Invalid retail tower label");
    view_string(view(image->bytes), name - MK4_RETAIL_DATA_IMAGE_BASE, tower->name, sizeof tower->name);
    while (tower->count < MENU_TOWER_FLOORS) {
      uint32_t packed = mk4_image_u32(image, list + tower->count * 4);
      if (!packed) {
        break;
      }
      for (int face = 0; face < 4; ++face) {
        require(((packed >> (face * 8)) & 255) < MK4_ROSTER_COUNT, "Invalid retail tower opponent");
      }
      tower->opponents[tower->count++] = packed;
    }
    require(tower->count > 0 && tower->count < MENU_TOWER_FLOORS, "Invalid retail tower length");
  }
}

int menu_tower_opponent(const FrontendMenu *menu, int tower, int floor) {
  require(tower >= 0 && tower < MENU_TOWERS && floor >= 0 && floor < menu->towers[tower].count, "Invalid tower position");
  return (int)((menu->towers[tower].opponents[floor] >> (menu->face * 8)) & 255);
}

void menu_match_finished(FrontendMenu *menu, bool won) {
  if (!won) {
    menu->screen = MENU_CONTINUE;
    return;
  }
  ++menu->floor;
  menu->screen = menu->floor == menu->towers[menu->tower].count ? MENU_COMPLETE : MENU_TOWER;
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

bool menu_command(FrontendMenu *menu, MenuCommand command, bool *quit) {
  bool confirm = command == MENU_CONFIRM;
  bool back = command == MENU_BACK;
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
    } else if (menu->screen == MENU_TOWER && !menu->climbing) {
      menu->screen = MENU_SELECT;
      menu->cursor = retail_cursor_for_roster(menu->p1);
    } else if (menu->screen == MENU_TOWER || menu->screen == MENU_CONTINUE || menu->screen == MENU_COMPLETE) {
      menu->climbing = false;
      menu->screen = MENU_MODE;
      menu->cursor = MENU_ARCADE;
    } else if (menu->screen == MENU_ARENA) {
      menu->screen = MENU_SELECT;
      menu->selecting_player = menu->mode == MENU_TEAM ? 1 : 0;
      menu->cursor = retail_cursor_for_roster(menu->selecting_player ? menu->p2 : menu->p1);
    }
    return false;
  }
  if (menu->screen == MENU_MODE) {
    if (command == MENU_UP) {
      menu->cursor = (menu->cursor + 5) % 6;
    } else if (command == MENU_DOWN) {
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
    if (command == MENU_LEFT) {
      move_character(menu, -1, 0);
    } else if (command == MENU_RIGHT) {
      move_character(menu, 1, 0);
    } else if (command == MENU_UP) {
      move_character(menu, 0, -1);
    } else if (command == MENU_DOWN) {
      move_character(menu, 0, 1);
    } else if (confirm) {
      if (menu->selecting_player == 0) {
        menu->p1 = roster_for_retail_cursor(menu->cursor, menu->p1);
        if (menu->mode == MENU_TEAM) {
          menu->selecting_player = 1;
          menu->cursor = retail_cursor_for_roster(menu->p2);
        } else {
          menu->p2 = (menu->p1 + 1) % roster_count();
          menu->screen = menu->mode == MENU_ARCADE ? MENU_TOWER : MENU_ARENA;
          menu->tower = 0;
          menu->floor = 0;
          menu->face = rand() % 4;
          menu->climbing = false;
        }
      } else {
        menu->p2 = roster_for_retail_cursor(menu->cursor, menu->p2);
        menu->screen = MENU_ARENA;
      }
    }
    return false;
  }
  if (menu->screen == MENU_COMPLETE) {
    if (confirm) {
      menu->climbing = false;
      menu->screen = MENU_MODE;
      menu->cursor = MENU_ARCADE;
    }
    return false;
  }
  if (menu->screen == MENU_TOWER || menu->screen == MENU_CONTINUE) {
    if (!menu->climbing) {
      if (command == MENU_LEFT) {
        menu->tower = (menu->tower + MENU_TOWERS - 1) % MENU_TOWERS;
      } else if (command == MENU_RIGHT) {
        menu->tower = (menu->tower + 1) % MENU_TOWERS;
      }
    }
    if (confirm) {
      menu->climbing = true;
      menu->p2 = menu_tower_opponent(menu, menu->tower, menu->floor);
      menu->arena = menu->p2 == 15 ? 0 : (8 + menu->floor) % MK4_ARENA_COUNT;
      return true;
    }
    return false;
  }
  if (menu->screen == MENU_ARENA) {
    if (command == MENU_LEFT) {
      menu->arena = (menu->arena + MK4_ARENA_COUNT - 1) % MK4_ARENA_COUNT;
    } else if (command == MENU_RIGHT) {
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

static void draw_preview(Canvas *canvas, const Assets *assets, int roster, Rect bounds, float yaw, double seconds) {
  static Bounds bounds_cache[MK4_ROSTER_COUNT];
  static bool cached[MK4_ROSTER_COUNT];
  const Character *character = &assets->characters.data[roster];
  const Clip *clip = &assets->clips.data[character->idle];
  if (!cached[roster]) {
    bounds_cache[roster] = character_bounds(character, clip);
    cached[roster] = true;
  }
  double frame = fmod(seconds * 24.0, (double)clip->frames.count);
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
  const float column_step = .29f;
  const float row_step = .04f;
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
    // Single-player modes face the next roster slot; see menu_command.
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

static void draw_select(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds) {
  int width = canvas->w;
  int height = canvas->h;
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
  SelectCamera camera = {(float)width * .5f, (float)height * .48f, (float)height * .54f};
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

static void draw_arena(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds) {
  canvas_rect(canvas, (Rect){0, 0, canvas->w, canvas->h}, 0xff10151d);
  centered(&assets->hud.font, canvas, 90, "CHOOSE THE ARENA", 1.4f);
  Rect stage = {272, 220, 736, 316};
  const Arena *arena = &assets->arenas.data[menu->arena];
  Canvas preview;
  canvas_init(&preview, stage.w, stage.h);
  draw_arena_preview(&preview, arena);
  Texture image = {.width = preview.w, .height = preview.h, .pixels = {preview.pixels, (size_t)preview.w * preview.h, (size_t)preview.w * preview.h}};
  canvas_blit(canvas, &image, (Rect){0, 0, preview.w, preview.h}, stage, false);
  canvas_free(&preview);
  canvas_outline(canvas, stage, 0xffffd56b);
  centered(&assets->hud.font, canvas, 562, arena->name, 1.25f);
  char choice[48];
  snprintf(choice, sizeof choice, "ARENA %02d / %02d", menu->arena + 1, MK4_ARENA_COUNT);
  centered(&assets->hud.font, canvas, 603, choice, .7f);
  centered(&assets->hud.font, canvas, 646, "LEFT / RIGHT TO CHOOSE   ENTER TO FIGHT   ESC TO GO BACK", .58f);
  draw_preview(canvas, assets, menu->p1, (Rect){78, 273, 172, 238}, -.18f, seconds);
  draw_preview(canvas, assets, menu->p2, (Rect){1030, 273, 172, 238}, .18f, seconds);
}

/* Use the retail portrait meshes' UV rectangles so atlas packing remains
 * asset-owned. The mesh identities follow the retail ladder face table. */
static void tower_portrait(Canvas *canvas, const Geometry *scene, int roster, Rect dest) {
  static const int meshes[MK4_ROSTER_COUNT] = {13, 15, 17, 16, 20, 19, 21, 23, 29, 24, 22, 28, 25, 26, 27, 18, 17, 14};
  const Mesh *mesh = &scene->meshes.data[meshes[roster]];
  int u0 = 256, v0 = 256, u1 = 0, v1 = 0;
  for (size_t i = 0; i < mesh->triangles.count; ++i) {
    for (int j = 0; j < 3; ++j) {
      const Vertex *v = &mesh->triangles.data[i].v[j];
      u0 = mini(u0, (int)v->u);
      v0 = mini(v0, (int)v->v);
      u1 = maxi(u1, (int)v->u);
      v1 = maxi(v1, (int)v->v);
    }
  }
  blit_page(canvas, scene, mesh->triangles.data[0].texture, (Rect){u0, v0, u1 - u0 + 1, v1 - v0 + 1}, dest);
}

static void draw_tower(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds) {
  const Geometry *scene = &assets->hud.ladder_scene;
  const Geometry *font = &assets->hud.font;
  int w = canvas->w, h = canvas->h;
  canvas_rect(canvas, (Rect){0, 0, w, h}, 0xff080b14);
  blit_page(canvas, scene, 0, (Rect){0, 0, 256, 256}, (Rect){0, 0, w, h});
  centered(font, canvas, h * 3 / 100, menu->screen == MENU_COMPLETE ? "THE TOWER IS CONQUERED" : "CHOOSE YOUR DESTINY", 1.5f);
  int base = h * 83 / 100;
  int pitch = h * 7 / 100;
  int size = pitch - 4;
  int column_width = w * 16 / 100;
  for (int col = 0; col < MENU_TOWERS; ++col) {
    const MenuTower *tower = &menu->towers[col];
    int cx = w * (10 + col * 20) / 100;
    bool selected = col == menu->tower;
    for (int floor = 0; floor < tower->count; ++floor) {
      int y = base - (floor + 1) * pitch;
      int side = pitch / 2;
      int front = pitch + 4;
      Rect block = {cx - front / 2, y, front, pitch - 2};
      float right = (float)(block.x + block.w);
      float top = (float)y;
      float bottom = (float)(y + block.h);
      float depth = (float)side;
      float corners[4][2] = {{right, top}, {right + depth, top - depth / 3}, {right + depth, bottom - depth / 3}, {right, bottom}};
      canvas_quad(canvas, &scene->textures.data[1], (Rect){1, 1, 126, 78}, corners, selected ? 0xffa0a0a0 : 0xff686878);
      blit_page(canvas, scene, 1, (Rect){1, 1, 126, 78}, block);
      tower_portrait(canvas, scene, menu_tower_opponent(menu, col, floor), (Rect){cx - size / 2, y + 1, size, size});
      if (selected && menu->climbing && floor < menu->floor) {
        select_shade(canvas, block, 0xb0000000);
        font_draw(font, canvas, cx - font_width(font, "WIN", .65f) / 2, y + size / 3, "WIN", .65f);
      }
      if (selected && floor == menu->floor) {
        unsigned red = (unsigned)(200 + 55 * (.5 + .5 * sin(seconds * 6)));
        for (int ring = 0; ring < 3; ++ring) {
          canvas_outline(canvas, (Rect){block.x - ring, block.y - ring, block.w + ring * 2, block.h + ring * 2}, 0xff000000 | (red << 16) | 0x3020);
        }
      }
    }
    uint32_t color = selected ? 0xffb02010 : 0xff303038;
    canvas_rect(canvas, (Rect){cx - column_width / 2, base, column_width, h / 25}, color);
    font_draw(font, canvas, cx - font_width(font, tower->name, .75f) / 2, base + 6, tower->name, .75f);
  }
  char status[128];
  const MenuTower *tower = &menu->towers[menu->tower];
  if (menu->screen == MENU_COMPLETE) {
    snprintf(status, sizeof status, "%s WINS - %s", roster_def(menu->p1)->name, tower->name);
  } else if (menu->screen == MENU_CONTINUE) {
    snprintf(status, sizeof status, "CONTINUE?  ENTER TO RETRY   ESC TO END RUN");
  } else if (menu->climbing) {
    snprintf(status, sizeof status, "FIGHT %d / %d - %s   GET READY", menu->floor + 1, tower->count,
        roster_def(menu_tower_opponent(menu, menu->tower, menu->floor))->name);
  } else {
    snprintf(status, sizeof status, "LEFT / RIGHT TO CHOOSE   ENTER TO FIGHT   ESC TO GO BACK");
  }
  centered(font, canvas, h * 92 / 100, status, .8f);
  if (menu->screen == MENU_COMPLETE) {
    centered(font, canvas, h * 97 / 100, "ENTER TO RETURN TO MENU", .65f);
  }
}

void menu_draw(Canvas *canvas, const Assets *assets, const FrontendMenu *menu, double seconds) {
  // Resets depth for the fighter previews; each screen repaints every pixel.
  canvas_clear(canvas);
  if (menu->screen == MENU_MODE) {
    draw_mode(canvas, assets, menu);
  } else if (menu->screen == MENU_SELECT) {
    draw_select(canvas, assets, menu, seconds);
  } else if (menu->screen == MENU_ARENA) {
    draw_arena(canvas, assets, menu, seconds);
  } else {
    draw_tower(canvas, assets, menu, seconds);
  }
}
