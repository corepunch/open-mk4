# Agent-readable retail disassembly

For character ports, start with the lightweight
[script and move queries](script-decoder.md). They print bounded reports and
need neither a generated bundle nor retail execution:

```sh
python3 tools/disassemble_retail.py --move spear
python3 tools/disassemble_retail.py --script 0x4e9d00
```

Explicitly generate the legacy research bundle only when needed:

```sh
python3 tools/disassemble_retail.py --bundle --output build/retail-disassembly
```

The tool needs Capstone 5 (tested with 5.0.7). It verifies the exact EXE SHA-256
before decoding and never executes the game. Outputs live outside the upstream
reference checkout. They are generated research material in the ignored build
directory; the generator and its regression tests are committed.

Start with `build/retail-disassembly/README.md`. The bundle contains:

- `scorpion.md`: both players' command records, predicates, native move roots
  and scope limitations.
- `scripts/*.md`: named Scorpion scripts and additional constant script entries
  found in native calls, with opcode names, inline fields, following words,
  clips, starts, stored counts, branches, native handlers and task callbacks.
- `vm-opcodes.md`: the researched handler-table region and operand layouts;
  unsupported operations remain explicit.
- `native/*.md`: native entry/basic-block graphs with original instruction
  bytes, local labels, incoming/outgoing references, callback stores, bounded
  switches, imported DLL function names, global aliases and data literals.
- `native-index.md`, `script-index.md`: address-based navigation.
- `unresolved.md`: register/table calls and jumps whose targets need further
  analysis. These are preserved, not treated as no-ops.
- `manifest.json`: machine-readable instruction/script graph, imports,
  script-entry references and unresolved edges.
- `clips.json`: all 612 descriptors, stored sample counts and AP0/AP1 ranges.
- `linear-code.asm`: full linear decode of executable sections, including a
  byte-preserving fallback for undecodable data. Embedded data can look like
  instructions; use the exact-entry graphs first.
- `data-words.txt`: all initialized non-executable section words, with startup
  fixups and original raw values where they differ. This preserves material
  beyond unresolved/computed script paths.

`--no-linear` omits the large executable-section fallback during quick
regeneration. It does not remove an existing fallback from a previous run.

## What the scripts are

The supplied retail move programs are binary word-based bytecode embedded in
`MK-4.EXE`, mixed with tables, native callbacks and parameter blocks. Startup
fixups turn some raw zero operands into packed pointers. A packed pointer is
a word index; multiply by four to obtain a byte address. The disassembler
preserves both forms.

The local reference provides reconstructed C/assembly. We have not recovered
the original text scripts or authoring tool. The binary proves the executable
representation; it does not establish whether the original authoring format
was text, assembly macros or another format.

A stored clip count does not supply move duration. Playback scripts can hold,
skip, reverse, loop, wait and call native code; native code can spawn tasks,
control physics, change the victim and run whole scene programs.

## Decoder checks and limits

The generator follows direct calls, literal callback-slot writes, constant
`push script; call script_enter` sites, and native switch tables with an
indexed comparison and unsigned guard. A modified index invalidates the
bound. Register-computed scripts and unbounded indirect dispatch remain open.
The root set includes shared action/VM/predicate tables, so this is a research
superset, not a list of moves all reachable by Scorpion in every state.

A branch into another known entry is a navigation boundary, labeled
`tail_or_shared_entry`. This is not recovery of original function boundaries,
original names, C signatures or source. Descriptive names annotate verified
addresses in this EXE; upstream addresses are not imported.

Structured script decoding stops on unknown operations or invalid operands.
The raw data fallback remains available. Opcode zero is a valid VM no-op even
with nonzero low bytes. The optional descriptor prefix for opcodes 0x47..0x49
is present only when inline byte 1 equals 2. Opcode 0x3e parks a task and must
not fall through into adjacent data. The 0x3b loop handler scans backwards by
words, including operands, for the preceding 0x3a marker. These details are
checked against the supplied x86 handlers.

`python3 tests/test_disassembly.py` checks instruction bytes/static closure,
retail switch bounds, callback and native-to-script references, generated
links, deterministic output, optional operands, loop/selector/park behavior,
explicit unknown paths and all VM fetches in the 27 saved Scorpion oracle
scenarios. Native tests skip explicitly if optional Capstone is unavailable;
script tests still run. The suite is part of `make check`.

This work provides readable evidence for the remaining port. It does not
implement specials, weapon combat or fatalities, and does not expand the
runtime verification claims recorded in `scorpion-retail-moves.md`.
