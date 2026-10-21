#include "../character.h"

static void jax_air_hp(Fighter *fighter);
static void jax_air_lp(Fighter *fighter);
static void jax_air_kick(Fighter *fighter);
static void jax_air_diagonal_hk(Fighter *fighter);
static void jax_air_diagonal_lk(Fighter *fighter);
static void start_air_attack(Fighter *fighter, int index);
static void jax_air_combo_lp(Fighter *fighter);
static void jax_air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 56, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x602}, {71434, -4587, 66846, 24903, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD, .action = 0x601}, {55705, -2621, 51773, 38010, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on

/* 4e6ee0 permits LP and either kick after a high air punch. 4e70c0 is
 * the grounded follow-up table after a low air contact, not another air kick. */
static const ButtonTable jax_air_punch_followup = {
    .press = {[BUTTON_LOW_PUNCH] = jax_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable jax_air_followup = {
    .press = {[BUTTON_HIGH_PUNCH] = jax_press_hp, [BUTTON_LOW_PUNCH] = jax_press_lp, [BUTTON_HIGH_KICK] = jax_press_hk, [BUTTON_LOW_KICK] = jax_press_lk}};
static const NativeAirProfile air_profile = {air_attacks, &jax_air_punch_followup, &jax_air_followup, mk4_land, NULL};

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
  static const FighterFn scripts[] = {jax_entry_retail_script_004e6658, jax_entry_retail_script_004e6710, jax_entry_retail_script_004e61f8,
      jax_entry_retail_script_004e6500, jax_entry_retail_script_004e63d8};
  scripts[index](fighter);
}

static void jax_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void jax_air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void jax_air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void jax_air_diagonal_hk(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void jax_air_diagonal_lk(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void jax_script_air_finish(Fighter *fighter) {
  int index = fighter->native_variant;
  if (index == 2) {
    native_air_straight_tail(fighter, &air_profile);
    return;
  }
  native_air_tail(fighter, &air_profile, index, index == 2 ? 8 : index == 4 ? air_attacks[4].extend : 0);
}

void jax_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : jax_air_hp);
}

void jax_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : fighter->buttons == &jax_air_punch_followup ? jax_air_combo_lp : jax_air_lp);
}

void jax_press_air_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_kick);
}

void jax_press_air_hk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, jax_air_diagonal_hk);
  } else {
    jax_press_air_kick(fighter);
  }
}

void jax_press_air_lk(Fighter *fighter) {
  if (fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z]) {
    fighter_interrupt(fighter, jax_air_diagonal_lk);
  } else {
    jax_press_air_kick(fighter);
  }
}

static void jax_air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  jax_air_lp(fighter);
}

static void jax_air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  jax_air_kick(fighter);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, jax_air_combo_kick);
}

const ButtonTable jax_diagonal_air = {.press = {[BUTTON_HIGH_PUNCH] = jax_press_air_hp,
                                          [BUTTON_LOW_PUNCH] = jax_press_air_lp,
                                          [BUTTON_HIGH_KICK] = jax_press_air_hk,
                                          [BUTTON_LOW_KICK] = jax_press_air_lk}};
