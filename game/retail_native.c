#include "retail_native.h"

typedef struct NativeAction {
  uint32_t address, characters;
  FighterFn enter;
} NativeAction;

typedef struct NativeGate {
  uint32_t address, characters;
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

CommandPredicate retail_native_predicate(unsigned character, uint32_t address) {
  if (address == 0) {
    return NULL;
  }
  require(character < MK4_ROSTER_COUNT, "Invalid retail predicate owner");
  for (size_t i = 0; i < sizeof gates / sizeof *gates; ++i) {
    if (gates[i].address == address && (gates[i].characters & (1u << character))) {
      return gates[i].test;
    }
  }
  fatal("Unimplemented retail command predicate %08x for roster %u", address, character);
}

bool retail_native_gate(const Fighter *fighter, uint32_t address) {
  CommandPredicate predicate = retail_native_predicate((unsigned)fighter->roster, address);
  return predicate == NULL || predicate(fighter);
}
