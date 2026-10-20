#include "retail_vm.h"
#include "script.h"

static _Noreturn void vm_error(const Fighter *fighter, const char *message) {
  fatal("Retail VM at %08x (next %08x): %s", fighter->retail_vm.instruction, fighter->retail_vm.pc, message);
}

static uint32_t fetch(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  if (vm->pc < vm->begin || vm->pc >= vm->end || vm->pc % 4 != 0) {
    vm_error(fighter, "script cursor outside reviewed range");
  }
  uint32_t word = mk4_image_u32(vm->image, vm->pc);
  vm->pc += 4;
  return word;
}

static void finish(Fighter *fighter) {
  FighterFn next = fighter->retail_vm.finish;
  retail_vm_cancel(fighter);
  next(fighter);
}

void retail_vm_cancel(Fighter *fighter) {
  if (fighter->retail_vm.active) {
    fighter->retail_vm = (RetailVm){0};
    fighter->avoids_high = false;
    fighter->continuation++;
  }
}

static void bind(Fighter *fighter, uint32_t packed, int first) {
  RetailVm *vm = &fighter->retail_vm;
  /* Descriptors are packed word addresses after the retail data fixups. */
  if (packed < 0x4fad7c / 4 || packed >= (0x4fad7c + MK4_CLIP_COUNT * 12) / 4 || (packed * 4 - 0x4fad7c) % 12 != 0) {
    vm_error(fighter, "invalid animation descriptor");
  }
  int clip = (int)((packed * 4 - 0x4fad7c) / 12);
  uint32_t count = mk4_image_u32(vm->image, packed * 4 + 4);
  if (count == 0 || count > 65535 || first >= (int)count) {
    vm_error(fighter, "invalid animation length or initial cursor");
  }
  fighter->playback_move = *fighter_move(fighter);
  fighter->playback_move.clip = clip;
  fighter->playback_move.first = 0;
  fighter->playback_move.last = (int)count - 1;
  fighter->playback_move.sequence = NULL;
  fighter->playback_move.flags |= SF_RECORD;
  fighter->current_move = &fighter->playback_move;
  fighter->sequence = NULL;
  fighter->sample_override = first < 0 ? (int)count - 1 : first;
}

static void sample(Fighter *fighter, int pose) {
  if (pose < 0 || pose > fighter_move(fighter)->last) {
    vm_error(fighter, "animation sample outside clip");
  }
  fighter->sample_override = pose;
}

static void play(Fighter *fighter, int target, bool inclusive) {
  RetailVm *vm = &fighter->retail_vm;
  int current = fighter_sample(fighter);
  if (target < 0 || target > fighter_move(fighter)->last) {
    vm_error(fighter, "playback target outside clip");
  }
  vm->target = target;
  vm->direction = target >= current ? 1 : -1;
  vm->play_ticks = (target >= current ? target - current : current - target) + (inclusive ? 1 : 0);
  vm->wake = g_battle.ticks + 1;
}

void retail_vm_reverse_return(Fighter *fighter) {
  play(fighter, 0, true);
  fighter->retail_vm.return_after_play = true;
}

static void contact(Fighter *fighter, unsigned selector) {
  RetailVm *vm = &fighter->retail_vm;
  /* Only B[6] and B[25] are reviewed for this Scorpion prototype. */
  if (selector != vm->selector || (selector != 6 && selector != 25)) {
    vm_error(fighter, "unreviewed contact selector");
  }
  uint32_t packed = mk4_image_u32(vm->image, 0x53d2b8 + selector * 4);
  if (packed < 0x4eb000 / 4 || packed > (0x4ec000 - 24) / 4) {
    vm_error(fighter, "contact record outside reviewed data");
  }
  uint32_t address = packed * 4;
  uint32_t reactions = mk4_image_u32(vm->image, address + 16);
  AttackRecord record = {(q16)mk4_image_u32(vm->image, address), (q16)mk4_image_u32(vm->image, address + 4), (q16)mk4_image_u32(vm->image, address + 8),
      (q16)mk4_image_u32(vm->image, address + 12), reactions & 255u, (reactions >> 8) & 255u, (reactions >> 16) & 255u, reactions >> 24,
      (q16)mk4_image_u32(vm->image, address + 20)};
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &record);
  if (vm->active && fighter->continuation == continuation) {
    vm->result = result == CONTACT_MISS ? 0u : 1u;
  }
}

void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn next, RetailNativeFn native) {
  require(image != NULL && image->bytes.count != 0 && next != NULL && native != NULL, "Retail VM needs owned image and typed continuations");
  require(begin < end && begin % 4 == 0 && end % 4 == 0 && end - begin <= 2048, "Invalid reviewed retail script range");
  fighter_cancel_mmove(fighter);
  fighter->sequence = NULL;
  fighter->retail_vm = (RetailVm){.image = image, .pc = begin, .begin = begin, .end = end, .active = true, .finish = next, .native = native};
  fighter->continuation++;
  retail_vm_step(fighter);
}

void retail_vm_step(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  if (!vm->active || g_battle.ticks < vm->wake) {
    return;
  }
  if (vm->play_ticks > 0) {
    int pose = fighter_sample(fighter);
    if (pose != vm->target) {
      sample(fighter, pose + vm->direction);
    }
    vm->play_ticks--;
    if (vm->play_ticks > 0) {
      vm->wake = g_battle.ticks + 1;
      return;
    }
    if (vm->return_after_play) {
      finish(fighter);
      return;
    }
  }
  for (int budget = 0; budget < 128; ++budget) {
    vm->instruction = vm->pc;
    uint32_t word = fetch(fighter);
    unsigned opcode = word >> 24, arg = word & 255u;
    switch (opcode) {
      case 0x02:
      case 0x09:
      case 0x0f: {
        bind(fighter, fetch(fighter), opcode == 9 ? -1 : opcode == 0x0f ? (int)arg : 0);
        if (opcode == 0x02) {
          play(fighter, (int)arg, false);
        } else if (opcode == 9) {
          play(fighter, 0, true);
        }
        break;
      }
      case 0x03:
        play(fighter, fighter_move(fighter)->last, true);
        break;
      case 0x1d:
      case 0x49:
        if ((word & 0xffff00u) != 0) {
          vm_error(fighter, "unreviewed sample prefix");
        }
        sample(fighter, (int)arg);
        break;
      case 0x1f:
        vm->selector = fetch(fighter);
        break;
      case 0x20:
        battle_sound(SOUND_A, (int)arg);
        break;
      case 0x24:
        mk4_script_attack_return(fighter);
        break;
      case 0x29:
        fighter->action = ACTION_RETAIL_1006;
        break;
      case 0x30:
        /* These roots entered from the controller; this is their explicit
         * return continuation, not a general retail tagged-stack emulation. */
        finish(fighter);
        return;
      case 0x32:
      case 0x35: {
        uint32_t address = fetch(fighter);
        if (!vm->native(fighter, address, opcode == 0x32)) {
          vm_error(fighter, "unreviewed native binding");
        }
        break;
      }
      case 0x34:
        vm->wake = g_battle.ticks + (word & 0xffffffu);
        break;
      case 0x37:
        vm->wake = g_battle.ticks + ((vm->result & 1u) ? (word >> 12) & 0xfffu : word & 0xfffu);
        break;
      case 0x3a:
        if (arg == 0) {
          vm_error(fighter, "zero loop counter");
        }
        vm->loop_count = arg;
        break;
      case 0x3b: {
        if (vm->loop_count == 0) {
          vm_error(fighter, "loop without a live counter");
        }
        if (--vm->loop_count != 0) {
          uint32_t marker = vm->instruction;
          do {
            if (marker == vm->begin) {
              vm_error(fighter, "loop marker outside script");
            }
            marker -= 4;
          } while (mk4_image_u32(vm->image, marker) >> 24 != 0x3a);
          vm->pc = marker + 4;
        }
        break;
      }
      case 0x51:
        contact(fighter, arg);
        break;
      default:
        vm_error(fighter, "unsupported opcode");
    }
    if (!vm->active || fighter->pending != NULL) {
      return;
    }
    if (vm->play_ticks > 0 || g_battle.ticks < vm->wake) {
      return;
    }
  }
  vm_error(fighter, "instruction budget exhausted without yielding");
}
