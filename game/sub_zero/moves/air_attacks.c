#include "../character.h"

static const ButtonTable air_followup;
static void air_native_tail(Fighter *fighter, int index, int elapsed);
static void air_combo_lp(Fighter *fighter);
static void air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{71434, -4587, 66846, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{55705, -2621, 51773, 38010, 22, 22, 4, ATTACK_MARK, 9830}, 6, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &air_followup, &retail_close_buttons[3], sub_zero_air_land, NULL};
static const ButtonTable air_followup = {
    .press = {[BUTTON_LOW_PUNCH] = sub_zero_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const MoveDef s_air_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = ACTION_RETAIL_200E};
static const mframe_t air_getup_frames[34] = {{0}};
static const mmove_t air_getup = {.firstframe = 46, .lastframe = 79, .frame = air_getup_frames, .endfunc = fighter_ready};

void sub_zero_air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  mk4_land(fighter);
}

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

/* A queued follow-up takes ownership of the contact pause after cancellation
 * has restored safe falling for every other interruption path. */
static void air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  retail_air_low_punch(fighter);
}

static void air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  retail_air_kick(fighter);
}

void sub_zero_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : retail_air_high_punch);
}

void sub_zero_press_air_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? fighter->weapon->lp : fighter->buttons == &air_followup ? air_combo_lp : retail_air_low_punch;
  fighter_interrupt(fighter, entry);
}

void sub_zero_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? retail_air_diagonal_high_kick : retail_air_kick);
}

void sub_zero_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? retail_air_diagonal_low_kick : retail_air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_combo_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */

void sub_zero_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

void sub_zero_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

void sub_zero_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

void sub_zero_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

void sub_zero_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

void sub_zero_script_air_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

void sub_zero_script_air_getup(Fighter *victim) {
  if (victim->health <= 0) {
    fighter_finish_mmove(victim, NULL);
    return;
  }
  victim->facing = -victim->facing;
  fighter_start_mmove(victim, S_KNOCKDOWN, &s_air_getup, &air_getup);
}

void sub_zero_script_air_release(Fighter *fighter) {
  mk4_mmove_air_throw_release(fighter);
  sub_zero_entry_retail_script_004e5e50(fighter);
}
