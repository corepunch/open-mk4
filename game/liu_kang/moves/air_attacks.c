#include "../character.h"

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;
static void air_land(Fighter *fighter);
static void air_return_land(Fighter *fighter);
static void air_native_tail(Fighter *fighter, int index, int elapsed);
static void start_air_attack(Fighter *fighter, int index);
static void air_hp(Fighter *fighter);
static void air_lp(Fighter *fighter);
static void air_kick(Fighter *fighter);
static void air_hk_diagonal(Fighter *fighter);
static void air_lk_diagonal(Fighter *fighter);
static void press_air_followup_kick(Fighter *fighter);
static void air_ground_hp(Fighter *fighter);
static void press_air_ground_hp(Fighter *fighter);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 61, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {69468, -26214, 64225, 30146, 21, 10, 8, 0, 8519}, 6, 3, 20},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, air_land, NULL};
static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = liu_kang_press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};
static const ButtonTable air_ground_followup = {{[BUTTON_HIGH_PUNCH] = press_air_ground_hp,
    [BUTTON_LOW_PUNCH] = liu_kang_press_lp,
    [BUTTON_HIGH_KICK] = liu_kang_press_kick,
    [BUTTON_LOW_KICK] = liu_kang_press_lk}};

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
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_peer(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {liu_kang_entry_retail_script_004e6658, liu_kang_entry_retail_script_004e6710, liu_kang_entry_retail_script_004e6df0,
      liu_kang_entry_retail_script_004e6500, liu_kang_entry_retail_script_004e63d8};
  scripts[index](fighter);
}

static void air_hp(Fighter *fighter) {
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

void liu_kang_press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : air_hp);
}

void liu_kang_press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

void liu_kang_press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

void liu_kang_press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_kick);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    retail_close_high_punch(fighter);
  } else {
    liu_kang_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    liu_kang_press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

void liu_kang_liu_script_spawn_air(Fighter *fighter, int task_type) {
  require(task_type == 64, "Wrong Liu Kang air-fireball task type");
  liu_kang_fireball_spawn(fighter, 2);
}

static void air_return_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

void liu_kang_liu_script_land(Fighter *fighter) {
  fighter_finish_mmove(fighter, mk4_land);
}

void liu_kang_liu_script_air_return(Fighter *fighter) {
  fighter->body.gravity = 0x20c;
  native_await(fighter, native_grounded, air_return_land);
}

void liu_kang_liu_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

void liu_kang_liu_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

void liu_kang_liu_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 6);
}

void liu_kang_liu_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

void liu_kang_liu_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 10);
}

void liu_kang_cpu_air_hp(Fighter *fighter) {
  if (fighter->armed) {
    weapon_air_frames(fighter, true, liu_kang_entry_retail_script_004e66d0);
  } else {
    air_hp(fighter);
  }
}

void liu_kang_cpu_air_hk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 3 : 2);
}

void liu_kang_cpu_air_lk(Fighter *fighter) {
  start_air_attack(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? 4 : 2);
}
