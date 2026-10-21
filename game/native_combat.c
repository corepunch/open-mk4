#include "native.h"
#include "retail_native.h"
#include "retail_vm.h"
#include "math_util.h"
#include <string.h>

/* Native 46d290/46d9b3 binds these two descriptors directly; unlike the
 * authored move programs, its repeat/contact controller is native code. */
static void hp_bind(Fighter *fighter, bool second, int first) {
  retail_native_begin(fighter, S_PUNCH, ACTION_RETAIL_010A, true);
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

bool native_grounded(const Fighter *fighter) {
  return !fighter_airborne(fighter);
}

static void uppercut_recovery(Fighter *fighter) {
  static const int cycle[] = {8, 7, 6, 7, 8};
  static const int late[] = {9, 9, 8, 7, 6, 7, 8};
  int repeats = fighter->contact == CONTACT_MISS ? 3 : fighter->contact == CONTACT_BLOCK ? 2 : 8;
  int recovery = fighter->contact == CONTACT_MISS ? 8 : 9;
  bool delayed = fighter->contact_tick == 9;
  int last = fighter_move(fighter)->last;
  int count = repeats * 5 + (delayed ? 2 : 0) + last + 1 - recovery;
  native_begin(fighter, 0, count - 1, fighter_ready);
  int row = 0;
  for (int repeat = 0; repeat < repeats; ++repeat) {
    const int *poses = delayed && repeat == 0 ? late : cycle;
    int length = delayed && repeat == 0 ? 7 : 5;
    for (int frame = 0; frame < length; ++frame) {
      fighter->native.poses[row++] = poses[frame];
    }
  }
  for (int sample = recovery; sample <= last; ++sample) {
    fighter->native.poses[row++] = sample;
  }
  native_play(fighter, false);
}

static void native_uppercut_tail(Fighter *fighter, FighterFn contact) {
  require(fighter_move(fighter)->clip == 293 && fighter_sample(fighter) == 5, "Uppercut needs its retail startup");
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 5, uppercut_recovery);
  for (int frame = 1; frame <= 5; ++frame) {
    native_frame(fighter, frame, frame == 1 ? pose : mini(8, frame + 4), frame >= 2 && frame <= 4 ? contact : NULL);
  }
  native_play(fighter, false);
}

/* 46cde0 queries A[2], or A[6] in a follow-up window, and skips crouch2001. */
static void uppercut_default_contact(Fighter *fighter) {
  if (fighter->connected || fighter_opponent(fighter)->action == ACTION_RETAIL_2001) {
    return;
  }
  const Mk4Image *image = retail_vm_image();
  unsigned selector = fighter->flags & FF_FOLLOW_UP ? 6 : 2;
  uint32_t packed = mk4_image_u32(image, 0x53d268 + selector * 4);
  AttackRecord record = retail_vm_attack_record(image, packed * 4);
  if (battle_contact(fighter, &record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

void native_uppercut_default(Fighter *fighter) {
  native_uppercut_tail(fighter, uppercut_default_contact);
}
