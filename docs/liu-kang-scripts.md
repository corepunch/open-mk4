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
| LP and LK throws, independent victim tails | 42c560/42b6b0; roster slot 3 | Pending research and implementation |
| High/low/air fireball | 4e4928/4e4948 -> 49bf90/49bf50 | Pending |
| Flying kick | 4e4978 -> 47edc0 | Pending |
| Weapon draw/strikes/throw/pickup; sidestep | 4e4998, 46d1d0; shared RUN records | Pending |
| Fatalities I/II and fan/spikes | 4e4968/4e4934/4e4954/4e4984 | Pending |
| CPU | Shared CPU controller; character entries | Pending wiring |

Normal attacks preserve their command routing, contact/miss/block recovery,
repeat/cancel windows and high-avoidance clearing on interruption. Shared
native tails use the existing portable body and reaction controllers. Exact
pose interpolation and camera/task ordering remain engine adaptations.
