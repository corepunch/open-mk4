# Fighter bytecode

Each `<fighter>.asm` contains generated, named retail script words, that
fighter's native ABI ID declarations, embedded-entry aliases and C exports.
`shared.asm` defines shared or unclassified streams once. Files are assembled
into one immutable program and executed by the same C VM; file ownership does
not introduce a fighter-specific dispatcher or duplicate native services.

```sh
make scripts-export       # regenerate from the hash-checked MK4/MK-4.EXE
make scripts-source-check # compare without writing
make                      # assemble checked-in wordcode and build the game
make bytecode-test        # run all existing scenario families twice
```

Names and native IDs come from the existing research/import metadata in
`tools/mk4_script_layout.json`. The exporter reads behavior through the existing
retail decoder. It places explicitly named streams with their fighter, then
uses catalog ownership for otherwise generic labels; ambiguous streams remain
shared. Kitana and Noob keep their established retail sharing/remaps instead
of receiving invented script copies.

For example, `sonya_fatality_2_second_cycle` and
`MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END` are both in `sonya.asm`. C callers
see only exported identifiers through the generated header. Other bytecode
can branch to the same definition by label. Native C implementations remain
in shared capability modules under `game/native/`.

Native constants use one build-time declaration, such as
`.export_equ MK4_NATIVE_JAX_MISSILE 0x496f80`, to define the value and expose
its existing name to C. No separate self-export line is needed.

The instruction set and packing are the original MK4 word format. A mnemonic
such as `wait 5` assembles to the original opcode/argument word. The interpreter
keeps a cursor, wake tick, loop counter and native/playback state per actor;
no Lua or stackful coroutine runtime is involved. Explicit jumps preserve
fall-through when source blocks are grouped into different files. The exporter
verification compares all original instruction words and typed references
after relocation, including entries inside operand words.

See [script/data format](../scripts/README.md) and
[architecture](../ARCHITECTURE.md). Do not hand-write fighter move behavior
or duplicate native services when updating the export.
