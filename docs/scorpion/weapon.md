# Scorpion: sword (draw, armed moveset, throw, knock-out, pickup)

Historical research notes retained for their move tables and source addresses.
The experimental retail-emulation recorders and JSON traces have been removed.
References below to an oracle, fixture, scenario or x86 observation describe
the discarded research, not a current test or an independently verified claim.
Use the decoded scripts in `build/retail-disassembly/` as the primary source;
check native helpers there or in `reference/mk4decomp/`. Engine proposals and
exact emulator timing in these notes are not implementation requirements.
Our port preserves script behavior and balance with our own C engine.

## 0. Summary for the implementer

| Input (Scorpion, P1) | Condition | Native | Action +0x74 | Selector +0x68 | Clip(s) | End step (miss / contact) |
| --- | --- | --- | --- | --- | --- | --- |
| F,F,HK | unarmed, sword never lost this round | `0x46d1d0` → script `0x4e62c0` | `0x1004` | – | 244 | 48 |
| F,F,HK | unarmed, normal-move follow-up window (group +0x40 bit `0x200`) | `0x46d1d0` → `0x494610` | `0x112` | `0x302` | 237 (instant sword + back-LP slash) | 48 / 44 |
| F,F,HK | **armed** | `0x46d1d0` → dispatch `0x2000d` → `0x498270` | `0x30f` | – | 208, sword becomes a projectile | controller 62, end 67 |
| HP | armed, standing | block `0x4ecd08[0]` = `0x496c20` → runner `0x494990(0x4ec880)` | `0x10e` | `0x301` | 235 | 43 / 42 |
| LP | armed, standing, not in throw range | `0x4ecd08[1]` = `0x496c30` → `0x494990(0x4ec898)` | `0x10e` | `0x300` | 236 | 43 / 42 |
| LP | armed, throw range | `0x496c30` → `0x46dbb0` → opponent throw `0x42c560` | – | – | (throw, other area) | – |
| Back+HP | armed | runner alternative `0x496c90`, script `0x4ec918` | `0x112` | `0x303` | 238 | 41 / 37 |
| Back+LP | armed | runner alternative `0x496c50`, script `0x4ec8b0` | `0x112` | `0x302` | 237 | 48 / 44 |
| D+LP (hold LP) | armed with weapon id `0x60` (Scorpion's sword) | crouch block `0x4e5070[1]` = `0x4610d0` → dispatch `0x40019` → `0x480770` | `0x407` | `0x316` | 223, 224 loop | while LP held |
| Jump + HP | armed | jump block HP → `0x46e6c0` (`0x603`) → `0x46ea60` | `0x603` | `0x104` | 252 | lands, controller 48 |
| Jump + LP | armed | jump block LP → `0x46ee30` (`0x604`) → `0x46ebc0` | `0x604` | `0x108` | 251 | lands, controller 48 |
| D+HP, BLK, HK, LK, back+HK, back+LK | armed | unchanged handlers (`0x46ccd0` uppercut etc.; oracle `crouch_hp`) | – | – | unarmed clips; crouch/block/walk clips swap to armed variants | – |
| other specials | armed | predicates do not read task +0x30 (static); behaviour while holding the sword unresolved | – | – | – | – |
| D (crouch) + RUN | unarmed, resting weapon within 1.0 | predicate `0x45efc0` → `0x46a180` → `0x46a020` | `0x2006` | – | 209 | crouch again at 15 |

There is **no put-away**: the draw command while armed throws the sword. Armed
state ends by throwing, by a knock-out reaction, when Scorpion starts
fatality `0x467ce0`/`0x467ec0` (static), or when
the round's world is rebuilt. After losing the sword it can only be recovered by
picking up a resting weapon; the draw command is refused for the rest of the
round (group +0x40 bit `0x2`).

Normal buttons cannot cancel the armed moves: every block-handler press
first runs `0x45dc00` → `0x45f870` → `0x48f6c0`, which installs the all-zero
button block `0x53d370` (oracle `hp_then_*_at_N`: presses of HP, LP, BLK, HK,
LK on steps 5, 18, 30 change nothing; every other armed move also runs with
`0x53d370` installed, see `state` in the fixture). The armed block is
reinstalled only by the end-of-move reset `0x48e080`.

## 1. Data: weapon record, armed flag, button blocks

### Fighter fields

| Field | Meaning | Evidence |
| --- | --- | --- |
| task +0x30 | packed pointer to the held weapon record, 0 = unarmed | written `0x493138` (create), `0x49394c` (pickup), cleared `0x493616` (detach) |
| group +0x40 bit `0x2` | "weapon drawn this round"; set by creation `0x4932d9`, never cleared by the end-of-move mask `0xfffffd17` (`0x48e0e0`) | static + every armed scenario `weapon_after.group_flags` |
| group +0x40 bit `0x200` | normal-move follow-up window, set by `0x4900e0`, cleared by `0x48e080` | static |
| part table `[group +0x1c]` | word 0 = count, word *n* = part *n*; slot 1 holds the sword, part 10 is the hand it is linked under | `0x408cb0`, `0x408580`, oracle `hand_slot` |
| `[0x530228]` (P1) / `[0x534640]` (P2) | installed 6-word button block (HP, LP, BLK, HK, LK, RUN) | `0x48f670`, `0x45da90` |

### Weapon record (8 words, table `0x53d5c0[character]`, BSS filled by startup fixups)

Scorpion: `0x4ecdf8`.

| Word | Scorpion | Use |
| --- | --- | --- |
| 0 | `0x60` | weapon id; world objects keep it at +0x30 |
| 1 | `0x506d04` | 7-word object template (`0x406dd0`): part slot 1 (`0x100898 >> 20`), flags `0x8000898`, render descriptor `0x50609c`, bounds `0x504a3c` (−0.106..0.106, −0.252..1.171, −0.020..0.020), local offset (−0.022, −0.083, −0.017) |
| 2 | `0x50b6d4` | hand binding: `[0x50b160 + 4 words]` = offset (−0.030, −0.109, −0.122) and packed rotation `0xca2fafc0` (`0x493690` → `0x4076b0`) |
| 3 | `0x4ecd08` | armed standing button block `{0x496c20, 0x496c30, 0x460200, 0x461030, 0x460fc0, 0}` |
| 4 | clip 286 | armed stance (`0x490f60` via `0x4947a0(4)`) |
| 5 | clip 277 | armed walk toward opponent, speed factor 1.0 (`0x49185d`) |
| 6 | clip 278 | armed walk away, speed factor 0.7 (`0x49187d`) |
| 7 | −0.3 (`0xffffb334`) | **unresolved**: no reader found |

Weapon ids by character (static, `0x53d5c0`): 0 `0x60`, 1 `0x61`, 2 `0x62`, 3 `0x66`,
4 `0x6b`, 5 `0x68`, 6 `0x64`, 7/13 `0x69`, 8 `0x6f`, 9/16 `0x6d`, 10 `0x67`,
11 `0x63`, 12/17 `0x6a`, 14 `0x6e`, 15 (Goro) `0x6c` but `0x493080` refuses to
create a weapon for character 15. Alternative tables `0x53d610`/`0x53d660`
(selected by per-player `0x53c284`/`0x53c288` = 2/3) are kombat-code variants.

### Armed clip substitutions (static, `0x4940e0(slot)` picks word 1 when armed)

| Slot | Unarmed | Armed | User |
| --- | --- | --- | --- |
| `0x53cd20` | 258 | 219 | crouch hold (`0x45fe50`) |
| `0x53cd28` | 256 | 221 | stand up (`0x45ffa0`) |
| `0x53cd30` | 257 | 220 | crouch (`0x460130`) |
| `0x53cd38` | 323 | 246 | `0x4603c0` |
| `0x53cd40` | 258 | 219 | `0x460410` |
| `0x53ce30` | 322 | 245 | block (`0x46b300`) |
| `0x53ce38` | 257 | 220 | crouch block (`0x46b350`) |
| `0x53ce40` | 259 | 217 / 218 | crouch-down (`0x494120`: 218 when the current clip is the armed stance) |
| `0x53ce50` | 258 | 219 | `0x46b610` |
| `0x53cf88` | 255 | 222 | `0x487030` |
| `0x53c6a0` | 258 | 219 | `0x4371a0` |

Oracle `crouch`: down held while armed → 218 samples 0..4, then 219 from step 5;
action `0x2010` then `0x2001`, block `0x4e5070`.

## 2. Draw (F,F,HK, unarmed)

1. **Trigger.** High-kick list record `0x4e4a58`: tokens F,F, window 24, flags
   `0x01`, predicate index 3 → `0x45ef50`, table 1 index `0x29` → `0x46d1d0`
   (command word `0x3011829`). `0x45ef50` is true if the fighter holds a weapon
   (task +0x30 ≠ 0), else false if group +0x40 bit `0x2` is set, else true
   (static; oracle `draw_predicate` = 1 before, 0 after a throw or knock-out,
   1 after a pickup). The matched command enters through `0x45f5f0`, whose
   prologue `0x45f870` installs the zero block.
2. **`0x46d1d0`.** Armed → `0x2000d` (section 7). Otherwise `0x48e3a0(0xe)`
   stores the tick counter in per-player stamp `0x53c2a8 + (0xe×2 + player)×4`;
   if group +0x40 bit `0x200` → `0x494610` (section 2.1); else action `0x1004`,
   script `0x4e62c0`.
3. **Playback** (`draw`): VM `0x2c` voice `0x48a230(0)`; open clip 244 and
   advance 22 (`0x2000016`); native `0x46d260` on step 22 creates the sword
   (`0x493080` → `0x4930a0`) and installs the zero block (`0x48f6c0`); script
   `0x4e62d8`: advance 13, sound `0x12` (step 35), finish forwards, native
   `0x470420` on step 48.
   Segments (clip 244): `{0, 0, +1, 1}`, `{35, 36, +1, 1}` until step 47; step
   48 runs the end-of-move reset and shows stance 286.
4. **Sword creation `0x4930a0`** (step 22): task +0x30 := record; build object
   from template word 1 (`0x406dd0`), register it in part slot 1 (`0x408580`),
   bind offset/rotation from word 2 (`0x493690`), link under part 10
   (`0x408cb0(0xa)`, `0x409970`), group +0x40 |= 2. If `[0x524f00] & 0x20`
   it first calls `0x493350` → `0x4ab570(0xe)` (kombat-code path, not traced).
5. **End** `0x470420` → `0x48e080` installs block `[record+3]` = `0x4ecd08`;
   `0x45fa30` sets stance `[record+4]` = 286; idle loop `0x45fa60`.
6. Inputs during the draw: none (zero block). No contact.

### 2.1 Draw inside a follow-up window (`draw_in_window_miss/_hit`)

With group +0x40 bit `0x200` (set by `0x4900e0`, static callers `0x46bb60`,
`0x46bf10` (high-punch continuation), `0x46c2c0`, `0x46cba0`, `0x46e7c0`
(air-punch post-contact); the same routines install the follow-up button blocks
`0x4e70a8`, `0x4e7078`, `0x4e7090`, `0x4e70c0`), `0x494610` creates the sword on step 0 (`0x493080`), runs
`0x4946f0` (opponent group +0x40 bit 8 check, `0x48ee60`, possibly
`0x480f80(0x78)` and `0x48fcd0(0x4ec418)`; side effects unresolved) and jumps
through `0x4ecb40[id − 0x60]` = `0x496c50`: the back+LP slash of section 4 with
identical timing (miss end 48, hit end 44). The oracle sets the bit with the
original `0x4900e0`; which normal-move paths reach it is in the normals spec.

## 3. Standing HP / LP slashes (runner `0x494990`)

**Trigger.** HP press (`0x45ca90`) or LP press (`0x45caf0`) in armed idle →
`0x45da90` → block `0x4ecd08` entry 0/1 → redirect `0x41fa50`. LP first runs
`0x46dbb0` (the normal throw test, static: distance `[0x53019c]` ≤ `0xb333`,
predicates `0x48ee60` and `0x48ebc0` false, `[0x524f00] & 1` clear, own window
bit `0x200` clear, `0x48e6e0` false (opponent action class `0x4000`), opponent
action (`0x48f8b0`) not one of `0x10a, 0x10b, 0x100, 0x102, 0x10e, 0x111,
0x112, 0x113` → throw `0x42c560`; oracle `lp_close_throw` at distance 0.5
reaches `0x42c560`).
Then `0x494990(params)`: task +0x6c := params[4]; if params[3] ≠ 0 and back is
held (`0x48ebd0`: group +0x40 bit 8 clear, P1 bit 4/8 by facing via `0x48f030`)
→ jump to params[3] (section 4).

**Parameter block** (6 words): 0 clip; 1 bytes {post-contact index, active
count, phase-2 end sample, phase-1 end sample}; 2 low byte selector, byte 1
start sample, high half sound id; 3 back-held alternative; 4 → task +0x6c
(**unresolved** reader).

| | HP `0x4ec880` | LP `0x4ec898` |
| --- | --- | --- |
| clip | 235 (43 samples) | 236 (43 samples) |
| word 1 | `0x08100400`: post-contact 0, active 4, 16, 8 | same |
| word 2 | `0x00220001`: selector 1, start 0, sound `0x22` | `0x00220000`: selector 0 |
| back alternative | `0x496c90` (back+HP) | `0x496c50` (back+LP) |

**Phases** (`0x494a00`): phase 0 sets +0x68 = `0x300 + selector`, action
`0x10e` (kept if it was `0x111/0x112/0x113/0x212`), cursor := start, plays to
sample 8; phase 1 (step 9) plays sound `0x22`, cursor+1, plays to 16; phase 2
(steps 17..20) advances one sample per step and queries `0x48d430(selector)`
each step; no contact → `0x494d00` on step 21: action `0x1006`, cursor+1, play
to the end, `0x470420`. Contact (any non-zero result) on step *n* →
`0x494dc0` → action `0x1006` → `0x4ecb80[0]` = `0x494d00` on the same step:
the cursor skips one sample and the move ends one step earlier.

| Scenario | Displayed (clip, sparse `{at, first, step, period}`) | Contact queries | End |
| --- | --- | --- | --- |
| `hp_slash_miss` | 235 `{0,0,+1,1}` | 17,18,19,20 sel 1 | 43 |
| `hp_slash_block/hit/air_hit` | 235 `{0,0,+1,1}`, `{17,18,+1,1}` | 17 | 42 |
| `hp_slash_late_hit` (hit on 20) | 235 `{0,0,+1,1}`, `{20,21,+1,1}` | 17..20 | 42 |
| `lp_slash_*` | as HP with clip 236, selector 0 | | 43 / 42 |

No motion. Miss, block, hit and air-hit take the same attacker branch; only the
victim reaction differs (section 9). Sound `0x22` on step 9.

## 4. Back+HP / back+LP (VM scripts)

Entered from the runner when back is held. Opcodes used besides the
documented ones (static, handlers): `0x5e` contact query `0x48d430(inline
selector)` (`0x45abb0`); `0x42` jump to operand if the contact bit is set
(`0x45b5c0`); `0x5f` horizontal speed along facing from operand (`0x45afe0` →
`0x490390`); `0x5b` stop velocities (`0x45b010` → `0x4906e0`); `0x34` wait.

* Back+LP `0x496c50`: action `0x112`, +0x68 `0x302`, script `0x4ec8b0`: clip
  237 advance 9; sound `0x22` (step 9); speed 0.1 (`0x1999`); advance 8; three
  times {`0x5e` sel 2, `0x42` → `0x4ec974`, advance 1}; `0x5b`; advance 2; wait
  4; action `0x1006`; `0x5b`; finish; `0x470420`. Contact tail `0x4ec974`:
  action `0x1006`, `0x5b`, finish, `0x470420`.
* Back+HP `0x496c90`: action `0x112`, +0x68 `0x303`, script `0x4ec918`: clip
  238 advance 8; sound `0x22` (step 8); advance 8; speed 0.1; advance to sample
  25; three times {`0x5e` sel 3, `0x42` → `0x4ec974`, advance 1}; action
  `0x1006`; `0x5b`; advance 2; sound `0x13`; wait 4; action `0x1006`; `0x5b`;
  finish; `0x470420`.

| Scenario | Displayed | Queries | Motion (x, retail units) | End |
| --- | --- | --- | --- | --- |
| `back_lp_miss` | 237 `{0,0,+1,1}`, `{21,20,0,1}`, `{25,22,+1,1}` | 17,18,19 | +0.1/step on steps 9..18 (total 1.0) | 48 |
| `back_lp_block/hit` | 237 `{0,0,+1,1}`, `{17,18,+1,1}` | 17 | stops at 17 (0.8) | 44 |
| `back_hp_miss` | 238 `{0,0,+1,1}`, `{26,25,+1,1}`, `{30,28,0,1}`, `{34,30,+1,1}` | 26,27,28 | +0.1/step on 16..27 (1.2) | 41 |
| `back_hp_block/hit` | 238 `{0,0,+1,1}` | 26 | stops at 26 (1.0) | 37 |

Velocity is group +0x6c (x) applied by the object integrator `0x407560`.

## 5. Sword spin (D+LP, held)

1. **Trigger.** Crouching (action `0x2001`, block `0x4e5070` installed by
   `0x45fe05`), LP press → `0x4610d0`: armed and record word 0 = `0x60` →
   dispatch `0x40019` (table `0x4e8910[0x19]` = `0x480770`); any other weapon or
   unarmed → crouch punch (`0x4711f0(8)`).
2. **Entry `0x480770`.** +0x68 `0x316`, action `0x407`, script `0x4e8850`: clip
   223 (4 samples) with prefix playback `0x48`, then bind clip 224 and native
   `0x4807b0`: sound `0x1d`, counter 12, loop `0x4807e0`.
3. **Loop, every step.** Counter −1; when it reaches 0 (every 12th step) →
   reset 12, sound `0x1d` and `0x48d430(0x16)`. Ends with recoil `0x480980` if
   the last query result word is 1 (block) or the opponent controller's +0x7c ≥ 3
   (`0x48f8d0`); the result word is `[0x53c448]`, which only a query step
   leaves at a contact value; else horizontal speed ×0.7 (`0x490650(0xb333)`), then steering
   (`0x496d20`): back (`0x48ed80`) → −0.1, forward (`0x48ed10`) → +0.1, times 0.6
   → speed set to ±0.06 (`0x491050`); continue only while P1 button bit 1 of
   `0x4d00a8` (LP, P2 bit `0x10`) is held, else `0x46f650`.
4. **Recoil `0x480980`**: `0x490140`, action `0x100f`, `0x490ab0(0x4e8868)`,
   wait 4, then 8, then `0x46f650`.

| Scenario | Displayed after the LP press on step 10 | Queries / result | End |
| --- | --- | --- | --- |
| `spin_release_16` | 223 `{10,0,+1,1}`; 224 `{14,0,+1,1}` | none | controller 16 |
| `spin_release_40` | 224 cycles 0..11 from steps 14, 26, 38 | 26, 38 miss | controller 40 |
| `spin_hit` (LP held) | 224 cycles every 12 steps | 26 hit, 38.. miss; continues | none in 90 |
| `spin_block` | 224 to 11, holds 11 for 12 steps | 26 block | recoil 26, controller 38, crouch |
| `spin_opponent_down` (+0x7c = 3 on 20) | 224 to 5, holds | none | recoil 20, controller 32 |
| `spin_steer_forward/back` | as release at 34 | 26 | x speed ±0.06 from step 15 |

Sounds `0x1d` on steps 14, 26, 38, … After `0x46f650` the controller blends to
the stance for 5 steps (`0x46f740`: weight += 0.2) and runs `0x470420`; the pose
blend itself (`0x407d50`) is stubbed, so those 5 steps hold the last pose.

## 6. Armed air HP / LP

Up held → vertical jump (`0x2007`, clip 304, block `0x4e6ec8` = `{0x4712c0,
0x4712d0, 0, 0x4712b0, 0x4712b0}`); diagonal jumps install `0x4e70d8`
(`{0x46c1f0, 0x46c200, 0, 0x46c1d0, 0x46c1e0}`). Both route HP to dispatch
`0x80000` → `0x4e7008[0]` = `0x46e6c0` (action `0x603`) and LP to `0x80001` →
`0x46ee30` (`0x604`). Armed (task +0x30 ≠ 0) these jump to `0x46ea60` /
`0x46ebc0`: scripts `0x4e66d0` (+0x68 `0x104`, `0x46eab0`: swoosh `0x48a360`,
param block `0x4e66e0` = clip 252, 6, `0x46eae0`) and `0x4e66f0` (+0x68 `0x108`,
`0x4e6700` = clip 251, 7, `0x46ec40`, `0x46e300`) for the shared air runner
`0x470980`.

| Scenario (press on step 10) | Displayed | Queries | Landing | End |
| --- | --- | --- | --- | --- |
| `jump_hp` | 304 `{0,0,+1,1}`, `{3,2,+1,1}`; 252 `{10,0,+1,1}`, `{20,9,0,1}`; 304 `{38,18,+1,1}`, `{48,27,0,1}` | 18,19,20 `0x48d3d0` sel 7 | step 38 | controller 48, end 53 |
| `jump_lp` | same with clip 251 | 19,20 sel 8 | 38 | 48 / 53 |

Jump: vy −0.18 per step at take-off, gravity +0x4c = `0x28f` (0.01) per step,
apex −1.71 on step 18 (Y down). Unarmed air punches use other scripts
(`0x4e6658`, `0x4e6710`) and are not this spec.

## 7. Throwing the sword (F,F,HK while armed)

`0x46d1d0` with task +0x30 ≠ 0 → `0x53c43c` = `0x2000d` → `0x45f5f0` → table 2
(`0x4ed828`) index `0xd` = `0x498270`: sound `0x4d`, `0x498290` phases (static
+ oracle `throw_weapon`):

| Phase | Step | Behaviour |
| --- | --- | --- |
| 0 | 0 | `0x49c610` stores facing vector (`[0x530238]`, `[0x53023c]`) in task +0x68/+0x6c; action `0x30f`; clip `[0x53d6b0]` = 208 sample 0; play to 10 |
| 1 | 11 | play to 20 |
| 2 | 22 | unless `[0x524f00] & 0x400`: `0x498480` detaches the sword (below), spawns task type `0x4d`, callback `0x498550`, with the object in R414 and object +0x28 = thrower group; play to the clip end |
| 3 | 62 | `0x46f650` (5-step blend, `0x470420` on 67; unarmed stance 290, block `0x4e5058`) |

Displayed: 208 `{0,0,+1,1}`, `{11,10,+1,1}`, `{62,60,0,1}`.

**Detach `0x493370`** (shared with knock-out): allocate a world object
(`0x4060c0`, list `[0x5301b0]`), +0x3c := `[0x53022c]`, +0x64 := −root +0x64,
+0x30 := weapon id (`0x4947a0(0)`); id `0x62` also runs `0x4988a0`; unlink the
mesh from the hand (`0x405ca0`), make it the object's model (+0x18 both ways),
`0x4089e0`, `0x4b8140`; object position := hand transform record +0x3c/+0x40/
+0x44 (part 10's world translation); clear the mesh's local offset/rotation;
task +0x30 := 0; `0x406340` links the object. Group +0x40 bit 2 stays set.

**Throw placement `0x498480`** then sets object y := −1.2 (`0xfffecccd`) and
+0x68 := thrower group +0x64; the flight task `0x498550` (same step) sets x/z :=
thrower x/z + 0.6 × facing vector and velocity := 0.2 × facing (`0x3333`), then
runs the shared projectile runner `0x49c690(0x4ed1c8)`
(selector 18, on-hit `0x4986d0`, `0x498730`, end `0x498710` = free object +
end task); id `0x6c` uses `0x4ed1d8` (selector 19).

| Scenario | Object | Contact |
| --- | --- | --- |
| `throw_weapon` | step 22: x 0.8, y −1.2, vx +0.2 per step, rotation `0x4b665`; freed after step 36 (gone at 37) | `0x48d410` sel 18 every step 22..36, all miss |
| `throw_weapon_hit` | hit on 22 → falls with the knock-out bounce (below): lands 59 and 81 (sound `0x15`), rests on y −0.46 at step 89 | 22 hit |

The miss path frees the object after 15 flight steps in this harness; the
runner's real end test (screen bounds / range) is **unresolved** with a fake
camera. After a throw the predicate refuses another draw (`draw_predicate` 0).

## 8. Losing the sword when hit

**Who drops.** `0x494520` (static): if armed, id `0x6c` always drops; others
drop unless `[0x524f00] & 0x400`; drop = `0x493370` + task type `0x60`,
callback `0x494590` with R414 = object. **Deterministic**, no random roll. It is
called at the start of reaction handlers (reaction table `0x4ea4e8[id]`, entered
on the victim through `0x480f50` → `0x48f880` → redirect), by Scorpion's own
fatality entries `0x467ce0` and `0x467ec0` (the performer drops it), by
`0x41ab90` and `0x4468c0` (callers outside this area); `0x481010` (round-end
pose) drops only id `0x6c`.

Reaction ids whose handler (or its direct jump/call chain inside
`0x481000..0x489000`) calls `0x494520` (static scan; dynamic checks: `0x30`
drops, `0x77` keeps): `0x0a–0x0d, 0x0f–0x17, 0x1a, 0x1c, 0x1d,
0x1f, 0x22, 0x26–0x2a, 0x2c, 0x2d, 0x2f–0x3b, 0x3e–0x42, 0x44–0x46, 0x49–0x4c,
0x53, 0x54, 0x56, 0x57, 0x5a, 0x5c, 0x5f, 0x60, 0x62, 0x64–0x6b, 0x6d, 0x79–0x82,
0x86–0x88, 0x8a–0x8e, 0x90–0x99, 0x9b, 0x9c, 0x9e–0xa0, 0xa2, 0xa4, 0xa7–0xae,
0xbc–0xc0`. All others keep the weapon (blocked reactions such as `0x04`,
`0x76`, `0x77` keep). Reaction `0x6c` (`0x484060`) detaches the victim's weapon
through `0x493370` directly (sound `0x489ff0(3)`, task `0x70`, scripts
`0x4e9818`/`0x4e9838`) — **unresolved** which attack uses it.

**Bounce task** (`0x494590` → `0x4941b0` → `0x494230`/`0x494390`, static and
oracle): object rotation/velocity cleared, bounce program `[0x53d580 + id −
0x60]` (Scorpion `0x4ecaf0`), landing sound `0x4eca38[id − 0x60]` (`0x15`,
played on every landing). Program: word 0 = floor y; then 3-word entries
{spin +0x80, launch vy +0x70, rest rotation +0x68} until `0xffff0000`; an entry
starting with `0xfffe0000` calls the native in its next word instead.
Scorpion `0x4ecaf0`: floor −0.1; {`0x20c4`, −0.15, `0x4b5c2`}, {`0x23d7`, −0.1,
`0x191eb`}, {0, −0.08, `0x191eb`}, end. Each flight: +0x80 := spin, +0x70 := vy,
+0x6c/+0x74 := `0x4ab690(0xa3d)` (random draw, deterministic seed) × P1's
facing vector `[0x530238]`/`[0x53023c]` (always P1's, static `0x494482`); every
step vy += `0x28f`; landing when vy ≥ 0 and y ≥ floor: y := floor, +0x80 := 0,
+0x68 := rest rotation, velocities cleared (`0x4906e0`), next entry. After the
last entry the task ends and the object lies still; the world update
`0x4051e0` → `0x407560` integrates position and rotation every step.

`knocked_out` (hit reaction `0x30` = `0x482580`, hand seeded at x 0.5, y −1.0):
sword at (0.488, −1.15) on step 0 with v (−0.0117, −0.15), spin `0x20c4`;
lands on steps 37, 59, 77 (sound `0x15` each); rests at (0.163, −0.1),
rotation `0x191eb`, from step 77. Victim: clip 180, push −0.18 decaying ×0.9,
controller 26, end 31, then unarmed stance 290 and block `0x4e5058`; draw
predicate 0. `blocked_keeps_weapon` (`0x486a70`): weapon, stance 286 and armed
block kept.

## 9. Picking up a weapon (either player, any weapon id `0x60..0x6f`)

1. **Trigger.** RUN list record `0x4e4a38`: unconditional, predicate index 2 →
   `0x45efc0`, table 1 index 3 → `0x46a020`. The predicate (runs for the
   pressing player) requires own action `0x2001` (crouched), task +0x30 = 0, and
   `0x45f8f0` to find a world object in `[0x5301b0]` with +0x30 in
   `0x60..0x6f` (id `0x290` excluded), velocities +0x6c/+0x70/+0x74 all zero
   (resting) and dx² + dz² ≤ 1.0 (`0x404af0` Q16 squares). Inside the predicate
   `0x46a180` already attaches it: `0x493750(0xa)` → if id `0x62` and not
   `[0x534b70] & 1`, `0x48a130/0x48a160`; reparent the mesh to the presser's
   part table and hand (`0x408580`, `0x405ca0`, `0x409970`), task +0x30 :=
   `0x4939c0(id)` (first record with that id), `0x493690` hand offset,
   `0x406790` frees the world object; sound `0x26` (P1) / `0x27` (P2).
2. **Move `0x46a020`.** Id `0x6c` → `0x46a1d0`. Otherwise action `0x2006`, clip
   `[0x53ce20]` = 209 from 0, play to the end (`0x428ce0`), `0x4902b0`, then
   crouch `0x45fc40` (armed clips).

`pickup` (RUN on step 96, fighter 0.5 beside the resting sword, crouched since
85): attach and sound on 96; 209 `{96,0,+1,1}` to sample 14; crouch 219 from
111; record `0x4ecdf8`, block `0x4e5070`, predicate 1 (armed). `pickup_too_far`
(1.5 away): no attach, stays crouched.

## 10. End of the armed state

* No sheathe routine exists; the only native reached by the draw command while
  armed is the throw (section 7).
* Round transition `0x421380` → `0x428020` → world init `0x404f20` resets every
  pool and object list (dropped/thrown swords vanish) and `0x4642f0` rebuilds
  the fighters; fresh controller tasks are spawned by `0x4228a0` (`0x49cae0`
  with `0x4233e0`/`0x423460`), whose round start `0x4235e0` creates a weapon
  immediately only if `[0x524f00] & 4`. **HYPOTHESIS**: the new task slot starts
  with +0x30 = 0 and the new group with +0x40 = 0 (allocation zeroing not
  traced).
* At the round-end pose `0x481010` only weapon `0x6c` is dropped.

## 11. Attack records

| Move | Routine, table[sel] → record | Box r0, r1, r2, r3 | Hit / air / block reaction (drops?) | Damage |
| --- | --- | --- | --- | --- |
| HP slash | `0x48d430`, `0x53d0c0[1]` → `0x4eb990` | 1.5, −0.54, 0.87, 0.6 | `0x30` (yes) / `0x0c` (yes) / `0x77` (no) | `0x3333` (0.20) |
| LP slash | `0x53d0c0[0]` → `0x4eb990` | same | same | 0.20 |
| back+LP | `0x53d0c0[2]` → `0x4eb9c0` | 1.9, −0.52, 1.27, 0.78 | `0x31` (yes) / `0x0c` / `0x77` | 0.20 |
| back+HP | `0x53d0c0[3]` → `0x4eb9d8` | 1.7, −0.52, 1.27, 0.78 | `0x32` (yes) / `0x0c` / `0x77` | 0.20 |
| spin | `0x53d0c0[22]` → `0x4eb978` | 1.2, 0.03, 2.95, 0.38 | `0x63` (no) / `0x0a` (yes) / `0x76` (no) | `0xccc` (0.05) per hit |
| air HP | `0x48d3d0`, `0x53d268[7]` → `0x4ebd08` | 1.69, −0.93, 1.04, 0.96 | `0x9c` (yes) both / `0x04` | `0x2e14` (0.18) |
| air LP | `0x53d268[8]` → `0x4ebcf0` | 1.55, −0.07, 0.98, 0.82 | `0x9b` (yes) both / `0x04` | 0.18 |
| thrown sword | `0x48d410`, `0x53d208[18]` → `0x4ebe10` | 0.88, −0.58, 1.13, 0.55; flags 2 | `0x9a` (no) both / `0x00` | `0x4000` (0.25) |

Blocked damage is 0.2 × damage (`0x48d5f5`, see combat-core).

## 12. Sounds

`0x489f90(id)` plays `0x4eaa18[id]`: draw `0x12`; slashes `0x22`; back+HP extra
`0x13`; spin `0x1d`; throw `0x4d`; sword landing `0x15`; pickup `0x26`/`0x27`.
`0x48a230(0)` (draw start) is a per-character voice from `0x53d0a0`;
`0x48a360` (armed air attacks) and `0x489ff0` (knock-out) pick from lists.

## 13. Engine requirements (C runtime)

1. `Fighter.weapon` (`const WeaponDef *`, NULL = unarmed) plus `bool
   weapon_spent` (group +0x40 bit 2) reset per round.
2. `typedef struct WeaponDef { int id; int stance_clip, walk_fwd_clip,
   walk_back_clip; const ButtonTable *buttons; const WeaponAttack *hp, *lp,
   *back_hp, *back_lp; Vec3 hand_offset; int hand_part; int bounce_floor; const
   int *bounce; int land_sound; } WeaponDef;` — one const row per weapon id,
   indexed by character like `0x53d5c0`.
3. Per-state button tables (HP, LP, BLK, HK, LK, RUN → FighterFn) installed by
   moves, with a constant locked table; button presses select through the
   installed table only (no direct state→move mapping).
4. A data-driven weapon attack runner (`WeaponAttack {clip; phase1_end;
   phase2_end; active; start; selector; sound; post_contact;}`) expressed with
   sparse segments and a contact event per active step; contact ends the active
   phase and skips one sample.
5. Clip substitution `armed ? alt : base` for crouch/block/stand/walk slots.
6. A world-object list: `WorldObject {Vec3 pos, vel, rot, spin; int weapon_id;
   ObjectFn think; int task_type;}` with the integrator semantics of `0x407560`,
   used by the bounce task, the thrown-sword projectile (shared projectile
   runner) and pickup search (resting, dx²+dz² ≤ 1.0).
7. Hold-to-continue input (spin reads the raw held LP bit every step) and
   joystick steering; airborne attacks keyed on the jump's button table.
8. Victim-side reaction table with a `drops_weapon` flag per reaction id.

## 14. Unresolved

* Weapon record word 7 (−0.3) and parameter-block word 4 (task +0x6c) readers.
* `0x4946f0` effects in the window draw; which normal moves set bit `0x200`
  (normals spec).
* Projectile flight end and hit handling inside `0x49c690`/`0x4986d0`/
  `0x498730` beyond the observed drop (shared projectile spec); real camera
  bounds not modelled.
* Reaction `0x6c` (`0x484060`) user; reaction-id drop list is static except
  `0x30`/`0x77`.
* `0x493350`/`0x4ab570` kombat-code paths (`[0x524f00]` bits 1, 4, `0x20`,
  `0x400`), alternative weapon tables `0x53d610`/`0x53d660`.
* Render-side: geometry block of the sword (**HYPOTHESIS** `0x50609c` → `0x4f1ad8`
  high half `0x1d` = block 29 of the character GEO), the 5-step pose blend
  `0x407d50`, rotation decode of `0xca2fafc0`.
* Matcher gating of F,F,HK during other moves (`0x45dd30`); the oracle injects
  matched commands.
* Throw-range LP while armed: whether the sword stays in hand during `0x42c560`.
* Round-reset zeroing of task +0x30 / group +0x40 (section 10).

