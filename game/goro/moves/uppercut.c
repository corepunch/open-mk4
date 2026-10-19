#include "../character.h"

/* 46ccd0 -> 4e62a0; the generated startup enters shared 46cde0
 * (native_uppercut_default: A[2]/A[6] and hit/block/miss recovery). */
void goro_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &goro_s_uppercut);
  goro_entry_retail_script_004e62a0(fighter);
}
