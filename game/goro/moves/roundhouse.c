#include "../character.h"

/* 46f1f0 binds the shared 313 roundhouse for Goro's slot (4e6840); 46f220
 * selects fast 4e6860 or normal 4e6878, and 46f270 queries B[33]. */
// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack attacks[8] = {
  [7] = {{.clip = 313, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const NativeGroundProfile profile = {.attacks = attacks, .ready = fighter_ready, .fast = 5, .round_pre = 21, .round_first = 22, .round_last = 38};

void goro_roundhouse(Fighter *fighter) {
  bool fast = fighter_fast_start(fighter);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &attacks[7].move);
  fighter->script_phase = 7;
  fighter->native_followup = fast;
  goro_entry_retail_script_004e6800(fighter);
}

void goro_script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    goro_entry_retail_script_004e6860(fighter);
  } else {
    goro_entry_retail_script_004e6878(fighter);
  }
}

void goro_script_roundhouse_finish(Fighter *fighter) {
  fighter->script_phase = 7;
  native_ground_tail(fighter, &profile);
}
