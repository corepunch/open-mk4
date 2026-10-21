#include "../character.h"

/* 438e50/60/70/80 select 4df8e8/928/968/9a8[3]:
 * bicycle, low fireball, flying kick, low fireball. */
void liu_kang_cpu_close_hp(Fighter *fighter) {
  retail_close_high_punch(fighter);
}
