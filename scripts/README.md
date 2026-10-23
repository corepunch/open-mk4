# MK4 editable scripts and data

`make scripts` links `mk4/project.asm` into `build/mk4.data` and generates
`build/mk4_linked.h`. `make game` and `make viewer` do this automatically and
recompile native callers when exported offsets change. Building and running
requires no EXE. `FILESYS.DAT` still supplies meshes, textures, animation payloads
and sampled audio.

- `program.asm`: link-order index for the shared wordcode modules in `code/`.
- `tables.asm`: link-order index for the records in `tables/`.
- `constants.inc`: named encoding fields such as `WITH_CLIP`.
- `exports.inc`: C-visible labels and aliases into shared instruction operands.
- `native.inc`: stable native ABI IDs resolved by the C registry. These numbers
  identify services; they are never memory addresses or host function pointers.

## Where to edit

| Change | Source |
| --- | --- |
| Punches, kicks, air attacks and roster branches | `mk4/code/normal_attacks.asm` |
| Throw performer streams | `mk4/code/throws.asm` |
| Special attacks and recovery | `mk4/code/special_attacks.asm`, `special_openers.asm` |
| Projectile casting | `mk4/code/projectiles.asm` |
| Hit/block/get-up/captured victim streams | `mk4/code/reactions.asm` |
| Weapon attack streams | `mk4/code/weapon_attacks.asm` |
| Finisher tasks | `mk4/code/finishers.asm`, `transformation_scenes.asm` |
| Boss attacks and victories | `mk4/code/boss_attacks.asm`, `victories.asm` |
| Fighter rigs, resources and scale | `mk4/tables/roster.asm` |
| Player/button routing | `mk4/tables/command_tables.asm`; follow the named command-list reference |
| CPU choices and finisher selection | `mk4/tables/cpu_finishers.asm` |
| Contact bounds, reactions and damage | `mk4/tables/attack_records.asm`, `contact_geometry.asm` |
| Weapon inventory and playback parameters | `mk4/tables/weapons.asm` |
| Idle/walk clips and contact-query routing | `mk4/tables/stance_and_queries.asm` |
| Clip frame counts and packed format | `mk4/tables/animations.asm` |
| Scene, body-part and fighter resources | `mk4/tables/scene_assets.asm`, `body_assets.asm`, `fighter_assets.asm` |

File headers and record comments name the C reader that defines each format.
The include indexes preserve adjacency between imported records. Some modules
also retain neighboring, unclassified data; their headers say so. A shared
script or command list is defined once even when several fighters use it.

Names describe established use or structure: `high_kick_clip_66` identifies a
roster-selected stream by the clip it opens; `scorpion_body_rig_joint_2`
identifies a rig node by its stored joint ID, not a guessed anatomical part.
`unresolved_<module>_<number>` is a stable handle for a block whose meaning is
not yet established. Keep these names until evidence supports a better one.
They are not offsets and do not need renumbering when editing nearby records.
Catalog comments refer to `tools/character_catalog.py` and
`tools/scorpion_catalog.py`; they are research names, not recovered original names.
`.string` rows are used only for archive filenames and text blocks; a block of
printable bytes with no established text role stays a numeric `.word` row.

## Editing and syntax

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
`.string "sc_geo.geo"` writes ASCII, a terminating NUL and zero padding to the
next four-byte boundary. Semicolons start comments outside quoted strings.
A trailing backslash continues an operand list; roster selectors use this to
place one target and fighter name on each line.
Separate script blocks have non-executable guard words, so falling through a
missing terminator cannot silently enter the next program.

Most instruction inline fields are shown in decimal. Packed bit fields remain
hexadecimal; `WITH_CLIP + 13` explicitly requests the optional clip operand and
sets the low-byte argument to 13. It is an encoding flag, not a time unit.
The authoritative opcode schema is `tools/mk4_asm.py`; execution semantics live
in `game/script/vm.c`. Useful common forms:

| Form | Meaning in the shared runtime |
| --- | --- |
| `wait 20` | Suspend until 20 simulation ticks later |
| `open_clip 0, clip(28)` | Bind clip 28 at sample 0 |
| `play_clip 10, clip(28)` | Bind clip 28 and play to sample 10 |
| `forward_to 20` | Advance the current clip to sample 20 |
| `jump 0, word(recovery)` | Branch to a script label |
| `call 0, MK4_NATIVE_…` | Dispatch a native call through the C registry |
| `tail 0, MK4_NATIVE_…` | Dispatch a native continuation |
| `loop 3` / `repeat 0` | Counted loop; the VM has one live loop counter |
| `select_roster 0, …` | Sixteen packed targets; Kitana/Noob alias slots 2/7 |
| `ready 0` | Return through the controller continuation |

Do not confuse animation samples with ticks. Playback speed, native waits and
contact branches affect duration. Native services may suspend or transfer
control; a `tail` is not an ordinary script label jump.

Known tables use record-sized rows and field comments. Contact records contain
`reach, top, length, height` in Q16, then packed `hit/air/block/flags` bytes and
Q16 damage (a fraction of full health). Command records contain an input
pattern, extra input and a packed predicate/flags/window/action word; a zero
pattern terminates the list. Preserve record widths and terminators. Follow a
shared label's references before changing it, since several actions may use it.

The host loads one package. `--data PATH` selects a package with a compatible
layout. Its layout identity must match the header compiled into the application;
a mismatch fails with a request to rebuild. Content-only edits keep that identity.
The package's instruction bitmap prevents execution of ordinary data words.

## Import and validation

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
The importer produces the original flat research layout. It deliberately does
not overwrite the curated names, comments or file organization in this tree;
compare a fresh import in a separate directory when researching provenance.

`make scripts-check` validates current source links and relocation contracts,
then audits the original catalog and native bindings. The original-EXE audit
remains provenance/registration evidence; it is not a comparison of edited
script behavior. Shared C VM and integration tests validate our runtime.
