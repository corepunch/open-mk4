# MK4 script source

`mk4/program.asm` is editable, checked-in MK4 wordcode disassembly. `make scripts`
assembles it into `build/mk4.scripts`; ordinary builds depend on that package.
Assembly uses no EXE, disassembler, generated C, or per-fighter implementation.
The host explicitly loads the package before initializing a match;
`mk4-game --scripts PATH` selects a different assembled package.
The shared C interpreter executes these assembled words. Original EXE data is
still needed for animation descriptors, roster/command tables, assets and other
native parameters. Script fetches outside the assembled word set fail instead
of silently executing scripts from the EXE.

Each instruction has a mnemonic, a 24-bit inline field, and its operands:

```text
.org 0x4d0000
example:
  wait 3
  jump 0, word(done)
done:
  return 0
```

`word(label)` produces a packed word address; `clip(14)` refers to animation
14's descriptor. Plain symbols are byte addresses (including native calls).
Optional-animation instructions consume a descriptor only when inline byte 1
is 2. All inline bits are preserved, including bits unused by a handler.
`symbols.inc` names external data and native ABI entries. Mnemonics and operand
counts are defined in `tools/mk4_asm.py` and checked against the existing decoder.

The first import preserves the PC image's address layout: EXE tables and native
continuations still reference these addresses. Labels resolve within each
contiguous block; `.org` anchors disjoint blocks. Inserting instructions must not
cross the next occupied block or invalidate an externally referenced entry.
`.expect` assertions prevent catalogued/native entry points from moving silently.
Changing a native/data entry location requires updating its references too.
Nine alternate entries share a physical operand/table word; they are explicit
aliases, not duplicate instructions. The source represents the programs reached
from all catalogued roots and named runtime script entries, not a claim that all
unused code in the executable has been discovered.

For a deliberate fresh import, use a new destination and review the diff:

```sh
python3 tools/import_mk4_scripts.py --output /tmp/mk4-script-import
```

Import verifies the supplied EXE revision, reuses the existing decoder, and
checks an exact word round trip. It never runs during a normal build and refuses
to overwrite an existing destination. `make scripts-check` audits the assembled
program with the original data tables. Runtime integration tests compile a small
modified source and execute it through the same C VM, in addition to checking
malformed packages and script/native lifetime contracts.
