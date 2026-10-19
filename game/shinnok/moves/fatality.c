#include "../character.h"

/* Spike-fatality contact 0x4ebd80 uses reaction 175 with zero damage. The
 * stage coordinator owns the victim pose and recovery for that scene. */
void shinnok_fatality_contact_hold(Fighter *fighter) {
  require(g_battle.phase == PHASE_FATALITY, "Shinnok reaction 175 outside fatality");
  fighter->pending = NULL;
  fighter->flags |= FF_REACTING | FF_BUSY;
}

/* 45e570: both fatality records accept distance <= 1 and 45e5e0's
 * finisher window. */
bool shinnok_can_close_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

// clang-format off: Keep table entries on one row, including long rows.
/* 475430 with descriptor 4fc240 (clip 443) and step 0xccc. */
static const SceneClip victory_blend[] = {
  {{.clip = 443, .last = 29, .flags = SF_LOCK | SF_RECORD}, 0, 0, 1, 0xccc, 0},
};
// clang-format on

static void vanish_tick(Fighter *fighter) {
  /* 455590 then emits 41 visits of owner-joint sparkles (4e3170); the
   * portable scene keeps only this cue. The track ends on this row. */
  (void)fighter;
  battle_sound(SOUND_B, 0x4c);
}

/* 4554d0: a child task sleeps five ticks, then queues B 0x4c. */
static void vanish_cue(Fighter *fighter) {
  native_motion_begin(fighter, 5, NULL);
  native_motion_frame(fighter, 4, NULL, 0, vanish_tick);
  native_motion_play(fighter);
}

/* 455000 state 1 and 4552e0: B 0x5c and the 4554d0 child as clip 12 starts. */
void shinnok_fatality_vanish_cue(Fighter *fighter) {
  battle_sound(SOUND_B, 0x5c);
  vanish_cue(fighter);
}

/* 455190: the child cue again, then clip 436 to sample 18 (4e2f10). */
void shinnok_fatality_return(Fighter *fighter) {
  vanish_cue(fighter);
  shinnok_entry_retail_script_004e2f10(fighter);
}

static void reappear_land(Fighter *fighter) {
  /* 455370 clears gravity and restores the floor Y; 455460 then cues B 0x47. */
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  battle_sound(SOUND_B, 0x47);
}

/* 4551b0: cursor 19, clear hidden bit 4, then an independent leap with
 * gravity 0x28f and Y speed -0x2666 while 4e2f20 finishes clip 436. */
void shinnok_fatality_reappear(Fighter *fighter) {
  fighter->sample_override = 19;
  fighter->appearance = APPEARANCE_NORMAL;
  fighter->body.vel[AXIS_Y] = -0x2666;
  fighter->body.gravity = 0x28f;
  fighter->body.on_land = reappear_land;
  shinnok_entry_retail_script_004e2f20(fighter);
}

static void victory_loop(Fighter *fighter) {
  fighter->state = S_VICTORY;
  fighter->native_variant |= SHINNOK_FATALITY_OWNER_READY;
  shinnok_entry_retail_script_004e2f30(fighter);
  shinnok_fatality_maybe_complete(fighter);
}

void shinnok_fatality_victory(Fighter *fighter) {
  native_scene_blends(fighter, victory_blend, 1, victory_loop);
}

/* The retail scene leaves its final bodies posed; the portable finisher
 * completes once the owner loops 443 and every scene body has finished. */
void shinnok_fatality_maybe_complete(Fighter *owner) {
  if ((owner->native_variant & SHINNOK_FATALITY_DONE) == SHINNOK_FATALITY_DONE && g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* Both fatalities start from a locked standing owner (stance clip 283,
 * 53d408[6]) and the dizzy victim. */
void shinnok_fatality_begin(Fighter *fighter, unsigned done) {
  static const MoveDef owner_pose = {.clip = 283, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x700};
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_move(fighter, S_PUNCH, &owner_pose);
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  fighter->native_variant = (int)(SHINNOK_FATALITY_DONE & ~done);
  mk4_script_scene_pose(victim, 13, 56, 0);
}
