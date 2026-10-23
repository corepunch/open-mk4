#include "script/native_ids.h"
#include "data/layout.h"
#include "script/vm.h"
#include "lua/script_lua.h"
#include "native/helpers.h"
#include "native/actions.h"

/* Actor script execution state. The Lua runtime decides which operation runs
 * next; this module owns playback, waits, contact results, pairing, native
 * continuations and the per-tick step that advances them. */

static void resume_script_return(Fighter *fighter);

void script_vm_wait_native(Fighter *fighter, unsigned ticks, FighterFn next) {
  require(fighter->execution.script.active, "Native wait without a live script");
  fighter->execution.script.parked = true;
  fighter->execution.script.native_resume = next;
  fighter->execution.script.wake = g_battle.ticks + ticks;
}

static _Noreturn void vm_error(const Fighter *fighter, const char *message) {
  fatal("Script %s: %s", script_lua_entry_name(fighter->execution.script.entry), message);
}

static void finish(Fighter *fighter) {
  FighterFn next = fighter->execution.script.finish;
  next(fighter);
}

void script_vm_return(Fighter *fighter) {
  require(fighter->execution.script.active, "Native return without a live script");
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
  if (packed < MK4_ANIMATION_TABLE / 4 || packed >= (MK4_ANIMATION_TABLE + MK4_CLIP_COUNT * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4 ||
      (packed * 4 - MK4_ANIMATION_TABLE) % MK4_ANIMATION_DESCRIPTOR_SIZE != 0) {
    vm_error(fighter, "invalid animation descriptor");
  }
  int clip = (int)((packed * 4 - MK4_ANIMATION_TABLE) / MK4_ANIMATION_DESCRIPTOR_SIZE);
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

/* Quarter-weight blend from the current pose into sample one of a new clip. */
static void blend_to(Fighter *fighter, uint32_t packed) {
  ScriptVm *vm = &fighter->execution.script;
  int source = fighter_move(fighter)->clip;
  int pose = fighter_sample(fighter);
  bind(fighter, packed, 0);
  if (fighter_move(fighter)->last < 1) {
    vm_error(fighter, "blend needs sample one");
  }
  fighter->blend_clip = source;
  fighter->blend_sample = pose;
  fighter->blend_duration = 1;
  fighter->blend_weight = 0x4000;
  vm->blend_ticks = (Q16_ONE - 0x4000) / 0xa3d + 1;
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
  require(address >= MK4_ATTACK_RECORDS_BEGIN && address <= MK4_ATTACK_RECORDS_END - MK4_ATTACK_RECORD_SIZE, "Attack record outside reviewed retail data");
  uint32_t reactions = mk4_image_u32(image, address + 16);
  return (AttackRecord){(q16)mk4_image_u32(image, address), (q16)mk4_image_u32(image, address + 4), (q16)mk4_image_u32(image, address + 8),
      (q16)mk4_image_u32(image, address + 12), reactions & 255u, (reactions >> 8) & 255u, (reactions >> 16) & 255u, reactions >> 24,
      (q16)mk4_image_u32(image, address + 20)};
}

static void contact(Fighter *fighter, uint32_t table, unsigned selector) {
  ScriptVm *vm = &fighter->execution.script;
  uint32_t packed = mk4_image_u32(vm->image, table + selector * 4);
  if (packed < MK4_ATTACK_RECORDS_BEGIN / 4 || packed > (MK4_ATTACK_RECORDS_END - MK4_ATTACK_RECORD_SIZE) / 4) {
    vm_error(fighter, "contact record outside reviewed data");
  }
  AttackRecord record = script_vm_attack_record(vm->image, packed * 4);
  uint64_t continuation = fighter->execution.generation;
  ContactResult result = battle_contact(fighter, &record);
  if (vm->active && fighter->execution.generation == continuation) {
    vm->result = result == CONTACT_MISS ? 0u : 1u;
  }
}

/* Start the script at a linked entry. Contact result, selector, playback
 * limit and native repetition state carry over from the previous script of
 * the same actor, as the retail task state did. */
void script_vm_start_at(Fighter *fighter, const Mk4Image *image, uint32_t entry, FighterFn next) {
  require(image != NULL && image->bytes.count != 0 && next != NULL, "Script start needs owned data and a typed continuation");
  require(entry >= SCRIPT_ADDRESS_BEGIN && entry < SCRIPT_ADDRESS_END && entry % 4 == 0, "Script entry outside the linked script section");
  ScriptVm previous = fighter->execution.script;
  fighter_execution_reset(fighter);
  fighter->execution.script = (ScriptVm){.image = image,
      .entry = entry,
      .active = true,
      .finish = next,
      .then = next == resume_script_return ? previous.then : (ScriptContinuation){0},
      .result = previous.result,
      .selector = previous.selector,
      .repeat_count = previous.repeat_count,
      .repeat_entry = previous.repeat_entry,
      .repeat_after = previous.repeat_after};
  fighter->execution.generation++;
  const MoveDef *move = fighter_move(fighter);
  if (move->clip >= 0 && move->last == MOVE_END) {
    bind(fighter, (MK4_ANIMATION_TABLE + (uint32_t)move->clip * MK4_ANIMATION_DESCRIPTOR_SIZE) / 4, fighter_sample(fighter));
  }
  script_lua_start(fighter, entry);
  script_vm_step(fighter);
}

void script_vm_enter(Fighter *fighter, uint32_t entry, FighterFn next) {
  script_vm_start_at(fighter, battle_data(), entry, next);
}

/* Script returns keep their own typed continuation; native addresses select
 * one implementation independently of actor/model identity. */
static void resume_script_return(Fighter *fighter) {
  ScriptContinuation next = fighter->execution.script.then;
  require(next.entry != 0, "Missing script return continuation");
  fighter->execution.script.then = (ScriptContinuation){0};
  script_vm_enter(fighter, next.entry, next.finish);
}

/* Native callers that enter a second script after an original return keep
 * that continuation in script ownership, independent of native frame completion. */
void script_vm_enter_then(Fighter *fighter, uint32_t entry, uint32_t after, FighterFn finish) {
  require(after >= SCRIPT_ADDRESS_BEGIN && after < SCRIPT_ADDRESS_END && after % 4 == 0 && finish != NULL, "Invalid script return continuation");
  fighter->execution.script.then = (ScriptContinuation){after, finish};
  script_vm_enter(fighter, entry, resume_script_return);
}

static void repeat_enter(Fighter *fighter, uint32_t entry) {
  ScriptVm *vm = &fighter->execution.script;
  if (vm->active) {
    script_vm_start_at(fighter, vm->image, entry, vm->finish);
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

/* One actor tick: advance engine-owned playback, blends and native waits;
 * when nothing owns the tick, resume the script for its next operations. */
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
  script_lua_step(fighter);
}

/* Playback primitives for the Lua operations; they keep the same bounds
 * checks and actor-owned state as the engine-side playback above. */
void script_vm_play(Fighter *fighter, int target, bool inclusive) {
  play(fighter, target, inclusive);
}

void script_vm_sample(Fighter *fighter, int pose) {
  sample(fighter, pose);
}

void script_vm_contact(Fighter *fighter, uint32_t table, unsigned selector) {
  contact(fighter, table, selector);
}

void script_vm_slow_play(Fighter *fighter, int target, bool inclusive) {
  slow_play(fighter, target, inclusive);
}

void script_vm_interpolated_play(Fighter *fighter, unsigned limit) {
  interpolated_play(fighter, limit);
}

void script_vm_blend(Fighter *fighter, uint32_t descriptor) {
  require(descriptor % 4 == 0, "Unaligned animation descriptor");
  blend_to(fighter, descriptor / 4);
}
