# MK4 workspace

## Commits

Commit each completed, verified implementation step directly to `main` before
starting the next step. Treat a step as a coherent change with its relevant
validation, and keep separate steps in separate commits.

Use a clear commit subject and a detailed description explaining the problem
or request, what changed, why the approach was chosen, and how it was verified.
Record relevant test results and any remaining limitations. Stage only the
files belonging to that step, preserving unrelated work.

## Implementation language

Game, engine, viewer and native regression tests use C11. Follow the procedural
state/context boundaries and evidence rules in `ARCHITECTURE.md`. Keep SDL out
of combat simulation. The standalone `fsys.cpp` development tool may remain
C++17; it must use the shared C archive validator and filename hash.

## Code formatting

Use the repository `.clang-format` for project-owned C/C++ sources, headers,
and C include fragments. Use two-space indentation and keep opening braces on
the same line as functions, structs, and control statements. Put each statement
on its own line; expand function and control-flow bodies across multiple lines,
even when they contain only one statement. Use normal spacing around operators
and after commas, and wrap long expressions at the 160-column limit. Prefer
readability over packing code into fewer lines.

Keep data tables readable as one entry per row, preserving their column layout
even when a row exceeds 160 columns. Surround only these table initializers
with `// clang-format off: Keep table entries on one row, including long rows.`
and `// clang-format on` so formatting preserves the layout. Keep functions and
control flow outside these exceptions.

Run `make format` before validation and committing. `make format-check` checks
formatting without modifying files and also runs as part of `make check`.
The targets find `clang-format` on `PATH` or through `xcrun` on macOS; override
`CLANG_FORMAT` to select another installation. Do not format upstream reference
checkouts. Keep formatting-only changes free of behavior changes.

## Testing

Keep tests proportional to plausible failure modes. Simple arithmetic, data
tables and small helpers do not automatically need dedicated tests. Avoid tests
that only repeat implementation constants or assert every intermediate tick.
Derive expected behavior from retail evidence rather than copying the code.

For fighter ports, prefer a small set of behavioral regressions for damage and
recovery, important branches, interruption, and both player slots for paired
moves. Include KO/reset when actor or object lifetimes make it relevant. Exercise
real command or move entries. Test shared scheduling and engine behavior once,
then reuse that coverage across fighters instead of duplicating exhaustive
matrices. Run relevant existing checks; add tests only where they catch a
meaningful bug cheaply.

## References

Local reference checkouts live in `reference/`:

- `reference/MortalKombat.Tools/` — https://github.com/ermaccer/MortalKombat.Tools
  Tools for MK4 PC archives and sounds. Start with `fsysextract/` for the
  FILESYS.DAT entry structure, `fsyscreate/` for rebuilding archives, and
  `esfcon/` for ESF audio conversion.
- `reference/mk4decomp/` — https://github.com/nathan-casabieille/mk4decomp
  Reconstruction of the PC game. Start with `src/engine/fsys.c` for archive
  lookup and filename hashing, and `analysis/notes/architecture.md` for format
  analysis. Addresses may differ from the executable in this workspace.

For the SDL2 viewer, follow the actual source path:

- `src/boot/anim_load_pack_file.c` and `src/engine/anim_decode_bitstream.c`:
  AP0/AP1 directory, metadata and frame decompression.
- `src/boot/m_stack_bracket1_tree_walk_recursive2.c`,
  `src/boot/push_set_call_cleanup.c`, `src/engine/anim_pose_pipeline.c`:
  rig hierarchy, bone-ID lookup and packed angle channels.
- **`src/boot/m_stack_push_two_entry_chain_call.c` and
  `src/boot/scaled_mask_or_store.c`: required post-build transform setup.**
  The root becomes kind 9 (C); descendants become kind 10 (B). Keeping
  allocation-default kind 8 (A) mangles valid idle frames.
- `src/engine/rot_matrix_b.c`, `rot_matrix_c.c`, `render_scene_node.c`:
  joint matrices and hierarchy composition. `app_init_post_joy.c` initializes
  the historically named `g_sinTable` with **cosine**, despite older notes.
- `src/engine/state_dispatch_table.c`: idle selection and special roster IDs.

Some architecture notes contain superseded hypotheses. Verify against source
and the supplied executable; `tests/verify_original.py` checks the decoder and
rotation fixtures against original x86 routines. See `viewer/FORMATS.md`.

Treat reference checkouts as upstream research material. Implement workspace
tools outside `reference/`; verify format assumptions against `MK4/FILESYS.DAT`
and `MK4/MK-4.EXE`. Consult upstream licenses before copying code.

## Icon generation

Never draw icons separately. Generate every icon in a set together as a single
strip or atlas in one ImageGen call. When visual states are needed, draw normal,
selected, pressed, hover, and disabled states together in that same atlas. Use
the authored state artwork directly; do not derive visual states through script
post-processing.

## Character script ports

See [the fighter port workflow](docs/character-script-port.md#workflow) for the
end-to-end research, fiber implementation, validation and commit procedure.

Start character research with `python3 tools/disassemble_retail.py --move NAME`
(or `--list-moves`), then follow its bounded `--script`, `--native`, and
`--attack` queries. See `docs/script-decoder.md`. Use these static queries and
the existing `build/retail-disassembly/` bundle as the primary source for
character ports. Full bundle generation requires explicit `--bundle`; do not
regenerate bulk dumps for routine move research.
Translate script control flow into procedural C in `game/c_<character>.c`;
consult native listings and the included reference source for called helpers.
Preserve move timing, command gates, branches, damage and balance intent while
using our engine's own implementation and state boundaries. Binary identity,
exact retail scheduler emulation and per-tick trace identity are not goals.
Do not build new retail-execution harnesses or record large JSON traces for
character work. Use focused native tests of commands, move branches, damage,
interruption and recovery. Add engine features only when a translated move
needs them.
