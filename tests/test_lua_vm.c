#include "game.h"
#include "lua/script_lua.h"
#include "script/vm.h"
#include "data/tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Lua script harness: every scenario family runs twice from the same inputs
 * and data, and the whole simulation state must agree after every tick, so
 * the scripts and runtime are deterministic and never fail across all roster
 * slots, buttons, commands, finishers, CPU matches and random input. The
 * same families proved the Lua scripts identical to the retired wordcode
 * interpreter before it was removed (docs/research/lua-vm.md). */

static Mk4Image image;

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL Lua VM: %s\n", message);
    exit(1);
  }
}

static uint32_t xorshift(uint32_t *state) {
  uint32_t x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *state = x;
  return x;
}

/* ---- simulation state hash ---- */

static Battle scratch;

static uint64_t hash_battle(void) {
  memcpy(&scratch, &g_battle, sizeof scratch);
  /* Four independent lanes keep the multiply chains short; this detects
   * any byte difference, which is all the harness needs. */
  const unsigned char *bytes = (const unsigned char *)&scratch;
  uint64_t lanes[4] = {0x9e3779b97f4a7c15ull, 0xbf58476d1ce4e5b9ull, 0x94d049bb133111ebull, 0x2545f4914f6cdd1dull};
  size_t words = sizeof scratch / (4 * sizeof(uint64_t));
  for (size_t i = 0; i < words; ++i) {
    uint64_t word[4];
    memcpy(word, bytes + i * sizeof word, sizeof word);
    for (int lane = 0; lane < 4; ++lane) {
      lanes[lane] = (lanes[lane] ^ word[lane]) * 0x100000001b3ull;
    }
  }
  uint64_t hash = lanes[0] ^ (lanes[1] << 1) ^ (lanes[2] << 2) ^ (lanes[3] << 3);
  for (size_t i = words * 4 * sizeof(uint64_t); i < sizeof scratch; ++i) {
    hash = (hash ^ bytes[i]) * 0x100000001b3ull;
  }
  return hash ^ (hash >> 31);
}

/* ---- scenarios ---- */

enum {
  KIND_BUTTON,
  KIND_CPU,
  KIND_RANDOM,
  KIND_COMMAND,
  KIND_VICTORY,
  KIND_FINISH
};

enum {
  POSE_STAND,
  POSE_CROUCH,
  POSE_BACK,
  POSE_JUMP,
  POSE_AIR_PAIR, /* both fighters jump; air throws and air-to-air contact */
  POSE_COUNT
};

static const Button buttons[] = {BUTTON_HIGH_PUNCH, BUTTON_LOW_PUNCH, BUTTON_HIGH_KICK, BUTTON_LOW_KICK, BUTTON_BLOCK, BUTTON_RUN};
static const q16 distances[] = {0xc000, 0x14000, 0x30000, 6 * Q16_ONE, 0x6000};
#define DISTANCE_COUNT 5

typedef struct Scenario {
  int kind, p1, p2, slot, stance, button, distance, max_ticks;
  bool victim_blocks, released, cpu_winner;
  int followup;           /* command scenarios: button index pressed once the command settles, or -1 */
  uint32_t seed, pattern; /* command scenarios: the retail direction pattern */
} Scenario;

typedef struct Diagnostic {
  int sample[2], state[2], action[2], phase;
  q16 health[2], x[2];
  uint32_t entry[2];
  bool active[2];
} Diagnostic;

#define MAX_TICKS 7200

typedef struct Run {
  int ticks;
  unsigned lua_entries;
  uint64_t hashes[MAX_TICKS + 1];
  Diagnostic diagnostics[MAX_TICKS + 1];
} Run;

static Run first_run, second_run;
static void describe(const Scenario *scenario);
static Scenario current_scenario_value;
static const Scenario *current_scenario;
static int current_tick;
static bool tracing;
static const Run *current_reference;

/* A fatal error inside the simulation exits; name the scenario and tick first. */
static void on_exit_report(void) {
  if (current_scenario == NULL) {
    return;
  }
  fprintf(stderr, "last scenario (%s run, tick %d): ", current_reference != NULL ? "second" : "first", current_tick);
  describe(current_scenario);
  if (current_reference != NULL && current_tick <= current_reference->ticks) {
    const Diagnostic *d = &current_reference->diagnostics[current_tick];
    fprintf(stderr, "  first run at that tick: samples %d/%d states %d/%d entries %s / %s\n", d->sample[0], d->sample[1], d->state[0], d->state[1],
        script_lua_entry_name(d->entry[0]), script_lua_entry_name(d->entry[1]));
  }
}

/* Trace mode prints each actor's script entry when it changes. */
static uint32_t traced_entries[2];

static void trace_entries(void) {
  for (int slot = 0; slot < 2; ++slot) {
    const Fighter *fighter = &g_battle.fighters[slot];
    uint32_t entry = fighter->execution.script.active ? fighter->execution.script.entry : 0;
    if (entry != traced_entries[slot]) {
      traced_entries[slot] = entry;
      if (entry != 0) {
        fprintf(stderr, "tick %d fighter %d sample %d enters %08x %s\n", current_tick, slot, fighter_sample(fighter), entry, script_lua_entry_name(entry));
      }
    }
  }
}

static void diagnose(Diagnostic *out) {
  memset(out, 0, sizeof *out);
  for (int i = 0; i < 2; ++i) {
    const Fighter *fighter = &g_battle.fighters[i];
    out->sample[i] = fighter_sample(fighter);
    out->state[i] = fighter->state;
    out->action[i] = fighter->action;
    out->health[i] = fighter->health;
    out->x[i] = fighter->body.pos[AXIS_X];
    out->entry[i] = fighter->execution.script.entry;
    out->active[i] = fighter->execution.script.active;
  }
  out->phase = (int)g_battle.phase;
}

static bool settled(const Fighter *fighter) {
  return fighter->state == S_IDLE && !fighter->execution.script.active && fighter->execution.frames == NULL && fighter->pending == NULL;
}

/* Standing HK within 0xc000 selects the native close knee instead; 0x14000 reaches the roster selector and still lands. */
static void start(const Scenario *scenario) {
  battle_init(&image, scenario->p1, scenario->p2);
  g_battle.data = &image;
  if (scenario->kind == KIND_BUTTON || scenario->kind == KIND_COMMAND) {
    g_battle.phase = PHASE_FIGHT;
    q16 distance = distances[scenario->distance];
    g_battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
    g_battle.fighters[1].body.pos[AXIS_X] = distance / 2;
  } else {
    g_battle.random[0] = scenario->seed;
    g_battle.random[1] = scenario->seed * 2654435761u + 1u;
    if (scenario->kind == KIND_CPU) {
      g_battle.fighters[0].human = g_battle.fighters[1].human = false;
    }
    if (scenario->cpu_winner) {
      g_battle.fighters[scenario->slot].human = false;
      /* Seeds 2 and 3 select the spike and fan stage fatalities by arena. */
      battle_set_arena(scenario->seed % 4 == 2 ? 0 : scenario->seed % 4 == 3 ? 8 : 1);
    }
  }
}

/* A press needs the switch clear in the previous two samples, so each
 * direction token is held one tick with a two-tick gap; the button follows
 * the last direction on the next tick, before an Up token becomes a jump.
 * A release-list command releases the button two ticks later. Jump stances
 * leave the ground first so air commands can match. */
static int command_press_tick(const Scenario *scenario) {
  int base = scenario->stance == POSE_JUMP ? 12 : 2, step = 0;
  for (uint32_t pattern = scenario->pattern; pattern != 0 && pattern != 0xffffffffu; pattern >>= 8) {
    step++;
  }
  return step == 0 ? base : base + (step - 1) * 3 + 1;
}

static unsigned command_inputs(const Scenario *scenario, int tick) {
  int slot = scenario->slot, base = scenario->stance == POSE_JUMP ? 12 : 2, step = 0;
  unsigned hold = 0;
  bool facing_left = g_battle.fighters[1 - slot].body.pos[AXIS_X] < g_battle.fighters[slot].body.pos[AXIS_X];
  for (uint32_t pattern = scenario->pattern; pattern != 0 && pattern != 0xffffffffu; pattern >>= 8, ++step) {
    unsigned token = pattern & 255u;
    Button direction = token == 1      ? BUTTON_UP
                       : token == 2    ? BUTTON_DOWN
                       : token == 0xaa ? (facing_left ? BUTTON_RIGHT : BUTTON_LEFT)
                                       : (facing_left ? BUTTON_LEFT : BUTTON_RIGHT);
    if (tick == base + step * 3) {
      hold |= BUTTON_BIT(direction);
    }
  }
  int press = step == 0 ? base : base + (step - 1) * 3 + 1;
  if (tick >= press && tick < press + 2) {
    hold |= BUTTON_BIT(buttons[scenario->button]);
  }
  if (scenario->released && tick >= press + 2 && tick < press + 4) {
    hold |= BUTTON_BIT(buttons[scenario->button]);
  }
  return hold;
}

/* Inputs for one tick; returns false once the scenario should stop. */
static bool drive(const Scenario *scenario, int tick, InputFrame input[2], uint32_t *state, uint32_t *held) {
  input[0].held = input[1].held = 0;
  if (scenario->kind == KIND_BUTTON) {
    int slot = scenario->slot, press = scenario->stance >= POSE_JUMP ? 14 : 2;
    unsigned hold = 0;
    /* Stance keys are released after the press so both actors can settle. */
    if (scenario->stance == POSE_CROUCH && tick < press + 2) {
      hold = BUTTON_BIT(BUTTON_DOWN);
    } else if (scenario->stance == POSE_BACK && tick < press + 2) {
      hold = BUTTON_BIT(slot == 0 ? BUTTON_LEFT : BUTTON_RIGHT);
    } else if (scenario->stance >= POSE_JUMP && tick < 2) {
      hold = BUTTON_BIT(BUTTON_UP);
    }
    if (tick >= press && tick < press + 2) {
      hold |= BUTTON_BIT(buttons[scenario->button]);
    }
    input[slot].held = hold;
    if (scenario->stance == POSE_AIR_PAIR && tick < 2) {
      input[1 - slot].held = BUTTON_BIT(BUTTON_UP);
    }
    if (scenario->victim_blocks && tick < press + 12) {
      input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
    }
    if (tick > press + 4 && settled(&g_battle.fighters[0]) && settled(&g_battle.fighters[1])) {
      return false;
    }
    return tick < scenario->max_ticks;
  }
  if (scenario->kind == KIND_VICTORY || scenario->kind == KIND_FINISH) {
    /* Win rounds with low kicks against a one-point opponent who never acts:
     * walk in when far, kick when near. A finish scenario then stands at its
     * distance in the finish window and enters the command pattern. */
    int slot = scenario->slot;
    Fighter *winner = &g_battle.fighters[slot], *loser = &g_battle.fighters[1 - slot];
    if (g_battle.phase == PHASE_FIGHT && g_battle.phase_ticks == 0) {
      loser->health = 1;
    }
    q16 gap = winner->body.pos[AXIS_X] - loser->body.pos[AXIS_X];
    bool left = gap < 0;
    gap = left ? -gap : gap;
    if (!scenario->cpu_winner && g_battle.phase == PHASE_FIGHT && held[0] == 0 && !winner->execution.script.active && winner->execution.frames == NULL) {
      if (gap > 0xc000) {
        input[slot].held = BUTTON_BIT(left ? BUTTON_RIGHT : BUTTON_LEFT);
      } else if (tick % 20 == 2) {
        input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
      }
    }
    if (scenario->kind == KIND_FINISH && scenario->cpu_winner && held[0] == 0 && battle_can_finish(winner)) {
      /* Choose the CPU's finisher directly instead of its arcade chance roll;
       * the CPU then approaches and performs it. */
      held[0] = (uint32_t)tick;
      int action = scenario->seed % 4 == 2 ? CPU_FINISH_SPIKE : scenario->seed % 4 == 3 ? CPU_FINISH_FAN : scenario->seed % 2 ? CPU_FINISH_2 : CPU_FINISH_1;
      winner->cpu.finish_chosen = true;
      winner->cpu.plan = action + 1;
      winner->cpu.target =
          action == CPU_FINISH_1 || action == CPU_FINISH_2 ? mk4_cpu_finish_distance(&image, (unsigned)winner->roster, action == CPU_FINISH_2) : 0xb333;
      winner->cpu.timer = 240;
    }
    if (scenario->kind == KIND_FINISH && !scenario->cpu_winner) {
      if (held[0] == 0 && battle_can_finish(winner)) {
        held[0] = (uint32_t)tick;
        q16 distance = scenario->distance == 0 ? 0x8000 : mk4_cpu_finish_distance(&image, (unsigned)winner->roster, scenario->distance == 2);
        winner->body.pos[AXIS_X] = loser->body.pos[AXIS_X] + (left ? -distance : distance);
        winner->body.last_inside[AXIS_X] = winner->body.pos[AXIS_X];
      }
      if (held[0] != 0) {
        input[slot].held = command_inputs(scenario, tick - (int)held[0]);
      }
    }
    if (g_battle.phase == PHASE_MATCH_OVER && g_battle.phase_ticks > 900) {
      return false;
    }
    return tick < scenario->max_ticks;
  }
  if (scenario->kind == KIND_COMMAND) {
    int slot = scenario->slot, press = command_press_tick(scenario);
    unsigned hold = command_inputs(scenario, tick);
    if (scenario->stance == POSE_JUMP && tick < 2) {
      hold |= BUTTON_BIT(BUTTON_UP);
    }
    if (scenario->followup >= 0 && tick > press + 6 && settled(&g_battle.fighters[slot])) {
      /* One follow-up press once the command settles (weapon attacks, stance changes). */
      if (held[1] == 0) {
        held[1] = (uint32_t)tick;
      }
      if (tick < (int)held[1] + 2) {
        hold |= BUTTON_BIT(buttons[scenario->followup % 6]);
      }
      if (scenario->followup >= 6 && tick < (int)held[1] + 4) {
        /* Back-held follow-ups reach the armed back+button attacks. */
        bool facing_left = g_battle.fighters[1 - slot].body.pos[AXIS_X] < g_battle.fighters[slot].body.pos[AXIS_X];
        hold |= BUTTON_BIT(facing_left ? BUTTON_RIGHT : BUTTON_LEFT);
      }
    }
    input[slot].held = hold;
    if (scenario->victim_blocks && tick < press + 12) {
      input[1 - slot].held = BUTTON_BIT(BUTTON_BLOCK);
    }
    bool followed = scenario->followup < 0 || (held[1] != 0 && tick > (int)held[1] + 6);
    if (followed && tick > press + 6 && settled(&g_battle.fighters[0]) && settled(&g_battle.fighters[1])) {
      return false;
    }
    return tick < scenario->max_ticks;
  }
  if (scenario->kind == KIND_RANDOM) {
    for (int i = 0; i < 2; ++i) {
      if (xorshift(state) % 6 == 0) {
        uint32_t roll = xorshift(state);
        unsigned mask = 0;
        if (roll % 5 != 0) {
          unsigned direction = (roll >> 3) % 9;
          static const unsigned directions[9] = {0, BUTTON_BIT(BUTTON_UP), BUTTON_BIT(BUTTON_DOWN), BUTTON_BIT(BUTTON_LEFT), BUTTON_BIT(BUTTON_RIGHT),
              BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_LEFT), BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_RIGHT),
              BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LEFT), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RIGHT)};
          mask = directions[direction];
          for (unsigned b = 0; b < 6; ++b) {
            if ((roll >> (8 + 3 * b)) % 4 == 0) {
              mask |= BUTTON_BIT(buttons[b]);
            }
          }
        }
        held[i] = mask;
      }
      input[i].held = held[i];
    }
  }
  if (g_battle.phase == PHASE_MATCH_OVER && g_battle.phase_ticks > 900) {
    return false;
  }
  return tick < scenario->max_ticks;
}

static void report_divergence(const Scenario *scenario, const Run *reference, const Run *other, int t);

/* With a reference run, every tick is compared as soon as it is simulated,
 * so a divergence is reported before a later fatal error can hide it. */
static void run_against(Run *out, const Scenario *scenario, const Run *reference) {
  current_scenario_value = *scenario;
  current_scenario = &current_scenario_value;
  current_reference = reference;
  current_tick = 0;
  unsigned before = script_lua_entries();
  start(scenario);
  uint32_t state = scenario->seed | 1u, held[2] = {0, 0};
  InputFrame input[2] = {{0}, {0}};
  int tick = 0;
  out->hashes[0] = hash_battle();
  diagnose(&out->diagnostics[0]);
  while (drive(scenario, tick, input, &state, held)) {
    Phase before_phase = g_battle.phase;
    battle_step(input);
    tick++;
    current_tick = tick;
    if (tracing) {
      trace_entries();
      if (g_battle.phase != before_phase) {
        fprintf(stderr, "tick %d phase %d -> %d (health %x/%x)\n", tick, (int)before_phase, (int)g_battle.phase, g_battle.fighters[0].health,
            g_battle.fighters[1].health);
      }
    }
    out->hashes[tick] = hash_battle();
    diagnose(&out->diagnostics[tick]);
    if (reference != NULL && (tick > reference->ticks || reference->hashes[tick] != out->hashes[tick])) {
      report_divergence(scenario, reference, out, tick);
    }
  }
  out->ticks = tick;
  out->lua_entries = script_lua_entries() - before;
}

static void run(Run *out, const Scenario *scenario) {
  run_against(out, scenario, NULL);
}

static const char *kind_names[] = {"button", "cpu", "random", "command", "victory", "finish"};

static void describe(const Scenario *scenario) {
  fprintf(stderr, "scenario %s p1=%d p2=%d slot=%d stance=%d button=%d distance=%d victim_blocks=%d seed=%u pattern=%08x released=%d\n",
      kind_names[scenario->kind], scenario->p1, scenario->p2, scenario->slot, scenario->stance, scenario->button, scenario->distance, scenario->victim_blocks,
      scenario->seed, scenario->pattern, scenario->released);
}

static unsigned long compared_ticks, compared_scenarios, lua_total;

static void report_divergence(const Scenario *scenario, const Run *reference, const Run *other, int t) {
  describe(scenario);
  const Diagnostic *a = &reference->diagnostics[t > reference->ticks ? reference->ticks : t], *b = &other->diagnostics[t];
  fprintf(stderr, "first difference at tick %d (phase %d/%d)\n", t, a->phase, b->phase);
  for (int i = 0; i < 2; ++i) {
    fprintf(stderr, "  fighter %d: sample %d/%d state %d/%d action %x/%x health %x/%x x %x/%x active %d/%d entry %08x %s / %08x %s\n", i, a->sample[i],
        b->sample[i], a->state[i], b->state[i], a->action[i], b->action[i], a->health[i], b->health[i], a->x[i], b->x[i], a->active[i], b->active[i],
        a->entry[i], script_lua_entry_name(a->entry[i]), b->entry[i], script_lua_entry_name(b->entry[i]));
  }
  check(false, "two runs of one scenario diverged");
}

static void compare(const Scenario *scenario) {
  run(&first_run, scenario);
  run_against(&second_run, scenario, &first_run);
  check(first_run.lua_entries == second_run.lua_entries, "both runs must start the same scripts");
  if (first_run.ticks != second_run.ticks) {
    describe(scenario);
    fprintf(stderr, "first run %d ticks, second %d\n", first_run.ticks, second_run.ticks);
    check(false, "two runs of one scenario diverged in length");
  }
  compared_ticks += (unsigned long)second_run.ticks;
  compared_scenarios++;
  lua_total += second_run.lua_entries;
}

static void equivalence_buttons(void) {
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    int other = roster == 4 ? 0 : 4;
    for (int slot = 0; slot < 2; ++slot) {
      for (int stance = 0; stance < POSE_COUNT; ++stance) {
        for (int button = 0; button < 6; ++button) {
          for (int distance = 0; distance < DISTANCE_COUNT; ++distance) {
            Scenario scenario = {.kind = KIND_BUTTON,
                .p1 = slot ? other : roster,
                .p2 = slot ? roster : other,
                .slot = slot,
                .stance = stance,
                .button = button,
                .distance = distance,
                .max_ticks = 400,
                .victim_blocks = (roster + button + distance) % 2 != 0,
                .followup = -1};
            compare(&scenario);
          }
        }
      }
    }
  }
  printf("buttons: %lu scenarios, %lu ticks, %lu script entries\n", compared_scenarios, compared_ticks, lua_total);
}

/* Every command record of every roster slot, from both player tables, at
 * three ranges, standing and airborne, victim idle or blocking, with
 * follow-up presses. Predicates that reject the state fall through to the
 * next record as they do in play. */
static void equivalence_commands(void) {
  unsigned long scenarios = compared_scenarios, ticks = compared_ticks;
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    int other = roster == 4 ? 0 : 4;
    for (int slot = 0; slot < 2; ++slot) {
      for (int button = 0; button < 6; ++button) {
        for (int released = 0; released < 2; ++released) {
          Mk4CommandCursor cursor = mk4_command_cursor(&image, (unsigned)roster, (unsigned)slot, buttons[button], released != 0);
          Mk4Command command;
          while (mk4_command_next(&image, &cursor, &command)) {
            for (int distance = 0; distance < 3; ++distance) {
              for (int stance = 0; stance < 4; ++stance) {
                for (int followup = -1; followup < (stance == 0 && distance == 0 ? 12 : 0); ++followup) {
                  Scenario scenario = {.kind = KIND_COMMAND,
                      .p1 = slot ? other : roster,
                      .p2 = slot ? roster : other,
                      .slot = slot,
                      .stance = stance & 1 ? POSE_JUMP : POSE_STAND,
                      .button = button,
                      .distance = distance == 2 ? 4 : distance,
                      .max_ticks = 400,
                      .released = released != 0,
                      .victim_blocks = stance >= 2,
                      .followup = followup,
                      .pattern = command.pattern};
                  compare(&scenario);
                }
              }
            }
          }
        }
      }
    }
  }
  printf("commands: %lu scenarios, %lu ticks\n", compared_scenarios - scenarios, compared_ticks - ticks);
}

static void equivalence_victories(void) {
  unsigned long scenarios = compared_scenarios, ticks = compared_ticks;
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    int other = roster == 4 ? 0 : 4;
    for (int slot = 0; slot < 2; ++slot) {
      for (int seed = 0; seed < 3; ++seed) {
        Scenario scenario = {.kind = KIND_VICTORY,
            .p1 = slot ? other : roster,
            .p2 = slot ? roster : other,
            .slot = slot,
            .max_ticks = 4000,
            .seed = 0x51u + (uint32_t)(seed * 977),
            .followup = -1};
        compare(&scenario);
      }
    }
    /* A CPU winner performs its first and second character finisher and both stage fatalities. */
    for (int seed = 0; seed < 4; ++seed) {
      Scenario scenario = {
          .kind = KIND_FINISH, .p1 = roster, .p2 = other, .slot = 0, .cpu_winner = true, .max_ticks = 6000, .seed = (uint32_t)seed, .followup = -1};
      compare(&scenario);
    }
  }
  printf("victories and CPU finishers: %lu scenarios, %lu ticks\n", compared_scenarios - scenarios, compared_ticks - ticks);
}

/* Every patterned command of the winner at the finish window, at touching
 * range and at the two CPU finisher distances; predicates select which run. */
static void equivalence_finishers(void) {
  unsigned long scenarios = compared_scenarios, ticks = compared_ticks;
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    int other = roster == 4 ? 0 : 4;
    for (int button = 0; button < 6; ++button) {
      for (int released = 0; released < 2; ++released) {
        Mk4CommandCursor cursor = mk4_command_cursor(&image, (unsigned)roster, 0, buttons[button], released != 0);
        Mk4Command command;
        while (mk4_command_next(&image, &cursor, &command)) {
          if (command.pattern == 0xffffffffu) {
            continue;
          }
          for (int distance = 0; distance < 3; ++distance) {
            Scenario scenario = {.kind = KIND_FINISH,
                .p1 = roster,
                .p2 = other,
                .slot = 0,
                .button = button,
                .distance = distance,
                .max_ticks = 4000,
                .released = released != 0,
                .followup = -1,
                .pattern = command.pattern};
            compare(&scenario);
          }
        }
      }
    }
  }
  printf("finishers: %lu scenarios, %lu ticks\n", compared_scenarios - scenarios, compared_ticks - ticks);
}

static void equivalence_cpu(int pairs_per_roster) {
  unsigned long scenarios = compared_scenarios, ticks = compared_ticks;
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    for (int n = 0; n < pairs_per_roster; ++n) {
      int opponent = (roster * 7 + 3 + n * 5) % MK4_ROSTER_COUNT;
      Scenario scenario = {
          .kind = KIND_CPU, .p1 = roster, .p2 = opponent, .max_ticks = MAX_TICKS, .seed = 0x2468ace1u + (uint32_t)(roster * 37 + n * 101), .followup = -1};
      compare(&scenario);
    }
  }
  printf("cpu matches: %lu scenarios, %lu ticks\n", compared_scenarios - scenarios, compared_ticks - ticks);
}

static void equivalence_random(int seeds_per_roster) {
  unsigned long scenarios = compared_scenarios, ticks = compared_ticks;
  for (int roster = 0; roster < MK4_ROSTER_COUNT; ++roster) {
    for (int n = 0; n < seeds_per_roster; ++n) {
      int opponent = (roster * 5 + 1 + n * 7) % MK4_ROSTER_COUNT;
      Scenario scenario = {
          .kind = KIND_RANDOM, .p1 = roster, .p2 = opponent, .max_ticks = 2400, .seed = 0xbeef00u + (uint32_t)(roster * 17 + n), .followup = -1};
      compare(&scenario);
    }
  }
  printf("random input: %lu scenarios, %lu ticks\n", compared_scenarios - scenarios, compared_ticks - ticks);
}

/* ---- coverage ---- */

static unsigned entries_seen, operations_used, operations_total, functions_run, functions_total;
static FILE *report;

static void visit_entry(uint32_t address, const char *name, unsigned count, bool owned) {
  entries_seen++;
  (void)owned;
  if (report != NULL) {
    fprintf(report, "%08x %s %u\n", address, name, count);
  }
}

static void visit_function(const char *name, unsigned count) {
  functions_total++;
  if (count != 0) {
    functions_run++;
  }
  if (report != NULL) {
    fprintf(report, "function %s %u\n", name, count);
  }
}

static void visit_operation(const char *name, unsigned count) {
  operations_total++;
  if (count != 0) {
    operations_used++;
  } else {
    printf("  operation never exercised: %s\n", name);
  }
}

static void coverage(void) {
  const char *path = getenv("MK4_GATE_REPORT");
  report = path != NULL ? fopen(path, "w") : NULL;
  script_lua_visit_entries(visit_entry);
  script_lua_visit_functions(visit_function);
  if (report != NULL) {
    fclose(report);
    report = NULL;
  }
  script_lua_visit_operations(visit_operation);
  printf("coverage: %u distinct entries started; %u of %u script functions executed; %u of %u operations exercised\n", entries_seen, functions_run,
      functions_total, operations_used, operations_total);
  fflush(stdout);
}

/* ---- lifetime and memory ---- */

static void tick(void) {
  InputFrame input[2] = {0};
  battle_step(input);
}

static Fighter *setup_kick(int slot) {
  battle_init(&image, slot ? 4 : 0, slot ? 0 : 4);
  g_battle.phase = PHASE_FIGHT;
  g_battle.data = &image;
  g_battle.fighters[0].body.pos[AXIS_X] = -6 * Q16_ONE / 2;
  g_battle.fighters[1].body.pos[AXIS_X] = 6 * Q16_ONE / 2;
  InputFrame input[2] = {0};
  input[slot].held = BUTTON_BIT(BUTTON_LOW_KICK);
  battle_step(input);
  input[slot].held = 0;
  battle_step(input);
  return &g_battle.fighters[slot];
}

static void lifetimes(void) {
  for (int slot = 0; slot < 2; ++slot) {
    Fighter *owner = setup_kick(slot);
    for (int n = 0; n < 8; ++n) {
      tick();
    }
    check(owner->avoids_high && owner->execution.script.active, "low kick must reach its avoidance window");
    fighter_interrupt(owner, fighter_ready);
    check(!owner->execution.script.active && !owner->avoids_high, "interrupt must drop the coroutine with the script state");
    for (int n = 0; n < 60; ++n) {
      tick();
    }
    check(owner->state == S_IDLE, "an abandoned coroutine must never resume");
    owner = setup_kick(slot);
    battle_reset_match();
    check(!owner->execution.script.active, "match reset must retire the script");
    g_battle.phase = PHASE_FIGHT;
    for (int n = 0; n < 60; ++n) {
      tick();
    }
    check(!owner->execution.script.active && owner->state == S_IDLE, "reset script must stay retired");
  }
}

static void memory(void) {
  unsigned before = script_lua_memory_kb();
  for (int n = 0; n < 200; ++n) {
    Fighter *owner = setup_kick(n % 2);
    for (int t = 0; t < (n % 7); ++t) {
      tick();
    }
    fighter_interrupt(owner, fighter_ready);
    tick();
  }
  unsigned after = script_lua_memory_kb();
  printf("Lua heap after collection: %u KB before, %u KB after 200 interrupted scripts\n", before, after);
  check(after <= before + 64, "abandoned coroutines must be collected");
}

static void benchmark(void) {
  Scenario scenario = {.kind = KIND_CPU, .p1 = 0, .p2 = 4, .max_ticks = 1800, .seed = 99u, .followup = -1};
  clock_t begin = clock();
  for (int n = 0; n < 5; ++n) {
    run(&second_run, &scenario);
  }
  double micros = (double)(clock() - begin) * 1e6 / CLOCKS_PER_SEC / (5.0 * (double)second_run.ticks);
  printf("%.2f microseconds per tick of a CPU match (including the state hash)\n", micros);
}

int main(int argc, char **argv) {
  bool quick = argc > 1 && strcmp(argv[1], "quick") == 0, extended = argc > 1 && strcmp(argv[1], "long") == 0;
  image = mk4_image_load("build/mk4.data");
  script_lua_set_coverage(true);
  script_lua_open("build/mk4_symbols.lua", "scripts/lua/program.lua");
  atexit(on_exit_report);
  if ((argc == 6 || argc == 11) && strcmp(argv[1], "trace") == 0) {
    /* trace <cpu|random|victory|cpufinish> <p1> <p2> <seed>, or trace <command|finish> <p1> <p2>
     * <slot> <button> <released> <pattern hex> <distance> <stance>: print every script entry. */
    bool cpu_finish = strcmp(argv[2], "cpufinish") == 0;
    int kind = strcmp(argv[2], "cpu") == 0                    ? KIND_CPU
               : strcmp(argv[2], "random") == 0               ? KIND_RANDOM
               : strcmp(argv[2], "victory") == 0              ? KIND_VICTORY
               : strcmp(argv[2], "finish") == 0 || cpu_finish ? KIND_FINISH
                                                              : KIND_COMMAND;
    Scenario scenario = {.kind = kind,
        .p1 = atoi(argv[3]),
        .p2 = atoi(argv[4]),
        .max_ticks = kind == KIND_VICTORY ? 4000 : MAX_TICKS,
        .seed = (uint32_t)atoi(argv[5]),
        .cpu_winner = cpu_finish,
        .followup = -1};
    if (argc == 11) {
      scenario = (Scenario){.kind = kind,
          .p1 = atoi(argv[3]),
          .p2 = atoi(argv[4]),
          .slot = atoi(argv[5]),
          .button = atoi(argv[6]),
          .released = atoi(argv[7]) != 0,
          .pattern = (uint32_t)strtoul(argv[8], NULL, 16),
          .distance = atoi(argv[9]),
          .stance = atoi(argv[10]) != 0 ? POSE_JUMP : POSE_STAND,
          .followup = -1,
          .max_ticks = kind == KIND_FINISH ? 4000 : 400};
    }
    tracing = true;
    fprintf(stderr, "=== first run ===\n");
    run(&first_run, &scenario);
    memset(traced_entries, 0, sizeof traced_entries);
    fprintf(stderr, "=== second run ===\n");
    run_against(&second_run, &scenario, &first_run);
    puts("trace complete without divergence");
    return 0;
  }
  equivalence_buttons();
  equivalence_commands();
  equivalence_victories();
  equivalence_finishers();
  equivalence_cpu(quick ? 1 : extended ? 9 : 3);
  equivalence_random(quick ? 1 : extended ? 12 : 3);
  coverage();
  lifetimes();
  memory();
  benchmark();
  script_lua_close();
  battle_init(&image, 0, 4);
  mk4_image_free(&image);
  printf("PASS: Lua scripts ran deterministically over %lu scenarios and %lu ticks; interruption, reset and re-entry retire coroutines\n", compared_scenarios,
      compared_ticks);
  return 0;
}
