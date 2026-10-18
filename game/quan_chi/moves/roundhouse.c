#include "../character.h"

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [7] = {{.clip = 313, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const NativeGroundProfile profile = {.attacks = attacks, .ready = fighter_ready, .fast = 5, .round_pre = 21, .round_first = 22, .round_last = 38};

void quan_chi_roundhouse(Fighter *fighter) {
  bool fast = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &attacks[7].move);
  fighter->script_phase = 7;
  fighter->native_followup = fast;
  fighter_start_fiber(fighter, quan_chi_entry_retail_script_004e6800, NULL);
}

void quan_chi_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    quan_chi_entry_retail_script_004e6860(fighter);
  } else {
    quan_chi_entry_retail_script_004e6878(fighter);
  }
}

void quan_chi_script_roundhouse_finish(Fighter *fighter) {
  fighter->script_phase = 7;
  native_ground_tail(fighter, &profile);
}
