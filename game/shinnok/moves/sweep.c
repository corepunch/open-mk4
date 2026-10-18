#include "../character.h"

static const AttackRecord shinnok_sweep_record = {95682, 31391, 68812, 35389, 18, 18, 4, ATTACK_LOW, 9175};

void shinnok_sweep(Fighter *fighter) {
  shinnok_start(fighter, S_SWEEP, &shinnok_s_sweep, shinnok_entry_retail_script_004e6798);
}

void shinnok_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &shinnok_sweep_record);
  }
  fighter_fiber_finish(shinnok_ready);
}

void shinnok_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, shinnok_sweep);
}
