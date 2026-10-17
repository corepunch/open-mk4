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

# Strict reviewed coverage; new ports must additionally have no legacy VM roots.
python3 tools/compile_retail.py --character liu_kang --audit --strict --summary
python3 tools/compile_retail.py --character liu_kang --audit --strict --require-generated --summary
```

Make discovers all profiles in `tools/ports/`. A new profile automatically
participates in generation, regeneration checks and strict audits. The Makefile
requires fully generated VM coverage for new profiles; Liu Kang is the explicit
reviewed pre-compiler migration baseline.

The last command currently returns 1: Liu Kang has five generated roots and
45 reviewed existing legacy roots. That is deliberately different from a clean
reviewed coverage audit. Unsupported graphs and all helper call sites remain
visible in JSON; they are never silently emitted as empty functions.

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
and evidence. Bindings may also explain a portable adaptation. A physics
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
reviewed existing legacy bodies, stance/weapon/reaction adapters and a reviewed
inventory fingerprint. New ports should have no legacy VM bodies. The Liu Kang
legacy entries identify pre-compiler ports and expose the remaining migration.

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
Generated roots must compile, have references in the integration source, be
included there, and match canonical regeneration. A script cannot be both
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

Some scripts inherit their clip or task-loop state from a native continuation.
Those are currently rejected unless their compiled graph establishes it itself;
add an explicit typed entry-context lowering before migrating such a root.
Yield analysis is conservative: uncertain cursor-only loops remain unsupported.

The shared clip primitives use the engine's existing cursor stepping. Counted
playback yields once per visit, full forward/reverse playback includes its final
sample, and a target request already passed in that direction does not rewind.
The controller return has one actor-tick handoff; tagged return uses the
profile's declared continuation. These preserve the portable engine's tested
behavior, not exact retail scheduler ordering or pose interpolation. Actor-owned
`playback_move` storage survives suspension and cannot dangle when a fiber is
cancelled. Native adapters retain responsibility for physics/object lifetimes.

## Liu Kang proof and validation

Live command entries run generated roots `4e6d08`, `4e60b8`, `4ed678`,
`4ed6c0` and `4ed6f8` from `game/generated/liu_kang.inc`. They cover explicit
sample/wait sequences, contact-dependent recovery, counted loops, full reverse
tails, sound events and independent projectile spawning. Existing entry gates,
object callbacks and landing helpers are reused. Handwritten equivalents for
those script bodies and duplicate normal timing/contact data were removed.

`tests/test_script_codegen.py` checks graph preservation, malformed and unknown
boundaries, definite initialization, wait-free cycles, selector specialization,
optional descriptor prefixes, deterministic generation, atomic validation
failures, real C11 compilation and execution of both emitted conditional paths.
Coverage mutation tests catch dropped release tables, changed CPU targets,
missing implementation references and wrong reaction adapters.

`tests/test_liu_kang.c` exercises real inputs in both slots: generated LK
hit/block/miss damage and 33/36-tick recovery, the 42-tick crouching-HK tail,
fireball release timing, independent projectile interruption, crouch/block/armed
reactions, air landing and allocation failure. Shared scheduling is covered once
by the existing fiber suite. `make check` includes regeneration, strict reviewed
coverage, compiler tests and existing game/asset/archive regressions.
