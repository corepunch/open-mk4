#include "../character.h"

static const MoveDef s_cartwheel = {.clip = 104, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x213};
static const AttackRecord cartwheel_record = {75300, -45088, 72083, 89063, 121, 121, 9, 0, 4587};

void sonya_script_cartwheel_tail(Fighter *fighter) {
  fighter->cartwheel_ready = g_battle.ticks + 48;
  native_queries(fighter, &cartwheel_record, 9, 1);
  fighter->action = ACTION_RETAIL_1006;
  mk4_script_finish_clip(fighter, 1);
  fighter_set_speed(fighter, 0);
  if (fighter->connected) {
    native_delay(fighter, 20);
  }
  fighter_fiber_finish(sonya_ready);
}

void sonya_cartwheel(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_KICK, &s_cartwheel);
  fighter_start_fiber(fighter, sonya_entry_retail_script_004e81d0, sonya_special_cleanup);
}

bool sonya_can_cartwheel(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->cartwheel_ready;
}
