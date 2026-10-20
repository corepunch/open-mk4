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

Validate real move entries, hit/block/miss, bounded recovery, both actor slots,
interruption and reset. This is not a claim of retail per-tick scheduler identity,
full Scorpion coverage, generic tagged returns or paired-task support.
