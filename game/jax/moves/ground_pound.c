#include "native_services.h"

/* LK F,F,D record 4e43a8 (no gate) and the medium CPU row 4df95c select the
 * phase machine 468f00. Phase 0 stores action 0x101, binds 53ce14 (clip
 * 108, 57 poses) through 428750 and plays to pose 15 (429230); phase 1
 * spawns effect task 0x8b and plays to 35 (428d70); phase 2 spawns task
 * 0x8a, queues C[0x50], starts shake 3 and, unless 48ee60 finds the
 * opponent airborne, subtracts 0x23d7 and dispatches reaction 136 before
 * playing to 38. Phases 3..5 alternate poses 37/38 for ten cycles, 428d10
 * finishes the clip and 46f650 returns to the controller. */
static const MoveDef s_pound = {.clip = 108, .last = 56, .flags = SF_LOCK | SF_RECORD, .action = 0x101};

enum {
  POUND_RAISE = 15,
  POUND_IMPACT = 35,
  POUND_SETTLE = 38,
  POUND_SHAKE_CYCLES = 10
};

static void pound_ready(Fighter *fighter) {
  fighter_finish_mmove(fighter, fighter_ready);
}

static void pound_impact(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  battle_sound(SOUND_C, 0x50);
  mk4_script_shake(fighter, 3);
  if (!fighter_airborne(victim)) {
    battle_damage(victim, 0x23d7);
    /* 4ea4e8[136] -> 4834d0 is roster-generic: the shared reaction table
     * runs Goro's port of it (disarm, 4ea8f8 voice, victim script 4e95c8). */
    battle_react(victim, 136);
  }
}

void jax_ground_pound(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_pound);
  fighter->sample_override = 0;
  int shake = 2 * POUND_SHAKE_CYCLES;
  int last = POUND_SETTLE + shake + (s_pound.last - POUND_SETTLE) - 1;
  native_begin(fighter, 0, last, pound_ready);
  int frame = 0;
  for (int pose = 1; pose <= POUND_SETTLE; ++pose, ++frame) {
    native_frame(fighter, frame, pose, pose == POUND_IMPACT ? pound_impact : NULL);
  }
  for (int cycle = 0; cycle < shake; ++cycle, ++frame) {
    native_frame(fighter, frame, cycle % 2 ? POUND_SETTLE : POUND_SETTLE - 1, NULL);
  }
  for (int pose = POUND_SETTLE + 1; pose <= s_pound.last; ++pose, ++frame) {
    native_frame(fighter, frame, pose, NULL);
  }
  native_play(fighter, false);
}
