#include "retail_vm.h"
#include "../character.h"

// clang-format off: Keep table entries on one row, including long rows.
static const WeaponStrike strikes[] = {
  {{.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {125763, -34603, 98304, 68157, 139, 139, 118, 0, 11141}, 0, 9, 16, 4, 0, 9, 11, 0x147a, 34},
  {{.clip = 238, .last = 36, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {125763, -34603, 98304, 68157, 140, 140, 118, 0, 13107}, 0, 9, 25, 4, 5, 1, 16, 0x1999, 34},
  {{.clip = 242, .last = 30, .flags = SF_LOCK | SF_RECORD, .action = 0x212}, {111411, -25552, 89784, 11796, 62, 62, 118, 0, 9830}, 0, 9, 11, 3, 8, 2, 18, 0x28f5, 34},
  {{.clip = 243, .last = 27, .flags = SF_LOCK | SF_RECORD, .action = 0x10e}, {115277, 41287, 73400, 31457, 141, 141, 118, ATTACK_MARK, 11796}, 0, 3, 15, 2, 9, 0, 0, 0, 34},
};
// clang-format on
static const MoveDef followup = {.clip = 237, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x112};
static const MoveDef victim = {.clip = 132, .last = 59, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x4003};
static const WeaponDef polearm = {0x64, shinnok_weapon_hp, shinnok_weapon_lp, shinnok_weapon_back_hp, shinnok_weapon_back_lp, NULL, weapon_throw,
    shinnok_weapon_equip, SCENE_MODEL_SHINNOK_WEAPON};

void shinnok_weapon_back_hp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[2], true);
}

void shinnok_weapon_back_lp(Fighter *fighter) {
  weapon_start_strike(fighter, &strikes[3], false);
}

void shinnok_weapon_hp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_high_punch(fighter);
  } else if (fighter_holds_back(fighter)) {
    shinnok_weapon_back_hp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[0], true);
  }
}

void shinnok_weapon_lp(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    retail_air_low_punch(fighter);
  } else if (fighter_distance(fighter) <= .7f && !fighter_airborne(fighter_opponent(fighter)) && !(fighter_opponent(fighter)->flags & FF_REACTING)) {
    retail_lp_throw(fighter);
  } else if (fighter_holds_back(fighter)) {
    shinnok_weapon_back_lp(fighter);
  } else {
    weapon_start_strike(fighter, &strikes[1], true);
  }
}

void shinnok_weapon_equip(Fighter *fighter) {
  fighter_equip_weapon(fighter, &polearm);
}

void shinnok_script_draw_equip(Fighter *fighter) {
  shinnok_weapon_equip(fighter);
  retail_vm_enter_owned(fighter, 0x4e62d8, fighter_ready, 6);
}

static void shinnok_weapon_followup(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &followup);
  retail_vm_enter_owned(fighter, 0x4ec8b0, fighter_ready, 6);
}

void shinnok_weapon_action(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    shinnok_weapon_equip(fighter);
    weapon_followup_victim(fighter);
    shinnok_weapon_followup(fighter);
  } else {
    weapon_draw(fighter, &polearm);
    retail_vm_enter_owned(fighter, 0x4e62c0, fighter_ready, 6);
  }
}

void shinnok_weapon_crouch_return(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, NULL);
  if (fighter->health > 0) {
    fighter_enter_state(fighter, S_CROUCH);
  }
}

void shinnok_weapon_sweep_getup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return;
  }
  retail_vm_enter_owned(fighter, 0x4e9498, mk4_mmove_thrown_finish, 6);
}

/* 4830a0/483160 resumes clip197 at zero and finishes before controller
 * return. The portable runner shares the same bounded phase for both slots. */
void shinnok_weapon_sweep_finish(Fighter *fighter) {
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  native_begin(fighter, first, last, mk4_mmove_thrown_finish);
  for (int frame = first; frame <= last; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

void shinnok_weapon_reaction(Fighter *fighter) {
  require(fighter->reaction == 140 || fighter->reaction == 141, "Invalid Shinnok weapon victim entry");
  mk4_script_disarm_self(fighter);
  if (fighter->reaction == 140) {
    battle_wall_recoil(fighter);
  } else {
    mk4_script_indexed_sound(fighter, 3);
    mk4_script_stop_body(fighter);
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
  }
  fighter_start_move(fighter, S_KNOCKDOWN, &victim);
  fighter->flags |= FF_REACTING;
  if (fighter->reaction == 140) {
    retail_vm_enter_owned(fighter, 0x4e8d08, mk4_mmove_thrown_finish, 6);
  } else {
    retail_vm_enter_owned(fighter, 0x4e9460, mk4_mmove_thrown_finish, 6);
  }
}
