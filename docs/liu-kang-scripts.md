# Liu Kang script port ledger

The supplied MK-4.EXE is the specification. Selector slot 3 is Liu Kang.
This ledger distinguishes implemented control flow from outstanding work.
Shared locomotion, contact scaling, reactions, landing and scene pools use the
existing procedural engine; this is not retail scheduler emulation.

All 45 remaining VM roots have been migrated to generated C. Together with the
previous five and five newly catalogued throw/fan continuations (`4de468`,
`4e0138/148/158/1a0`), the audit passes **57/57 generated, zero legacy**, including the two shared air-weapon selector programs.
`make scripts` regenerates `game/liu_kang/generated/scripts.inc` from the pinned EXE;
`game/liu_kang/port.json` supplies reviewed native entry contexts and returns.
Normal command/CPU entries call these generated fibers, and script-only native
thunks become direct C jumps. Shared fibers, contacts, clip binding, pair
playback, landing, weapon motion, audio and scene pools supply the engine work.
Native contact/recovery portions and native-only programs remain procedural C.

`make scripts-check` now requires generated-only coverage for every profile,
including Liu Kang. It checks eight input event tables, four CPU rows, nine
stance tables and 41 contact/VM-dispatched reactions, plus source references,
static script links, the inventory fingerprint and canonical regeneration.
See [the compiler contract](script-compiler.md) for entry-context and native
binding rules. This establishes declared VM coverage, not exhaustive x86
reachability or native implementation equivalence.

| Family | Retail evidence | State |
| --- | --- | --- |
| HP/LP, uppercut, close attacks and combo buttons | 46d290, 4e62f0/330/360/3b0, 4e62a0, 4e61e0, 4e69b0, 4e6190 | Implemented using the reached shared branches |
| HK | 4e6c90 -> 46a760 -> 4e5d30 -> 4e5c70; clip 66, B[1], counted startup 3 + ceil(17/2), then three native contact visits, reverse recovery | Implemented |
| LK/crouching attacks | 4e6d08, 4e6020[3] -> 4e60b8, 46b7f0, 4e6158 | Implemented; LK avoidance ticks 7–9, B[6] at 10, contact hold 8 / miss 5 |
| Sweep | 4e67d8 -> 46efd0/46f130; clip 62, B[36], double steps to 10 then to 22 | Implemented; contact visits 17/18, reaction 19 |
| Roundhouse | 4e6e90 -> 46f390, 4e6890/4e68a8 -> 46f3e0/46f500; clip 64, B[34] | Implemented; fast startup skips four ticks; hit adds 28 recovery ticks |
| Air attacks | 4e6658, 4e6710, 4e6df0/46c970, 4e6544, 4e641c | Implemented; straight kick uses clip 61 from cursor 4, B[39], three active visits |
| LP and LK throws, independent victim tails | 4de678/4e9b68/485b10; 42c040/4de3b0/410/428/468/478, 4e9a90/ab0 | Implemented; see timing below |
| High/low/air fireball | 4e4928/4e4948 -> 49bf90/49bf50/49bfe0 | Implemented; independent task 64, C[0]/C[1] |
| Bicycle kick | LK release 4e49a8 -> 45eab0/47eaf0, 4e8610/678/688 | Implemented; hold LK >=120 ticks then release |
| Flying kick | 4e4978 -> 47edc0, 4e86a8/d0/e8/718 | Implemented; hit/block/miss and landing |
| Weapon draw/strikes/throw/pickup; sidestep | 4e4998, 46d1d0; 4ece58/4ecd50, 4963e0/4d0/6670/6920; RUN records | Implemented |
| Fatalities I/II and fan/spikes | 4e4968/4e4934/4e4954/4e4984 | Implemented; cinematic presentation adaptations below |
| CPU | 438e50/60/70/80 -> 4df8e8/928/968/9a8[3]; 4dfe10/50[3] | Implemented; native entries and character finisher spacing |

Normal attacks preserve their command routing, contact/miss/block recovery,
repeat/cancel windows and high-avoidance clearing on interruption. Shared
native tails use the existing portable body and reaction controllers. Exact
pose interpolation and camera/task ordering remain engine adaptations.

LP plays owner/victim 79/151 to sample 45, releases reaction 104 into clip
174, applies 0x2666 damage, and finishes owner sample 60. Victim impact is at
174 sample 17; after sample 26 it flips and gets up with clip 191 (4ea220).
The portable getup selects the normal row rather than reproducing native
human/CPU task dispatch.

LK plays 35/133 for 30 paired samples, then eight owner samples while the
victim holds 30. Damage 0x2e14 commits at 38; two more paired ticks release
reaction 80. Victim advances to 35, cycles 31–35 three times, then reverses
to 0 with -0x3333 speed and friction. Owner cycles 39–43, reverses to 0,
then retreats with clip 182 at -0x11eb, double steps to 28 and finishes 64.
Pair alignment preserves each rig's bind-ground Y. Both actors release
input/paired state on interruption; committed victim recovery survives KO.

Fireballs use F,F+HP (ground/air) or F,F+LP (ground), inclusive 24-tick
window. Ground startup double steps to sample 20, queues A[33]/indexed[2],
then spawns an independent 48-tick object at speed 0x3333. Low loops 18–23
three times after reaching 23; high reaches 27 then rewinds (its forward
18/27 requests have already passed). Air plays six samples, spawns with
indexed[3], holds 16, reverses and falls with gravity 0x20c. Spawn offsets
come from 4ed730/740/760; rig 50678c is shared mesh 50. Native trail objects
are bounded presentation in the portable renderer; their allocation/task
ordering is not reproduced. High fireballs pass a crouching action 2001; low
ones query it. Both use one scaled 10% contact. Reactions 64/65 now translate
clips 130/127, friction and the low-hit forward recovery.
Blocked contact uses reaction 2 (`486ca0 -> 486d30/486e20`): retreat
0x2147 with friction, two seven-pose standing cycles (`4e9e68`),
crouching clip 255 out and back (`486f90`), or armed clips 247/222.
Armed standing plays all five clip-247 samples before rewinding (`4e9ec8`).

Flying kick uses F,F+HK (24): clip 92, speed 0x23d7, Y -0x170a, gravity
0x106; contact D[12] after six opening visits. A hit plays clip 93, holds 20,
then reverses 92 and lands at gravity 0x51e; a block holds 12+12 and lands
at 0x28f. A miss reverses after landing. Reaction 31 uses clip 131, speed
-0x1999, Y -0x23d7 and gravity 0x24d. Reached normal records 15/19 now
use tumble/getup and the clip 192/down-held 193/standing 194 branches.

Weapon draw is B,F+LK (24). Descriptor 4ece58/type 66 selects rig 506cb0
(mesh 96) and its own armed HP/LP entries. HP (4ec680) double steps clip 233
to 18, then uses clip 216 at samples 0/10 with weapon[15]. Each buffered HP
allows the next strike; no press recovers through five samples and a stance
blend, or finishes the second half. Opcode 6c/45b050 installs delayed motion
6/6/0x11eb; its single following word is now decoded. LP uses 4ec708 clip
240/weapon[13]; contact plus buffered HP selects 4ec720/730 clip 237 and
weapon[16]. Back HP uses clip 242/weapon[17] and a ten-tick armed blend;
back LP uses clip 227/weapon[11], double steps then two query visits. The
corresponding victim reactions 57/59/60/61/62 now use shared reaction
controllers. Thrown/picked-up weapons retain type 66 and its callbacks.

RUN priority is shared sidestep before pickup. Sidestep keeps the eight-tick
double-press window, 80-tick cooldown, directional sample order and reduced
travel against an airborne target. Armed air attacks use the existing weapon
HP/LP and shared landing. Asset loading adds the original type-66 rig.

Fatality I requires F,F,F,D+BLOCK with HK+LK held, without Up/HP/LP/RUN,
within 60 ticks and distance 1.8–3 (45e410, mask 4e4158). Fatality II
requires F,D,D,U+HP, 50 ticks, distance <=1. Fan is F,F,B+LP, spikes
F,F,B+HK, both 40 ticks and close range, stages 8/0 respectively. They
require the real deciding-KO winner, defeat action 1009 and finish clock.

Dragon scene 450930 aligns at 0x20ccc, waits 16+85, creates the original
lk_fat_g.geo rig 508204 at Y -0x12c49, waits 20, plays clip 418 0–159 then
160–518 and loops 446–518. Victim 451190 waits 269, blends 389/419 and
plays all 289 samples. Blood/cut task 451300 starts after actor sample 159:
wait 4, 23 four-visit cycles, 26 six-visit cycles, wait 8, remove joints 5/6.
The portable scene renders the dragon in the owner's presentation slot;
it does not rebuild a native object/task hierarchy. Native camera zoom,
per-joint blood random allocation and minor sound tasks are adapted to the
portable wide scene framing and bounded particle/piece pools.

Second scene 43c3d0 runs event selectors 42/43/44 with waits 15/20/60;
4e0020 pairs clips 537/539 to 37, parks victim reaction 176, finishes owner
59, waits 18, queues A[33]/indexed[57], plays 538 and spawns task 38 before
reversing. The finisher projectile uses 4e0078/mesh 50 and dispatches 177
within 2.5 scene units. Our pair-axis distance replaces fixed scene Z.
Generic flesh pieces replace the original per-piece task programs; the
160-tick explosion tail still completes before the result. Object-pool
failure resolves the scene without stranding either actor. Fan/spikes reuse
the already translated shared stage scene, with Liu Kang victory clip 119.

The release-table audit adds bicycle kick (53ca38/53ca80 slot 3, record
4e49a8), absent from the old press-only query. 45eab0 requires at least
120 ticks since LK press and rejects opponent action 100b. The shared input
layer now dispatches release-tagged commands without updating press latches
and suppresses word-B releases when LP/LK/RUN has a new press that tick.

47eaf0/4e8610 plays six clip-68 samples while momentum is saved/stopped,
then restores forward 0x2147, Y -0xa3d and gravity 0xc4. Native 4704d0
queries D[6] for up to 60 visits while airborne; animation cycles 7–22. A
block launches recoil Y -0x147a/gravity 0xe5 before normal landing. A hit
deals 0x2666 once, disarms and holds reaction 84/clip 136 independently.
Owner clamps absolute Y to <=-0x15999, halves forward speed and aligns
victim X/Z at 0xe666 for 58 ticks. Release reaction 81/action 100b uses
clip 182 with -0x4ccc/friction on ground, or 183 with -0xccc, Y -0xf5c,
gravity 0x312 and getup 185 when above 0x3333. Owner snaps to ground,
finishes 68 from sample 6 and returns. Interruption releases the committed
victim tail; no repeated damage is added for the visual kicks.

Final normal-path audit: crouching HK 4e60b8 finishes clip 54, holds
11/10/11/12 for two two-tick cycles, then rewinds the complete 13-sample
clip before returning. Its 42-tick recovery includes that reverse tail;
returning immediately after the pose loop would allow an early next move.

CPU slot-3 rows are 0x40004, 0x20002, 0x40003, 0x20002: near bicycle,
medium low fireball, flying kick and low projectile. The shared near choice
now has a distinct entry, falling back to the old special entry for existing
fighters. The CPU bypasses human charge/chord gates and invokes translated
move entries directly. Its fatality approach distances are 2/1 units from
4dfe10/4dfe50[3], with first-fatality minimum 1.8; it retreats for the dragon
and approaches for the close fireball scene. Straight/diagonal air attacks
select the corresponding clip, including armed HP. Shared plans and random
probability gates remain the engine's established CPU adaptation.

Coverage: all slot-3 standing/crouching/close/combo/air attacks, both ground
throws, high/low/air fireballs, flying kick, charged bicycle kick, weapon
draw/strikes/repeats/throw/pickup, sidestep, fatalities and stage finishers
are wired and exercised. Shared locomotion, stance, contact, reaction, KO
and reset controllers retain the engine boundaries described in the guide.
Liu Kang has no retail BLOCK air-throw record; no extension was added.
Cinematic camera/decorative details listed above remain presentation
adaptations, not claims of native task or rendering identity.

## Generated migration evidence and ownership

The compiler now handles inherited clip/sample contracts, paired descriptors and
counts, sample-start playback, landing waits, optional full playback, and
cursor-progress proofs. Native tails `46dd00/10/20` are pure script-enter thunks
for `4e6330/360/3b0`; they are statically linked rather than recursively called.
LK throw `4de428` returns to its native damage coordinator, then generated
`4de468/478` performs retreat. Committed victims own their pose before body
separation (`FF_PAIR_RELEASE`); interruption cannot rewind or cancel that work.
Clip rebinding no longer invalidates throw/air/bicycle identity checks.

The fan owner runs generated `4e0138` from the existing bounded native scene
counter, then generated release/victory scripts. Its victim runs generated
`4e0188` until the native arena continuation replaces it with `4e01a0`.
Fireball-fatality `4e0020` parks as authored; the independent explosion's
160-tick native completion retires the parked fibers on the simulation stack.
Object-pool exhaustion uses the same completion path. These routines reuse the
portable scene pools/framing; retail camera-task interpolation and decorative
scheduling remain the documented presentation adaptations.

Two corrected assumptions are visible in native tests: `459f80` decrements the
opcode-19 count twice per visit, so count 17 takes nine visits; sword HP targets
9 and 18 each take five visits under the engine's clamped two-sample stepping.
The roster sound-list selector 8 uses `489ff0`'s `4ead18` remap and the flat
`53d000[8]` table through the common audio loader. It is not indexed sound 8.
