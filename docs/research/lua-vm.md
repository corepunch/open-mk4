# Lua as the MK4 script VM

Status: Lua is the script runtime. The game, the viewer self-test and every
test that runs a match open the Lua program next to the data package; the
word interpreter, the assembly code modules and the package instruction
bitmap are removed. `scripts/mk4/program.asm` is generated from the Lua
modules and holds one placeholder label per script function, so data tables,
command lists, reaction tables and natives keep referring to scripts by
linked address. `make lua-test` keeps the scenario families below as a
determinism and coverage harness: each scenario runs twice and the whole
simulation state must agree after every tick.

The data tables stay in the linked data section. They are binary record
formats read by C, and moving them to Lua is a separate data-format question
with no VM benefit.

## What the wordcode VM was, and what Lua replaced

`game/script/vm.c` is a flat state machine on `Fighter`. `ScriptVm` holds the
cursor, the wake tick, the playback plan (`play_ticks`, `target`, `stride`,
`period`, paired, blend and interpolated modes), the contact result, the
selector, one live loop counter, native parking and the typed continuations.
`script_vm_step` runs once per actor per tick: a prologue advances playback,
blends and native waits; only when nothing owns the tick does it fetch and
dispatch words until one yields. Interruption, round change and reset zero
the `ScriptVm`, bump a generation counter and run one detached cleanup.

Most of that is engine state, not instruction semantics. Lua replaces only
the word stream: which primitive runs next. Playback, waits, contact,
pairing and lifetime stay in C and are shared by both interpreters, which is
why the two can be compared tick for tick.

## Runtime

- `third_party/lua-5.4.7/`: unmodified Lua 5.4.7 (MIT) without the `lua` and
  `luac` front ends, built into `build/liblua.a`.
- `game/lua/script_lua.c`: one host-owned Lua state; one coroutine per actor
  script; 107 registered operations covering the 108 non-no-op opcodes (the
  ten no-op slots have no operation). A sandbox removes `math.random`, files
  and code loading. Each operation ends by settling: if the actor moved on
  (VM inactive, generation changed, a new script installed) the coroutine
  yields and is abandoned; if playback, a wait, a park or a pending
  interrupt owns the next ticks it yields; otherwise the script continues in
  the same tick, exactly as the word loop would. Runaway guards: 1,024
  operations per resume and 1,000,000 Lua instructions.
- Seam in the shared VM: `ScriptVm.resume`, `script_vm_set_entry_hook` in
  `script_vm_start_at`, and public wrappers for the playback primitives that
  were static (`play`, `sample`, `contact`, `slow_play`, `interpolated_play`,
  `blend`). Without a hook the VM is unchanged.
- Entry identity is still the linked address: `tools/mk4_lua_symbols.py`
  emits `build/mk4_symbols.lua` (labels, script labels and native IDs) from
  the linker, and the runtime claims the addresses it has functions for.
  Data tables, command lists, reaction tables and the 410 native call sites
  that enter scripts by address are untouched.
- Coroutines are anchored on the main stack while they run, so a native that
  starts another script from inside an operation cannot have its resumer
  collected. One coroutine per actor is kept reachable; the heap stays flat
  (1,054 KB after a full collection before and after 200 interrupted
  scripts).

## Translation

`tools/mk4_asm_to_lua.py` reads the project through the linker and writes
one Lua module per assembly module, `program.lua` listing them in link
order:

| Wordcode | Lua |
| --- | --- |
| label | `function S.label()` with the label's comments |
| `jump`, fall-through into a labelled stream | `return S.other()` |
| `loop N … repeat` | `for _ = 1, N do … end` (checked to nest inside one stream) |
| `select_roster` | `return mk4.select_roster { scorpion = S.a, … }` |
| conditional branch | `if mk4.blocked() then return S.other() end` |
| `wait_branch 0x4002` | `mk4.wait_branch(2, 4)` (miss, hit) |
| `prefixed_to WITH_CLIP + 10, clip(65)` | `mk4.forward_to(10, 65)` |
| `tail 0, MK4_NATIVE_X` | `mk4.tail(native.X)` as a statement; the function ends after it |
| embedded entry (`high_kick + 64`, `… + 4`) | its own function owning the stream after the no-op words |
| no-op opcodes | omitted |

All 3,309 instructions and 612 labels translate; with the eight embedded
entries that gives 621 functions in 6,179 lines. `make lua-scripts-check`
re-translates and compares, so the assembly and the Lua cannot drift while
both exist. The operation reference is `scripts/lua/README.md`.

## The gate

`tests/test_lua_vm.c` runs each scenario twice, with the Lua runtime
disabled and enabled, and hashes the whole `Battle` after every tick (only
the word cursor, the live loop counter and the resume routine are blanked,
since those legitimately differ). The Lua run is compared as it goes, so the
first differing tick is reported with both fighters' state and the active
script names. Scenario families, all 18 roster slots unless noted:

| Family | Inputs | Scenarios |
| --- | --- | --- |
| Buttons | six buttons × standing, crouching, back, jumping, both jumping × five ranges × both player slots × victim idle or blocking | 4,320 |
| Commands | every record of every command list, from both player tables, standing and airborne, victim idle or blocking, three ranges, and twelve follow-up presses (plain and back-held) | 5,968 |
| Victories | low-kick round wins against a one-point opponent, three seeds, both slots | 108 |
| Finishers | every patterned command entered at the finish window at touching range and the two CPU finisher distances; plus a CPU winner forced to each of its two character finishers and the spike and fan stage fatalities | 534 |
| CPU matches | three CPU-versus-CPU matches per roster slot to match over | 54 |
| Random input | three seeds per roster slot, 2,400 ticks of random holds for both players | 54 |

Results (release build, Apple M-series):

| Measure | Value |
| --- | --- |
| Scenarios compared, all identical | 16,134 |
| Ticks compared | 2,649,840 |
| Distinct linked entries started, all Lua-owned | 456 |
| Script functions executed | 549 of 621 |
| Operations exercised | 88 of 107 |
| Gate wall clock | 2 min 18 s |
| Cost per CPU-match tick including the hash | 24.3 µs wordcode, 24.4 µs Lua |
| Lua heap after a full collection | 1,054 KB, flat across 200 interrupted scripts |

The gate also passes built with `-Werror -pedantic-errors`, ASan and UBSan
(`quick` mode), and a one-tick change to a Lua wait is caught at the first
affected tick. Interrupt, KO and match reset retire the coroutine with the
VM state; a new entry replaces a stale coroutine.

Not reached by the gate: 72 functions, mostly Raiden's and Sonya's second
fatalities and a few finisher victim variants, some weapon-reaction and
launch recoveries, Kai's handstand streams, Liu Kang's throw retreat and
the LK slam release streams. They were translated by the same mechanical
rules, but their behaviour has not been compared. Nineteen operations are
not exercised because no translated script uses them (`cursor_advance`,
`cursor_last`, `submit_last`, `play_double`, `play_interleaved`,
`inherited_interleaved`, `wait_result`, `return_word`, `predicate_bit`,
`airborne`, `blocked`, `edge`, `query_branch`, `reaction_distance`,
`sound_token`, `position`, `edge_push`) or are queries for tests (`result`,
`current_sample`).

The gate found one pre-existing engine failure that is independent of Lua:
a CPU match between Cage (slot 14) and Kitana (slot 16), seed 19089178,
stops at tick 1509 in wordcode mode with `invalid animation length or
initial cursor` when `cage_shadow_victim` starts on a victim whose current
sample lies beyond its clip. The CPU seeds were changed to keep the gate
running; the failure itself is not fixed. `./build/test-lua-vm trace cpu 14
16 19089178` reproduces it. The UBSan build also reports a pre-existing
signed overflow in `game/native/attacks.c` (the orbit release heading,
`x * Q16_ONE` on an `int`), again in both interpreters.

## Fidelity notes

- `result`, `selector`, `playback_limit` and the repeat state live in C and
  persist across script starts in both interpreters. The wordcode loop
  counter also persists, but every `repeat` in the corpus follows its `loop`
  inside one stream (the translator rejects anything else), so `for` loops
  are equivalent.
- The Kitana and Noob roster slots select the Sonya and Reiko entries, as
  opcode `3c` does.
- A `tail` does not end a stream by itself; wordcode continues with the next
  word when the native returns without taking the actor over, which in the
  corpus means a guard word or the next stream. The translation keeps that:
  the function continues or falls into `return S.next()`.
- Lua `pairs` order is unspecified; the translated scripts use arrays and
  explicit keys only. Floats never cross the boundary.

## Steps taken after the gate

1. Lua is linked into the game and every test that runs a match; the host
   opens the runtime beside the data package (`--scripts` selects another
   program) and closes it at exit. The viewer does not run scripts and does
   not link Lua.
2. The assembly code modules are deleted. `tools/mk4_script_labels.py` writes
   `scripts/mk4/program.asm` from the Lua modules; the linker no longer emits
   an instruction bitmap (package version 2) and the loader no longer reads
   one. The linker still assembles instruction words for research imports of
   the original executable, which the game does not execute.
3. `game/script/vm.c` keeps the engine-owned playback step and primitives and
   calls the Lua runtime directly; a start without a Lua function is fatal.
4. The wordcode-specific tests were retired with the interpreter;
   `tests/test_script_runtime.c` keeps the native-boundary, loaded-data and
   lifetime checks, and `tests/test_lua_vm.c` keeps the scenario families as
   a determinism harness. The catalogue audit now checks that every original
   opcode has a registered Lua operation.
5. The repository rules in `AGENTS.md`, `ARCHITECTURE.md`, `scripts/README.md`
   and `docs/character-script-port.md` describe the Lua runtime.

`tools/mk4_asm_to_lua.py` remains as the research translator for assembled
original words (for example an import of the executable); it has no input in
the project tree any more.
