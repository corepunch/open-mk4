#include "retail_native.h"
#include "retail_vm.h"

/* 470c60/470cb0: native contact rows, followed by a negative terminator
 * and native continuation. Poses and selectors stay in the retail image. */
static void kick_row(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->native_rows++ < 64, "Unbounded native contact rows");
  int32_t pose = (int32_t)mk4_image_u32(vm->image, vm->native_cursor);
  vm->native_cursor += 4;
  if (pose < 0) {
    uint32_t next = mk4_image_u32(vm->image, vm->native_cursor);
    uint32_t script;
    if (next == 0x46a770 || next == 0x46a6c0) {
      /* 470e80 marks the recovery action before these two continuations. */
      fighter->action = ACTION_RETAIL_1006;
      script = next == 0x46a770 ? 0x4e5c70 : 0x4e5d20;
    } else {
      require(next == 0x46a7a0, "Unimplemented native contact-row continuation");
      script = 0x4e6998;
    }
    retail_vm_enter(fighter, script, vm->finish);
    return;
  }
  require(pose <= fighter_move(fighter)->last, "Native contact pose outside clip");
  fighter->sample_override = pose;
  uint64_t continuation = fighter->continuation;
  if (!vm->native_contact && fighter_opponent(fighter)->action != ACTION_RETAIL_2001) {
    require(vm->native_query == 0x48d3f0, "Unimplemented native contact-row query");
    uint32_t record = mk4_image_u32(vm->image, 0x53d2b8 + vm->native_selector * 4);
    AttackRecord attack = retail_vm_attack_record(vm->image, record * 4);
    ContactResult result = battle_contact(fighter, &attack);
    if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
      return;
    }
    vm->native_contact = result != CONTACT_MISS;
  }
  retail_vm_wait_native(fighter, 1, kick_row);
}

void retail_kick_contact_rows(Fighter *fighter, uint32_t table) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && table >= RETAIL_SCRIPT_BEGIN && table <= RETAIL_SCRIPT_END - 12, "Native contact rows need a live retail task and data");
  vm->native_selector = mk4_image_u32(vm->image, table);
  vm->native_query = mk4_image_u32(vm->image, table + 4);
  vm->native_cursor = table + 8;
  vm->native_rows = 0;
  vm->native_contact = false;
  kick_row(fighter);
}

void retail_kick_contact_default(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e5d00);
}

void retail_kick_contact_wide(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e5d30);
}

void retail_kick_contact_narrow(Fighter *fighter) {
  retail_kick_contact_rows(fighter, 0x4e6978);
}

/* Native 46a680 and the post-gate block 46a81a set the action and enter the retail selector.
 * The script binds the clip before yielding; no fighter C clip seed is used. */
static void kick_enter(Fighter *fighter, int action, uint32_t script) {
  retail_native_begin(fighter, S_KICK, action, false);
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_high_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_RETAIL_0100, 0x4e5ca0);
}

void retail_low_kick(Fighter *fighter) {
  kick_enter(fighter, ACTION_RETAIL_0102, 0x4e5d90);
}

/* Playback after the original controller's LP throw gates (46dcbc). */
void retail_jab(Fighter *fighter) {
  retail_native_begin(fighter, S_PUNCH, ACTION_RETAIL_010B, true);
  retail_vm_enter(fighter, 0x4e62f0, fighter_ready);
}

static bool sweep_sound(BattleEvent *event, Fighter *owner, Fighter *victim) {
  (void)event;
  (void)owner;
  (void)victim;
  battle_sound(SOUND_B, 0x1c);
  return false;
}

void retail_sweep(Fighter *fighter) {
  retail_native_begin(fighter, S_SWEEP, ACTION_RETAIL_010C, false);
  /* 46ef90 spawns independent native 46ef30, which cues B[28] after14. */
  BattleEvent *event = battle_event_start(fighter, sweep_sound, false, false);
  if (event != NULL) {
    battle_event_delay(event, 14);
  }
  retail_vm_enter(fighter, 0x4e6798, fighter_ready);
}

void retail_sweep_state(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void sweep_recover(Fighter *fighter) {
  retail_vm_forward_return(fighter);
}

/* 46efd0/46f080: submit the next pose and query once, repeat only on a
 * first miss, then mark recovery, wait one visit and finish the bound clip. */
static void sweep_query(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->native_rows < 2, "Sweep query without a live native continuation");
  int pose = fighter_sample(fighter);
  if (pose < fighter_move(fighter)->last) {
    fighter->sample_override = pose + 1;
  }
  uint32_t packed = mk4_image_u32(vm->image, 0x53d2b8 + vm->native_selector * 4);
  AttackRecord attack = retail_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &attack);
  if (!vm->active || fighter->continuation != continuation || fighter->pending != NULL) {
    return;
  }
  if (result != CONTACT_MISS || ++vm->native_rows == 2) {
    fighter->action = ACTION_RETAIL_1006;
    retail_vm_wait_native(fighter, 1, sweep_recover);
  } else {
    retail_vm_wait_native(fighter, 1, sweep_query);
  }
}

static void sweep_contact_start(Fighter *fighter, unsigned selector) {
  require(fighter->retail_vm.active, "Sweep native needs a live retail task");
  fighter->retail_vm.native_rows = 0;
  fighter->retail_vm.native_selector = selector;
  sweep_query(fighter);
}

void retail_sweep_standard_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 35);
}

void retail_sweep_advancing_tail(Fighter *fighter) {
  sweep_contact_start(fighter, 36);
}

void retail_press_sweep(Fighter *fighter) {
  fighter_interrupt(fighter, retail_sweep);
}
