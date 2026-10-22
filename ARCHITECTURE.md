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
| Audio, fighter scripts, weapons, projectiles and arena callbacks | Shared retail VM and native engine services | Original script words and loaded tables select behavior; typed C services implement native boundaries once. Portable presentation and scheduler policies remain engine adaptations. |

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

Game, engine, viewer and native tests use C11. Use procedural functions, plain
structs and explicit state/context ownership. `Battle g_battle` is the single
active MK4 simulation; `battle_init(data,p1,p2)` fully resets it with an
explicit borrowed data image and assembled program. The host owns and loads
those resources; neither simulation nor VM opens a default EXE. Visual asset
binding cannot replace the match program. Simulation functions
operate on Fighter, body, object and scene contexts. They never read SDL.
Assets, pose buffers, archive storage, audio and host resources retain explicit
owners. The standalone `fsys.cpp` development tool remains C++17 and uses the
same C archive validator and filename hash.

## Input boundary

`game/input.h` defines `InputFrame`: a fixed-width mask of absolute game
buttons for one simulation tick. It contains no device identifiers, SDL types
or derived press/release events. The simulation retains the retail two-sample
press rule. CPU decisions remain simulation-owned rather than emulated devices.

`game/input.c` accumulates host samples until a tick consumes them. Short taps
survive between rendered frames; later catch-up ticks receive current held
state. Multiple transitions before a tick collapse to one sample. Opposing
directions cancel. Shell transitions discard pending taps and suppress held
buttons until release. This is an in-process contract, not a serialized network
or replay format.

`game/sdl_input.c` owns keyboard bindings and SDL gamepad lifetime, mapping
D-pad/left-stick directions and buttons into that same contract. Keyboard and
pad holds combine per player. Gamepads take the first free player slot and
support hotplug; stick axes use a quarter-range dead zone. Menu and shell
commands are separate device-independent enums in `game/host_commands.h`.
The menu accepts semantic actions rather than SDL keycodes. The host supplies
menu animation time and seeds menu selection randomness; menu code does not
call SDL. Mapping is fixed for now; configuration and timestamped event replay
are separate features.

## MK4 script and action runtime

The runtime has four concrete boundaries:

| Module | Responsibility |
| --- | --- |
| `game/data/` | Validate and decode MK4 table formats and expose the bound match data. |
| `game/script/` | Execute MK4 instructions and resolve typed native IDs to C services. |
| `game/runtime/` | Own actor execution lifetime and native frame playback. |
| `game/native/` | Implement shared gameplay capabilities and native action entries. |

`game/roster.c` builds engine/UI views from decoded tables. Data readers do not
resolve host callbacks. Native IDs are included only by modules that use them,
rather than exposed through every simulation structure.

All 18 fighters run MK4 wordcode linked from `scripts/mk4/program.asm`
through `game/script/vm.c`. `scripts/mk4/tables.asm` supplies the other image
records, including roster/input/action/CPU/weapon/reaction tables, animation
metadata, rigs, sound selectors and scene data. `make scripts` links them into
one `mk4.data` package without reading the EXE. The generated C header resolves
native callers to the same labels; a layout fingerprint rejects stale packages.
No script address is pinned to its original location. Stable native IDs select
C services and remain distinct from linked data offsets.

Original fixup records identify packed references at import; decoded asset
structures identify raw pointers. Ordinary numeric fields remain unchanged.
Instruction membership is explicit and accidental fallthrough between separate
blocks encounters a non-executable guard. See [source workflow](scripts/README.md).
Selection remaps follow each original table's own native path;
model identity is not a substitute for script or table selection.
`game/roster.c` exposes runtime-derived compatibility views for engine/UI
callers, not handwritten fighter definitions. Unstarted attack states have
no invented animation or balance values.

Original words own script branches, waits, loops and animation commands.
There is no script-to-C compiler, generated script/frame output, per-fighter C
folder or per-fighter script registration or coverage manifest. Research evidence
lives in shared source comments and typed tooling bindings. Script entries are checked image addresses,
not host function pointers. The VM implements all 118 non-null opcode slots
in the supplied handler table; the null slot remains invalid.

Each Fighter owns one `ActionExecution`: VM state, native playback, typed
returns, a generation counter and one cleanup callback.
`game/runtime/execution.c` provides the shared actor step, phase reset and
action cancellation boundary. Phase changes retain cleanup; cancellation
detaches and invokes it once. The unused `MoveSequence` path is removed.
The shared 60 Hz actor pump advances that state on one thread. There are no
coroutine stacks, worker threads or raw retail task-stack emulation. Typed
script returns remain distinct from typed native completions. Starting a new
phase or interrupting one invalidates stale callbacks.
The simulation captures animation history after each tick (at the existing
30 Hz animation sampling cadence), including early phase exits and resets.
`game/draw.h` takes an explicit `const Battle *` and borrowed assets. Drawing
never captures history or advances combat, scripts or presentation effects.
The renderer reads the supplied match on the same host thread; animation
snapshots are interpolation history, not a detached full-scene snapshot for
concurrent rendering. `game/game.h` depends on shared asset/pose types, not
rasterizer or SDL interfaces.

`game/script/entries.c` resolves input/native entry addresses and predicates;
`game/script/bindings.c` resolves typed script-native calls, tails, handlers,
physics callbacks and independent tasks. Each address selects one shared
implementation. Neither lookup uses roster masks or originating-fighter
context. The actor retains its own model, assets, controls and peer identity.

## Native engine services

Retail also contains native routines. Their original logic belongs in shared
C services, not new fighter modules. `game/native/capabilities.h` exposes those
services; the capability modules under `game/native/` group attacks, motion,
projectiles, throws, weapon attacks, cinematics, severing and shared context
operations. Historical function names are navigation labels. A native routine
is implemented once and reused by all retail callers.

Native animation routines may assemble bounded actor-owned playback rows
through `game/runtime/playback.c`, matching their native contracts. Physics, collision,
friction and landing remain physical conditions. These rows are not generated
from script words. Scripts always execute in the VM; no C phase may replace
an authored script wait, branch, loop or animation command. Engine controller
input, match/KO completion and portable presentation remain explicit native
boundaries rather than a recreation of retail global memory.

All non-null entries in the original 194-slot reaction table resolve through
their loaded address. Original reaction streams run in the same VM. A native
entry may own its own prelude or recovery, with typed script/native joins.
Registration and reviewed-root coverage are integration evidence, not proof
of exact native gameplay parity.

## Actor, object and scene ownership

Fighters, projectiles, independent events, scene pieces and cinematic bodies
use bounded pools. Paired moves own explicit peer/capture state; release or
interruption retires it without selecting a fighter-specific implementation.
A detached cinematic body borrows model/assets and owns playback independently
of the two players. It receives no player input or CPU controller.
Round changes and match reset cancel active script/native continuations and
run owned cleanup once. A native callback address is never executed as script
data; a script entry is never called as a host pointer.

## Evidence and validation

Start research with the pinned `reference/mk4decomp` interpreter/handler sources
and matching assembly. Verify local addresses and operands against the supplied
EXE; upstream VAs and old annotation names can differ. Consult licenses before
copying source. Exact scheduler behavior, binary identity and per-tick trace
identity are not goals. Camera, particles, rendering and host policy use our
engine's portable boundaries.

The strict audit covers 1,133 catalogued roots across all 18 roster slots,
loaded dispatch data, opcode support, typed native roles and source reachability.
Uncatalogued programs need the same evidence discipline when encountered;
the audit does not claim exhaustive native semantic verification.

Tests focus on what we implement: interpreter/control flow, malformed data,
native ABI roles, script/native handoffs, loaded-table mutation, interruption,
cleanup, engine lifecycle/body constraints, asset/pose/audio decoding and
rendering. Trust authored retail character logic; do not restore per-fighter
timing, damage or balance matrices. `make check` includes strict audits, shared
runtime/engine/asset checks and the headless game demo. Source gates reject
fighter C/header/include files and generated fighter outputs.

Run `make format` before validation. Commit each coherent verified step directly
to `main`, preserving unrelated work and recording evidence, checks and material
limitations. See the [runtime workflow](docs/character-script-port.md),
[interpreter evidence](docs/research/retail-vm.md),
[decoder guide](docs/script-decoder.md) and
[migration ledger](docs/research/shared-retail-runtime.md).
