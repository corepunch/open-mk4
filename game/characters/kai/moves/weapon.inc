static void kai_draw_weapon(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
}

static void kai_script_draw_equip(Fighter *fighter) {
  /* 46d260's actor-local continuation is exactly the generated 4e62d8 body.
   * Its global task queue bookkeeping is owned by the portable fiber scheduler. */
  retail_script_004e62d8(fighter);
}
