# Script decoder

The decoder reads the supplied EXE; it does not execute retail code. Decoded
operand layouts and native dependencies are evidence for a C port, not proof
that a move's semantics or implementation are complete.

## Targeted queries

Run from the workspace root. Output goes to stdout; `--output FILE.md` saves
only the requested report. None of these queries reads the asset archive or
creates a bundle. Only native queries need Capstone.

```sh
python3 tools/disassemble_retail.py --list-moves
python3 tools/disassemble_retail.py --move spear
python3 tools/disassemble_retail.py --script 0x4e6d08
python3 tools/disassemble_retail.py --native 0x46bf10 --limit 80
python3 tools/disassemble_retail.py --attack B 6
python3 tools/disassemble_retail.py --attack weapon 2
python3 tools/disassemble_retail.py --attack record 0x4ebeb8
python3 tools/disassemble_retail.py --script 0x4e9d00 --raw --output build/spear-victim.md
```

Addresses are hexadecimal, with or without `0x`. Attack selectors accept
decimal or `0x` hexadecimal. `--limit` defaults to 80 instructions, with a
maximum of 512. Truncation and unresolved boundaries are reported explicitly.
Native queries decode from the given entry without a whole-binary discovery
pass or recursive call expansion. Follow printed dependencies with another
query. Script queries currently select Scorpion in character dispatch tables.
Raw mode includes fixed script words or native instruction bytes.

Running without a query shows help. The old bulk export now requires explicit
`--bundle`; it remains available for research, separate from the query path.

## Function names and mk4decomp sources

Script operands and dependency lists now include build-specific research names,
for example `native_target=0x48d3f0 (contact_query_b)`. Native queries annotate
direct calls and jumps with names as well. Addresses remain visible, and unknown
targets remain addresses. These names are descriptive annotations, not recovered
original source names or proof that the helper has been fully understood.

`tools/retail_symbols.py` shares names between queries and bundle output. Its
small explicit correspondence table links checked helpers to the local
mk4decomp source, showing both our address and the upstream entry or embedded
offset. Initial links cover script entry and the low-kick script's task-field
clear/set and pose-wait helpers. Source links also appear in generated native
and script bundle pages; no bundle regeneration is needed for queries.

The reference checkout at `af88007` targets another executable revision. Never
apply `config/symbols.yaml` directly to our addresses or assume a single address
delta: our contact query B at `0x48d3f0` is an unrelated move-table selector at
that address upstream. The correspondence table was checked against local
exact-entry x86 listings and upstream source. Embedded low-kick helpers occur
in the matching assembly branch of `five_block_dispatch_variants.c`; its
`NON_MATCHING` C twin omits them. Function names alone therefore cannot replace
bytecode decoding or examination of the matching source branch.

To extend coverage, inspect the local helper with `--native`, compare its
operations and callers with the reference, then add its descriptive alias and
source correspondence with the evidence recorded. Keep unresolved semantics
visible. The upstream contributions are under the reference project's
[MIT license](../reference/mk4decomp/LICENSE).

## Compact move pages

`--move NAME` gathers a curated set of attacker/victim script roots, actual
command records and predicates, contact records and their reaction-table
entries, known follow-up button tables, and native helpers. `--list-moves`
lists 30 named entries. Pages work without Capstone and print or save one
Markdown report, with up to six preview instructions per script (subject to
the overall `--limit`). Follow the printed `--script` and `--native` commands
for details. Branches in previews retain their source addresses.

The catalog in `tools/scorpion_catalog.py` is deliberately small and explicit.
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
