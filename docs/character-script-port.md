# Porting a character's scripts to C

Retail scripts and their native continuations are the specification. Preserve
commands, branches, animation phases, damage, reaction selection and recovery
in procedural C. Reproducing the retail VM, scheduler, scratch globals or
per-tick binary traces is unnecessary. Game simulation remains C11 without SDL.

The active simulation is `g_battle`, defined in `game/battle.c`.
Move/physics/event/end callbacks take `Fighter *`; projectile callbacks take
`Projectile *`; predicates take `const Fighter *`. They access shared match
state through the global instead of receiving a `Battle *`. The host calls
`battle_init(p1, p2)`, binds borrowed assets with `battle_bind_assets(assets)`,
then steps `battle_step(input)`. Tests reset the singleton for each scenario.
This is one active simulation, not a reentrant multi-battle API. SDL and
asset ownership remain with the host; drawing only reads simulation state.

## Short workflow

1. Select the character and inspect a move with `--c`. It shows all decoded
   paths, command gates, contacts and helper contracts. Native-only moves show
   existing C adaptations; unresolved native behavior remains an explicit query.
2. Follow only the printed script/native/contact dependencies needed by that
   move. Use the existing `build/retail-disassembly/` for broader navigation;
   full export requires explicit `--bundle` and is unnecessary for routine work.
3. Translate decisions into `game/c_<character>.c`. Reuse shared helpers below.
   Add an engine feature only when a reached move requires it. Character-specific
   constants and decisions stay with the character.
4. Verify the changed branch through its actual command or move entry. Add a
   small regression only for a meaningful failure mode. Shared helpers are
   checked once; avoid copying an exhaustive scenario matrix for every fighter.
5. Run `make format`, relevant native checks, then `make check`. Commit the
   coherent verified step on `main` before starting another step. Record source
   addresses, results and remaining limitations in the commit description.

```sh
python3 tools/disassemble_retail.py --character sub_zero --list-moves
python3 tools/disassemble_retail.py --character sub_zero --move low_kick --c
python3 tools/disassemble_retail.py --character sub_zero --move ice --c
python3 tools/disassemble_retail.py --character sub_zero --move clone --c
python3 tools/disassemble_retail.py --character sub_zero --script 4ed588
python3 tools/disassemble_retail.py --native 45b650 --limit 48
python3 tools/disassemble_retail.py --attack C 3
```

See [script-decoder.md](script-decoder.md). `--character` accepts roster names
or IDs 0..17 and defaults to Scorpion for compatibility. Keep it on subsequent
script queries: selectors, including nested ones, use that character. Kitana
uses Sonya's script slot; Noob uses Reiko's. Other characters' shared roots are
navigation aids, not complete special-move catalogs. Unknown native behavior
is an investigation boundary, never a no-op or proof of completion.

All addresses refer to the supplied `MK4/MK-4.EXE`, SHA-256
`4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d`.
`RetailImage` checks the revision and applies startup fixups. Script/descriptor
pointers are packed word indices (multiply by four); native entries are byte
addresses. Upstream reference addresses differ: verify correspondence locally.

## Shared engine features

| Need | Reuse | Important boundary |
| --- | --- | --- |
| Commands | `game/commands.c`, ordered `CommandDef` and stance tables | Preserve unconditional-record priority, two-sample presses, inclusive windows and predicate masks; entries start next tick |
| Fixed playback / waits | `MoveSequence`, sparse `MoveSegment`/`MoveEvent`, `fighter_start_sequence` | Active phases have their own tick clock; simulation ticks and stored pose samples are different clocks |
| Native continuations | `Fighter.script`, explicit phase/timer | Start the move first, then install the continuation; replacement clears old move state |
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

`fighter_start_sequence` replaces the current continuation without restarting
the move. It resets the phase tick to zero, clears the native script and sample
override, and preserves action, contact results and input restrictions. A hold
segment `{0, sample, 0, 1}` plus `count = 45` expresses a 45-tick wait; `end`
is its C continuation. The world and independent objects continue stepping.
Tick zero runs entry events once; completion runs on tick 45, once. An optional
`until` predicate delays completion while retaining the last pose. The runner
stops old events when a callback replaces even the same phase or queues an
interruption. A new move clears the active sequence.

Scorpion spear reaches sample 30 at normal/fast startup tick 15/11, sets action
`0x1007`, then enters a 45-tick hold. This follows `4ed470/4ed498` and removes
the old shared total-tick-60 cutoff. Successful pull arrival replaces the hold
with the existing spear-return continuation. The existing policy of retaining
a live paired pull beyond the minimum wait remains an engine adaptation;
interruption releases the captive victim rather than firing a stale return.

## Sub-Zero source-to-code ledger

Implementation: [game/c_sub_zero.c](../game/c_sub_zero.c). Retail, roster and
asset IDs happen to be 4 here; other characters need not share those indices.
The catalog links these sources directly through `--character sub_zero --move`.

| Area | Retail roots | Translation and details to preserve |
| --- | --- | --- |
| High kick | `4e5ca0 -> 4e5c80`, `4e6978/6998`, B[2] | Clip 71; 29-step sparse playback; contact at steps 11–13; retain existing kick fixture |
| Low kick / crouch HK / combo HP | `4e5d90 -> 4e6da0`, `4e5fd8`, `46c2a0 -> 4e69c8` | Clips 70/52/532, their distinct records, avoidance interval and follow-up tables |
| Shared normals / air / throws | `4e62f0`, `4e62a0`, `4e6798`, `4e6658/6710/61f8/6500/63d8`, `4de6e4`, `4de2f8/318/328` | Own stance selection, hit/block/miss recovery, combo gates, paired damage/release and interrupted attachment cleanup |
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
holds sample 28 for ten ticks, finishes clip 82, and lands through `mk4_land`.
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
