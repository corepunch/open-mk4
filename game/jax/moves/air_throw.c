#include "../character.h"

/* Jax's BLOCK air record 4e43d4 (flags 0x11, gate 45ed70) enters 46a990,
 * not the shared 46aa80 runner. It sets action 0x502, prepares the victim
 * through 480fc0[8], cancels both jump rotations through 48f9f0 and enters
 * 4e5dd8, which opens owner clip 84 and victim clip 144. */
static const MoveDef s_air_grab = {.clip = 84, .last = 71, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0502};
static const MoveDef s_air_grabbed = {.clip = 144, .last = 43, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4003};

static void air_grab_getup_frame(Fighter *victim);
static void air_grab_getup_end(Fighter *victim);

/* 480f50[166] -> 484a10 clears the paired bit and enters 4e98e8. */
static void air_grab_victim(Fighter *victim) {
  victim->flags &= ~(unsigned)FF_PAIRED;
  victim->flags |= FF_REACTING;
  victim->reaction = 166;
  jax_entry_retail_script_004e98e8(victim);
}

void jax_air_throw(Fighter *fighter) {
  mk4_air_throw_frames_with_moves(fighter, &s_air_grab, &s_air_grabbed, jax_entry_retail_script_004e5dd8, air_grab_victim);
}

/* 46a9f0 stores gravity 0x3d7 on both paired actors and enters 4e5df8. */
void jax_air_throw_fall(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->body.gravity = 0x3d7;
  victim->body.gravity = 0x3d7;
  jax_entry_retail_script_004e5df8(fighter);
}

/* 46aa40 dispatches reaction 166 to the victim, subtracts 0x2147 and enters
 * the owner release 4e5e18. The released victim owns its fall and getup. */
void jax_air_throw_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->flags |= FF_PAIR_RELEASE;
  air_grab_victim(victim);
  battle_damage(victim, 0x2147);
  jax_entry_retail_script_004e5e18(fighter);
}

/* 484a30 first reschedules once. Each later visit lets a living victim's
 * held Up dispatch the straight jump through 4711f0(2) unless the round is
 * already decided (532354), then 4285d0 advances one stored pose. The final
 * pose of clip 140 returns through the controller; a KO victim stays down. */
static void air_grab_getup_frame(Fighter *victim) {
  if (g_battle.phase == PHASE_FIGHT && victim->health > 0 && (victim->held & BUTTON_BIT(BUTTON_UP))) {
    mk4_script_stop_body(victim);
    fighter_finish_mmove(victim, mk4_jump);
  }
}

static void air_grab_getup_end(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_finish_mmove(victim, victim->health > 0 ? fighter_ready : NULL);
}

void jax_air_throw_getup(Fighter *victim) {
  int first = fighter_sample(victim), last = fighter_move(victim)->last;
  require(last != MOVE_END && first <= last, "Jax air-grab getup needs the bounded clip-140 pose");
  native_begin(victim, first, last, air_grab_getup_end);
  native_frame(victim, first, first, NULL);
  for (int frame = first + 1; frame <= last; ++frame) {
    native_frame(victim, frame, frame, air_grab_getup_frame);
  }
  native_play(victim, false);
}
