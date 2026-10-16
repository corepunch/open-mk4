# MK4 architecture and implementation alignment

## Language decision

Use C for the game and engine. The PC reference reconstructs the game as C
translation units, compiled with Microsoft Visual C++ 5.0 SP3. Visual C++ names
the C/C++ toolchain, not the input language. This is strong evidence for a
C-style engine; it does not prove that every original source file was C, or
establish the language of the separate arcade release. The reference also uses
x86 assembly where needed for byte matching. Our portable code does not need
those compiler-specific matching tricks. Use C11 with a current C compiler.

Evidence in the pinned reference:

- [README](reference/mk4decomp/README.md): reconstructed C sources and matching
  build methodology. Its matching linker overlays compiled functions on a PE
  scaffold; a byte-identical output is not recovery of the original source.
- [Compiler identification](reference/mk4decomp/analysis/notes/architecture.md):
  MSVC CRT and linker evidence. The compiler brand alone cannot distinguish C
  from C++.
- [Tick entry point](reference/mk4decomp/src/game/tick.c) and
  [tick declarations](reference/mk4decomp/include/game/tick.h): procedural entry
  points, globals, and explicit dispatch.

The supplied executable is a different revision from the reference's target.
Addresses must be checked against `MK4/MK-4.EXE`; upstream notes are hypotheses
until confirmed by source and local fixtures.

## Inspection of the supplied retail executable

`python3 tools/inspect_retail.py` repeats the read-only PE/string inspection.
Add `--disassemble-entry` for x86 entry-point disassembly using Capstone. This
is targeted binary analysis, not recovery of the original source files.

Inspected `MK4/MK-4.EXE` SHA-256:
`4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d`.

| Evidence in this file | Observation | What it establishes |
| --- | --- | --- |
| PE optional header | Linker 5.10; x86 PE32; entry VA `0x004c5240` | Consistent with the MSVC 5.x toolchain identified by the reference; not a language or exact service-pack identifier. |
| File offset `0x000cdc8c` | `Microsoft Visual C++ Runtime Library` | MSVC runtime branding; both C and C++ applications can link this runtime. |
| COFF and PE debug/export directories | Symbol count zero; debug and export directories absent | No retained source-level debug map or exported function names to recover here. |
| ASCII and UTF-16LE string scans | No credible `.c`, `.cpp`, `.cc`, or `.cxx` source filename candidates | The proposed filename-extension check is inconclusive in this stripped build. |
| Asset path literals | `c:\source\mk4\win\anim\`, `menu.tga`, `geogfx\%s`, `sound\pcmk%04ld.esf` | Retained development asset root, not source filenames. |
| CRT error table | Includes `pure virtual function call` at file offset `0x000cdae3` | Generic runtime text; not evidence of game-defined virtual classes. |
| C++ feature scans | No RTTI class-name candidates or raw `0xe06d7363` throw marker | No positive signal from these scans; C++ can omit both features. |

There is one raw `0x19930520` marker at file offset `0x001199e0`, VA
`0x0051afe0`. Do not label it a C++ FuncInfo structure merely from that magic.
Its local code references at `0x004c513f` and `0x004c5148` load the record into
EBX; the code at `0x004c5150` stores ECX, EAX and EBP into its fields. That
matches CRT nonlocal-goto/unwind notification bookkeeping, not proof of a
C++ gameplay exception handler. The reference likewise initializes its
`g_sehUnwindBase` to this value in `src/data.c`.

Conclusion: the retail file supports the MSVC-era toolchain finding, but does
not provide definitive original-source language evidence. The procedural C
reconstruction is the strongest available architectural guide. We choose C
for compatibility with that structure without claiming the binary proves
all original translation units were C.

## Retail structure and our boundaries

| Retail PC subsystem | Our implementation | Assessment |
| --- | --- | --- |
| Platform main loop and timed logic dispatch | SDL host and fixed 60 Hz battle loop | Keep host timing and input separate from game state. |
| `GameLogicStep`, menu state machine, `GameTick` | Battle phases and simulation tick | Same procedural boundary; our phases are a small independent simulation, not the retail scripts. |
| `FightFrameStep`, entity tree callbacks, queued event dispatch/commit | Two fighter records, move records, hit snapshot | Retail has a larger callback/script system. Preserve simultaneous hits; do not claim our update order matches retail. |
| FSYS filename hash and sorted archive index | Shared package module | Already verified against supplied archive and hash routines. |
| GEO loading, executable rig records, AP0/AP1 decoding | Shared asset module | Preserve format checks, rig channel lookup, root C/descendant B transform setup and cosine table interpretation. |
| Pose hierarchy, matrix composition, renderer dispatch | Shared pose math and software renderer | Pose formulas have original x86 fixtures. SDL presentation and modern rendering are portable replacements. |
| Audio, character move scripts, weapons, projectiles, arena callbacks | Not implemented | Language/structure alignment does not supply missing retail behavior. |

Source paths for retail flow:

- [Game logic](reference/mk4decomp/src/game/tick.c) updates the top-level state
  machine, input/audio and transform chain before the active fight tick.
- [Fight tick](reference/mk4decomp/src/game/game_tick.c) invokes fight stepping,
  event dispatch and post-fight/player helpers, with pause checks between stages.
- [Fight frame](reference/mk4decomp/src/game/fight.c),
  [event queue](reference/mk4decomp/src/game/event_queue.c) and
  [scenegraph types](reference/mk4decomp/include/engine/scenegraph.h) expose the
  callback-driven entity architecture. Packed pointer indices and fixed virtual
  addresses belong to the original 32-bit representation, not our host API.
- [Pose pipeline](reference/mk4decomp/src/engine/anim_pose_pipeline.c) and the
  verified paths in [FORMATS](viewer/FORMATS.md) establish the animation boundary.
  Some native-port comments describe older stub states; verify actual routines.

## Implementation rules

Use plain structs for state, enums for states, const data tables, and functions
that receive their context explicitly. No C++ classes, STL, references or
exceptions in migrated game/engine modules. Keep simulation independent of SDL
and rendering, and keep archive ownership separate from decoded assets.

Do not reproduce the original global-address memory model or introduce an ECS
framework merely to imitate the retail callback trees. Add an explicit event
queue or script interpreter when a verified gameplay behavior needs it. Keep
new combat values labeled as our own until measured against retail.

Migration must preserve existing archive rejection behavior, decoded animation
words, original matrix fixtures, battle regressions, and deterministic rendering.
Each verified migration step is committed separately on `main`.

## Current alignment

The game, shared archive/assets/pose/renderer modules, SDL host, viewer and
native regression tests now compile as C11. `Battle`, `Assets`, `Canvas`, and
`SdlApp` have explicit context APIs; decoded assets and SDL resources have
explicit owners and cleanup. Pose output is a bounded caller-owned buffer,
so posing does not allocate per call. The package validator returns an error
for the standalone watcher; invalid asset loads terminate CLI applications
with a diagnostic. Shadow projection reports invalid geometry through `bool`.

The match itself is one registry of character definitions in `game/roster.c`.
A definition names its game (`GAME_MK4` today, `GAME_WARGODS` reserved), asset,
motion, immutable moves, command table and optional state-entry callbacks.
Each `game/c_*.c` owns that character's moves and custom functions;
`game/mk4_common.c` owns shared MK4 motion and commands. Scorpion and Sub-Zero
have explicit jab/kick entry callbacks. Other entries select a default move
through the shared runner. Goro names clips from his separate animation bank.
Spawn writes the roster index and initializes `Fighter.current_move`.
`game/moves.c` owns move entry, frame/end callbacks and sample selection.
Scorpion and Sub-Zero high kicks use sparse playback segments measured from
the supplied x86 VM and native recovery code. Scorpion reverses to sample 0;
Sub-Zero continues to sample 27. Their uninterrupted no-contact paths finish
at playback steps 30 and 29. Scorpion's jab, sweep and uppercut also have
recovered playback, with buffered jab repeats and uppercut miss/block/hit
recovery loops. All 27 normal-move branch scenarios match saved original VM,
native-continuation and task-scheduler fixtures. Contact results in the oracle
are injected; collision, damage, victim reactions and hitstop remain prototype
rules. Sparse events request contact in simulation; one-shot continuations can
run after simultaneous hit resolution. Entering another move clears pending
continuations and buffered presses. Rendering only reads samples.
The special-command and VM inventory preserves known clip/start/wait/spawn/
callback edges with explicit unresolved native dependencies. Specials, weapon
combat and fatalities are not yet implemented. The original oracle and its
stubs/limits are documented in [Scorpion retail moves](docs/scorpion-retail-moves.md).
`battle_step` remains
the shared update and does not branch on game id. Static move pointers can
be copied within this process, but need stable IDs for serialized snapshots.
Input is a frame of held retail switches (four absolute directions, HP, LP,
BLK, HK, LK, RUN). The simulation applies the retail two-sample press rule,
keeps the 8-entry direction history and press latches, and handles presses
after the fighters move, like retail press tasks: special-command records
first, then the current stance's button table. The selected entry starts on
the next tick. See [input and commands](docs/scorpion/input-and-commands.md).
`game/input.c` samples keyboard bindings, maps shell keys, and fills the CPU
frame. The step function never reads SDL. War Gods can be added by appending
roster rows and loading that game's asset bank; it does not need a second
battle type. Options and the reasons for this shape are in
[docs/fighting-game-architecture-options.md](docs/fighting-game-architecture-options.md).

The separate extraction/watch frontend remains `fsys.cpp` in C++17 and links
the same C archive validator. No C++ compiler or runtime is needed to build
the game/viewer targets. The runtime does not copy retail global-address
machinery or pretend our simplified combat values are recovered retail rules.

Validation of the migration:

- `make check`: archive extraction/watch tests, malformed archive consistency,
  assets/rigs/poses, battle rules and lighting/shadow regressions all pass.
- Game, viewer and native tests rebuild with `CXX=false`, C11,
  `-Werror -pedantic-errors`.
- AddressSanitizer/UndefinedBehaviorSanitizer runs pass for the native tests
  and both headless frontends.
- `tests/verify_original.py`: all 36,723 frames match the supplied original
  x86 routines; all 16 matrix fixtures verify. Unicorn was installed into
  `/tmp/mk4-oracle-deps` for this optional check, not added as a runtime dependency.
- All decoded-frame text and the gallery/fight BMPs are byte-identical to the
  pre-migration C++ implementation. The 360-frame fight still simulates 360
  ticks and finishes with health 40/100 in round 1.

## Script-first port policy (current)

Character work starts with `build/retail-disassembly/scripts/` and the native
helpers those scripts call. The goal is equivalent commands, animation phases,
branches and combat balance in our own procedural C engine, not executable or
per-tick emulator identity. Historical alignment statements above describe earlier
checkpoints and are not proof of the current port. Do not add retail execution
recorders or raw JSON traces for character implementation.

The shared simulation now uses Q16 per-tick bodies and fractional health, attack
records, and explicit reaction continuations. Collision uses our fixed standing
and crouching gameplay hulls, independent of rendered animation joints. Script
contacts resolve in stable player order; they can interrupt the next fighter
before its update. Approximate roster moves keep their earlier interval-based
resolver. This mixed roster is transitional, not a claim of retail engine identity.
Reaction sequences continue after a KO so landing/getup branches can settle.
Tests cover our gameplay rules and existing compact animation fixtures without
executing the retail EXE.
