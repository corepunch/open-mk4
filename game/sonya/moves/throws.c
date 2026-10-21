#include "native_services.h"

static void lk_victim_land(Fighter *victim);

static void lk_victim_land(Fighter *victim) {
  native_await(victim, native_grounded, mk4_mmove_thrown_finish);
}

/* The victim advances seven poses and then completes clip 203 in one run. */
void sonya_lk_victim(Fighter *victim) {
  mk4_mmove_bind(victim, 203, 112, 75);
  native_finish_clip(victim, lk_victim_land);
}
