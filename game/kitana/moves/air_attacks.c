#include "../character.h"

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;
static void air_land(Fighter *fighter);
static void air_native_tail(Fighter *fighter, int index, int elapsed);
static void start_air_attack(Fighter *fighter, int index);
static void air_lp(Fighter *fighter);
static void air_kick(Fighter *fighter);
static void air_hk_diagonal(Fighter *fighter);
static void air_lk_diagonal(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
static void air_ground_hp(Fighter *fighter);
static void press_air_ground_hp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 57, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {74055, 1310, 39321, 19660, 23, 23, 4, 2, 6553}, 6, 10, 6},
  {{.clip = 60, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {55705, 1966, 56950, 30146, 22, 22, 4, 2, 9830}, 6, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, air_land, NULL};
static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = kitana_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable air_ground_followup = {{[BUTTON_HIGH_PUNCH] = press_air_ground_hp,
    [BUTTON_LOW_PUNCH] = kitana_press_lp,
    [BUTTON_HIGH_KICK] = kitana_press_kick,
    [BUTTON_LOW_KICK] = kitana_press_lk}};

static void air_land(Fighter *fighter) {
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
  fighter->facing = fighter->body.pos[AXIS_X] <= kitana_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {kitana_entry_retail_script_004e6658, kitana_entry_retail_script_004e6710, kitana_entry_retail_script_004e61f8,
      kitana_entry_retail_script_004e6500, kitana_entry_retail_script_004e63d8};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

void kitana_air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void air_hk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void air_lk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

void kitana_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : kitana_air_hp);
}

void kitana_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

void kitana_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

void kitana_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    kitana_ground_attack(fighter, G_CLOSE_HP);
  } else {
    kitana_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    kitana_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

void kitana_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

void kitana_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

void kitana_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

void kitana_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 6);
}

void kitana_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

void kitana_script_air_release(Fighter *fighter) {
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = 85;
  fighter_start_fiber(victim, kitana_entry_retail_script_004e98a0, NULL);
  battle_damage(victim, 0x2666);
  mk4_script_finish_clip(fighter, 1);
  fighter->body.gravity = 0x20c;
  fighter->body.vel[AXIS_Y] = -0x51e;
  native_until(fighter, native_grounded);
  fighter_fiber_finish(mk4_land);
}

void kitana_script_air_getup(Fighter *victim) {
  mk4_script_finish_clip(victim, 1);
  mk4_script_stop_body(victim);
  if (victim->health <= 0) {
    fighter_fiber_finish(NULL);
  }
  mk4_script_bind(victim, 197, 29, 0);
  mk4_script_finish_clip(victim, 1);
  mk4_script_thrown_finish(victim);
}

void kitana_cpu_air_hk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 3 : 2);
}

void kitana_cpu_air_lk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 4 : 2);
}
