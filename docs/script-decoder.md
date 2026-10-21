# Script decoder

The decoder reads the supplied EXE; it does not execute retail code. Decoded
operand layouts and native dependencies are evidence for a C port, not proof
that a move's semantics or implementation are complete.

Start with the shared-runtime inventory and coverage audit described in
[script-compiler.md](script-compiler.md). `tools/compile_retail.py --inventory`
reads typed retail roots; `--audit` checks interpreter opcodes, typed native
roles and reviewed integration boundaries. The former script-to-C/frame
emitter is removed. The decoder's `--c` and `--port` views are read-only
research/navigation reports. Extend the shared interpreter or one native
service for an unresolved boundary; execute the original words directly.

## Targeted queries

Run from the workspace root. Output goes to stdout; `--output FILE.md` saves
only the requested report. None of these queries reads the asset archive or
creates a bundle. Native queries and `--port` worksheets need Capstone.

```sh
python3 tools/disassemble_retail.py --list-moves
python3 tools/disassemble_retail.py --character sub_zero --list-moves
python3 tools/disassemble_retail.py --character sub_zero --move ice
python3 tools/disassemble_retail.py --character sub_zero --move ice --port
python3 tools/disassemble_retail.py --move spear --port
python3 tools/disassemble_retail.py --character sub_zero --action 0x1007
python3 tools/disassemble_retail.py --character sub_zero --move low_kick --c
python3 tools/disassemble_retail.py --move spear
python3 tools/disassemble_retail.py --script 0x4e6d08
python3 tools/disassemble_retail.py --move low_kick --c
python3 tools/disassemble_retail.py --native 0x46bf10 --limit 80
python3 tools/disassemble_retail.py --attack B 6
python3 tools/disassemble_retail.py --attack C 3
python3 tools/disassemble_retail.py --attack D 5
python3 tools/disassemble_retail.py --attack weapon 2
python3 tools/disassemble_retail.py --attack record 0x4ebeb8
python3 tools/disassemble_retail.py --script 0x4e9d00 --raw --output build/spear-victim.md
```

Addresses are hexadecimal, with or without `0x`. Attack selectors accept
decimal or `0x` hexadecimal. `--limit` defaults to 80 instructions, with a
maximum of 512. Truncation and unresolved boundaries are reported explicitly.
Native queries decode from the given entry without a whole-binary discovery
pass or recursive call expansion. Follow printed dependencies with another
query. `--character NAME` (or roster ID 0..17) selects scripts and both players' command lists. It defaults to Scorpion. Selectors remap Kitana to Sonya and Noob to Reiko; nested selectors keep that context. Keep the option on follow-up script queries. P2 Up uses table `53cb10`; `53ca80` belongs to LK release.
Raw mode includes fixed script words or native instruction bytes.

Running without a query shows help. The old bulk export now requires explicit
`--bundle`; it remains available for research, separate from the query path.

## Action-code catalog

`--action CODE` accepts hexadecimal with or without `0x`, such as `1007`,
`1006`, `300`, or `2001`. It reports the C identifier, descriptive confidence,
selected native writers/readers checked against exact instruction bytes,
decoded script writers for the selected roster, and direct portable C action
assignments/comparisons with file/line links. `--limit` bounds displayed matches;
`--output` saves the report. No archive, Capstone, game execution, bulk bundle,
or upstream checkout is needed. Follow the printed native/script queries for
context. The CLI does not accept `--c` or `--raw` for action reports.

`game/retail_actions.inc` is the shared catalog, included as a C enum and read
by `tools/retail_actions.py`. Neutral identifiers such as `ACTION_RETAIL_1007`
preserve the number even when a description is inferred. Verified means the
recorded use is established, not that every use of that value is understood.
Unknown values remain explicitly uncatalogued. Coverage includes the
direct action literals in the portable game, with unresolved descriptions
where no stronger meaning has been recorded. Native sites are deliberately
selected, not an exhaustive executable index; lexical C navigation does not
cover indirect assignments or masked classifications. Script roots are the
existing character catalogs, each with the decoder's 512-instruction bound.

`tools/retail_action_evidence.json` records 34 additional inspected writer spans
for normals, air attacks, specials and weapons. Every report checks those exact
bytes against this EXE. Tests also decode each span and verify the literal
written to controller `+0x74`; changing the input value fails evidence validation.
Action reports link checked mk4decomp assembly correspondences where available.
Both character modules and their weapon routines use the shared C identifiers.

For `1007`, projectile recovery is an inference supported by ice/spear script
placement and the spear owner-action check. The upstream name
`RoundOverFsmCluster` for that checking routine is misleading: our checked
caller is the spear victim continuation. Do not derive gameplay flags or a
complete semantic mapping from that upstream name or the code's bit pattern.

## Readable C-style view

`--move NAME --port` collects the current engine entry and nested frame-program C
functions, supporting data links, exact script words/decoded operand roles,
contact-record C initializers, and native literals with their instruction bytes.
It includes helper contracts, action evidence, and checked mk4decomp source and
matching-assembly links. The C bodies come from the workspace; they are review
and extraction fragments with engine/data dependencies, not automatically
decompiled x86 or a standalone translation unit. Same-file callback arguments
are included. Dynamic button/weapon callbacks and engine calls remain explicit
source boundaries. No other character's C body is substituted for an unported
character.

The worksheet uses separate budgets of `--limit` C source lines, decoded script
instructions and native instructions (default 512 each). Native entries share
one budget and are not recursively expanded; omitted routines retain exact
follow-up queries. C bodies are emitted completely or linked, never sliced
mid-function. Native operands are not automatically labeled Q16, actions or
function arguments based on their numeric value. In particular, group field
`+0x74` can hold motion data where a controller's `+0x74` holds an action.
`--port` requires `--move`, supports `--output`, and excludes `--c`/`--raw`.

Use `--character sub_zero --move ice --c` or `--script 0x4ed588 --c`
for complete C-like control flow over the decoded graph. This view supports all
researched opcode layouts, both outcomes of conditional branches, backward
jumps, shared blocks, counted loops, selected-character jumps, cooperative task
spawns, callback installation, native tails, tagged returns, controller returns
and parked tasks. Labels/gotos preserve the graph without guessing structured
loops or silently falling through into an unrelated block. Character selectors
are specialized to the requested roster, including Kitana/Noob remapping.

The C view defaults to a 512-instruction total budget; other queries default to
80. An explicit `--limit` still applies. Every omitted edge ends in a named
continuation boundary, so truncated output cannot look like a complete path.
Unknown opcode paths terminate explicitly while other decoded paths remain
visible. An unresolved native helper never stops printing later VM instructions.
Its exact handler address, inline value and fixed operands remain available in
the C call and dependency list. This is not automatic x86-to-C decompilation.

Waits and playback operations suspend/resume. Adapt them to the existing engine
continuations; do not turn them into blocking host calls or implement another
VM. Native calls can suspend too. Task/callback installation does not call its
target inline. The VM has one task loop counter, including when entered midway
through a loop. Predicate opcode 36 branches on result flag 4; opcode 55 passes
the low-byte argument and branches on flag 1. Wait opcode 37 separately tests
flag 1 and is not assumed to mean hit/miss.

Adjacent sample submission and wait operations combine as
`show_sample_for_ticks(sample, ticks)` only when no branch enters the wait.
Clip binding, native calls and other operations stay in order. Contact records
and source links follow the code. Use the detailed view for every original
address and fixed word. `--c` needs no archive or Capstone, supports `--output`,
and cannot combine with `--raw`.

## Function names and mk4decomp sources

Script operands and dependency lists now include build-specific research names,
for example `native_target=0x48d3f0 (contact_query_b)`. Native queries annotate
direct calls and jumps with names as well. Addresses remain visible, and unknown
targets remain addresses. These names are descriptive annotations, not recovered
original source names or proof that the helper has been fully understood.

`tools/retail_symbols.py` loads the checked-in `retail_symbol_map.json`,
produced from `reference/mk4decomp/config/symbols.yaml` and its matching assembly:

```sh
python3 tools/map_retail_symbols.py
```

The mapper accepts unique complete instruction-shape matches, preserving
registers, operand widths, small constants, internal branches and consistent
one-to-one address relocations. It does not use numeric address equality or a
uniform revision offset. The initial map contains 355 mapped symbols; repeated
wrappers remain ambiguous, and functions without matching assembly remain
unmapped. The JSON records both addresses, source files, instruction counts,
EXE/source fingerprints, ambiguity candidates and unmatched names. Ordinary
queries read this compact map without scanning reference sources or requiring
the upstream executable. Rebuild it explicitly after updating the reference.

Queries link both the upstream C file and the matching assembly used as evidence.
An entry inside a mapped cluster is labeled as an interior entry, retaining its
local address and container offset; it is never presented as the cluster's
function entry. For example our ice entry `49bc60` lies inside the locally mapped
`49bc00` container; upstream `49bc60` names that container itself. Existing
manually checked embedded helper correspondences remain available as well.
Names and source links are navigation aids, not recovered signatures or proof
that a helper's complete behavior is understood.

To extend coverage, inspect the local helper with `--native`, compare its
operations and callers with the reference, then add its descriptive alias and
source correspondence with the evidence recorded. Keep unresolved semantics
visible. The upstream contributions are under the reference project's
[MIT license](../reference/mk4decomp/LICENSE).

## Actionable helper contracts and native C views

`tools/retail_helpers.py` is the shared semantic catalog. Each researched local
entry records inputs/context ownership, effects, timing/control transfer,
related helpers, data tables and an existing engine adapter where available.
Queries prefer these semantic aliases while retaining the mapped upstream name
and both addresses in source references. Add discoveries here once, so later
characters do not repeat Sub-Zero's helper research.

```sh
python3 tools/disassemble_retail.py --character sub_zero --move ice --c
python3 tools/disassemble_retail.py --character sub_zero --move clone --c
python3 tools/disassemble_retail.py --native 457b50 --c
```

Move C pages include decoded command records and predicates, all selected script
paths, contacts/reaction entries, follow-up buttons and deduplicated helper
contracts. Native-only moves include the relevant portable C definitions and
continuations instead of failing with "no script roots". `--native ADDRESS --c`
shows its contract, one explicit level of related helpers, decoded tables and
the current engine C adapter. Definitions are read from their actual source,
with file/line links, rather than maintained as a second implementation.
**These bodies are existing portable adaptations, not automatically decompiled
native x86 or proof of retail parity.** Unknown helpers remain explicit native
boundaries and never receive a fabricated C body. No source changes, game
execution, recursive native closure or bulk bundle generation occurs.

`tools/retail_helper_view.py` decodes typed table layouts from the checked EXE:
ice spawn/animation/trail, closed scene rigs, X,Z,Y camera paths, pulse shakes,
weapon descriptors/bounces, indexed sound tokens and blood splashes. A limited
table is marked incomplete. Playback, object lifetime and scene clocks remain
separate; generated code is intended for adaptation to the shared C engine.

Recovered distinctions include opcode 36's result flag 4 versus opcode 55's
flag 1, task-spawn opcode 40 ignoring its inline byte, opcode 25 using sound
lookup `4ead30` rather than bank B, and `405430` always succeeding in this EXE.
The indexed sound helper links to the common `sound_event` resolver. Queue it
with `battle_sound(SOUND_INDEXED, selector)`; assets load all 66 tokens,
selectors above 65 and zero entries are silent, and sampled tokens resolve
through division by five. Selector 4 is token `0x852`, sample 426. Keep the
indexed cue at its script position, separately from later A/B/C events.

Direct table decoding separates the spawn rig from the animated track:
`4ed5d0` initially creates mesh 74, but `4ed898`'s track records point to rig
records whose mesh fields are **75–82**. The portable projectile keeps its
initial mesh separately and begins the eight-frame loop on its first animation
tick; mesh 74 is never replayed in that loop. Camera tables skip their leading
parameter word; weapon bounce triples start after their floor header. Focused
checks protect these layout distinctions against repeating the earlier
hand-transcription mistakes.

## Compact move pages

`--move NAME` gathers a curated set of attacker/victim script roots, actual
command records and predicates, contact records and their reaction-table
entries, known follow-up button tables, and native helpers. `--list-moves`
lists the selected character's available navigation roots. Pages work without Capstone and print or save one
Markdown report, with up to six preview instructions per script (subject to
the overall `--limit`). Follow the printed `--script` and `--native` commands
for details. Branches in previews retain their source addresses.

The catalogs in `tools/scorpion_catalog.py` and `tools/character_catalog.py` are deliberately small and explicit. Scorpion and Sub-Zero have character-specific roots; the other roster choices expose selected shared roots for research, not completed special-move inventories.
It is not a transitive call graph: a missing script, contact record or cancel
table does not mean the move lacks that behavior. Counts describe selected
roots only, and native semantic dependencies remain visible even when no
unknown opcode was encountered. Port status belongs in
`docs/character-script-port.md`; it is not hardcoded into decoder output.

For example, the sword back-slash pages decode weapon contact `0x5e` and then
report the unresolved `0x42` operation and its handler. They do not skip it or
claim complete move coverage. Extend the catalog and opcode semantics as
actual porting work reaches new paths, rather than expanding the whole engine.

## Additional opcode evidence

These layouts were checked from exact-entry x86 handlers in `MK4/MK-4.EXE`.
Addresses refer to the revision checked by `RetailImage`, not the upstream
reference checkout. Unknown operations still terminate their path explicitly.

| Opcode | Handler | Following words | Behavior established from the handler |
| --- | --- | --- | --- |
| `0c` | `45a510` | 1 packed clip descriptor | Set cursor to inline low byte; bind descriptor, submit pose and increment cursor through `4285b0` |
| `26` | `4596a0` | 0 | Low-byte argument to indexed sound helper `48a100` |
| `2a` | `4595a0` | 0 | Low-byte argument in scratch `53c43c`, call spawn helper `457b50`; its effects remain a native dependency |
| `30` | `459820` | 0 | Tail to tagged continuation pop `41f780`; no script fallthrough |
| `37` | `4598b0` | 0 | `45c020` unpacks two 12-bit waits; result bit 0 clear selects low 12 bits, set selects high 12 bits |
| `51` | `45ab30` | 0 | Query B (`48d3f0`) with low-byte selector; record table `53d2b8` |
| `5a` | `45afb0` | 1 signed Q16 value | Fetch through `45bff0`, negate, pass to horizontal speed helper `490390` |
| `5e` | `45abb0` | 0 | Weapon query (`48d430`) with low-byte selector; record table `53d0c0` |
| `61` | `45b3a0` | 0 | Call `490330`, clearing bit 0 of fighter group flags at `+40` |
| `62` | `45b310` | 0 | Low-byte argument to gated indexed helper `40a470`; its effects remain a native dependency |

This removes the opcode stops in standing LK (`4e6d08`), crouching HK
(`4e60b8`), spear victim (`4e9d00`), teleport victim (`4e9938`) and fire
victim (`4e97f8`). Native tails and dynamic continuation returns remain explicit
work; decoding past a wrapper does not resolve its transitive behavior.

Liu Kang weapon opcode `6c` (`45b050`) stops current motion, unpacks the
inline low/high 12-bit delay/duration through `45c020`, consumes one signed
Q16 speed word through `45bff0`, and installs body callback `494860`. It
does not wait in the script; pose playback and scheduled travel overlap.
The decoder now follows `4ec680` through its HP continuation branches.

Opcode `56` (`45ac30`) masks its inline low byte and invokes scene-event
request `4315b0`, then resumes dispatch. It consumes no following word.
This reaches Liu Kang's `4dfff8` setup events without treating them as waits
or combat contacts.

Command queries now include `low_kick_release`, using P1/P2 tables
`53ca38`/`53ca80`. Liu Kang slot 3 selects `4e49a8` (47eaf0, predicate
45eab0); Scorpion and Sub-Zero release lists remain empty. Press lists alone
are insufficient to establish the special-move inventory.
