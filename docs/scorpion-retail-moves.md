# Scorpion: retail animation and move evidence

Addresses in this note refer to the supplied June-1998 EXE, SHA-256
`4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d`.
The reference checkout is useful disassembly/reconstructed C, not recovered
original source. Its addresses, scratch globals and some native-port comments
differ from this executable. In particular its VM dispatch table is elsewhere.

## Where lengths and starts live

There are two different quantities:

1. A stored clip has a descriptor at `0x4fad7c + id * 12`: data pointer,
   **frame count at +4**, and flags/word stride at +8. AP0 identifies the clip,
   compression and byte size; AP1 holds the pose payload. A clip has samples
   numbered `0 .. count-1`. These records do not define a move's duration.
2. A move opens clips, chooses starting samples, advances forwards/backwards,
   waits, changes speeds, calls collision routines and branches. These controls
   live in executable bytecode **and native code**, including parameter blocks.
   An attack can also change the opponent's animation or spawn another entity.
   Input, contact, interruptions and the scheduler affect the executed path.
   There is no universal `<start, length>` record for every move.

`python3 tools/retail_moves.py --output build/retail-moves.json` extracts all
612 clip descriptors, their 36,723 samples' ranges, channel counts, compression,
payload sizes and archive offsets. It also follows the two high-kick scripts
through their native recovery edges. It rejects unsupported EXE hashes and
unresearched script operations instead of guessing. It does not yet inventory
all Scorpion commands, specials, weapons, fatalities or interrupted paths.

## The complete uninterrupted, no-contact high kick

Entry `0x46a680` writes action `0x100` at controller +0x74 and starts script
`0x4e5ca0`. Opcode `0x3c` selects an entry at `0x4e5ca4` by character ID.
Startup pointer fixups must be applied: these entries are zero in the raw PE,
and become **word indices**, not byte pointers. Kitana maps to Sonya's entry;
Noob maps to Reiko's.

| | Scorpion (0) | Sub-Zero (4) |
| --- | --- | --- |
| Selected script | `0x4e6c90` | `0x4e5c80` |
| Stored clip | 66, descriptor `0x4fb094` | 71, descriptor `0x4fb0d0` |
| Stored samples | 18 (0..17) | 28 (0..27) |
| Open instruction | `02 000003`, descriptor operand | `47 00020a`, descriptor operand |
| Initial progression | 0,1,2, then two pose evaluations per visit | 0..10 forwards |
| Native tail | `0x46a760` → `0x470c60` | `0x46a790` → `0x470c60` |
| Native parameter block | `0x4e5d30` | `0x4e6978` |
| Contact queries | selector 1, steps 11..13 | selector 2, steps 11..13 |
| Recovery script | `0x4e5c70` via `0x46a770` | `0x4e6998` via `0x46a7a0` |
| Recovery direction | **backwards to frame 0** | forwards to frame 27 |
| Return to controller `0x46f650` | step 30 | step 29 |

Step zero is the entry dispatch. Subsequent steps each dispatch the installed
controller callback once with the pause flag cleared. These are **playback
dispatch counts**, not an independent measurement of retail wall-clock FPS.
The JSON fixture records every script fetch, pose submission and contact query.
Some visits submit two poses: the last submitted pose is the displayed sample,
not the advanced cursor left in group +0x28 for the next visit.

The old renderer stretched each entire clip over the same invented
10+5+18 ticks using a 40%/20%/40% curve. It consequently held Scorpion's
extended leg and returned straight to idle without his backwards recovery.
Sub-Zero's forward clip already contains a recovery, making the error less
obvious. Neither generic curve was the retail timing.

## Independent execution check

`PYTHONPATH=/tmp/mk4-oracle-deps python3 tests/verify_moves.py` runs the supplied
x86 VM and native playback/recovery routines in Unicorn and compares the result
with `tests/fixtures/original_kicks.json`. Unicorn is optional and development
only. Hosts that prohibit executable JIT memory need to authorize this local
oracle run. `--record` deliberately regenerates the JSON and matching C fixture
for reviewed changes. Normal regression tests use the saved C fixture.

Only pose submission (`0x407920`), audio (`0x489f00`) and contact queries
(`0x48d3f0`) are replaced. Contact explicitly clears result bit 0 (no hit).
The VM, frame counters, tagged continuation stack, native parameter runner,
direction changes and return to the controller execute original x86 code.
The harness checks both CPU and controller stacks. This verifies uninterrupted
no-contact playback; it does **not** verify retail damage, hitboxes, hitstop,
block behavior, input cancels, wall-clock speed or full-match behavior.

## Character C files and callbacks

The requested Quake II pattern fits: immutable moves, per-frame events,
completion callbacks and a current-move pointer on the live fighter.
Quake II's actual `mmove_t` stores first/last frames, an `mframe_t` array and
an end function; its monster callbacks choose `currentmove`.
See id Software's [definitions](https://github.com/id-Software/Quake-2/blob/master/game/g_local.h),
[runner](https://github.com/id-Software/Quake-2/blob/master/game/g_monster.c) and
[soldier callbacks](https://github.com/id-Software/Quake-2/blob/master/game/m_soldier.c).
These are architectural references; no Quake implementation is copied here.

The game now implements this boundary. Each character has its own C file;
Scorpion's `scorpion_jab` and `scorpion_kick` select named immutable moves.
`Fighter.current_move` can point at another move without adding a roster
branch to `battle_step`. `MoveSequence` stores sparse playback segments,
timed events and an end callback. A segment specifies its start tick, first
sample, signed sample step and ticks per step; it runs until the next segment.
Scorpion's entire kick needs five segments rather than thirty sample rows.
Events use move-relative simulation frames, separately from clip samples, so
an event can occur during a hold or reverse passage without ambiguity.
`game/moves.c` dispatches those callbacks
in simulation; rendering calls the read-only `fighter_frame` accessor.
The two kicks use the original displayed sample sequences and return steps,
with contact eligibility at steps 11..13. Damage, reach, push and stun remain
the prototype's values. Other move scripts are still approximated.

For MK4 a move must additionally identify the clip, and support repeated,
skipped, reversed and held samples. A linear first/count alone would recreate
this bug. Animation progression belongs in the simulation; rendering reads it.
Hit windows and animation samples are separate quantities. Specials belong in
the same character module as normals, but a spear needs projectile/contact/
victim logic and a teleport needs position/state logic as well as pose samples.
Merely listing those clips would not implement the specials.

The remaining Scorpion recovery work must identify the command entry, action,
all scripts and native continuations, every clip/start/advance/wait, sounds,
motion, contact windows and selectors, branches, opponent effects and exit
conditions. Verify each reachable scenario against retail before labeling it
recovered. Preserve unresolved edges explicitly; a plausible animation is not
evidence of recovered gameplay.

## Command and native-entry inventory

`python3 tools/scorpion_moves.py --output build/scorpion-moves.json` extracts
Scorpion's seven per-button command lists for **both players**, including the
three raw words per record, direction tokens, time window, flags, predicate,
dispatch table and native target. This is an inventory with explicit open
edges; it is not a list of fully ported moves. The move-list texture in
`geogfx/sc_m_geo.geo` independently confirms these inputs:

| Move | Input | Window | Native entry |
| --- | --- | --- | --- |
| Spear | B,B,LP | 27 | `0x49a630` |
| Teleport punch (also airborne) | D,B,HP | 24 | `0x47fc90` |
| Breathe fire | D,F,LP | 20 | `0x468610` |
| Draw weapon | F,F,HK | 24 | `0x46d1d0` |
| Fatality I | B,F,F,B+BLK | 50 | `0x467ce0` |
| Fatality II | B,F,D,U+HP | 50 | `0x467cd0` |
| Spike fatality | B,F,F+LK | 40 | `0x467e70` |
| Fan fatality | F,D,D+LK | 40 | `0x467e00` |

The texture also lists sword spin as D+LP and air throw as BLK in air. These
are contextual actions; they do not appear as separate direction sequences
in these lists. The command-window integers above are extracted values, not
seconds. Native predicates still control state, distance and finisher access.

Direction tokens are low-byte first: `AA` back, `DD` forward, `02` down,
`01` up. Dispatch bits in the metadata select **different** native tables:
`01` -> `0x4e6ef8`, `02` -> `0x4ed828`, `04` -> `0x4e8910`,
`08` -> `0x4e7008`, `20` -> `0x4dfc90`. Other bits have gating roles;
using the whole flag byte as one table ID misidentifies teleport and finishers.
The high metadata byte indexes predicate pointers at `0x4e4ec0`.

The extractor also preserves kick contact records selected through `0x53d2b8`.
Their six words hold four signed Q16 rectangle values, packed reaction IDs and
Q16 damage. The final word `0x2147` is a fraction of full health, **not push**:
native `0x48d64c` passes record +0x14 to `0x48de80`, which subtracts health.
These records alone do not specify transformed collision, block behavior,
hitstop or victim movement. Those routines and the shared weapon/projectile/
finisher scene machinery must be ported before claiming retail gameplay.

## Normal-move branch oracle

`tests/verify_scorpion.py` records 27 original-x86 scenarios: jab, sweep,
uppercut and kick with miss/block/hit/air-hit results; early/late repeated jab
presses, repeated jab loops, wrong facing, and late uppercut contacts. It
executes the native input-repeat helper rather than replacing its decision.
The only injected input is the original player's LP press latch. Native
contact routines are explicitly replaced with the requested result; this
checks the attacker's continuation, not retail collision or victim behavior.
There is no world/physics pump, so this harness must not be used to assert
projectile, teleport or fatality completeness.

Uninterrupted no-contact playback now uses sparse segments in `c_scorpion.c`
for jab (16 dispatches), sweep (41), uppercut (37) and high kick (30). C tests
compare every submitted visible sample and end step with the x86 fixtures.
Contact eligibility uses the original query visits: jab 8, sweep 18..19,
uppercut 7..9, kick 11..13. Combat damage and geometry remain approximations.
The C runtime also matches all 27 recorded branches, including uppercut block
at 31 and hit at 61; last-visit contact adds two dispatches. Buffered LP input
(currently the host's punch button) is consumed at the native jab decisions
and can open clip 295. Repeat presses during recovery do not reopen the jab;
facing away rejects the follow-up. The jab alternates reusable move phases,
and uppercut reuses a five-dispatch recovery loop with a counted end callback.
Neither needs an authored row per frame.

`Fighter.contact` carries miss/block/hit/air-hit and the contact tick. Sparse
contact events can request queries outside a continuous active interval. A
one-shot `after_contact` continuation handles the jab boundary where retail
queries contact before selecting the next phase. Shared hit resolution keeps
simultaneous trades; entering a reaction clears a pending continuation. Tests
exercise actual simulator hit/block delivery as well as controlled oracle
results. These tests establish playback/branch behavior under the stated
inputs, not the retail damage, collision, victim reaction or hitstop rules.

## Special-program edges and scheduler verification

The inventory now decodes both spear entry alternatives, spear's return
script, fire's parent script, teleport entry/landing, and weapon draw/return.
Each instruction preserves its VA, raw word, native handler, operand words,
clip descriptor/count/start where applicable, and unresolved native edges.
Unsupported opcodes stop a path explicitly. A native tail is an open edge,
not proof that the move ends. `whole_move_verified` remains false for these
programs.

Spear (`0x4ed448`) opens clip 14 at sample 0, or at sample 9 through its
conditional alternative (`0x4ed47c`). Both spawn task type `0x43` with callback
`0x49a6a0`, progress to sample 30, then request a **45-tick scheduler wait**.
The native child controls the chain and victim; `0x4ed4a8` is another return
program reached through native code. Clip 14 contains 80 samples; neither its
stored count nor the parent's forward progression supplies the full spear
move behavior.

Teleport's `0x38` instruction installs `0x480040` in group +0x44. It does
**not** call that routine inside the VM. The world/physics update must drive
this callback; its completion determines how the native continuation advances.
The initial motion operands are preserved without converting them into an
invented speed or teleport distance. Fire and weapon draw also leave the VM
through native routines with effect/weapon dependencies.

The Scorpion oracle now executes the **original task pump `0x41f570`** after
entry. It restores task-local fields and honors the signed 16-bit countdown
at task +0xdc. The initial save mirrors `0x41f650..0x41f6cd`. All 27 normal
fixtures, including every fetch and pose call, remain unchanged. A separate
synthetic test stream opens clip 66 for one tick and requests a 45-tick wait;
the original VM resumes at tick 46 with the pose held throughout. This guards
against silently collapsing long waits to a single callback dispatch. The
synthetic stream is a harness test, not an extracted Scorpion move.

Researching spear also exposed another fixture requirement: a parent task
must occupy a real task-pool slot, since unlink scans that pool for the
predecessor. The oracle uses `0x538728`. A full spear oracle additionally needs
initialized group/transform pools and loaded geometry resources. Merely
initializing the pools reaches a resource-bind failure at `0x408600`; tracing
that failure path does not validate a successful spear. No spear timing or
projectile gameplay is claimed from that incomplete environment.

Still open: the other normal/contextual attacks and combo routes, specials'
full entity/physics/victim behavior, weapon combat/geometry, all finisher scene
programs, actual collision/reactions/damage/hitstop and authentic four-button
command dispatch. The current host still supplies simplified punch/kick
commands. Command extraction is not their implementation.
