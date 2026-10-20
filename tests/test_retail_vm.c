#include "game.h"
#include "retail_vm.h"
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
        check(owner->retail_vm.begin == (crouch ? 0x4e60b8u : 0x4e6d08u), "button entry selected the wrong retail root");
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
    check(g_battle.fighters[1 - slot].health == 0 && !owner->retail_vm.active, "KO must allow bounded owner recovery");
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
      for (uint32_t at = 0x4e6d08; at < 0x4e6d9c; at += 4) {
        put_word(at, 0x1f000000);
      }
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
  lifetimes();
  assets_bound();
  battle_init(0, 4);
  mk4_image_free(&image);
  puts("PASS: direct Scorpion scripts, hit/block/miss, both slots, runtime loop words, interruption, KO and reset");
  return 0;
}
