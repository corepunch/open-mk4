# Porting a character's scripts to C

Start with `python3 tools/disassemble_retail.py --list-moves` to check the
available research roots. **The compact move catalog currently covers Scorpion
only.** `--move spear` is an example of navigating his evidence, not a starting
point for Sub-Zero. There is no `--character` option, and `--script` follows
slot 0 (Scorpion) whenever it encounters a character selector, even inside an
otherwise Sub-Zero-specific query. See the Sub-Zero entry points below before
translating any character-selected path.

Follow bounded `--script`, `--native` and `--attack` queries for details; see
[script-decoder.md](script-decoder.md). Use the existing
`build/retail-disassembly/` bundle for broader navigation. Regenerate with
`python3 tools/disassemble_retail.py --bundle --output build/retail-disassembly` only
when its inputs or decoder change. This is static decoding, not retail execution.

1. Start at the character's command table and stance dispatch tables. List each
   command, input window, predicate and entry. Include standing, crouching,
   jumping, armed and follow-up button tables; specials alone are not a character.
2. Open each decoded script. Translate clip selection, cursor changes, waits,
   contact queries and conditional branches into that character's C file.
   Use `MoveSequence` for fixed playback and `Fighter.script` for native loops.
   `sample_override` is the explicit animation cursor for those loops.
3. Follow calls and spawned tasks into `native/`. Use the included decompilation
   as an explanatory reference, checking revision-dependent constants against
   the supplied executable. `tools/retail_moves.py` and `tools/scorpion_moves.py`
   expose helpers for reading additional tables without running the game;
   their command inventory is not character-generic. Unknown operations are
   work to resolve, not permission to treat them as no-ops. A report with zero
   unknown opcodes can still have unresolved native behavior.
4. Preserve gameplay data: startup/active/recovery, damage and chip, launch/pull
   velocity, stun, cooldowns, range, command gates and allowed cancels. Name any
   intentional engine choice in the coverage notes. Do not reproduce scratch
   globals, packed pointers, CPU registers or cooperative-task implementation.
5. Add only the missing shared primitive that the current move needs: movement,
   an attack record, projectile, reaction or paired-fighter continuation. Keep
   character decisions in `game/c_<name>.c`, and rendering read-only.
6. Test our C behavior with a few meaningful scenarios: command acceptance and
   rejection, hit/block/miss, interrupted recovery and cleanup, both player
   sides, and relevant combo/cancel branches. Avoid raw per-tick dumps or new
   retail-emulation harnesses. Keep existing asset/decoder regression coverage.
7. Run `make format`, the relevant native checks, and required repository checks.
   Commit the coherent, verified step on main before starting the next one.

## Sub-Zero: implementation baseline

The current implementation is [game/c_sub_zero.c](../game/c_sub_zero.c).
Retail character ID, roster index and asset index are all **4** here; do not
assume these three indices will always coincide for other roster entries.

| Area | Current behavior | Work still needed |
| --- | --- | --- |
| High kick | Clip 71, sparse playback, control returns at step 29 on the uninterrupted no-contact path | Record-based contacts and verified contact-dependent branches; current damage, stun, reach and push are approximate |
| Other normals, throws and reactions | Generic move definitions and approximate combat | Follow the relevant shared entry and every Sub-Zero selector; recover branches, records and victim continuations |
| Controls | `mk4_controls`, with zero special-command records and approximate stance tables | Character-owned `Controls`, commands, stance and follow-up tables |
| Movement | Shared `mk4_motion`; walk/run are prototype values | Preserve the existing engine choice until the relevant retail movement path is researched |
| Specials, weapon combat and finishers | No Sub-Zero-specific implementations | Translate the entries below; existing Scorpion coverage does not cover Sub-Zero |

Keep the existing high-kick sample fixture in
[tests/fixtures/original_kicks.inc](../tests/fixtures/original_kicks.inc).
`check_original_kicks` in [tests/test_battle.c](../tests/test_battle.c) exercises
sample selection, next-tick entry, return to movement and interruption. It
does not establish complete retail contact behavior. Older architecture and
research notes describe earlier checkpoints; source and this coverage table
define the starting point for this port.

## Sub-Zero: locating commands and scripts

All addresses below were checked against `MK4/MK-4.EXE`, SHA-256
`4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d`.
Use `RetailImage` from [tools/retail_moves.py](../tools/retail_moves.py): it
checks that revision and applies startup pointer fixups. Some pointer slots
are zero in the file before fixups. Command-list and script-selector pointers
are packed word indices: multiply them by four. Native predicate/action
entries are byte addresses; do not multiply those by four.

### Command lists

These are the **already indexed character-4 slots**, not table bases. Both
players' slots resolve to the same list in this build. Records are three
words (`pattern`, `extra`, `command`), 12 bytes apart; a zero pattern ends the
list. Preserve record order, including unconditional records.

| Event | P1 slot | P2 slot | Decoded list root |
| --- | --- | --- | --- |
| Up | `0x53c898` | `0x53cb20` | `0x53cd10` (empty) |
| HP | `0x53c8e0` | `0x53cb68` | `0x4e4b18` |
| LP | `0x53c928` | `0x53cbb0` | `0x4e4b38` |
| BLK | `0x53c970` | `0x53cbf8` | `0x4e4b60` |
| HK | `0x53c9b8` | `0x53cc40` | `0x4e4b80` |
| LK | `0x53ca00` | `0x53cc88` | `0x4e4ba0` |
| RUN | `0x53cad8` | `0x53ccd0` | `0x4e4bc0` |
| LK release | `0x53ca48` | `0x53ca90` | `0x53cd10` (empty) |

Do not reuse `tools/scorpion_moves.py:commands` unchanged: it reads character
0 and its P2 Up base is actually the LK-release table. The coincident empty
Scorpion lists conceal that error. The correct P2 Up base is `0x53cb10`.
See [input and commands](scorpion/input-and-commands.md), sections 1–3, for
the switch map, record layout and matching rules.

The character-4 records are below, in order within each button list.
Directions are earliest first, relative to the opponent. Flags are the raw
retail flags, not a C action ID. Windows are inclusive tick limits. These
are navigation roots, not claims that the predicates or whole moves have
been translated.

| Record | Button | Pattern | Window | Flags | Predicate | Native entry / research role |
| --- | --- | --- | --- | --- | --- | --- |
| `0x4e4b18` | HP | F,B,F,D | 60 | `0x41` | `0x45e460` | `0x467ce0`, fatality 1 dispatcher |
| `0x4e4b24` | HP | B,B,D,B | 50 | `0x01` | `0x45e530` | `0x467cd0`, fatality 2 dispatcher |
| `0x4e4b38` | LP | Unconditional | 0 | `0x04` | `0x45f270` | `0x47dc00`, slide |
| `0x4e4b44` | LP | D,F | 26 | `0x02` | `0x45f1f0` | `0x49bc60`, freeze projectile |
| `0x4e4b50` | LP | D,B | 18 | `0x14` | `0x45ece0` | `0x47f4d0`, ice clone |
| `0x4e4b60` | BLK | Unconditional | 0 | `0x04` | `0x45f270` | `0x47dc00`, slide |
| `0x4e4b6c` | BLK | F,B,F,D | 60 | `0x41` | `0x45e460` | `0x467ce0`, fatality 1 dispatcher |
| `0x4e4b80` | HK | D,U,U,U | 48 | `0x01` | `0x45e5a0` | `0x467e00`, fan stage fatality |
| `0x4e4b8c` | HK | D,F | 24 | `0x01` | `0x45ef50` | `0x46d1d0`, draw/armed weapon branch |
| `0x4e4ba0` | LK | Unconditional | 0 | `0x04` | `0x45f270` | `0x47dc00`, slide |
| `0x4e4bac` | LK | D,D,D | 40 | `0x01` | `0x45e4f0` | `0x467e70`, spikes stage fatality |
| `0x4e4bc0` | RUN | Unconditional | 0 | `0x01` | `0x45e820` | `0x469250`, shared RUN double-tap path |
| `0x4e4bcc` | RUN | Unconditional | 0 | `0x01` | `0x45efc0` | `0x46a020`, weapon pickup |
| `0x4e4bd8` | RUN | F,B,F,D | 60 | `0x41` | `0x45e460` | `0x467ce0`, fatality 1 dispatcher |

For a bounded, read-only check of any list, run this from the workspace root
and substitute a slot from the table above. It prints raw records without
creating a bundle or running the EXE:

```sh
python3 -B - <<'PY'
import sys
sys.path.insert(0, 'tools')
from retail_moves import RetailImage
image = RetailImage('MK4/MK-4.EXE')
pc = image.word(0x53c928) * 4  # Sub-Zero P1 LP slot
for _ in range(32):
    words = [image.word(pc + 4 * k) for k in range(3)]
    if words[0] == 0:
        break
    print(f'{pc:08x}:', *(f'{word:08x}' for word in words))
    pc += 12
else:
    raise ValueError('List exceeds this query bound')
PY
```

Decode the third word as `gate<<24 | flags<<16 | window<<8 | action`.
Pattern bytes are consumed low byte first: `AA` back, `DD` forward, `02`
down, `01` up; `FFFFFFFF` is unconditional. Predicate pointers are at
`0x4e4ec0 + gate*4`. The first set dispatch flag in order `01,02,04,08,20`
selects the action table at `0x4e6ef8`, `0x4ed828`, `0x4e8910`, `0x4e7008`
or `0x4dfc90`; read its byte-address entry at `table + action*4`.

The slide rows have `FFFFFFFF` in both first words. **Unconditional means
no direction-history pattern, not no gate.** Predicate `0x45f270` checks
cooldown slot 2 against `0x40` ticks, then calls `0x45f2b0` with held-switch
data at `0x4e4248`. For a human, its P1 expected/mask pairs are
`(0x20,0x71)` for switch word A4 and `(0x03,0x07)` for A8; P2 uses
`(0x2000,0x7100)` and `(0x30,0x70)`. This requires BLK+LP+LK held and
rejects Up, HP, HK or RUN; these masks impose no back/forward requirement.
Follow `--native 0x45f270`, `--native 0x45f2b0` and `--native 0x45f3e0`
before translating the predicate. Preserve its priority over later LP/BLK/LK
records. Do not invent a direction sequence for it or drop it as an empty
pattern. The C matcher already supports count-zero records with predicates.

### Normal, air and weapon selection

Shared stance tables are standing `0x4e5058`, crouching `0x4e5070`, straight
jump `0x4e6ec8` and diagonal jump `0x4e70d8`; the locked table is
`0x53d370`. Follow-up tables include `0x4e7078`, `0x4e7090`, `0x4e70a8`
and `0x4e70c0`. Start with section 6 of
[input and commands](scorpion/input-and-commands.md), then check each reached
entry for character selection. Shared entry addresses do not imply shared
animation, contact selector or recovery.

At VM opcode `0x3c`, read the packed pointer at `opcode_address + 4 + 4*4`
for Sub-Zero, then query that selected script directly. Repeat this at any
nested selector: the current decoder always follows slot 0. Its report title
still says "Scorpion selection" for direct scripts with no selector.

| Path | Selector script | Sub-Zero selected script |
| --- | --- | --- |
| High kick | `0x4e5ca0` | `0x4e5c80` (Scorpion instead selects `0x4e6c90`) |
| Low kick | `0x4e5d90` | `0x4e6da0` |
| Sweep | `0x4e6798` | `0x4e6e68` |
| Roundhouse | `0x4e6800` | `0x4e6840` |
| Crouching HK | `0x4e6020` | `0x4e5fd8` |
| Straight air kick | `0x4e61f8` | `0x4e6238` |

Native selectors need the same care. Follow-up HP at `0x46c210` reads the
native table `0x4e6c50`: slot 4 is `0x46c2a0`, which starts `0x4e69c8`
(clip 532), rather than Scorpion's `0x46c290` / `0x4e69b0` path. For weapon
research, start at `0x46d1d0` and the character-4 packed descriptor slot
`0x53d5d0`, which resolves to `0x4ecdb8`. Follow the descriptor's armed
tables; do not copy Scorpion's sword descriptor, contact records or spin.

## Worked example: Sub-Zero's high kick

This is a source-to-engine map for extending the existing move, not a new
implementation or a claim that its contact branches are complete.

1. **Selection.** Standing HK without a matching special falls through the
   stance handler `0x461030`. Its far-kick branch reaches shared native entry
   `0x46a680`, which writes action `0x100` and enters `0x4e5ca0`. Resolve
   character 4 to `0x4e5c80`; querying only `--move high_kick` would show the
   Scorpion branch and clip 66 instead.
2. **Playback and continuation.** Sub-Zero's script sets selector 2, opens
   clip 71, and tails to `0x46a790`. That native calls `0x470c60` with data
   at `0x4e6978`: selector 2, query B `0x48d3f0`, sample operands 10/11/12,
   sentinel `FFFFFFFF`, continuation `0x46a7a0`. The continuation enters
   `0x4e6998`, changes action to `0x1006`, advances two samples, releases
   attack bookkeeping, finishes forward and returns to the controller.
3. **Contact.** `--attack B 2` resolves record `0x4ebaf8`: Q16 rectangle
   `(0x116c8,-0xde76,0x1476c,0x12395)`, damage `0x2147` (about 13%),
   hit/air/block reactions `16/13/6`, flags 0. These are different data from
   the current approximate `MoveDef`; do not just change its integer damage
   field and keep the approximate collision interval.
4. **C mapping.** Retain the existing `kick_segments`, 29-step no-contact
   sequence and end callback. A future port can add `MoveEvent` contacts
   through `battle_contact` and `SF_RECORD` to exclude the approximate
   resolver. Read `0x470c60` and `0x470dc0` before deciding retry, crouch and
   connected behavior. Branch-dependent playback belongs in explicit C
   continuations where a fixed sequence is insufficient. Event times are
   move ticks; samples 10/11/12 are not automatically event ticks. Existing
   no-contact evidence places queries at steps 11–13 and forward recovery
   through sample 27, returning at step 29.
5. **Acceptance.** Preserve the existing no-contact sample/return fixture;
   test no damage before the active window, one connected attack's damage,
   block chip/reaction, air reaction, miss recovery, interruption and both
   player slots. Derive any changed contact-recovery expectations from the
   native branches, not from the no-contact fixture.

Use these small queries, following each printed dependency as needed:

```sh
python3 tools/disassemble_retail.py --native 0x461030 --limit 100
python3 tools/disassemble_retail.py --native 0x46a680
python3 tools/disassemble_retail.py --script 0x4e5c80
python3 tools/disassemble_retail.py --native 0x46a790
python3 tools/disassemble_retail.py --native 0x470c60 --limit 120
python3 tools/disassemble_retail.py --native 0x470dc0
python3 tools/disassemble_retail.py --native 0x46a7a0
python3 tools/disassemble_retail.py --script 0x4e6998
python3 tools/disassemble_retail.py --attack B 2
```

## Sub-Zero: specials and engine integration

These are confirmed starting dependencies. Every row still needs semantic
research and C implementation; a decoded stream is not a completed move.

| Move | Bounded research path | Known boundary / work to resolve |
| --- | --- | --- |
| Freeze projectile | `--native 0x49bc60` → `--script 0x4ed588` → spawned native `0x49bcb0` | Attacker clip 16; task parameters `0x4ed5d0`, contact-loop block `0x4ed5e0`; resolve conditional startup, flight, lifetime, contact and teardown |
| Ice clone | `--native 0x47f4d0 --limit 220` → callback `0x47f9d0`, then `0x47fa90` / `0x47fbd0` | Entry compares distance against Q16 `0x10000` and branches; resolve near/air behavior, stationary object, lifetime and freeze contact |
| Slide | `--native 0x47dc00` → `--script 0x4e84d8` → native `0x47dc40` | Clip 95 and initial speed Q16 `0x2666`; follow contact continuation `0x47de80` and recovery `0x47de00` for hit/block/miss behavior |
| Freeze victim | Reaction 67 → `--native 0x486230` via reaction table `0x4ea4e8` | Determine freeze duration, pose/physics, thaw, damage/scaling, repeated-freeze rules and replacement by another reaction |
| Finishers | Command predicates above → `0x467ce0` / `0x467cd0` → scenes `0x43cbb0` / `0x43bd70` | Dispatchers are shared; follow character 4 into its own scenes and victim programs. Scorpion scene addresses and range gates are not substitutes |

Contact tables C and D are not exposed as named `--attack` selectors by the
CLI. Resolve their packed record pointers with `RetailImage`, then use
`--attack record ADDRESS`. Do not pass the pointer-table address as a record.
The projectile path `0x49c690` → `0x49c810` → query C (`0x48d410`) uses
selector 3 from `0x4ed5e0`; table `0x53d208[3]` points to `0x4ebf18`.
Clone query D (`0x48d450`) uses selector 5 at `0x47fc0f`;
`0x53d178[5]` points to `0x4ebf30`. Both records have zero direct damage,
MARK flag 2 and hit/air reaction 67. Slide uses D selector 7, record
`0x4ec1b8`, damage `0x147a` (about 8%), reactions `122/122/165`.
Inspect the reactions as well as the records:

```sh
python3 tools/disassemble_retail.py --attack record 0x4ebf18
python3 tools/disassemble_retail.py --attack record 0x4ebf30
python3 tools/disassemble_retail.py --attack record 0x4ec1b8
python3 tools/disassemble_retail.py --native 0x486230 --limit 120
python3 tools/disassemble_retail.py --native 0x483500 --limit 100
python3 tools/disassemble_retail.py --native 0x486d00 --limit 100
```

Use the current procedural interfaces; add a shared feature only when the
move's evidence requires it:

| Boundary | Existing interface and lifecycle | Sub-Zero responsibility |
| --- | --- | --- |
| Commands | `game/commands.c`: ordered `CommandDef` records, predicates, then stance fallback; entry is pending until the next tick | Own `Controls` in `c_sub_zero.c`; preserve unconditional slide priority, held-switch predicates, windows and air/block permission |
| Playback | `fighter_start_move` resets script, phase/timer, sample override, pending entry and contact bookkeeping; locked moves remove stance buttons | Install the continuation after starting the move; distinguish ticks from samples and restore control on each exit |
| Contacts | `battle_contact` / `battle_contact_from` apply records, damage and reactions; `SF_RECORD` skips the approximate resolver | Use projectile/object origin where appropriate; prevent repeated damage or freeze from one connected attack; retain our documented hull approximation |
| Projectiles/objects | `battle_spawn_projectile` returns a slot or NULL; four slots, owner index, callback and bounded lifetime; fight ticks update objects after fighters | Existing kinds/drawing cover spear and sword only. Add the necessary freeze/clone state and read-only presentation; handle full pool, expiry and owner interruption explicitly |
| Victim reactions | `battle_react` clears physics and schedules a replacement continuation; unsupported IDs fall back to approximate hit/knockdown | Reactions 67, 122 and 165 currently have no explicit handlers. Translate the required reactions; do not accept the fallback as freeze or slide coverage |
| Cleanup | Round spawn resets fighters/objects; round completion and finisher start clear objects | Move replacement does not automatically remove owned objects or release another fighter. Specify whether each object survives an interruption from evidence; clear locks, appearance and callbacks on thaw, replacement, KO and reset as appropriate |

The common update is in [game/battle.c](../game/battle.c), move lifecycle in
[game/moves.c](../game/moves.c), contact/object allocation in
[game/combat.c](../game/combat.c), and reactions in
[game/reactions.c](../game/reactions.c). `Fighter` currently has Scorpion
cooldown fields `spear_ready` and `teleport_ready`, but no Sub-Zero cooldown
or frozen-state model. Choose explicit per-fighter/object state as the move
needs it; do not repurpose those fields or introduce retail scratch globals.
Scorpion's code is an example of these APIs, not proof that its constants,
weapon behavior, reactions or cleanup apply to Sub-Zero.

## Sub-Zero: acceptance and implementation order

For every ported move, record its command/stance entry, selected scripts,
native dependencies, contact/reaction records, phase timing, intentional
engine choices, tests and unresolved gameplay questions in its coverage notes.
Keep unresolved moves labeled as approximate or unimplemented. Specifically
resolve these questions before calling freeze/clone complete:

- How long is the victim frozen, what pose and physics apply on ground/in
  air, and what restores input and normal reactions after thaw?
- Does a second freeze reject, refresh, release or otherwise affect a frozen
  victim? How do normal hits, throws, scaling and combo counters interact?
- What are the projectile/clone cooldowns, active-object gates, distance
  boundaries and air permissions? Predicate `0x45ece0` compares cooldown
  slot 6 with `0xb4` ticks but has additional checks; this is not its whole gate.
- Which objects survive owner damage, and what happens when a victim reaction
  is replaced, a fighter is KO'd, a finish window opens or a round resets?

Add focused scenarios to [tests/test_battle.c](../tests/test_battle.c).
Use the actual input history path for command tests, not only direct entry
calls. Cover accepted input and wrong order, extra direction, late window,
held block, busy/reacting, cooldown and grounded/airborne rejection where
applicable. Check a window/cooldown immediately below, at and above its boundary.
For slide, test the held chord and rejected extra buttons on LP, BLK and LK
presses, with two released samples before a repeated press. Test both player
slots and facing directions, command priority and stance fallback.

For gameplay, cover hit, block, miss, airborne victim, one-shot contact,
recovery and interruption. Freeze needs thaw and attack/reaction replacement
cases; clone needs near-distance boundary, contact, expiry and coexistence
rules; slide needs travel/contact/recoil/recovery branches. Exercise object
pool exhaustion, owner interruption, KO, next round, rematch and a Sub-Zero
mirror match so state cannot leak between owners. Assert the researched
outcomes; the questions above are not invented retail rules.

A practical sequence of coherent implementation steps is: (1) character
controls and normal/air/throw branches, retaining high-kick playback;
(2) the freeze reaction with the projectile that needs it; (3) clone;
(4) slide; (5) weapon paths and finishers. Split further when useful, with
relevant tests and coverage notes in each step. These are future work, not
changes made by this guide.

For each implementation step, run `make format`, then the focused native
battle test (`make build/test-battle` and `./build/test-battle`), followed by
`make check` before committing. The latter includes format, native game/asset,
frontend, archive, metadata and decoder checks. For a documentation-only
update, verify the listed slots/records against `RetailImage`, run the quoted
queries, check Markdown links and run formatting/required repository checks.
Reuse the saved kick fixture and `tests/test_retail_moves.py` /
`tests/test_disassembly.py`; do not add a retail-execution harness, regenerate
bulk bundles or record new per-tick dumps for this character port.

## Scorpion coverage

The working implementation is `game/c_scorpion.c` (the existing filename).

| Area | Current implementation |
| --- | --- |
| Jab, high kick, sweep, uppercut | Existing playback/branches plus script attack records |
| Spear | B,B,LP, startup alternative, projectile, chip, pull, 64-tick stun, 60% combo scaling, cooldown and interruption cleanup |
| Teleport | D,B,HP, air permission, cooldown/hit-count gate, vanish, cross-over, hit hang/drop, block recoil/landing, miss recovery |
| Fire | D,F,LP, normal/follow-up playback, one active query, 12% damage, recovery |
| Sword | Draw/follow-up draw, HP/LP and back slashes, held/steered spin with recoil, armed air punches, throw, disarm, resting pickup and round reset |
| Air normals | HP/LP, straight and diagonal kicks, extend/hold/retract, contact freezes, HP-to-LP/kick follow-ups and landing |
| Ground normals/combos | Far HP repetition, close HP/HK and their distinct cancel tables, combo HP, LK, crouching punches/kicks and roundhouse |
| Throws | Paired LP/LK throws, air throw, damage/release phases and victim recovery |
| Victim reactions | Explicit record-selected hitstun, combo scaling, crouch branches, launches, tumbles and getups |
| Fatalities | Both character fatalities and both stage fatalities; winner/range/stage gates, paired phases, creature asset and match completion |

Spear sources: scripts `0x4ed448`, `0x4ed4a8`; native `0x49a6a0`,
`0x49b170`, `0x487350`, `0x487420`; contact record `0x4ebeb8`.
Teleport: scripts `0x4e87b0`, `0x4e8818`; helpers `0x480040`, `0x47fd90`,
`0x47fec0`; record `0x4ebf48`. Fire: `0x4e5b58`, `0x468640`, `0x4689b0`;
record `0x4ebf90`.

Intentional engine choices: fixed standing/crouching collision hulls; a ten-tick
teleport vanish replacing the retail camera-bearing trigger, with distance and
wall exits retained; simple rope/flame/blade rendering. Sword drops use a single settling arc rather
than retail cosmetic bounces; thrown swords have a six-unit flight budget. Spear pull uses explicit C
state instead of a rope-node task chain. Audio, original camera cuts and particle/gore
rendering are not implemented. Finisher presentation uses our fixed camera, flame
lines, darkened burn victim and stage removal/impalement. Script coverage does not
claim identical retail rendering, collision hulls or scheduler timing.

Air attack sources: scripts `0x4e6658`, `0x4e6710`, `0x4e61f8`, `0x4e6500`,
`0x4e63d8`; native contact continuations `0x46e7c0`, `0x46cb10`, `0x46ef10`,
`0x46e6b0`, `0x46e230`. Low HP/straight-kick contacts expose a ground follow-up
table; HP uses `0x46c3f0` to permit close HP against a grounded victim already in
hitstun. Sword back-punch variants capture direction when the button is pressed.
Diagonal
jump classification uses horizontal motion in our current jump controller.

Air throw now follows scripts `0x4e5e28/0x4e5e50`: paired clips 82/202,
28-tick capture, 13% release damage, separate falls and recovery. Attachment
only suppresses body separation; interruption releases the victim. Getup uses
clip 131:46–79 and returns control instead of reproducing retail task parking.

Scorpion's normal, air, sword, teleport and fire contact records now select
explicit shared reaction phases in `game/reactions.c`, including crouch branches,
no-guard combo windows, scaling, friction, launches, landing and getup. Sources
are the reaction table at `0x4ea4e8`, each entry's targeted native query, and its
referenced script. The port expresses tumbles along the gameplay pair axis;
camera roll, blood/sound helpers and retail wall-contact task dispatch are not
recreated. The world still uses our gameplay hulls. Goro retains his separate
animation-bank fallback. Focused tests exercise reaction recovery from ground
and air, combo flags/scaling, crouch selection, and replacing an active reaction.

Ground LP and LK throws now use paired clips 75/152 and 37/157, respectively.
Both deal their script damage at sample 42 (15% LP, 18% LK). LP releases into
reaction 100; LK retains the pair through sample 65, then plays the victim's
586:65–130 continuation. Root alignment uses our paired world origin. The
retail CPU probability-based throw reversal is not a human command branch and
is left to a future CPU policy.

The match now opens a winner-only finish window after the deciding KO. It uses
our explicit five-second window, freezes the round timer, allows positioning and
command history, and holds the loser in clip 13/action 0x1009. Timeout wins skip
this window. Starting a finisher locks both controllers and removes projectiles;
completion awards no extra round. Arena selection is now available to simulation
for the two stage-fatality predicates.


## Scorpion finisher commands and sources

After the deciding KO, enter a command during the five-second finish window.
Directions are relative to the opponent; press the attack with the last direction.

| Finisher | Command | Gate | Script/native sources |
| --- | --- | --- | --- |
| Burn | B,F,F,B + BLOCK | 1.8–3 units; 50 ticks | `467ce0`, `443840`, owner `4e0278..2f0`, victim `4e0300..35c` |
| Creature | B,F,D,U + HP | At most 1 unit; 50 ticks | `41e960`, `4d2e58/88/b0`, victim `4d2ef8/2f10` |
| Fan | F,D,D + LK | Prison (8), at most 1 unit; 40 ticks | `43d040`, `4e0118..1a8`, 31 repeats initialized at `43d3d0` |
| Spikes | B,F,F + LK | Goro's Lair (0), at most 1 unit; 40 ticks | `467e70`, `4e6950`, reaction 175 `4ea300/330` |

The creature uses `sc_fat2_.geo`, slot `50c2e8`, rig `50cb28`, clips 578–581.
Its animated IDs start at 1 (the human rigs start at 2), and its native root
placement is Y=-0.9. Victim clips 584/585 stay on the defeated character's rig.
Finite scene completion replaces retail parked tasks. Blends and stage camera
travel are presentation choices; the owner/victim clips, repeated bite phases,
fan repetitions and spike wait phases come from the listed scripts. The match
result retains the final poses and clears every finisher field on rematch.
