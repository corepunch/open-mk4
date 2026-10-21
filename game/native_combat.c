#include "native.h"
#include "retail_native.h"
#include "retail_vm.h"
#include "math_util.h"
#include <string.h>

enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK,
  G_ROUNDHOUSE
};

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

static void close_followup(Fighter *fighter) {
  fighter->buttons = NULL;
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
}

/* Only entry/branch construction evaluates frame numbers. Action callbacks
 * below query combat/input state; none advances a timer or tests a deadline. */
static int ground_pose(const NativeGroundProfile *profile, int index, int frame, int active, int fast, bool hit, int initial) {
  switch (index) {
    case G_CROUCH_LP:
    case G_CROUCH_LK: {
      int reverse = active + (hit ? 4 : 1);
      return frame < active ? mini(9, frame + (index == G_CROUCH_LP ? 2 : 1)) : maxi(0, 9 - maxi(0, frame - reverse));
    }
    case G_CLOSE_HP: {
      int reverse = hit ? 18 : 12;
      return frame < 10 ? frame : frame < reverse ? 9 : maxi(0, 8 - frame + reverse);
    }
    case G_COMBO_HP:
      return mini(profile->combo_last, frame + profile->combo_offset - (hit && frame > active ? mini(3, frame - active) : 0));
    case G_CLOSE_HK: {
      int reverse = hit ? 10 : 12;
      return frame < 8 ? frame + 5 : frame < reverse ? 12 : maxi(0, 12 - frame + reverse);
    }
    case G_ROUNDHOUSE:
      return frame <= 6       ? frame * 2
             : frame < active ? mini(profile->round_pre, 12 + (frame - 6) * (fast ? 2 : 1))
                              : mini(profile->round_last, profile->round_first + frame - active);
    default:
      return initial;
  }
}

static void ground_build(Fighter *fighter, int first, bool immediate);

static void ground_query(Fighter *fighter) {
  const NativeGroundProfile *profile = fighter->native_context;
  int index = fighter->script_phase;
  const Fighter *victim = fighter_opponent(fighter);
  bool gate = (index == G_CROUCH_LP || index == G_CROUCH_LK) && victim->action == ACTION_RETAIL_2011;
  if (index == G_CLOSE_HP) {
    gate = victim->action == ACTION_RETAIL_2001 || victim->action == ACTION_RETAIL_0106 || victim->action == ACTION_RETAIL_0107;
  }
  fighter->action = ACTION_RETAIL_1006;
  if (gate || fighter->connected ||
      battle_contact(fighter, index == G_ROUNDHOUSE && profile->roundhouse_record != NULL ? profile->roundhouse_record : &profile->attacks[index].record) ==
          CONTACT_MISS) {
    return;
  }
  if (index >= G_CLOSE_HP && index <= G_CLOSE_HK) {
    fighter->buttons = profile->followups[index - G_CLOSE_HP];
    fighter->flags = (fighter->flags | FF_FOLLOW_UP) & ~(unsigned)FF_BUSY;
  }
  ground_build(fighter, fighter->mmove_frame, true);
}

static void ground_build(Fighter *fighter, int first, bool immediate) {
  const NativeGroundProfile *profile = fighter->native_context;
  int index = fighter->script_phase;
  const GroundAttack *attack = &profile->attacks[index];
  int fast = index == G_ROUNDHOUSE && fighter->native_followup ? profile->fast : 0;
  int active = attack->active - fast;
  bool hit = fighter->connected;
  int end = (hit && !(index == G_ROUNDHOUSE && fighter->contact == CONTACT_BLOCK) ? attack->end_contact : attack->end_miss) - fast;
  int reverse = index == G_CLOSE_HP ? (hit ? 18 : 12) : index == G_CLOSE_HK ? (hit ? 10 : 12) : -1;
  int initial = fighter_sample(fighter);
  /* The compiled startup may hand off after the nominal active frame (for
   * example, combo HP includes an explicit boundary wait). The native runner
   * owns the contact probe: execute it on its first owned row rather than
   * dropping it because the actor clock already crossed that boundary. */
  int query_first = maxi(active, fighter->native_elapsed);
  native_begin(fighter, first, maxi(first, end), profile->ready);
  for (int frame = first; frame <= maxi(first, end); ++frame) {
    FighterFn action = frame >= query_first && frame <= query_first + (index == G_ROUNDHOUSE ? 2 : 0) ? ground_query : NULL;
    if (frame == reverse) {
      action = close_followup;
    }
    native_frame(fighter, frame, ground_pose(profile, index, frame, active, fast, hit, initial), action);
  }
  if (first > active) {
    fighter->action = ACTION_RETAIL_1006;
  }
  native_play(fighter, immediate);
}

static void ground_begin(Fighter *fighter) {
  ground_build(fighter, fighter->native_elapsed, true);
}

void native_ground_tail(Fighter *fighter, const NativeGroundProfile *profile) {
  fighter->native_context = profile;
  fighter->native_elapsed = fighter->ticks;
  ground_begin(fighter);
}

void native_ground_next(Fighter *fighter, const NativeGroundProfile *profile) {
  fighter->native_context = profile;
  ground_build(fighter, fighter->ticks + 1, false);
}

/* Native 46d290/46d9b3 binds these two descriptors directly; unlike the
 * authored move programs, its repeat/contact controller is native code. */
static void hp_bind(Fighter *fighter, bool second, int first) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_010A};
  fighter_start_move(fighter, S_PUNCH, &move);
  retail_vm_bind_animation(fighter, second ? 0x4fbb68 : 0x4fbb5c, first);
}

static void hp_build(Fighter *fighter);

static void hp_query(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  bool ducking_kick = victim->action == ACTION_RETAIL_0102 && victim->avoids_high;
  if (!fighter->connected && !ducking_kick) {
    const Mk4Image *image = retail_vm_image();
    uint32_t record = mk4_image_u32(image, 0x53d268);
    AttackRecord attack = retail_vm_attack_record(image, record * 4);
    battle_contact(fighter, &attack);
  }
}

static void hp_branch(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  int phase = fighter->script_phase;
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
  if (repeat) {
    battle_sound(SOUND_A, 10);
  }
  phase = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
  hp_bind(fighter, phase != 4, phase == 1 ? 1 : phase == 3 ? 2 : 11);
  fighter->script_phase = phase;
  fighter->flags &= ~(unsigned)FF_BUSY;
  hp_build(fighter);
}

static void hp_build(Fighter *fighter) {
  int phase = fighter->script_phase;
  int active = phase == 0 ? 8 : phase == 1 ? 7 : 6;
  int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
  int count = phase >= 4 ? fighter_move(fighter)->last - 11 + 1 : active + 3;
  native_begin(fighter, 1, count, phase >= 4 ? fighter_ready : hp_branch);
  for (int frame = 1; frame <= count; ++frame) {
    int sample = phase >= 4 ? 11 + frame : phase == 2 && frame == active + 2 ? 17 : first + frame;
    native_frame(fighter, frame, mini(sample, fighter_move(fighter)->last), phase < 4 && frame >= active && frame <= active + 2 ? hp_query : NULL);
  }
  native_play(fighter, false);
}

void retail_high_punch(Fighter *fighter) {
  hp_bind(fighter, false, 0);
  fighter->flags &= ~(unsigned)FF_BUSY;
  battle_sound(SOUND_A, 10);
  hp_build(fighter);
}

void native_air_cleanup(Fighter *fighter) {
  if (fighter_airborne(fighter) && fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  fighter->buttons = NULL;
}

static void air_guard(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    const NativeAirProfile *profile = fighter->native_context;
    fighter_finish_mmove(fighter, profile->land);
  }
}

static void air_hold_ground(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void air_fall(Fighter *fighter) {
  const NativeAirProfile *profile = fighter->native_context;
  if (fighter->script_phase == AIR_COMBO) {
    fighter->body.gravity = 0x51e;
  }
  fighter->script_phase = AIR_FALL;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, profile->land);
    return;
  }
  native_begin(fighter, 0, 0, profile->land);
  native_frame(fighter, 0, fighter_sample(fighter), air_hold_ground);
  native_play(fighter, false);
}

static void air_unfreeze(Fighter *fighter) {
  fighter->buttons = NULL;
  fighter->body.gravity = fighter->native_variant >= 3 || fighter->script_phase == AIR_COMBO_FREEZE ? 0x51e : 0x28f;
  air_fall(fighter);
}

static void air_query(Fighter *fighter) {
  const NativeAirProfile *profile = fighter->native_context;
  int index = fighter->native_variant;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, profile->land);
    return;
  }
  if (fighter->connected || battle_contact(fighter, &profile->attacks[index].record) == CONTACT_MISS) {
    return;
  }
  bool combo = fighter->script_phase == AIR_COMBO;
  bool low = fighter->body.ground - fighter->body.pos[AXIS_Y] <= 0xcccc;
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
  fighter->action = ACTION_RETAIL_1006;
  fighter->script_phase = combo ? AIR_COMBO_FREEZE : AIR_FREEZE;
  if (index == 0 || index == 2) {
    if (low) {
      fighter->body.pos[AXIS_Y] = fighter->body.ground;
      if (!combo) {
        fighter->buttons = profile->ground_followup;
        fighter->flags |= FF_FOLLOW_UP;
      }
    } else if (index == 0 && !combo) {
      fighter->buttons = profile->followup;
    }
  }
  native_hold(fighter, index >= 3 ? 8 : 12, air_unfreeze);
}

/* 46c820 keeps existing gravity, restores51e if frozen and waits for landing. */
void native_air_straight_land(Fighter *fighter) {
  if (fighter->body.gravity == 0) {
    fighter->body.gravity = 0x51e;
  }
  air_fall(fighter);
}

static void air_straight_miss(Fighter *fighter) {
  const NativeAirProfile *profile = fighter->native_context;
  if (profile->straight_miss != NULL) {
    profile->straight_miss(fighter);
  } else {
    air_fall(fighter);
  }
}

/* 46c7f0/4e6258 holds narrow pose7 for four B[40] queries. The wide
 * 46c970 branch advances poses11..13 and queries B[39] three times. */
static void air_straight_build(Fighter *fighter) {
  bool narrow = fighter_move(fighter)->clip == 327;
  int count = narrow ? 4 : 3;
  native_begin(fighter, 1, count, air_straight_miss);
  for (int frame = 1; frame <= count; ++frame) {
    native_frame(fighter, frame, narrow ? 7 : 10 + frame, air_query);
  }
  native_play(fighter, true);
}

static void air_build(Fighter *fighter) {
  const NativeAirProfile *profile = fighter->native_context;
  const AirAttack *attack = &profile->attacks[fighter->native_variant];
  int retract = attack->extend + attack->hold;
  int first = fighter->native_elapsed ? fighter->native_elapsed : 1;
  /* Native reverse-clip playback starts at the bound descriptor's last
   * stored pose. A profile's recovery budget can exceed a selected short
   * diagonal clip; those extra rows are not additional animation poses. */
  int reverse = attack->retract;
  if (fighter_move(fighter)->last != MOVE_END) {
    reverse = mini(reverse, fighter_move(fighter)->last + 1);
  }
  int last = retract + reverse;
  native_begin(fighter, first, last, air_fall);
  for (int frame = first; frame <= last; ++frame) {
    int sample = frame < attack->extend ? frame : frame < retract ? attack->extend - 1 : maxi(0, reverse - 1 - frame + retract);
    native_frame(fighter, frame, sample, frame >= attack->extend && frame < retract ? air_query : air_guard);
  }
  native_play(fighter, fighter->native_elapsed != 0);
}

void native_air_tail(Fighter *fighter, const NativeAirProfile *profile, int index, int elapsed) {
  fighter->native_context = profile;
  fighter->native_variant = index;
  fighter->native_elapsed = elapsed;
  air_build(fighter);
}

bool native_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void uppercut_recovery(Fighter *fighter) {
  static const int cycle[] = {8, 7, 6, 7, 8};
  static const int late[] = {9, 9, 8, 7, 6, 7, 8};
  int repeats = fighter->contact == CONTACT_MISS ? 3 : fighter->contact == CONTACT_BLOCK ? 2 : 8;
  int recovery = fighter->contact == CONTACT_MISS ? 8 : 9;
  bool delayed = fighter->contact_tick == 9;
  int count = repeats * 5 + (delayed ? 2 : 0) + 20 - recovery;
  native_begin(fighter, 0, count - 1, fighter_ready);
  int row = 0;
  for (int repeat = 0; repeat < repeats; ++repeat) {
    const int *poses = delayed && repeat == 0 ? late : cycle;
    int length = delayed && repeat == 0 ? 7 : 5;
    for (int frame = 0; frame < length; ++frame) {
      fighter->native.poses[row++] = poses[frame];
    }
  }
  for (int sample = recovery; sample < 20; ++sample) {
    fighter->native.poses[row++] = sample;
  }
  native_play(fighter, false);
}

void native_uppercut_tail(Fighter *fighter, FighterFn contact) {
  require(fighter_move(fighter)->clip == 293 && fighter_sample(fighter) == 5, "Uppercut needs its generated startup");
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 5, uppercut_recovery);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, frame == 1 ? pose : mini(8, frame + 4), frame >= 2 && frame <= 4 ? contact : NULL);
  }
  native_play(fighter, false);
}

/* 46cde0 queries A[2], or A[6] in a follow-up window, and skips crouch2001. */
static void uppercut_default_contact(Fighter *fighter) {
  static const AttackRecord normal = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};
  static const AttackRecord followup = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d};
  if (fighter->connected || fighter_opponent(fighter)->action == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, fighter->flags & FF_FOLLOW_UP ? &followup : &normal) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

void native_uppercut_default(Fighter *fighter) {
  native_uppercut_tail(fighter, uppercut_default_contact);
}

void native_air_straight_tail(Fighter *fighter, const NativeAirProfile *profile) {
  int clip = fighter_move(fighter)->clip;
  require(clip == 61 || (clip == 327 && profile->straight_miss != NULL), "Straight-air kick needs its selected clip and recovery");
  fighter->native_context = profile;
  fighter->native_variant = 2;
  air_straight_build(fighter);
}
