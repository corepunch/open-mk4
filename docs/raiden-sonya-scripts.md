# Raiden and Sonya script port ledger

The supplied `MK4/MK-4.EXE` is the specification: roster slots 1 and 2.
The compiler generates **64/64 Raiden** and **63/63 Sonya** reviewed VM roots,
with **zero legacy roots and zero coverage errors**. Each audit covers eight
input event tables, four CPU rows and nine stance tables, plus the default
weapon and reached contacts/reactions (36 Raiden, 40 Sonya).

`tools/ports/raiden.json` and `sonya.json` contain the reviewed inventory
fingerprints, native entry clip/cursor contracts, engine continuations and C
integration references. `make scripts` regenerates the two includes in
`game/generated/`; `make scripts-check` requires canonical generation and
strict generated-only coverage for every profile. These checks establish
catalogued VM coverage, not exhaustive native reachability or x86 equivalence.

Native-only routines and native parts of mixed routines remain in
`game/c_raiden.c` and `game/c_sonya.c`. VM bodies, including scripts entered by
native tails and independent victims, come from the compiler. No runtime
bytecode interpreter or native-address dispatcher is added.

## Source and integration map

| Family | Raiden evidence | Sonya evidence | Engine integration |
| --- | --- | --- | --- |
| Normals, crouch and combo branches | 4e5ca0[1], 4e5d90[1], 4e6020[1], 4e6798[1], 4e6800[1] | Same selectors at slot 2 | Generated branches; typed native contact/recovery adapters and stance tables |
| Air attacks | 4e6658, 4e6710, 4e61f8[1], 4e6500[1], 4e63d8[1] | Same selectors at slot 2 | Existing body/landing helpers, generated playback and native active visits |
| LP throw | 4de798[1] -> 42cc00 -> 4de6b8 or 4de640; 4de668, 4e9bd0/4e9b40 | 4de798[2] -> 4de690; 4e9b88 | Shared pair setup/cleanup, independent victim fibers |
| LK throw | 4ddfb8[1] -> 42bce0 -> 4de218; 4e99c0 | 4ddfb8[2] -> 42bed0 -> 4de360 | Roster-specific owner/victim programs |
| Air throw | Shared 4e5e28/50, 4e9910 | 46ae60 -> 4e5ed0; 46aed0 release; 4e98a0 | Shared pair ownership; Sonya-specific release/getup |
| Projectile | 49bdd0 -> 4ed5f0, independent 49be10 | 49b9a0 -> 4ed4c0, independent 49ba50 | Shared object pool/contact helper, generated owner and victim programs |
| Forward specials | 47d5f0 -> 4e8440/58; native teleport 47cd00 | Native bicycle 47e650; leg grab 47dee0 -> 4e8578/85a8, victim 4e9a68; square wave 4e85c0; cartwheel 4e81d0 | Native contacts, body motion and cooperative recovery; generated reached VM bodies |
| Weapon | 4ece18, native strike blocks 4ec810/848/838/870 | 4ece38, native 4ec760/778, generated 4ec7a0/7f0 | Existing weapon draw/strike/throw/pickup and stance helpers |
| Finishers I/II | 440fc0 -> 4e0210/220; 4647f0 -> 4e56c8 through 5788 | 43f420 -> 4e01c0/1d0; 465cb0 -> 4e5930 through 5a20 | Generated playback/loops; native scenes, blends, particles and teardown |
| Arena finishers, victory | Shared 4e0118/138/148/158/1a0, 4e6950; 4e6a40[1] | Same arena programs; 4e6a40[2] | Existing scene helpers, generated result poses and reset lifetime |

Bounded `--move`, `--script`, `--native` and `--attack` queries and the existing
`build/retail-disassembly/` bundle supplied the research. Matching assembly in
`reference/mk4decomp` was consulted for unresolved helpers. Upstream sources
remain research material; implementation uses this workspace's engine helpers.

## Important branches and source values

Raiden's HK/LK/sweep/roundhouse clips are 71/70/62/64. Sonya's are
65/308/63/313; crouching HK selects 52/53. Native active visits retain each
roster's contact record and contact-selected recovery rather than inheriting
Scorpion's selected streams.

Raiden's short LP throw binds 76/153, releases at paired sample 25 with
reaction 102 and damage `0x2666`; the extended branch binds 80/154, releases
at 57 with reaction 107, and commits `0x2e14` in 4de668. The native selector's
camera ordering is resolved along the engine fighter axis using the original
pair displacement saved before alignment. LK binds 33/149, commits `0x35c2`
at 41 and releases reaction 111 into victim clip 169.

Sonya's LP binds 78/156, commits `0x2666` at 21 and releases reaction 103.
LK binds 43/163, commits `0x2e14` at 44, releases at 75, then runs independent
victim clip 203 from 75. Her air throw binds 77/161, releases at 24 with damage
`0x2666`, and runs victim clip 198 with the native 53cf70 getup choice (197).
Every pair has interruption cleanup, independent recovery after release, and
KO-safe termination. Alignment preserves the two rigs' ground offsets.

Lightning uses Down,Back+LP (16 ticks), ring Down,Forward+LP (16). Both spawn
independent 48-tick objects at speed `0x3333`; interruption after release cannot
recall the projectile. Lightning uses C[2], damage `0x23d7`, reaction 66; ring
uses C[4], damage `0x1eb8`, reaction 68. Their victim programs are generated and
installed in the shared reaction table.

Raiden's Forward,Forward+LK torpedo keeps fast/ordinary generated startup,
D[0] contact, miss landing, blocked recoil and a released victim continuation.
Its native carry uses the existing pair axis and body controller. Down,Up
teleport is a direction press command: Up checks the prior Down record before
recording itself, replaces the pending jump, sinks, relocates behind the victim,
rises and lands. Its cooldown is actor-owned.

Sonya's Down+Block+LP leg-grab chord uses D[3] once after the eleven-sample
opening and retains hit versus miss/block recovery. The owner starts the pair
at sample 11 while the victim opens at zero: 22/back19/22/back19/34 leaves the
released clip-201 victim at **23**, not 34. The profile checks this inherited
cursor before generated motion/playback. Bicycle uses Forward,Back+HP;
square wave Back,Back,Down+HK retains its asymmetric horizontal block and
negative launch gravity; cartwheel Back,Down,Forward+LK retains its nine native
contact visits and actor-owned cooldown. Cartwheel's block reaction 9 reuses
the shared standing/crouching/armed block paths with native speed `0x3333`.

Raiden's weapon remains type `0x61`, mesh 103; Sonya's type `0x62`, mesh 101.
Descriptors are added to the shared asset table without moving existing model
indices. Strikes use the native packed startup/contact/recovery fields, shared
`WeaponStrike`, delayed horizontal motion, generated back-strike programs and
shared weapon contact tails. Thrown weapons preserve their identity through
foreign pickup. Generated weapon victims are registered in shared dispatch.

## Shared compiler and engine work

Opcode 71 (`45bc70/45bd20 -> 428b10`) now requires an initialized source pose,
blends into destination sample zero with native step `0xa3d` and first weight
`0x4000`, and resumes at sample one. Descriptor bounds, definite initialization
and yielding/progress checks apply before generation; runtime checks require at
least two destination samples. Other native blends reuse the same helper.

Typed shared bindings cover reverse paired playback, motion scheduling,
projectile ownership, weapon contact tails, landing, disarm, impact and bounded
scene task lifetimes. Existing actor fibers and actor-owned clip bindings retain
state and storage across suspension. No new scheduler or retail execution
harness is needed. Pure script-enter thunks become static generated CFG links.

## Portable adaptations and limits

The engine retains its own camera, arena limits, collision/body scheduling and
simultaneous-contact rules. Native camera-relative motion resolves through the
fighter axis. Native human/CPU getup selection uses the portable getup path;
exact retail task-stack order, interpolation and per-tick traces are not goals.

The four character fatalities and shared fan/spike scripts run their generated
playback, waits and loops. Native cinematic adapters use existing bounded scene
models, particles, pieces, blending and teardown. Bespoke retail electricity,
cut geometry, camera composition and decorative task ordering are represented
by these engine effects; visual identity to retail is not claimed. Result-pose
loops retain their ordinary round/reset lifetime, without awarding another win.

## Validation

`tests/test_raiden_sonya.c` exercises real input or CPU move entries in both
slots: roster-selected normals, hit/block damage, missed recovery, LP/LK throw
release, interruption and KO, projectile survival after interruption, teleport,
torpedo, Sonya's special entries and air throw, weapon draw/back attacks,
all eight character/arena scene choices and reset cleanup. Shared scheduler,
contact and scene behavior also reuse the existing fiber/battle/fighter tests.
`tests/test_script_codegen.py` covers opcode-71 source initialization and
sample-one bounds alongside deterministic graph and C11 compilation checks.

Run `make format`, `make scripts-check` and `make check`. The strict audits
must also pass with `--strict --require-generated` for both profiles.
