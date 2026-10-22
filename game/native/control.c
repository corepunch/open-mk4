#include "native/actions.h"
#include "math_util.h"
#include "data/tables.h"
#include "script/vm.h"

/* Shared engine action ownership for native/script attacks. The native
 * caller supplies its original action and whether commands remain live. */
void action_begin(Fighter *fighter, StateId state, int action, bool commands) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
  fighter_start_move(fighter, state, &move);
  fighter->action = action;
  if (commands) {
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

/* Local 45e530/45e570 share the finish-window predicate after their ranges;
 * 45ef20 reads the active group's gravity. No character identity is involved. */
bool action_can_finish_close(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

bool action_can_finish_range(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

bool action_has_gravity(const Fighter *fighter) {
  return fighter->body.gravity != 0;
}

/* 467ce0/43cbb0 and467cd0/43bd70 select the actual native scene entry
 * through retail tables. Hidden fighters share the scene-selection remaps. */
static void finish_enter(Fighter *fighter, bool second) {
  uint32_t target = mk4_finish_target(battle_data(), (unsigned)fighter->roster, second);
  if (target != 0) {
    script_native_entry(target)(fighter);
  }
}

void action_finish_first(Fighter *fighter) {
  finish_enter(fighter, false);
}

void action_finish_second(Fighter *fighter) {
  finish_enter(fighter, true);
}

/* Native490350 is shared by the callback word at4de664. */
void action_flip_facing(Fighter *fighter) {
  fighter->facing = -fighter->facing;
}

void action_throw_turn(Fighter *fighter) {
  uint32_t callback = mk4_image_u32(battle_data(), MK4_DATA_THROW_TURN);
  require(script_native_dispatch(fighter, callback, MK4_CALL, 0), "Unsupported throw callback table entry");
}
