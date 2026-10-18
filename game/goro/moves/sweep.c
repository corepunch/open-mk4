#include "../character.h"

static void goro_event_counter_sweep(Fighter *fighter);
static void goro_sweep(Fighter *fighter);

static void goro_event_counter_sweep(Fighter *fighter) {
  /* Native 43b320 writes 0x0409 to the selected fighter before script entry. */
  goro_start_actor_event(fighter, goro_entry_retail_script_004dfbb8, 601, 52, 0x0409);
}

static void goro_sweep(Fighter *fighter) {
  goro_start_script(fighter, S_SWEEP, &goro_s_sweep, goro_entry_retail_script_004e6798);
}

void goro_enter_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, goro_sweep);
}
