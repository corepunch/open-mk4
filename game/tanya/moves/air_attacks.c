#include "../character.h"

static void tanya_air_hp(Fighter *fighter);
static void tanya_air_lp(Fighter *fighter);
static void tanya_air_kick(Fighter *fighter);
static void tanya_air_diagonal_hk(Fighter *fighter);
static void tanya_air_diagonal_lk(Fighter *fighter);
static void start_air_attack(Fighter *fighter, int index);
static void tanya_air_combo_lp(Fighter *fighter);
static void tanya_air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 55, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x602}, {74055, -7864, 60293, 24903, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x601}, {75366, -2621, 67502, 34078, 22, 22, 4, 2, 9830}, 10, 10, 10},
};
// clang-format on

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact, not another air kick. */
static const ButtonTable tanya_air_punch_followup = {
    {[BUTTON_LOW_PUNCH] = tanya_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable tanya_air_followup = {{[BUTTON_HIGH_PUNCH] = tanya_press_high_punch,
    [BUTTON_LOW_PUNCH] = tanya_press_low_punch,
    [BUTTON_HIGH_KICK] = tanya_press_high_kick,
    [BUTTON_LOW_KICK] = tanya_press_low_kick}};
static const NativeAirProfile air_profile = {air_attacks, &tanya_air_punch_followup, &tanya_air_followup, mk4_land};

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
  static const FighterFn scripts[] = {tanya_entry_retail_script_004e6658, tanya_entry_retail_script_004e6710, tanya_entry_retail_script_004e61f8,
      tanya_entry_retail_script_004e6500, tanya_entry_retail_script_004e63d8};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static void tanya_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void tanya_air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void tanya_air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void tanya_air_diagonal_hk(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void tanya_air_diagonal_lk(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void tanya_script_air_release(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter_start_fiber(victim, tanya_entry_retail_script_004e9910, NULL);
  mk4_script_air_throw_release(fighter);
  tanya_entry_retail_script_004e5e50(fighter);
}

void tanya_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  native_air_tail(fighter, &air_profile, index, index == 2 ? 8 : index == 4 ? air_attacks[4].extend : 0);
}

void tanya_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_hp);
}

void tanya_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->buttons == &tanya_air_punch_followup ? tanya_air_combo_lp : tanya_air_lp);
}

void tanya_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_kick);
}

void tanya_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? tanya_air_diagonal_hk : tanya_air_kick);
}

void tanya_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? tanya_air_diagonal_lk : tanya_air_kick);
}

static void tanya_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  tanya_air_lp(fighter);
}

static void tanya_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  tanya_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, tanya_air_combo_kick);
}

const ButtonTable tanya_diagonal_air = {{[BUTTON_HIGH_PUNCH] = tanya_press_air_hp,
    [BUTTON_LOW_PUNCH] = tanya_press_air_lp,
    [BUTTON_HIGH_KICK] = tanya_press_air_hk,
    [BUTTON_LOW_KICK] = tanya_press_air_lk}};
