#include "../character.h"

static void kai_air_hp(Fighter *fighter);
static void kai_air_lp(Fighter *fighter);
static void kai_air_kick(Fighter *fighter);
static void kai_air_diagonal_hk(Fighter *fighter);
static void kai_air_diagonal_lk(Fighter *fighter);
static void start_air_attack(Fighter *fighter, int index);
static void kai_air_combo_lp(Fighter *fighter);
static void kai_air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 57, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x602}, {74055, 1310, 39321, 19660, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 60, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x601}, {55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact, not another air kick. */
static const ButtonTable kai_air_punch_followup = {
    {[BUTTON_LOW_PUNCH] = kai_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable kai_air_followup = {{[BUTTON_HIGH_PUNCH] = kai_press_air_ground_hp,
    [BUTTON_LOW_PUNCH] = kai_press_low_punch,
    [BUTTON_HIGH_KICK] = kai_press_high_kick,
    [BUTTON_LOW_KICK] = kai_press_low_kick}};
static const NativeAirProfile air_profile = {air_attacks, &kai_air_punch_followup, &kai_air_followup, mk4_land, NULL};

static void start_air_attack(Fighter *fighter, int index) {
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
  static const FighterFn scripts[] = {kai_entry_retail_script_004e6658, kai_entry_retail_script_004e6710, kai_entry_retail_script_004e61f8,
      kai_entry_retail_script_004e6500, kai_entry_retail_script_004e63d8};
  scripts[index](fighter);
}

static void kai_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void kai_air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void kai_air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void kai_air_diagonal_hk(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void kai_air_diagonal_lk(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void kai_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, kai_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  kai_entry_retail_script_004e5e50(fighter);
}

void kai_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  if (index == 2) {
    native_air_straight_tail(fighter, &air_profile);
    return;
  }
  native_air_tail(fighter, &air_profile, index, index == 2 ? 8 : index == 4 ? air_attacks[4].extend : 0);
}

void kai_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_hp);
}

void kai_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->buttons == &kai_air_punch_followup ? kai_air_combo_lp : kai_air_lp);
}

void kai_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_kick);
}

void kai_press_air_hk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, kai_air_diagonal_hk);
  } else {
    kai_press_air_kick(fighter);
  }
}

void kai_press_air_lk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, kai_air_diagonal_lk);
  } else {
    kai_press_air_kick(fighter);
  }
}

static void kai_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  kai_air_lp(fighter);
}

static void kai_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  kai_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, kai_air_combo_kick);
}

const ButtonTable kai_diagonal_air = {{[BUTTON_HIGH_PUNCH] = kai_press_air_hp,
    [BUTTON_LOW_PUNCH] = kai_press_air_lp,
    [BUTTON_HIGH_KICK] = kai_press_air_hk,
    [BUTTON_LOW_KICK] = kai_press_air_lk}};
