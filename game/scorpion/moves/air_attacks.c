#include "../character.h"

static void air_land(Fighter *fighter);
static void air_native_tail(Fighter *fighter, int index, int elapsed);
// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
};
// clang-format on
static const NativeAirProfile air_profile = {air_attacks, &retail_air_buttons[2], &retail_close_buttons[3], air_land, NULL};

static void air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  fighter_start_move(fighter, S_JUMP, &scorpion_s_teleport_land);
  fighter->sample_override = 18;
  scorpion_teleport_land_script(fighter);
}

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */

void scorpion_script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

void scorpion_script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

void scorpion_script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

void scorpion_script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 10);
}

void scorpion_script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}
