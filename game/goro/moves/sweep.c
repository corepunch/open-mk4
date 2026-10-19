#include "../character.h"

/* 46ef90 -> 4e6798 slot 15 (4e6e68): clip 63 then shared 46f080 B[35]. */
void goro_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &goro_s_sweep);
  goro_entry_retail_script_004e6798(fighter);
}
