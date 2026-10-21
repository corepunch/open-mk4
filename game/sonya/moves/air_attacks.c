#include "../character.h"

static const ButtonTable air_followup;
static void air_land(Fighter *fighter);
static void air_release_land(Fighter *fighter);
static void air_release_fall(Fighter *fighter);
static void air_getup_rise(Fighter *victim);
static void air_native_tail(Fighter *fighter, int index, int elapsed);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{74055, 1310, 39321, 19660, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &air_followup, &retail_close_buttons[3], air_land, NULL};
static const ButtonTable air_followup = {
    .press = {[BUTTON_LOW_PUNCH] = sonya_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static void air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  mk4_land(fighter);
}

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

void sonya_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : retail_air_high_punch);
}

void sonya_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : retail_air_low_punch);
}

void sonya_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? retail_air_diagonal_high_kick : retail_air_kick);
}

void sonya_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? retail_air_diagonal_low_kick : retail_air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, retail_air_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */

void sonya_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

void sonya_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

void sonya_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

void sonya_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

void sonya_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void air_release_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

static void air_release_fall(Fighter *fighter) {
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = -0x51e;
  native_await(fighter, native_grounded, air_release_land);
}

/* The released victim's stream starts after the owner's damage, so its
 * setup sees the damaged victim. */
void sonya_script_air_release(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 85;
  battle_damage(victim, 0x2666);
  fighter_cancel_mmove(victim);
  sonya_entry_retail_script_004e98a0(victim);
  native_finish_clip(fighter, air_release_fall);
}

static void air_getup_rise(Fighter *victim) {
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  mk4_mmove_bind(victim, 197, 29, 0);
  native_finish_clip(victim, mk4_mmove_thrown_finish);
}

void sonya_script_air_getup(Fighter *victim) {
  native_finish_clip(victim, air_getup_rise);
}

void sonya_cpu_air_hk(Fighter *fighter) {
  retail_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 3 : 2);
}

void sonya_cpu_air_lk(Fighter *fighter) {
  retail_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 4 : 2);
}
