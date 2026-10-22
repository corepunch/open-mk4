# Direct retail script prototype

Safety checkpoint: `archive/pre-retail-vm-d7b69ba` preserves main before this work.
The user explicitly requested a runtime interpreter instead of script-to-C/frame
lowering; that request supersedes the compiler-only policy for this prototype.

## Upstream inspection

Inspected mk4decomp commit `af8800788f704be1e3c85a0e3cbf5688f112c426`
(MIT author contributions; no upstream implementation copied).

- `src/engine/fight_script_vm.c`: `PendingMatch_00459510` fetches a word
  through `ScaledIterStep_0045c020`, takes its top byte and resolves the
  handler from the live image table at upstream VA 0x4e8970. Group +0x48
  stores a packed word cursor. Native calls and tails resolve code addresses.
- The same file implements waits by installing actor callbacks, tagged
  continuations and a pause flag. Re-entry resumes the VM. Loop opcode 0x3b
  decrements one actor counter and scans backwards to a 0x3a marker.
- `src/engine/arg_sar_store_jmp.c`: script entry converts a byte address
  to a packed word cursor and invokes the interpreter.
- `src/engine/anim_frame_advance.c`: playback advances the animation cursor
  and reports completion independently of script fetch.
- This portable twin is under NON_MATCHING. It is incomplete: foreign handlers
  rewind one word and park/retry. The matching build preserves original data
  and reconstructs original code. Neither path is a ready portable combat API.

Upstream addresses differ from our supplied EXE. Our existing decoder and
native correspondences remain the evidence for local words and semantics.

## Bounded first implementation

Run original script words from the supplied local PE, applying its known data
fixups without emitting C or frame rows. Keep all mutable execution state on
Fighter. The image is read-only shared asset data; retail addresses are checked
indices, never host pointers or executable code. Native operations use explicit
allowlisted adapters to existing combat helpers. Unsupported operations fail
with a diagnostic and bounded work prevents a non-yielding script hanging a tick.

Start with Scorpion low kick (0x4e6d08) and crouching high kick (0x4e60b8).
The first has contact-dependent recovery and a reverse native tail; the second
has a counted loop, forward completion and reverse playback. Low kick itself
has no counted contact loop. Keep existing generated entries for other moves.
No coroutine library or retail global arena is required for these entries.
Bounded local native queries also checked 0x46a8d0 -> 0x429380 -> 0x429600:
the first installs its resume tag and calls the animation continuation;
0x429380 supplies the current cursor and 0x429600 calls the decrement helper
0x429740 until completion, then returns through the tag to controller 0x46f650.

Validate real move entries, hit/block/miss, bounded recovery, both actor slots,
interruption and reset. This is not a claim of retail per-tick scheduler identity,
full Scorpion coverage, generic tagged returns or paired-task support.

## Initial prototype (historical)

`game/script/vm.c` reads the relocated image directly. It implements only the
operations reached by the two reviewed roots. `game/scorpion/moves/retail_vm.c`
owns the three native bindings: clear/set high-punch avoidance and reverse
playback followed by controller return. Contact geometry, damage and reactions
use `battle_contact`; its record is decoded from the image B table, not copied
into a new attack table. Native recovery advances one cursor on the simulation
clock; it does not install an mmove or compile frame rows.

Assets retain their selected EXE image and `battle_bind_assets` exposes it as
borrowed read-only script data. In the regular game these two Scorpion button
entries automatically use the interpreter. Rule-only callers without an image
retain generated entries. Tests may bind a caller-owned image through
`g_battle.retail_scripts` after battle initialization, without loading graphics.
The image must outlive the battle that borrows it. Reset cancels VM state;
interruptions and replacement playback retire it immediately.

Run `make retail-vm-test` for command-entry regressions and malformed-stream
rejection. The loop test changes one word in an in-memory test image and checks
that an extra loop adds its source-defined waits, without C regeneration.
Other tests exercise hit/block/miss, both player slots, bounded pose/recovery,
interruption, KO and match reset. No source EXE is modified.

The prototype deliberately does not implement arbitrary native calls, tasks,
paired scripts, special moves, general tagged call stacks or the whole opcode
set. Script fetch is limited to each reviewed root range; unsupported operations,
unbound natives, invalid samples and non-yielding instruction runs fail with
source-PC diagnostics. The shared image loader retains the asset loader's
structural revision checks; it is not a cryptographic whole-EXE verifier.
Existing compiler audits still validate retained generated paths and do not
establish interpreter coverage. This is a first migration seam, not removal
of the compiler or completion of the Scorpion port.

## Verification

- `make format` and `make format-check` passed.
- `PYTHONPATH=/tmp/open-mk4-test-deps make check` passed, including the new
  runtime tests, all strict generated audits, existing gameplay/assets/render
  regressions, disassembly/port/compiler tests and the 360-frame headless run.
  Capstone 5.0.9 was installed only under `/tmp/open-mk4-test-deps`; the pinned
  upstream checkout lives in ignored `reference/mk4decomp` for source-link tests.
- The runtime test rebuilt with C11, `-Werror -pedantic-errors`, ASan and UBSan
  and passed, including real asset ownership/binding and teardown.
- `git diff --check` passed. No original asset files or upstream sources changed.

## Spear migration

Migrated retail roots: low kick 0x4e6d08, crouching high kick 0x4e60b8,
spear startup 0x4ed448, owner return 0x4ed4a8, captured victim 0x4e9d00,
and stun victim 0x4e9f20. Their generated C/frame programs are removed. Scorpion's standalone generic
clip frame atlas also had no consumers and is removed; its generator no longer
recreates atlases for modules that do not include one.
Rule-only tests now use an immutable default EXE cache when no host image is
bound; migrated roots never fall back to generated programs.

The shared VM executes the source fast-start branch, two-sample playback,
task-spawn operand, 45-tick recovery wait, and victim clip/return operations.
It reads the spear attack record from retail data at 0x4ebeb8. Existing native
spear helpers still own projectile flight, contact, pull movement, arrival,
combo scaling, stun/cooldown and cleanup. Those routines were native in retail;
the migration does not move their behavior into invented scripts. Native waits
now suspend the VM directly instead of constructing frame rows. Cleanup is
detached before invocation and cancels committed victim work on interruption.
Lethal contacts retain the ordinary defeated reaction instead of parking a
victim after the round retires projectile objects.

Bounded local queries inspected 0x49a680, 0x486000, 0x487420 and 0x45b1f0.
The native stun owns its 0x40 countdown; the script owns clip 13 and the tail
call. No synthetic VM body or retail-execution harness was introduced.

`--require-implemented` checks every catalogued root against the shared
interpreter's decoded bounds, opcodes and typed native bindings. Input and CPU
targets are checked against shared native registrations. Coverage is derived
from the EXE and implementation without per-fighter review manifests.

Validation policy: trust the retail scripts rather than re-test their authored
balance and frame timing. Verify shared interpreter semantics and our native /
actor lifetime boundaries, reuse existing move regressions, and avoid duplicate
per-move matrices. Full opcode coverage is necessary but does not by itself
verify native function contracts. Redundant new spear scenarios were removed;
the existing spear regressions now exercise the VM and a small lethal-contact
regression protects our cleanup bridge.

## Shared cursor and playback expansion

Following mk4decomp `fight_script_vm.c`, `MoveListCursorCluster`, and
`PauseTestCmp2CallStore`, the runtime now supports packed absolute jumps,
16-way character selection (including the retail Kitana/Noob remaps), counted
reverse playback, reverse completion, cursor submission and the optional clip
prefix on forward-to-sample playback. Eleven more Scorpion normal-attack roots
now execute original words and reuse their existing native tails: high-kick
startup/recovery, sweep, uppercut, crouching LK, close HK/HP, combo HP, and the
three roundhouse entry streams. Their generated bodies are removed.

Coverage is 17 interpreted roots and 53 retained generated roots. Verification
uses the small shared-runtime suite (including one cursor/control-flow fixture),
strict source coverage and canonical generation checks. No new character move
or balance tests were added. Runtime development should primarily follow the
actual mk4decomp interpreter and matching handler assembly, not duplicate the
old frame compiler's execution model.

## Complete Scorpion script-body migration

All 70 reviewed Scorpion roots now execute retail image words. The remaining
53 generated bodies and `game/scorpion/generated/scripts.inc` are removed;
`frames.inc` was already removed in the spear step. Native callbacks, physics,
projectiles and scene coordinators remain C because the retail scripts call
native routines for those operations. Per-fighter native organization is still
present; this step removes script-to-C execution, not every native adaptation.

The runtime adds predicate/contact branches, opponent clip binding, paired
playback, physics installation, landing waits, task parking and the remaining
reviewed native opcode boundaries. mk4decomp `fight_script_vm.c` supplies the
fetch/dispatch and dirty-bit branch contracts; its matching
`pending_match_variants.c` preserves the paired wait state machine (upstream
0x45bb70, local 0x45bb10), including the final reschedule. Local bounded handler
queries resolve revision differences. `anim_frame_advance.c` supplies the
separate cursor/completion model. No upstream global arena or host execution
of image addresses is introduced.

Native action cleanup transfers between VM and native playback without firing
at a continuation boundary; cancellation releases it once. Independent scene
actors now also resume the shared VM. The fan's native coordinator re-enters
its retail repeat stream instead of expanding a generated frame phase.

Validation: C11 runtime suite and game build, strict Scorpion inventory audit
(70 interpreted, zero generated/legacy, zero errors), canonical script/control
checks and native source/reachability checks. One compact shared fixture covers
predicate flags and cleanup ownership; no character scenario matrix was added.
This is coverage of Scorpion's reviewed roots, not support for every retail
opcode or the full original tagged scheduler. Other fighters still use their
existing generated bodies.
