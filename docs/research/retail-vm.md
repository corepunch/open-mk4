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

## Implemented prototype

`game/retail_vm.c` reads the relocated image directly. It implements only the
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
