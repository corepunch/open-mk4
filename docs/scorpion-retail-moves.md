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
