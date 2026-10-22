#include "script/native_ids.h"
#include "native/actions.h"
#include "script/bindings.h"

typedef struct NativeAction {
  uint32_t address;
  FighterFn enter;
} NativeAction;

typedef struct NativeGate {
  uint32_t address;
  CommandPredicate test;
} NativeGate;

#include "entries.inc"

FighterFn script_native_entry(uint32_t address) {
  for (size_t i = 0; i < sizeof entries / sizeof *entries; ++i) {
    if (entries[i].address == address) {
      return entries[i].enter;
    }
  }
  fatal("Unimplemented retail native entry %08x", address);
}

CommandPredicate script_native_predicate(uint32_t address) {
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

bool script_native_gate(const Fighter *fighter, uint32_t address) {
  CommandPredicate predicate = script_native_predicate(address);
  return predicate == NULL || predicate(fighter);
}
