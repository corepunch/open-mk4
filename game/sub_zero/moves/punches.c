#include "../character.h"

/* Shared CPU selections use these native entries directly, as 45f5f0 does.
 * Human command patterns and held-button predicates do not gate drone actions. */
void sub_zero_cpu_close_hp(Fighter *fighter) {
  retail_close_high_punch(fighter);
}
