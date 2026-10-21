#include "native_services.h"

static const MoveDef victim = {.clip = 132, .last = 59, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x4003};

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
