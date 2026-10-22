#include "retail_native.h"
#include "math_util.h"
#include "retail_data.h"
#include "retail_vm.h"

typedef struct NativeAction {
  uint32_t address;
  FighterFn enter;
} NativeAction;

typedef struct NativeGate {
  uint32_t address;
  CommandPredicate test;
} NativeGate;

#include "retail_native_input.inc"

/* Shared engine action ownership for native/script attacks. The native
 * caller supplies its original action and whether commands remain live. */
void retail_native_begin(Fighter *fighter, StateId state, int action, bool commands) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_ATTACK | SF_LOCK | SF_RECORD};
  fighter_start_move(fighter, state, &move);
  fighter->action = action;
  if (commands) {
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

FighterFn retail_native_entry(uint32_t address) {
  for (size_t i = 0; i < sizeof entries / sizeof *entries; ++i) {
    if (entries[i].address == address) {
      return entries[i].enter;
    }
  }
  fatal("Unimplemented retail native entry %08x", address);
}

CommandPredicate retail_native_predicate(uint32_t address) {
  if (address == 0) {
    return NULL;
  }
  for (size_t i = 0; i < sizeof gates / sizeof *gates; ++i) {
    if (gates[i].address == address) {
      return gates[i].test;
    }
  }
  fatal("Unimplemented retail command predicate %08x", address);
}

bool retail_native_gate(const Fighter *fighter, uint32_t address) {
  CommandPredicate predicate = retail_native_predicate(address);
  return predicate == NULL || predicate(fighter);
}

/* Local 45e530/45e570 share the finish-window predicate after their ranges;
 * 45ef20 reads the active group's gravity. No character identity is involved. */
bool retail_can_finish_close(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

bool retail_can_finish_range(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

bool retail_has_gravity(const Fighter *fighter) {
  return fighter->body.gravity != 0;
}

/* 467ce0/43cbb0 and467cd0/43bd70 select the actual native scene entry
 * through retail tables. Hidden fighters share the scene-selection remaps. */
static void finish_enter(Fighter *fighter, bool second) {
  uint32_t target = retail_finish_target(battle_data(), (unsigned)fighter->roster, second);
  if (target != 0) {
    retail_native_entry(target)(fighter);
  }
}

void retail_finish_first(Fighter *fighter) {
  finish_enter(fighter, false);
}

void retail_finish_second(Fighter *fighter) {
  finish_enter(fighter, true);
}

/* Native490350 is shared by the callback word at4de664. */
void retail_flip_facing(Fighter *fighter) {
  fighter->facing = -fighter->facing;
}

void retail_throw_turn(Fighter *fighter) {
  uint32_t callback = mk4_image_u32(battle_data(), MK4_RETAIL_DATA_THROW_TURN);
  require(retail_native_vm(fighter, callback, RETAIL_CALL, 0), "Unsupported throw callback table entry");
}
