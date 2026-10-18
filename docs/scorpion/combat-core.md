# Scorpion combat core: contact, block, damage and victim reactions

Historical research notes retained for their move tables and source addresses.
The experimental retail-emulation recorders and JSON traces have been removed.
References below to an oracle, fixture, scenario or x86 observation describe
the discarded research, not a current test or an independently verified claim.
Use the decoded scripts in `build/retail-disassembly/` as the primary source;
check native helpers there or in `reference/mk4decomp/`. Engine proposals and
exact emulator timing in these notes are not implementation requirements.
Our port preserves script behavior and balance with our own C engine.

## 1. Units and coordinate frame

| Fact | Evidence |
| --- | --- |
| Group position at +0x54/+0x58/+0x5c (X, Y, Z), Q16. 1.0 = 0x10000. | 0x48d750 subtracts group +0x54/+0x5c from joint positions; 0x407560 integrates +0x54.. += +0x6c.. (bytes). |
| Joint world positions are in the same space as group positions. | 0x4b83f0 writes `M·local + parent` into node(+0x28)+0x3c/+0x40/+0x44, the root's parent being group +0x54.. (bytes, 0x4b82bc/0x4b857a); 0x48d750 compares them directly with group +0x54. |
| Runtime float units = retail Q16 / 65536. | Rig translations are Q16 (`common/mk4_assets.c` divides by 65536) and are the same numbers 0x406e31 copies into node +0x30..+0x38 (bytes). |
| **+Y is down.** Airborne ⇔ group Y < group floor (+0x48). | 0x48ef40 (bytes); oracle `*_air` cases (y = −1.0, floor 0 ⇒ result 3). |
| Round-start X: P1 −1.3 (`0xfffeb334`), P2 +1.3 (`0x14ccc`), Z = 0. | 0x422ea0, 0x422f50 (bytes). Runtime currently uses ±2.0 (prototype). |
| Facing: group +0x34 bit 0 = 1 ⇒ faces −X. P1 gets `|0x1c0000`, P2 `|0x1c0001`. | 0x422e8e, 0x422f82 (bytes); contact facing test 0x48efc0/0x48f030. |
| Standing invariant: root node local Y (+0x34 of node at group +0x18) is 0 and group Y = floor. 0x4702b0 moves any root Y offset into group Y (called by the stance reset 0x45f870); landing (0x490d12) sets Y = floor and root local Y = 0. | bytes |

Consequence for the runtime (verified by construction, HYPOTHESIS only in
the group rotation): the retail **group origin is the rig root, not the
soles**. For Scorpion the rig root translation is (−0.0031, −1.173, 0.0049),
so in a Y-down world with soles at 0:

```
origin_y(fighter)      = rig_root.y − height            /* Scorpion: −1.173 − height */
joint(fighter, n)      = (x, −height, z) + turn · (pose[n].position − rig_root)   (+ rig_root.y in Y)
                       = x/z: fighter.x + turn·(pose[n] − rig_root);  y: pose[n].position.y − height
```

`pose[n].position` is `pose()` output (root includes per-frame root
motion). Retail keeps root-motion X/Z in the joints (root world = group +
M·rootmotion, 0x4b8140); `game/draw.c` keeps them for rendering too, so
contact, attachments and the drawn mesh agree. HYPOTHESIS: retail group
matrix M (built from group +0x60 angles through table 0x4f2820) equals the
runtime `turn`.

---

## 2. Contact entry points and selector tables

Every query sets a table, adds the selector in `[0x53c42c]`, and runs the
shared body 0x48d4a0 (bytes):

| Routine | Table (packed ptrs to 6-word records) | Users |
| --- | --- | --- |
| `0x48d3d0` (A) | `0x53d268` | jab (sel 1), uppercut (sel 2 or 6), VM op 0x52 |
| `0x48d3f0` (B) | `0x53d2b8` | kick runner 0x470c60 (sel 1 Scorpion, 2 Sub-Zero, 0 for 0x46a6b0), sweep 0x46f080 (sel 0x23 = 35) |
| `0x48d410` (C) | `0x53d208` | spear contact 0x49c810 (records with flag 2) |
| `0x48d430` | `0x53d0c0` | VM op 0x5e (0x45abb0) |
| `0x48d450` (D) | `0x53d178` | many specials; with selector 8 a miss also calls 0x4bd5e0(0x1392) |

### Attack record (6 words)

| Word | Meaning |
| --- | --- |
| 0 `r0` | forward reach of the far edge, Q16 |
| 1 `r1` | top edge, Q16, relative to attacker origin Y (Y-down) |
| 2 `r2` | length back from the far edge, Q16 |
| 3 `r3` | height (bottom = r1 + r3), Q16 |
| 4 | byte0 hit reaction id, byte1 air-hit reaction id, byte2 blocked reaction id, byte3 flags (bit0 LOW, bit1 MARK: sets victim group +0x40 bit 0x10) |
| 5 | damage, Q16 fraction of full health |

Scorpion's normals (verified (bytes), also stored in the fixture `records`):

| Move | Slot → record | Words | Box X (facing victim) | Box Y | Hit / air / block | Damage |
| --- | --- | --- | --- | --- | --- | --- |
| jab | 0x53d26c → 0x4ebd38 | `f333 ffffb852 cccc 7333 10b25 a3d` | [0.150, 0.950] | [−0.280, 0.170] | 37 / 11 / 1 | 0.0400 |
| uppercut | 0x53d270 → 0x4ebd50 | `e3d7 ffff0290 947a 10518 62222 28f5` | [0.310, 0.890] | [−0.990, 0.030] | 34 / 34 / 6 | 0.1600 |
| uppercut (alt, attacker group +0x40 bit 0x200) | 0x53d280 → 0x4ebd68 | `12106 fffeee57 fa9f 17a9f 62222 a3d` | [0.150, 1.129] | [−1.069, 0.410] | 34 / 34 / 6 | 0.0400 |
| high kick | 0x53d2bc → 0x4ebae0 | `15e76 ffff2667 151a9 111eb 60d10 2147` | [0.050, 1.369] | [−0.850, 0.220] | 16 / 13 / 6 | 0.1300 |
| sweep | 0x53d344 → 0x4ebb58 | `175c2 7a9f 10ccc 8a3d 1041212 23d7` | [0.410, 1.460] | [0.479, 1.019] | 18 / 18 / 4, **LOW** | 0.1400 |

(Box X is `[r0 − r2, r0]`, Box Y `[r1, r1 + r3]`, both relative to the
attacker origin. Other tables share the layout; `attack_record()` in
`tools/scorpion_moves.py` reads it. Only these five records are tied to
Scorpion's normals here.)

---

## 3. Contact geometry (0x48d750) — historical observation

The attacker's box is relative to its **group origin** (no bone). The
victim's hurtbox is the bounding box of its **joint origins**.

1. If `[0x527ae4]` ≠ 0 (round end / contacts disabled) → miss.
2. Evaluate the victim hierarchy (0x4089e0 clears node flags, 0x4b8140 poses
   it). The joint list is group +0x1c: `[count, node_1 … node_count]`,
   indexed by **rig bone ID** (node +0x1c = rig word0 >> 20, 0x406c76/0x406eb3);
   slots ≤ 0 are skipped. Scorpion: 16 joints (all rig nodes except node 1,
   ID −1; node 10 with ID 17 is included).
3. `f = +1` if the attacker faces the victim else −1: bit0 = (victim.x ≥
   attacker.x) XOR attacker group +0x34 bit 0 (0x48efc0, 0x48f030). If
   `[0x534af0]` ≠ 0 the two X values are swapped first (camera flag written
   by 0x4895ca; HYPOTHESIS: always 0 in a fixed 1D layout).
4. `u` = direction attacker origin → victim origin in XZ. For each joint:
   `proj = dot(joint − attacker, u)` (implemented as
   `cos(atan2(joint) − atan2(victim)) · |joint − attacker|` with tables
   0x53c3f0/0x53c3f4/0x53c3ec, 2048-entry sine, chopped sqrt 0x4ab290).
   `maxX/minX` over proj, `maxY/minY` over joint Y. Initial values are ∓50.
5. Victim interval along `u`: `lo = minX + 0.08·w`, `hi = maxX − 0.08·w`,
   `w = maxX − minX` (0x147a; computed after adding attacker X to both).
6. Attack interval: `x1 = r0·f`, `x2 = x1 − r2·f` (relative to attacker
   along `u`); attack `[min, max]`. Y: `y1 = ay + r1`, `y2 = y1 + r3`.
7. **Hit iff** `y1 ≤ maxY && minY ≤ y2 && max(x1,x2) ≥ lo && hi ≥ min(x1,x2)`
   (all inclusive). Otherwise miss (0x48dda0: result 0, flag bit 0 clear).

In the runtime's 1D layout (both origins at Z = 0) `proj` reduces to
`±(joint.x − attacker.x)` independent of joint Z, so a hurtbox can be a
const table per (clip, sample): forward/back X extents and Y extents of the
joints relative to the origin, mirrored by the victim's facing.

The `model_hit` function in the oracle reproduces the fixed-point steps
exactly; it matched the x86 in 240/240 seeded random cases (48 hits, 192
misses, with Z offsets, both facings, 1..16 joints).

Measured reach against the reference Scorpion joints (attacker and victim
facing each other, same floor; distance between origins; Y window =
victim-origin Y offset that still hits at distance 0.9, Y-down):

| Move | vs idle 290 s0: max distance | Y window | vs crouch 258 s0: max distance | Y window |
| --- | --- | --- | --- | --- |
| jab | 1.1292 | [−1.3058, 0.7715] | 1.2995 | [−1.3286, 0.1149] |
| uppercut | 1.0693 | [−2.0158, 0.6314] | **miss at 0.9** (box bottom 0.03 above crouch head 0.055) | — |
| uppercut alt | 1.3084 | [−2.0948, 1.0115] | 1.4784 | [−2.1176, 0.3549] |
| high kick | 1.5483 | [−1.8758, 0.8215] | 1.7185 | [−1.8986, 0.1649] |
| sweep | 1.6394 | [−0.5468, 1.6205] | 1.8095 | [−0.5696, 0.9639] |

The reference joints are derived from `common/mk4_assets.c` (listed in the
oracle); the boundary arithmetic is retail.

---

## 4. Resolution (0x48d4a0) — historical observation, 63 scenarios

After a geometric overlap:

1. `[0x532358] = 1` (a contact happened; read by finisher predicate 0x45e5e0).
2. Victim group +0x40 bit 0x20 ⇒ **ignore** (result 0, no damage).
3. Victim +0x40 bit 0x10 := record flag bit1 (MARK).
4. Victim +0x40 bit 0x08 ⇒ skip block test (always a hit).
5. Block test (record LOW ⇒ 0x48e800 else 0x48e890):
   - airborne victim (0x48ee60) ⇒ never blocks;
   - **CPU victim** (`[0x5347c8]` for P1 / `[0x532248]` for P2 is 0 —
     HYPOTHESIS name: "human player" flag; 0x470420 picks the AI controller
     0x4335e0 when it is 0): blocks iff victim action (controller +0x74) is
     `0x2002` or `0x2005`. **LOW records do not change the CPU test** (a
     standing-block action blocks a sweep).
   - **human victim**: victim +0x40 bit 0x01 ⇒ no block; else blocks iff the
     block switch is held now (P1 victim bit 0x20, P2 victim bit 0x2000 of
     `~(0x4d00a8<<16 | 0x4d00a4 & 0xffff)`); LOW additionally needs down
     (P2 victim 0x200, P1 victim 0x2) held. A block **animation is not
     required** for humans.
6. **Blocked**: damage `d·0.2` (0x3333) through 0x48de80; if the victim
   survives: result **1**, reaction = record byte2. If chip damage kills,
   fall through to the hit path below (result 2, hit reaction) without
   applying full damage.
7. **Hit**: damage `d` (0x48de80). Then 0x48ee60: airborne ⇒ result **3**,
   reaction = byte1; else if Y > floor snap Y to floor (0x4907b0); result
   **2**, reaction = byte0. Victim controller +0x7c (hit counter) += 1
   (only on hit/air-hit).
8. Dispatch reaction (0x480f80), set `[0x53c448]` = result and `[0x53c44c]`
   bit 0. Results reach the attacker exactly as the existing branch oracle
   injects them.

Selected verified rows (victim at distance 0.9, idle joints):

| Case | Result | Reaction | Victim health | +0x7c | +0x80 |
| --- | --- | --- | --- | --- | --- |
| kick, CPU idle (action 0x200b) | 2 | 16 | 0.87001 | 1 | 0.1300 |
| kick, CPU 0x2002 or 0x2005 | 1 | 6 | 0.97401 | 0 | 0.0260 |
| kick, human holding block | 1 | 6 | 0.97401 | 0 | 0.0260 |
| kick, airborne (Y −1, any block) | 3 | 13 | 0.87001 | 1 | 0.1300 |
| sweep, human block without down | 2 | 18 | 0.86000 | 1 | 0.1400 |
| sweep, human block + down | 1 | 4 | 0.97202 | 0 | 0.0280 |
| sweep, CPU 0x2002 (standing block) | 1 | 4 | 0.97202 | 0 | 0.0280 |
| sweep, victim at Y −1.0 | miss | — | 1.0 | 0 | 0 |
| kick, victim +0x40 = 0x20 | miss | — | 1.0 | 0 | 0 |
| kick, +0x40 = 0x08 vs CPU block | 2 | 16 | 0.87001 | 1 | 0.13 |
| kick, +0x40 = 0x01: human block / CPU block | 2 / 1 | 16 / 6 | | | |
| kick, chip KO (health 0.02, blocking) | 2 | 16 | 0 | 1 | 0.026 (winner 1) |
| kick, lethal (health 0.10) | 2 | 16 | 0 | 1 | 0.13 (winner 1) |
| kick, `[0x532354]` already set | 2 | 16 | unchanged | 1 | unchanged |
| kick vs Goro (char 15) | 2 | 16 | 0.93501 | 1 | 0.065 |
| kick, victim damage scale 0.5 / 2.0 / −1.0 | 2 | 16 | 0.935 / 0.870 / 0.870 | | |
| kick, `[0x53d7f8]` ≠ 0 | 2 | 16 | unchanged | 1 | 0.13 |
| kick, `[0x527ae4]` ≠ 0 | miss | | | | |
| kick, attacker facing away | miss | | | | |
| kick, victim Y 0.25 below floor | 2 (Y snapped to floor) | 16 | | | |

All five normals behave identically in the block/air matrix (fixture
`resolution`, 9 cases each).

## 5. Damage and health (0x48de80) — historical observation

```
if (ko_winner /*0x532354*/) return;                 /* no more damage after a KO */
d = mul(victim_char == 15 /*Goro*/ ? 0.5 : 1.0, d);
s = victim_is_p2 ? [0x534834] : [0x5347a4];         /* victim damage scale */
if (s < 0 || s > 1.0) s = 1.0;   d = mul(s, d);
victim_ctrl[+0x80] += d;                            /* damage accumulator */
health = (victim_is_p1 ? [0x534a9c] : [0x5322ec]);
if (![0x53d7f8]) health -= d;  if (health < 0) health = 0;
if (health == 0) ko_winner = victim_is_p1 ? 2 : 1;
```

`mul(a,b) = (a·b) >> 16` (signed, floor). Full health is 0x10000
(0x422710 / 0x42277e reset both players; 0x422690 clamps a carried-over value
to 1.0 in the `[0x53dad4] == 1` mode). Scale cells are reset to 1.0 at round
start (0x42241f) and whenever the victim returns to neutral (0x48e080).

## 6. Call-site gates of Scorpion's normals

| Move | Gate before the query | Latch / branch | Evidence |
| --- | --- | --- | --- |
| jab (0x46ddd0, selector 1) | no query if victim action is 0x2001 (crouch), 0x106, 0x107; 0x10c (victim sweeping) with cursor < 5; 0x102 with victim +0x5c == 1 | none (one query per strike visit) | historical observation, fixture `jab_gates` |
| uppercut (0x46cde0) | no query if victim action 0x2001; selector 6 instead of 2 if attacker group +0x40 bit 0x200 (0x46c760, HYPOTHESIS meaning) | after contact attacker action := 0x1006 (0x470e80); result 1 vs 2/3 selects the recovery loop count | bytes |
| high kick (runner 0x470c60, block 0x4e5d30 = {sel 1, 0x48d3f0, samples 17,17,16, −1, 0x46a770}) | no query if victim action 0x2001 | first contact latches task-local `[0x53c444] = 0x10000`; later visits skip | bytes |
| sweep (0x46f080, selector 35) | none | queries each visit until a contact, then 0x46f130 | bytes; visits 18..19 in `original_scorpion.json` |

Consequence: in retail a crouching (action 0x2001) victim cannot be hit by
Scorpion's jab, uppercut or high kick at all; geometry is never consulted.

## 7. Victim reaction dispatch (0x480f80)

`routine = word[0x4ea4e8 + id·4]`; 0x48f880 retasks the victim controller
(0x41fa50: callback := routine, +0x84 := 0, +0x10 := 1, sleep := 1,
continuation stack reset to task +0x88), runs 0x48f9c0 as the victim (0x406bb0,
0x4906e0 clears velocities/gravity/spin, 0x4901d0), then the word at 0x4e8998
(`0x481010`) as the victim. The victim's own move is cancelled immediately.
The reaction runs on the victim's next scheduler visit (same tick if the
victim task follows the attacker in the list).

| Id | Routine | Used by |
| --- | --- | --- |
| 1 | 0x486c40 | jab blocked |
| 4 | 0x486bb0 | sweep blocked |
| 6 | 0x486af0 | uppercut / kick blocked |
| 11 | 0x482070 | jab air hit (sets victim damage scale 0.6) |
| 13 | 0x481d30 | kick air hit (sets victim damage scale 0.6) |
| 16 | 0x482a10 | kick hit (calls stagger check 0x488ea0, script 0x4e9208) |
| 18 | 0x483050 | sweep hit and air hit (scripts 0x4e9460/0x4e9498) |
| 34 | 0x4838e0 | uppercut hit and air hit (script 0x4e9758) |
| 37 | 0x483b40 | jab hit (stagger if +0x80 ≥ 0.2, ≥ 0.15 for CPU, or victim is Goro; else script 0x4e9798) |

### Victim playback and motion — historical observation, Scorpion victim (char 0)

Tick 1 is the first victim dispatch after the contact; "control" is the
tick at which the victim enters 0x470420/0x46f650 (fighter control).
Segments are `{at, first, step, period}` relative to tick 1 (= move tick 0).
Motion is Q16 per tick along the victim's away-from-attacker axis
(velocity = speed × victim task +0x40/+0x44 direction cells, negated);
X/Y shown in units.

| Scenario (result) | Clip phases (segments) | Control at | Motion |
| --- | --- | --- | --- |
| jab hit (2, id 37) | 311: `{0,4,1,1} {9,14,2,1}` | 13 | none |
| jab hit, accumulated ≥ threshold (2, id 37 → stagger) | 182: `{0,0,2,1} {18,38,0,1} {20,40,2,1} {24,50,0,1}` | 26 | vx 0.1 constant ticks 1..19 (Δx 1.90), stop at 20 |
| kick hit (2, id 16) | 311: `{0,4,0,1} {13,6,1,1}` (hold 4 for 13 ticks, then 6..18) | 27 | v0 0.15 (0x2666) away, ×0.9 (0xe666) per tick by group callback 0x4913a0 before integration; stops when v² ≤ 0x41 (tick 15); Δx 1.0405 |
| sweep hit / sweep air (2 / 3, id 18) | 195: `{0,0,1,1}` (29 ticks), 197: `{0,0,1,1}` (29 ticks) | 59 | vx 0.06 constant to tick 29 (Δx 1.74); Y forced to floor; sound 19 at tick 30 |
| uppercut hit (2, id 34; also alt selector) | 128: `{0,0,1,1} {54,55,2,1}` (81 ticks), 328: `{0,0,1,1}` (15 ticks) | 97 | vx 0.03, vy₀ −0.20, gravity +0.008/tick (group +0x4c); apex 2.40 above floor at tick 25; lands tick 50 (Δx 1.47); sounds 23, 19 at 54; action 0x200e during 328 |
| uppercut air from Y −1.0 (3, id 34) | 128 `{0,0,1,1} {53,52,1,1} {55,55,2,1}`, 328 from tick 83 | 98 | same launch from current height; lands tick 54 |
| kick air from Y −1.0 (3, id 13) | 189: frames 0..29 ticks 1..30, hold 29 to landing; 191: `{0,0,1,1}` from tick 35 | 51 | vx 0.07, vy₀ −0.14, gravity 0.01; lands tick 34 (Δx 3.59); sound 19; action 0x200e during 191 |
| jab air from Y −0.5 (3, id 11) | 183: 0..29, hold, 31..33; 185: `{0,0,1,1}` from tick 35 | 51 | vx 0.05, vy₀ −0.14, gravity 0.01; lands tick 31 |
| jab blocked (1, id 1) | 321: `{0,3,2,1} {4,7,-2,1}` | 8 | action 0x2005; v0 0.24 ×0.9 (Δx 1.13 by tick 7, still moving) |
| kick / uppercut blocked (1, id 6) | 321: `{0,3,2,1} {4,7,-2,1} {7,3,2,1} {11,7,-2,1}` | 15 | action 0x2005; v0 0.20 ×0.9 |
| sweep blocked (1, id 4) | same as id 6 | 15 | v0 0.15 ×0.9 |
| uppercut lethal (KO) | 128 as above, then **hold 109** (no getup) | none in 159 ticks | KO winner 1 |
| kick lethal (KO) | as kick hit | 27 (controller then sees `[0x532354]`) | |

CPU-controlled victims run the AI during the getup (0x4830d0 → 0x4339b0):
in `sweep_hit_cpu_victim` the AI starts its own sweep (clip 63, action
0x10c) at tick 40 instead of finishing 197. Human victims with no input
finish the getup. Clip numbers above are the char-0 victim's; other victims
may select other clips (op 0x3c tables) — not verified.

### Juggles, combo counter and damage scaling

- Air hits (result 3) are always possible; air victims never block.
- Air reactions 11/13 set the victim's damage scale to 0.6 (0x9999,
  0x482a50 callers 0x481d3a/0x48207a). Verified: a second kick at tick 8
  during kick-air takes 0.078 (= 0.13 × 0.6), result 3, counter 2, and
  restarts reaction 13 from its current height (`kick_air_then_kick`).
- Other reactions set 0.5/0.4/0.6 (0x482300, 0x4823c0, 0x4824a0,
  0x482b35.., 0x482e90) and 0x494640 sets 0.3 when the counter > 3
  (bytes; callers not traced).
- Returning to neutral (0x48e080) resets: +0x7c counter, +0x80 accumulator,
  action, group +0x44 callback, group +0x40 bits 0x2e8, and the victim's
  damage scale to 1.0; a counter ≥ 3 first calls 0x48e290 (HYPOTHESIS:
  combo announcement).
- `uppercut_then_kick`: a kick at tick 20 of the uppercut launch misses
  (victim above the kick box).

### No hitstop

Diffing all globals 0x524000–0x540000 across an uppercut contact shows only
debug-box cells, health, `[0x532358]` and contact scratch; the attacker's
sleep (+0xdc) and the freeze selector `[0x53dbc0]` are unchanged. The
attacker continues its script on the next tick (as the existing branch
fixtures assume). Historical observation for the contact call; not for later ticks.

## 8. Physics used by reactions (historical observation as executed)

Per frame the fight step (0x41fd70) runs the task pump 0x41f570, then
0x4891e0 (camera/separation, **not executed by the oracle**), whose tail
0x490c20 does, for P1 then P2 group: call group +0x44 if set; if gravity
+0x4c ≠ 0: `vy += g`; when `vy ≥ 0` and `y + root.y + vy ≥ floor`: zero
+0x6c..+0x80 and +0x4c, `y = floor`, root local Y = 0, then call the
controller's one-shot landing callback (+0x4c, 0x490da0). Then 0x4051e0
integrates every group: `pos += vel`, `angles += spin` (0x407560).

## 9. Simultaneous hits (HYPOTHESIS)

Contact runs inside the attacker's task. A hit retasks the victim before
its own task runs later in the same pass, so the victim's query never
happens that tick: no trades; the earlier task in the list wins ties. The
list order of P1/P2 controllers was not measured. `resolve_hits`'s
snapshot trade is therefore not retail behaviour (unverified).

## 10. Throw contact (partial)

Throw attempts are not queries through 0x48d4a0. Entry 0x42c560 calls the
precondition 0x42c3d0 (victim action 0x1003 ⇒ fail; human/CPU checks;
victim health 0 ⇒ fail; then 0x4398c0), sets attacker action 0x500 and
`[0x532254] = 0x40`, and starts per-character programs; Scorpion's script
(table 0x4de7a0, char 0 → 0x4de6e4) opens victim clip 152 (op 0x0e) and
own clip 75, and subtracts 0x2666 (0.15) health with op 0x63. Distance
test, escape and victim playback are **unresolved**.

## 11. Round, health and finish-him hook

- Health: P1 `[0x534a9c]`, P2 `[0x5322ec]`, 0x10000 = full.
- KO: 0x48de80 writes `[0x532354]` = winning player (1 or 2). From then on
  damage is not applied, reactions still dispatch. A knocked-down KO victim
  never gets up (uppercut_lethal holds 128:109); a standing one returns to
  0x46f650, which calls 0x48fc20 before any input handling and returns early
  when it reports a KO (KO path of 0x48fc20/0x48fb90 not traced).
- Round task code reading the flag: 0x420841 picks the winner's controller
  into `[0x534824]`; round end sets `[0x527ae4] = 1` (contacts off, 0x4219dd).
  Finisher commands require `[0x532354]` ≠ 0, `[0x532358]` = 0 and
  `[0x4ee624]` ≠ 0 (predicate 0x45e5e0) — handed to the fatality research.
- Round timer: **unresolved**.

## 12. Engine requirements (proposals in game.h style)

1. Retail units in simulation: `float x, y, z` with Y down and
   `floor`, or Q16 `int32_t`; 1.0 = one retail unit. Start at ±1.3.
2. Velocity and gravity: `float vx, vy, vz, gravity;` plus
   `FighterFn physics` (group +0x44 analogue) and `FighterFn on_land`.
   Order per tick: moves → physics callback → gravity/landing → integrate.
3. Const attack records:
   ```c
   typedef struct AttackRecord {
     int32_t reach, top, length, height;      /* Q16 */
     uint8_t hit, air, blocked, flags;        /* ATTACK_LOW = 1, ATTACK_MARK = 2 */
     int32_t damage;                          /* Q16 of full health */
   } AttackRecord;
   ContactResult battle_contact(Battle *b, Fighter *attacker, const AttackRecord *r);
   ```
   called from move events (replacing `SF_SCRIPT_CONTACT`/reach/push).
4. Hurtbox: per (clip, sample) const extents `{back, front, top, bottom}` of
   the 16 joints relative to the origin (generated offline from `pose()`
   with rig root removed, keeping root-motion X/Z), mirrored by facing.
5. Fighter combat state: `int32_t health; int hits; int32_t accumulated,
   damage_scale; unsigned body_flags /* 0x01 no-block, 0x08 unblockable,
   0x20 untouchable */; bool human; int action;` and Battle
   `int ko_winner; bool contacts_off;`.
6. Block input: a held-block / held-down bit read at contact time for
   human fighters; CPU fighters block through action codes 0x2002/0x2005.
7. Reaction table `const ReactionDef reactions[64]` (id → victim
   MoveSequence phases + motion params + damage-scale side effect) and a
   victim-side `fighter_start_move` at contact time that cancels the
   victim's move and pending continuations.
8. Return-to-neutral hook (0x48e080 equivalent) clearing counters/scale.
9. Deterministic task order for ties (P1 before P2, HYPOTHESIS).

## 13. Unresolved

- 0x4b8140 hierarchy evaluation was stubbed; joints are runtime-derived.
  Retail group matrix vs runtime `turn` (0x4f2820 table) unverified.
- Camera/separation/arena bounds 0x4891e0 (0x489c70, 0x489e30, 0x4897e0
  when fighters < 0.5 apart and |ΔY| ≤ 0.7) and walls: not run; wall
  interaction of pushback unknown.
- Attacker during reactions was not simulated; ties (§9) not executed.
- Residual block-slide velocity after control returns (0x48e080 →
  0x490330/0x490310) not traced.
- Human held-block continuation (block-hold controller near 0x4602b4) needs
  animation data; not traced.
- Stagger/branch helpers 0x488ea0, 0x482950, 0x483af0, 0x48ebd0, 0x494520,
  0x482d90 only exercised on the paths listed.
- Damage-scale setters 0x482300/0x4823c0/0x4824a0/0x494640 callers.
- Meaning of victim actions 0x106/0x107/0x102 and victim +0x5c (jab gate);
  attacker flag 0x200 (uppercut selector 6).
- Other victims' reaction clips; Goro special cases beyond half damage.
- Throw distance/escape (0x42c3d0, 0x4398c0, 0x42c590), round timer,
  0x48e290 combo display.

