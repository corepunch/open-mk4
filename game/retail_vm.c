#include "retail_vm.h"
#include "script.h"

/* Rule-only clients still execute retail words. The game supplies its selected
 * image through assets; this immutable cache owns the default test image. */
const Mk4Image *retail_vm_image(void) {
  static Mk4Image image;
  if (g_battle.retail_scripts != NULL) {
    return g_battle.retail_scripts;
  }
  if (image.bytes.data == NULL) {
    image = mk4_image_load("MK4/MK-4.EXE");
  }
  return &image;
}

void retail_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next) {
  require(fighter->retail_vm.active, "Native wait without a retail task");
  fighter->retail_vm.parked = true;
  fighter->retail_vm.native_resume = next;
  fighter->retail_vm.wake = g_battle.ticks + ticks;
}

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

static void jump(Fighter *fighter, uint32_t packed) {
  RetailVm *vm = &fighter->retail_vm;
  if (packed < vm->begin / 4 || packed >= vm->end / 4) {
    vm_error(fighter, "script branch outside reviewed range");
  }
  vm->pc = packed * 4;
}

static void finish(Fighter *fighter) {
  FighterFn next = fighter->retail_vm.finish;
  next(fighter);
}

void retail_vm_cancel(Fighter *fighter) {
  if (fighter->retail_vm.active) {
    FighterFn cleanup = fighter->retail_vm.cleanup;
    fighter->retail_vm = (RetailVm){0};
    fighter->avoids_high = false;
    fighter->continuation++;
    if (cleanup != NULL) {
      cleanup(fighter);
    }
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
  vm->stride = 1;
  vm->target = target;
  vm->direction = target >= current ? 1 : -1;
  vm->play_ticks = (target >= current ? target - current : current - target) + (inclusive ? 1 : 0);
  vm->wake = g_battle.ticks + 1;
}

void retail_vm_reverse_return(Fighter *fighter) {
  play(fighter, 0, true);
  fighter->retail_vm.return_after_play = true;
}

AttackRecord retail_vm_attack_record(const Mk4Image *image, uint32_t address) {
  require(address >= 0x4eb000 && address <= 0x4ec000 - 24, "Attack record outside reviewed retail data");
  uint32_t reactions = mk4_image_u32(image, address + 16);
  return (AttackRecord){(q16)mk4_image_u32(image, address), (q16)mk4_image_u32(image, address + 4), (q16)mk4_image_u32(image, address + 8),
      (q16)mk4_image_u32(image, address + 12), reactions & 255u, (reactions >> 8) & 255u, (reactions >> 16) & 255u, reactions >> 24,
      (q16)mk4_image_u32(image, address + 20)};
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
  AttackRecord record = retail_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &record);
  if (vm->active && fighter->continuation == continuation) {
    vm->result = result == CONTACT_MISS ? 0u : 1u;
  }
}

void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn next, RetailNativeFn native) {
  require(image != NULL && image->bytes.count != 0 && next != NULL && native != NULL, "Retail VM needs owned image and typed continuations");
  require(begin < end && begin % 4 == 0 && end % 4 == 0 && end - begin <= 65536, "Invalid reviewed retail script range");
  fighter_cancel_mmove(fighter);
  fighter->sequence = NULL;
  fighter->retail_vm = (RetailVm){.image = image, .pc = begin, .begin = begin, .end = end, .active = true, .finish = next, .native = native};
  fighter->continuation++;
  const MoveDef *move = fighter_move(fighter);
  if (move->clip >= 0 && move->last == MOVE_END) {
    bind(fighter, (0x4fad7c + (uint32_t)move->clip * 12) / 4, fighter_sample(fighter));
  }
  retail_vm_step(fighter);
}

void retail_vm_step(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  if (!vm->active || g_battle.ticks < vm->wake) {
    return;
  }
  if (vm->parked) {
    FighterFn next = vm->native_resume;
    if (next != NULL) {
      vm->native_resume = NULL;
      vm->parked = false;
      next(fighter);
    }
    return;
  }
  if (vm->play_ticks > 0) {
    int pose = fighter_sample(fighter);
    if (pose != vm->target) {
      int distance = vm->target >= pose ? vm->target - pose : pose - vm->target;
      sample(fighter, distance <= vm->stride ? vm->target : pose + vm->direction * vm->stride);
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
      case 0x11:
      case 0x0f: {
        bind(fighter, fetch(fighter), opcode == 9 ? -1 : (opcode == 0x0f || opcode == 0x11) ? (int)arg : 0);
        if (opcode == 0x02) {
          play(fighter, (int)arg, false);
        } else if (opcode == 9) {
          play(fighter, 0, true);
        }
        break;
      }
      case 0x06: {
        int target = fighter_sample(fighter) - (int)(word & 0xffffffu);
        play(fighter, target < 0 ? 0 : target, false);
        break;
      }
      case 0x07:
        play(fighter, 0, true);
        break;
      case 0x05:
      case 0x47: {
        if (opcode == 0x47) {
          unsigned prefix = (word >> 8) & 255u;
          if (prefix == 2) {
            bind(fighter, fetch(fighter), 0);
          } else if (prefix != 0) {
            vm_error(fighter, "unreviewed playback prefix");
          }
        }
        int target = opcode == 0x05 ? (int)(word & 0xffffffu) : (int)arg;
        if (target > fighter_sample(fighter)) {
          play(fighter, target, false);
        }
        break;
      }
      case 0x04:
      case 0x19:
      case 0x4d: {
        int current = fighter_sample(fighter);
        int target = opcode == 0x4d ? (int)arg : current + (int)arg;
        play(fighter, target, false);
        if (opcode != 0x04) {
          vm->stride = 2;
          vm->play_ticks = (target - current + 1) / 2;
        }
        break;
      }
      case 0x03:
        play(fighter, fighter_move(fighter)->last, true);
        break;
      case 0x12:
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
      case 0x27:
      case 0x2b:
        battle_sound(opcode == 0x20 ? SOUND_A : opcode == 0x27 ? SOUND_C : SOUND_B, (int)arg);
        break;
      case 0x24:
        mk4_script_attack_return(fighter);
        break;
      case 0x29:
        fighter->action = ACTION_RETAIL_1006;
        break;
      case 0x2d:
        fighter->action = ACTION_RETAIL_1007;
        break;
      case 0x3f:
      case 0x30:
        /* These roots entered from the controller; this is their explicit
         * return continuation, not a general retail tagged-stack emulation. */
        finish(fighter);
        return;
      case 0x32:
      case 0x35: {
        uint32_t address = fetch(fighter);
        if (!vm->native(fighter, address, opcode == 0x32 ? RETAIL_TAIL : RETAIL_CALL, 0)) {
          vm_error(fighter, "unreviewed native binding");
        }
        break;
      }
      case 0x25:
      case 0x26:
        mk4_script_indexed_sound(fighter, (int)arg);
        break;
      case 0x62:
        mk4_script_effect(fighter, (int)arg);
        break;
      case 0x40: {
        unsigned type = fetch(fighter);
        uint32_t address = fetch(fighter);
        if (!vm->native(fighter, address, RETAIL_SPAWN, type)) {
          vm_error(fighter, "unreviewed task binding");
        }
        break;
      }
      case 0x41:
        if (!vm->native(fighter, 0x45b1f0, RETAIL_HANDLER, arg)) {
          vm_error(fighter, "unreviewed reaction binding");
        }
        break;
      case 0x33:
        jump(fighter, fetch(fighter));
        break;
      case 0x3c: {
        /* mk4decomp MoveListCursorCluster: task character selects one of
         * sixteen packed script pointers; Kitana/Noob reuse Sonya/Reiko. */
        unsigned character = (unsigned)fighter->roster;
        character = character == 16 ? 2 : character == 17 ? 7 : character;
        uint32_t packed = 0;
        for (unsigned index = 0; index < 16; ++index) {
          uint32_t candidate = fetch(fighter);
          if (index == character) {
            packed = candidate;
          }
        }
        jump(fighter, packed);
        break;
      }
      case 0x45: {
        uint32_t packed = fetch(fighter);
        if (fighter_fast_start(fighter)) {
          jump(fighter, packed);
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
    if (!vm->active || vm->parked || fighter->pending != NULL) {
      return;
    }
    if (vm->play_ticks > 0 || g_battle.ticks < vm->wake) {
      return;
    }
  }
  vm_error(fighter, "instruction budget exhausted without yielding");
}
