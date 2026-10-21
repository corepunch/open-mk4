#include "../character.h"

/* Requested portable extension: Goro's retail BLOCK list (53c99c) is empty.
 * The shared 46aa80 pair, generated 4e5e28/4e5e50 owner and 4e9910 victim
 * streams run on frame tables. */

/* The extension lives on the jump stance because the generated retail
 * command list keeps Goro's BLOCK records exactly as typed (none). */
void goro_press_air_block(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    fighter_interrupt(fighter, retail_air_throw);
  }
}

/* 46aae0 releases the pair with the shared damage, then enters 4e5e50. */

/* 46ab20: the owner lands through the common landing controller. */

/* 484b60: the released victim gets up with clip 131 samples 46..79. */
