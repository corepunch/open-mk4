#include "retail_vm.h"
#include "../character.h"
#include "math_util.h"

static void quake_weight(Fighter *fighter, float weight);
static void quake_jump(Fighter *fighter);
static void quake_cleanup(Fighter *fighter);
static const MoveDef s_quake = {.clip = 602, .last = 29, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};

enum {
  QUAKE_BLEND_STEP = 0x3333,
  QUAKE_DAMAGE = 0x3d70,
  QUAKE_REACTION = 136
};

static void quake_weight(Fighter *fighter, float weight) {
  fighter->blend_weight = (q16)weight;
}

static void quake_cleanup(Fighter *fighter) {
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
}

static void quake_jump(Fighter *fighter) {
  quake_cleanup(fighter);
  retail_vm_enter_owned(fighter, 0x4dfc50, fighter_ready, 15);
}

/* 43b7e0 (HK B,F,D,D): voice 0x604, then 48fde0 blends from the current
 * pose to clip 602 (descriptor 4fc9b4) sample 0 in 0x3333 steps before
 * 4dfc50 plays the jump to its landing pose 21. */
void goro_quake_stomp(Fighter *fighter) {
  int source = fighter_move(fighter)->clip, sample = fighter_sample(fighter);
  goro_begin_special(fighter, S_KICK, &s_quake, 0x604);
  fighter->mmove_cleanup = quake_cleanup;
  mk4_mmove_bind(fighter, 602, 30, 0);
  fighter->blend_clip = source;
  fighter->blend_sample = sample;
  fighter->blend_duration = 1;
  fighter->blend_weight = 0;
  int frames = (Q16_ONE + QUAKE_BLEND_STEP - 1) / QUAKE_BLEND_STEP;
  native_begin(fighter, 1, frames, quake_jump);
  for (int frame = 1; frame <= frames; ++frame) {
    fighter->native.frames[frame - 1] = (mframe_t){quake_weight, (float)mini(Q16_ONE, frame * QUAKE_BLEND_STEP), NULL};
  }
  native_play(fighter, false);
}

/* 43b8c0: 48ee60 skips an airborne victim; otherwise the landing deals
 * 24% and installs reaction 136 on the victim without a contact query.
 * Either way 43b920 disturbs resting Goro objects and enters 4dfc60. */
void goro_script_quake_land(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  if (!fighter_airborne(victim)) {
    battle_damage(victim, QUAKE_DAMAGE);
    battle_react(victim, QUAKE_REACTION);
  }
  goro_quake_resting_weapons();
  retail_vm_enter_owned(fighter, 0x4dfc60, fighter_ready, 15);
}
