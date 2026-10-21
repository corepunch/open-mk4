#include "../character.h"

static void jarek_air_hp(Fighter *fighter);
static void jarek_air_lp(Fighter *fighter);
static void jarek_air_kick(Fighter *fighter);
static void jarek_air_diagonal_hk(Fighter *fighter);
static void jarek_air_diagonal_lk(Fighter *fighter);
static void start_air_attack(Fighter *fighter, int index);
static void jarek_air_combo_lp(Fighter *fighter);
static void jarek_air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = 7, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 55, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
};
// clang-format on

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact, not another air kick. */
static const ButtonTable air_punch_followup = {
    .press = {[BUTTON_LOW_PUNCH] = jarek_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const NativeAirProfile air_profile = {air_attacks, &air_punch_followup, &retail_close_buttons[3], mk4_land, jarek_entry_retail_script_004e6270};

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
  static const FighterFn scripts[] = {jarek_entry_retail_script_004e6658, jarek_entry_retail_script_004e6710, jarek_entry_retail_script_004e61f8,
      jarek_entry_retail_script_004e6500, jarek_entry_retail_script_004e63d8};
  scripts[index](fighter);
}

static void jarek_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void jarek_air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void jarek_air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void jarek_air_diagonal_hk(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void jarek_air_diagonal_lk(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void jarek_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  if (index == 2) {
    native_air_straight_tail(fighter, &air_profile);
    return;
  }
  native_air_tail(fighter, &air_profile, index, index == 2 ? 8 : index == 4 ? 10 : 0);
}

void jarek_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jarek_air_hp);
}

void jarek_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter->buttons == &air_punch_followup ? jarek_air_combo_lp : jarek_air_lp);
}

void jarek_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_kick);
}

void jarek_press_air_hk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, jarek_air_diagonal_hk);
  } else {
    jarek_press_air_kick(fighter);
  }
}

void jarek_press_air_lk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, jarek_air_diagonal_lk);
  } else {
    jarek_press_air_kick(fighter);
  }
}

static void jarek_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  jarek_air_lp(fighter);
}

static void jarek_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  jarek_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jarek_air_combo_kick);
}
