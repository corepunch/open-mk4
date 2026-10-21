#include "retail_vm.h"
#include "script.h"
#include "retail_native.h"

static void resume_script_return(Fighter *fighter);

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
    if (fighter->retail_vm.blend_ticks > 0) {
      fighter->blend_duration = 0;
      fighter->blend_weight = -1;
    }
    fighter->retail_vm = (RetailVm){0};
    fighter->avoids_high = false;
    fighter->continuation++;
    if (cleanup != NULL) {
      cleanup(fighter);
    }
  }
}

static void bind_image(Fighter *fighter, const Mk4Image *image, uint32_t packed, int first) {
  /* Descriptors are packed word addresses after the retail data fixups. */
  if (packed < 0x4fad7c / 4 || packed >= (0x4fad7c + MK4_CLIP_COUNT * 12) / 4 || (packed * 4 - 0x4fad7c) % 12 != 0) {
    vm_error(fighter, "invalid animation descriptor");
  }
  int clip = (int)((packed * 4 - 0x4fad7c) / 12);
  uint32_t count = mk4_image_u32(image, packed * 4 + 4);
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

void retail_vm_bind_animation(Fighter *fighter, uint32_t descriptor, int first) {
  require(descriptor % 4 == 0, "Unaligned animation descriptor");
  bind_image(fighter, fighter->retail_vm.active ? fighter->retail_vm.image : retail_vm_image(), descriptor / 4, first);
}

static void bind(Fighter *fighter, uint32_t packed, int first) {
  bind_image(fighter, fighter->retail_vm.image, packed, first);
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
  if (target < 0 || target > fighter_move(fighter)->last + 1) {
    vm_error(fighter, "playback target outside clip");
  }
  vm->stride = 1;
  vm->period = 1;
  vm->target = target > fighter_move(fighter)->last ? fighter_move(fighter)->last : target;
  vm->direction = target >= current ? 1 : -1;
  vm->play_ticks = (target >= current ? target - current : current - target) + (inclusive ? 1 : 0);
  vm->wake = g_battle.ticks + (vm->play_ticks > 0 ? 1 : 0);
}

/* PauseTestCmp2CallStore consumes a descriptor only for prefix byte 2. */
static void optional_bind(Fighter *fighter, uint32_t word) {
  unsigned prefix = (word >> 8) & 255u;
  if (prefix == 2) {
    bind(fighter, fetch(fighter), 0);
  } else if (prefix != 0) {
    vm_error(fighter, "unreviewed playback prefix");
  }
}

static void slow_play(Fighter *fighter, int target, bool inclusive) {
  play(fighter, target, inclusive);
  RetailVm *vm = &fighter->retail_vm;
  vm->period = 2;
  vm->wake = g_battle.ticks + (target == fighter_sample(fighter) ? 1 : 2);
}

void retail_vm_reverse_return(Fighter *fighter) {
  play(fighter, 0, true);
  fighter->retail_vm.return_after_play = true;
}

void retail_vm_forward_return(Fighter *fighter) {
  play(fighter, fighter_move(fighter)->last, true);
  fighter->retail_vm.return_after_play = true;
}

AttackRecord retail_vm_attack_record(const Mk4Image *image, uint32_t address) {
  require(address >= 0x4eb000 && address <= 0x4ec2a8 - 24, "Attack record outside reviewed retail data");
  uint32_t reactions = mk4_image_u32(image, address + 16);
  return (AttackRecord){(q16)mk4_image_u32(image, address), (q16)mk4_image_u32(image, address + 4), (q16)mk4_image_u32(image, address + 8),
      (q16)mk4_image_u32(image, address + 12), reactions & 255u, (reactions >> 8) & 255u, (reactions >> 16) & 255u, reactions >> 24,
      (q16)mk4_image_u32(image, address + 20)};
}

static void contact(Fighter *fighter, uint32_t table, unsigned selector) {
  RetailVm *vm = &fighter->retail_vm;
  uint32_t packed = mk4_image_u32(vm->image, table + selector * 4);
  if (packed < 0x4eb000 / 4 || packed > (0x4ec2a8 - 24) / 4) {
    vm_error(fighter, "contact record outside reviewed data");
  }
  AttackRecord record = retail_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->continuation;
  ContactResult result = battle_contact(fighter, &record);
  if (vm->active && fighter->continuation == continuation) {
    vm->result = result == CONTACT_MISS ? 0u : 1u;
  }
}

void retail_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, uint32_t begin, uint32_t end, FighterFn next, RetailNativeFn native) {
  require(image != NULL && image->bytes.count != 0 && next != NULL && native != NULL, "Retail VM needs owned image and typed continuations");
  require(begin <= entry && entry < end && begin % 4 == 0 && end % 4 == 0 && end - begin <= RETAIL_SCRIPT_END - RETAIL_SCRIPT_BEGIN,
      "Invalid reviewed retail script range");
  RetailVm previous = fighter->retail_vm;
  FighterFn cleanup = previous.cleanup != NULL ? previous.cleanup : fighter->mmove_cleanup;
  fighter->retail_vm.cleanup = NULL;
  fighter->mmove_cleanup = NULL;
  fighter_cancel_mmove(fighter);
  fighter->sequence = NULL;
  fighter->retail_vm = (RetailVm){.image = image,
      .entry = entry,
      .pc = entry,
      .begin = begin,
      .end = end,
      .active = true,
      .finish = next,
      .then = next == resume_script_return ? previous.then : (RetailScriptContinuation){0},
      .cleanup = cleanup,
      .native = native,
      .loop_count = previous.loop_count,
      .result = previous.result,
      .selector = previous.selector,
      .repeat_count = previous.repeat_count,
      .repeat_entry = previous.repeat_entry,
      .repeat_after = previous.repeat_after,
      .native_owner = previous.native_owner_set ? previous.native_owner : (unsigned)fighter->roster,
      .native_owner_set = true};
  fighter->continuation++;
  const MoveDef *move = fighter_move(fighter);
  if (move->clip >= 0 && move->last == MOVE_END) {
    bind(fighter, (0x4fad7c + (uint32_t)move->clip * 12) / 4, fighter_sample(fighter));
  }
  retail_vm_step(fighter);
}

void retail_vm_enter(Fighter *fighter, uint32_t entry, FighterFn next) {
  retail_vm_start_at(fighter, retail_vm_image(), entry, RETAIL_SCRIPT_BEGIN, RETAIL_SCRIPT_END, next, retail_native_vm);
}

/* Legacy native services are still selected by the originating integration
 * context. A victim/scene actor keeps its own model roster independently. */
static void resume_script_return(Fighter *fighter) {
  RetailScriptContinuation next = fighter->retail_vm.then;
  require(next.entry != 0, "Missing retail script return continuation");
  fighter->retail_vm.then = (RetailScriptContinuation){0};
  retail_vm_enter_owned(fighter, next.entry, next.finish, next.native_owner);
}

/* Native callers that enter a second script after an original return keep
 * that continuation in VM ownership, independent of native frame completion. */
void retail_vm_enter_then_owned(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish, unsigned native_owner) {
  require(after >= RETAIL_SCRIPT_BEGIN && after < RETAIL_SCRIPT_END && after % 4 == 0 && finish != NULL && native_owner < MK4_ROSTER_COUNT,
      "Invalid retail script return continuation");
  fighter->retail_vm.then = (RetailScriptContinuation){after, finish, native_owner};
  retail_vm_enter_owned(fighter, entry, resume_script_return, native_owner);
}

void retail_vm_enter_owned(Fighter *fighter, uint32_t entry, FighterFn next, unsigned native_owner) {
  require(native_owner < MK4_ROSTER_COUNT, "Invalid native integration context");
  fighter->retail_vm.native_owner = native_owner;
  fighter->retail_vm.native_owner_set = true;
  retail_vm_enter(fighter, entry, next);
}

static void repeat_enter(Fighter *fighter, uint32_t entry) {
  RetailVm *vm = &fighter->retail_vm;
  if (vm->active) {
    retail_vm_start_at(fighter, vm->image, entry, vm->begin, vm->end, vm->finish, vm->native);
  } else {
    retail_vm_enter(fighter, entry, fighter_ready);
  }
}

void retail_vm_repeat(Fighter *fighter, uint32_t entry, unsigned count, uint32_t after) {
  require(count > 0, "Native script repetition must be bounded");
  fighter->retail_vm.repeat_count = count;
  fighter->retail_vm.repeat_entry = entry;
  fighter->retail_vm.repeat_after = after;
  repeat_enter(fighter, entry);
}

void retail_vm_repeat_next(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  require(vm->active && vm->repeat_count > 0, "Native script cycle without a live counter");
  uint32_t next = --vm->repeat_count == 0 ? vm->repeat_after : vm->repeat_entry;
  repeat_enter(fighter, next);
}

void retail_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn next, RetailNativeFn native) {
  retail_vm_start_at(fighter, image, begin, begin, end, next, native);
}

void retail_vm_step(Fighter *fighter) {
  RetailVm *vm = &fighter->retail_vm;
  if (!vm->active || g_battle.ticks < vm->wake) {
    return;
  }
  if (vm->wait_ground) {
    if (fighter_airborne(fighter)) {
      return;
    }
    vm->wait_ground = false;
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
  if (vm->blend_ticks > 0) {
    if (--vm->blend_ticks > 0) {
      fighter->blend_weight += 0xa3d;
      vm->wake = g_battle.ticks + 1;
      return;
    }
    fighter->blend_duration = 0;
    fighter->blend_weight = -1;
    sample(fighter, 1);
  }
  if (vm->play_ticks > 0) {
    int pose = fighter_sample(fighter);
    if (pose != vm->target) {
      int distance = vm->target >= pose ? vm->target - pose : pose - vm->target;
      sample(fighter, distance <= vm->stride ? vm->target : pose + vm->direction * vm->stride);
    }
    if (vm->paired && !(fighter->flags & FF_PAIR_RELEASE)) {
      Fighter *victim = (Fighter *)fighter_opponent(fighter);
      if (victim->pending != NULL || !(fighter->flags & FF_PAIRED) || !(victim->flags & FF_PAIRED)) {
        fighter_ready(fighter);
        return;
      }
      if (pose != fighter_sample(fighter) && !(victim->flags & FF_PAIR_RELEASE)) {
        int pose = fighter_sample(victim) + vm->direction;
        if (pose >= 0 && pose <= fighter_move(victim)->last) {
          victim->sample_override = pose;
        }
      }
    }
    vm->play_ticks--;
    if (vm->play_ticks > 0) {
      vm->wake = g_battle.ticks + (vm->play_ticks == 1 && fighter_sample(fighter) == vm->target ? 1 : vm->period);
      return;
    }
    vm->paired = false;
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
      case 0x00:
        break;
      case 0x0e:
        bind_image((Fighter *)fighter_opponent(fighter), vm->image, fetch(fighter), 0);
        break;
      case 0x08:
      case 0x01:
      case 0x0c:
      case 0x02:
      case 0x09:
      case 0x11:
      case 0x0f: {
        bind(fighter, fetch(fighter), opcode == 9 ? -1 : (opcode == 0x08 || opcode == 0x0f || opcode == 0x11 || opcode == 0x0c) ? (int)arg : 0);
        if (opcode == 0x01 || opcode == 0x08) {
          play(fighter, fighter_move(fighter)->last, true);
        } else if (opcode == 0x0c) {
          int current = fighter_sample(fighter);
          play(fighter, current < fighter_move(fighter)->last ? current + 1 : current, false);
        } else if (opcode == 0x02) {
          play(fighter, (int)arg, false);
        } else if (opcode == 9) {
          play(fighter, 0, true);
        }
        break;
      }
      case 0x0d:
        sample(fighter, fighter_sample(fighter) + 1);
        break;
      case 0x46:
      case 0x68:
      case 0x69: {
        if (opcode != 0x69) {
          optional_bind(fighter, word);
        }
        if (opcode == 0x68) {
          sample(fighter, 0);
        }
        int last = fighter_move(fighter)->last;
        int target = opcode == 0x46 && arg != 0 ? fighter_sample(fighter) + (int)arg : last;
        slow_play(fighter, target > last ? last : target, opcode != 0x46 || arg == 0 || target > last);
        break;
      }
      case 0x71: {
        int source = fighter_move(fighter)->clip;
        int pose = fighter_sample(fighter);
        bind(fighter, fetch(fighter), 0);
        if (fighter_move(fighter)->last < 1) {
          vm_error(fighter, "blend needs sample one");
        }
        fighter->blend_clip = source;
        fighter->blend_sample = pose;
        fighter->blend_duration = 1;
        fighter->blend_weight = 0x4000;
        vm->blend_ticks = (Q16_ONE - 0x4000) / 0xa3d + 1;
        vm->wake = g_battle.ticks + 1;
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
      case 0x47:
      case 0x48: {
        if (opcode == 0x47 || opcode == 0x48) {
          optional_bind(fighter, word);
        }
        int target = opcode == 0x48 ? fighter_move(fighter)->last : opcode == 0x05 ? (int)(word & 0xffffffu) : (int)arg;
        if (opcode == 0x48 || target > fighter_sample(fighter)) {
          play(fighter, target, opcode == 0x48);
        }
        break;
      }
      case 0x4a:
        if ((int)arg < fighter_sample(fighter)) {
          play(fighter, (int)arg, false);
        }
        break;
      case 0x4c:
      case 0x4b:
        play(fighter, (int)arg, true);
        vm->paired = true;
        break;
      case 0x10:
      case 0x04:
      case 0x19:
      case 0x4d: {
        int current = fighter_sample(fighter);
        int target = opcode == 0x4d ? (int)arg : current + (int)arg;
        int last = fighter_move(fighter)->last;
        if (opcode != 0x4d && target > last) {
          target = last;
        }
        play(fighter, target, false);
        vm->paired = opcode == 0x10;
        if (opcode == 0x19 || opcode == 0x4d) {
          vm->stride = 2;
          vm->play_ticks = opcode == 0x19 ? ((int)arg + 1) / 2 : (target - current + 1) / 2;
        } else {
          vm->play_ticks = (int)arg;
        }
        break;
      }
      case 0x6a:
      case 0x03:
        play(fighter, fighter_move(fighter)->last, true);
        break;
      case 0x12:
      case 0x1d:
      case 0x49:
        if (opcode == 0x49) {
          optional_bind(fighter, word);
        } else if ((word & 0xffff00u) != 0) {
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
      case 0x4e:
        /* Native helpers receive the active fighter explicitly. */
        break;
      case 0x22:
        mk4_script_impact_callback(fighter);
        break;
      case 0x2e:
        fighter->flags |= FF_NO_GUARD;
        break;
      case 0x61:
        fighter->flags &= ~(unsigned)FF_BUSY;
        break;
      case 0x75:
        mk4_script_guard_block_sound_26(fighter);
        break;
      case 0x76:
        mk4_script_guard_block_sound_28(fighter);
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
        mk4_script_indexed_sound(fighter, (int)arg);
        break;
      case 0x26:
        mk4_script_context_sound(fighter, (int)arg);
        break;
      case 0x16:
        mk4_script_scene_active(fighter);
        break;
      case 0x28:
        mk4_script_impact(fighter);
        break;
      case 0x2a:
        mk4_script_shake(fighter, (int)arg);
        break;
      case 0x2c:
        mk4_script_draw_voice(fighter);
        break;
      case 0x4f:
        mk4_script_context(fighter, (int)arg);
        break;
      case 0x5f:
        fighter_set_speed(fighter, (q16)fetch(fighter));
        break;
      case 0x63:
        mk4_script_damage(fighter, (q16)fetch(fighter));
        break;
      case 0x6b:
        mk4_script_roster_sound(fighter, (int)arg);
        break;
      case 0x6d:
        mk4_script_pair_adjust(fighter, (q16)fetch(fighter));
        break;
      case 0x6e:
        mk4_script_disarm(fighter);
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
      case 0x56:
      case 0x64:
        if (!vm->native(fighter, opcode == 0x56 ? 0x45ac30 : 0x45b380, RETAIL_HANDLER, arg)) {
          vm_error(fighter, "unreviewed scene/facing binding");
        }
        break;
      case 0x41:
      case 0x1e:
        if (!vm->native(fighter, opcode == 0x41 ? 0x45b1f0 : 0x45b230, RETAIL_HANDLER, arg)) {
          vm_error(fighter, "unreviewed reaction binding");
        }
        break;
      case 0x55:
      case 0x36: {
        uint32_t address = fetch(fighter);
        uint32_t target = fetch(fighter);
        if (!vm->native(fighter, address, RETAIL_PREDICATE, 0)) {
          vm_error(fighter, "unreviewed predicate binding");
        }
        if (vm->result & (opcode == 0x36 ? 4u : 1u)) {
          jump(fighter, target);
        }
        break;
      }
      case 0x42: {
        uint32_t target = fetch(fighter);
        if (vm->result & 1u) {
          jump(fighter, target);
        }
        break;
      }
      case 0x38: {
        uint32_t address = fetch(fighter);
        if (!vm->native(fighter, address, RETAIL_PHYSICS, 0)) {
          vm_error(fighter, "unreviewed physics binding");
        }
        break;
      }
      case 0x39:
        fighter->body.physics = NULL;
        break;
      case 0x58:
        fighter_set_speed(fighter, (q16)fetch(fighter));
        fighter->body.vel[AXIS_Y] = (q16)fetch(fighter);
        fighter->body.gravity = (q16)fetch(fighter);
        break;
      case 0x59: {
        int left = (int32_t)fetch(fighter);
        int right = (int32_t)fetch(fighter);
        int up = (int32_t)fetch(fighter);
        int gravity = (int32_t)fetch(fighter);
        mk4_script_reaction_motion(fighter, left, right, up, gravity);
        break;
      }
      case 0x5c:
        fighter->body.gravity = (q16)fetch(fighter);
        break;
      case 0x6c: {
        int speed = (int32_t)fetch(fighter);
        mk4_script_schedule_motion(fighter, speed, (int)(word & 0xfffu), (int)((word >> 12) & 0xfffu));
        break;
      }
      case 0x5a:
        fighter_set_speed(fighter, (q16)(0u - fetch(fighter)));
        break;
      case 0x5b:
        mk4_script_stop(fighter);
        break;
      case 0x60:
        vm->wait_ground = fighter_airborne(fighter);
        if (vm->wait_ground) {
          return;
        }
        break;
      case 0x73:
        /* Native45c070 preserves VM scratch and selects a sampled token
         * from the low bit of the shared retail random word. */
        battle_sound(SOUND_TOKEN, battle_random(0x10000) & 1 ? 0x66d : 0x64a);
        break;
      case 0x3e:
        retail_vm_wait_native(fighter, 0, NULL);
        return;
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
      case 0x2f:
      case 0x44:
      case 0x45: {
        uint32_t packed = fetch(fighter);
        if (opcode == 0x2f ? fighter_airborne(fighter) : opcode == 0x44 ? (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0 : fighter_fast_start(fighter)) {
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
      case 0x52:
      case 0x53:
      case 0x5e:
        contact(fighter, opcode == 0x51 ? 0x53d2b8 : opcode == 0x52 ? 0x53d268 : opcode == 0x53 ? 0x53d178 : 0x53d0c0, arg);
        break;
      default:
        vm_error(fighter, "unsupported opcode");
    }
    if (!vm->active || vm->parked || fighter->pending != NULL) {
      return;
    }
    if (vm->play_ticks > 0 || vm->blend_ticks > 0 || g_battle.ticks < vm->wake) {
      return;
    }
  }
  vm_error(fighter, "instruction budget exhausted without yielding");
}
