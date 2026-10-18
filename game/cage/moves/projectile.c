#include "../character.h"

static void cage_cpu_projectile(Fighter *fighter);

static void cage_cpu_projectile(Fighter *fighter) {
  cage_sweep(fighter);
}
