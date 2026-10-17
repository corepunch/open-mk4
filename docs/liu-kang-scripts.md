# Liu Kang script port ledger

The supplied MK-4.EXE is the specification. Selector slot 3 is Liu Kang.
This ledger distinguishes implemented control flow from outstanding work.
Shared locomotion, contact scaling, reactions, landing and scene pools use the
existing procedural engine; this is not retail scheduler emulation.

| Family | Retail evidence | State |
| --- | --- | --- |
| HP/LP, uppercut, close attacks and combo buttons | 46d290, 4e62f0/330/360/3b0, 4e62a0, 4e61e0, 4e69b0, 4e6190 | Implemented using the reached shared branches |
| HK | 4e6c90 -> 46a760 -> 4e5d30 -> 4e5c70; clip 66, B[1], contact ticks 11–13, reverse recovery | Implemented |
| LK/crouching attacks | 4e6d08, 4e6020[3] -> 4e60b8, 46b7f0, 4e6158 | Implemented; LK avoidance ticks 7–9, B[6] at 10, contact hold 8 / miss 5 |
| Sweep | 4e67d8 -> 46efd0/46f130; clip 62, B[36], double steps to 10 then to 22 | Implemented; contact visits 17/18, reaction 19 |
| Roundhouse | 4e6e90 -> 46f390, 4e6890/4e68a8 -> 46f3e0/46f500; clip 64, B[34] | Implemented; fast startup skips four ticks; hit adds 28 recovery ticks |
| Air attacks | 4e6658, 4e6710, 4e6df0/46c970, 4e6544, 4e641c | Implemented; straight kick uses clip 61 from cursor 4, B[39], three active visits |
| LP and LK throws, independent victim tails | 4de678/4e9b68/485b10; 42c040/4de3b0/410/428/468/478, 4e9a90/ab0 | Implemented; see timing below |
| High/low/air fireball | 4e4928/4e4948 -> 49bf90/49bf50/49bfe0 | Implemented; independent task 64, C[0]/C[1] |
| Flying kick | 4e4978 -> 47edc0, 4e86a8/d0/e8/718 | Implemented; hit/block/miss and landing |
| Weapon draw/strikes/throw/pickup; sidestep | 4e4998, 46d1d0; shared RUN records | Pending |
| Fatalities I/II and fan/spikes | 4e4968/4e4934/4e4954/4e4984 | Pending |
| CPU | Shared CPU controller; character entries | Pending wiring |

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

Flying kick uses F,F+HK (24): clip 92, speed 0x23d7, Y -0x170a, gravity
0x106; contact D[12] after six opening visits. A hit plays clip 93, holds 20,
then reverses 92 and lands at gravity 0x51e; a block holds 12+12 and lands
at 0x28f. A miss reverses after landing. Reaction 31 uses clip 131, speed
-0x1999, Y -0x23d7 and gravity 0x24d. Reached normal records 15/19 now
use tumble/getup and the clip 192/down-held 193/standing 194 branches.
