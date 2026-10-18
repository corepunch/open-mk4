#include "native.h"
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
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_handoff(ground_begin);
  }
  ground_begin(fighter);
}

void native_ground_next(Fighter *fighter, const NativeGroundProfile *profile) {
  fighter->native_context = profile;
  ground_build(fighter, fighter->ticks + 1, false);
}

static void hp_build(Fighter *fighter);

static void hp_query(Fighter *fighter) {
  const NativeHPProfile *profile = fighter->native_context;
  const Fighter *victim = fighter_opponent(fighter);
  bool ducking_kick = victim->action == ACTION_RETAIL_0102 && victim->avoids_high;
  if (!fighter->connected && !ducking_kick) {
    battle_contact(fighter, profile->record);
  }
}

static void hp_branch(Fighter *fighter) {
  const NativeHPProfile *profile = fighter->native_context;
  const Fighter *victim = fighter_opponent(fighter);
  int phase = fighter->script_phase;
  bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
  phase = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
  fighter_start_move(fighter, S_PUNCH, phase == 4 ? profile->first : profile->second);
  fighter->native_context = profile;
  fighter->script_phase = phase;
  fighter->sample_override = phase == 1 ? 1 : phase == 3 ? 2 : 11;
  fighter->flags &= ~(unsigned)FF_BUSY;
  hp_build(fighter);
}

static void hp_build(Fighter *fighter) {
  const NativeHPProfile *profile = fighter->native_context;
  int phase = fighter->script_phase;
  int active = phase == 0 ? 8 : phase == 1 ? 7 : 6;
  int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
  int count = phase >= 4 ? (phase == 4 ? 10 : 8) : active + 3;
  native_begin(fighter, 1, count, phase >= 4 ? profile->ready : hp_branch);
  for (int frame = 1; frame <= count; ++frame) {
    int sample = phase >= 4 ? mini(phase == 4 ? 20 : 18, 11 + frame) : phase == 2 && frame == active + 2 ? 17 : first + frame;
    native_frame(fighter, frame, sample, phase < 4 && frame >= active && frame <= active + 2 ? hp_query : NULL);
  }
  native_play(fighter, false);
}

void native_hp_start(Fighter *fighter, const NativeHPProfile *profile) {
  fighter->native_context = profile;
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_handoff(hp_build);
  }
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
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_handoff(air_build);
  }
  air_build(fighter);
}

bool native_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

void native_uppercut_tail(Fighter *fighter, FighterFn contact, FighterFn ready) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 5, NULL);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, frame == 1 ? pose : mini(8, frame + 4), frame >= 2 && frame <= 4 ? contact : NULL);
  }
  native_wait(fighter);
  static const int cycle[] = {8, 7, 6, 7, 8};
  static const int late[] = {9, 9, 8, 7, 6, 7, 8};
  int repeats = fighter->contact == CONTACT_MISS ? 3 : fighter->contact == CONTACT_BLOCK ? 2 : 8;
  int recovery = fighter->contact == CONTACT_MISS ? 8 : 9;
  bool delayed = fighter->contact_tick == 9;
  int count = repeats * 5 + (delayed ? 2 : 0) + 20 - recovery;
  native_begin(fighter, 0, count - 1, ready);
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

/* 470c60 block 4e6978: B[2], poses 10/11/12, then 46a7a0 -> 4e6998. */
static const AttackRecord narrow_kick_record = {0x116c8, -0xde76, 0x1476c, 0x12395, 16, 13, 6, 0, 0x2147};

static void narrow_kick_contact(Fighter *fighter) {
  if (!fighter->connected && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    battle_contact(fighter, &narrow_kick_record);
  }
}

void native_narrow_kick_contact(Fighter *fighter, FighterFn recovery) {
  require(fighter_move(fighter)->clip == 71 && fighter_sample(fighter) == 10, "Narrow high kick needs its selected startup");
  native_begin(fighter, 1, 3, recovery);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, 10 + frame, narrow_kick_contact);
  }
  native_play(fighter, false);
}
