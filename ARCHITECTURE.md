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

Use plain structs for state, enums for states, const data tables, and procedural
functions. The game has one active `Battle g_battle`, defined in `game/battle.c`
and declared in `game/game.h`. Game functions and callbacks access it directly;
they receive the fighter, object or other local arguments they operate on.
`battle_init(p1, p2)` fully initializes that singleton. Tests initialize it for
each scenario instead of creating separate active battle instances. The API is
single-simulation and is not reentrant. Assets and host resources keep explicit
owners and arguments. No C++ classes, STL, references or
exceptions in migrated game/engine modules. Keep simulation independent of SDL
and rendering, and keep archive ownership separate from decoded assets.

Moves run as actor-owned frame programs (see the frame-table migration below).
The existing P1/P2 actor pump steps each fighter's program once per battle tick
on one thread. There are no coroutine stacks, worker threads, wall-clock sleeps
or retail task-stack emulation, so all move state lives in `g_battle`. A wait is
a held row whose end function is its continuation. A newly started peer program
runs its setup immediately and its first row on its next actor tick. Callbacks
remain supported for independent objects, physics and shared engine behaviors.

Do not reproduce the original global-address memory model or introduce an ECS
framework merely to imitate the retail callback trees. Add an explicit event
queue or script interpreter when a verified gameplay behavior needs it. Keep
new combat values labeled as our own until measured against retail.

Migration must preserve existing archive rejection behavior, decoded animation
words, original matrix fixtures, battle regressions, and deterministic rendering.
Each verified migration step is committed separately on `main`.

## Current alignment

The game, shared archive/assets/pose/renderer modules, SDL host, viewer and
native regression tests now compile as C11. `Battle` lives in `g_battle`;
`Assets`, `Canvas`, and `SdlApp` retain explicit context APIs. Decoded assets and SDL resources have
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
Scorpion executes its 70 reviewed VM roots directly from the retail image
through `game/retail_vm.c`. Other fighters retain generated C11 bodies until
migrated. Typed bindings connect retail native edges to shared engine helpers
and character-owned native continuations; image addresses are validated data
indices and never host code pointers. Fighter clip bindings own
their storage; animation changes preserve action, contact and controller state.
Native contact/recovery runners, pair coordination, objects and scene blends
remain procedural C. The coverage audit distinguishes interpreted and generated roots, rejects legacy VM bodies and
checks canonical regeneration and typed input/CPU/stance/weapon/reaction coverage.

The old x86 normal-move fixtures remain research evidence. Regression scenarios
reuse their branch inputs and check control-flow choices, damage, interruption,
clip bounds and bounded recovery; portable frame visits do not claim per-tick
retail task-scheduler identity. Shared frame-runner tests cover holds, jumps,
loops, cleanup and both player orders. Source playback and architecture details are documented
in [the porting guide](docs/character-script-port.md) and
[compiler guide](docs/script-compiler.md).
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
records, and explicit reaction continuations. When decoded assets are bound, collision projects animated joint origins onto
the contact axis and trims the horizontal interval as in native 48d750. Shared
pose selection supplies real clip lengths and model heading to both simulation
and drawing. Rule-only tests may omit assets and use the explicit fallback hull. Script
contacts resolve in stable player order; they can interrupt the next fighter
before its update. Approximate roster moves keep their earlier interval-based
resolver. This mixed roster is transitional, not a claim of retail engine identity.
Reaction sequences continue after a KO so landing/getup branches can settle.
Tests cover our gameplay rules and existing compact animation fixtures without
executing the retail EXE.

Liu Kang's current script coverage is recorded in
[the character ledger](docs/liu-kang-scripts.md), including the LK-release
charge path and character-specific CPU range rows. Near and medium CPU
special selections have separate entries where the native roster rows differ.
Character finishers can provide their own approach/minimum distances; zero
distance metadata retains the previous shared defaults. Cinematic objects
step on the simulation clock during fatalities, and scene cameras may widen
their focal scale without changing ordinary fight framing.

## Direct retail script execution

The current direction is to execute original retail script words, with shared
opcode implementations and typed native bindings instead of generated C/frame
bodies. Script control flow stays in the EXE data. Native engine behavior still
uses C11; per-fighter move folders are transitional native organization.

Scorpion low kick, crouching high kick, spear startup/return, and spear
capture/stun victims now use this interpreter exclusively. Actor-owned state
stores the instruction cursor, waits, counted loop, contact result and animation
continuation. Images are immutable asset data; checked VAs never become host
code pointers. The host supplies its selected image through Assets. Rule-only
clients lazily share the default EXE image, so there is no generated fallback
for migrated roots. Native spear flight/pull/cooldown remain existing engine
adapters, with VM-owned cleanup and native suspension.

Other roots retain generated programs until their migration. The manifest and
strict audit distinguish both implementations and reject duplicate bodies.
Interruption, move replacement and reset cancel VM state. No coroutine stacks
or retail global arena are used. See [runtime evidence and scope](docs/research/retail-vm.md).

## Deterministic script compilation

`tools/compile_retail.py` lowers the decoded, roster-specialized VM graph to
C11 frame programs in each fighter's `generated/` folder. Branches select named
phases, waits are fixed rows, and contacts use the shared battle API.
Native boundaries have explicit engine symbols, roles, arguments and timing in
`tools/retail_bindings.py`; unsupported instructions and bindings fail before
output. This is offline compilation, without runtime bytecode/address dispatch.

`game/script.c` owns the small shared clip primitives. A clip-only binding
preserves combat state, action, flags, physics and the running frame program.
Its MoveDef storage is the actor's `playback_move`, so cancelling a program cannot
invalidate `current_move`. Full move transitions still use the normal engine entry API.

Reviewed character profiles keep generated roots separate from existing legacy
C adaptations. The strict audit checks typed input/CPU/stance/weapon/reaction
coverage, source definitions, graph fingerprints, integration and regeneration;
it does not prove native equivalence or exhaustive x86 reachability. All profiles
must pass `--require-generated`; Make has no legacy exception. Raiden's 64 and
Sonya's 63 catalogued roots join the completed Scorpion, Sub-Zero and Liu Kang
integrations. See [their port ledger](docs/raiden-sonya-scripts.md). Native-entry
contracts seed clip/cursor validation and are checked at runtime. Pure native
script-enter thunks link directly into the generated CFG; proven cursor progress
allows loops without adding a dispatcher. Committed paired victims own their
pose independently before separation, while native scene completion cancels
parked frame programs.
See [the compiler contract](docs/script-compiler.md).

## Frame-table migration

The frame runner uses Quake II's monster callback shape: `mframe_t` contains
only optional `aifunc`, movement distance and optional `thinkfunc`. Each row
executes on one fixed 60 Hz simulation step. Frame data has no duration,
entry/tick/leave split or completion predicate. A frame callback may hold any
row through `mmove_hold`; every held visit repeats the action callback and
calls movement with zero distance. `fighter_mmove_nextframe` requests a bounded
jump, including frame zero, before the next callback. An unchanged move loops;
`fighter_stop_mmove` explicitly ends playback while retaining its pose.

Move bounds identify decoded poses directly for ordinary forward ranges. An
optional move-owned sample map handles reverse playback, skipped poses and
multiple gameplay frames displaying the same decoded pose. It contains only
pose indices. Actions remain attached to their execution frames. Generated
script callbacks preserve decoded boundary effects on the existing simulation
clock. The 30 Hz generic clip tables map two fixed frames to each pose.

Action entry and playback-only phase transitions remain separate. A phase
retains action, contact, input and pair state, and starts on the next actor
tick. Callbacks that replace or interrupt a phase stop its old update. This
actor scheduling boundary is intentional; the runner does not inherit Quake
II's 10 Hz clock or process a newly installed phase in the same update.
Action cleanup is retained across phases and detached before invocation on
completion, replacement, interruption or round/match reset. Camera and object
updates remain independent. Generated script coverage and behavioral checks
remain required for each move.

Reference reviewed: the local Quake-2-master `game/g_local.h`, `g_monster.c`
(`M_MoveFrame`) and `m_infantry.c` (attack and held firing callbacks). Its source
is GPL-licensed; this runner is an independent implementation of the callback
contract within our existing actor/simulation boundaries.

Every fighter's script roots, native move continuations, paired victims and
fatality actors use frame programs; the compiler has no other backend.
Compiler-generated frame tables come from the decoded VM graphs. Native phase
callbacks provide contact decisions and lifetime coordination. Source-defined
waits, counted loops and sample paths are expanded offline; no runtime bytecode
or hidden continuation stack is introduced. Unsupported frame operations fail
generation, and bindings need an explicit nonblocking review or a dedicated
frame lowering. Runtime predicates select named successor phases, and repeating
scene streams form finite phase cycles. Shared weapon strikes, throw/pickup,
sidestep and the foreign sword paths also use phases.

Native phases whose frame ranges depend on a selected attack profile assemble
bounded actor-owned `mframe_t` rows once at phase entry (`game/native.c`). Each
row names its action callback; callback bodies do not test elapsed ticks or
count down an action timer. Source-pose boundary callbacks select the pose used
by contact before the action, then the next row displays it. Native tails
install their rows directly; the action's cleanup carries across phases.
Independent movement uses bounded fixed-frame callback rows on the existing body
physics clock; collision, friction and landing remain physical conditions.

## Shared retail runtime and native boundaries

All fighters use the shared interpreter and retail-loaded selection tables.
Original script words own script control flow and animation commands. C fighter
definitions, private character headers and generated script/frame outputs are
not runtime architecture. `game/native_services.h` exposes the remaining native
engine implementations; local callbacks and records stay static. Native services
operate on explicit Fighter/body/scene contexts and are implemented once per
retail routine rather than copied by roster.

Native implementations live in `game/native/` capability modules; no C source
or header belongs in fighter directories. Per-fighter `port.json` files and
historical source aliases are research/navigation data only. Strict audits check the reviewed inventory,
source reachability and runtime/native boundaries; registration does not prove
native gameplay parity.


### Independent cinematic bodies

A detached body may keep animating while both players retain their own scene
phases. `Battle.scene_actors` is a bounded pool for those render-only bodies.
Each slot borrows a roster model and assets, owns its pose/clip/frame state,
and runs the existing fixed-frame player. It receives no input table, CPU,
combat entry. Only the clip and frame APIs apply. Rendering reads its
state without advancing it. Reset and round changes cancel its frame program
and run cleanup once, just as explicit scene-body retirement does.
