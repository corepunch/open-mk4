#include "script/native_ids.h"
#include "data/layout.h"
#include "script/vm.h"
#include "script/opcodes.h"
#include "mk4_program.h"
#include "native/helpers.h"
#include "native/actions.h"
#include <stdio.h>

static void resume_script_return(Fighter *fighter);

const char *script_vm_entry_name(uint32_t address) {
  static const struct {
    uint32_t address;
    const char *name;
  } names[] = {

#include "mk4_script_names.inc"
  };
  if (address < SCRIPT_ADDRESS_BEGIN || address >= SCRIPT_ADDRESS_END) {
    return "<no script>";
  }
  size_t low = 0, high = sizeof names / sizeof *names;
  while (low < high) {
    size_t middle = low + (high - low) / 2;
    if (names[middle].address <= address) {
      low = middle + 1;
    } else {
      high = middle;
    }
  }
  return low != 0 ? names[low - 1].name : "<unnamed script>";
}

void script_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next) {
  require(fighter->execution.script.active, "Native wait without a retail task");
  fighter->execution.script.parked = true;
  fighter->execution.script.native_resume = next;
  fighter->execution.script.wake = g_battle.ticks + ticks;
}

static _Noreturn void vm_error(const Fighter *fighter, const char *message) {
  fatal("Retail VM at %08x (%s, next %08x): %s", fighter->execution.script.instruction, script_vm_entry_name(fighter->execution.script.instruction),
      fighter->execution.script.pc, message);
}

static _Noreturn void vm_native_error(const Fighter *fighter, const char *reason, uint32_t reference) {
  char message[256];
  snprintf(message, sizeof message, "%s: %s (reference %u)", reason, script_native_name(reference), reference);
  vm_error(fighter, message);
}

static uint32_t fetch(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  if (vm->pc < vm->begin || vm->pc >= vm->end || vm->pc % 4 != 0) {
    vm_error(fighter, "script cursor outside reviewed range");
  }
  uint32_t word = mk4_program_word(vm->image, vm->pc);
  vm->pc += 4;
  return word;
}

static void jump(Fighter *fighter, uint32_t packed) {
  ScriptVm *vm = &fighter->execution.script;
  if (packed < vm->begin / 4 || packed >= vm->end / 4) {
    vm_error(fighter, "script branch outside reviewed range");
  }
  vm->pc = packed * 4;
}

static void finish(Fighter *fighter) {
  FighterFn next = fighter->execution.script.finish;
  next(fighter);
}

void script_vm_return(Fighter *fighter) {
  require(fighter->execution.script.active, "Native return without a retail task");
  finish(fighter);
}

void script_vm_reset(Fighter *fighter) {
  if (fighter->execution.script.active) {
    if (fighter->execution.script.blend_ticks > 0 || fighter->execution.script.interpolated_play) {
      fighter->blend_duration = 0;
      fighter->blend_weight = -1;
    }
    fighter->execution.script = (ScriptVm){0};
    fighter->avoids_high = false;
    fighter->execution.generation++;
  }
}

static void bind_image(Fighter *fighter, const Mk4Image *image, uint32_t packed, int first) {
  /* Descriptors are packed word addresses after the retail data fixups. */
  if (packed < dat_animation_table / 4 || packed >= (dat_animation_table + MK4_CLIP_COUNT * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4 ||
      (packed * 4 - dat_animation_table) % MK4_ANIMATION_DESCRIPTOR_SIZE != 0) {
    vm_error(fighter, "invalid animation descriptor");
  }
  int clip = (int)((packed * 4 - dat_animation_table) / MK4_ANIMATION_DESCRIPTOR_SIZE);
  uint32_t count = mk4_image_u32(image, packed * 4 + MK4_ANIMATION_FRAME_COUNT_OFFSET);
  if (count == 0 || count > 65535 || first >= (int)count) {
    vm_error(fighter, "invalid animation length or initial cursor");
  }
  fighter->playback_move = *fighter_move(fighter);
  fighter->playback_move.clip = clip;
  fighter->playback_move.first = 0;
  fighter->playback_move.last = (int)count - 1;
  fighter->playback_move.flags |= SF_RECORD;
  fighter->current_move = &fighter->playback_move;
  fighter->sample_override = first < 0 ? (int)count - 1 : first;
}

void script_vm_bind_animation(Fighter *fighter, uint32_t descriptor, int first) {
  require(descriptor % 4 == 0, "Unaligned animation descriptor");
  bind_image(fighter, fighter->execution.script.active ? fighter->execution.script.image : battle_data(), descriptor / 4, first);
}

static void bind(Fighter *fighter, uint32_t packed, int first) {
  bind_image(fighter, fighter->execution.script.image, packed, first);
}

static void sample(Fighter *fighter, int pose) {
  if (pose < 0 || pose > fighter_move(fighter)->last) {
    vm_error(fighter, "animation sample outside clip");
  }
  fighter->sample_override = pose;
}

static void play(Fighter *fighter, int target, bool inclusive) {
  ScriptVm *vm = &fighter->execution.script;
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
  ScriptVm *vm = &fighter->execution.script;
  vm->period = 2;
  vm->wake = g_battle.ticks + (target == fighter_sample(fighter) ? 1 : 2);
}

/* 45a070/45a0f0 ->45a260/45a140: full poses alternate with half-weight
 * adjacent-pose submissions, using one actor-owned source-frame budget. */
static void interpolated_play(Fighter *fighter, unsigned limit) {
  ScriptVm *vm = &fighter->execution.script;
  int first = fighter_sample(fighter), last = fighter_move(fighter)->last;
  if (first < 0 || last == MOVE_END || first > last) {
    vm_error(fighter, "interpolated playback needs a bounded cursor");
  }
  unsigned remaining = (unsigned)(last - first + 1);
  unsigned count = limit == 0 || limit > remaining ? remaining : limit;
  vm->playback_limit = limit;
  vm->target = first + (int)count - 1;
  vm->play_ticks = 0;
  vm->interpolated_play = true;
  vm->interpolated_pair = false;
  vm->wake = g_battle.ticks + 1;
}

void script_vm_reverse_return(Fighter *fighter) {
  play(fighter, 0, true);
  fighter->execution.script.return_after_play = true;
}

void script_vm_forward_return(Fighter *fighter) {
  play(fighter, fighter_move(fighter)->last, true);
  fighter->execution.script.return_after_play = true;
}

AttackRecord script_vm_attack_record(const Mk4Image *image, uint32_t address) {
  require(address >= dat_attack_records_begin && address <= dat_attack_records_end - MK4_ATTACK_RECORD_SIZE, "Attack record outside reviewed retail data");
  uint32_t reactions = mk4_image_u32(image, address + 16);
  return (AttackRecord){(q16)mk4_image_u32(image, address), (q16)mk4_image_u32(image, address + 4), (q16)mk4_image_u32(image, address + 8),
      (q16)mk4_image_u32(image, address + 12), reactions & 255u, (reactions >> 8) & 255u, (reactions >> 16) & 255u, reactions >> 24,
      (q16)mk4_image_u32(image, address + 20)};
}

static void contact(Fighter *fighter, uint32_t table, unsigned selector) {
  ScriptVm *vm = &fighter->execution.script;
  uint32_t packed = mk4_image_u32(vm->image, table + selector * 4);
  if (packed < dat_attack_records_begin / 4 || packed > (dat_attack_records_end - MK4_ATTACK_RECORD_SIZE) / 4) {
    vm_error(fighter, "contact record outside reviewed data");
  }
  AttackRecord record = script_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->execution.generation;
  ContactResult result = battle_contact(fighter, &record);
  if (vm->active && fighter->execution.generation == continuation) {
    vm->result = result == CONTACT_MISS ? 0u : 1u;
  }
}

void script_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, uint32_t begin, uint32_t end, FighterFn next, ScriptNativeFn native) {
  require(image != NULL && image->bytes.count != 0 && next != NULL && native != NULL, "Retail VM needs owned image and typed continuations");
  require(begin <= entry && entry < end && begin % 4 == 0 && end % 4 == 0 && end - begin <= SCRIPT_ADDRESS_END - SCRIPT_ADDRESS_BEGIN,
      "Invalid reviewed retail script range");
  ScriptVm previous = fighter->execution.script;
  fighter_execution_reset(fighter);
  fighter->execution.script = (ScriptVm){.image = image,
      .entry = entry,
      .pc = entry,
      .begin = begin,
      .end = end,
      .active = true,
      .finish = next,
      .then = next == resume_script_return ? previous.then : (ScriptContinuation){0},
      .dispatch = native,
      .loop_count = previous.loop_count,
      .result = previous.result,
      .selector = previous.selector,
      .repeat_count = previous.repeat_count,
      .repeat_entry = previous.repeat_entry,
      .repeat_after = previous.repeat_after};
  fighter->execution.generation++;
  const MoveDef *move = fighter_move(fighter);
  if (move->clip >= 0 && move->last == MOVE_END) {
    bind(fighter, (dat_animation_table + (uint32_t)move->clip * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4, fighter_sample(fighter));
  }
  script_vm_step(fighter);
}

void script_vm_enter(Fighter *fighter, uint32_t entry, FighterFn next) {
  script_vm_start_at(fighter, battle_data(), entry, SCRIPT_ADDRESS_BEGIN, SCRIPT_ADDRESS_END, next, script_native_dispatch);
}

/* Script returns keep their own typed continuation; native addresses select
 * one implementation independently of actor/model identity. */
static void resume_script_return(Fighter *fighter) {
  ScriptContinuation next = fighter->execution.script.then;
  require(next.entry != 0, "Missing retail script return continuation");
  fighter->execution.script.then = (ScriptContinuation){0};
  script_vm_enter(fighter, next.entry, next.finish);
}

/* Native callers that enter a second script after an original return keep
 * that continuation in VM ownership, independent of native frame completion. */
void script_vm_enter_then(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish) {
  require(after >= SCRIPT_ADDRESS_BEGIN && after < SCRIPT_ADDRESS_END && after % 4 == 0 && finish != NULL, "Invalid retail script return continuation");
  fighter->execution.script.then = (ScriptContinuation){after, finish};
  script_vm_enter(fighter, entry, resume_script_return);
}

static void repeat_enter(Fighter *fighter, uint32_t entry) {
  ScriptVm *vm = &fighter->execution.script;
  if (vm->active) {
    script_vm_start_at(fighter, vm->image, entry, vm->begin, vm->end, vm->finish, vm->dispatch);
  } else {
    script_vm_enter(fighter, entry, fighter_ready);
  }
}

void script_vm_repeat(Fighter *fighter, uint32_t entry, unsigned count, uint32_t after) {
  require(count > 0, "Native script repetition must be bounded");
  fighter->execution.script.repeat_count = count;
  fighter->execution.script.repeat_entry = entry;
  fighter->execution.script.repeat_after = after;
  repeat_enter(fighter, entry);
}

void script_vm_repeat_next(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
  require(vm->active && vm->repeat_count > 0, "Native script cycle without a live counter");
  uint32_t next = --vm->repeat_count == 0 ? vm->repeat_after : vm->repeat_entry;
  repeat_enter(fighter, next);
}

void script_vm_start(Fighter *fighter, const Mk4Image *image, uint32_t begin, uint32_t end, FighterFn next, ScriptNativeFn native) {
  script_vm_start_at(fighter, image, begin, begin, end, next, native);
}

void script_vm_step(Fighter *fighter) {
  ScriptVm *vm = &fighter->execution.script;
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
  if (vm->interpolated_play) {
    int current = fighter_sample(fighter);
    fighter->blend_duration = 0;
    fighter->blend_weight = -1;
    if (current >= vm->target) {
      vm->interpolated_play = false;
      vm->interpolated_pair = false;
    } else {
      if (vm->interpolated_pair) {
        sample(fighter, current + 1);
        vm->interpolated_pair = false;
      } else {
        int first = current + 1;
        int second = first < vm->target ? first + 1 : first;
        sample(fighter, second);
        vm->interpolated_pair = first != second;
        if (vm->interpolated_pair) {
          fighter->blend_clip = fighter_move(fighter)->clip;
          fighter->blend_sample = first;
          fighter->blend_duration = 1;
          fighter->blend_weight = 0x8000;
        }
      }
      vm->wake = g_battle.ticks + 1;
      return;
    }
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
      case MK4_OP_NOP:
      case MK4_OP_NOP_0B:
      case MK4_OP_NOP_15:
      case MK4_OP_NOP_18:
      case MK4_OP_NOP_21:
      case MK4_OP_NOP_23:
      case MK4_OP_NOP_57:
      case MK4_OP_NOP_66:
      case MK4_OP_NOP_67:
        /* These loaded dispatch slots point directly back to4594b0. */
        break;
      case MK4_OP_OPEN_PEER_CLIP:
        bind_image((Fighter *)fighter_opponent(fighter), vm->image, fetch(fighter), 0);
        break;
      case MK4_OP_CLIP_ALT:
      case MK4_OP_CLIP:
      case MK4_OP_BIND_SUBMIT:
      case MK4_OP_PLAY_CLIP:
      case MK4_OP_CLIP_OPERATION:
      case MK4_OP_BIND_CURSOR:
      case MK4_OP_OPEN_CLIP: {
        int first = 0;
        if (opcode == MK4_OP_CLIP_OPERATION) {
          first = -1;
        } else if (opcode == MK4_OP_CLIP_ALT || opcode == MK4_OP_OPEN_CLIP || opcode == MK4_OP_BIND_CURSOR || opcode == MK4_OP_BIND_SUBMIT) {
          first = (int)arg;
        }
        bind(fighter, fetch(fighter), first);
        if (opcode == MK4_OP_CLIP || opcode == MK4_OP_CLIP_ALT) {
          play(fighter, fighter_move(fighter)->last, true);
        } else if (opcode == MK4_OP_BIND_SUBMIT) {
          int current = fighter_sample(fighter);
          play(fighter, current < fighter_move(fighter)->last ? current + 1 : current, false);
        } else if (opcode == MK4_OP_PLAY_CLIP) {
          play(fighter, (int)arg, false);
        } else if (opcode == MK4_OP_CLIP_OPERATION) {
          play(fighter, 0, true);
        }
        break;
      }
      case MK4_OP_PLAY_DOUBLE: {
        bind(fighter, fetch(fighter), 0);
        int last = fighter_move(fighter)->last;
        sample(fighter, last > 0 ? 1 : 0);
        play(fighter, last, true);
        vm->stride = 2;
        vm->play_ticks = (last - fighter_sample(fighter) + 1) / 2 + 1;
        break;
      }
      case MK4_OP_PLAY_INTERLEAVED:
        bind(fighter, fetch(fighter), 0);
        interpolated_play(fighter, 200);
        break;
      case MK4_OP_RETURN_WORD:
        vm->playback_limit = arg;
        vm->returned_word = fetch(fighter);
        finish(fighter);
        return;
      case MK4_OP_INHERITED_INTERLEAVED:
        interpolated_play(fighter, arg);
        break;
      case MK4_OP_CURSOR_ADVANCE:
        sample(fighter, fighter_sample(fighter) + (int)arg);
        break;
      case MK4_OP_CURSOR_LAST:
      case MK4_OP_SUBMIT_LAST:
        /* The engine owns the sampled pose directly; native429970's pose
         * submission does not require a separate retail render command. */
        sample(fighter, fighter_move(fighter)->last);
        break;
      case MK4_OP_PLAYBACK_NATIVE:
        sample(fighter, fighter_sample(fighter) + 1);
        break;
      case MK4_OP_HALF_COUNT:
      case MK4_OP_HALF_RESTART:
      case MK4_OP_HALF_FINISH: {
        if (opcode != MK4_OP_HALF_FINISH) {
          optional_bind(fighter, word);
        }
        if (opcode == MK4_OP_HALF_RESTART) {
          sample(fighter, 0);
        }
        int last = fighter_move(fighter)->last;
        int target = opcode == MK4_OP_HALF_COUNT && arg != 0 ? fighter_sample(fighter) + (int)arg : last;
        slow_play(fighter, target > last ? last : target, opcode != MK4_OP_HALF_COUNT || arg == 0 || target > last);
        break;
      }
      case MK4_OP_BLEND: {
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
      case MK4_OP_BACKWARD_COUNT: {
        int target = fighter_sample(fighter) - (int)(word & 0xffffffu);
        play(fighter, target < 0 ? 0 : target, false);
        break;
      }
      case MK4_OP_BACKWARD:
        play(fighter, 0, true);
        break;
      case MK4_OP_FORWARD_TO:
      case MK4_OP_PREFIXED_TO:
      case MK4_OP_PREFIXED_FINISH: {
        if (opcode == MK4_OP_PREFIXED_TO || opcode == MK4_OP_PREFIXED_FINISH) {
          optional_bind(fighter, word);
        }
        int target = opcode == MK4_OP_PREFIXED_FINISH ? fighter_move(fighter)->last : opcode == MK4_OP_FORWARD_TO ? (int)(word & 0xffffffu) : (int)arg;
        if (opcode == MK4_OP_PREFIXED_FINISH || target > fighter_sample(fighter)) {
          play(fighter, target, opcode == MK4_OP_PREFIXED_FINISH);
        }
        break;
      }
      case MK4_OP_PLAYBACK_PAIR:
        if ((int)arg < fighter_sample(fighter)) {
          play(fighter, (int)arg, false);
        }
        break;
      case MK4_OP_PLAYBACK_PAIR_REVERSE:
      case MK4_OP_PLAYBACK_SINGLE:
        play(fighter, (int)arg, true);
        vm->paired = true;
        break;
      case MK4_OP_PLAY_PAIR:
      case MK4_OP_FORWARD_COUNT:
      case MK4_OP_DOUBLE_COUNT:
      case MK4_OP_DOUBLE_TO: {
        int current = fighter_sample(fighter);
        int target = opcode == MK4_OP_DOUBLE_TO ? (int)arg : current + (int)arg;
        int last = fighter_move(fighter)->last;
        if (opcode != MK4_OP_DOUBLE_TO && target > last) {
          target = last;
        }
        play(fighter, target, false);
        vm->paired = opcode == MK4_OP_PLAY_PAIR;
        if (opcode == MK4_OP_DOUBLE_COUNT || opcode == MK4_OP_DOUBLE_TO) {
          vm->stride = 2;
          vm->play_ticks = opcode == MK4_OP_DOUBLE_COUNT ? ((int)arg + 1) / 2 : (target - current + 1) / 2;
        } else {
          vm->play_ticks = (int)arg;
        }
        break;
      }
      case MK4_OP_NATIVE_SOUND:
      case MK4_OP_FORWARD:
        play(fighter, fighter_move(fighter)->last, true);
        break;
      case MK4_OP_CURSOR_SET:
      case MK4_OP_PLAYBACK_TAIL:
      case MK4_OP_PREFIXED_SAMPLE:
        if (opcode == MK4_OP_PREFIXED_SAMPLE) {
          optional_bind(fighter, word);
        } else if ((word & 0xffff00u) != 0) {
          vm_error(fighter, "unreviewed sample prefix");
        }
        sample(fighter, (int)arg);
        break;
      case MK4_OP_ATTACK_SELECTOR:
        vm->selector = fetch(fighter);
        break;
      case MK4_OP_SOUND_A:
      case MK4_OP_SOUND_C:
      case MK4_OP_SOUND_B:
        battle_sound(opcode == MK4_OP_SOUND_A ? SOUND_A : opcode == MK4_OP_SOUND_C ? SOUND_C : SOUND_B, (int)arg);
        break;
      case MK4_OP_REACTION_CONTEXT:
        /* Native helpers receive the active fighter explicitly. */
        break;
      case MK4_OP_TASK_CALLBACK:
        mk4_script_impact_callback(fighter);
        break;
      case MK4_OP_NO_GUARD:
        fighter->flags |= FF_NO_GUARD;
        break;
      case MK4_OP_ALLOW_COMMANDS:
        fighter->flags &= ~(unsigned)FF_BUSY;
        break;
      case MK4_OP_BLOCK_SOUND:
        mk4_script_guard_block_sound_26(fighter);
        break;
      case MK4_OP_BLOCK_SOUND_ALT:
        mk4_script_guard_block_sound_28(fighter);
        break;
      case MK4_OP_RELEASE_ATTACK:
        mk4_script_attack_return(fighter);
        break;
      case MK4_OP_ACTION_RECOVERY:
        fighter->action = ACTION_CODE_1006;
        break;
      case MK4_OP_ACTION_FOLLOWUP:
        fighter->action = ACTION_CODE_1007;
        break;
      case MK4_OP_READY:
      case MK4_OP_RETURN:
        /* These roots entered from the controller; this is their explicit
         * return continuation, not a general retail tagged-stack emulation. */
        finish(fighter);
        return;
      case MK4_OP_TAIL:
      case MK4_OP_CALL: {
        uint32_t address = fetch(fighter);
        if (!vm->dispatch(fighter, address, opcode == MK4_OP_TAIL ? MK4_TAIL : MK4_CALL, 0)) {
          vm_native_error(fighter, "unreviewed native binding", address);
        }
        break;
      }
      case MK4_OP_SOUND_INDEXED:
        mk4_script_indexed_sound(fighter, (int)arg);
        break;
      case MK4_OP_SOUND_CONTEXT:
        mk4_script_context_sound(fighter, (int)arg);
        break;
      case MK4_OP_SCENE_ACTIVE:
        mk4_script_scene_active(fighter);
        break;
      case MK4_OP_SOUND_SPECIAL:
        mk4_script_impact(fighter);
        break;
      case MK4_OP_SPAWN_EFFECT:
        mk4_script_shake(fighter, (int)arg);
        break;
      case MK4_OP_SOUND_VARIANT:
        mk4_script_draw_voice(fighter);
        break;
      case MK4_OP_NATIVE_RELEASE:
        mk4_script_context(fighter, (int)arg);
        break;
      case MK4_OP_SPEED:
        fighter_set_speed(fighter, (q16)fetch(fighter));
        break;
      case MK4_OP_DAMAGE:
        mk4_script_damage(fighter, (q16)fetch(fighter));
        break;
      case MK4_OP_SOUND_ROSTER:
        mk4_script_roster_sound(fighter, (int)arg);
        break;
      case MK4_OP_NATIVE_WORD:
        mk4_script_pair_adjust(fighter, (q16)fetch(fighter));
        break;
      case MK4_OP_DISARM_PEER:
        mk4_script_disarm(fighter);
        break;
      case MK4_OP_EFFECT:
        mk4_script_effect(fighter, (int)arg);
        break;
      case MK4_OP_SPAWN: {
        unsigned type = fetch(fighter);
        uint32_t address = fetch(fighter);
        if (!vm->dispatch(fighter, address, MK4_SPAWN, type)) {
          vm_native_error(fighter, "unreviewed task binding", address);
        }
        break;
      }
      case MK4_OP_SCENE_EVENT:
      case MK4_OP_FACE:
        if (!vm->dispatch(fighter, opcode == MK4_OP_SCENE_EVENT ? native_context : native_face_peer_and_resume_script, MK4_HANDLER, arg)) {
          vm_error(fighter, "unreviewed scene/facing binding");
        }
        break;
      case MK4_OP_NATIVE_RETURN:
      case MK4_OP_NATIVE_STEP:
        if (!vm->dispatch(fighter, opcode == MK4_OP_NATIVE_RETURN ? native_script_reaction_dispatch : native_dispatch_opponent_reaction, MK4_HANDLER, arg)) {
          vm_error(fighter, "unreviewed reaction binding");
        }
        break;
      case MK4_OP_PREDICATE_BIT:
      case MK4_OP_PREDICATE_ARGUMENT:
      case MK4_OP_PREDICATE_FLAGS: {
        uint32_t address = fetch(fighter);
        uint32_t target = fetch(fighter);
        uint64_t continuation = fighter->execution.generation;
        if (!vm->dispatch(fighter, address, MK4_PREDICATE, opcode == MK4_OP_PREDICATE_ARGUMENT ? arg : 0)) {
          vm_native_error(fighter, "unreviewed predicate binding", address);
        }
        if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
          return;
        }
        if (vm->result & (opcode == MK4_OP_PREDICATE_FLAGS ? 4u : 1u)) {
          jump(fighter, target);
        }
        break;
      }
      case MK4_OP_EDGE: {
        uint32_t target = fetch(fighter);
        bool outer = action_outer_edge(fighter);
        vm->result = (vm->result & ~1u) | (outer ? 1u : 0u);
        if (outer) {
          jump(fighter, target);
        }
        break;
      }
      case MK4_OP_REACTION_DISTANCE: {
        q16 limit = (q16)fetch(fighter);
        if (g_battle.distance <= limit) {
          uint64_t continuation = fighter->execution.generation;
          if (!vm->dispatch(fighter, native_script_reaction_dispatch, MK4_HANDLER, arg)) {
            vm_error(fighter, "unreviewed distance reaction binding");
          }
          if (!vm->active || fighter->execution.generation != continuation || fighter->pending != NULL) {
            return;
          }
          vm->result |= 1u;
        } else {
          vm->result &= ~1u;
        }
        break;
      }
      case MK4_OP_BLOCKED: {
        uint32_t target = fetch(fighter);
        if (fighter->contact == CONTACT_BLOCK) {
          jump(fighter, target);
        }
        break;
      }
      case MK4_OP_QUERY_BRANCH: {
        uint32_t target = fetch(fighter);
        uint64_t continuation = fighter->execution.generation;
        contact(fighter, dat_contact_query_b_table, arg);
        if (vm->active && fighter->execution.generation == continuation && (vm->result & 1u)) {
          jump(fighter, target);
        }
        break;
      }
      case MK4_OP_CONTACT: {
        uint32_t target = fetch(fighter);
        if (vm->result & 1u) {
          jump(fighter, target);
        }
        break;
      }
      case MK4_OP_HANDLER: {
        uint32_t address = fetch(fighter);
        if (!vm->dispatch(fighter, address, MK4_PHYSICS, 0)) {
          vm_native_error(fighter, "unreviewed physics binding", address);
        }
        break;
      }
      case MK4_OP_CLEAR_HANDLER:
        fighter->body.physics = NULL;
        break;
      case MK4_OP_PEER_MOTION: {
        q16 speed = (q16)fetch(fighter);
        q16 up = (q16)fetch(fighter);
        q16 gravity = (q16)fetch(fighter);
        action_directional_motion(fighter, speed, up, gravity);
        break;
      }
      case MK4_OP_EDGE_PUSH:
        action_edge_push(fighter);
        break;
      case MK4_OP_POSITION: {
        q16 horizontal = (q16)fetch(fighter);
        q16 vertical = (q16)fetch(fighter);
        fighter->body.pos[AXIS_X] += q16_mul(fighter_axis(fighter, AXIS_X), horizontal);
        fighter->body.pos[AXIS_Z] += q16_mul(fighter_axis(fighter, AXIS_Z), horizontal);
        fighter->body.pos[AXIS_Y] += vertical;
        break;
      }
      case MK4_OP_SOUND_TOKEN:
        battle_sound(SOUND_TOKEN, 0x627);
        break;
      case MK4_OP_MOTION:
        fighter_set_speed(fighter, (q16)fetch(fighter));
        fighter->body.vel[AXIS_Y] = (q16)fetch(fighter);
        fighter->body.gravity = (q16)fetch(fighter);
        break;
      case MK4_OP_MOTION_BLOCK: {
        int left = (int32_t)fetch(fighter);
        int right = (int32_t)fetch(fighter);
        int up = (int32_t)fetch(fighter);
        int gravity = (int32_t)fetch(fighter);
        mk4_script_reaction_motion(fighter, left, right, up, gravity);
        break;
      }
      case MK4_OP_GRAVITY:
        fighter->body.gravity = (q16)fetch(fighter);
        break;
      case MK4_OP_SCHEDULE_MOTION: {
        int speed = (int32_t)fetch(fighter);
        mk4_script_schedule_motion(fighter, speed, (int)(word & 0xfffu), (int)((word >> 12) & 0xfffu));
        break;
      }
      case MK4_OP_SPEED_NEGATIVE:
        fighter_set_speed(fighter, (q16)(0u - fetch(fighter)));
        break;
      case MK4_OP_LANDING:
        mk4_script_stop(fighter);
        break;
      case MK4_OP_NATIVE_CONTEXT:
        vm->wait_ground = fighter_airborne(fighter);
        if (vm->wait_ground) {
          return;
        }
        break;
      case MK4_OP_SOUND_RANDOM:
        /* Native45c070 preserves VM scratch and selects a sampled token
         * from the low bit of the shared retail random word. */
        battle_sound(SOUND_TOKEN, battle_random(0x10000) & 1 ? 0x66d : 0x64a);
        break;
      case MK4_OP_PARK:
        script_vm_wait_native(fighter, 0, NULL);
        return;
      case MK4_OP_JUMP:
        jump(fighter, fetch(fighter));
        break;
      case MK4_OP_SELECT_ROSTER: {
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
      case MK4_OP_AIRBORNE:
      case MK4_OP_DOWN:
      case MK4_OP_FAST_START: {
        uint32_t packed = fetch(fighter);
        if (opcode == MK4_OP_AIRBORNE ? fighter_airborne(fighter)
            : opcode == MK4_OP_DOWN   ? (fighter->held & BUTTON_BIT(BUTTON_DOWN)) != 0
                                      : fighter_fast_start(fighter)) {
          jump(fighter, packed);
        }
        break;
      }
      case MK4_OP_WAIT:
        vm->wake = g_battle.ticks + (word & 0xffffffu);
        break;
      case MK4_OP_WAIT_RESULT:
      case MK4_OP_WAIT_BRANCH:
        vm->wake = g_battle.ticks + ((vm->result & 1u) ? (word >> 12) & 0xfffu : word & 0xfffu);
        break;
      case MK4_OP_LOOP:
        if (arg == 0) {
          vm_error(fighter, "zero loop counter");
        }
        vm->loop_count = arg;
        break;
      case MK4_OP_REPEAT: {
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
          } while (mk4_program_word(vm->image, marker) >> 24 != MK4_OP_LOOP);
          vm->pc = marker + 4;
        }
        break;
      }
      case MK4_OP_QUERY_B:
      case MK4_OP_QUERY_A:
      case MK4_OP_QUERY_D:
      case MK4_OP_WEAPON_QUERY:
        contact(fighter,
            opcode == MK4_OP_QUERY_B   ? dat_contact_query_b_table
            : opcode == MK4_OP_QUERY_A ? dat_contact_query_a_table
            : opcode == MK4_OP_QUERY_D ? dat_contact_query_d_table
                                       : dat_weapon_attack_table,
            arg);
        break;
      default:
        vm_error(fighter, "unsupported opcode");
    }
    if (!vm->active || vm->parked || fighter->pending != NULL) {
      return;
    }
    if (vm->play_ticks > 0 || vm->blend_ticks > 0 || vm->interpolated_play || g_battle.ticks < vm->wake) {
      return;
    }
  }
  vm_error(fighter, "instruction budget exhausted without yielding");
}
