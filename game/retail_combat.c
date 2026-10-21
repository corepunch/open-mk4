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
