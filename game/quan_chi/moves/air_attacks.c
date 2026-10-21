#include "../character.h"

static void quan_chi_air_combo_lp(Fighter *fighter);
static void quan_chi_air_combo_kick(Fighter *fighter);
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

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact, not another air kick. */
static const ButtonTable quan_chi_air_punch_followup = {
    .press = {[BUTTON_LOW_PUNCH] = quan_chi_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const NativeAirProfile air_profile = {
    air_attacks, &quan_chi_air_punch_followup, &retail_close_buttons[3], mk4_land, quan_chi_entry_retail_script_004e6270};

/* 484b60: the released victim turns for clip 131's reversed getup, samples
 * 46..79, unless the throw was lethal. */

void quan_chi_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  if (index == 2) {
    native_air_straight_tail(fighter, &air_profile);
    return;
  }
  native_air_tail(fighter, &air_profile, index, index == 2 ? 8 : index == 4 ? air_attacks[4].extend : 0);
}

void quan_chi_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : retail_air_high_punch);
}

void quan_chi_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed                                     ? fighter->weapon->lp
                             : fighter->buttons == &quan_chi_air_punch_followup ? quan_chi_air_combo_lp
                                                                                : retail_air_low_punch);
}

void quan_chi_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, retail_air_kick);
}

void quan_chi_press_air_hk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, retail_air_diagonal_high_kick);
  } else {
    quan_chi_press_air_kick(fighter);
  }
}

void quan_chi_press_air_lk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, retail_air_diagonal_low_kick);
  } else {
    quan_chi_press_air_kick(fighter);
  }
}

static void quan_chi_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  retail_air_low_punch(fighter);
}

static void quan_chi_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  retail_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, quan_chi_air_combo_kick);
}

const ButtonTable quan_chi_diagonal_air = {.press = {[BUTTON_HIGH_PUNCH] = quan_chi_press_air_hp,
                                               [BUTTON_LOW_PUNCH] = quan_chi_press_air_lp,
                                               [BUTTON_HIGH_KICK] = quan_chi_press_air_hk,
                                               [BUTTON_LOW_KICK] = quan_chi_press_air_lk}};
