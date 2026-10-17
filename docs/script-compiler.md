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

Make discovers all profiles in `tools/ports/`. Every profile participates in
generation, canonical regeneration checks and the generated-only audit. There
are no character exceptions in the Makefile. Liu Kang passes with 55/55 generated
roots and zero legacy roots: the original 50 plus five throw/fan continuations
found while following native script entries. Detailed JSON still reports every
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

`tools/ports/<character>.json` declares generated script roots, output include,
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
`sample` and an evidence string naming the native entry. Compilation seeds its
initialization/cursor analysis from that contract; generated C checks the actual
clip and sample before rebinding actor-owned storage. Missing context fails
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
Paired clip binding/counts, sample-start playback, landing waits and optional
full playback use shared engine helpers. Typed native adapters retain ownership
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

All catalogued Liu Kang VM roots run from `game/generated/liu_kang.inc`:
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
