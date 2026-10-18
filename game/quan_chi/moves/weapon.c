#include "../character.h"

static void quan_chi_weapon_draw(Fighter *fighter);

static void quan_chi_weapon_draw(Fighter *fighter) {
  fighter_enter_state(fighter, S_PUNCH);
}
