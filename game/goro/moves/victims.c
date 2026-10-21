#include "retail_vm.h"
#include "../character.h"

static void begin_victim(Fighter *victim);
static void stomp_victim_next(Fighter *victim);
static void stomp_victim_rest(Fighter *victim);
static const MoveDef s_victim = {.clip = 247, .last = MOVE_END, .flags = SF_LOCK | SF_REACTION, .action = ACTION_RETAIL_4001};

/* 488ba0 and 482790 select 4ea210: standing 328 or held-DOWN crouching 184. */

enum {
  STOMP_VICTIM_CLIP = 178,
  STOMP_VICTIM_COUNT = 28,
  STOMP_VICTIM_LOOP = 15,
  STOMP_VICTIM_CYCLES = 5, /* 484330 plus 484420 states 0..3 */
  STOMP_VICTIM_REST = 20   /* 4845f0 parks the task for 0x14 visits */
};

/* Shared reaction table entries own a non-Goro victim; the generated
 * victim streams bind their own clips from a locked reaction move. */
static void begin_victim(Fighter *victim) {
  mk4_script_stop_body(victim);
  fighter_start_move(victim, S_KNOCKDOWN, &s_victim);
  victim->flags |= FF_REACTING;
}

/* 189 (482420): wall recoil 488ea0, disarm 494520, then 4e8dd0. */
void goro_punch_victim(Fighter *victim) {
  begin_victim(victim);
  battle_wall_recoil(victim);
  mk4_script_disarm_self(victim);
  retail_vm_enter_owned(victim, 0x4e8dd0, fighter_ready, 15);
}

/* 190 (483570): disarm 494520, then 4ea3e0 -> shared 4e9670 flight. */
void goro_kick_victim(Fighter *victim) {
  begin_victim(victim);
  mk4_script_disarm_self(victim);
  retail_vm_enter_owned(victim, 0x4ea3e0, fighter_ready, 15);
}

/* 192 (482480 -> 4824a0): shake 6; 482a50 sets later damage to 40%;
 * 488ea0 wall recoil; disarm; 488fd0 drains the attacker's run meter
 * (48fae0: -1.0, 60-visit delay); then 4e8e50. */
void goro_uppercut_victim(Fighter *victim) {
  Fighter *attacker = (Fighter *)fighter_opponent(victim);
  begin_victim(victim);
  mk4_script_shake(victim, 6);
  victim->damage_scale = 0x6666;
  battle_wall_recoil(victim);
  mk4_script_disarm_self(victim);
  attacker->run_energy = 0;
  attacker->run_delay = 60;
  retail_vm_enter_owned(victim, 0x4e8e50, fighter_ready, 15);
}

/* 4824f0: back speed 0x7ae, or 0xf5c after more than one hit, then 4e8e80. */
void goro_script_uppercut_victim_slide(Fighter *victim) {
  fighter_set_speed(victim, victim->hits > 1 ? -0xf5c : -0x7ae);
  retail_vm_enter_owned(victim, 0x4e8e80, fighter_ready, 15);
}

/* 136 (4834d0): disarm, 48a320 -> 489e80 picks one of list 4ea8f8's three
 * sampled tokens at random, then 4e95c8. */
void goro_quake_victim(Fighter *victim) {
  static const int voices[] = {0x78, 0x7d, 0x82};
  begin_victim(victim);
  mk4_script_disarm_self(victim);
  battle_sound(SOUND_TOKEN, voices[battle_random(3)]);
  retail_vm_enter_owned(victim, 0x4e95c8, fighter_ready, 15);
}

/* 191 (4842d0): disarm; an airborne victim falls with 0x1999 velocity and
 * gravity before 4ea400 plays clip 178. */
void goro_stomp_victim(Fighter *victim) {
  begin_victim(victim);
  mk4_script_disarm_self(victim);
  if (fighter_airborne(victim)) {
    victim->body.vel[AXIS_Y] = 0x1999;
    victim->body.gravity = 0x1999;
  }
  retail_vm_enter_owned(victim, 0x4ea400, fighter_ready, 15);
}

/* 484650: each cycle shakes the camera, cues effect 13 and replays clip
 * 178 from pose 15. After a cycle it continues only while the stomper still
 * has action 0409 or 0400, at most five cycles, then parks and gets up. */
static void stomp_victim_next(Fighter *victim) {
  int action = fighter_opponent(victim)->action;
  if (action != 0x409 && action != 0x400) {
    stomp_victim_rest(victim);
    return;
  }
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
}

static void stomp_victim_getup(Fighter *victim) {
  mk4_mmove_getup(victim, 0x4ea210);
}

static void stomp_victim_rest(Fighter *victim) {
  native_hold(victim, STOMP_VICTIM_REST, stomp_victim_getup);
}

void goro_script_stomp_victim_hold(Fighter *victim) {
  int span = STOMP_VICTIM_COUNT - STOMP_VICTIM_LOOP;
  mk4_mmove_bind(victim, STOMP_VICTIM_CLIP, STOMP_VICTIM_COUNT, STOMP_VICTIM_LOOP);
  native_begin(victim, 0, span * STOMP_VICTIM_CYCLES - 1, stomp_victim_rest);
  for (int cycle = 0; cycle < STOMP_VICTIM_CYCLES; ++cycle) {
    for (int pose = 0; pose < span; ++pose) {
      FighterFn action = pose == 0 && cycle > 0 ? stomp_victim_next : NULL;
      native_frame(victim, cycle * span + pose, STOMP_VICTIM_LOOP + pose, action);
    }
  }
  mk4_script_shake(victim, 0);
  mk4_script_effect(victim, 13);
  native_play(victim, false);
}

/* 482790 finishes the bound clip before 488ba0's getup. */
void goro_victim_finish_getup(Fighter *victim) {
  mk4_mmove_finish_getup(victim, 0x4ea210);
}

/* 482540: 433c00 clears the CPU reaction latch, then 488ba0. */
void goro_victim_getup(Fighter *victim) {
  mk4_mmove_getup(victim, 0x4ea210);
}
