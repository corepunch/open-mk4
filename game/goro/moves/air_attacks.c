#include "../character.h"

static void goro_air_hp(Fighter *fighter);
static void goro_air_lp(Fighter *fighter);
static void goro_air_kick(Fighter *fighter);
static void goro_air_diagonal_hk(Fighter *fighter);
static void goro_air_diagonal_lk(Fighter *fighter);
static void start_air_attack(Fighter *fighter, int index);
static void goro_air_combo_lp(Fighter *fighter);
static void goro_air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
static void press_air_lp(Fighter *fighter);
static void press_air_kick(Fighter *fighter);

/* Goro shares 46e730/46eea0/46c7f0/46e150 with the humanoid roster. His
 * diagonal HK slot 4e6e48 instead enters 46e4d0, whose block 4e65a0 binds
 * clip 56 and B[31]; its callbacks match Cage's 4e65e8 apart from data. */
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = 7, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 56, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x602}, {71434, -4587, 66846, 24903, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 60, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x601}, {55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact. */
static const ButtonTable goro_air_punch_followup = {
    .press = {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const NativeAirProfile air_profile = {air_attacks, &goro_air_punch_followup, &retail_close_buttons[3], mk4_land, goro_entry_retail_script_004e6270};

static void start_air_attack(Fighter *fighter, int index) {
  static const FighterFn scripts[] = {goro_entry_retail_script_004e6658, goro_entry_retail_script_004e6710, goro_entry_retail_script_004e61f8,
      goro_entry_retail_script_004e6500, goro_entry_retail_script_004e63d8};
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  if (combo) {
    fighter->body.gravity = 0;
  }
  fighter->native_variant = index;
  fighter->script_phase = combo ? 1 : 0;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->flags &= ~(unsigned)FF_BUSY;
  scripts[index](fighter);
}

static void goro_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void goro_air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void goro_air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void goro_air_diagonal_hk(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void goro_air_diagonal_lk(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void goro_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  if (index == 2) {
    native_air_straight_tail(fighter, &air_profile);
    return;
  }
  native_air_tail(fighter, &air_profile, index, index == 4 ? air_attacks[4].extend : 0);
}

void goro_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : goro_air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter->buttons == &goro_air_punch_followup ? goro_air_combo_lp : goro_air_lp);
}

static void press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_kick);
}

void goro_press_air_lp(Fighter *fighter) {
  press_air_lp(fighter);
}

void goro_press_air_hk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, goro_air_diagonal_hk);
  } else {
    press_air_kick(fighter);
  }
}

void goro_press_air_lk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, goro_air_diagonal_lk);
  } else {
    press_air_kick(fighter);
  }
}

static void goro_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  goro_air_lp(fighter);
}

static void goro_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  goro_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, goro_air_combo_kick);
}
