# MK4 editable scripts and data

`make scripts` links `mk4/project.asm` into `build/mk4.data` and generates
`build/mk4_linked.h`. `make game` and `make viewer` do this automatically and
recompile native callers when exported offsets change. Building and running
requires no EXE. `FILESYS.DAT` still supplies meshes, textures, animation payloads
and sampled audio.

- `program.asm`: executable wordcode with named instruction labels.
- `tables.asm`: roster, commands, CPU, reactions, animation descriptors, rig,
  scene, sound and native parameter data. Unknown retained data stays literal.
- `exports.inc`: C-visible labels and aliases into shared instruction operands.
- `native.inc`: stable native ABI IDs resolved by the C registry. These numbers
  identify services; they are never memory addresses or host function pointers.

Edit instructions or table values, then run `make game`. There are no `.org`
or `.expect` constraints. The linker resolves references after layout, including
references from tables to scripts and from native C services to script entries.
Inserting a wait or changing a branch does not require adjusting addresses.
Table record shapes and counts remain the MK4 format contract; adding a roster
slot or changing a record's fields also needs corresponding reader support.

```text
.section data
example_table:
  .word word(example), 3
.section script
example:
  wait 3
  jump 0, word(done)
done:
  return 0
.export EXAMPLE_ENTRY example
```

An instruction takes a 24-bit inline field followed by its operands.
`word(label)` encodes a packed reference; a plain label is a byte reference.
`clip(14)` refers to animation descriptor 14. `.word` stores 32-bit fields,
`.zero N` reserves aligned bytes, and `.alias name label + offset` names a field.
Separate script blocks have non-executable guard words, so falling through a
missing terminator cannot silently enter the next program.

The host loads one package. `--data PATH` selects a package with a compatible
layout. Its layout identity must match the header compiled into the application;
a mismatch fails with a request to rebuild. Content-only edits keep that identity.
The package's instruction bitmap prevents execution of ordinary data words.

The importer is an explicit research operation, never a build dependency:

```sh
python3 tools/import_mk4_data.py --output /tmp/mk4-import
```

It verifies the original EXE hash, uses the existing decoder and original fixup
records, and decodes raw asset references using the loader's structure layouts.
It checks every retained data word and instruction after relocation. It never
classifies a Q16 scalar as a pointer merely because the number resembles one.
`tools/mk4_import_layout.json` preserves original names/locations for reproducible
research; normal linking never reads it. Unused original data is retained for
research, without claiming every unreferenced structure is understood.

`make scripts-check` validates current source links and relocation contracts,
then audits the original catalog and native bindings. The original-EXE audit
remains provenance/registration evidence; it is not a comparison of edited
script behavior. Shared C VM and integration tests validate our runtime.
