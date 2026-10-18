#include "../character.h"

static const AttackRecord jarek_sweep_record = {95682, 31391, 68812, 35389, 18, 18, 4, ATTACK_LOW, 9175};

void jarek_sweep(Fighter *fighter) {
  jarek_start(fighter, S_SWEEP, &jarek_s_sweep, jarek_entry_retail_script_004e6798);
}

void jarek_script_sweep_finish(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &jarek_sweep_record);
  }
  fighter_fiber_finish(jarek_ready);
}

void jarek_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_sweep);
}
