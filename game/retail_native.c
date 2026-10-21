#include "retail_native.h"
#include "math_util.h"

typedef struct NativeAction {
  uint32_t address, characters;
  FighterFn enter;
} NativeAction;

typedef struct NativeGate {
  uint32_t address;
  CommandPredicate test;
} NativeGate;

#include "retail_native_input.inc"

FighterFn retail_native_entry(unsigned character, uint32_t address) {
  require(character < MK4_ROSTER_COUNT, "Invalid retail native owner");
  for (size_t i = 0; i < sizeof entries / sizeof *entries; ++i) {
    if (entries[i].address == address && (entries[i].characters & (1u << character))) {
      return entries[i].enter;
    }
  }
  fatal("Unimplemented retail native entry %08x for roster %u", address, character);
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
