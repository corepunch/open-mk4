# Scorpion airborne behavior: jumps, jump attacks, air throw

Historical research notes retained for their move tables and source addresses.
The experimental retail-emulation recorders and JSON traces have been removed.
References below to an oracle, fixture, scenario or x86 observation describe
the discarded research, not a current test or an independently verified claim.
Use the decoded scripts in `build/retail-disassembly/` as the primary source;
check native helpers there or in `reference/mk4decomp/`. Engine proposals and
exact emulator timing in these notes are not implementation requirements.
Our port preserves script behavior and balance with our own C engine.

## 1. Units and conversion to the runtime

| Fact | Evidence |
| --- | --- |
| Group position +0x54/+0x58/+0x5c (X, Y, Z), velocity +0x6c/+0x70/+0x74, Q16 per step. | 0x407560 adds +0x6c.. to +0x54.. (bytes); oracle. |
| Group angles +0x60/+0x64/+0x68 and angular rates +0x78/+0x7c/+0x80, Q16 radians, wrapped to `[0, 0x6487e)` (2π). | 0x407560 → 0x407510 wraps with `0x6487e` (bytes). |
| Per-step vertical acceleration ("gravity") is group +0x4c; ground Y is group +0x48. | 0x490c60 (bytes). |
| Ground Y = the rig root's Y translation. Scorpion: `0xfffed3b7` (−1.17299). | 0x4092a0 moves root node +0x34 into group +0x58 and zeroes it; 0x48c451 copies +0x58 into +0x48 when the group is built (bytes). Node +0x30..+0x38 hold the rig translation (0x406e31, see combat-core.md), so the value is the rig record Y at `0x511d7c + 16`. |
| Render path: the accumulated Q16 group/node translation is shifted `>> 8` into the Q8 vertex space (`sar edx, 8` at 0x4b9a78/0x4b9a87/0x4b9a96) and added to Q8 mesh vertices. | bytes. |
| Round-start X: P1 −1.3 (`0xfffeb334`), P2 +1.3; Z = 0. | 0x422ea0, 0x422f50 (bytes). |
| Arena: circular, radius² in `0x534540` = radius(`0x4dad60[stage]`)²; most stages 9.5, others 8.5/10/7. Leaving it restores the last inside position and zeroes vx/vz. | 0x423690, 0x489c70 (bytes); x86 (with radius unseeded the clamp fired at x = 0). |

**Conversion (verified by construction):** 1 retail unit = 1 mesh unit, the
unit that `common/mk4_assets.c` produces (Q8 vertex / 256, Q16 rig / 65536).
Our renderer keeps the rig root translation inside the bone pose and places
the soles at `height = 0`; retail moves the root Y into the group, so

```
runtime_x      = retail_x / 65536.0
runtime_height = (ground - (group_y + root_offset)) / 65536.0   /* Y up, soles 0 */
root_offset    = node(+0x18)+0x34   (0 except during flips, see §5.3)
```

Per step at 60 steps/s (HYPOTHESIS: the retail tick rate; the oracle measures
steps only): straight jump vy₀ 0.18999 u/step (11.40 u/s), gravity
0.009995 u/step² (35.98 u/s²); flips gravity 0.008987 u/step², vx 0.05499
u/step. The prototype `mk4_motion` (6.8 u/s, 15 u/s², ±2.0 start, ±5 walls) is
not retail. Recommend integer Q16 per-step integration in the exact order of
§2 rather than float seconds; it reproduces every height in the fixture.

## 2. Tick order (what must run every step)

The retail game tick 0x41fd70 runs, in order (bytes): input edges 0x45c560
(button events queued), event dispatch 0x45c7c0 (each event becomes a task),
0x4a1cd0, **task pump** 0x49cb80→0x41f570 (fighter controllers), then
**world pump 0x4891e0**, then 0x4227a0, then **0x4051e0**, which applies the
callback `[0x4d011c]` = **0x407560** (position/angle integration) to every
group list.

0x4891e0 (x86): recomputes the fighters' midpoint, distance `0x53019c`,
headings P1 `0x530238/0x53023c` (unit vector to P2) and P2 `0x530230/0x530234`,
camera side flag `0x534af0`, the arena clamp 0x489c70 (and 0x489e30 when far),
the close-range separation 0x4897e0 when |ΔY| ≤ 0xb333, the `0x532254`
countdown has expired and distance < 0x8000,
then **0x490c20 → 0x490c60 for P1 then P2**:

```
if (group.cb44) cb44();                      /* +0x44 physics callback */
if (group.gravity) {
  group.vy += group.gravity;                 /* +0x70 += +0x4c */
  if (group.vy >= 0 && y + root_offset + vy >= ground) {
    zero +0x6c..+0x80 and +0x4c; y = ground; root_offset = 0;
    controller(+0x4c) landing callback via 0x490da0 (one-shot, cleared)
  }
}
```

then 0x407560 adds velocities to positions. So gravity is applied before the
position update, and the landing test uses the *next* position. Closed form
for a jump with initial vy₀ < 0 and gravity g, until landing:
`h(step) = (step+1)·|vy₀| − g·(step+1)(step+2)/2` (asserted by the oracle for
`jump_up`).

## 3. Input path (how a jump and an air attack are triggered)

**Joystick word** `0x4d00a4` (active low, bytes 0x4905f0, event table 0x4e4268):
P1 bits 1 up, 2 down, 4 left, 8 right, 0x10 HP, 0x20 block, 0x40 HK;
P2 = same << 8. `0x4d00a8`: P1 LP 1, LK 2, run 4 (P2 0x10/0x20/0x40).

**Stance loop** 0x45fa60 (human; reached from the controller 0x46f650 after a
5-step pose blend). Each step: block held → 0x460200; else direction nibble
from 0x460d20 (`~0x4d00a4`, P2 `>> 8`): 9 → 0x460c40, 5 → 0x460c70,
1 → 0x460c00, 8/4 → walk, 2 → crouch, else idle 0x4608b0 (bytes 0x45fad6..).

- 0x460c00 (up): reset continuation stack, push return **0x46f650**,
  0x4711f0 with index 2.
- 0x460c40 (up+right) / 0x460c70 (up+left): 0x48f030 sets bit 0 when the
  joystick direction points at the opponent on screen (`my_x ≥ opp_x`,
  swapped by camera flag `0x534af0`). Toward → index 3, away → index 4; both
  push return **0x470420**.
- 0x4711f0: zero velocities (0x4906e0), face-reset 0x45f870, dispatch
  `0x4e7020[index]`: 2 → **0x46f7e0** straight, 3 → **0x46fdf0** toward flip,
  4 → **0x46fe80** away flip (bytes; data words at 0x4e7020).

**Buttons.** A press queues an event whose task body (e.g. HP 0x45ca90) writes
the press latch (HP `0x53c1e4`, LP `0x53c204`, BLK `0x53c174`, LK `0x53c1fc`,
run `0x524e78`; HK none), scans the character's command list for that button
(0x45dc70 → 0x45dd30/0x45de00), then 0x45da20 with `0x53c430` = button index
(HP 0, LP 1, BLK 2, HK 3, LK 4, run 5). If a command matched, it is dispatched
through 0x45f5f0. Otherwise the **per-player button table** whose pointer is in
`0x530228` (P1; P2 `0x534640`) is indexed by the button; a non-zero entry is
installed as the controller's callback through 0x45db10/0x41fa50 (fighter
context, 0x45dc00 face check, 0x48ef30: grounded fighters get 0x4906e0), with
a 1-step sleep. Moves install their table with 0x48f670 (human only).
Command-record flag bit 0x10 ("allowed while Y ≠ ground") is tested at
0x45dfb4–0x45dff6 (bytes).

Tables (data words; entries 0x4712xx/0x46c1xx set `0x53c43c` and jump to the
dispatcher 0x45f5f0, flag byte 8 → table 0x4e7008):

| Table | When installed | HP | LP | BLK | HK | LK | run |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0x4e5058 | stance (ground) | ground moves (other researchers) | | | | | |
| **0x4e6ec8** | straight jump, step 0 (0x46f9eb) | 0x46e6c0 (0x603) | 0x46ee30 (0x604) | – | 0x46c7a0 (0x600) | 0x46c7a0 (0x600) | – |
| **0x4e70d8** | flip, step 2 (0x46ff71) | 0x46e6c0 (0x603) | 0x46ee30 (0x604) | – | 0x46e380 (0x602) | 0x46df50 (0x601) | – |
| 0x53d370 | flip steps 0–1, every air attack, flip touchdown | – | – | – | – | – | – |
| **0x4e6ee0** | jump HP after a high contact (0x46e8b2) | – | 0x46ee30 (0x604) | – | 0x46c7a0 (0x600) | 0x46c7a0 (0x600) | – |
| 0x4e70c0 | straight kick after a low contact (0x46cbe2) | 0x46c3f0 | 0x46c6f0 | – | 0x46c680 | 0x46c740 | – |

Block has no air entry: the air throw is a **command record** (§8).

Press timing (x86): the button event runs before the pump, so the table
present at the end of the previous step decides. Straight jump: attacks from
step 1 (`jump_up_hk_step1`). Flip: steps 0–2 hit 0x53d370 (nothing happens:
`jump_toward_hk_step1`, `jump_toward_hp_step2`); first attack at step 3
(`jump_toward_hp_step3`). A press on step 0 itself would use the stance
table (not traced; the oracle requires the jump to start at T0).

## 4. Jump initiation

| Routine | Action (+0x74) | Clip | vx | vy₀ | gravity | +0x44 (human) |
| --- | --- | --- | --- | --- | --- | --- |
| 0x46f7e0 phase 0 (0x46f964) | 0x2007 | 304 (slot 0x53ce64), 28 samples | 0 | `0xffffcf5d` (block 0x4e6900) | `0x28f` | none |
| 0x46fdf0 | 0x2009 | 301 (slot 0x53ce6c / 0x4fbb98), 9 samples | +`0xe14` toward | `0xffffcf5d` (0x4e6918) | `0x24d` | 0x4706f0 (0x4e6910) |
| 0x46fe80 | 0x200a | 301 | −`0xe14` | `0xffffd1ec` (0x4e6928) | `0x24d` | 0x470650 (0x4e6924) |

vx is a speed along the heading vector `[task+0x40]/[task+0x44]` (0x490390 →
0x491190), i.e. toward or away from the opponent in X/Z. All three start with
sound id 4 through 0x489ff0 (Scorpion: sample **1075**). Flips also set
task+0x5c = 1, the whoosh countdown run by 0x4707e0: sound 0x58 (**165**) at
steps 0, 12, 24, 36 (x86).

**Straight → angled (delayed direction).** 0x46f7e0 phases 1–3 (steps 1–3)
test left (0x48ed80) and right (0x48ed10). On the first hit, 0x46fbc0/0x46fb80
pick ±0xe14 by 0x48f030 and 0x46fc00 sets vx, action 0x2009/0x200a,
gravity 0x24d (group+0x4c), installs **0x4707e0** in +0x44 (0x4e6908) and
enters the flip loop 0x46ff20 with clip 301 sample 0. **vy is kept** (no new
impulse), and the spin rate is `+0x4ccc` for both directions (x86:
`jump_up_then_away_2` spins forward, unlike the direct away flip). Steps 4+ ignore direction (`jump_up_then_toward_4` = `jump_up`).
x86: toward at step 1 / 3, away at step 2.

Return: straight/delayed jumps return to **0x46f650** (pushed by 0x460c00);
flips from the stance to **0x470420** (pushed by 0x460ca0/0x460ce0). Both then
blend to the stance clip **290** sample 0 over 5 steps (0x407d00 weights
0x3333, 0x6666, … 0xffff) before the stance loop resumes.

## 5. Physics and playback per jump (x86)

### 5.1 Straight jump `jump_up` (end 48)

| Phase | Steps | Display | Notes |
| --- | --- | --- | --- |
| take-off | 0–2 | 304: 0, 1, 2 | 0x46f7e0 phases 0–2, one sample per step (0x429970) |
| rise | 3–18 | 304: 2, 3 … 17 | 0x428d70 plays to sample 0x11 |
| fall | 19–37 | hold 17 | 0x48f390 waits for gravity = 0 (no pose submissions) |
| land | 37 | – | world pump lands (h 222 → 0) |
| landing | 38–47 | 304: 18 … 27 | 0x46fb00 → 0x46fd90: action 0x2008 then 0x48e080 clears it to 0; sound from list 0x4ea8b0 (450; 445 on a later landing) |
| return | 48 | blend → 290 | 0x46f650 |

Segments: `{0,0,1,1} {3,2,1,1} {19,17,0,1} {38,18,1,1}` (clip 304),
`count = 48`. Height per step (Q16, step 0..37): 11796, 22937, 33423, 43254,
52430, 60951, 68817, 76028, 82584, 88485, 93731, 98322, 102258, 105539,
108165, 110136, 111452, 112113, **112119** (step 18), 111470, … , 12006, 222, 0.
Peak 1.711 u.

### 5.2 Flips `jump_toward` (end 43) / `jump_away` (end 40)

| Phase | Toward steps | Away steps | Display | Physics |
| --- | --- | --- | --- | --- |
| tuck | 0–5 | 0–5 | 301: 0 … 5 | ballistic |
| spin start | 6–8 | 6–8 | 301: 6, 7, 8 | 0x470080: root offset +0x3333 (0x4702e0: node+0x34 += d, Y −= d), pitch rate ±`0x4ccc` (`0x53c448`, 0x46ff4a) |
| spin | 9–35 | 9–32 | hold 8 | 0x48f490 waits for vy ≥ 0, then 0x48f410 waits for Y ≥ `0xfffdb334` (absolute −2.3) |
| unfold | 36–40 | 33–38 | 301: 7 → 3 (−1/step) | 0x470170: 0x490720/0x490de0 zero angles and rates (snap pitch to 0), 0x490bd0 re-faces the opponent |
| touchdown | 41–42 | 39 | 301: 2, 1 | lands step 41 / 39; 0x48a2a0 landing sound when the counter ends |
| return | 43 | 40 | blend → 290 | 0x470420 → stance |

Pitch (group +0x60) per step: toward `+0x4ccc·(step−5)` from step 6,
away starts at `0x5fbb2` (2π − 0x4ccc) and decreases by 0x4ccc; wrapped to 2π;
total ≈ 1.43 turns before the snap at step 36/33. Peak: toward 1.914 u
(125412, step 20), away 1.712 u (112230, step 19). X travel from the
start −1.3: toward +2.255 u (lands at x 62568), away −2.145 u (−225752). Toward segments
`{0,301,0,1,1} {9,301,8,0,1} {36,301,7,-1,1} {43,301,1,0,1}`.

### 5.3 Rotation pivot (HYPOTHESIS for the render composition)

During the spin, root node +0x34 = +0.2 (down) and group Y is 0.2 higher, so
the effective root is unchanged and the group origin is 0.2 u above the
pelvis. The group angles presumably rotate the hierarchy about the group
origin before the root node's local translation; which world axis +0x60
denotes after yaw +0x64 is not verified (needs 0x4bccb0). The runtime should
rotate the posed body about the lateral axis through a point 0.2 u above the
root bone, at 0.3 rad/step, and snap to upright at the unfold step.

### 5.4 Delayed direction (x86)

`jump_up_then_toward_1`: step 0 304:0, steps 1–9 301:0..8, hold to 35,
unfold 36–40, lands 41, return 43 (to 0x46f650). Spin starts step 7. Peak
1.892 u. `…toward_3`: 304:0,1,2 then 301 from step 3; peak 1.855; lands 40.
`…away_2`: 301 from step 2, vx −0xe14; lands 40.

### 5.5 Landing recovery and its cancels

The straight landing 0x4288a0 (cursor 18) → 0x4288d0 plays 304 to 27 and,
each step, dispatches the direction table **0x4dd920** (bytes: 1 → 0x460c00,
2 → 0x45fc90, 4 → 0x460710, 5 → 0x460c70, 8 → 0x460570, 9 → 0x460c40). The
button table during the landing is the stance table 0x4e5058.

- `jump_up_held` (x86): up held through the landing: step 38 submits 304:18
  and, in the same step, a new jump (304:0, sound 1075). The landing is
  cancelled on its first step.
- `jump_up_land_crouch`: down from step 30: step 38 action 0x2010, 259:0..4
  (crouch entry), 0x2001 at 43, held 259:4; on release 259:4..0 (0x2004),
  return 0x470420 at 75.
- `jump_up_lp_during_landing`: LP at step 40 → ground jab (0x10b, clip 294).
- Flip touchdown has no landing clip; the air table 0x4e70d8 stays installed
  on the first touchdown step (step 41) and is replaced by 0x53d370 then the
  stance table (HYPOTHESIS: a press on step 41 would start an air attack on the
  ground; not traced).

## 6. Physics callbacks at group +0x44

| Callback | Installed by | Human behavior | CPU-only behavior |
| --- | --- | --- | --- |
| none | straight jump | – | 0x46fcb0 installs **0x46fcd0** if `0x534780 > 1` |
| 0x4706f0 | toward flip | → 0x4707e0 | if `0x534780 > 1` and height ≥ 0x8000: with 0x48ee60 bit set, 0x470860 = 0, distance ≤ 0x20000 and 0x48f8f0 result ≥ 0 → 0x46fd10; with it clear, vy ≥ 0 and distance ≤ 0x20000 → 0x46fd10 |
| 0x470650 | away flip | → 0x4707e0 | if `0x534780 > 1`, height ≥ 0x8000, 0x48ee60 bit set, 0x470860 = 0 and distance ≤ 0x10000 → 0x46fd10 |
| 0x4707e0 | delayed flips; tail of the above | whoosh: task+0x5c counts down, sound 0x58 every 12 steps | same |
| 0x46fcd0 | CPU straight jump | – | height ≥ 0xcccc, 0x470860 clear, distance ≤ 0x18000 → 0x46fd10 |

0x46fd10 (CPU): distance < 0xcccc → `0x4e6ef8[0x11]` = 0x46df50 (angled LK);
else 0x470960, then ΔY to the opponent > 0x6666 → 0x11, else 0x10 = 0x46e380
(angled HK), dispatched by 0x4912f0 (bytes). "Human" = bit 4 of `0x53c44c`
clear after 0x48f2f0 (`0x5347c8`/`0x532248` non-zero). These CPU paths are
not traced.

## 7. Jump attacks

Common entry (bytes): 0x46ea40 saves the previous action (task+0x70 =
task+0x74), sets the action, 0x490330 clears group+0x40 bit 0, 0x4704a0 zeroes
angular state (0x490720), resets yaw (0x490e30), **re-faces the opponent**
(0x490bd0) and removes the spin pivot (0x4702b0); then a VM script sets the
attack selector and sound **145** and tails into native extend/hold/retract
runners. Velocity and gravity are untouched: the jump arc continues
(x86: identical heights with and without the attack until contact). A flip's
pitch and offset are cancelled at the attack step (`jump_toward_hk_mid`:
pitch 117960 → 0, off 0x3333 → 0, h continuous). +0x44 is cleared.
The table becomes 0x53d370, so a second press does nothing
(`jump_up_hp_twice`). Armed (task+0x30 ≠ 0) HP/LP branch to 0x46ea60 /
0x46ebc0 (script 0x4e66d0; not traced).

| Button (table) | Entry | Action | Script → native | Clip (count) | Extend | Hold (contact) | Retract | Contact routine / selector | Record |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| HP (both) | 0x46e6c0 | 0x603 | 0x4e6658 → 0x46e730, 0x470980 block 0x4e6670, hold 0x4704d0 block 0x4e6680 | 97 (9) | 0..5, 1/step (p..p+5) | 5 for 10 steps (p+6..p+15) | 8 → 0 (p+16..p+24) | 0x48d3d0 / 4 | `0x53d268[4]` = 0x4ebc30 |
| LP (both) | 0x46ee30 | 0x604 | 0x4e6710 → 0x46eea0, 0x4e6728, hold 0x4e6738 | 96 (11) | 0..9 (p..p+9) | 9 for 10 (p+10..p+19) | 10 → 0 (p+20..p+30) | 0x48d3d0 / 5 | 0x4ebc48 |
| HK/LK straight | 0x46c7a0 | 0x600 | 0x4e61f8 (op 0x3c, selector 0x28, sound 0x1f) → 0x46c7f0, hold 0x4e6258 | 327 (8) | 0..7 (p..p+7) | 7 for 4 (p+8..p+11) | 7, 6 → 0 (p+12..p+19) | 0x48d3f0 / 0x28 | `0x53d2b8[0x28]` = 0x4ebc00 |
| HK angled | 0x46e380 | 0x602 | 0x4e6500 (selector 0x1e) → 0x46e3d0, 0x4e6558, hold 0x4e6568 | 55 (6) | 0..5 (p..p+5) | 5 for 10 (p+6..p+15) | 5, 4 → 0 (p+16..p+21) | 0x48d3f0 / 0x1e | 0x4eba38 |
| LK angled | 0x46df50 | 0x601 | 0x4e63d8 (selector 0x1b) → 0x46df90, hold 0x4e6438 | 58 (10) | 0..9 (p..p+9) | 9 for 10 (p+10..p+19) | 9, 8 → 0 (p+20..p+29) | 0x48d3f0 / 0x1b | 0x4eba80 |

p = press step; the attack's first sample is shown on step p. After the
retract, sample 0 is held until landing. Hold-block words (bytes):
`[hold steps, selector, contact routine, on_contact, on_land, on_timeout, …]`,
e.g. HP `0xa, 4, 0x48d3d0, 0x46e7c0, 0x46e240, 0x46e790, 0x46e300`, straight
kick `4, 0x28, 0x48d3f0, 0x46cb10, 0x46c8f0, 0x46c810`. The timeouts run
op 0x09 (cursor = last sample) and 0x46c820 (backwards to 0).

Contact records (bytes, Q16, see `docs/scorpion/combat-core.md` for the
meaning of each word): HP `rect (67502, −39976, 56360, 43253)`, reactions
hit 38 / air 10 / block 143, damage 5898; LP `(76677, −20316, 68157, 62914)`,
39/39/4, 7864; straight kick `(67502, −18350, 58982, 43253)`, 21/10/8, 8519;
angled LK `(75366, −2621, 67502, 34078)`, 22/22/4, flags 2, 9830; angled HK
`(74055, −7864, 60293, 24903)`, 23/23/4, flags 2, 6553.

**Early / mid / late** (x86; presses at steps 4, 12, 26; the jump lands at 37
straight, 41 toward):

- Straight HP `{0,304,0,1,1} {3,304,2,0,1} {4,97,0,1,1} {10,97,5,0,1}
  {20,97,8,-1,1} {29,97,0,0,1} {38,304,18,1,1}` (early); mid shifts by 8;
  late `{26,97,0,1,1} {32,97,5,0,1} {38,304,18,1,1}`: landing at 37 ends the
  hold (queries 32–37) and the normal landing follows.
- Late LP: queries 36–37 only; late straight kick: 34–37.
- `jump_up_hp_landing` (p = 34): 97:0..3 then landing at 38, no query.
  `jump_up_hk_landing`: 327:0..7, one query at 42, landing clip at 43 (the
  kick finishes its extend on the ground, end 53).
- Toward flips: identical attack timing; the landing is 304:18..27 at 42–51
  (not the flip unfold), return 52 via 0x46f650.
- Away flips: same, landing 40.
- On landing during any attack phase the on_land path 0x46e240 →
  0x46fd90 plays the straight landing.

### 7.1 Contact branches (x86, injected result)

The on_contact runs on the step of the first non-miss query. The attacker's
path does **not** depend on block/hit/air-hit (fixtures identical), except the
gate 0x46ea10: block (`0x53c448 == 1`) when the previous action was 0x20e
jumps to 0x47d920 (action 0x1010). 0x20e is written at 0x47c076, not by a
Scorpion routine found here.

| Attack | High (height > 0xcccc above ground) | Low (≤ 0xcccc) |
| --- | --- | --- |
| HP 0x46e7c0 | action 0x1006; zero v and gravity (0x4906e0); install **0x4e6ee0**; sleep 12 (freeze, pose held); then gravity 0x28f, wait landing, 0x46e240 landing | 0x4906c0 snaps Y to ground; hold pose; landing clip after 13 steps |
| LP 0x46ef10 | freeze 12 (script 0x4e6770), then gravity 0x28f | x86 `jump_up_lp_low_hit` (h 12006): same freeze 12 in the air, then fall |
| straight kick 0x46cb10 | script 0x4e6280: freeze 12, then gravity 0x28f | snap to ground (0x4906c0), 0x4900e0, install **0x4e70c0** (ground follow-ups), sleep 12 |
| angled HK 0x46e6b0 | script 0x4e6630: zero v (op 0x5b), wait, op 0x61; freeze 8; then gravity **0x51e** | (`jump_toward_hk_low_hit` at h 55962 is still "high") |
| angled LK 0x46e230 | script 0x4e64e0: freeze 8, then gravity 0x51e | not traced |

Measured (contact at the first hold step, p = 12): HP freeze steps 18–29,
fall from rest, lands 48, landing 49–58, end 59; LP 22–33, lands 51, end 62;
straight kick 20–31, lands 49, end 60; toward HK 18–25, g 0x51e, lands 39
(x stops: vx = 0), end 50; toward LK 22–29, lands 43, end 54. Peak is lower
when the freeze happens before the apex (toward HK 1.883).

### 7.2 Air combos and follow-ups (x86)

- HP high contact installs 0x4e6ee0: **LP** during the freeze → 0x604 at
  step 20 (96:0..9, hold 9 with queries 30–39, 10 → 0), body still frozen
  until step 50, then falls with gravity 0x51e, lands 63, end 74
  (`jump_up_hp_hit_then_lp`). **HK** → 0x600 at 20 (327), frozen until 39,
  g 0x51e, lands 52, end 63. HP is ignored (`…then_hp` = `jump_up_hp_hit`).
  After the LP follow-up the table is empty: HK at 34 is ignored.
- LP, straight kick, angled kick contacts install no air table: follow-ups
  are ignored (`jump_up_lp_hit_then_hk`, `jump_up_hk_hit_then_lp`,
  `jump_toward_hk_hit_then_lp`, `jump_toward_lk_hit_then_hk`).
- Low straight-kick contact → 0x4e70c0; **HP** 3 steps later starts ground
  move 0x46c3f0 (action 0x10a, clip 296) on step 37 (`jump_up_hk_low_hit_then_hp`).
  These ground follow-ups belong to the ground-combo research.

## 8. Air throw (block in the air)

**Trigger** (bytes + x86): BLOCK command list (`0x53c960` → 0x4e49f8 for
Scorpion; second record 0x4e4a04, after fatality 1), record words `ffffffff ffffffff 06110022`: unconditional pattern,
window 0, flags 0x11 (0x01 → table 0x4e6ef8, 0x10 → allowed airborne),
action 0x22 → `0x4e6ef8[0x22]` = **0x46aa80**, predicate index 6 →
`0x4e4ec0[6]` = **0x45ed70**. Also required: group+0x40 bits 0 and 2 clear
(0x45e001). Predicate 0x45ed70: both P1 and P2 pass 0x45eef0
(absolute **Y ≤ `0xfffdcccd`**, i.e. −2.2; for Scorpion ≥ 1.027 u above ground)
and squared 3-D distance between the group origins ≤ `0x20000`
(distance ≤ 1.414 u). The record is matched before the button table, so the
block press is consumed by the throw; when it fails the air table has no
block entry and nothing happens.

x86 (P2 Sub-Zero is a live human controller 1.0 u in front and jumps on the
same step): block at step 10 → throw; at step 3 (h 0.66) → nothing; at
standard 2.6 u separation → nothing; victim on the ground → nothing; at
step 28 while descending (y still ≤ −2.2) → throw.

**Attacker** (0x46aa80, x86 `air_throw`):

| Steps | Display | Physics / events |
| --- | --- | --- |
| 10 | 82:0 | action 0x502; 0x480fc0 (index 8) takes the victim (below); 0x48f9f0; `0x532254` = 0x40; script 0x4e5e28: op 0x4f camera shot 23, op 0x25 (0xd) attach, op 0x0e victim clip 202, op 0x0f own clip 82, op 0x4b (0x1c) both play; sound **2100**; zero v, gravity 0 (frozen in the air) |
| 10–38 | 82: 0 … 28 | one sample per step; victim 202 in lockstep (0..27) |
| 38 | – | 0x46aae0: sound 0x59 via 0x480f50; **damage `0x2147`** to the victim (0x48de80 adds `0x2147 × 0x534834` to victim task+0x80; 8519 recorded) |
| 39–48 | hold 28 | script 0x4e5e50: advance 1, op 0x58 motion `vx 0x7ae, vy −0x1999, g 0x28f` (along the heading), wait 10 |
| 49–63 | 82: 30 … 44 | finish forwards (sample 29 not displayed) |
| 64–67 | hold 44 | op 0x60 → 0x48f390 wait for landing; lands at 67 (+0.84 u X in total) |
| 68–77 | 304: 18 … 27 | 0x46ab20 → 0x46fd90 landing; facing re-evaluated (bit set at 68: P1 is past the victim) |
| 78 | blend | return 0x46f650 |

**Victim program** (x86; Sub-Zero; P2 task redirected through
0x48f880/0x41fa50 to `0x4ea7f0[8]` = 0x487880, runs 0x4878c0):

| Steps | Victim display | State |
| --- | --- | --- |
| 10 | 202:0 | X snapped to the attacker's X (−85196), Y to the attacker's Y, facing bit cleared, velocities and gravity 0 |
| 10–37 | 202: 0 … 27 | follows the attacker's playback |
| 38–42 | hold 27 | released: vy₀ = +`0x3333` (down), gravity 0x28f; damage applied at 38 |
| 43 | – | lands |
| 44–59 | 202: 29 … 44 | sounds 925 and 195 at 44 |
| 60–63 | hold 44 | VM wait |
| 64– | 131: 46 → 79 | 0x484b60 decodes clip 185 sample 0 (0x408190 → 0x401000) to align the root: group Y raised to h 60833, X +1643, facing bit set |
| 98 | hold 79 | parks (op 0x3e, 0x45b530). Getting up from here is **not** traced |

Late throw (`air_throw_late_descent`, block at 28): same program shifted by
18 steps; attacker peak after release 1.712 u, end 96.

## 9. Facing during jumps (x86)

Facing is group +0x34 bit 0 (1 ⇒ faces −X) plus yaw +0x64 recomputed by
0x491230 from the opponent direction. In the air it is not updated every step.
It is re-evaluated by 0x490bd0 (toggle via 0x490350 when 0x48efc0 reports the
wrong side) at: flip unfold start (`jump_toward_crossup`: P1 lands past a foe
standing 1.2 u ahead; bit set at step 36), air-attack start (`…crossup_hk`:
HK at 20 before crossing; corrected at landing step 42), and landing.

## 10. Air-to-special gate (teleport punch)

The HP command record 0x4e49b8 (`0000aa02 00000000 07141800`: tokens
D, B; window 24; flags 0x14) has flag 0x10, so 0x45de00 skips the
"Y must equal ground" test (0x45dfb4–0x45dff6) and teleport punch
(`0x4e8910[0]` = 0x47fc90, predicate 0x45f0c0) can start while airborne.
Not executed here (the oracle does not drive the direction-history events);
see `docs/scorpion/teleport-and-fire.md`. No other Scorpion command record
has flag 0x10 except the air throw.

## 11. Sounds (sample ids recorded at 0x4bd4f0)

1075 jump (0x489ff0 id 4, Scorpion's voice table); 165 flip whoosh (id 0x58
every 12 steps); 145 jump-attack whoosh (script op 0x20); 450 / 445 landing
(0x48a2a0, random pick from 0x4ea8b0); 2100 air throw; 925 + 195 victim
impact; 2085 Sub-Zero's jump (victim). On-contact sounds are inside the
stubbed contact routines.

## 12. Engine requirements (minimal C additions in `game.h` style)

1. **Q16 ballistic state** on `Fighter`: `int32_t x, y, z, vx, vy, vz,
   gravity, ground, root_offset; int32_t pitch, pitch_rate;` with one
   `fighter_physics_step()` reproducing §2 (callback, `vy += g`, landing test
   on the next position, then integrate), a landing event that clears motion
   and invokes `FighterFn on_land`. Keep `height`/`x` floats as derived views
   for rendering (§1).
2. **Per-character ground** = rig root Y (`roster` asset data), heights
   relative to it. Retail placement ±1.3 and a circular arena clamp (radius
   per stage, 9.5 default) instead of ±5 walls.
3. **Six buttons** (HP, LP, BLK, HK, LK, RUN) and press latches; a
   `const FighterFn *button_table` pointer on `Fighter` (6 entries, NULL =
   ignored) swapped by moves: stance, straight-air (0x4e6ec8), flip-air
   (0x4e70d8), empty, HP-follow-up (0x4e6ee0), ground follow-up (0x4e70c0).
   Command records (with "airborne allowed") are scanned before the table.
4. **Relative directions**: toward/away from 0x48f030 semantics (screen side
   with a camera flag); heading vector to the opponent recomputed each step.
5. **Delayed-direction window**: a straight jump polls left/right on steps 1–3
   and converts in place (new vx and gravity, keep vy).
6. **Flip rotation**: pitch rate ±0.3 rad/step from step 6, pivot 0.2 u above
   the root, snap to 0 at unfold; renderer applies the group pitch.
7. **Contact windows with continuations**: per-step queries (routine, selector
   → record) during the hold phase; `on_contact` that freezes physics N steps
   (12 straight, 8 angled), swaps the button table, then sets gravity
   (0x28f or 0x51e); height test (0xcccc) choosing snap-to-ground; `on_land`
   that aborts the sequence into the landing move.
8. **Sequence abort on landing** and a landing move (304:18..27) whose first
   step can be cancelled by direction/buttons (table 0x4dd920 + stance table).
9. **Victim control** for the throw: attach the victim to the attacker
   (position snap, facing), drive the victim's clip in lockstep, release with
   damage and a victim fall, then a victim get-up program with root
   alignment.
10. **Pose blend** (5 steps) when returning to the stance.
11. Re-facing points (attack start, flip unfold, landing) rather than per-step
    facing updates while airborne.

### 12.1 Expressing the recovered data with the current move runtime

Air phases end on a physics event, not on a step count, so each airborne
`MoveSequence` needs "hold the last segment until `on_land`" (e.g. a large
`count` plus the landing callback). With that, the recovered playback is
small (all relative to the move's own step 0; clip in the `MoveDef`):

```c
/* 304: straight jump (action 0x2007); ends on landing, then jump_land. */
static const MoveSegment jump_up_segments[] = {{0, 0, 1, 1}, {3, 2, 1, 1}, {19, 17, 0, 1}};
/* 304: landing (0x46fd90), 10 steps, then return/blend to stance (5 steps). */
static const MoveSegment jump_land_segments[] = {{0, 18, 1, 1}};
/* 301: flip (0x2009/0x200a); spin from step 6; unfold when Y >= -2.3 abs. */
static const MoveSegment flip_rise_segments[] = {{0, 0, 1, 1}, {9, 8, 0, 1}};
static const MoveSegment flip_unfold_segments[] = {{0, 7, -1, 1}};   /* 7..1, lands on 2 or 3 */
/* Jump attacks (start on the press step; then hold sample 0 until landing). */
static const MoveSegment jump_hp_segments[] = {{0, 0, 1, 1}, {6, 5, 0, 1}, {16, 8, -1, 1}, {25, 0, 0, 1}};   /* 97 */
static const MoveSegment jump_lp_segments[] = {{0, 0, 1, 1}, {10, 9, 0, 1}, {20, 10, -1, 1}, {31, 0, 0, 1}};  /* 96 */
static const MoveSegment jump_kick_segments[] = {{0, 0, 1, 1}, {8, 7, 0, 1}, {13, 6, -1, 1}, {20, 0, 0, 1}};  /* 327 */
static const MoveSegment flip_hk_segments[] = {{0, 0, 1, 1}, {6, 5, 0, 1}, {17, 4, -1, 1}, {22, 0, 0, 1}};    /* 55 */
static const MoveSegment flip_lk_segments[] = {{0, 0, 1, 1}, {10, 9, 0, 1}, {21, 8, -1, 1}, {30, 0, 0, 1}};   /* 58 */
/* Contact events: hp 6..15, lp 10..19, kick 8..11, flip hk 6..15, flip lk 10..19. */
```

Physics constants (Q16 per step): straight `vy0 = -0x30a3, g = 0x28f`;
toward `vx = +0xe14, vy0 = -0x30a3, g = 0x24d`; away `vx = -0xe14,
vy0 = -0x2e14, g = 0x24d`; spin `±0x4ccc` rad/step with pivot `0x3333`;
post-contact fall `g = 0x28f` (straight) or `0x51e` (angled, combos);
throw release `vx = 0x7ae, vy0 = -0x1999, g = 0x28f`; victim release
`vy0 = +0x3333, g = 0x28f`.

## 13. Unresolved

- Render composition of group angles +0x60/+0x64/+0x68 (rotation axis/order,
  pivot) – 0x4bccb0 and the root-node transform not analysed (§5.3).
- Victim reactions and damage for air attacks (contact routines stubbed);
  the 0x47d920 branch (block after previous action 0x20e).
- Victim after the air throw: parked at step 98 (0x45b530); what wakes it
  (get-up / controller return) is not traced. Clip 131/185 roles not named.
- Camera director task (type 0x12, 0x431700) for shot 23; stubbed.
- CPU auto air attacks (0x46fcd0, 0x4706f0, 0x470650 → 0x46fd10, 0x470860,
  0x48f8f0, 0x470960) and `0x534780` meaning.
- Armed jump punches (task+0x30 ≠ 0: 0x46ea60, 0x46ebc0, script 0x4e66d0).
- Button press on the jump's own step 0, and on a flip's first touchdown
  step (air table still installed).
- Ground follow-ups from 0x4e70c0 (0x46c3f0, 0x46c6f0, 0x46c680, 0x46c740)
  beyond their first action.
- Event-task ordering: the oracle runs a press before the pump on its step; in
  retail the event task's list position could delay it by one step.
- Retail wall-clock tick rate (steps are assumed 60 Hz for unit conversion).
- Opcodes 0x5b/0x5c/0x60/0x61 used by contact scripts are executed but not
  added to `tools/scorpion_moves.py`'s decoder.

