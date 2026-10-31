#include "script/bindings.h"
#include "script/native_ids.h"
#include "script/vm.h"
#include "native/actions.h"

/* Serialized references index typed host pointers emitted by the linker.
 * Calls retain their distinct ABI and lifetime; no executable address is cast. */
typedef struct NativeSymbol {
  const char *name;
  FighterFn entry, reaction, throw_reaction, engine_reaction;
  FighterFn call, tail, handler, physics, tail_finish;
  CommandPredicate gate;
  void (*call_global)(void);
  void (*tail_global)(void);
  unsigned (*call_result)(Fighter *fighter);
  unsigned (*predicate_result)(Fighter *fighter);
  void (*spawn)(Fighter *fighter, int argument);
  void (*handler_arg)(Fighter *fighter, int argument);
  void (*handler_checked)(Fighter *fighter, int argument);
  void (*tail_selector)(Fighter *fighter, uint32_t selector);
  void (*tail_reference)(Fighter *fighter, uint32_t reference);
  uint32_t task, tail_script;
  bool tail_inherit;
} NativeSymbol;

#include "mk4_natives.inc"

static const NativeSymbol *symbol(uint32_t reference) {
  return reference > 0 && reference < MK4_NATIVE_COUNT ? &native_symbols[reference] : NULL;
}

const char *script_native_name(uint32_t reference) {
  const NativeSymbol *native = symbol(reference);
  return native != NULL ? native->name : "<invalid native>";
}

FighterFn script_native_entry(uint32_t reference) {
  const NativeSymbol *native = symbol(reference);
  if (native == NULL || native->entry == NULL) {
    fatal("Unlinked native action %s", script_native_name(reference));
  }
  return native->entry;
}

CommandPredicate script_native_predicate(uint32_t reference) {
  if (reference == 0) {
    return NULL;
  }
  const NativeSymbol *native = symbol(reference);
  if (native == NULL || native->gate == NULL) {
    fatal("Unlinked native command gate %s", script_native_name(reference));
  }
  return native->gate;
}

bool script_native_gate(const Fighter *fighter, uint32_t reference) {
  CommandPredicate predicate = script_native_predicate(reference);
  return predicate == NULL || predicate(fighter);
}

FighterFn script_native_engine_reaction(uint32_t reference) {
  const NativeSymbol *native = symbol(reference);
  return native != NULL ? native->engine_reaction : NULL;
}

FighterFn script_native_throw_reaction(uint32_t reference) {
  const NativeSymbol *native = symbol(reference);
  return native != NULL ? native->throw_reaction : NULL;
}

FighterFn script_native_reaction(uint32_t reference) {
  const NativeSymbol *native = symbol(reference);
  if (native == NULL) {
    return NULL;
  }
  return native->reaction != NULL ? native->reaction : native->throw_reaction != NULL ? native->throw_reaction : native->engine_reaction;
}

bool script_native_dispatch(Fighter *fighter, uint32_t reference, ScriptNativeKind kind, unsigned argument) {
  const NativeSymbol *native = symbol(reference);
  if (native == NULL || fighter->roster < 0 || fighter->roster >= MK4_ROSTER_COUNT) {
    return false;
  }
  switch (kind) {
    case MK4_CALL:
      if (native->call != NULL) {
        native->call(fighter);
      } else if (native->call_global != NULL) {
        native->call_global();
      } else if (native->call_result != NULL) {
        fighter->execution.script.result = native->call_result(fighter);
      } else {
        return false;
      }
      return true;
    case MK4_TAIL:
      if (native->tail != NULL) {
        native->tail(fighter);
      } else if (native->tail_script != 0) {
        FighterFn finish = native->tail_inherit ? fighter->execution.script.finish : native->tail_finish;
        script_vm_enter(fighter, native->tail_script, finish);
      } else if (native->tail_selector != NULL) {
        native->tail_selector(fighter, fighter->execution.script.selector);
      } else if (native->tail_reference != NULL) {
        native->tail_reference(fighter, reference);
      } else if (native->tail_global != NULL) {
        native->tail_global();
      } else {
        return false;
      }
      return true;
    case MK4_SPAWN:
      if (native->spawn == NULL || argument != native->task) {
        return false;
      }
      native->spawn(fighter, (int)argument);
      return true;
    case MK4_HANDLER:
      if (native->handler != NULL) {
        native->handler(fighter);
      } else if (native->handler_arg != NULL) {
        native->handler_arg(fighter, (int)(argument & 255u));
      } else if (native->handler_checked != NULL) {
        if (action_reaction_entry(fighter->execution.script.image, argument & 255u) == NULL) {
          return false;
        }
        native->handler_checked(fighter, (int)(argument & 255u));
      } else {
        return false;
      }
      return true;
    case MK4_PREDICATE:
      if (native->predicate_result == NULL) {
        return false;
      }
      fighter->execution.script.result = native->predicate_result(fighter);
      return true;
    case MK4_PHYSICS:
      if (native->physics == NULL) {
        return false;
      }
      fighter->body.physics = native->physics;
      return true;
  }
  return false;
}
