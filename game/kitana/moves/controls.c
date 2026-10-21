#include "../character.h"

void kitana_command_shadow_low(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_low);
}

void kitana_command_shadow_up(Fighter *fighter) {
  fighter_interrupt(fighter, kitana_shadow_up);
}
