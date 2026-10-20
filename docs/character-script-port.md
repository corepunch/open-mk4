# Direct retail execution migration

All 70 reviewed Scorpion roots now run original script words in `game/retail_vm.c`;
Scorpion has no generated script or frame include. Its native adapters remain C.
Do not re-express script control flow as C or frame programs. Keep native calls
in procedural C and reuse existing helpers; C module boundaries may shrink as
scripts replace their generated bodies. The historical compiler workflow below
still applies to fighters that have not migrated. The strict coverage gate now
uses `--require-implemented`, accepting reviewed interpreted or generated roots
while rejecting duplicate, legacy and unreviewed implementations. See
[the runtime evidence and migration ledger](research/retail-vm.md).

# Historical compiler workflow

Retail scripts and their native continuations are the specification. Generate
VM control flow with `tools/compile_retail.py`; preserve commands, branches,
animation phases, damage, reactions and recovery in procedural C. Reproducing the retail VM, scheduler, scratch globals or
per-tick binary traces is unnecessary. Game simulation remains C11 without SDL.

The active simulation is `g_battle`, defined in `game/battle.c`.
Move/physics/event/end callbacks take `Fighter *`; projectile callbacks take
`Projectile *`; predicates take `const Fighter *`. They access shared match
state through the global instead of receiving a `Battle *`. The host calls
`battle_init(p1, p2)`, binds borrowed assets with `battle_bind_assets(assets)`,
then steps `battle_step(input)`. Tests reset the singleton for each scenario.
This is one active simulation, not a reentrant multi-battle API. SDL and
asset ownership remain with the host; drawing only reads simulation state.

## Workflow

Port one coherent move or move family at a time. Finish its reached branches,
owner and victim recovery, and relevant tests; commit that step on `main`
before starting the next. A roster entry, playable animation or successful
`--port` query alone does not establish that a fighter is ported.

Use the deterministic compiler for script bodies. Do not manually translate
supported VM instructions into handwritten per-tick cases, duration tables or
AI-generated C. Extend a missing lowering once, or bind a researched native
boundary to an existing engine helper. Handwritten C owns native behavior and
entry/lifetime adaptations, not a second transcription of the script graph.

The complete tool contract and profile format are in
[script-compiler.md](script-compiler.md). The complete migrations cover Liu Kang (57 roots), Scorpion (70) and
Sub-Zero (63), including followed native continuations, with zero legacy entries.
Every profile must satisfy `--require-generated`; Make has no migration bypass.

### 1. Inventory the fighter and choose a move

Read [ARCHITECTURE.md](../ARCHITECTURE.md), then inspect the existing
`game/<character>/character.c`, its `CharacterDef`, and `game/roster.c`. Separate shared
locomotion/reactions from character decisions. Inventory standing and crouching
attacks, close attacks/combos, air attacks, both throws, specials, weapons,
finishers and CPU entries. Mark each as researched, implemented, tested or
unresolved; existing fallback moves are not evidence of retail behavior.

Start with the typed inventory and coverage audit:

```sh
python3 tools/compile_retail.py --character liu_kang --inventory --output build/liu-inventory.json
python3 tools/compile_retail.py --character liu_kang --audit --summary
```

The inventory includes both players' press/release command records, four CPU
choice rows, stance/follow-up tables, the default weapon descriptor and buttons,
contact reactions and the selected catalog's decoded script/native frontiers.
Do not infer completeness from `--list-moves`: it is a curated navigation index.
Add missing research roots to `tools/character_catalog.py` and implementation
references to a reviewed `game/<character>/port.json` profile. Review every
new input/CPU target, reaction, and inventory fingerprint change. The audit
rejects dropped release tables, missing definitions, unreviewed VM roots,
changed reaction dispatch and stale generated output.

Use the bounded move view for names and the audit's unresolved boundaries:

```sh
python3 tools/disassemble_retail.py --character sub_zero --list-moves
python3 tools/disassemble_retail.py --character sub_zero --move lk_throw --c
```

`--character` accepts roster names or IDs 0..17. Keep it on follow-up script
queries. Kitana uses Sonya's script slot; Noob uses Reiko's. Other characters'
shared roots are navigation aids, not complete special-move catalogs. Verify
the selected table slot even when fighters share an entry.

### 2. Follow the move through scripts and native continuations

Read compiler diagnostics first. Reuse a known lowering or typed native binding;
research only an unbound helper or unsupported operation. Use the view that
answers that specific question. These examples are read-only;
`--output PATH` can save a query when needed.

| Query | What to use it for |
| --- | --- |
| `--character sub_zero --move lk_throw --c` | Retail C-like control flow: branches, waits, selected scripts and native exits |
| `--character sub_zero --move lk_throw --port` | Current workspace C, supporting data links, operand evidence and checked reference links |
| `--character sub_zero --script 4de490 --c` | A particular script dependency, decoded in the correct roster context |
| `--character sub_zero --script 4de490 --raw` | Exact script words alongside their decoded roles |
| `--native 42c220 --limit 100` | Bounded exact-entry x86 listing and known helper contracts |
| `--attack C 3` | Contact descriptor: damage, reaction and other decoded fields |
| `--action 1007` | Action catalog, confidence, checked writers/readers and current C references |

Prefix the query options above with `python3 tools/disassemble_retail.py`.
`--c` is research pseudocode, not compile-ready C. `--port` extracts existing
workspace implementations, not automatically translated x86; an unported
fighter needs a new implementation. Native listings and `--port` require
Capstone. The worksheet is bounded: follow its omitted-dependency queries
rather than assuming it contains the full native call graph.

Follow both conditional outcomes, character selectors, contact outcomes,
spawned objects, installed callbacks and native tails until each affected actor
returns to control, lands, remains defeated, or enters another understood move.
A script ending in a native jump has not finished its behavior. Include victim
reaction tables and recovery, not just the attacker's visible clip. Consult
existing `build/retail-disassembly/` material for navigation. Full generation
requires explicit `--bundle`; do not regenerate it for routine move research.
See [script-decoder.md](script-decoder.md) for query formats and limits.

### 3. Resolve constants and helper contracts from evidence

The supplied `MK4/MK-4.EXE` is authoritative, SHA-256
`4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d`.
`RetailImage` checks this revision and applies startup fixups. Script/descriptor
pointers are packed word indices (multiply by four); native entries are byte
addresses. Determine the table type before interpreting a value as a pointer.

Use the query's checked mk4decomp links to inspect
`reference/mk4decomp/`. Its executable revision has different addresses; a
function name or a fixed address offset is insufficient correspondence.
Verify the local entry's instructions, field accesses, branches and callees.
Embedded entry points may exist only in the reference's matching assembly,
not its reconstructed C. Keep reference checkouts unchanged and implement our
adaptation outside `reference/`. Consult upstream licenses before copying code.

For every encountered literal, establish its role from its writer and consumer:
clip/sample, count, selector, action, reaction, Q16 value, angle or pointer.
For example, controller `+0x74` is an action field, while group `+0x74` can be
motion data. Equal numbers do not make the fields interchangeable. Query
`--action 1007` before using `ACTION_RETAIL_1007`; its recovery use is evidenced,
but a complete semantic name remains uncertain. Keep neutral names where the
evidence does not support a stronger claim.

Record helper inputs, actor/object context, effects, yield/return behavior and
portable equivalent. When new research is reusable, update the existing index
alongside the implementation:

| Knowledge | Workspace location |
| --- | --- |
| Researched move entries and script roots | `tools/character_catalog.py` |
| Local helper contracts and engine equivalents | `tools/retail_helpers.py` |
| Typed compiler bindings, actor arguments, roles and native flow | `tools/retail_bindings.py` |
| Generated roots, integration and reviewed coverage boundaries | `game/<character>/port.json` |
| Typed executable data views | `tools/retail_helper_view.py` |
| Local aliases and checked reference correspondences | `tools/retail_symbols.py`, `tools/retail_symbol_map.json` |
| Action names/confidence and checked native evidence | `game/retail_actions.inc`, `tools/retail_action_evidence.json` |
| Existing portable C entry extraction | `tools/retail_port.py` |

The symbol map is checked research data; do not rebuild it for every fighter.
Extend `retail_port.py` when adding a newly supported character or move so
`--port` finds its own C entry. Preserve source addresses and bytes for new
action evidence; record unknowns instead of guessing or treating them as no-ops.

### 4. Record native boundaries and observable behavior

Generate the VM body first; its comments retain every source address and raw
word. Keep a small ledger for native helper timing, entry gates, damage, actor
ownership and interruption policy. Do not hand-copy every decoded phase into
a second timing specification. Distinguish stored animation samples from simulation
ticks: a move can skip samples, run backwards, repeat them or hold a pose while
physics and other actors continue. `fighter->ticks` can reset at a phase change;
it is not necessarily total elapsed move time.

[Sub-Zero's throws](sub-zero-throws.md) show why this matters. LK plays ten
owner-only samples, then paired counts `29 + 5 + 8`: damage occurs at elapsed
52, not at the victim's sample 52. Five more paired samples reach 57, then the
owner waits 60 ticks and returns at 117. The victim has its own cursor cycles
and clip 200, returning at 127. LP instead uses clips 76/153, damage at 25,
and reaction 102's tumble/hold/getup. Sharing Scorpion's throw runner would
lose those differences despite the shared top-level command.

Keep measured retail facts separate from portable adaptations and unresolved
presentation. A concise source-to-code ledger such as the Sub-Zero ledger below
makes the implementation reviewable without preserving a large execution trace.

### 5. Generate the script and bind native engine helpers

Preview a selected script with an explicit controller/tagged-return adaptation:

```sh
python3 tools/compile_retail.py --character liu_kang --script 4e60b8 --return-to fighter_ready --output build/crouch-hk.inc
```

Unknown operations and native boundaries are compilation errors. Register a
native boundary in `tools/retail_bindings.py` with its actual C symbol/source,
allowed role (call, tail, predicate, physics callback, independent spawn or VM
handler), typed arguments, result flags, timing/flow and local EXE evidence.
A predicate returns flags; a spawn binding declares accepted task types and
creates an independent object. The compiler never treats a callback as an
inline call or substitutes a prose helper name for a C implementation.

Noob Saibot and Kitana share a task-type-75 boundary in low-punch roots
`4ed268` and `4ed2c0`. Their callbacks select player-group contexts and
initialize the retail packed transform chain, then wait on the paired actor's
contact window. The portable `ShadowActor` stores its clip, owner/victim group,
contact phases and lifetime, and uses the render hierarchy and overlap contact
model. Kitana's upper branch also preserves its action, gravity, and landing
phases. The runtime task record itself is unavailable, so this is an explicit
portable event model rather than a reconstructed retail scheduler.

Other global callback sequences use `BattleEvent` when their task state is
not present in the VM graph. Keep phase, clip, contact, and recovery state in
the event record; do not retain retail scratch pointers or make the callback
a no-op. When descriptor data is missing, reuse the fighter's reviewed contact
record and document the approximation in its adapter.

If no existing helper fits, implement the verified native behavior once in
shared engine code or a small character adapter. Keep SDL out of simulation.
Then add the root to the generation profile and regenerate:

```sh
make scripts
make scripts-check
```

The profile declares `generated_scripts`, `return_to`, `output`, and the C
integration source. Add evidence-backed `entry_contexts` for inherited clips
and samples. A root that returns to a native caller which then enters another
generated root uses `return_to: "caller"` with a `successor` naming that root.
Bind pure script-enter native tails with `script_target` so the compiler links
their CFG directly. Extend cursor progress analysis or shared lowerings when
required; never bypass the audit with unused function references. Include its
generated `.inc` and start those functions from the existing command/move
entries. Never edit the generated include. Every root becomes a frame program:
fixed rows, named branch phases and boundary callbacks. No runtime VM,
byte-address dispatcher, scheduler emulation or new execution harness is
introduced. `game/script.c` owns shared clip primitives and stores rebound
animation data on the actor.

Native adapters build frame rows too. A blocking wait does not exist: each
wait is a held row whose end function is the continuation.

The external move entry validates its gates, calls `fighter_start_move`,
initializes phase data and pose, sets any action cleanup in `mmove_cleanup`, and
then enters the generated root or native rows. Entry setup runs immediately; the
first row runs on the actor's next tick. All scheduling stays on the single
simulation thread, in P1/P2 order, at 60 Hz.

| Frame helper | Purpose |
| --- | --- |
| `native_begin`, `native_frame`, `native_play` | Build bounded rows with poses and per-row actions, then install them |
| `native_hold(fighter, frames, end)` | Hold the current pose, then continue |
| `native_play_rows(fighter, sample, step, end)` | Step the pose toward a target, then continue |
| `native_finish_clip(fighter, end)` | Finish the bound clip and hold its last pose one visit |
| `native_query_rows(fighter, record, frames, step, end)` | Contact rows that leave early through `end` on contact |
| `native_await(fighter, condition, end)` | Hold the live pose until a condition (landing, height) holds |
| `fighter_finish_mmove(fighter, next)` | End the program, run its cleanup once, then enter `next` |

A row callback that changes the program (a new phase, `fighter_start_move`, or a
finish) ends that row; the runner does not run the old row further. A
continuation must always install new rows or finish, never return to the old
rows. Changing the move with `fighter_start_move` runs the current cleanup, so
detach `mmove_cleanup` around an internal rebind that must keep it. A `call`
binding runs inside a generated row that installs its own successor afterwards,
so it must not swap the phase itself; defer that through a physics hook or a
separate tail binding (Raiden's torpedo miss is an example). Use engine state
and predicates for landing/contact waits, rather than sleeping or waiting for
the drawing loop.

Scorpion's spear entry shows the pattern (see
[spear.c](../game/scorpion/moves/spear.c)):

```c
void scorpion_spear(Fighter *fighter) {
  ...
  fighter_start_move(fighter, S_PUNCH, &s_spear);
  fighter->spear_ready = g_battle.ticks + 90;
  fighter->mmove_cleanup = spear_cleanup;
  scorpion_entry_retail_script_004ed448(fighter);
}

void scorpion_script_spear_done(Fighter *fighter) {
  if (spear_not_pulling(fighter)) {
    scorpion_special_ready(fighter);
  } else {
    fighter->mmove_cleanup = spear_cleanup;
    native_await(fighter, spear_not_pulling, spear_wait_done);
  }
}
```

The generated `4ed448` waits 45 ticks after sample 30, at normal/fast elapsed
tick 15/11. Projectile callbacks can replace this program with the pull-arrival
return. Extract the complete entry, helpers and cleanup with
`--move spear --port` when studying it.

### 6. Give paired actors and objects explicit lifetimes

Define which actor owns each phase and when the victim becomes independent.
A released victim's generated or native program starts immediately, with its
first row on its next actor tick; a peer may still receive its regular pump
that battle tick. Do not infer a full displayed tick from startup order. Check
both P1 and P2 ownership. Paired pose submission and released recovery helpers
in `game/sub_zero/moves/throws.c` and `game/script.c` are examples. Cancel a
victim's capture hold with `fighter_cancel_mmove` before entering its released
program, so the hold's cleanup runs once.

Release a victim through its own roster's recovery controller. Preserve its
bind-ground Y offset when aligning it to another rig, and clear paired flags,
input restrictions, temporary presentation and callbacks at the correct boundary.
Owner interruption or victory must not erase an already released victim's fall
and getup. Conversely, interruption before the damage event must not allow a
cancelled program to run later and apply damage.

Use `mmove_cleanup` for owned temporary state on completion, replacement or
reset. It survives phase changes and runs once when the program ends: do not
start a replacement program from cleanup. Independent projectiles/statues use
object callbacks and the source's lifetime policy; they need not disappear when
their owner resumes controls. Handle full object pools, either participant's
interruption, KO and round/rematch cleanup using the shared engine boundaries.
See the API details below and [game/native.c](../game/native.c).

### 7. Wire real inputs and verify the behavior

Update the character's `MoveDef`/state entry table, ordered `CommandDef` records,
stance controls and optional `CpuMoves` as required. Preserve command priority,
button masks, inclusive windows, ground/range/cooldown gates and follow-up
branches. For a newly added roster definition, wire `game/mk4_characters.h` and
`game/roster.c`; asset identity and retail script-selector identity are distinct.
Character C files are already included by the Makefile's `game/*/character.c` wildcard.

Test through the real command or move entry, not only a directly invoked helper.
Keep the set small and choose assertions for plausible failures: a changed
command gate or important hit/miss/block branch, one-time scaled damage,
interruption and eventual owner/victim recovery. Check a clip/cursor boundary
when incorrect timing would change behavior. For paired moves, run the
attacker in both player slots and include a victim with a different rig when
alignment matters. Cover KO or reset where an independent tail could survive
incorrectly. Test shared scheduling and engine behavior once, then reuse that
coverage across fighters.

Simple arithmetic, data tables and small helpers do not automatically need
dedicated tests. Skip tests that merely repeat implementation constants or
check every intermediate tick. Expected behavior should come from the retail
evidence; a focused test is useful when it catches a meaningful bug cheaply.

For example, after changing Liu Kang generated scripts or native adapters:

```sh
make format
make scripts-check
python3 tests/test_script_codegen.py
make build/test-liu-kang build/test-battle build/test-frames
./build/test-liu-kang
./build/test-battle
./build/test-frames
python3 tests/test_retail_port.py
make check
```

Run the relevant decoder/helper/action tests when changing their research data.
Add compact EXE-backed assertions for newly decoded table layouts or constants;
use native C tests for gameplay. Do not add retail-execution harnesses, binary
scheduler emulation or large per-tick JSON traces. `tests/test_sub_zero.c` and
`tests/test_frames.c` provide existing gameplay and scheduling examples.

### 8. Record scope and commit the verified step

Before committing, require regeneration and a clean reviewed coverage audit:

```sh
python3 tools/compile_retail.py --character liu_kang --audit --strict --summary
```

For a new port, also require `--require-generated`; this rejects retained
legacy/unreviewed VM roots. `--strict` alone validates the reviewed boundaries
and permits the explicitly listed existing legacy ports. It does not claim
all native x86 dependencies are generated or prove gameplay equivalence.
The detailed JSON audit retains every unsupported instruction/helper call site.

Update the move ledger and extraction/helper indexes affected by the port.
Document unresolved native behavior and any deliberate engine adaptation,
including presentation limits. Check the diff and stage only this step's files.
Commit directly to `main` with a clear subject and a body explaining the problem,
source addresses, final behavior, approach, validation results and remaining
limitations. Finish a coherent step before moving to the next move family.

A move is complete when its command path, reached branches, damage and reactions,
owner/victim/object lifetimes, interruption and return to control are understood
and covered by relevant checks. List remaining fighter work explicitly; do not
infer complete fighter coverage from the curated move catalog.

## Shared engine features

| Need | Reuse | Important boundary |
| --- | --- | --- |
| Commands | `game/commands.c`, ordered `CommandDef` and stance tables | Preserve unconditional-record priority, two-sample presses, inclusive windows and predicate masks; entries start next tick |
| Generated VM control flow | `tools/compile_retail.py`, `game/script.c`, checked generation profiles | Fail unsupported paths before output; require regeneration and coverage gates |
| Fixed playback / waits | Generated frame programs, `native_*` rows (`game/native.c`), held rows with continuations | Rows have no duration; simulation ticks and stored pose samples are different clocks |
| Native continuations | Frame rows and named continuations; callbacks for independent engine work | Initialize the move and its `mmove_cleanup` before entering the program; detach cleanup around an internal move rebind |
| Poses | `game/pose.c`, `fighter_pose_sample`, `fighter_pose` | Bind host-owned assets with `battle_bind_assets`; capture real wrapped samples, rig, heading and root offsets |
| Contact | `battle_contact` for a fighter, `battle_contact_from` for an independent object | Object contacts return their result without consuming the owner's current attack latch |
| Geometry | Shared decoded joint poses and pair-axis projection | Asset-backed play uses the `48d750` joint interval and 8% trim; rule-only callers without assets use an explicit fallback hull |
| Motion | Q16 `Body`, `fighter_axis`, `fighter_set_speed` | X/Z heading follows the pair; angles/spin are Q16 radians, root offsets accompany group-Y changes |
| Reactions | `game/reactions.c`, `battle_react` | Preserve hit counts/scaling, freeze/backfire/thaw and interruption; unsupported roster reactions still have a fallback |
| Weapons | `game/weapons.c`, `WeaponDef`, `WeaponStrike` | Shared lifecycle and recovery; actual weapon identity travels through drop/throw/pickup, including foreign owners |
| Scene playback | `game/scene.c`, `SceneClip` | Independent actor clocks, forward/reverse/repeated clips and native-rate pose blends; no VM/task-stack recreation |
| Scene objects | `game/presentation.c` | Bounded mesh pieces, joint attachments/visibility, particles and camera state; reset on a new round |
| CPU | `game/cpu.c`, `CpuMoves` | Simulation-owned plans and direct character entries; battle-owned two-word random state, strict per-thousand gates and explicit difficulty/adaptive context; host only selects human/CPU ownership |
| Jump / landing | `mk4_jump`, `mk4_land`, `mk4_return_to_stance` | Shared flight and root-aware touchdown; landing controls and five native standing-blend weights also serve air strikes and clone retreats |
| Sound | `battle_sound`, `common/mk4_audio.c`, `game/audio.c` | Simulation queues selectors; assets decode original ESFs; SDL playback belongs only to the host |

The host owns decoded assets. Drawing reads simulation state. Four projectile
slots and bounded scene pools are explicit engine limits. A full pool must not
corrupt another move. Independent objects may outlive owner interruption;
round/finisher cleanup follows the reached source path.

### Shared move phases

`fighter_start_sequence` remains for shared engine continuations. It replaces
the current continuation without restarting the move, resets the phase tick to
zero, clears the sample override, and preserves action, contact results and
input restrictions. A hold segment `{0, sample, 0, 1}` plus `count = 45`
expresses a 45-tick wait; `end` is its C continuation. Character moves use frame
programs instead.

### Frame programs

Every character move, victim continuation and scene runs as an actor-owned frame
program: generated phases and native rows stepped once per actor tick by the
P1/P2 pump. Entry setup runs immediately; the first row runs on the actor's next
tick, so a released peer cannot run a row twice when its slot is pumped later in
the same tick. A wait is a held row whose end function is the continuation;
there are no coroutine stacks. External move replacement, queued interruption,
round reset, rematch and `battle_init` cancel the program and run its cleanup
once. Cleanup releases independent objects or paired victims and must not start
another program. All move state lives in `g_battle`.

Scorpion's spear shows the timing: normal/fast paths launch at ticks 10/8, reach
sample 30 at 15/11, then wait 45 ticks; the live-pull predicate may extend that
hold. Projectile contacts and movement keep their independent callback. Pull
arrival installs an independent victim-stun program and the generated owner
return. `spear_cleanup` removes owner projectiles and releases a still-snared
victim on cancellation, while preserving an already installed arrival stun.
Focused regressions cover both players and startup paths, exact recovery,
blocked/hit pull behavior, interruption during startup/recovery/pull, full
object pools, rematch during pull, and two simultaneous spears whose projectiles
can interrupt one another.

Scene blend boundaries, fan repetitions, delayed cues and completion actions are
resolved when their frame ranges are constructed. The timing guard rejects
native actor/scene deadline branches.

Use `--move NAME --port` (with `--character sub_zero` where appropriate) to
extract these actual C bodies, nested helpers/callbacks, supporting data links,
raw operands and native values. `--c` remains the retail control-flow view.
`high_punch`, `lp_throw` and `lk_throw` are catalogued alongside the special
moves. Native throw math, random roster sound lists and auxiliary tasks remain
explicit boundaries; the portable engine's existing gameplay adaptations are
retained rather than silently claiming those routines have been fully translated.
Sub-Zero ground throws follow their own retail programs: LP 76/153, damage at
25, reaction 102 and reversed getup 328; LK 41/165, disarm at 39, damage at 52,
release at 57, a 60-tick owner wait and independent victim 41..46 cursor cycles
followed by clip 200. See [throw evidence and timing](sub-zero-throws.md).

Paired victims keep their own fall/getup lifetimes. Weapon objects fly and
settle independently, and shared locomotion and generic hit reactions remain
engine callbacks.

## Sub-Zero source-to-code ledger

Implementation: [game/sub_zero/character.c](../game/sub_zero/character.c). Retail, roster and
asset IDs happen to be 4 here; other characters need not share those indices.
The catalog links these sources directly through `--character sub_zero --move`.

| Area | Retail roots | Translation and details to preserve |
| --- | --- | --- |
| High kick | `4e5ca0 -> 4e5c80`, `4e6978/6998`, B[2] | Clip 71; 29-step sparse playback; contact at steps 11–13; retain existing kick fixture |
| Low kick / crouch HK / combo HP | `4e5d90 -> 4e6da0`, `4e5fd8`, `46c2a0 -> 4e69c8` | Clips 70/52/532, their distinct records, avoidance interval and follow-up tables |
| Shared normals / air / throws | `4e62f0`, `4e62a0`, `4e6798`, `4e6658/6710/61f8/6500/63d8`, `4de6b8/898`, `4de490/4b8/4e0`, `4e9bd0`, `4e9ee0` | Own stance selection, hit/block/miss recovery, combo gates, paired damage/release and interrupted attachment cleanup |
| Air throw | `45ed70/80`, `46aa80/aae0/ab20`, `4e5e28/50`, `4878c0`, reaction 89 `484b40/4e9910/484b60` | Shared Scorpion/Sub-Zero runner in `game/mk4_common.c`: upright paired clips 82/202; indexed cue 13; damage `0x2147` at sample 28; independent victim drop, impact and getup; ten-tick owner launch wait and shared landing |
| Ice | `49bc60`, `4ed588/4ed818`, `49bcb0`, `49c300/690/810`, C[3] | Clip 16, normal/fast startup, independent 0.2-unit flight, no direct damage, reaction 67; an active clone blocks ice, another shot does not |
| Fast startup | Opcode 45, native `45b650` | Grounded target: attacker's follow-up flag; airborne target: victim's reacting flag |
| Freeze | `486230/430/4b0/530`, `4e9d38` | Hold the captured pose for 142 ticks; double freeze backfires; air thaw gravity `0x147`, landing and control restoration; no Goro exclusion |
| Clone | `47f4d0/6d0/880/9d0/fa90/fbd0`, D[5], motion `4e87a0` | Snapshot before retreat; distance <1 omits the object; 180-tick cooldown; 74 human / 53 CPU lifetime; clip 301 forward, ascent spin `-0x6147`, apex retraction and landing |
| Slide | `47dc00/40`, `4e84d8/f8/8530`, reactions 122/165 | LP+BLK+LK chord; 0.15-unit travel, contact/recoil branches; 64-tick cooldown only on unblocked contact |
| Scepter | Descriptor `4ecdb8`, table `4ecc90`, params `4ec458/490/480/4b8`, runner `494a00` | HP 231 (18%), LP 250 (15%, low), back HP 237 (15%), back LP 228 (reaction 151, no damage); contact-selected recoveries including LP stance blend |
| Follow-up draw | `494610/6f0`, `482c20`, direct `495710` | Equip, grounded reaction 120, retain no-guard, 30% damage scale after >3 hits, omit ordinary back-HP travel setup |
| Shared weapon lifecycle | `4e62c0/d8`, `498290/480`, `45efc0`, `45f8f0`, `46a180` | Shared draw/air/drop/throw/pickup; .75 heading-relative throw origin; pickup requires action `2001`; scepter crouch LP does not acquire sword spin |
| Walk / run | `460570/710`, `491780`, `460980/9f0`, `48fae0`, `422800` | Forward 0.06, backward 70%, run 0.13; forward/back unarmed and armed gaits; 2% run cost, 40-tick delay, 5% regeneration; direction follows the X/Z pair axis |
| Wall behavior | `423690/4dad60`, `489cb0`, `488ea0`, `487530/570` | Per-stage radius, last-inside restoration with inward recovery; reached reaction branches recoil a grounded attacker at radius²−10 unless victim is marked; preserve pose, .3 retreat/friction, 8+5 recovery |
| Jump transitions | `46f7e0`, `46fdf0/fe80`, `46ff20`, `470080/140/170`, `46fd90`, `46f650/6e0/740` | Straight samples 0–2, rise 2–17, hold 17, landing 18–27 with stance cancels; directional tuck 0–8, signed spin/root offset, apex and Y=-2.3 retraction 7–1; direction at ticks 1–3 retains vertical velocity; root-aware landing, completed retraction and interruptible five-weight standing blend |
| RUN double tap | `45e820`, `469250/2e0/450`, descriptor `53ce18` | Previous RUN press within 8 ticks; 80-tick cooldown; clip 300; Down reverses playback/lateral direction; air victim scales speed to 30%; priority over pickup |
| Fatality I | `4e0498[4] -> 445100`, `4e0368..3e8`, victim `445380`, `4e03f0..438`, `445510` | Owner 385, victim 389/387 entry blends; slow victim blends to 46/49/50; paired 1.45-unit alignment; head attachment and continuing owner loop 96–140 |
| Fatality II | `4dfe90[4] -> 41b3a0`, `4d2c10/78/a0/c8`, victim `4d2bf0`, `41b450/610` | Owner 552 breath loops, independent victim 206; wait 20, slide eight samples, calculate delay from current range; uppercut/shatter, eight cursor passes, wait 60, victory |
| CPU policy | `433630`, `4df478/498/4c0/4d8`, `4398c0`, `4ab540/570/5d0`, `46fcd0/470650/6f0`, `436840/8b0`, `439c00` | Distance-weighted selections, 30-tick approaches, low-level 60–239 tick pauses, 8–27 tick footwork legs, changed-action probability gate, direct ice/clone/slide, a bounded distant-clone wait and difficulty-gated automatic air attacks |
| Finish window | `48fc20`, `46a520/580`, `421d40/421ef0`, `45e5e0` | Preserve defeat recovery until dizzy; 60-tick grace clears contact latch, then 240 ticks or first contact; no new press on expiry; an already accepted entry may start on that tick |
| Stage finishers | Fan `43d040`, `4e0118..1a8`; spikes `467e70`, `4e6950`, reaction 175 | Sub-Zero-specific commands select shared scenes; retain 31 fan repetitions and spike wait/impaled position |

## Command records

Sub-Zero's P1/P2 button lists are HP `4e4b18`, LP `4e4b38`, BLK `4e4b60`,
HK `4e4b80`, LK `4e4ba0`, RUN `4e4bc0`; Up and LK release are empty.
P2 Up's table base is **`53cb10`**, not the LK-release table at `53ca80`.
The query tool now selects these lists directly. Preserve the following order.

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

Sub-Zero's new airborne BLOCK air throw is an explicit requested extension:
his retail BLOCK list ends at `4e4b78` after slide/fatality, without Scorpion's
`4e4a04` air-throw record. The portable command follows those two records,
preserving their priority. The shared retail gate requires both absolute Y
positions <= `-0x23333` and squared 3-D Q16 distance <= `0x20000`.
The entry rechecks the victim, disarms it, cancels jump rotation and stops both
bodies. At sample 28 it deals 13% damage and releases the victim downwards at
`0x3333` with gravity `0x28f`, independently of player update order. The owner
launches at tick 29 with forward `0x7ae`, Y velocity `-0x1999`, gravity `0x28f`;
holds sample 29 for ten ticks, finishes clip 82, and lands through `mk4_land`.
Victim clip 202 finishes after landing, waits four ticks, then gets up with
clip 131 samples 46–79. Impact reuses the roster voice, A[19] and three-pulse
camera shake. Context camera task 23 uses normal portable fight framing;
retail root realignment/task parking use the existing portable recovery.

Slide requires LP+BLK+LK held together and rejects Up/HP/HK/RUN; horizontal
directions are outside its mask. Fatality I requires HP+BLK+RUN and rejects
Up/HK/LP/LK; it accepts the direction chord independently of that mask.
Fatality I/fan/spikes require distance <=1. Fatality II accepts 1.8–3 units.
Fan requires arena 8, spikes arena 0. Finishers require the deciding-KO winner
and victim action `1009`. The command matcher checks grounded entry on the press; do not reject the queued continuation merely because physics advanced before it executes. Hold block while entering repeated Up directions to suppress jumps, then release it for the final attack. Timeout does not open that window.

## Verification and scope

Retail behavior determines the assertions. Use a few focused checks of a
changed command gate, branch, damage, interruption or recovery. An entry-level
check catches wrong helper wiring that a helper-only check cannot. Reuse shared
pose, geometry, weapon, scene and sound checks for subsequent characters.
Avoid duplicate constant checks and exhaustive per-tick assertions; see the
testing guidance in [step 7](#7-wire-real-inputs-and-verify-the-behavior).
`tests/test_sub_zero.c` exercises the character through the input path;
`tests/test_mk4_assets.c` covers real looping poses, rotated contact, frozen
pieces and PCM/ADPCM decoding. Existing compact kick/Scorpion fixtures stay.
Do not add retail-execution harnesses or large per-tick JSON recordings.

Presentation uses recovered closed-surface rigs, animated head/spine models,
camera splines and independent pulse shakes (`457b90`, fatality I uses
`4e3638[5]`: two pulses, three ticks per half-cycle, Y `0x2e14`),
the ice blast mesh animation (GEO `0x100` additive blending),
the shared ice statue and its orbiting light, original weapon
meshes and three-arc settling records. Ice trails use sprite 339 and the
ten-node shrinking decoration from `40d720/40d920`; `405430` is an
always-success gate in this executable. The first fatality's joint-directed
blood stream translates `445c40`'s 203-tick cadence independently of the actor
clips. Shared blood mesh 0 becomes floor splash meshes 4–12 on contact.
The software rasterizer, blue freeze
material, lighting and bounded particle pools remain portable engine choices.
CPU distance plans, probability gates and automatic
air attacks now live in the shared simulation; unported character action rows
still fall back to their own stance tables. These translations do not claim
retail scheduler or random-call-stream identity.

The decompiler now distinguishes recovered data from the current port. The ice
spawn rig uses mesh 74; its `4ed898` track uses meshes 75–82, one tick each.
The shared projectile presentation keeps the initial mesh separate from the
loop. Opcode 25 uses `battle_sound(SOUND_INDEXED, selector)` and the
complete 66-entry table at `4ead30`, separately from A/B/C. Ice (`4ed588`)
and the breath fatality (`4d2c10`) each queue selector 4 once on successful entry:
token `0x852` resolves to `pcmk0426.esf`. Normal/fast ice release still uses C[0]
at tick 15/8. Rejected commands do not play the entry cue. The shared air
throw runner uses indexed selector 13 (token 2100, sample 420), with the
Sub-Zero command extension described above.
Existing adaptations remain reuse references, not evidence overriding the EXE.

Sound formats are checked against the local executable, including corrections
to reference hypotheses: buffers are mono; ESF bit 31 controls looping.
`4bd4f0` maps sampled tokens through division by five. The 674 archive samples
are decoded once. Three looping ADPCM files lack a final nibble; the decoder
safely pads the final declared sample. Music/control tokens below 100 are
outside sampled-effect playback. Additional sound cues can use the same event
queue without character-owned audio devices. Each bank retains its own selector
count; zero and out-of-range indexed entries resolve to silence.

For each new character, add only researched roots to
`tools/character_catalog.py`, translate its selected data/decisions, and reuse
shared systems. Mark any unresolved native dependency explicitly before
calling the affected move complete.

## Liu Kang coverage

See [Liu Kang's source and timing ledger](liu-kang-scripts.md). All 57 catalogued
VM roots are generated and pass the generated-only audit. Slot 3 now
has procedural C entries for normals, close/combo and air attacks, both
ground throws and independent victim recovery, high/low/air fireballs,
flying and charged bicycle kicks, the type-66 weapon programs, sidestep,
both character fatalities and both stage finishers. The release-table audit
is essential: `53ca38/53ca80[3]` reaches bicycle kick after a 120-tick LK
hold. CPU selection distinguishes near bicycle from medium low fireball
and uses character-owned first/second fatality approach distances.

`tests/test_liu_kang.c` covers real commands in both slots, damage and
contact branches, repeats, charge/release priority, interruption, KO and
recovery, object-pool exhaustion and rematch cleanup. The original weapon
and dragon rigs are loaded and posed in the asset checks. Cinematic
framing, minor sound/decorative scheduling and generic blood/flesh piece
allocation are explicit portable presentation adaptations in the ledger.

## Sub-Zero generated coverage

All 63 reviewed Sub-Zero VM roots are generated by `make scripts`; the strict
`--character sub_zero --audit --strict --require-generated` gate requires zero
legacy roots. This includes native-entered spine, throw, fan and victory
continuations. Extend the typed bindings and catalog when following a native
entry reveals another VM program; do not retain its old timing table.

The native C owner retains collision runners, paired alignment, projectile and
scene-object allocation, blends, camera records and cleanup. The generated owner
and victim functions own VM playback, sound, motion instructions and branches.
Shared bindings and primitives are in `game/script.c`; independent air-throw
release/getup uses `game/mk4_common.c`. See the compiler integration notes for
source-derived corrections to diagonal clips, draw/air-throw playback and peer
first-row timing. Native behavioral tests cover commands in both slots, contact
branches, interruption, KO, independent recovery and finisher reset.


## Scorpion generated coverage (historical)

Scorpion generates all 70 reviewed VM roots. The 48 initial roots grew by 22
when bounded native queries exposed roundhouse starts, fan continuations,
burn owner/victim continuations, beast approach/victim recovery, the final
teleport-victim wait, spin startup and two shared air-weapon selectors. Every discovered body is in the catalog and profile.
`make scripts-check` checks canonical C and all strict generated-only audits;
there are no legacy bodies or per-character Make exceptions.

The spear retains the shared object pool, pull and interruption cleanup. Its
startup, spawn, return playback and victim programs are generated. Pull arrival
owns the independent stun lifetime; VM reaction 71 records that continuation
without restarting it. Teleport launch/landing and ground victim programs are
generated; native code owns arena relocation, contact and the blocked flight.
Back-sword hit/block/miss paths use generated contact queries and opcode 42.
Burn and beast VM timing tables are removed; native C retains blends, pieces,
rig appearance, particle allocation and scene completion. Fan/spike helpers
are shared with Sub-Zero. The former shared handwritten air-throw VM loops
are removed; both fighters supply generated owner and victim functions to the
same native pair setup/release/getup helpers. RUN sidestep uses one shared
native helper across the three completed ports.

Generated playback uses one explicit actor-owned cursor and held-row
waits. It preserves script control flow and source values without copying the
retail distinction between last submitted pose and its advanced cursor or its
continuation-stack scheduling. Consequently the native tests reuse the old
normal-fixture branch inputs but check branch choice, clip bounds, damage,
interruption and bounded recovery, rather than per-tick x86 identity. Draw and
spear recovery include the tagged-return wait. Native uppercut queries retain
all three contact visits before their existing recovery loops.

The existing portable presentation choices remain: context cameras follow live
actors, creature rigs reuse the owner actor, voice lists use the shared roster
resolver, and fire/gore decorations use bounded engine pools. These are native
presentation adaptations, not handwritten VM exceptions. The audit establishes
reviewed static coverage; native behavioral tests remain necessary.


## Raiden and Sonya generated coverage

Raiden now generates 64/64 reviewed VM roots; Sonya generates 63/63, each with
zero legacy bodies and no coverage errors. The roster-specific LK throw entries
are 4de218 and 4de360. Sonya also owns a different air-throw pair, 4e5ed0/4e98a0,
rather than the shared 82/202 owner/victim clips. Shared contact reactions are
registered in the common reaction table so arbitrary victim rosters execute the
generated recovery programs.

Native adapters keep their source-derived contact/recovery phases, typed
bindings, pair/object lifetimes and engine physics. Profiles declare checked
native clip/cursor entry contracts. Opcode 71 uses a shared yielding pose blend;
shared projectile, pair cleanup, delayed motion and weapon helpers serve both
characters. See [the combined ledger](raiden-sonya-scripts.md) for evidence,
commands, native adaptations and validation.

### Frame-program ports

Every profile generates its roots as frame programs; the compiler has no other
backend. Run the inventory, compiler and strict generated-coverage checks
described above. Do not replace a compiler-supported VM body with a handwritten
timeline. Keep native contact decisions in callbacks, use phase transitions for
waits and branches, and attach action cleanup for interruption. Paired moves
give released victims independent phases. The shared weapon strike/throw/pickup
and sidestep runners use phases too, including the foreign sword paths.

## Native action frames

Native moves use fixed 60 Hz `mframe_t` rows with optional movement and action
callbacks. Frames have no duration. Select hit, sound, movement, release and
recovery actions when constructing a phase, then let the frame runner dispatch
them. Entry-time constructors can resolve a range from the current pose, move
branch or scene phase; callbacks must not compare elapsed actor/scene clocks or
count down scratch fields.

Use the helpers in `game/native.h` for bounded playback, contacts, blends,
repetitions and physical holds. Native tails install their rows directly and
keep the action cleanup; a wait is a held row with a continuation. Independent motion and battle-event rows
support concurrent actor/scene work. Landing and ownership guards inspect live
state on held frames. Keep input charge/cooldown rules and independent world
object lifetimes in their owning systems.

Supported VM bodies remain compiler-owned. Native fan coordinators compose a
generated root's phase into their complete repetition schedule. Regenerate with
`make scripts`. `make native-frames-check` rejects actor/scene clock branches and
runs as part of `make check`.

## Character folders

See [the source layout](../game/README.md). Each character owns its
`character.c`, private header, move C modules, generated scripts/frame maps and
reviewed `port.json`. Move helpers remain private to one C11 translation unit.
The source query and coverage tools follow native fragments while preserving
actual file/line links and separate generated-code boundaries.
