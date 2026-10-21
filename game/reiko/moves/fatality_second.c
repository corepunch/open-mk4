#include "native_services.h"

enum {
  REIKO_FINISH_1 = 27,
  REIKO_FINISH_2
};

#include "retail_native.h"
#include "math_util.h"
#include <math.h>

/* B,B,D,D+HK 467cd0 -> second-fatality table 4dfe90[7] = 41c360: Reiko throws
 * a 61-star barrage (41d260, table 4d3018) at the dizzy victim. The first
 * arrival starts the victim's clip-554 stagger with push-back (41c510) and
 * jolt (41c690) tasks; the final head star (#60) is the headshot signal
 * (5322a8 = -1) that ends them, and the victim falls. */
#define SECOND_ARRIVED 1
#define SECOND_HEADSHOT 2

static const MoveDef second_owner = {.clip = 284, .last = 39, .flags = SF_LOCK | SF_RECORD, .action = 0x700};

/* 45e530: 0x1cccc <= distance <= 0x30000, then the 45e5e0 finish window. */
/* A star leaves the owner's hand joint (9 or 10) and homes on its target
 * joint at 0.15 per visit until every component is within 0.2 (41cd60).
 * Portable adaptation: the flight is scheduled from the launch distance and
 * the stars themselves are not drawn or attached; only the arrival signals
 * that drive both actors are kept. */
static int star_flight(const Fighter *owner, int hand, const Fighter *victim, int joint) {
  Vec3 from, to;
  Mat3 unused;
  if (!fighter_joint_position(owner, hand, &from, &unused) || !fighter_joint_position(victim, joint, &to, &unused)) {
    return 1;
  }
  double distance = sqrt((double)(to.x - from.x) * (to.x - from.x) + (double)(to.y - from.y) * (to.y - from.y) + (double)(to.z - from.z) * (to.z - from.z));
  return maxi(1, (int)ceil((distance - .2) / .15));
}

static bool pushback_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* 41c510: -0.15 along the victim's own axis at arrival and every five
   * visits, twenty times; after the headshot, one more step. */
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  if (event->data[0] < 20 || ((owner->native_variant & SECOND_HEADSHOT) && event->data[0] == 20)) {
    for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
      victim->body.pos[axis] -= q16_mul(fighter_axis(victim, axis), 0x2666);
    }
    event->data[0]++;
    battle_event_delay(event, event->data[0] < 20 ? 5 : 1);
    return event->data[0] <= 20;
  }
  return event->data[0] == 20;
}

static bool jolt_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* 41c690: one five-visit cycle of Y -0x1999, +0x28f, +0x51e, +0x7ae,
   * +0xa3d; 25 cycles, then one more after the headshot. */
  static const q16 jolt[] = {-0x1999, 0x28f, 0x51e, 0x7ae, 0xa3d};
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  int cycles = event->data[0] / 5;
  if (cycles >= 25 && !(cycles == 25 && (owner->native_variant & SECOND_HEADSHOT))) {
    return cycles == 25;
  }
  victim->body.pos[AXIS_Y] += jolt[event->data[0] % 5];
  event->data[0]++;
  return event->data[0] < 26 * 5;
}

static void first_arrival(Fighter *owner, Fighter *victim) {
  owner->native_variant |= SECOND_ARRIVED;
  BattleEvent *push = battle_event_start(owner, pushback_step, false, false);
  BattleEvent *jolt = battle_event_start(owner, jolt_step, false, false);
  require(push != NULL && jolt != NULL, "Reiko's star barrage needs its bounded victim tasks");
  battle_blood_hit(owner, victim, (Vec3){fighter_x(victim), q16_float(victim->body.pos[AXIS_Y]) - 1.2f, fighter_z(victim)}, 0x3333);
}

/* data[0]/data[1]: elapsed visit of the first and head-star arrivals,
 * data[2]: elapsed visits since the thrower started. */
static bool arrivals_step(BattleEvent *event, Fighter *owner, Fighter *victim) {
  if (g_battle.phase != PHASE_FATALITY) {
    return false;
  }
  int elapsed = ++event->data[2];
  if (elapsed == event->data[0]) {
    first_arrival(owner, victim);
  }
  if (elapsed == event->data[1]) {
    /* 41cd60 on the joint-2 star: 5322a8 = -1 and a 48a300 hit sound. */
    owner->native_variant |= SECOND_HEADSHOT;
    battle_scene_impact(victim);
  }
  return !(owner->native_variant & SECOND_HEADSHOT);
}

static BattleEvent *arrivals_of(const Fighter *owner) {
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (event->active && event->step == arrivals_step && &g_battle.fighters[event->owner] == owner) {
      return event;
    }
  }
  return NULL;
}

static bool star_throw(BattleEvent *event, Fighter *owner, Fighter *victim) {
  /* Star k leaves at 3 * (k / 2) + (k & 1) from alternating hands, #60
   * after a further 25 visits (4d3018: joint 3 first, joint 2 last). */
  int star = event->frame_value;
  BattleEvent *arrivals = arrivals_of(owner);
  if (arrivals != NULL && (star == 0 || star == 60)) {
    arrivals->data[star == 0 ? 0 : 1] = arrivals->data[2] + star_flight(owner, star & 1 ? 10 : 9, victim, star == 0 ? 3 : 2);
  }
  return g_battle.phase == PHASE_FATALITY;
}

static bool stars_done(BattleEvent *event, Fighter *owner, Fighter *victim) {
  return event->frame_count > 0 && fighter_opponent(owner) == victim;
}

/* 41d260, task type 0 at owner pose 29 of clip 553. */
void reiko_second_stars(Fighter *fighter, int task_type) {
  require(task_type == 0, "Unexpected Reiko star barrage task");
  BattleEvent *event = battle_event_start(fighter, stars_done, false, false);
  require(event != NULL, "Reiko's star barrage needs its bounded thrower task");
  battle_event_program(event, 116);
  for (int star = 0; star < 60; ++star) {
    battle_event_frame(event, 3 * (star / 2) + (star & 1), star, star_throw);
  }
  battle_event_frame(event, 115, 60, star_throw);
  BattleEvent *arrivals = battle_event_start(fighter, arrivals_step, false, false);
  require(arrivals != NULL, "Reiko's star barrage needs its arrival signals");
  arrivals->data[0] = arrivals->data[1] = -1;
  arrivals->data[2] = 0;
}

/* 41f1b0 parks the owner on the final barrage pose. */
void reiko_second_owner_park(Fighter *fighter) {
  fighter_stop_mmove(fighter);
}

static void arrival_gate(Fighter *victim) {
  victim->mmove_hold = !(fighter_opponent(victim)->native_variant & SECOND_ARRIVED);
}

static void victim_stagger(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4d2d60, fighter_ready, 7);
}

/* 41c4b0: a floor pool at joint 4, then 4d2d90. */
void reiko_second_victim_fall(Fighter *victim) {
  retail_vm_enter_owned(victim, 0x4d2d90, fighter_ready, 7);
}

static void second_end(Fighter *victim) {
  fighter_stop_mmove(victim);
  if (g_battle.phase == PHASE_FATALITY) {
    battle_end_finisher();
  }
}

/* 41c4e0 pools at joint 2 and parks. Retail never ends this scene; as in
 * Fujin's and Cage's ports the result follows after a 60-visit hold. */
void reiko_second_victim_end(Fighter *victim) {
  native_hold(victim, 60, second_end);
}

void reiko_second_fatality(Fighter *fighter) {
  if (!battle_begin_finisher(fighter, REIKO_FINISH_2)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->native_variant = 0;
  /* 41c430 turns the dizzy victim towards its peer (490bd0) and polls the
   * first-arrival signal before 4d2d60. */
  mk4_script_scene_pose(victim, 13, 0);
  victim->flags |= FF_BUSY | FF_UNTOUCHABLE;
  victim->facing = victim->body.pos[AXIS_X] <= fighter->body.pos[AXIS_X] ? 1 : -1;
  native_begin(victim, 0, 0, victim_stagger);
  native_frame(victim, 0, 0, arrival_gate);
  native_play(victim, false);
  /* 41c360 state 1: 4611c0 binds the stance clip 284, then 4d2d10. */
  fighter_start_move(fighter, S_PUNCH, &second_owner);
  fighter->sample_override = 0;
  fighter->flags |= FF_BUSY | FF_UNTOUCHABLE;
  retail_vm_enter_owned(fighter, 0x4d2d10, fighter_ready, 7);
}
