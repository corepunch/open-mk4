#include "menu.h"
#include "game.h"
#include "draw.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

int main(void) {
  Assets assets = {0};
  assets_load(&assets, "MK4/FILESYS.DAT", "MK4/MK-4.EXE");
  assets_load_battle(&assets, "MK4/FILESYS.DAT", "MK4/MK-4.EXE");
  FrontendMenu menu;
  bool quit = false;
  menu_init(&menu, &assets.retail_image);
  require(!menu_command(&menu, MENU_CONFIRM, &quit) && menu.screen == MENU_SELECT, "Arcade must select a fighter first");
  require(!menu_command(&menu, MENU_CONFIRM, &quit) && menu.screen == MENU_TOWER, "Arcade must choose a tower instead of an arena");
  Canvas canvas;
  canvas_init(&canvas, 1280, 800);
  menu_draw(&canvas, &assets, &menu, 0);
  save_bmp(&canvas, "build/tower.bmp");
  require(!menu_command(&menu, MENU_LEFT, &quit) && menu.tower == MENU_TOWERS - 1, "Tower selection wraps left");
  require(!menu_command(&menu, MENU_RIGHT, &quit) && menu.tower == 0, "Tower selection wraps right");
  for (int col = 0; col < MENU_TOWERS; ++col) {
    for (int face = 0; face < 4; ++face) {
      menu.screen = MENU_TOWER;
      menu.tower = col;
      menu.face = face;
      menu.floor = 0;
      menu.climbing = false;
      require(menu_command(&menu, MENU_CONFIRM, &quit), "Confirm starts the selected tower");
      int opponent = menu.p2;
      menu_match_finished(&menu, false);
      require(menu.screen == MENU_CONTINUE && menu.floor == 0, "Loss must retain progression");
      menu_draw(&canvas, &assets, &menu, 0);
      require(menu_command(&menu, MENU_CONFIRM, &quit) && menu.p2 == opponent, "Continue must retry the same opponent");
      for (int floor = 0; floor < menu.towers[col].count; ++floor) {
        uint32_t list = mk4_image_u32(&assets.retail_image, 0x4e557c + col * 16) * 4;
        int expected = (int)((mk4_image_u32(&assets.retail_image, list + floor * 4) >> (face * 8)) & 255);
        require(menu.p2 == expected, "Opponent must come from the selected byte of the retail list");
        menu_match_finished(&menu, true);
        require(menu.floor == floor + 1, "A match win advances exactly one floor");
        if (menu.screen != MENU_COMPLETE) {
          menu_command(&menu, MENU_RIGHT, &quit);
          require(menu.tower == col, "Progression must lock the chosen difficulty");
          require(menu_command(&menu, MENU_CONFIRM, &quit), "The next opponent must start");
        }
      }
      require(menu.screen == MENU_COMPLETE, "Exhausting the retail list must complete the run");
      menu_draw(&canvas, &assets, &menu, 0);
      menu_command(&menu, MENU_CONFIRM, &quit);
      require(menu.screen == MENU_MODE && !menu.climbing, "Completion returns to mode selection");
    }
  }
  /* Loading a changed list proves that the frontend has no copied ladder. */
  uint32_t list = mk4_image_u32(&assets.retail_image, 0x4e557c) * 4;
  assets.retail_image.bytes.data[list - 0x400000] = 12;
  menu_init(&menu, &assets.retail_image);
  require(menu_tower_opponent(&menu, 0, 0) == 12, "Frontend must consume the loaded retail list");
  menu.cursor = MENU_TEAM;
  menu_command(&menu, MENU_CONFIRM, &quit);
  menu_command(&menu, MENU_CONFIRM, &quit);
  require(menu.screen == MENU_SELECT && menu.selecting_player == 1, "Local play must select both fighters");
  menu_command(&menu, MENU_CONFIRM, &quit);
  require(menu.screen == MENU_ARENA && menu_command(&menu, MENU_CONFIRM, &quit), "Local play must retain arena selection");
  battle_init(0, 4);
  static Battle before;
  memcpy(&before, &g_battle, sizeof before);
  Canvas preview;
  canvas_init(&preview, 736, 316);
  for (int arena = 0; arena < MK4_ARENA_COUNT; ++arena) {
    draw_arena_preview(&preview, &assets.arenas.data[arena]);
    size_t visible = 0;
    for (int pixel = 0; pixel < preview.w * preview.h; ++pixel) {
      visible += isfinite(preview.depth[pixel]);
    }
    require(visible > (size_t)preview.w * preview.h / 8, "Each arena preview must contain visible loaded geometry");
    menu.arena = arena;
    menu_draw(&canvas, &assets, &menu, 0);
    char path[64];
    snprintf(path, sizeof path, "build/arena-menu-%02d.bmp", arena);
    save_bmp(&canvas, path);
  }
  require(memcmp(&before, &g_battle, sizeof before) == 0, "Arena previews must not reset or mutate the battle");
  canvas_free(&preview);
  canvas_free(&canvas);
  assets_free(&assets);
  puts("PASS: retail tower loading, all column/face progressions, retry, completion, and local-play routing");
  return 0;
}
