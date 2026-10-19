#include "../character.h"

static const MoveDef s_air_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const mframe_t air_getup_frames[34] = {{0}};
static const mmove_t air_getup = {.firstframe = 46, .lastframe = 79, .frame = air_getup_frames, .endfunc = fighter_ready};

void goro_script_lp_pair_start(Fighter *fighter) {
  /* Native 42b680 acquires and aligns the captured peer; the shared frame
   * entry already owns that setup and its interruption cleanup. */
  require((fighter->flags & FF_PAIRED) && (fighter_opponent(fighter)->flags & FF_PAIRED), "Goro LP needs its captured peer");
}

void goro_lk_throw(Fighter *fighter) {
  mk4_mmove_throw_start(fighter, &goro_s_throw, &goro_s_knockdown, goro_entry_retail_script_004de2f8);
}

void goro_lp_throw(Fighter *fighter) {
  if (!mk4_can_ground_throw(fighter, false)) {
    fighter_ready(fighter);
    return;
  }
  fighter->script_phase = 0;
  mk4_mmove_throw_start(fighter, &goro_s_throw, &goro_s_knockdown, goro_entry_retail_script_004de798);
}

/* Requested portable extension: Goro's retail BLOCK list (53c99c) is empty.
 * The shared 46aa80 pair, generated 4e5e28/4e5e50 owner and 4e9910 victim
 * streams run on frame tables. */
void goro_air_throw(Fighter *fighter) {
  mk4_air_throw_frames(fighter, goro_entry_retail_script_004e5e28, goro_entry_retail_script_004e9910);
}

/* The extension lives on the jump stance because the generated retail
 * command list keeps Goro's BLOCK records exactly as typed (none). */
void goro_press_air_block(Fighter *fighter) {
  if (mk4_can_air_throw(fighter)) {
    fighter_interrupt(fighter, goro_air_throw);
  }
}

/* 46aae0 releases the pair with the shared damage, then enters 4e5e50. */
void goro_script_air_release(Fighter *fighter) {
  mk4_mmove_air_throw_release(fighter);
  goro_entry_retail_script_004e5e50(fighter);
}

/* 46ab20: the owner lands through the common landing controller. */
void goro_script_air_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

/* 484b60: the released victim gets up with clip 131 samples 46..79. */
void goro_script_air_getup(Fighter *victim) {
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_air_getup, &air_getup);
}
