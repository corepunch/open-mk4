#include "game.h"
#include "retail_vm.h"
#include "retail_data.h"
#include "retail_native.h"
#include "native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Mk4Image image;

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL retail VM: %s\n", message);
    exit(1);
  }
}

static void tick(void) {
  Input input[2] = {0};
  battle_step(input);
}

static Fighter *setup(int slot, int scenario) {
  battle_init(slot ? 4 : 0, slot ? 0 : 4);
  g_battle.phase = PHASE_FIGHT;
  g_battle.retail_scripts = &image;
  Fighter *owner = &g_battle.fighters[slot];
  Fighter *victim = &g_battle.fighters[1 - slot];
  q16 distance = scenario == 2 ? 6 * Q16_ONE : 0xc000;
  owner->body.pos[AXIS_X] = slot ? distance / 2 : -distance / 2;
  victim->body.pos[AXIS_X] = -owner->body.pos[AXIS_X];
  return owner;
}

static void press(int slot, bool crouch, int scenario) {
  Input input[2] = {0};
  input[slot].held = BUTTON_BIT(crouch ? BUTTON_HIGH_KICK : BUTTON_LOW_KICK);
  if (crouch) {
    input[slot].held |= BUTTON_BIT(BUTTON_DOWN);
  }
  if (scenario == 1) {
    input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
  }
  battle_step(input);
  input[slot].held = 0;
  battle_step(input);
}

static int recover(Fighter *owner, int slot, bool crouch, int scenario) {
  int ticks = 0;
  while (owner->retail_vm.active && ticks < 100) {
    check(owner->current_mmove == NULL && owner->sequence == NULL, "VM must run without a generated frame/sequence program");
    check(fighter_sample(owner) >= 0 && fighter_sample(owner) <= fighter_move(owner)->last, "VM pose outside decoded clip");
    Input input[2] = {0};
    if (scenario == 1) {
      input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
    }
    battle_step(input);
    ticks++;
  }
  check(ticks < 100 && !owner->retail_vm.active && !owner->avoids_high, "VM must finish and retire avoidance");
  check(owner->state == S_IDLE && owner->buttons != NULL, "VM must return to the real stance controller");
  Fighter *victim = &g_battle.fighters[1 - slot];
  q16 damage = crouch ? 0x1eb8 : 0x1c28;
  damage = scenario == 2 ? 0 : scenario == 1 ? q16_mul(damage, 0x3333) : damage;
  if (victim->health != Q16_ONE - damage) {
    fprintf(stderr, "slot=%d crouch=%d scenario=%d health=%d expected=%d\n", slot, crouch, scenario, victim->health, Q16_ONE - damage);
  }
  check(victim->health == Q16_ONE - damage, "retail contact must apply hit/block/miss damage exactly once");
  return ticks;
}

static void put_word(uint32_t address, uint32_t word) {
  for (int byte = 0; byte < 4; ++byte) {
    image.bytes.data[address - 0x400000 + (unsigned)byte] = (uint8_t)(word >> (byte * 8));
  }
}

static void behavior(void) {
  for (int slot = 0; slot < 2; ++slot) {
    for (int crouch = 0; crouch < 2; ++crouch) {
      int recovery[3];
      for (int scenario = 0; scenario < 3; ++scenario) {
        Fighter *owner = setup(slot, scenario);
        press(slot, crouch != 0, scenario);
        check(owner->retail_vm.active, "real button entry must select the runtime interpreter");
        check(owner->retail_vm.entry == (crouch ? 0x4e6020u : 0x4e5d90u), "button entry selected the wrong retail root");
        recovery[scenario] = recover(owner, slot, crouch != 0, scenario);
      }
      if (!crouch) {
        check(recovery[0] == recovery[1] && recovery[0] > recovery[2], "contact-result wait must distinguish contact from miss");
      }
    }
  }
}

static void runtime_words(void) {
  Fighter *owner = setup(0, 2);
  press(0, true, 2);
  int original = recover(owner, 0, true, 2);
  uint32_t loop = mk4_image_u32(&image, 0x4e60d8);
  check(loop == 0x3a000002, "retail crouch kick loop evidence changed");
  put_word(0x4e60d8, 0x3a000003);
  owner = setup(0, 2);
  press(0, true, 2);
  int extended = recover(owner, 0, true, 2);
  put_word(0x4e60d8, loop);
  check(extended == original + 8, "one extra source loop must add its four two-tick waits without regenerating C");
}

static void lifetimes(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Fighter *owner = setup(slot, 2);
    press(slot, false, 2);
    for (int n = 0; n < 8; ++n) {
      tick();
    }
    check(owner->avoids_high && owner->retail_vm.active, "low kick must enter its high-punch avoidance window");
    fighter_interrupt(owner, fighter_ready);
    check(!owner->retail_vm.active && !owner->avoids_high, "interrupt must immediately cancel interpreter and avoidance");
    for (int n = 0; n < 60; ++n) {
      tick();
    }
    check(owner->state == S_IDLE, "cancelled VM must not resume its native tail");
    owner = setup(slot, 0);
    press(slot, false, 0);
    g_battle.fighters[1 - slot].health = 1;
    for (int n = 0; n < 60; ++n) {
      tick();
    }
    check(g_battle.fighters[1 - slot].health == 0 && (!owner->retail_vm.active || (owner->state == S_VICTORY && owner->retail_vm.entry == 0x4e6a40)),
        "KO must retire attack ownership before entering the original victory program");
    owner = setup(slot, 2);
    press(slot, true, 2);
    battle_reset_match();
    check(!owner->retail_vm.active && !owner->avoids_high, "match reset must retire live interpreter state");
    g_battle.phase = PHASE_FIGHT;
    for (int n = 0; n < 60; ++n) {
      tick();
    }
    check(!owner->retail_vm.active && owner->state == S_IDLE, "reset VM must stay retired");
  }
}

static int control_finished;

static void control_end(Fighter *fighter) {
  control_finished++;
  retail_vm_cancel(fighter);
}

static bool reject_native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  (void)fighter;
  (void)address;
  (void)kind;
  (void)argument;
  return false;
}

static void control_flow(void) {
  /* One shared-runtime fixture: counted playback, absolute jump and packed
   * character selection. It tests our cursor implementation, not a fighter. */
  Fighter *owner = setup(0, 2);
  static const MoveDef locked = {.clip = 54, .last = 12, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(owner, S_KICK, &locked);
  const uint32_t base = 0x570000;
  const uint32_t words[] = {0x0f000000, (0x4fad7c + 54 * 12) / 4, 0x3a000002, 0x04000001, 0x3b000000, 0x33000000, (base + 32) / 4, 0xff000000, 0x3c000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + i * 4, words[i]);
  }
  for (unsigned i = 0; i < 16; ++i) {
    put_word(base + 36 + i * 4, (base + 100) / 4);
  }
  put_word(base + 100, 0x06000001);
  put_word(base + 104, 0x07000000);
  put_word(base + 108, 0x30000000);
  control_finished = 0;
  retail_vm_start(owner, &image, base, base + 112, control_end, reject_native);
  for (int n = 0; n < 10 && owner->retail_vm.active; ++n) {
    tick();
  }
  check(control_finished == 1 && fighter_sample(owner) == 0, "runtime must jump over invalid data, select packed streams and finish reverse playback");
}

static void playback_modes(void) {
  /* Shared cursor/clock contract, independent of any authored fighter move. */
  Fighter *owner = setup(0, 2);
  static const MoveDef locked = {.clip = 54, .last = 12, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(owner, S_KICK, &locked);
  const uint32_t base = 0x570000;
  const uint32_t descriptor = (0x4fad7c + 54 * 12) / 4;
  uint32_t words[] = {0x49000202, descriptor, 0x46000002, 0x0d000000, 0x4a000001, 0x71000000, descriptor, 0x30000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + i * 4, words[i]);
  }
  control_finished = 0;
  retail_vm_start(owner, &image, base, base + sizeof words, control_end, reject_native);
  check(fighter_sample(owner) == 2, "optional sample descriptor must consume its operand before half-speed playback");
  tick();
  check(fighter_sample(owner) == 2, "half-speed playback must hold the first visit");
  for (int i = 0; i < 50 && owner->retail_vm.active; ++i) {
    check(owner->current_mmove == NULL, "playback and blend must not create generated frame programs");
    tick();
  }
  check(control_finished == 1 && fighter_sample(owner) == 1 && owner->blend_duration == 0 && owner->blend_weight == -1,
      "reverse-to and blend must resume the script at sample one and retire blending");
  put_word(base, 0x71000000);
  put_word(base + 4, descriptor);
  put_word(base + 8, 0x30000000);
  retail_vm_start(owner, &image, base, base + 12, control_end, reject_native);
  fighter_interrupt(owner, fighter_ready);
  check(!owner->retail_vm.active && owner->blend_duration == 0, "interruption must retire a VM-owned blend");
}

static bool repeat_native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  (void)argument;
  if (kind != RETAIL_TAIL || (address != 1 && address != 2)) {
    return false;
  }
  if (address == 1) {
    retail_vm_repeat(fighter, 0x570008, 3, 0x570014);
  } else {
    retail_vm_repeat_next(fighter);
  }
  return true;
}

static void native_repetition(void) {
  Fighter *owner = setup(0, 2);
  static const MoveDef locked = {.clip = 54, .last = 12, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(owner, S_KICK, &locked);
  const uint32_t words[] = {0x32000000, 1, 0x04000001, 0x32000000, 2, 0x30000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(0x570000 + i * 4, words[i]);
  }
  control_finished = 0;
  retail_vm_start(owner, &image, 0x570000, 0x570000 + sizeof words, control_end, repeat_native);
  for (int i = 0; i < 10 && owner->retail_vm.active; ++i) {
    check(owner->current_mmove == NULL, "native repetition must re-enter retail words without copying frame rows");
    tick();
  }
  check(control_finished == 1 && fighter_sample(owner) == 3, "native counter must repeat the source stream and resume its successor once");
  retail_vm_start(owner, &image, 0x570000, 0x570000 + sizeof words, control_end, repeat_native);
  fighter_interrupt(owner, fighter_ready);
  check(owner->retail_vm.repeat_count == 0 && !owner->retail_vm.active, "interruption must retire the native repetition contract");
}

static void native_row_data(void) {
  /* Mutate the native parameter source, not an authored move expectation. */
  const uint32_t table = 0x4e5d30;
  uint32_t saved[8];
  for (unsigned i = 0; i < 8; ++i) {
    saved[i] = mk4_image_u32(&image, table + i * 4);
  }
  put_word(table + 8, 2);
  put_word(table + 12, 0xffffffffu);
  put_word(table + 16, 0x46a770);
  Fighter *owner = setup(0, 2);
  static const MoveDef locked = {.clip = 54, .last = 12, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(owner, S_KICK, &locked);
  put_word(0x570000, 0x3e000000);
  retail_vm_start(owner, &image, 0x570000, 0x570004, control_end, reject_native);
  check(retail_native_vm(owner, 0x46a760, RETAIL_TAIL, 0), "native rows must use the shared address binding");
  check(fighter_sample(owner) == 2 && owner->current_mmove == NULL, "native rows must load live poses without copying frame arrays");
  tick();
  check(owner->retail_vm.entry == 0x4e5c70 && owner->action == ACTION_RETAIL_1006, "native row terminator must select its retail continuation");
  fighter_interrupt(owner, fighter_ready);
  check(!owner->retail_vm.active && owner->retail_vm.native_resume == NULL, "interruption must retire native row continuation state");
  for (unsigned i = 0; i < 8; ++i) {
    put_word(table + i * 4, saved[i]);
  }
}

/* The native query advances the bound cursor; it does not own fighter poses. */
static void native_cursor_return(void) {
  Fighter *owner = setup(0, 2);
  static const MoveDef locked = {.clip = 54, .last = 12, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(owner, S_SWEEP, &locked);
  owner->sample_override = 4;
  put_word(0x570000, 0x3e000000);
  control_finished = 0;
  retail_vm_start(owner, &image, 0x570000, 0x570004, control_end, reject_native);
  check(retail_native_vm(owner, 0x46f080, RETAIL_TAIL, 0), "native cursor query must use the shared binding");
  check(fighter_sample(owner) == 5 && owner->current_mmove == NULL, "native query must advance the bound cursor");
  tick();
  check(fighter_sample(owner) == 6 && owner->action == ACTION_RETAIL_1006, "second miss must advance once and start recovery");
  for (unsigned i = 0; i < 20 && !control_finished; ++i) {
    tick();
  }
  check(control_finished == 1 && fighter_sample(owner) == 12, "native recovery must finish the loaded clip and return once");
}

static void native_shared_record(void) {
  Fighter *owner = setup(0, 0);
  Fighter *victim = fighter_peer(owner);
  uint32_t record = mk4_image_u32(&image, 0x53d268) * 4;
  uint32_t saved = mk4_image_u32(&image, record + 20);
  put_word(record + 20, 0);
  FighterFn entry = retail_native_entry((unsigned)owner->roster, 0x46d290);
  entry(owner);
  check(fighter_move(owner)->last == (int)mk4_image_u32(&image, 0x4fbb60) - 1, "shared native must bind the loaded descriptor length");
  bool contacted = false;
  for (int i = 0; i < 64 && owner->current_mmove != NULL; ++i) {
    check(fighter_sample(owner) >= 0 && fighter_sample(owner) <= fighter_move(owner)->last, "shared native pose must stay within its loaded descriptor");
    tick();
    contacted |= owner->connected;
  }
  check(contacted && victim->health == Q16_ONE, "native contact must use the mutated retail record rather than a C damage copy");
  check(owner->current_mmove == NULL && owner->state == S_IDLE, "shared native must return through the normal actor controller");
  put_word(record + 20, saved);
}

static void native_contracts(void) {
  Fighter *owner = setup(0, 2);
  check(retail_native_vm(owner, 0x4913a0, RETAIL_PHYSICS, 0) && owner->body.physics == fighter_friction,
      "native physics binding must install its typed callback without running it");
  check(!retail_native_vm(owner, 0x4913a0, RETAIL_CALL, 0), "physics callbacks must not be invoked through the call ABI");
  check(!retail_native_vm(owner, 0x49a6a0, RETAIL_SPAWN, 0), "spawn ABI must reject an unreviewed task type before calling native code");
  check(!retail_native_vm(owner, 0x12345678, RETAIL_TAIL, 0), "native lookup must reject unknown retail addresses");
  owner->roster = 4;
  owner->retail_vm.native_owner = 0;
  owner->retail_vm.native_owner_set = true;
  check(retail_native_vm(owner, 0x480040, RETAIL_PHYSICS, 0) && owner->body.physics != NULL,
      "native binding context must remain independent of a victim or scene actor's model roster");
  owner->roster = MK4_ROSTER_COUNT;
  check(!retail_native_vm(owner, 0x4913a0, RETAIL_PHYSICS, 0), "native lookup must reject an invalid owner before shifting its mask");
}

static void data_tables(void) {
  uint32_t distance_saved = mk4_image_u32(&image, 0x4dfe10);
  put_word(0x4dfe10, 0x12345);
  check(retail_cpu_finish_distance(&image, 0, false) == 0x12345, "CPU approach must read the live retail distance row");
  put_word(0x4dfe10, distance_saved);
  uint32_t scene_slot = 0x4e0498;
  uint32_t scene_saved = mk4_image_u32(&image, scene_slot);
  uint32_t alternate = mk4_image_u32(&image, scene_slot + 4);
  FighterFn original_scene = retail_native_entry(0, scene_saved);
  put_word(scene_slot, alternate);
  check(retail_finish_target(&image, 0, false) == alternate && retail_native_entry(0, alternate) != original_scene,
      "scene dispatch must resolve the live retail target rather than a fighter callback table");
  put_word(scene_slot, scene_saved);
  /* Mutate shared source records: no compiled command or roster definition
   * may shadow a retail table read, including the second player's list. */
  uint32_t slot = 0x53cb58 + 4 * 17;
  uint32_t saved = mk4_image_u32(&image, slot);
  uint32_t base = 0x4ee000;
  uint32_t words[4];
  for (unsigned i = 0; i < 4; ++i) {
    words[i] = mk4_image_u32(&image, base + i * 4);
  }
  put_word(slot, base / 4);
  put_word(base, 0xddaa);
  put_word(base + 4, 0x1234);
  put_word(base + 8, 0x01010b01);
  put_word(base + 12, 0);
  uint32_t action_slot = 0x4e6ef8 + 4;
  uint32_t action_saved = mk4_image_u32(&image, action_slot);
  put_word(action_slot, 0x456780);
  RetailCommandCursor cursor = retail_command_cursor(&image, 17, 1, BUTTON_HIGH_PUNCH, false);
  RetailCommand command;
  check(retail_command_next(&image, &cursor, &command), "runtime must read the selected player/character list");
  check(command.pattern == 0xddaa && command.extra == 0x1234 && command.window == 11 && command.target == 0x456780 &&
            command.predicate == mk4_image_u32(&image, 0x4e4ec4),
      "runtime must decode the record and resolve live predicate/action tables");
  check(!retail_command_next(&image, &cursor, &command), "retail terminator must close the list");
  Fighter *owner = setup(1, 2);
  owner->roster = 17;
  put_word(base, 0xffffffffu);
  put_word(base + 8, 0x00010001);
  put_word(action_slot, 0x469250);
  fighter_press(owner, BUTTON_HIGH_PUNCH);
  check(owner->pending == retail_native_entry(17, 0x469250), "live input must use the selected retail action rather than a compiled command table");

  roster_def(17);
  uint32_t cpu_slot = 0x4df8e8 + 7 * 4;
  uint32_t cpu_saved = mk4_image_u32(&image, cpu_slot);
  put_word(cpu_slot, 0x0001000f);
  check(battle_cpu_action(owner, CPU_SPECIAL_NEAR) && owner->pending == retail_native_entry(17, 0x469250),
      "CPU dispatch must read the live remapped retail action row");
  put_word(cpu_slot, 0);
  owner->pending = NULL;
  check(!battle_cpu_action(owner, CPU_SPECIAL_NEAR) && owner->pending == NULL, "zero CPU rows must not invent another special action");
  put_word(cpu_slot, cpu_saved);

  for (unsigned i = 0; i < 4; ++i) {
    put_word(base + i * 4, words[i]);
  }
  put_word(slot, saved);
  put_word(action_slot, action_saved);
  check(retail_character_slot(16) == 2 && retail_character_slot(17) == 7, "shared stance selection must apply retail hidden-roster remaps");
  for (int character = 0; character < MK4_ROSTER_COUNT; ++character) {
    roster_def(character);
  }
}

static int cleanup_calls;

static void cleanup_once(Fighter *fighter) {
  (void)fighter;
  cleanup_calls++;
}

static bool predicate_native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  (void)argument;
  check(address == 0x12345678 && kind == RETAIL_PREDICATE, "predicate must use its typed native boundary");
  fighter->retail_vm.result = 4;
  return true;
}

static void native_address_identity(void) {
  Fighter *owner = setup(0, 2);
  owner->body.vel[AXIS_X] = 0x1234;
  owner->body.vel[AXIS_Z] = -0x2345;
  check(retail_native_vm(owner, 0x48f6e0, RETAIL_CALL, 0), "shared native address must not depend on a fighter registration mask");
  check(owner->saved_motion[AXIS_X] == 0x1234 && owner->body.vel[AXIS_X] == 0, "native momentum save must use the calling actor");
  check(retail_native_vm(owner, 0x48f750, RETAIL_CALL, 0), "shared native restore must use the same typed address boundary");
  check(owner->body.vel[AXIS_X] == 0x1234 && owner->body.vel[AXIS_Z] == -0x2345, "native restore must preserve both horizontal components");
}

static void native_handoff(void) {
  /* Shared contracts: predicate flags select a packed branch, and cleanup
   * survives VM/native playback transitions until the action is cancelled. */
  Fighter *owner = setup(0, 2);
  const uint32_t base = 0x570000;
  const uint32_t words[] = {0x36000000, 0x12345678, (base + 16) / 4, 0xff000000, 0x3e000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + i * 4, words[i]);
  }
  retail_vm_start(owner, &image, base, base + sizeof words, control_end, predicate_native);
  check(owner->retail_vm.parked, "predicate branch must skip invalid data and reach the parked continuation");
  cleanup_calls = 0;
  owner->retail_vm.cleanup = cleanup_once;
  static const mframe_t frames[] = {{0}};
  static const mmove_t phase = {.frame = frames};
  fighter_mmove_phase(owner, &phase);
  check(!owner->retail_vm.active && cleanup_calls == 0, "native takeover must retain action cleanup");
  retail_vm_start_at(owner, &image, base + 16, base, base + sizeof words, control_end, predicate_native);
  check(owner->current_mmove == NULL && cleanup_calls == 0, "retail continuation must inherit native action cleanup");
  fighter_cancel_mmove(owner);
  fighter_cancel_mmove(owner);
  check(cleanup_calls == 1, "interruption must release inherited cleanup exactly once");
}

static void assets_bound(void) {
  Assets assets = {0};
  assets_load(&assets, "MK4/FILESYS.DAT", "MK4/MK-4.EXE");
  for (int crouch = 0; crouch < 2; ++crouch) {
    Fighter *owner = setup(0, 2);
    battle_bind_assets(&assets);
    press(0, crouch != 0, 2);
    check(owner->retail_vm.active && owner->retail_vm.image == &assets.retail_image, "asset binding must select the host-owned executable image");
    recover(owner, 0, crouch != 0, 2);
  }
  battle_init(0, 4);
  assets_free(&assets);
}

/* A table edit must select a native service independently of model identity. */
static void reaction_native_table(void) {
  Fighter *actor = setup(0, 2);
  uint32_t original = mk4_image_u32(&image, 0x4ea4e8);
  put_word(0x4ea4e8, 0x4854e0);
  FighterFn entry = retail_throw_reaction(&image, 0);
  check(entry != NULL, "loaded reaction target must resolve its shared native entry");
  /* Native4854e0 inherits the descriptor installed by the paired script. */
  retail_vm_bind_animation(actor, 0x4fad7c + 149 * 12, 0);
  entry(actor);
  check(actor->retail_vm.active && actor->retail_vm.entry == 0x4e99c0, "native reaction must execute its original script");
  for (int visit = 0; visit < 128 && actor->retail_vm.active; ++visit) {
    tick();
  }
  check(!actor->retail_vm.active, "shared native reaction must retire its continuation");
  actor = setup(0, 2);
  put_word(0x4ea4e8, 0x4851d0);
  entry = retail_throw_reaction(&image, 0);
  check(entry != NULL, "native recovery must resolve from the loaded table");
  entry(actor);
  for (int visit = 0; visit < 128 && actor->state != S_IDLE; ++visit) {
    tick();
  }
  check(actor->state == S_IDLE && actor->current_mmove == NULL, "native clip/hold/getup chain must release its playback ownership");
  put_word(0x4ea4e8, original);
}

static void native_throw_selection(void) {
  Fighter *owner = setup(0, 0);
  Fighter *peer = &g_battle.fighters[1];
  owner->body.pos[AXIS_X] = -0x4000;
  peer->body.pos[AXIS_X] = 0x4000;
  uint32_t original = mk4_image_u32(&image, 0x4ddfb8);
  put_word(0x4ddfb8, 0x42bda0);
  retail_lk_throw(owner);
  check(owner->retail_vm.active && owner->retail_vm.entry == 0x4de2a0, "native throw selection must follow the loaded table independently of model");
  for (int visit = 0; visit < 256 && (owner->retail_vm.active || peer->state != S_IDLE); ++visit) {
    tick();
  }
  check(!owner->retail_vm.active && peer->state == S_IDLE && !(owner->flags & FF_PAIRED) && !(peer->flags & FF_PAIRED),
      "shared capture and original victim continuation must release both contexts");
  put_word(0x4ddfb8, original);
}

/* Change the image, not a fighter profile: native startup must bind loaded data. */
static void native_inherited_descriptor(void) {
  Fighter *owner = setup(0, 2);
  uint32_t original = mk4_image_u32(&image, 0x4e6c10);
  uint32_t replacement = mk4_image_u32(&image, 0x4e6c14);
  put_word(0x4e6c10, replacement);
  retail_native_entry(0, 0x46be40)(owner);
  int clip = (int)((replacement * 4 - 0x4fad7c) / 12);
  check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e61e0 && fighter_move(owner)->clip == clip,
      "native inherited animation must come from the loaded descriptor table");
  check(
      retail_native_character_slot(16) == 2 && retail_native_character_slot(17) == 4, "native hidden-roster aliases must remain distinct from script aliases");
  fighter_cancel_mmove(owner);
  put_word(0x4e6c10, original);
}

static void native_button_table(void) {
  Fighter *owner = setup(0, 2);
  uint32_t original = mk4_image_u32(&image, 0x4e7078);
  put_word(0x4e7078, 0x46c740);
  owner->buttons = &retail_close_buttons[0];
  owner->flags = FF_FOLLOW_UP;
  fighter_press(owner, BUTTON_HIGH_PUNCH);
  check(owner->pending != NULL, "loaded button controller must schedule the native entry");
  tick();
  check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e5d90, "button dispatch must read the changed native target rather than a C button row");
  fighter_cancel_mmove(owner);
  put_word(0x4e7078, original);

  owner = setup(0, 2);
  original = mk4_image_u32(&image, 0x4e7008);
  put_word(0x4e7008, 0x46c7a0);
  owner->buttons = &retail_air_buttons[0];
  owner->body.pos[AXIS_Y] = owner->body.ground - 2 * Q16_ONE;
  owner->body.gravity = 0x28f;
  fighter_press(owner, BUTTON_HIGH_PUNCH);
  check(owner->pending != NULL && fighter_airborne(owner), "loaded air controller must preserve airborne state");
  tick();
  check(owner->retail_vm.entry == 0x4e61f8, "air dispatch must consume the loaded action target");
  fighter_cancel_mmove(owner);
  put_word(0x4e7008, original);
}

static void native_script_continuation(void) {
  for (unsigned interrupted = 0; interrupted < 2; ++interrupted) {
    Fighter *owner = setup(0, 2);
    native_hold(owner, 2, NULL);
    native_script_end(owner, 0x4e6d08, fighter_ready, 4);
    if (interrupted) {
      fighter_interrupt(owner, fighter_ready);
    }
    for (int visit = 0; visit < 5 && !owner->retail_vm.active; ++visit) {
      tick();
    }
    check(owner->native.script_end.entry == 0, "native script completion data must be consumed or canceled with its phase");
    if (interrupted) {
      check(!owner->retail_vm.active && owner->current_mmove == NULL, "canceled native completion must not resume a retail script");
    } else {
      check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e6d08 && owner->retail_vm.native_owner == 4,
          "native completion must enter its stored original script with the typed integration context");
    }
  }

  Fighter *owner = setup(0, 2);
  uint32_t saved[2] = {mk4_image_u32(&image, 0x4e6d08), mk4_image_u32(&image, 0x4e6d0c)};
  put_word(0x4e6d08, 0x34000002);
  put_word(0x4e6d0c, 0x30000000);
  retail_native_begin(owner, S_KICK, 0x101, false);
  retail_vm_enter_then_owned(owner, 0x4e6d08, 0x4e5d90, fighter_ready, 4);
  native_begin(owner, 0, 0, NULL);
  native_script_end(owner, 0x4e62f0, fighter_ready, 1);
  for (int visit = 0; visit < 5 && owner->retail_vm.entry != 0x4e5d90; ++visit) {
    tick();
  }
  check(owner->retail_vm.entry == 0x4e5d90 && owner->retail_vm.native_owner == 4 && owner->retail_vm.then.entry == 0 && owner->native.script_end.entry == 0,
      "VM return continuation must stay independent of native completion data and consume once");

  static const MoveDef actor_view = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK};
  Fighter *actor = battle_scene_actor_start_script(owner, fighter_peer(owner), &actor_view, 0x4e6d08, fighter_ready, 1);
  check(actor != NULL && actor->roster == fighter_peer(owner)->roster && actor->retail_vm.active && actor->retail_vm.native_owner == 1,
      "raw scene script entry must preserve model identity independently of native integration context");
  battle_reset_match();
  check(!owner->retail_vm.active && owner->retail_vm.then.entry == 0 && !actor->retail_vm.active,
      "reset must cancel both raw scene script and VM return continuations");
  put_word(0x4e6d08, saved[0]);
  put_word(0x4e6d0c, saved[1]);
}

static void native_close_lifetimes(void) {
  const FighterFn entries[] = {retail_close_high_punch, retail_combo_high_punch, retail_close_high_kick};
  for (unsigned index = 0; index < sizeof entries / sizeof *entries; ++index) {
    Fighter *owner = setup(0, 2);
    entries[index](owner);
    for (int visit = 0; visit < 128 && owner->state != S_IDLE; ++visit) {
      tick();
    }
    check(owner->state == S_IDLE && !owner->retail_vm.active && owner->current_mmove == NULL,
        "native clip completion and inherited/script recovery must retire their ownership");
  }
}

static void native_air_ownership(void) {
  for (unsigned armed = 0; armed < 2; ++armed) {
    Fighter *owner = setup(0, 2);
    owner->armed = armed != 0;
    owner->body.pos[AXIS_Y] = owner->body.ground - 2 * Q16_ONE;
    owner->body.gravity = 0x28f;
    retail_air_high_punch(owner);
    check(owner->current_move == &owner->playback_move && fighter_move(owner)->clip >= 0 && fighter_move(owner)->last != MOVE_END,
        "native air startup must bind loaded animation without a fighter C seed");
    check(owner->retail_vm.active || owner->current_mmove != NULL, "native/script air handoff must have a playback owner");
    fighter_interrupt(owner, fighter_ready);
    tick();
    check(!owner->retail_vm.active && owner->current_mmove == NULL, "interrupt must retire native/script air playback ownership");
  }
}

/* Original native contact rows select their recovery continuation at runtime. */
static void native_air_loaded_recovery(void) {
  Fighter *owner = setup(0, 2);
  uint32_t original = mk4_image_u32(&image, 0x4e6694);
  put_word(0x4e6694, 0x46e230);
  owner->body.pos[AXIS_Y] = owner->body.ground - 3 * Q16_ONE;
  owner->body.gravity = 0x28f;
  retail_air_high_punch(owner);
  unsigned visits = 0;
  while (owner->retail_vm.entry != 0x4e64e0 && visits++ < 32) {
    tick();
  }
  check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e64e0,
      "native contact miss must consume its loaded callback and enter the original recovery script");
  fighter_interrupt(owner, fighter_ready);
  tick();
  check(!owner->retail_vm.active && owner->current_mmove == NULL && owner->body.gravity != 0,
      "interrupting scripted air recovery must retire playback and restore flight");
  put_word(0x4e6694, original);
}

static void native_air_capture_lifetimes(void) {
  const FighterFn entries[] = {retail_air_throw, retail_air_throw_lift, retail_air_throw_slam, retail_air_throw_flip, retail_air_throw_slam};
  for (unsigned index = 0; index < sizeof entries / sizeof *entries; ++index) {
    Fighter *owner = setup(0, 0);
    Fighter *peer = fighter_peer(owner);
    bool ko = index == 4;
    peer->health = ko ? 1 : Q16_ONE;
    owner->body.pos[AXIS_Y] = owner->body.ground - 3 * Q16_ONE;
    peer->body.pos[AXIS_Y] = peer->body.ground - 3 * Q16_ONE;
    entries[index](owner);
    check((owner->flags & FF_PAIRED) && (peer->flags & FF_PAIRED), "native air capture must acquire both contexts");
    for (int visit = 0; visit < 512; ++visit) {
      tick();
      if (ko ? peer->health == 0 && peer->current_mmove == NULL && !peer->retail_vm.active && !(owner->flags & FF_PAIRED)
             : owner->state == S_IDLE && peer->state == S_IDLE) {
        break;
      }
    }
    check((ko ? peer->health == 0 && peer->state != S_IDLE && !peer->retail_vm.active && peer->current_mmove == NULL
              : owner->state == S_IDLE && peer->state == S_IDLE) &&
              !(owner->flags & FF_PAIRED) && !(peer->flags & FF_PAIRED),
        "native air capture/release must retire independent contexts and preserve KO ownership");
  }
}

int main(int argc, char **argv) {
  image = mk4_image_load("MK4/MK-4.EXE");
  if (argc == 2) {
    if (strcmp(argv[1], "opcode") == 0) {
      put_word(0x4e6d08, 0xff000000);
    } else if (strcmp(argv[1], "native") == 0) {
      put_word(0x4e6d0c, 0x12345678);
    } else if (strcmp(argv[1], "sample") == 0) {
      put_word(0x4e6d24, 0x490000ff);
    } else if (strcmp(argv[1], "budget") == 0) {
      put_word(0x4e6d08, 0x3a0000ff);
      put_word(0x4e6d0c, 0x3b000000);
    } else if (strcmp(argv[1], "range") == 0) {
      Fighter *owner = setup(0, 2);
      put_word(0x570000, 0x1f000000);
      retail_vm_start(owner, &image, 0x570000, 0x570004, control_end, reject_native);
    } else {
      check(false, "unknown rejection test");
    }
    setup(0, 2);
    press(0, false, 2);
    check(false, "malformed script was accepted");
  }
  behavior();
  runtime_words();
  control_flow();
  playback_modes();
  native_handoff();
  native_address_identity();
  native_contracts();
  native_repetition();
  native_row_data();
  native_cursor_return();
  native_shared_record();
  data_tables();
  reaction_native_table();
  native_throw_selection();
  native_inherited_descriptor();
  native_button_table();
  native_script_continuation();
  native_close_lifetimes();
  native_air_ownership();
  native_air_loaded_recovery();
  native_air_capture_lifetimes();
  lifetimes();
  assets_bound();
  battle_init(0, 4);
  mk4_image_free(&image);
  puts("PASS: direct Scorpion scripts, hit/block/miss, both slots, runtime loop words, interruption, KO and reset");
  return 0;
}
