#include "retail_vm.h"
#include "../character.h"

void cage_cpu_kick(Fighter *fighter);

/* Native479b70 queries D[25] at poses22/23, then holds24 for twelve
 * contact visits while descriptor4e80c8 installs47bf50 velocity decay.
 * The descriptor is data, never the adjacent powered-punch script. */
static const AttackRecord shadow_record = {0x17fbe, -0x51a9, 0x13020, 0x2b43, 162, 162, 165, 0, 0x23d7};

static void shadow_ready(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

static void shadow_recover(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  native_hold(fighter, 10, shadow_ready);
}

static void shadow_contact(Fighter *fighter) {
  if (fighter_sample(fighter) == 24) {
    fighter->body.vel[AXIS_X] = q16_mul(fighter->body.vel[AXIS_X], 0xe666);
    fighter->body.vel[AXIS_Z] = q16_mul(fighter->body.vel[AXIS_Z], 0xe666);
  }
  if (battle_contact(fighter, &shadow_record) != CONTACT_MISS) {
    mk4_script_stop_body(fighter);
    fighter->sample_override = 24;
    native_hold(fighter, 10, shadow_ready);
  }
}

void cage_script_cpu_kick_tail(Fighter *fighter) {
  native_begin(fighter, 22, 35, shadow_recover);
  for (int frame = 22; frame <= 35; ++frame) {
    native_frame(fighter, frame, frame < 24 ? frame : 24, shadow_contact);
  }
  native_play(fighter, false);
}

void cage_shadow_victim(Fighter *fighter) {
  retail_vm_enter_owned(fighter, 0x4e9718, fighter_ready, 14);
}

void cage_cpu_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &cage_s_cpu_kick_script);
  retail_vm_enter_owned(fighter, 0x4e80a8, fighter_ready, 14);
}
