# MK4 named scripts and data

Fighter scripts are retail wordcode in [`../fighters/`](../fighters/README.md).
The shared C interpreter in `game/script/vm.c` executes the original opcode
numbers, packed arguments, branches and cooperative waits. Native C services
live once under `game/native/`.

`make scripts` links `mk4/project.asm` into `build/mk4.data`, generates
`build/mk4_linked.h` for C callers and compiles script names for diagnostics.
`make game` does this automatically and recompiles consumers when exported
offsets change. Normal builds and startup require no EXE. `FILESYS.DAT`
still supplies meshes, textures, animation payloads and sampled audio.

## Sources and regeneration

- `../fighters/<name>.asm`: named bytecode, native ABI IDs and embedded aliases
  for one fighter; shared streams are defined once in
  `../fighters/shared.asm`.
- `mk4/program.asm`: generated include order for fighter bytecode.
- `mk4/tables.asm`: include order for data records in `mk4/tables/`.
- `mk4/constants.inc`: named encoding fields.
- `mk4/exports.inc`: shared data exports and aliases.
- `mk4/native.inc`: shared ABI include entry point; native declarations live
  beside bytecode and are exported by the linker.

```sh
make scripts-export        # decode MK4/MK-4.EXE and regenerate named bytecode
make scripts-source-check  # compare generated files without writing
make scripts               # assemble checked-in source; no EXE needed
make scripts-check         # source/word verification, strict audits, native naming
```

`tools/mk4_script_layout.json` preserves established entry names, original
addresses and native IDs. Names and catalog usage determine
file ownership through tooling. All authored operations and operands come
from the hash-checked EXE. No per-fighter C implementation or handwritten
configuration is generated. Regrouping inserts explicit jumps for original
fall-through; native calls, loops and embedded operand entries retain their
semantics. Verification compares every retained word after typed relocation.

To inspect a separate export:

```sh
python3 tools/import_mk4_data.py --scripts --output /tmp/mk4-scripts
```

This produces fighter files, their link index and the opcode header, not a
second copy of the data tables. To build that export separately, supply the
same `scripts/mk4/` table/project sources. The flat research import remains:

```sh
python3 tools/import_mk4_data.py --output /tmp/mk4-flat-import
```

It checks every retained data/instruction word and typed relocation from the
EXE. Raw asset references follow loader structure layouts; numeric magnitude
never classifies a Q16 scalar as a pointer. `tools/mk4_import_layout.json`
preserves original table names/locations and discovered script roots.

## Data locations

| Data | Source under `mk4/tables/` |
| --- | --- |
| Fighter rigs, resources and scale | `roster.asm` |
| Player/button routing and command lists | `command_tables.asm` |
| CPU choices and finisher selection | `cpu_finishers.asm` |
| Contact bounds, reactions and damage | `attack_records.asm`, `contact_geometry.asm` |
| Weapon inventory and playback parameters | `weapons.asm` |
| Idle/walk clips and query routing | `stance_and_queries.asm` |
| Clip frame counts and format | `animations.asm` |
| Scene, body-part and fighter resources | `scene_assets.asm`, `body_assets.asm`, `fighter_assets.asm` |

File headers and record comments name the C reader defining each format.
Some modules retain neighboring, unclassified data; their headers say so.
Shared records and scripts remain single definitions. Names describe verified
roles or structure, not recovered original source names. Keep established
`unresolved_*` handles until evidence supports a better name.

## Source syntax and runtime contract

```text
.section script
example:
  wait 5
  scene_active 0
  sound_c 2
  ready 0
```

The first instruction word stores the opcode in its upper byte and an inline
argument in its low 24 bits. Additional operands remain 32-bit words, retaining
Q16 values and stable native identifiers. `word(label)` encodes a packed script
or data reference; a plain label is a byte reference. `clip(N)` encodes the
linked animation descriptor. Labels, exports and aliases resolve after layout;
there are no fixed `.org` or `.expect` requirements.

`.word` stores 32-bit fields, `.zero N` reserves aligned bytes, and
`.alias name label + offset` names a field or embedded script entry. `.string`
writes ASCII, a terminating NUL and padding to a four-byte boundary. Semicolons
start comments outside strings; a trailing backslash continues operands.
Changing an instruction containing an embedded entry also requires checking
that alias's offset and no-op operand interpretation.

`.export_equ NAME value` defines a constant and exposes the same name to C in
one line, for example `.export_equ MK4_NATIVE_JAX_MISSILE 0x496f80`.
It replaces `.equ NAME value` followed by `.export NAME NAME`. Use `.equ` for
private constants and `.export C_NAME label` when the C interface name differs
from its source label. These are build-time directives, not retail VM opcodes.

Script labels and executable embedded aliases automatically become C constants
with the `scr_` prefix. Assembly uses `quan_chi_lk_throw_startup` directly;
C uses `scr_quan_chi_lk_throw_startup` from `mk4_linked.h`. No script `.export`
line, intermediate C enum or redundant JSON export mapping is needed. Native
function names remain distinct from script addresses, even when the bare names
match. Data exports retain their existing interface.

The host loads one package; `--data PATH` selects a compatible one. The loader
checks package shape, compiled layout identity and executable-word membership.
Data and script addresses are checked indices, never host pointers. Script
entries may legitimately begin inside an operand word, as in the original VM.
Guards/data are not executable. Compiled diagnostic names retain meaningful
labels without a runtime Lua symbol map.

Trust retail-authored fighter logic; tests target the shared runtime/native
boundaries we implement. `make bytecode-test` runs the existing button, command,
victory, finisher, CPU and random-input families twice and compares simulation
state each tick, including script cursors. `make check` includes the runtime,
strict all-roster audits, import/link verification and engine/asset checks.
