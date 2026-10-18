# Deterministic retail script compiler

`tools/compile_retail.py` lowers the existing decoder's selected graph to
ordinary C11 fighter functions. It reads the pinned EXE, specializes character
selectors, preserves explicit branch/fallthrough targets, and emits exact contact
records and source-word comments. It never executes retail code. Generated C
calls the existing fighter fibers, combat, audio and physics APIs. There is no
runtime bytecode interpreter or native-address dispatch.

## Commands

```sh
# Begin a character port with static facts and unresolved implementation work.
python3 tools/compile_retail.py --character liu_kang --inventory --output build/liu-inventory.json
python3 tools/compile_retail.py --character liu_kang --audit --summary
python3 tools/compile_retail.py --character liu_kang --audit --output build/liu-audit.json

# Preview a script, with an explicit engine continuation for VM returns.
python3 tools/compile_retail.py --character liu_kang --script 4e60b8 --return-to fighter_ready --output build/crouch-hk.inc

# Regenerate checked-in character code, and verify it without writing.
make scripts
make scripts-check

# Strict generated-only coverage (also required by make scripts-check).
python3 tools/compile_retail.py --character liu_kang --audit --strict --summary
python3 tools/compile_retail.py --character liu_kang --audit --strict --require-generated --summary
```

Make discovers all profiles in `game/*/port.json`. Every profile participates in
generation, canonical regeneration checks and the generated-only audit. There
are no character exceptions in the Makefile. Scorpion passes with 70/70 and
Sub-Zero with 63/63 generated roots; Liu Kang passes with 57/57 generated
roots. Raiden passes with 64/64 and Sonya with 63/63 generated roots; both have
zero legacy roots. Liu Kang includes the original 50, five followed throw/fan
continuations, and two shared air-weapon selector programs. Detailed JSON still reports every
unsupported instruction or native boundary for an incomplete future port.

A successful compile returns 0. Unsupported or invalid compilation, a stale
`--check`, malformed profile or I/O failure returns 2. `--audit --strict`
returns 1 for coverage gaps; `--require-generated` also returns 1 while any
catalogued VM root is not generated. Default audit mode reports gaps without
failing. `--inventory` and audits do not load the archive or generate a bulk
disassembly bundle. Detailed reports are static evidence, not runtime traces.

## Typed native bindings

`tools/retail_bindings.py` is the compiler's executable contract registry.
`retail_helpers.py` remains the readable research index; its prose and optional
pseudocode templates are not compiler bindings.

Each binding declares the local EXE address, C symbol and source definition,
allowed roles, engine argument types, result (`void` or native flag bits), flow
and evidence. Cursor effects (`preserve`, `finish`, `unknown`) keep native
playback from invalidating loop-progress proofs. Bindings may also explain a portable adaptation. A physics
callback has the existing `FighterFn` signature and is installed without calling
it. A spawn binding declares accepted task types and uses the existing object
pool to create an independent lifetime. Its C entry performs allocation; it
must not synchronously run the spawned object's animation/task body.

Predicates return flags immediately; native opcode 36 tests bit 4, opcode 55
bit 1. Contact operations produce the contact bit used by conditional waits.
Arguments are engine-facing values, not guessed signatures of retail scratch
registers. The compiler rejects incompatible roles and absent operands.

Research a new native boundary with the bounded decoder/reference tools, then
bind an existing helper or add a small native adapter. Do not replace a VM
body with a new handwritten timing table merely to make the audit pass.

## Generation profiles and coverage

`game/<character>/port.json` declares generated script roots, output include,
controller/tagged-return adaptation, integration source, move references,
entry contexts, stance/weapon/reaction adapters and a reviewed inventory
fingerprint. `legacy_scripts` remains readable for auditing older profiles, but
every Make audit requires it to be empty. Liu Kang has no legacy VM bodies.

The audit independently checks that the command reader includes all eight
known input event kinds, including LK release, and reads both players' records.
It also reads the four CPU special-choice rows, nine shared stance/follow-up
tables, default weapon descriptor/buttons, contacts/reaction dispatch and
catalogued script/native frontiers. The inventory fingerprint includes source
words and typed tables. A new root, CPU/input target, reaction or changed word
requires review, not automatic fingerprint acceptance.

References must resolve to actual C functions or initialized tables. Shared
reaction entries must match the reviewed C dispatch symbol. Paired or cinematic
reactions that bypass the shared reaction table require an explicit adaptation.
Generated roots must compile, be referenced by the integration source or reached
through a statically linked generated CFG, be included there, and match canonical
regeneration. A script cannot be both
legacy and generated. Missing or stale entries fail the strict coverage gate.

These checks establish declared static coverage. They do not infer native
semantics from function names, recursively decompile every x86 callee, prove
an adapter equivalent, or establish that a curated move catalog is exhaustive.
Inspect newly found table handlers and extend the catalog when required.

## Safety and playback boundaries

Compilation finishes validation before writing any output, then atomically
replaces the destination. Unknown opcodes,
unbound native calls, missing graph edges, incompatible helper roles, illegal
operand overlaps, out-of-range cursor targets, uninitialized playback/loop/result state and cycles that can
bypass all positive waits are rejected. The known selected air-table word that
also executes as a VM no-op is supported explicitly. Returns require a supplied
engine continuation rather than an invented retail task stack.

For a root entered from native code with an inherited clip, declare an
`entry_contexts` record keyed by its hex entry. It requires `clip`, `count`,
`sample` and an evidence string naming the native entry. Set `clip` to the
decoded integer when all incoming paths establish one clip. Use `"current"`
when evidence establishes a common sample count and cursor but paths preserve
different clip IDs; generated C verifies that count and cursor, then rebinds
the actor-owned storage with the live clip ID. Compilation seeds its
initialization/cursor analysis from that contract. Missing context fails
compilation. Do not initialize unknown flags, selectors or loop counters to
invented values. A per-entry `return_to` overrides the profile continuation;
`"caller"` means ordinary C return for a native coordinator's script subroutine.
It still preserves the controller-return handoff wait.

A native tail that *only* enters another VM script declares `script_target` in
its binding. The compiler follows that bounded static edge, validates the
combined graph and emits a direct label jump. Jab repetition therefore does
not recursively call C functions or grow the actor stack. The integration audit
counts such reachable roots without requiring unused wrapper references.

Yield analysis propagates finite clip/cursor states through resets, branches,
counts and target playback. A target loop is accepted only when progress to
its target is proven on every incoming path; same-cursor loops still fail.
Contact-result branches lower to tested C edges. Binding opcode 11 preserves its
inline cursor; reaction-context opcode 4e uses the explicit Fighter argument and
does not perform playback. Shared native bindings live in `game/script.c`,
including audio, damage/disarm, pair stepping, jab gates and impact/motion setup.
Paired clip binding/counts, sample-start playback, landing waits and optional
full playback use shared engine helpers. Opcode 71 blends an initialized source
pose into destination sample zero, yields through the shared native blend, and
resumes at sample one. Destination count must exceed one; the compiler applies
its usual descriptor bounds and source initialization checks. Typed native adapters retain ownership
of independent victims, delayed motion, camera orchestration and scene teardown.

The shared clip primitives use the engine's existing cursor stepping. Counted
playback yields once per visit, full forward/reverse playback includes its final
sample, and a target request already passed in that direction does not rewind.
The controller return has one actor-tick handoff; tagged return uses the
profile's declared continuation. These preserve the portable engine's tested
behavior, not exact retail scheduler ordering or pose interpolation. Actor-owned
`playback_move` storage survives suspension and cannot dangle when a fiber is
cancelled. Native adapters retain responsibility for physics/object lifetimes.

## Liu Kang proof and validation

All catalogued Liu Kang VM roots run from `game/liu_kang/generated/scripts.inc`:
normals and jab branches, paired throws and releases, air attacks, weapon draw
and sword programs, projectiles, flying/bicycle exits, and the fireball/fan/spike
scene scripts. Native-only attacks and native portions of contact/recovery/scene
routines remain C adapters. The former VM timing tables and handwritten script
loops were removed, including the fan's embedded follow-up scripts.

Generation exposed two old timing assumptions: opcode 19's count 17 consumes
nine double-step visits (459f80 decrements twice), and sword HP's successive
sample targets 9/18 consume five visits each with the engine's clamped cursor.
The native regressions now check those decoded boundaries. This remains a
portable gameplay adaptation, not exact retail scheduler/pose identity.

`tests/test_script_codegen.py` checks graph preservation, malformed and unknown
boundaries, definite initialization, wait-free cycles, selector specialization,
optional descriptor prefixes, inherited-entry contracts, native script links,
proven cursor progress, deterministic generation, atomic validation
failures, real C11 compilation and execution of both emitted conditional paths.
Coverage mutation tests catch dropped release tables, changed CPU targets,
missing implementation references and wrong reaction adapters.

`tests/test_liu_kang.c` exercises real inputs in both slots: generated LK
hit/block/miss damage and 33/36-tick recovery, the 42-tick crouching-HK tail,
fireball release timing, independent projectile interruption, crouch/block/armed
reactions, air contact/follow-ups and landing, paired interruption/KO, weapon repeats,
finisher completion/reset and allocation failure. The additional throw voice uses
the existing audio loader with the native roster remap, rather than aliasing the
unrelated indexed sound bank. Shared scheduling is covered once
by the existing fiber suite. `make check` includes regeneration, strict generated-only
coverage, compiler tests and existing game/asset/archive regressions.

## Sub-Zero integration

Sub-Zero generates all 63 reviewed roots, including the native-entered spine
continuations, fan continuations, the victory selector reached by `4712e0`, and both shared
air-weapon selectors.
Its owner and victim fibers call generated code independently; native adapters
retain contact runners, pair alignment, object allocation, blends and camera
operations. The former VM timing/finisher phase tables are removed.

Shared bindings now cover reaction motion (`45af60`, four signed operands),
friction, the native twelve-tick sound callback and the separate opcode-26
20-entry sound bank at `4eaaf8`. Normal and fast ice startup, slide contact
branches, and the LP/LK throw continuations come directly from decoded words.
Static selection also corrects Sub-Zero's diagonal air clips to 56 and 59 and
uses their own contact records. Generated draw includes the script's final wait;
air throw holds sample 29 after its counted advance, before forward playback.

A newly started peer fiber starts its wait deadline on the current simulation
tick. It cannot consume that wait again when its player slot is pumped later
in the same tick. Shared fiber tests cover both actor orders. Result-screen
presentation may continue an existing generated finisher pose loop; reset retires
that actor stack through the normal fiber lifetime API.

Portable scene adaptations remain explicit: context-camera requests use shared
framing, ice breath uses the bounded effect pool, native blends use scene helpers,
and stage tasks use shared arena completion. These adaptations do not translate
VM opcodes in C adapters or emulate retail tasks.


## Scorpion integration and source navigation

Scorpion's 70 roots include the native-entered burn and beast scene programs,
fan continuations and teleport victim tail. Their generated code uses shared
horizontal-motion, decorative-effect and seven-row camera-pulse bindings.
Opcode 42 branches on the preceding contact result, so both back-sword programs
compile completely, including their hit/block recovery and miss fallthrough.
Runtime inherited-cursor failures report both required and actual clip/sample.

The `--port` worksheet links direct decoded edges to their typed compiler
bindings, source definitions and declared lifetime. Generated roots are indexed
as source for all three completed fighters. An uninstalled future fighter's
generated thunk is identified explicitly without breaking the research query.
Source/action navigation scans generated includes as well as native C.

The shared air-throw setup no longer starts or retains handwritten playback
loops. Callers provide two ordinary generated C function pointers; native code
handles pair ownership, interruption and released-victim getup. Shared weapon draw only prepares native equipment state; callers run generated
playback, and the old shared draw loop is removed. Shared stage
coordinators and RUN sidestep keep engine behavior out of per-fighter copies.


The final weapon pass also generates `4e66d0/4e66f0` for each completed fighter.
Those programs pass their decoded `1f` selector into the shared native air-weapon
runner. Scorpion additionally generates the spin startup `4e8850`; its contact
and travel loop remains native `4807b0`. No handwritten VM startup remains in
the draw, air-weapon or spin entries.


## Raiden and Sonya integration

The reviewed profiles generate 64 Raiden and 63 Sonya roots, including normal
selector branches, ground/air throws, released victims, projectile/weapon
reactions, special exits, victories and all four finisher families. Native-only
teleport/bicycle routines and native contact/recovery adapters remain in their
character files. [The ledger](raiden-sonya-scripts.md) records the evidence,
portable adaptations and focused regression coverage.

Shared bindings now reuse pair setup/release/cleanup, reverse pair playback,
pose blending, delayed horizontal motion, independent projectile objects,
weapon tails and scene task lifetimes. Direction press commands match against
prior input history before recording their own trigger (Raiden Down,Up).
No alternate script scheduler or retail execution harness is introduced.

## Incremental mmove backend

A profile's `mmove_scripts` is a subset of `generated_scripts`. Those roots
use the same decoder, graph/cursor validation, typed bindings, inventory and
canonical-output checks, but emit const frame rows and nonblocking boundary
callbacks. Statically selected roster branches and counted loops are expanded
offline. Playback preserves source sample skips, reverse playback and waits through
fixed 60 Hz gameplay frames. `mframe_t` has only the optional movement and
action callbacks plus movement distance, following Quake II monsters. Decoded
boundary effects are attached as `thinkfunc` callbacks on their execution
frames. Optional move-owned pose maps handle skipped, reversed and repeated
source samples; plain sample changes need no callback. No frame has a duration. Runtime branches not yet supported by this backend
fail generation. Native branches may select another generated phase explicitly.

Bindings used by this backend must opt in with `mmove=True` after their native
implementation has been reviewed as nonblocking. Yielding bindings are rejected,
not replaced with no-ops or fibers. The strict audit reports failed frame
lowering and still requires regeneration and all reviewed roots. Existing roots
continue to use fibers until individually migrated.

The first converted root is Sub-Zero sweep `4e6798`: roster slot 4 selects
`4e6e68`, clip 63, doubled startup through sample 10, A[175], then sample 22.
Native `46f080/46f130` retains the established two contact visits and remaining
clip recovery in native frame phases. Its command entry allocates no fiber.

Sub-Zero slide roots `4e84d8`, `4e84f8`, and `4e8530` now use the mmove
backend. The native contact phase checks each simulation tick from 9 through
25 and selects hit, blocked, or miss recovery. The generated hit/block loops
retain four/six repetitions of samples 10,9,8,9; only an unblocked hit stamps
the cooldown. Native return reverses samples 10 through 0. Action cleanup
stops motion and clears no-guard even when interrupted during blocked recoil.

Sub-Zero LP throw `4de798` and reaction `4e9bd0` use generated frame rows
for both participants. The pair-to native primitive has an explicit offline
lowering: owner rows advance the captured victim at the same boundary and
abort if either participant loses capture ownership. No yielding helper is
called. Release at tick 25 starts the victim's independent frame phase; its
first frame cannot execute during its installation tick, in either player
slot. Native getup 328 is a separate nonblocking phase. Owner recovery ends at
49; victim tumble/hold/getup ends at 123, with dead victims staying down.
The frame-program entry rejects an active fiber rather than silently nesting
backends. LP cleanup releases captured peers before damage and leaves released
victims independent; victim cancellation removes its landing callback. Finish
window detection includes active mmove phases. LK throw and other unconverted
moves still use the existing fiber implementation.

Ground-attack phase branches now lower runtime predicates and conditional waits
to named C callbacks and bounded hold tables. The compiler specializes successors
by clip cursor, counted-loop state and selector; it rejects excessive expansion.
Only the native result flags persist on the actor. No runtime program counter,
bytecode dispatcher or suspended stack is introduced. Native Sub-Zero ground
contact/recovery loops use held-frame action callbacks, including HP repeats and
uppercut hit/block/miss recovery.

Sub-Zero is now an `mmove_only` profile: all 63 catalogued roots must be listed
in `mmove_scripts`, and the strict audit rejects coroutine calls in its native
integration source. Removing a root from phase generation fails both generation
and coverage. The normal EXE inventory, typed binding review and canonical
generated-output checks still apply. Other fighters can migrate incrementally.

Repeating scene streams compile into finite, named phase cycles. Parking uses
a callback that holds its frame; landing releases a held frame when grounded.
Held callbacks run on every visit with zero movement distance. Frame jumps
are explicit, and moves without an end transition loop automatically. A
terminal generated phase stops explicitly to preserve its final pose. Native scene blends
advance from tick callbacks and preserve their original blend increments. Cameras,
particles and projectiles remain independently scheduled. The first fatality
intentionally keeps its final pose loop after match completion until reset.

### Runtime control registration

A definition in a coverage profile does not establish a gameplay entry. The
coverage audit follows C function and initialized-data references from each
fighter's `CharacterDef`, including generated code and callback tables. Static
symbols retain their translation-unit ownership; prototypes and detached
reference cycles cannot satisfy coverage. The runtime roster lookup does not
make another fighter's registration count for the current fighter. This is a
conservative source-connectivity check, not native-behavior or branch parity.

Profiles with `controls_output`, `command_bindings` and `command_predicates`
compile their reviewed command registrations with `tools/retail_controls.py`.
The output retains the EXE's record order, direction patterns, inclusive timing
windows, air/block flags and release events. Native addresses remain offline
keys; runtime records hold typed C callbacks. The audit rejects missing retail
command targets, unknown predicates, stale generated tables and generated
arrays that are not reachable from the owning `CharacterDef`. A partially bound
profile may emit its implemented rows, but remains an audit failure until every
retail command target is registered. `make scripts` regenerates these tables;
`make scripts-check` verifies their canonical contents alongside VM output.

For profiles using generated controls, `cpu_bindings` separately reviews the
native targets selected by the four typed CPU rows. The same output emits
`retail_cpu_moves` with near, medium, far and projectile entries in their
portable slots. An input command reaching a special does not satisfy a missing
CPU registration. The audit checks the required CPU targets and requires the
CPU table itself to be reachable from the owning character registration.
Partial bindings remain explicit coverage errors rather than falling back to
an unrelated move.

Reachability also scans authored fighter callbacks and `ButtonTable`,
`CommandDef`, `Controls` and `CpuMoves` definitions absent from the manifest.
An unlisted follow-up cycle cannot satisfy itself. Mechanical generated VM
continuations and their integration thunks remain checked through catalog
roots rather than being mistaken for separately callable moves. This is a
conservative source graph; behavioral tests still establish input gates,
contact, cancellation and recovery.

Fujin LP capture and both native-selected owner branches now use frame tables.
`mk4_mmove_throw_start` acquires the pair and installs a held victim frame;
release replaces only the captured peer, while the owner finishes its generated
stream. Native 45b1f0's historical current-context name is misleading here:
480f80 reaches 48f880/48fd00, which installs the selected reaction through the
task's peer fields +38/+3c. Applying it to the executing owner cancels that
owner mid-script. Neither LP branch allocates a coroutine.
