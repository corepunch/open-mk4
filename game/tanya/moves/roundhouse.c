#include "../character.h"

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [7] = {{.clip = 64, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 15, 15, 5, 0, 11796}, 14, 39, 67},
};
// clang-format on
static const NativeGroundProfile profile = {.attacks = attacks, .ready = fighter_ready, .fast = 4, .round_pre = 19, .round_first = 20, .round_last = 44};

void tanya_roundhouse(Fighter *fighter) {
  bool fast = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &attacks[7].move);
  fighter->script_phase = 7;
  fighter->native_followup = fast;
  fighter_start_fiber(fighter, tanya_entry_retail_script_004e6800, NULL);
}

void tanya_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    tanya_entry_retail_script_004e6890(fighter);
  } else {
    tanya_entry_retail_script_004e68a8(fighter);
  }
}

void tanya_script_roundhouse_finish(Fighter *fighter) {
  fighter->script_phase = 7;
  native_ground_tail(fighter, &profile);
}
