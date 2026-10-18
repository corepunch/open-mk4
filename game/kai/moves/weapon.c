#include "../character.h"

static void kai_draw_weapon(Fighter *fighter);

static void kai_draw_weapon(Fighter *fighter) {
  fighter_start_fiber(fighter, kai_entry_retail_script_004e62c0, NULL);
}

void kai_script_draw_equip(Fighter *fighter) {
  /* 46d260's actor-local continuation is exactly the generated 4e62d8 body.
   * Its global task queue bookkeeping is owned by the portable fiber scheduler. */
  kai_entry_retail_script_004e62d8(fighter);
}
