# Scorpion: input sampling, command matching and normal-move selection

EXE: `MK4/MK-4.EXE`, SHA-256 `4bb7112e…0d1d` (the supplied June-1998 PC build).
All addresses are this EXE. "Packed" pointers are word indices (byte = value×4).
Facts below were read from the x86 code (VA cited) and, where marked
**[oracle]**, measured by executing the original routines in
`tests/verify_scorpion_input.py` (fixture `tests/fixtures/original_scorpion_input.json`,
191 scenarios). **HYPOTHESIS** marks interpretations that are not proven.

This note covers how a press becomes an action. It does not cover move
playback, contact, damage or projectile/teleport behaviour (other notes).

## 1. Per-tick pipeline

The frame routine at `0x41fde0..0x41fe25` runs, in order:

1. `0x45c560` **switch scan**. Three raw switch words are compared with the
   two previous samples: `0x4d00a4` (prev `0x534860`, prev-prev `0x534864`),
   `0x4d00a8` (`0x534868`/`0x53486c`) and `0x4d00b0` (`0x534870`/`0x534874`).
   Only the low 16 bits are used. **Switches are active low** (bit 0 = down).
   The PC input layer writes these words at `0x4b4a40..0x4b4ba9` (keyboard /
   joystick mapping not researched here).
2. `inc [0x534858]`, the input/game tick counter used for all time stamps.
3. `0x45c7c0` drains the switch-event queue (ring `0x534878..0x5348c8`,
   20 slots, write `0x5346b0`, read `0x5346dc`) and spawns one **type 0x11
   task per event** (`0x49cb00` → `0x41f290`), callback = the switch handler.
4. `0x49cb80` → `0x41f570` task pump. Handler tasks are appended to the end
   of the task list, so they run **in the same tick, after the fighter
   tasks**.

### Press/release detection (`0x45c660`)

```
changed = old0 ^ new;   if !changed: nothing
pressed = ~new & changed & old0 & old1     // up in BOTH previous samples
if pressed:  table = press table (0x4e4268 / 0x4e42e0 / 0x4e4330), bits = pressed
else:        table = release table (0x53cd08 / 0x4e4318 / 0x53cd0c), bits = changed
for (mask, handler) in table: if bits & mask: enqueue handler
```

* A press needs the switch **up for the two previous samples**. A one-tick
  release between taps is lost **[oracle `spear_retap_after_1_tick`]**; two
  ticks work (`spear_retap_after_2_ticks`).
* Releases are only examined in a word **when that word has no new press in
  the same tick** (quirk of the `else`).
* Holding a switch produces exactly one event.

### Switch map (game mode 4 = fight)

Each switch handler is a stub `push table; call 0x48e650/0x48e680`, which
checks the player is human (`0x5347c8` P1 / `0x532248` P2) and jumps to
`table[[0x524e84]]` (the game-mode slot; mode 4 in fight).

| Word / bit | P1 | Fight handler | Command list (per character, 18 entries) | Latch written | Token pushed |
|---|---|---|---|---|---|
| a4 `0x0001` | Up | `0x45c9b0` | `0x53c888` | – | 1 → dir history |
| a4 `0x0002` | Down | `0x45ca10` | none | – | 2 → dir history |
| a4 `0x0004` | Left | `0x45ca30` | none | – | 3 → dir history |
| a4 `0x0008` | Right | `0x45ca50` | none | – | 4 → dir history |
| a4 `0x0010` | HP | `0x45ca90` | `0x53c8d0` | `0x53c1e4` (before match) | 5 → button history |
| a4 `0x0020` | BLK | `0x45cb90` | `0x53c960` | `0x53c174` (before) | 7 |
| a4 `0x0040` | HK | `0x45cc40` | `0x53c9a8` | none | 8 |
| a8 `0x0001` | LP | `0x45caf0` | `0x53c918` | `0x53c204` (before) | 6 |
| a8 `0x0002` | LK | `0x45ccb0` | `0x53c9f0` | `0x53c1fc` (before) | 9 |
| a8 `0x0004` | RUN | `0x45cdf0` | `0x53cac8` | `0x524e78` (**after** match) | 10 |
| a8 `0x0002` release | LK release | `0x45cd60` | `0x53ca38` | – | – |

P2 uses a4 bits `0x100/0x200/0x400/0x800` (U/D/L/R), `0x1000` HP,
`0x2000` BLK, `0x4000` HK, a8 `0x10` LP, `0x20` LK, `0x40` RUN, release
a8 `0x20`. P2 handlers `0x45ce80, 0x45cee0, 0x45cf00, 0x45cf20, 0x45cf40,
0x45d360, 0x45d400, 0x45cfc0, 0x45d470, 0x45d510, 0x45cda0`; P2 lists
`0x53cb10, 0x53cb58, 0x53cba0, 0x53cbe8, 0x53cc30, 0x53cc78, 0x53ccc0`,
LK release `0x53ca80`; P2 latches HP `0x53c200`, LP `0x524e48`, BLK `0x53c1bc`,
LK `0x524e44`, RUN `0x524ea8`. For character 0 the P1 and P2 lists point at
the same records. Note: `tools/scorpion_moves.py` labels `0x53ca80` as P2
"up"; it is the **P2 LK-release** table (both are the empty list `0x53cd10`
for Scorpion, so its inventory is unaffected).

Word `0x4d00b0` bits `0x1000`/`0x800` (P1) and `0x1`/`0x2` (P2) inject
command word `0x1d01000f` directly (`0x45d060..0x45d351`, 120-tick cooldown
`0x53dc4c`/`0x53dc50`, direction `0x53dc44`/`0x53dc48` = 1/2). These are
PC-only side-step keys (**HYPOTHESIS**, see §5 RUN). Other `0x4d00b0` bits
(`0x4`, `0x20`) are non-fight handlers.

## 2. Raw representation and histories

**Held** state is the inverted raw word: `~[0x4d00a4]` etc. It is read
directly by stance code: block held (`0x48ec90`: P1 `0x20`, P2 `0x2000`),
back held (`0x48ebd0`), run held (`0x460e50`: a8 `0x4`/`0x40`), joystick
(`0x460d20`: `~a4 & 0xf`, P2 `>>8`).

**Pressed** is an event: the switch handler task. A press is evaluated once,
at its event; there is **no input buffering** in the matcher. (Moves that
accept late presses read the latches, e.g. the jab reads `0x53c204`.)

Histories, initialised by `0x45c880` (callers `0x4265c5`, `0x48bbd3`), each
8 words, newest at index 0, shifted by `0x45d950` on every push:

| | Tokens | Tick stamps |
|---|---|---|
| P1 directions | `0x524ed8` | `0x534660` |
| P1 buttons | `0x5346f0` | `0x534618` |
| P2 directions | `0x53c1c0` | `0x5346b8` |
| P2 buttons | `0x534b10` | `0x532408` |

Direction tokens are **absolute**: 1 up, 2 down, 3 left, 4 right. Only
direction histories are read by the matcher. No static reader of the button
histories exists in the EXE (written only by `0x45d8a0`/`0x45d8e0`).

The button token is pushed **after** matching/dispatch; the Up token after
the Up list is matched; Down/Left/Right only push. A diagonal produces one
token per closing switch, so the roll D, D+B, B yields D, B
**[oracle `teleport_diagonal_roll`]**. Holding a direction yields one token
(`spear_single_hold` fails).

### Facing-relative tokens (`0x45dd30`, `0x45e050`, `0x45e070`)

At match time: `facing_left = opponent.x(+0x54) < self.x` (strict; swapped
when the mirror flag `0x534af0` is set). Pattern byte `0xDD` (forward) becomes
token 4 (right) facing right / 3 facing left; `0xAA` (back) becomes 3 / 4.
`0x02` down, `0x01` up are compared raw. Because conversion happens at match
time, inputs entered before a cross-over change meaning afterwards
**[oracle `teleport_facing_left`, `teleport_absolute_left_while_facing_left`]**.

## 3. Command records and the matcher

A list is `{pattern, unused, word}` × N, terminated by `pattern == 0`.

* `pattern`: up to four token bytes, **low byte = earliest input**;
  `0xffffffff` = unconditional (no tokens, no window, no block check).
* `unused`: `0` or `0xffffffff`; never read by `0x45de00`.
* `word` = `gate<<24 | flags<<16 | window<<8 | action`.

`command_list_select 0x45dd30` (called by `0x45dc70` P1 / `0x45dcd0` P2 with
the list table): saves the direction history/stamp pointers in the handler
task (+0x64/+0x68), computes facing, selects `table[character]`
(task +0x34), and enters `command_match 0x45de00`:

```
for record in list:
  if pattern == 0: return NO_MATCH
  if pattern != -1:
     n = token count; compare pattern byte k (k=0 earliest) with dir_history[n-1-k]
       (0xDD/0xAA converted by facing); any mismatch -> next record
     if !(flags & 0x40) and BLOCK held: next record
     if window and now - dir_stamp[n-1] > window: return NO_MATCH   // stops the list!
  if !(flags & 0x10) and !(group.flags40 & 0x200) and group.y(+0x58) != group.floor(+0x48):
       next record
  if group.flags40 & 0x1 or group.flags40 & 0x4: return NO_MATCH  // stops the list
  if gate: call predicate[gate] (table 0x4e4ec0); false -> next record
  return word
```

Exact semantics, confirmed **[oracle]** unless stated (trace labels:
`reject tokens_or_block` = `0x45df60`, `no_match_stop` = `0x45e024`, reached
by both list end and abort):

* **Window**: elapsed = tick of the button press − tick of the **first**
  direction token of the pattern; match iff `elapsed <= window`
  (`teleport_window_24` match, `_25` no; spear 27/28; fire 20/21; weapon
  24/25; fatality-2 50/51; fan 41 no). There is no limit on gaps between
  tokens other than the overall window, and the directions need not be held.
* **Order and contiguity**: the last *n* direction presses must equal the
  pattern exactly. Any extra direction press in between or after breaks it
  (`teleport_extra_direction_between`, `_after`, `_wrong_order`). Button
  presses in between do not matter (`teleport_buttons_between`).
* **Window failure aborts the whole list**, not just the record. Example:
  B,F,F,B + BLK at elapsed 51 while airborne next to an airborne opponent
  aborts on fatality-1 and never reaches the air-throw record
  (`air_throw_shadowed_by_slow_fatality_tokens` → plain block).
* **Block held** rejects every record without flag 0x40 (`teleport_block_held`,
  `spear_block_held`). The only 0x40 record is fatality 1, whose button is BLK.
  Unconditional records skip the block test entirely (the BLK air throw).
* **Ground**: records without 0x10 need `y == floor` unless the fighter is in
  a follow-up window (flags40 `0x200`) (`spear_airborne`, `fire_airborne` fail;
  `teleport_airborne` matches).
* **Busy**: flags40 bit `0x1` returns no match for the whole list
  (`teleport_busy_flag`); bit `0x4` takes the same path (static, `0x45e01a`,
  not exercised). In the game the same begin-action that sets bit 0
  also installs the empty button table, so a rejected press then does nothing.
* **Priority**: records are tried in table order; the first full match wins.
* **No match** falls back to the stance button table (§6). A matched word
  never also performs the normal move.
* There are **no charge or release-only commands for Scorpion** (his
  LK-release list is empty); the release mechanism exists for characters
  3, 8 and 13 (`0x53ca38[c]`). Up lists exist for characters 1, 7, 10 and 17.

### Flag bits

| Bit | Meaning | Evidence |
|---|---|---|
| `0x01` | dispatch through action table T1 `0x4e6ef8` (after begin-action + `0x4906e0`) | `0x45f67c` |
| `0x02` | table T2 `0x4ed828` | `0x45f6d1` |
| `0x04` | table T4 `0x4e8910` | `0x45f731` |
| `0x08` | table T8 `0x4e7008` (no `0x4906e0`) | `0x45f790` |
| `0x10` | allowed when not on the ground | `0x45dfb4` |
| `0x20` | table T20 `0x4dfc90` | `0x45f7db` |
| `0x40` | allowed while BLK is held | `0x45df16` |
| `0x80` | unused by input code | – |

The dispatch test order is 01, 02, 04, 08, 20 (first set bit wins).

### Dispatch (`0x45da20` P1 / `0x45da50` P2 → `0x45da90`)

* Match: fighter task +0x14 = `word`, `task_interrupt 0x41fa50(task, 0x45f5f0)`.
  The interrupt clears task +0x84, sets callback +8/+0xd8, +0x10 = 1,
  sleep +0xdc = 1 and **resets the continuation stack** (+4 = base), so the
  current move is abandoned.
* No match: handler = `button_table[button]` where the button table pointer
  is `[0x530228]` (P1) / `[0x534640]` (P2) and button index is HP 0, LP 1,
  BLK 2, HK 3, LK 4, RUN 5. A zero entry does nothing. Otherwise `0x45db10`
  runs `0x45dc00` (begin-action that preserves the facing bit), clears
  velocity if grounded, and interrupts the fighter with that handler.
* `action_dispatch 0x45f5f0` (runs in the fighter task; the pump restores
  +0x14 into `0x53c43c`): if flags40 `0x200`, snap to floor (`0x4906c0`);
  begin-action `0x45f870` (busy bit 0 via `0x490200`, empty button table
  `0x53d370` via `0x48f6c0`, motion reset); then `call table[action]`.
  A zero entry goes to `0x45fa30` (back to stance).

**Latency (HYPOTHESIS, from pump order, not measured):** the handler runs in
the pump of tick T after the fighter was already dispatched, and the
interrupted fighter (sleep 1) runs the new callback in the pump of T+1.

## 4. Scorpion command list (character 0)

Lists: HP `0x4e49b8`, LP `0x4e49d8`, BLK `0x4e49f8`, HK `0x4e4a58`,
LK `0x4e4a18`, RUN `0x4e4a38`; Up and LK-release are the empty list `0x53cd10`.

| Rec VA | Button | Tokens (earliest first) | Window | Flags | Gate → predicate | Table[action] → entry | First action write **[oracle]** |
|---|---|---|---|---|---|---|---|
| `0x4e49b8` | HP | D, B | 24 | `0x14` | 7 → `0x45f0c0` | T4[0] → `0x47fc90` teleport | `0x209` (+0x68 = `0x404`) |
| `0x4e49c4` | HP | B, F, D, U | 50 | `0x01` | 0x1b → `0x45e570` | T1[0x43] → `0x467cd0` fatality 2 | not written within 60k instr. |
| `0x4e49d8` | LP | B, B | 27 | `0x02` | 0x15 → `0x45ea60` | T2[6] → `0x49a630` spear | `0x300` |
| `0x4e49e4` | LP | D, F | 20 | `0x01` | none | T1[0x28] → `0x468610` fire | `0x115` |
| `0x4e49f8` | BLK | B, F, F, B | 50 | `0x41` | 0x25 → `0x45e530` | T1[0x40] → `0x467ce0` fatality 1 | unresolved (dependency) |
| `0x4e4a04` | BLK | unconditional | – | `0x11` | 6 → `0x45ed70` | T1[0x22] → `0x46aa80` air throw | `0x502` |
| `0x4e4a58` | HK | F, F | 24 | `0x01` | 3 → `0x45ef50` | T1[0x29] → `0x46d1d0` draw weapon | `0x1004` unarmed; armed → `0x2000d` → T2[0xd] `0x498270`, `0x30f` |
| `0x4e4a18` | LK | F, D, D | 40 | `0x01` | 0x1a → `0x45e5a0` | T1[0x41] → `0x467e00` fan stage fatality | returns without write |
| `0x4e4a24` | LK | B, F, F | 40 | `0x01` | 0x33 → `0x45e4f0` | T1[0x42] → `0x467e70` spike stage fatality | returns without write |
| `0x4e4a38` | RUN | unconditional | – | `0x01` | 2 → `0x45efc0` | T1[3] → `0x46a020` weapon pick-up | unresolved |
| `0x4e4a44` | RUN | unconditional | – | `0x01` | 0x1d → `0x45e820` | T1[0xf] → `0x469250` side-step (HYPOTHESIS) | unresolved |

Raw words: `0x07141800, 0x1b013243, 0x15021b06, 0x00011428, 0x25413240,
0x06110022, 0x03011829, 0x1a012841, 0x33012842, 0x02010003, 0x1d01000f`.
Gates index the predicate pointer table `0x4e4ec0` (spike: `0x33` →
`0x4e4f8c` → `0x45e4f0`). The LP fire record has gate 0 (no predicate).

### Predicates (plain language)

`stamp[i]` is the per-player tick table `0x53c2a8 + (i*2 + player)*4`,
written by `0x48e3a0(i)`; "ago" = `now - stamp[i]` (`0x48e3f0`).
`opp` = the other fighter task (`0x5323f8`/`0x5323fc`).

* **`0x45f0c0` teleport**: `opp+0x7c < 3` and `stamp[1] ago >= 69`
  (stamp 1 is written by the teleport continuation `0x47fcf0`).
  **[oracle]** counter 2 ok / 3 rejected; 68 rejected / 69 ok.
  `opp+0x7c` is cleared by the stance reset `0x48e080`; its increment sites
  (`0x47bac4`, `0x483b20`) are not identified (**HYPOTHESIS**: hits taken in
  the current combo/juggle).
* **`0x45ea60` spear**: `stamp[0x13] ago >= 90` (written at the end of the
  spear, `0x49a680`), opponent action ≠ `0x1008`, and `opp+0x7c <= 3`.
  **[oracle]** 89/90, `0x1008` rejected, counter 3 ok / 4 rejected.
* **`0x45ef50` draw weapon**: true if the fighter already holds a weapon
  (task +0x30 ≠ 0); otherwise true unless group flags40 bit `0x2` is set
  (**HYPOTHESIS**: weapon lost this round; no setter found). Entry
  `0x46d1d0` stamps slot 0xe; armed → put-away path `0x2000d`.
* **`0x45e570` fatality 2**: distance `[0x53019c] <= 0x10000` and *finisher
  allowed* `0x45e5e0`.
* **`0x45e530` fatality 1**: `0x1cccc <= distance <= 0x30000` and finisher
  allowed. **[oracle]** 0x1cccb / 0x30001 rejected.
* **`0x45e5a0` fan**: arena `[0x5348dc] == 8`, distance `<= 0x10000`,
  finisher allowed. **`0x45e4f0` spike**: arena `== 0`, same.
* **Finisher allowed `0x45e5e0`**: round winner `[0x532354]` ≠ 0, finisher
  already done `[0x532358]` == 0, byte `[0x4ee624]` ≠ 0, this fighter is the
  winner (`0x48fb90`: 1 → P1 else P2), and the opponent controller callback
  (`winner_task+0x3c → +8`) == `0x46a580` (dazed "finish him" loop).
  **[oracle]** each condition removed individually rejects fatality 2.
* **`0x45ed70` → `0x45ed80` air throw**: both fighters' y (+0x58) `<= -2.2`
  (`0xfffdcccd`, y grows downwards) and `dx²+dy²+dz² <= 2.0` in Q16
  (`0x20000`, products via `0x404af0`), i.e. distance ≤ 1.414. Uses the
  absolute P1/P2 groups. **[oracle]** close → `0x46aa80`, x apart 2.0 → block.
* **`0x45efc0` weapon pick-up**: own action == `0x2001` (crouching), unarmed,
  and a world object of type `0x60..0x6f` in the list `[0x5301b0]` that is
  idle (+0x6c/+0x70/+0x74 = 0) with `dx²+dz² <= 1.0` (Q16)
  (`0x45f8f0`). Side effect when true: `0x46a180` (sound 0x26/0x27, call
  `0x493750(10)`) — stubbed in the oracle.
* **`0x45e820` RUN double tap**: `stamp[0xa] ago >= 80` and `now - previous
  RUN press <= 8` (the RUN latch is written after matching, so it still holds
  the previous press). On success writes `0x53dc4c`/`0x53dc50`. **[oracle]**
  8 ok, 9 rejected, cooldown 79 rejected. Action `0xf` (`0x469250`) moves
  along ±z with parameter blocks `0x4e7058`/`0x4e7068` (`±0x0ccc`), sound
  `0x52`, stamps slot 0xa: **HYPOTHESIS side-step**.

## 5. Controller idle loop and movement initiation

`fighter_controller_return 0x46f650`: action `0x200b`; `0x4906c0` (y =
floor, zero velocity); `0x48fc20` (once `[0x532354]` names a round winner:
the loser goes to T1[0x3f] `0x46a470`; the winner goes to T1[0x3e]
`0x46a1e0` only when `[0x527ae4]` ≠ 0, otherwise keeps normal control —
**HYPOTHESIS** finish-him phase); `0x48e080` stance reset (flags40
`&= 0xfffffd17`, clear busy `0x490330` and bit 2 `0x490310`, task
+0x7c/+0x74/+0x80 = 0 — so the idle action reads 0 after the reset, group
+0x44 = 0, **button table = standing `0x4e5058`**, or when armed
`[weapon_descriptor + 3 words]` via `0x4947a0(3)`); `0x48f2f0` (drone/CPU
check, bit 4 → AI `0x433c00`/`0x4339b0`); `0x490f60`; then `0x46f6e0` →
`0x46f740` each tick:

1. If phase (+0x84) was set: idle pose fraction `+0x20` += `0x3333`; above
   `0x10000` → `0x470420`, else one more `0x46f6e0` pass.
2. `0x48ec90`: BLK held → `0x4601f0` → `0x460200` block.
3. `0x460de0(0x4e68c0)`: joystick `~a4 & 0xf` (absolute) indexes
   `0x4e68c0`: U `0x460c00`, D `0x45fc90`, L `0x460710`, R `0x460570`,
   U+L `0x460c70`, U+R `0x460c40`; **D+L, D+R, U+D and none are 0** (stay).
4. Otherwise sleep one tick with callback `0x46f740`.

Initiation (static; the oracle does not run these loops):

| Input | Path | Entry | Action |
|---|---|---|---|
| Up | `0x460c00` → `0x4711f0` idx 2 | T8'[2] `0x46f7e0` | `0x2007`, installs `0x4e6ec8` |
| Up+toward | `0x460c40/0x460c70` → `0x460ca0` idx 3 | `0x46fdf0` | `0x2009`, then `0x46ff20` installs `0x4e70d8` |
| Up+away | → `0x460ce0` idx 4 | `0x46fe80` | `0x200a`, same table |
| Down | `0x45fc90` → idx 0 | `0x46b5e0` | `0x2001`, loop `0x45fcd0` installs crouch `0x4e5070`, clears busy |
| release Down | `0x45fdc5` idx 1 | `0x46b1b0` | `0x2004` stand up |
| Left/Right | `0x460710` / `0x460570` loop | toward/away by `0x48f030` | walk (codes not observed) |
| RUN held while moving toward | `0x460e50` → `0x460ec0` → `0x460970/0x460960` | run loop `0x4609f0` (joystick table `0x4e4f98`) | not observed |
| BLK held (idle/crouch) | `0x460200` / `0x45ff60` | `0x46b1f0` / `0x46b350` | `0x2005` / `0x2002` |

`T8'` = `0x4e7020` = T8 + 6 words. Walking and running keep the standing
button table; there is no separate walk/run button table.

## 6. Normal-move selection

The button table pointer `[0x530228]`/`[0x534640]` is written only through
`0x48f670` (`[0x53c3f8]`/`[0x53c3fc]` hold its packed address).

| Table | Installed by | When |
|---|---|---|
| `0x4e5058` standing | `0x48e080` | every controller (re)entry, unarmed |
| `0x4ecd08` armed standing | `0x48e080` → `0x4947a0(3)` | holding the sword descriptor `0x4ecdf8` (`[0x53d5c0+0]`, type `0x60`) |
| `0x4e5070` crouching | `0x45fcd0` phase 0 | crouch loop |
| `0x4e6ec8` jump up | `0x46f7e0` | straight jump |
| `0x4e6ee0` jump up after punch | `0x46e7c0` | after air punch (from `0x46e6c0` param block `0x4e6680`) |
| `0x4e70d8` diagonal jump | `0x46ff20` | forward/back jump |
| `0x4e7078` follow-up | `0x46bf10` (tail of close-HP script `0x4e61e0`) | + flags40 `0x200` |
| `0x4e7090` follow-up | `0x46c2c0` (tail of `0x4e69b0/0x4e69c8/0x4e6a28`) | + `0x200` |
| `0x4e70a8` follow-up | `0x46bb60` (tail of close-HK script `0x4e6190`) | + `0x200` |
| `0x4e70c0` follow-up | `0x46cba0`, `0x46e7c0` | + `0x200` |
| `0x53d370` locked (all zero) | `0x48f6c0` | every begin-action, block, etc. |

Conditions used: *far* = `[0x53019c] > 0xcccc` (HP/HK); *close LP throw* =
`<= 0xb333`; *close LK throw* = `< 0xb333`; *back held* = `0x48ebd0` (raw
switch away from the opponent, false while flags40 `0x8`); *opp airborne* =
opponent y < floor (`0x48ee60`); *opp reacting* = opponent action
`& 0xf000 == 0x4000` (`0x48e6e0`).

### Stance × button **[oracle unless marked static]**

| Stance | Button / condition | Handler | Dispatch | Entry | First action (+0x74) |
|---|---|---|---|---|---|
| standing | HP far, or opp airborne, or opp reacting | `0x460f40` | T1[0xb] | `0x46d290` | `0x10a` |
| standing | HP close (≤ `0xcccc`) | `0x460f40` | T1[9] | `0x46be40` | `0x108` |
| standing | LP | `0x460fb0` | T1[8] | `0x46dca0` jab | `0x10b` (+0x68 `0x101`) |
| standing | LP ≤ `0xb333`, opp grounded, not reacting, opp action ∉ {`0x10a,0x10b,0x100,0x102,0x10e,0x111..0x113`}, self not `0x200`, `[0x524f00]&1 == 0` | `0x46dbb0` inside jab | jmp | `0x42c560` (T1[2]) throw | `0x500` (back/toward do not change it) |
| standing | LK | `0x460fc0` | T1[4] | `0x46a7b0` | `0x102` |
| standing | LK < `0xb333` (same opponent tests) | inside `0x46a7b0` | jmp | `0x42b6b0` | `0x501` |
| standing | back + LK | `0x460fc0` | T1[6] | `0x46ef90` sweep | `0x10c` |
| standing | HK far, opp airborne, or opp action `0x4005`/`0x4001` | `0x461030` | T1[5] | `0x46a680` | `0x100` |
| standing | HK close otherwise | `0x461030` | T1[0xc] | `0x46ba60` | `0x103` |
| standing | back + HK | `0x461030` | T1[7] | `0x46f1f0` roundhouse | `0x10d` |
| standing | BLK | `0x460200` | `0x4711f0` idx 0xc | `0x46b1f0` | `0x2005` |
| standing | RUN | none | – | – | – |
| crouching | HP | `0x4610c0` | T1[1] | `0x46ccd0` uppercut | `0x109` |
| crouching | LP | `0x4610d0` | T8'[8] | `0x46b7f0` | `0x107` |
| crouching | LP, armed descriptor type `0x60` | `0x4610d0` | `0x40019` → T4[0x19] | `0x480770` | `0x407` |
| crouching | BLK | `0x461130` | T8'[9] | `0x46b350` | `0x2002` |
| crouching | HK | `0x461190` | T8'[7] | `0x46b5a0` | `0x104` |
| crouching | LK | `0x461160` | T8'[6] | `0x46b710` | `0x106` |
| jump up | HP | `0x4712c0` (`0x80000`) | T8[0] | `0x46e6c0` | `0x603` |
| jump up | LP | `0x4712d0` (`0x80001`) | T8[1] | `0x46ee30` | `0x604` |
| jump up | HK, LK | `0x4712b0` (`0x80004`) | T8[4] | `0x46c7a0` | `0x600` |
| jump up after punch | HP | none | | | |
| jump up after punch | LP / HK / LK | as jump up | | | |
| diagonal jump | HP / LP | `0x46c1f0` / `0x46c200` | T8[0] / T8[1] | `0x46e6c0` / `0x46ee30` | `0x603` / `0x604` |
| diagonal jump | HK | `0x46c1d0` (`0x80002`) | T8[2] | `0x46e380` | `0x602` |
| diagonal jump | LK | `0x46c1e0` (`0x80003`) | T8[3] | `0x46df50` | `0x601` |
| any jump | BLK, RUN | none (BLK in air only via the air-throw command) | | | |
| armed standing | HP / LP | `0x496c20` / `0x496c30` | weapon parameter programs | unresolved | – |
| armed standing | BLK / HK / LK (incl. back) | same as standing | | | |
| follow-up `0x4e7078` | HP, unarmed | `0x46c210` | char table `0x4e6c50[0]` | `0x46c290` (script `0x4e69b0`) | not written before script (unresolved) |
| follow-up (all four) | LP | `0x46c6f0` | unarmed → jab `0x46dca0` (with throw test) | | `0x10b`/`0x500` |
| follow-up `0x4e7078`/`0x4e70c0` | HK | `0x46c680` → `0x461030` | as standing HK | | |
| follow-up `0x4e7090` | HK | `0x46c650` → `0x46c570`, then `0x461030` | | | |
| follow-up `0x4e70a8` | HK | `0x46c6a0`: back → `0x46f1f0`, else T1[5] (no close HK) | | | `0x10d`/`0x100` |
| follow-up (all) | LK | `0x46c740` → `0x460fc0` | as standing LK | | |
| follow-up `0x4e7090` | HP | `0x46c450` → `0x460f40` (armed → weapon) | | | |
| follow-up `0x4e70a8` | HP | `0x46c480`: `0x46c500` uppercut test ((opp action `0x4004` or opp+0x7c > 1) and own height above floor `<= 0x8000` and down held → `0x46ccd0`); else armed → weapon, else `0x460f40` | | | `0x10a`/`0x108` [oracle, test false] |
| follow-up `0x4e70c0` | HP | `0x46c3f0`: opp airborne or far → `0x460f40`, else `0x46be40` | | | |
| follow-up armed | HP / LP | `[descriptor+3][0/1]` | weapon entry | unresolved (reaches `0x481010` then needs weapon world) | |
| locked `0x53d370` | any | none | | | |

The diagonal-jump handlers `0x46c1d0..0x46c20a` and jump-up handlers
`0x4712b0..0x4712da` are stubs that set `0x53c43c` and jump to
`action_dispatch`.

### Shared entries and character selectors

Action tables are shared by all 18 characters. Entries that fork per
character with VM opcode `0x3c` (Scorpion = selector slot 0):

| Entry | Script | Scorpion script |
|---|---|---|
| `0x46a7b0` LK | `0x4e5d90` | `0x4e6d08` |
| `0x46ef90` sweep | `0x4e6798` | `0x4e6e68` |
| `0x46a680` HK | `0x4e5ca0` | `0x4e6c90` |
| `0x46f1f0` roundhouse | `0x4e6800` | `0x4e6840` |
| `0x46b5a0` crouch HK | `0x4e6020` | `0x4e60b8` |
| `0x46c7a0` jump kick | `0x4e61f8` | `0x4e6238` |

Native character fork: follow-up HP `0x46c210` selects `0x4e6c50[character]`
(Kitana 0x10 → 2, Noob 0x11 → 4); Scorpion → `0x46c290`. Other entries above
(jab `0x4e62f0`, close HP `0x4e61e0`, close HK `0x4e6190`, crouch LK
`0x4e6158`, air punches `0x4e6658/0x4e6710`, diagonal kicks
`0x4e6500/0x4e63d8`, uppercut, HP far, throws, blocks) use one program for
every character.

## 7. Group flags40 bits used by input

| Bit | Set by | Cleared by | Input effect |
|---|---|---|---|
| `0x001` busy | `0x490200` (begin-action `0x45f870`) | `0x490330` (33 call sites: stance reset, crouch loop, jab entry, VM op `0x61` handler `0x45b3a0`, …) | command lists return no match |
| `0x002` | not found (**HYPOTHESIS** weapon lost) | – | draw-weapon predicate false when unarmed |
| `0x004` | `0x4901d0` | `0x490310` | command lists return no match |
| `0x008` | `0x490140` (18 sites) | `0x4902b0` / reset | back-held test false |
| `0x200` follow-up window | `0x4900e0` (follow-up setters) | `0x490270` / reset | commands allowed off the ground; dispatch snaps to floor |

## 8. Proposed C runtime model

Fits `game/game.h` style (plain structs, const tables, explicit context) and
covers all 18 characters. Replaces the current 10-bit `Input` and `CommandDef`.

```c
/* Absolute switches, matching the retail bit order per player. */
typedef enum Switch {
  SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT,       /* absolute, not facing relative */
  SW_HP, SW_LP, SW_BLK, SW_HK, SW_LK, SW_RUN,
  SW_COUNT
} Switch;

typedef struct InputFrame {   /* host -> simulation, one per tick */
  uint16_t down;              /* bit i = switch i held this sample */
} InputFrame;

enum { TOKEN_UP = 1, TOKEN_DOWN = 2, TOKEN_LEFT = 3, TOKEN_RIGHT = 4,
       TOKEN_HP = 5, TOKEN_LP = 6, TOKEN_BLK = 7, TOKEN_HK = 8, TOKEN_LK = 9, TOKEN_RUN = 10 };
enum { PATTERN_UP = 0x01, PATTERN_DOWN = 0x02, PATTERN_BACK = 0xaa, PATTERN_FORWARD = 0xdd };
#define INPUT_HISTORY 8

typedef struct InputHistory { uint8_t token[INPUT_HISTORY]; int32_t tick[INPUT_HISTORY]; } InputHistory;

typedef struct PlayerInput {
  uint16_t prev[2];           /* two previous samples for the debounce rule */
  uint16_t down;
  InputHistory directions;    /* read by the matcher */
  InputHistory buttons;       /* kept for parity; unread by retail */
  int32_t latch[SW_COUNT];    /* tick of last press: HP/LP/BLK/LK before matching,
                                 RUN after matching, HK never */
} PlayerInput;

/* pressed = down & ~prev[0] & ~prev[1]  (inverted retail active-low test);
   released only when no press in the same retail word group:
   group A = {U,D,L,R,HP,BLK,HK}, group B = {LP,LK,RUN}. */

enum {                        /* CommandDef.flags, retail bit values */
  CMD_T1 = 0x01, CMD_T2 = 0x02, CMD_T4 = 0x04, CMD_T8 = 0x08,
  CMD_AIR = 0x10, CMD_T20 = 0x20, CMD_BLOCK_OK = 0x40
};
typedef bool (*CommandPredicate)(const Fighter *self); /* shared state: g_battle */

typedef struct CommandDef {
  uint8_t tokens[4];          /* earliest first; PATTERN_* values */
  uint8_t count;              /* 0 = unconditional record */
  uint8_t window;             /* ticks, inclusive, from first token's press */
  uint8_t flags;
  uint8_t action;             /* index into the selected action table */
  CommandPredicate predicate; /* NULL = none */
} CommandDef;

typedef enum CommandList { CL_UP, CL_HP, CL_LP, CL_BLK, CL_HK, CL_LK, CL_LK_RELEASE, CL_RUN, CL_COUNT } CommandList;
typedef struct CommandSet { const CommandDef *list[CL_COUNT]; int count[CL_COUNT]; } CommandSet;

typedef struct ButtonTable { FighterFn press[6]; } ButtonTable;   /* HP LP BLK HK LK RUN; NULL = nothing */

/* Fighter additions: */
/*   const ButtonTable *buttons;  uint32_t flags40;  int action;  int counter7c;
     int32_t stamp[20];           -- 0x53c2a8 slots (1 teleport, 0xa side-step, 0xe draw, 0x13 spear)
     const struct WeaponDef *weapon; */
/* Battle additions: int32_t tick; int winner; bool finisher_done, finishers_enabled; int arena; */
```

Matcher (one function, exact retail order): for a press of button `b`, walk
`set->list[b]`; for counted records compare `tokens[k]` with
`directions.token[count-1-k]` after resolving BACK/FORWARD by
`opponent.x < self.x`; reject without `CMD_BLOCK_OK` if BLK is down; if
`now - directions.tick[count-1] > window` **return no match**; reject when
airborne unless `CMD_AIR` or `flags40 & 0x200`; **return no match** if
`flags40 & (1|4)`; call the predicate. On a match, interrupt the fighter with
`(flags, action)`; otherwise call `fighter->buttons->press[b]`. Push the
button token after this, and the direction token for Up after its list;
other directions push only. Per-character data: an 18-entry `CommandSet`
array mirrors `0x53c888[c]`.

Engine requirements this implies:

1. 6-button + 4 absolute-direction input with the two-sample debounce,
   per-player 8-entry direction history with tick stamps, press latches.
2. `CommandSet` per character + predicate functions; action tables T1/T2/T4/
   T8/T20 as const `FighterFn` arrays indexed by action (shared, with
   character forks inside entries).
3. Fighter fields: `flags40` bits 0x1/0x2/0x4/0x8/0x200, retail action code,
   `counter7c`, stamp slots, held-weapon descriptor (button table at word 3).
4. Battle fields: global tick, distance in Q16 (retail value at `0x53019c`),
   round winner / finisher flags / arena id.
5. Stance-owned button tables, swapped exactly at the setters in §6;
   begin-action installs the empty table.
6. Interrupt semantics: a matched or normal press abandons the current move
   (continuation stack reset) and starts the entry on the next tick.

## 9. Oracle

```sh
PYTHONPATH=/tmp/mk4-oracle-deps python3 tests/verify_scorpion_input.py          # compare
PYTHONPATH=/tmp/mk4-oracle-deps python3 tests/verify_scorpion_input.py --record # regenerate
```

Each tick it writes the two P1 raw switch words (active low), runs the
original `0x45c560`, increments `0x534858`, runs `0x45c7c0` and the original
pump `0x41f570`; handler tasks are real pool tasks. Initialisation runs the
original `0x45c880`. After input, the fighter's new callback runs in the
fighter context until an action-table entry is reached, then until that
entry's first write of task +0x74 (budget 60k instructions).

Seeded state (`world()`): fighter tasks in pool slots `0x538728`/`0x538810`
(not linked into the pump), groups at `0x1001000`/`0x1003000` with x
(+0x54), y (+0x58), floor (+0x48 = 0), flags40, player number; character 0
for both; opponent action/counter; distance `0x53019c`; winner/finisher
byte/arena; stamp slots; object list head `0x5301b0` (one object); button
table pointer `0x530228`; mode 4; both players human. Airborne = y −3.0.

Stubs: begin-action `0x45f870`, `0x46a180` (pick-up side effect),
pose `0x407920`, sounds `0x489f00/0x489f30/0x489f90/0x489fc0/0x489ff0`.

Scenarios (191): window boundaries, wrong order, extra directions, buttons in
between, diagonal roll, wrong button, block held, airborne, busy, facing left,
each predicate threshold, debounce retap, list abort, run double tap, weapon
pick-up; then every button in 11 button tables, close/back/toward variants,
distance thresholds and opponent states. Each resolution has
`status` = `action_written`, `returned_without_action` or
`unresolved_dependency`; only `action_written` rows support an action code.

## 10. Unresolved

* Fighter dispatch latency after an interrupt (§3), not measured: the
  fighter tasks are not pumped.
* PC key/joystick mapping into `0x4d00a4/0x4d00a8/0x4d00b0` (`0x4b4a40..`).
* Meaning of opponent task +0x7c (`0x47bac4`, `0x483af0`, `0x483b20`) and of
  flags40 bits `0x2`, `0x4`, `0x8` (setters listed, semantics HYPOTHESIS).
* Action codes not observed: fatality entries `0x467cd0/0x467ce0/0x467e00/
  0x467e70`, pick-up `0x46a020`, side-step `0x469250`, follow-up HP
  `0x46c290`, armed HP/LP (`0x496c20`, `0x496c30`, follow-up weapon entries
  reaching `0x481010`), walk/run loops `0x460570/0x460710/0x4609f0`.
* Throw gate helper `0x48ebc0` (parameter program `0x4ec2b8` run by
  `0x48fd00`) and option word `[0x524f00]` bit 0 are executed but not
  interpreted.
* Exact phase timing at which follow-up tables are installed and removed
  (`0x46bb60`, `0x46bf10`, `0x46c2c0`, `0x46cba0`, `0x46e7c0`), and where
  `0x46cba0` is entered from.
* Uppercut `0x46ccd0` calls `0x45e5e0` and `0x467ce0` (finisher path inside a
  normal move) — not explored.
* P2 handlers were read, not executed; CPU/drone input (`0x433c00`) not covered.
* Fighter distance `0x53019c` is injected; its computation at `0x489220..0x4892b0`
  is not verified here.
