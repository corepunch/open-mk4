#include "game.h"
#include "retail_vm.h"
#include "script.h"
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
  FighterFn entry = retail_native_entry(0x46d290);
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
  check(retail_native_vm(owner, 0x480040, RETAIL_PHYSICS, 0) && owner->body.physics != NULL,
      "native address selection must remain independent of a victim or scene actor's model roster");
  owner->roster = MK4_ROSTER_COUNT;
  check(!retail_native_vm(owner, 0x4913a0, RETAIL_PHYSICS, 0), "native lookup must reject an invalid actor model");
}

static void data_tables(void) {
  uint32_t distance_saved = mk4_image_u32(&image, 0x4dfe10);
  put_word(0x4dfe10, 0x12345);
  check(retail_cpu_finish_distance(&image, 0, false) == 0x12345, "CPU approach must read the live retail distance row");
  put_word(0x4dfe10, distance_saved);
  uint32_t scene_slot = 0x4e0498;
  uint32_t scene_saved = mk4_image_u32(&image, scene_slot);
  uint32_t alternate = mk4_image_u32(&image, scene_slot + 4);
  FighterFn original_scene = retail_native_entry(scene_saved);
  put_word(scene_slot, alternate);
  check(retail_finish_target(&image, 0, false) == alternate && retail_native_entry(alternate) != original_scene,
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
  check(owner->pending == retail_native_entry(0x469250), "live input must use the selected retail action rather than a compiled command table");

  roster_def(17);
  uint32_t cpu_slot = 0x4df8e8 + 7 * 4;
  uint32_t cpu_saved = mk4_image_u32(&image, cpu_slot);
  put_word(cpu_slot, 0x0001000f);
  check(battle_cpu_action(owner, CPU_SPECIAL_NEAR) && owner->pending == retail_native_entry(0x469250),
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
  uint32_t inventory_slot = 0x53d5c0 + 17 * 4;
  uint32_t inventory_saved = mk4_image_u32(&image, inventory_slot);
  uint32_t inventory_selected = mk4_image_u32(&image, 0x53d5c0);
  put_word(inventory_slot, inventory_selected);
  RetailWeaponInventory inventory;
  retail_weapon_read(&image, 17, &inventory);
  check(inventory.address == inventory_selected * 4, "inventory dispatch must read the changed raw slot independently of script and CPU aliases");
  put_word(inventory_slot, inventory_saved);

  for (int character = 0; character < MK4_ROSTER_COUNT; ++character) {
    roster_def(character);
  }
}

static int cleanup_calls;

static void cleanup_once(Fighter *fighter) {
  (void)fighter;
  cleanup_calls++;
}

static unsigned predicate_argument;

static bool predicate_native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  predicate_argument = argument;
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
  for (unsigned id = 0; id < RETAIL_REACTION_COUNT; ++id) {
    if (retail_reaction_target(&image, id) != 0) {
      check(retail_reaction_entry(&image, id) != NULL, "every loaded reaction entry needs a shared native implementation");
    }
  }
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
  actor = setup(0, 2);
  put_word(0x4ea4e8, 0x486230);
  battle_react(actor, 0);
  tick();
  check(actor->appearance == APPEARANCE_FROZEN && actor->action == 0x1003,
      "engine-native reactions must also follow the loaded address rather than a copied reaction-ID table");
  fighter_interrupt(actor, fighter_ready);
  for (int branch = 0; branch < 3; ++branch) {
    actor = setup(0, 2);
    actor->crouching = branch == 2;
    if (branch != 0) {
      retail_weapon_equip(actor);
    }
    put_word(0x4ea4e8, 0x486c40);
    battle_react(actor, 0);
    for (int visit = 0; visit < 128; ++visit) {
      tick();
      if (actor->state == S_IDLE && actor->pending == NULL) {
        break;
      }
    }
    check(actor->state == S_IDLE && !actor->retail_vm.active && actor->current_mmove == NULL,
        "shared block script/native branches must release playback ownership");
  }
  put_word(0x4ea4e8, original);
}

/* These original addresses differ by facing operations and recovery row,
 * independently of the actor model or the caller's script context. */
static void native_getup_entries(void) {
  const uint32_t entries[] = {0x485ad0, 0x485b10, 0x485b50, 0x488c10};
  for (unsigned i = 0; i < sizeof entries / sizeof *entries; ++i) {
    Fighter *actor = setup(0, 2);
    actor->held = BUTTON_BIT(BUTTON_DOWN);
    int facing = actor->facing;
    uint32_t row = i == 0 ? 0x4ea210 : i == 3 ? 0x4ea230 : 0x4ea220;
    check(retail_native_vm(actor, entries[i], RETAIL_TAIL, 0), "getup address must work independently of native owner");
    check(actor->facing == (i >= 2 ? facing : -facing), "getup native must preserve its original facing operations");
    uint32_t descriptor = mk4_image_u32(&image, row + 12) * 4;
    check(fighter_move(actor)->clip == (int)((descriptor - 0x4fad7c) / 12), "getup must select the held-down descriptor from retail data");
    fighter_cancel_mmove(actor);
    check(actor->current_mmove == NULL, "getup interruption must release native playback");
  }
}

static void native_weapon_parameters(void) {
  Fighter *actor = setup(0, 2);
  const uint32_t parameters = 0x4ec458, motion = 0x4ec470;
  uint32_t original_phases = mk4_image_u32(&image, parameters + 4);
  uint32_t original_query = mk4_image_u32(&image, parameters + 8);
  uint32_t original_speed = mk4_image_u32(&image, motion + 4);
  put_word(parameters + 4, (original_phases & 0x00ffffff) | 9u << 24);
  put_word(parameters + 8, original_query & 0xffffff00);
  put_word(motion + 4, 0x1234);
  weapon_start_retail_strike(actor, parameters, motion, ACTION_RETAIL_0111, true);
  AttackRecord expected = retail_vm_attack_record(&image, mk4_image_u32(&image, 0x53d0c0) * 4);
  check(actor->weapon_strike == &actor->loaded_weapon_strike, "native strike parameters must have actor-owned lifetime");
  check(actor->weapon_strike->windup == 9 && actor->weapon_strike->record.damage == expected.damage && actor->weapon_strike->speed == 0x1234,
      "native weapon phases, selector and motion must come from the supplied retail rows");
  fighter_cancel_mmove(actor);
  put_word(parameters + 4, original_phases);
  put_word(parameters + 8, original_query);
  put_word(motion + 4, original_speed);
}

static void native_weapon_inventory(void) {
  Fighter *actor = setup(0, 2);
  uint32_t original = mk4_image_u32(&image, 0x53d5c0);
  uint32_t selected = mk4_image_u32(&image, 0x53d5c0 + 17 * 4);
  put_word(0x53d5c0, selected);
  retail_weapon_equip(actor);
  check(actor->armed && actor->weapon->inventory == selected * 4 && actor->weapon->type == (int)mk4_image_u32(&image, selected * 4),
      "equipment must load the raw roster inventory independently of the actor model");
  uint32_t table = actor->weapon->buttons;
  uint32_t target = mk4_image_u32(&image, table);
  put_word(table, 0x496c20);
  retail_weapon_high_punch(actor);
  check(actor->weapon_strike != NULL && actor->weapon_strike->parameters == 0x4ec880,
      "armed input must resolve the loaded native button target rather than fighter callbacks");
  fighter_cancel_mmove(actor);
  uint32_t alternative = mk4_image_u32(&image, 0x4ec88c);
  put_word(0x4ec88c, 0x495740);
  retail_weapon_back_high_punch(actor);
  check(actor->weapon_strike->parameters == 0x4ec4b8 && actor->action == ACTION_RETAIL_0113,
      "back input must follow the original parameter alternative and native action class");
  fighter_cancel_mmove(actor);
  put_word(0x4ec88c, alternative);
  put_word(table, target);
  put_word(0x53d5c0, original);
}

static void native_contact_record_lifetime(void) {
  Fighter *actor = setup(0, 2);
  retail_vm_bind_animation(actor, 0x4fad7c + 227 * 12, 29);
  AttackRecord record = retail_vm_attack_record(&image, mk4_image_u32(&image, 0x53d0c0 + 11 * 4) * 4);
  q16 damage = record.damage;
  weapon_script_contact_tail(actor, &record, 2);
  record.damage = 0;
  check(actor->native_record == &actor->native_record_value && actor->native_record->damage == damage,
      "delayed weapon contact playback must own a copy of caller-local record data");
  fighter_cancel_mmove(actor);
  record.damage = damage;
  native_query_rows(actor, &record, 2, 1, control_end);
  record.damage = 0;
  check(actor->native_record == &actor->native_record_value && actor->native_record->damage == damage,
      "shared native contact rows must retain caller data across the yielding boundary");
  fighter_cancel_mmove(actor);
  retail_vm_bind_animation(actor, 0x4fad7c + 227 * 12, 29);
  uint32_t original = mk4_image_u32(&image, 0x53d0c0 + 11 * 4);
  uint32_t selected = mk4_image_u32(&image, 0x53d0c0);
  put_word(0x53d0c0 + 11 * 4, selected);
  check(retail_native_vm(actor, 0x496970, RETAIL_TAIL, 0), "weapon contact native must not depend on fighter registration masks");
  AttackRecord expected = retail_vm_attack_record(&image, selected * 4);
  check(actor->native_record == &actor->native_record_value && actor->native_record->damage == expected.damage,
      "weapon native must read its contact selector from the loaded bank and own the decoded record");
  put_word(0x53d0c0 + 11 * 4, original);
  fighter_cancel_mmove(actor);
}

static void native_projectile_context(void) {
  Fighter *actor = setup(0, 2);
  check(!retail_native_vm(actor, 0x498bb0, RETAIL_SPAWN, 74), "shared projectile native must reject the wrong typed task argument");
  check(retail_native_vm(actor, 0x498bb0, RETAIL_SPAWN, 75), "shared projectile spawn must be independent of fighter owner masks");
  Projectile *projectile = NULL;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    if (g_battle.projectiles[i].kind != 0) {
      projectile = &g_battle.projectiles[i];
      break;
    }
  }
  check(projectile != NULL && projectile->kind == PROJECTILE_FIREBALL && projectile->owner == 0 && projectile->mesh_animation != NULL,
      "original native task must allocate the common projectile resource with its caller identity");
  battle_init(0, 2);
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    check(g_battle.projectiles[i].kind == 0, "reset must retire the independent native projectile");
  }
}

static void native_return_ownership(void) {
  Fighter *actor = setup(0, 2);
  Fighter *peer = fighter_peer(actor);
  actor->flags |= FF_PAIRED;
  peer->flags |= FF_PAIRED;
  actor->body.vel[AXIS_X] = 0x1234;
  check(retail_native_vm(actor, 0x42ce80, RETAIL_TAIL, 0), "native throw return must not depend on a fighter context mask");
  check(!(actor->flags & FF_PAIRED) && !(peer->flags & FF_PAIRED) && actor->body.vel[AXIS_X] == 0 && actor->state == S_IDLE,
      "controller return must retire pair and motion ownership together");
  actor = setup(0, 2);
  actor->health = 0;
  fighter_start_move(actor, S_HIT, roster_def(actor->roster)->moves[S_HIT]);
  check(retail_native_vm(actor, 0x42ce60, RETAIL_TAIL, 0), "native KO return must resolve the shared controller boundary");
  check(actor->health == 0 && actor->state == S_HIT && actor->current_mmove == NULL && !actor->retail_vm.active,
      "native controller return must not revive a defeated actor");
  check(retail_native_vm(actor, 0x470420, RETAIL_TAIL, 0) && actor->state != S_IDLE, "ordinary native controller return must also preserve KO ownership");
  actor = setup(0, 2);
  actor->body.pos[AXIS_Y] = actor->body.ground - Q16_ONE;
  actor->body.vel[AXIS_Y] = -0x123;
  actor->body.gravity = 0x147;
  check(retail_native_vm(actor, 0x470420, RETAIL_TAIL, 0), "ordinary native controller return must resolve independently of script context");
  check(actor->state == S_JUMP && actor->body.pos[AXIS_Y] == actor->body.ground - Q16_ONE && actor->body.vel[AXIS_Y] == -0x123 && actor->body.gravity == 0x147,
      "native controller return must preserve airborne physics rather than snap or freeze the actor");
}

static void native_sound_data(void) {
  Fighter *actor = setup(0, 2);
  uint32_t pointer = mk4_image_u32(&image, 0x53d018);
  uint32_t remap = mk4_image_u32(&image, 0x4ead18);
  const uint32_t table = 0x570100, list = table + 32;
  put_word(0x53d018, table / 4);
  put_word(0x4ead18, (remap & 0xffffff00u) | 1);
  put_word(table, 0x01240123);
  g_battle.sound_count = 0;
  mk4_script_roster_sound(actor, 6);
  check(g_battle.sound_count == 1 && g_battle.sounds[0].bank == SOUND_TOKEN && g_battle.sounds[0].selector == 0x124,
      "native roster sound must use loaded remap bytes and direct half-word tokens");
  put_word(table, 0);
  put_word(table + 8, list / 4);
  put_word(list, 0x01250001);
  g_battle.sound_count = 0;
  mk4_script_roster_sound(actor, 6);
  check(g_battle.sound_count == 1 && g_battle.sounds[0].selector == 0x125, "native roster sound must decode the selected half-word token list");
  put_word(0x53d018, pointer);
  put_word(0x4ead18, remap);
  uint32_t count = mk4_image_u32(&image, 0x4ea828), token = mk4_image_u32(&image, 0x4ea82c);
  put_word(0x4ea828, 1);
  put_word(0x4ea82c, 0x126);
  g_battle.sound_count = 0;
  retail_throw_grunt();
  check(g_battle.sound_count == 1 && g_battle.sounds[0].selector == 0x126, "native throw voice must read its word-sized token list");
  put_word(0x4ea828, count);
  put_word(0x4ea82c, token);
}

static void native_callback_table(void) {
  const uint32_t table = 0x570180, descriptor = 0x4fad7c;
  for (unsigned interrupted = 0; interrupted < 2; ++interrupted) {
    Fighter *actor = setup(0, 2);
    uint32_t count = mk4_image_u32(&image, descriptor + 4);
    put_word(descriptor + 4, 2);
    put_word(table, descriptor / 4);
    put_word(table + 4, 0x42ce80);
    native_animation_table(actor, table);
    put_word(descriptor + 4, count);
    check(actor->native.native_end.address == 0x42ce80 && actor->native.script_end.entry == 0,
        "native animation table must own its callback separately from VM script completion");
    put_word(table + 4, 0x4e6d08);
    if (interrupted) {
      fighter_interrupt(actor, fighter_ready);
    }
    for (int visit = 0; visit < 5 && actor->current_mmove != NULL; ++visit) {
      tick();
    }
    check(actor->current_mmove == NULL && !actor->retail_vm.active && actor->native.native_end.address == 0,
        "native table completion or interruption must consume its captured callback without entering callback data as script words");
  }
  Fighter *actor = setup(0, 2);
  native_hold(actor, 2, NULL);
  native_address_end(actor, 0x42ce80);
  native_script_end(actor, 0x4e6d08, fighter_ready);
  check(actor->native.native_end.address == 0, "script completion must replace an earlier native callback");
  native_address_end(actor, 0x42ce80);
  check(actor->native.script_end.entry == 0, "native callback must replace an earlier script completion");
  fighter_cancel_mmove(actor);
  check(actor->native.script_end.entry == 0 && actor->native.native_end.address == 0, "cancellation must clear both typed completion records");
}

static void native_reaction_table(void) {
  Fighter *actor = setup(0, 0);
  actor->roster = 15;
  uint32_t target = mk4_image_u32(&image, 0x4ea4e8 + 100 * 4);
  uint32_t packed = mk4_image_u32(&image, 0x4e9bb8);
  uint32_t callback = mk4_image_u32(&image, 0x4e9bbc);
  put_word(0x4ea4e8 + 100 * 4, 0x485b70);
  put_word(0x4e9bb8, 0x4fad7c / 4);
  put_word(0x4e9bbc, 0x42ce80);
  Fighter *owner = fighter_peer(actor);
  owner->retail_vm.image = &image;
  owner->flags |= FF_PAIRED;
  actor->flags |= FF_PAIRED;
  check(retail_native_vm(owner, 0x45b1f0, RETAIL_HANDLER, 100), "shared reaction handler must accept loaded native targets in any script context");
  tick();
  check(!(actor->flags & FF_PAIRED) && !(owner->flags & FF_PAIRED), "shared reaction dispatch must release captured pair ownership");
  check(actor->native.native_end.address == 0x42ce80 && fighter_move(actor)->clip == 0 && !actor->retail_vm.active,
      "reaction dispatch must use loaded native and animation/callback tables independently of roster or selector");
  fighter_interrupt(actor, fighter_ready);
  put_word(0x4ea4e8 + 100 * 4, target);
  put_word(0x4e9bb8, packed);
  put_word(0x4e9bbc, callback);
  uint32_t count = mk4_image_u32(&image, 0x4fc8f8);
  put_word(0x4ea4e8 + 100 * 4, 0x484f10);
  put_word(0x4fc8f8, 67);
  check(retail_native_vm(owner, 0x45b230, RETAIL_HANDLER, 100), "opponent reaction handler must use the same loaded native selection");
  tick();
  check(fighter_move(actor)->clip == (0x4fc8f4 - 0x4fad7c) / 12 && fighter_move(actor)->last == 66 && actor->native.move.lastframe <= 2,
      "native release must load the descriptor length rather than a copied fighter frame range");
  fighter_interrupt(actor, fighter_ready);
  put_word(0x4fc8f8, count);
  put_word(0x4ea4e8 + 100 * 4, target);
}

static void native_stagger_boundary(void) {
  uint32_t target = mk4_image_u32(&image, 0x4ea4e8 + 100 * 4);
  put_word(0x4ea4e8 + 100 * 4, 0x482e00);
  for (int crouch = 0; crouch < 2; ++crouch) {
    Fighter *actor = setup(0, 0);
    actor->action = crouch ? 0x2001 : 0x2000;
    Fighter *owner = fighter_peer(actor);
    uint64_t generation = owner->continuation;
    check(retail_native_vm(owner, 0x45b1f0, RETAIL_HANDLER, 100) && owner->continuation == generation && actor->pending != NULL,
        "script reaction dispatch must use the loaded peer native independently of owner context without interrupting the caller");
    tick();
    check(actor->retail_vm.active && actor->retail_vm.entry == (crouch ? 0x4e9360u : 0x4e9308u) && actor->current_mmove == NULL,
        "loaded stagger native must preserve the incoming action branch and transfer playback to original script words");
    if (crouch) {
      fighter_interrupt(actor, fighter_ready);
      check(!actor->retail_vm.active, "native-started reaction script must remain independently interruptible");
    } else {
      for (int visit = 0; visit < 128 && actor->retail_vm.active; ++visit) {
        tick();
      }
      check(!actor->retail_vm.active && actor->state == S_IDLE, "native-started script playback must return through the shared controller boundary");
    }
  }
  put_word(0x4ea4e8 + 100 * 4, target);
  Fighter *actor = setup(0, 2);
  actor->hits = 4;
  battle_react(actor, 151);
  tick();
  check(actor->retail_vm.active && actor->retail_vm.entry == 0x4e96e0,
      "native freeze interruption must transfer to the shared fall script rather than a retired C reaction row");
  fighter_interrupt(actor, fighter_ready);
}

static uint32_t returned_word;
static unsigned returned_limit;

static void counted_operand_end(Fighter *fighter) {
  returned_word = fighter->retail_vm.returned_word;
  returned_limit = fighter->retail_vm.playback_limit;
  control_end(fighter);
}

static void animation_controller_opcodes(void) {
  const uint32_t base = 0x570180, descriptor = 0x4fad7c;
  uint32_t count = mk4_image_u32(&image, descriptor + 4);
  put_word(descriptor + 4, 9);
  static const MoveDef locked = {.clip = 0, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD};
  Fighter *actor = setup(0, 2);
  fighter_start_move(actor, S_PUNCH, &locked);
  put_word(base, 0x1a000000);
  put_word(base + 4, descriptor / 4);
  put_word(base + 8, 0x30000000);
  retail_vm_start(actor, &image, base, base + 12, control_end, reject_native);
  tick();
  check(actor->retail_vm.interpolated_play && actor->blend_duration == 1 && actor->blend_sample == 1 && actor->blend_weight == 0x8000,
      "interleaved playback must submit adjacent source poses at the original half weight");
  fighter_interrupt(actor, fighter_ready);
  check(
      !actor->retail_vm.active && actor->blend_duration == 0 && actor->blend_weight == -1, "interruption must release interpolated pose ownership with the VM");
  for (int limited = 0; limited < 2; ++limited) {
    actor = setup(0, 2);
    fighter_start_move(actor, S_PUNCH, &locked);
    retail_vm_bind_animation(actor, descriptor, 0);
    put_word(base, limited ? 0x1cffff03 : 0x17000000);
    put_word(base + 4, limited ? 0x30000000 : descriptor / 4);
    put_word(base + 8, 0x30000000);
    control_finished = 0;
    retail_vm_start(actor, &image, base, base + 12, control_end, reject_native);
    for (int visit = 0; visit < 32 && actor->retail_vm.active; ++visit) {
      tick();
    }
    check(control_finished == 1 && fighter_sample(actor) == (limited ? 2 : 8) && actor->current_mmove == NULL,
        "native animation controllers must honor the inline source budget and loaded clip length without generated frame programs");
  }
  actor = setup(0, 2);
  put_word(base, 0x1bffff03);
  put_word(base + 4, 0xdeadbeef);
  returned_word = returned_limit = 0;
  control_finished = 0;
  retail_vm_start(actor, &image, base, base + 8, counted_operand_end, reject_native);
  check(control_finished == 1 && returned_word == 0xdeadbeef && returned_limit == 3 && !actor->retail_vm.active,
      "counted operand return must deliver data to its typed caller rather than execute the operand");
  put_word(descriptor + 4, count);
}

static bool distance_reaction_probe(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  check(address == 0x45b1f0 && kind == RETAIL_HANDLER && argument == 25, "distance reaction opcode must use its typed low-byte handler argument");
  fighter->native_variant++;
  return true;
}

static void motion_distance_opcodes(void) {
  const uint32_t base = 0x570180;
  const uint32_t words[] = {0x65000000, 0x1000, 0xffffe000, 0x100, 0x7000ff19, Q16_ONE, 0x3e000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + 4 * i, words[i]);
  }
  for (int reverse = 0; reverse < 2; ++reverse) {
    Fighter *actor = setup(0, 2);
    static const MoveDef locked = {.clip = 0, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD};
    fighter_start_move(actor, S_PUNCH, &locked);
    fighter_peer(actor)->facing = reverse ? 1 : -1;
    g_battle.distance = reverse ? 2 * Q16_ONE : Q16_ONE;
    actor->retail_vm.result = 4;
    actor->native_variant = 0;
    retail_vm_start(actor, &image, base, base + sizeof words, control_end, distance_reaction_probe);
    check(actor->body.vel[AXIS_X] == (reverse ? -0x1000 : 0x1000) && actor->body.vel[AXIS_Y] == -0x2000 && actor->body.gravity == 0x100,
        "directional motion must follow peer-facing order and consume all three signed motion words");
    check(actor->native_variant == (reverse ? 0 : 1) && actor->retail_vm.result == (reverse ? 4u : 5u),
        "distance reaction must use the world distance, preserve other flags and update bit zero");
    fighter_interrupt(actor, fighter_ready);
  }
}

static void edge_task_lifetime(void) {
  const uint32_t base = 0x570180;
  put_word(base, 0x6f000000);
  put_word(base + 4, (base + 16) / 4);
  put_word(base + 8, 0x72000000);
  put_word(base + 12, 0x3e000000);
  put_word(base + 16, 0xff000000);
  Fighter *actor = setup(0, 2), *peer = fighter_peer(actor);
  static const MoveDef locked = {.clip = 0, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD};
  fighter_start_move(actor, S_PUNCH, &locked);
  fighter_start_move(peer, S_PUNCH, &locked);
  g_battle.arena_radius = Q16_ONE;
  actor->body.pos[AXIS_X] = 2 * Q16_ONE;
  peer->body.pos[AXIS_X] = 3 * Q16_ONE;
  retail_vm_start(actor, &image, base, base + 20, control_end, reject_native);
  BattleEvent *event = NULL;
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    if (g_battle.events[i].active) {
      event = &g_battle.events[i];
      break;
    }
  }
  check(event != NULL && actor->body.vel[AXIS_X] < 0 && actor->body.vel[AXIS_X] == peer->body.vel[AXIS_X],
      "outer-peer edge task must apply one shared world recoil without taking fighter script ownership");
  for (int visit = 0; visit < 32 && event->active; ++visit) {
    tick();
  }
  check(
      !event->active && actor->body.vel[AXIS_X] == 0 && peer->body.vel[AXIS_X] == 0, "independent native edge task must retire and clean both motion contexts");
  battle_init(0, 4);
  check(!event->active, "round reset must leave the native event resource retired");
}

static void scene_task_lifetime(void) {
  Fighter *actor = setup(0, 2);
  g_battle.phase = PHASE_FATALITY;
  check(!retail_native_vm(actor, 0x46a280, RETAIL_SPAWN, 39), "hover native must reject an unrelated task type");
  check(retail_native_vm(actor, 0x46a280, RETAIL_SPAWN, 38), "shared hover native must accept any valid actor context");
  BattleEvent *event = NULL;
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    if (g_battle.events[i].active) {
      event = &g_battle.events[i];
      break;
    }
  }
  check(event != NULL && !event->cancel_on_owner_change, "native scene task must have independent event ownership");
  fighter_interrupt(actor, fighter_ready);
  check(event->active, "actor script interruption must preserve the independent scene task");
  g_battle.phase = PHASE_MATCH_OVER;
  check(!event->step(event, actor, fighter_peer(actor)), "scene completion must retire independent hover motion");
  battle_init(0, 4);
  check(!event->active, "round reset must release the scene task resource");
}

static void conditional_primitive_opcodes(void) {
  const uint32_t base = 0x570180;
  const uint32_t words[] = {0x43000000, 0x12345678, (base + 68) / 4, 0x55ffff07, 0x12345678, (base + 68) / 4, 0x3d000000, (base + 40) / 4, 0x12000002,
      0x30000000, 0x5d000000, 0x1000, 0xffffe000, 0x74000000, 0x54003002, 0x12000003, 0x30000000, 0xff000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + 4 * i, words[i]);
  }
  for (int blocked = 0; blocked < 2; ++blocked) {
    Fighter *actor = setup(0, 2);
    static const MoveDef locked = {.clip = 0, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD};
    fighter_start_move(actor, S_PUNCH, &locked);
    q16 x = actor->body.pos[AXIS_X], y = actor->body.pos[AXIS_Y];
    actor->contact = blocked ? CONTACT_BLOCK : CONTACT_HIT;
    control_finished = 0;
    g_battle.sound_count = 0;
    retail_vm_start(actor, &image, base, base + sizeof words, control_end, predicate_native);
    check(predicate_argument == 7, "inline native predicate arguments must be masked and forwarded through the typed boundary");
    if (blocked) {
      check(actor->retail_vm.active && actor->retail_vm.wake == g_battle.ticks + 2 && actor->body.pos[AXIS_X] == x + 0x1000 &&
                actor->body.pos[AXIS_Y] == y - 0x2000 && g_battle.sound_count == 1 && g_battle.sounds[0].selector == 0x627,
          "block-value branch must consume signed position operands, native token and flag-selected packed wait");
      for (int visit = 0; visit < 4 && actor->retail_vm.active; ++visit) {
        tick();
      }
    }
    check(control_finished == 1 && fighter_sample(actor) == (blocked ? 3 : 2),
        "predicate bit-zero branch and numeric block-result branch must remain distinct and continue at their selected targets");
  }
}

static void cursor_alias_opcodes(void) {
  Fighter *actor = setup(0, 2);
  const uint32_t base = 0x570180, descriptor = 0x4fad7c;
  const uint32_t words[] = {0x0f000000, descriptor / 4, 0x0bffffff, 0x15000000, 0x18000000, 0x21000000, 0x23000000, 0x57000000, 0x66000000, 0x67000000,
      0x0affff02, 0x34000001, 0x13000000, 0x34000001, 0x14000000, 0x30000000};
  for (unsigned i = 0; i < sizeof words / sizeof *words; ++i) {
    put_word(base + 4 * i, words[i]);
  }
  uint32_t count = mk4_image_u32(&image, descriptor + 4);
  put_word(descriptor + 4, 9);
  control_finished = 0;
  retail_vm_start(actor, &image, base, base + sizeof words, control_end, reject_native);
  put_word(descriptor + 4, count);
  check(fighter_sample(actor) == 2, "cursor-add must mask to its low byte and dispatcher aliases must consume no operands");
  tick();
  check(fighter_sample(actor) == 8, "final-cursor opcode must use the bound descriptor length");
  tick();
  check(control_finished == 1 && fighter_sample(actor) == 8, "final-pose submission must preserve the descriptor sample and continue dispatch");
}

static void native_task_completion(void) {
  const uint32_t base = 0x570180;
  put_word(base, 0x34000001);
  put_word(base + 4, 0x32000000);
  put_word(base + 8, 0x49cb60);
  for (int interrupted = 0; interrupted < 2; ++interrupted) {
    Fighter *actor = setup(0, 2);
    control_finished = cleanup_calls = 0;
    retail_vm_start(actor, &image, base, base + 12, control_end, retail_native_vm);
    actor->retail_vm.cleanup = cleanup_once;
    if (interrupted) {
      fighter_interrupt(actor, fighter_ready);
    }
    tick();
    check(control_finished == (interrupted ? 0 : 1) && cleanup_calls == 1 && !actor->retail_vm.active,
        "native task retirement must consume the caller completion and cleanup once, while interruption suppresses completion");
  }
  Fighter *actor = setup(0, 2);
  g_battle.phase = PHASE_FATALITY;
  g_battle.finisher = FINISH_SPIKE;
  g_battle.winner = 0;
  battle_react(actor, 175);
  tick();
  check(actor->retail_vm.active && actor->retail_vm.entry == 0x4ea300, "loaded stage native must transfer victim motion and playback to original words");
  for (int visit = 0; visit < 512 && g_battle.phase == PHASE_FATALITY; ++visit) {
    tick();
  }
  check(g_battle.phase == PHASE_MATCH_OVER && !actor->retail_vm.active && actor->current_mmove == NULL,
      "stage native continuations must join the battle result and retire all actor playback ownership");
  actor = setup(0, 2);
  retail_vm_bind_animation(actor, 0x4fad7c, 0);
  check(retail_native_vm(actor, 0x486000, RETAIL_TAIL, 0), "bounded shake native must use the shared typed boundary");
  tick();
  fighter_interrupt(actor, fighter_ready);
  check(
      actor->visual_offset[AXIS_Y] == 0 && actor->current_mmove == NULL, "interrupting a native presentation phase must remove its owned offset and playback");
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
  retail_native_entry(0x46be40)(owner);
  int clip = (int)((replacement * 4 - 0x4fad7c) / 12);
  check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e61e0 && fighter_move(owner)->clip == clip,
      "native inherited animation must come from the loaded descriptor table");
  check(
      retail_native_character_slot(16) == 2 && retail_native_character_slot(17) == 4, "native hidden-roster aliases must remain distinct from script aliases");
  fighter_cancel_mmove(owner);
  put_word(0x4e6c10, original);

  owner = setup(0, 2);
  uint32_t descriptor = 0x4fad7c;
  uint32_t count = mk4_image_u32(&image, descriptor + 4);
  put_word(descriptor + 4, 7);
  retail_vm_bind_animation(owner, descriptor, 0);
  check(retail_native_vm(owner, 0x483df0, RETAIL_TAIL, 0) && fighter_move(owner)->clip == 0,
      "shared stagger tail must retain a caller-supplied descriptor outside the historical fighter variants");
  for (int visit = 0; visit < 32 && owner->current_mmove != NULL; ++visit) {
    tick();
  }
  check(owner->current_mmove == NULL && owner->state == S_IDLE, "short inherited descriptor must complete without hard-coded clip bounds");
  put_word(descriptor + 4, count);

  owner = setup(0, 2);
  retail_vm_bind_animation(owner, descriptor, 2);
  owner->reaction = 78;
  retail_reaction_entry(&image, 78)(owner);
  check(fighter_move(owner)->clip == (int)((0x4fb700 - 0x4fad7c) / 12) && fighter_sample(owner) == 2,
      "LK victim native must bind the loaded descriptor at its inherited cursor rather than a fighter-specific starting pose");
  fighter_interrupt(owner, fighter_ready);
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
    native_script_end(owner, 0x4e6d08, fighter_ready);
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
      check(owner->retail_vm.active && owner->retail_vm.entry == 0x4e6d08,
          "native completion must enter its stored original script with the typed integration context");
    }
  }

  Fighter *owner = setup(0, 2);
  uint32_t saved[2] = {mk4_image_u32(&image, 0x4e6d08), mk4_image_u32(&image, 0x4e6d0c)};
  put_word(0x4e6d08, 0x34000002);
  put_word(0x4e6d0c, 0x30000000);
  retail_native_begin(owner, S_KICK, 0x101, false);
  retail_vm_enter_then(owner, 0x4e6d08, 0x4e5d90, fighter_ready);
  native_begin(owner, 0, 0, NULL);
  native_script_end(owner, 0x4e62f0, fighter_ready);
  for (int visit = 0; visit < 5 && owner->retail_vm.entry != 0x4e5d90; ++visit) {
    tick();
  }
  check(owner->retail_vm.entry == 0x4e5d90 && owner->retail_vm.then.entry == 0 && owner->native.script_end.entry == 0,
      "VM return continuation must stay independent of native completion data and consume once");

  static const MoveDef actor_view = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK};
  Fighter *actor = battle_scene_actor_start_script(owner, fighter_peer(owner), &actor_view, 0x4e6d08, fighter_ready);
  check(actor != NULL && actor->roster == fighter_peer(owner)->roster && actor->retail_vm.active,
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
  Fighter *capture_owner = setup(0, 2);
  Fighter *capture_peer = fighter_peer(capture_owner);
  native_hold(capture_owner, 128, NULL);
  capture_owner->action = ACTION_RETAIL_0206;
  capture_owner->flags |= FF_PAIRED;
  battle_react(capture_peer, 84);
  tick();
  check(capture_peer->retail_vm.active && capture_peer->retail_vm.entry == 0x4ea0f8, "capture native must transfer victim playback to its loaded script");
  for (int visit = 0; visit < 64 && capture_peer->retail_vm.active; ++visit) {
    tick();
  }
  check(capture_peer->current_mmove != NULL, "script tail must transfer ownership back to the shared native cycle");
  capture_owner->action = 0;
  for (int visit = 0; visit < 128 && capture_peer->state != S_IDLE; ++visit) {
    tick();
  }
  check(capture_peer->state == S_IDLE && !(capture_owner->flags & FF_PAIRED) && !(capture_peer->flags & FF_PAIRED),
      "owner exit must release both native capture contexts");

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
    } else if (strcmp(argv[1], "callback") == 0) {
      Fighter *actor = setup(0, 2);
      native_hold(actor, 1, NULL);
      native_address_end(actor, 0x4e6d08);
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
  native_getup_entries();
  native_weapon_parameters();
  native_weapon_inventory();
  native_contact_record_lifetime();
  native_projectile_context();
  native_return_ownership();
  native_sound_data();
  native_callback_table();
  native_reaction_table();
  native_stagger_boundary();
  native_task_completion();
  cursor_alias_opcodes();
  conditional_primitive_opcodes();
  motion_distance_opcodes();
  animation_controller_opcodes();
  edge_task_lifetime();
  scene_task_lifetime();
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
  puts("PASS: shared retail VM, native boundaries, loaded data, interruption, KO and reset");
  return 0;
}
