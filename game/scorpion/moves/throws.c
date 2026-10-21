#include "../character.h"

static void thrown_end_done(Fighter *victim);

static void thrown_end_done(Fighter *victim) {
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void scorpion_thrown_end_script(Fighter *victim) {
  native_finish_clip(victim, thrown_end_done);
}
