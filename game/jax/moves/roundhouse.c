#include "../character.h"

void jax_roundhouse(Fighter *fighter);
static const MoveDef s_roundhouse = {.clip = 64, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010D};
static const AttackRecord s_roundhouse_record = {0x10f5c, -0xab85, 0xf0a3, 0x6b85, 15, 15, 5, 0, 0x2e14};
static const GroundAttack jax_ground_attacks[8] = {
    [7] = {.active = 14, .end_miss = 39, .end_contact = 67},
};
static const NativeGroundProfile jax_ground_profile = {.attacks = jax_ground_attacks,
    .ready = jax_ready,
    .fast = 4,
    .round_pre = 19,
    .round_first = 20,
    .round_last = 44,
    .roundhouse_record = &s_roundhouse_record};

void jax_roundhouse(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_roundhouse);
  jax_entry_retail_script_004e6800(fighter);
}

void jax_script_roundhouse_start(Fighter *fighter) {
  fighter->native_followup = fighter_fast_start(fighter);
  if (fighter->native_followup) {
    jax_entry_retail_script_004e6890(fighter);
  } else {
    jax_entry_retail_script_004e68a8(fighter);
  }
}

void jax_script_roundhouse_tail(Fighter *fighter) {
  fighter->script_phase = 7;
  native_ground_tail(fighter, &jax_ground_profile);
}
