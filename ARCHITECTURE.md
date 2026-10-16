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
