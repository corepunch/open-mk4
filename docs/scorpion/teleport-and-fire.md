# Scorpion: teleport punch and breathe fire (retail spec)

Historical research notes retained for their move tables and source addresses.
The experimental retail-emulation recorders and JSON traces have been removed.
References below to an oracle, fixture, scenario or x86 observation describe
the discarded research, not a current test or an independently verified claim.
Use the decoded scripts in `build/retail-disassembly/` as the primary source;
check native helpers there or in `reference/mk4decomp/`. Engine proposals and
exact emulator timing in these notes are not implementation requirements.
Our port preserves script behavior and balance with our own C engine.

## 0. The world step these moves depend on

Order inside one fight frame (verified at `0x41fde9..0x41fe7e`):

1. `0x45c560` input/command recognition, `0x45c7c0`, `0x4a1cd0`.
2. `0x49cb80` → **task pump `0x41f570`** (fighter controller tasks, spawned
   children, camera task). Moves run here.
3. **`0x4891e0` fighter-pair/world pump**: computes the P1→P2 unit axis
   (`0x530238`,`0x53023c`; negated copies `0x530230`,`0x530234`), fighter
   distance `0x53019c` (X/Z plane), camera side, per-fighter squared arena
   radius (`0x489c70`: P1 → `0x534b34`, P2 → `0x5300c4`, wall clamp), the
   overlap resolver `0x4897e0` (when `|Δy| ≤ 0xb333`, countdown `0x532254`
   expired and distance `< 0x8000`), then
   `0x490c20` → `0x490c60` for P1 then P2: **call group `+0x44` if non-zero**,
   then if `+0x4c` (gravity) ≠ 0: `vy(+0x70) += gravity`; landing when
   `y + [root+0x34] + vy ≥ floor` → zero `+0x6c..+0x80`, `+0x4c`, root
   `+0x34`; `y = floor`; jump to controller task `+0x4c` if set (`0x490da0`).
4. `0x4227a0` timers.
5. `0x4051e0` object lists: per-object `0x407560` (`[0x4d011c]`):
   `pos += vel` (`+0x54..+0x5c += +0x6c..+0x74`), angles `+0x60..+0x68 +=
   +0x78..+0x80` (wrapped).

Consequences an implementation must keep:

- The `+0x44` callback runs **after** all tasks and **before** integration.
- The axis used by a task on tick *t* is the one computed by the world pump of
  tick *t−1* (visible in the early block scenario, §1.6).
- Task fields (all verified writes at `0x4233e0`/`0x423460`): controller task
  `+0x34` character, `+0x38/+0x3c` opponent group/task, `+0x40/+0x44` packed
  pointers to the own axis words (P1: `0x530238/0x53023c`, P2:
  `0x530230/0x530234`), `+0x68` attack selector, `+0x74` action,
  `+0x7c` hits-taken counter (incremented on the victim task by `0x48d6f6`).
- Group fields: `+0x24/+0x28` clip/cursor, `+0x34` bit 0 facing flip, `+0x40`
  flags (`0x40` teleporting/camera-ignore, `4` in hit reaction, `1` busy),
  `+0x44` physics callback, `+0x48` floor, `+0x4c` gravity, `+0x54/58/5c`
  position, `+0x64` yaw, `+0x6c/70/74` velocity, `+0x78..+0x80` angular rate.
- Arena: `0x534540` = R² from table `0x4dad60` (9.5 for most arenas,
  8.5 for entry 6), set at `0x4236ce..0x423703`. Oracle uses 9.5 (`0x5a4000`).

## 1. Teleport punch (D,B,HP; also airborne)

### 1.1 Trigger

- High-punch command record `0x4e49b8`: words `0x0000aa02, 0, 0x07141800`
  → tokens D then B (low byte first), window `0x18` (24), flags `0x14`,
  action 0, predicate index 7.
- Flags: `0x04` selects table `0x4e8910` (`[0]` = `0x47fc90`); `0x10`
  allows the command while airborne. Matcher `0x45de00`: without `0x10` a
  record is rejected when `y ≠ floor` (unless group `+0x40 & 0x200`,
  unresolved meaning) at `0x45dfb4..0x45dff8`; any record is rejected when
  group `+0x40 & 1` or `& 4` (`0x45e001..0x45e022`); then the predicate runs
  (`0x45e0a0`, table `0x4e4ec0`).
- Predicate `0x45f0c0` passes when **both** hold:
  1. opponent task `+0x7c` (hits taken) `< 3` (`0x45f5b0`);
  2. ticks since this fighter's last teleport `≥ 0x45` (69):
     `0x48e3f0(1)` = `[0x534858] − [0x53c2a8 + 8·1 + 4·player]`. The stamp is
     written by `0x48e3a0` on the move's first wait dispatch (step 0).
- Entry `0x47fc90`: task `+0x68 = 0x404`, `+0x74 = 0x209`; `0x490c00` =
  face opponent (`0x490bd0`) then **flip facing** (`0x490350`, xor bit 0);
  `0x4901a0` sets group flag `0x40`; script `0x4e87b0`.
- Script `0x4e87b0`: `58` motion (`0x45aef0`: horizontal = own axis ×
  `−0x2666` via `0x490390`, `vy = −0x170a`, gravity `+0x4c = 0x1ca`);
  `0f` open clip 312 at sample 0 (no pose submit); `27` sound `0x23`;
  `38` install `+0x44 = 0x480040`; `32` → `0x47fcf0`.

### 1.2 Vanish flight (group callback `0x480040`, every world pump)

Motion: Scorpion moves **away** from the opponent at 0.15/tick (`0x2666`),
rising with `vy0 = −0x170a` (−0.090) and gravity `0x1ca` (0.0070/tick²).
Facing is flipped, so he faces his travel direction. Per tick the callback:

1. `0x42fed0`: camera-relative bearing of Scorpion (`0x431c70` uses the
   camera object `[0x524ed0]` position, minus camera yaw `+0x64`); if
   `|Δ| > 0x5999` (0.35 rad) → **reappear**.
2. else if own `r² > R² − 5.0` (`[0x534b34]` vs `[0x534540] − 0x50000`) →
   reappear.
3. else if fighter distance `[0x53019c] ≥ 0x5e667` (≈5.9) → reappear.
4. else if `vx + vz == 0` → reappear.
5. else if cursor `≤` stored count → `0x4297a0` advance clip 312 one sample
   and submit the pose (clamped at sample 9).

Reappear `0x480140` (all verified by trace):

- offset = 2.4 (`0x26666`) if distance > 4.0, else `2.4 + (4.0 − d)`.
- new X/Z = opponent X/Z + own axis × offset (i.e. on the far side of the
  opponent). If that point's r² > R², new X/Z = **opponent X/Z** exactly.
  Y, vy and gravity are kept (the arc continues).
- `0x4298d0`: cursor = last sample (9), submit pose.
- `0x490390(−0x2666)`: horizontal velocity = own axis × −0.15. The axis is
  still the pre-teleport one, so in the normal case he now travels back
  **toward** the opponent; at the wall (placed on the opponent) he travels
  away from him.
- `0x4914a0`: clear `+0x44`.

Measured reappear tick R (the trigger is the camera bearing unless noted):

| Scenario | Start X (self, opp) | R | Reappear X/Z | Trigger |
| --- | --- | --- | --- | --- |
| ground / airborne / opponent airborne | −2, +2 | 10 | +4.25 | camera |
| near | −1, +1 | 11 | +3.60 | camera |
| far | −4, +4 | 0 | +6.25 | distance ≥ 5.9 |
| opponent at wall | +4.5, +8.5 | 6 | +8.35 (on opponent) | camera, outside-arena clamp |
| self at wall | −8.5, −4.5 | 4 | −2.25 | camera |
| self against wall | −9.2, −5.2 | 1 | −2.95 | own r² > R² − 5 |
| off axis (opp Z = +2) | −2, +1 | 6 | (+2.87, +3.25) | camera |

(Positions are after the same tick's integration step, so −0.15 along X
is already applied.) **The vanish length is camera dependent**; the oracle
runs the original fight camera task (§4, §5 item 1). The opponent's height is never
read: an airborne opponent does not change the teleport.

### 1.3 Contact window

On tick R+1 the controller task (`0x47fcf0`, re-checks `+0x44` every tick)
sees the callback cleared: `0x490bd0` re-faces the opponent (flips facing
when needed, e.g. the wall case), then param runner `0x4704d0` with block
`0x4e87e0` = `{count 60, selector 4, query 0x48d450, on_contact 0x47fd90,
on_landed 0x47fe40, on_expired 0x47fd90}`:

- first query immediately (R+1);
- each later tick: `0x48ef30` (airborne = `y < floor`). Airborne →
  `count−−`, query again (or `on_expired` at 0). Grounded → action `0x1006`
  (`0x470e80`) and `on_landed`.
- any contact → action `0x1006`, `on_contact`.

Query `0x48d450`, selector 4, table `0x53d178` → record `0x4ebf48`:
rect `(76021, −15073, 43909, 37355)`, reaction 90 (ground and air),
blocked reaction 0, flags 2 (blockable), damage `0x170a` (9.0% of health).
Static reading of `0x48d4a0` (not executed: contact is injected): blocked →
chip `damage × 0x3333` (20%) and status 1; unblocked → full damage, status 3
when the victim is airborne (`0x48ee60`) else 2, reaction from the record,
victim task `+0x7c++`, reaction routine via table `0x4ea4e8` (`0x480f80`).
Damage is halved for character `0xf` (`0x48deb7`).

### 1.4 Branches after contact (attacker side)

- **Miss**: lands (world pump) on tick L; tick L+1: `0x47fe40` = face
  opponent, return to controller `0x46f650` (end = L+1). Ground start: L = 24,
  end 25, independent of R. Airborne start (Y two units up): L = 39, end 40.
- **Hit / air hit** (status 2 or 3, identical for the attacker): `0x47fd90`
  clears flag `0x40` (`0x4902f0`), zeroes velocity, gravity and spin
  (`0x4906e0`); hangs 6 ticks; then gravity `0x1999` (0.1/tick²), waits for
  landing (`0x48f390` polls `+0x4c`), `0x47fe40`, controller. Hit on tick C
  from the ground arc: end `C + 9` (C=11 → 20, C=18 → 27); from two units
  higher: C=11 → 24.
- **Block** (status 1): `0x47fd90` → `0x47fec0`: action `0x1011`,
  `0x532254 = 5` (suppresses the overlap resolver for 5 ticks),
  `0x48fed0`: in the victim's context Scorpion's X/Z = opponent X/Z +
  opponent's own axis × 0.5 (snapped half a unit in front of the
  defender, on the side given by the previous tick's axis);
  `0x490890(0x4e8808)`: horizontal = own axis × `−0x28f` (0.01/tick away),
  `vy = −0x2147`, gravity `0x1ca`; `0x47db60`: clip 301 at its last sample,
  spin rate `+0x78 = −0x4ccc`, `+0x44 = 0x4707e0` (sound `0x58` every 12
  ticks via task `+0x5c`). Wait for apex `vy ≥ 0` (`0x48f490`), pass
  `0x48f410` (one-tick wait, then continue once `y ≥ −0x28000`), then `0x47ff80`: clear spin and angles,
  face opponent, script `0x4e8818`: `09` clip 301 backwards 8→0, `60` wait
  landing, `39` clear `+0x44`, `0x47ffc0` → generic landing `0x46fd90`
  (action `0x2008`, clip 304 samples 18→27), controller. Block on C: end
  `C + 50` in both measured cases (landing height dependent in general).
- **Counter expiry** (60 airborne queries): `0x47fd90` with status ≠ 1 →
  hit-style hang and drop. Not reached by any scenario.

No routine executed during the move reads button or direction latches
(oracle read-watch on `0x4d00a0..0x4d00b0`, `0x53c170..0x53c1c0`,
`0x53c200..0x53c210`, `0x524e40..0x524e50`): there are no cancels or
follow-ups.

### 1.5 Displayed samples (sparse `{at, clip, first, step, period}`)

| Scenario | Segments | End |
| --- | --- | --- |
| ground miss | `{0,312,0,+1,1} {10,312,9,0,1}` | 25 |
| ground hit/air hit at 11 | same | 20 |
| ground hit at 18 | same | 27 |
| ground block at 11 | `{0,312,0,+1,1} {10,312,9,0,1} {11,301,8,0,1} {32,301,7,−1,1} {40,301,0,0,1} {51,304,18,+1,1}` | 61 |
| ground block at 18 | `… {10,312,9,0,1} {18,301,8,0,1} {39,301,7,−1,1} {47,301,0,0,1} {58,304,18,+1,1}` | 68 |
| airborne miss / hit at 11 | `{0,312,0,+1,1} {10,312,9,0,1}` | 40 / 24 |
| reappear on tick R | `{0,312,0,+1,1} {R,312,9,0,1}` (R ≥ 10: hold 9 from 10) | 25 |

Rule: clip 312 advances one sample per world pump while vanishing, clamped at
9; reappearance forces sample 9; it is held until the controller returns
(miss/hit). The block branch is clip 301 (hold 8 until apex+2, reverse to 0,
hold until landing) then clip 304 from 18.

### 1.6 Motion table (ground, distance 4, P1 at X = −2)

Per tick `x` (Q16) = −140902, −150732 … −229372 (t9); t10 = 278528;
then −9830 per tick to 150738 at t23; landed t24 (y = 0x10000). Y:
60096, 55114, 50590, 46524, 42916, 39766, 37074, 34840, 33064, 31746,
30886, 30484 (apex t11), 30540, … 61384 (t23). Full rows: fixture.

Early block artifact: with contact on R+1 the snap uses the axis from before
the swap of sides, placing Scorpion at the opponent's front (X 1.49); with
contact at 18 he is placed on his own side (X 2.51). Implement the rule
(opponent + opponent→attacker axis × 0.5 as of the previous world pump).

### 1.7 Sounds

`0x23` at entry (`0x489f90`), `0x58` every 12 ticks during block flight.
Hit/block sounds belong to the stubbed contact/reaction code.

## 2. Breathe fire (D,F,LP)

### 2.1 Trigger

Low-punch record `0x4e49e4`: `0x0000dd02, 0, 0x00011428` → D then F,
window 20, flags `0x01` (table `0x4e6ef8`, action `0x28` → `0x468610`),
**no predicate**, grounded only (no `0x10`). Entry `0x468610`: task `+0x74 =
0x115`, script `0x4e5b58`.

### 2.2 Program

- `0f` open clip 448 at 0; `12` cursor = 1; `05` advance to 4 (one per tick);
  `35 0x416ae0`; `05` to 8; `35 0x416d40`; `05` to 11; `32 0x468640`.
  Each native call costs one dispatch (sample held).
- `0x416ae0` (step 4): creates two flame render objects attached to bone 2
  and bone 10 of Scorpion's skeleton (`0x408cb0` on group `+0x1c`), tags
  −20/−21, effect descriptors `0x50c39c`, `0x50c3d4`/`0x50c3f0`. Render only.
- `0x416d40` (step 9): finds the tagged objects (`0x408ad0`) and sets their
  `+0x3c` (`0x8000`, `−0x16666`). Render only.
- `0x468640` state machine (task `+0x84`, table `0x4688c4`):
  - state 0 (step 13): sound `0xf` (`0x489f30`); spawn smoke task
    `0x4163f0` (type `0xb5`, particles, render only); test opponent group
    `+0x40 & 4` (set by `0x481010` when a reaction starts, i.e. the opponent
    is already in a hit reaction):
    - clear → target sample 18, state 3, `0x428f60` = half speed: one sample
      per two ticks, the in-between tick submits a 50% blend of cursor and
      cursor+1 (`0x407d00`, weight `0x8000`).
    - set → target 16, state 1, `0x428d70` = full speed.
  - state 3 / 1 (target reached): spawn hit task `0x4689b0` (type `0x72`,
    task `+0x24` = parent), target 28, state 4 (half speed) / 2 (full).
  - default: `0x416db0` (flame objects off), script `0x4e5b98`: advance to
    30, `0x4688e0` → `0x416cb0`, finish clip forwards to 34, controller.

### 2.3 Fire contact (child task `0x4689b0`)

First dispatch is in the same task pass as the spawn: `0x49c540` copies the
opponent links into the child (`+0x38` opponent group, `+0x3c` opponent task,
`+0x40/+0x44` axis pointers), counter 30, query `0x48d450` with selector
`0x1e`. Contact → exit. No contact → it checks
`[[0x53c414]·4 + 0x74] == 0x115` intending "parent still breathing", but
`0x49c540` has overwritten scratch `0x53c414` with packed `&0x53023c` (P1)
or `&0x530234` (P2), so the test reads **`[0x5302b0]` / `[0x5302a8]`**. No
instruction in the linear decode references those words; in the oracle they
are zero, the test fails and the child exits. **Measured result: exactly one
fire contact query**, at step 27 (normal) or 19 (opponent in reaction).
Probe `probe_fire_child_reads_0x5302b0` writes `0x115` there and the child
then queries every tick 27..53, proving the dependency. Treat this
single-tick window as retail behaviour unless runtime evidence shows those
words non-zero.

Record (selector 30, `0x4ebf90`): rect `(151126, −51576, 146604, 47054)`,
reaction 173 (ground and air), blocked reaction 0, flags 0, damage `0x1eb8`
(12.0%). Flags lack bit 1, so by the static reading of `0x48d4a0` a blocked
contact applies 20% chip **and then the full hit path** (unblockable).
Victim reaction 173 → `0x483c50` (enter reaction `0x481010`, spawn task
`0x416e50` type `0xb6`, grounded → `0x483d80`, airborne → `0x489ff0(1)` and
`0x481fa0` loop). Not executed; see §5. The fire does not travel: there is no
projectile; the flame objects are children of Scorpion's bones.

The parent ignores the child's result: hit, block and miss play identically.
Distance does not change the program (near 1.5, normal 4, far 8 all equal).

### 2.4 Displayed samples (clip 448)

| Case | Segments | End |
| --- | --- | --- |
| opponent not in reaction | `{0,1,+1,1} {4,4,+1,1} {9,8,+1,1} {13,11,+1,2}+blend {48,29,+1,1}` | 54 |
| opponent in reaction (flag 4) | `{0,1,+1,1} {4,4,+1,1} {9,8,+1,1} {13,11,+1,1} {19,16,+1,1} {32,28,+1,1}` | 39 |

`+blend`: on ticks 14, 16 … 46 the displayed pose is the 50% blend of
sample n and n+1 (n = 11 … 27). Contact query ticks: 27 / 19. Sound `0xf`
at 13. Spawns: smoke at 13, hit task at 27 / 19. Scorpion does not move.

## 3. Engine requirements (minimal C proposals)

1. **Body physics per fighter** (Q16, explicit context):
   `typedef struct Body { int32_t pos[3], vel[3], spin[3], angle[3];
   int32_t gravity, floor; BodyFn physics; unsigned flags; } Body;`
   with a world step in retail order: tasks → pair update (axis, distance,
   r², wall clamp, overlap push with countdown) → `physics` callbacks →
   gravity/landing → integration. Landing must be able to resume a waiting
   move (`Fighter.on_landed`).
2. **Pair geometry**: `typedef struct Pair { int32_t axis[2], distance; int32_t
   radius_sq[2]; } Pair;` updated once per tick, read with one tick of lag by
   moves.
3. **Camera model** with the retail framing/tracking task and a
   "camera ignores fighter" flag, or at minimum the bearing test
   `|bearing − camera_yaw| > 0.35 rad`. The teleport's vanish length depends
   on it.
4. **Conditional phases**: extend `MoveSequence` with
   `bool (*until)(const Battle *, const Fighter *)` so a phase can end on
   "off camera / wall / distance", "landed", "apex", or a counter, plus a
   `clip` per segment (moves switch 312 → 301 → 304) and an optional
   `blend` flag for half-step in-between poses.
5. **Child tasks**: `typedef struct Task { FighterFn think; Fighter *owner;
   int counter; int selector; } Task;` in `Battle` (fire hit task, smoke
   effect), with same-tick first dispatch after the spawning task.
6. **Attack records** from table `0x53d178`: `typedef struct AttackRecord {
   int32_t rect[4]; uint8_t reaction, air_reaction, block_reaction, flags;
   int32_t damage; } AttackRecord;` and contact statuses 1/2/3 with chip
   (20%) and unblockable semantics.
7. **Special state**: per-fighter teleport timestamp (69-tick gate), opponent
   hits-taken counter (`< 3` gate), flags `0x40` (teleporting) and `4`
   (in reaction), facing flip and re-face helpers, block snap (`+0.5`).
8. **Victim reactions** 90 and 173 (not specified here) and four-button
   input with D,B / D,F tokens and windows 24 / 20.

## 5. Unresolved

1. Camera: the oracle creates the fight camera task by hand and warms it up;
   real round-start camera pose (`0x431250`, table `[0x53c36c]`) and camera
   mode selection are not reproduced. Reappear tick R depends on them.
2. Fighter groups are not in the `0x4051e0` lists in the oracle; their
   integration by `0x407560` is assumed equivalent (**HYPOTHESIS**). The
   camera object is in list `0x53c25c` (verified).
3. Landing uses skeleton root `+0x34` (seeded 0) in `0x490c60`.
4. Contact geometry `0x48d750`, block test `0x48e890/0x48e800`, damage
   `0x48de80`, reactions `0x484bc0` (90, ground script `0x4e9938` starts with
   unresearched op `0x62`) and `0x483c50` (173): not executed.
5. Fire child liveness word `[0x5302b0]/[0x5302a8]`: zero in the oracle;
   indexed writes cannot be excluded statically.
6. Render effects: flame objects (`0x416ae0/0x416d40/0x416db0/0x416cb0`),
   smoke task `0x4163f0` (geometry `0x4d2640` bind fails; with a fake bind it
   fails later at `0x408d30`), tag lookups `0x408ad0` find nothing because
   the fake skeleton has no hierarchy under group `+0x18`.
7. Flag `0x40` is still set when the miss path returns to `0x46f650`; who
   clears it (and whether it hides the model) is unresolved.
8. Runner expiry branch (`0x4e87f4` → `0x47fd90`) not exercised.
9. Group `+0x40 & 0x200` exemption in the airborne command check.
10. Command window time base (which history timestamp is compared) and
    the P2 mirror (oracle runs Scorpion as P1 only).
