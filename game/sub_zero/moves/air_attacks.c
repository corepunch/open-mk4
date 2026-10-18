#include "../character.h"

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;
static void air_native_tail(Fighter *fighter, int index, int elapsed);
static void start_air_attack(Fighter *fighter, int index);
static void air_lp(Fighter *fighter);
static void air_kick(Fighter *fighter);
static void air_combo_lp(Fighter *fighter);
static void air_combo_kick(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
static void air_ground_hp(Fighter *fighter);
static void press_air_ground_hp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 56, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {71434, -4587, 66846, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 59, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {55705, -2621, 51773, 38010, 22, 22, 4, ATTACK_MARK, 9830}, 6, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, sub_zero_air_land, NULL};
static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = sub_zero_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable air_ground_followup = {{[BUTTON_HIGH_PUNCH] = press_air_ground_hp,
    [BUTTON_LOW_PUNCH] = sub_zero_press_lp,
    [BUTTON_HIGH_KICK] = sub_zero_press_kick,
    [BUTTON_LOW_KICK] = sub_zero_press_lk}};
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

static void start_air_attack(Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  if (combo) {
    fighter->body.gravity = 0;
  }
  fighter_upright(fighter);
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= sub_zero_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {sub_zero_entry_retail_script_004e6658, sub_zero_entry_retail_script_004e6710, sub_zero_entry_retail_script_004e61f8,
      sub_zero_entry_retail_script_004e6500, sub_zero_entry_retail_script_004e63d8};
  fighter->mmove_cleanup = native_air_cleanup;
  scripts[index](fighter);
}

void sub_zero_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

void sub_zero_air_hk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

void sub_zero_air_lk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

/* A queued follow-up takes ownership of the contact pause after cancellation
 * has restored safe falling for every other interruption path. */
static void air_combo_lp(Fighter *fighter) {
  fighter->body.gravity = 0;
  air_lp(fighter);
}

static void air_combo_kick(Fighter *fighter) {
  fighter->body.gravity = 0;
  air_kick(fighter);
}

void sub_zero_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : sub_zero_air_hp);
}

void sub_zero_press_air_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? fighter->weapon->lp : fighter->buttons == &air_followup ? air_combo_lp : air_lp;
  fighter_interrupt(fighter, entry);
}

void sub_zero_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? sub_zero_air_hk_diagonal : air_kick);
}

void sub_zero_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? sub_zero_air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_combo_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    sub_zero_ground_attack(fighter, G_CLOSE_HP);
  } else {
    sub_zero_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    sub_zero_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

bool sub_zero_lp_pair_released(const Fighter *victim) {
  const Fighter *owner = fighter_opponent(victim);
  return !(owner->flags & FF_PAIRED) || owner->pending != NULL;
}

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
