#include "../character.h"

/* 469a00..469ad0 store a roster index in 53c42c and enter 469ae0.
 * State 0 writes it to task +0x34, then 4aa7e0 parks the actor for four
 * ticks. Its "LOADING" text (4ce220) and object freeze (53dbc0) cover
 * the retail sound/model reload; decoded assets stay resident here, so
 * only the actor's four-tick hold remains. State 1 queues the target's
 * name token, stamps slot 5 (48e3a0) and returns through 470420. */
static const MoveDef morph_hold = {.clip = CLIP_ASSET_IDLE, .last = MOVE_END, .flags = SF_LOCK};

bool shinnok_can_morph(const Fighter *fighter) {
  /* 45e6a0 reads only the round winner; the command interface passes the actor. */
  (void)fighter;
  return g_battle.ko_winner == 0;
}

static void morph_complete(Fighter *fighter) {
  /* 469ae0 plays its inline token for roster < 16. For every morph target
   * (0..14) it equals indexed cue 4ead30[38 + roster]. */
  battle_sound(SOUND_INDEXED, 38 + fighter->roster);
  fighter->morph_tick = g_battle.ticks;
  fighter_finish_mmove(fighter, fighter_ready);
}

static void morph(Fighter *fighter, int roster) {
  fighter_change_character(fighter, roster);
  /* A locked ground move, like the sidestep, so the controller return
   * re-enters the target's own standing state. */
  fighter_start_move(fighter, S_WALK, &morph_hold);
  native_hold(fighter, 4, morph_complete);
}

void shinnok_morph_scorpion(Fighter *fighter) {
  morph(fighter, 0);
}

void shinnok_morph_raiden(Fighter *fighter) {
  morph(fighter, 1);
}

void shinnok_morph_sonya(Fighter *fighter) {
  morph(fighter, 2);
}

void shinnok_morph_liu_kang(Fighter *fighter) {
  morph(fighter, 3);
}

void shinnok_morph_sub_zero(Fighter *fighter) {
  morph(fighter, 4);
}

void shinnok_morph_fujin(Fighter *fighter) {
  morph(fighter, 5);
}

void shinnok_morph_reiko(Fighter *fighter) {
  morph(fighter, 7);
}

void shinnok_morph_quan_chi(Fighter *fighter) {
  morph(fighter, 8);
}

void shinnok_morph_tanya(Fighter *fighter) {
  morph(fighter, 9);
}

void shinnok_morph_reptile(Fighter *fighter) {
  morph(fighter, 10);
}

void shinnok_morph_kai(Fighter *fighter) {
  morph(fighter, 11);
}

void shinnok_morph_jarek(Fighter *fighter) {
  morph(fighter, 12);
}

void shinnok_morph_jax(Fighter *fighter) {
  morph(fighter, 13);
}

void shinnok_morph_cage(Fighter *fighter) {
  morph(fighter, 14);
}
