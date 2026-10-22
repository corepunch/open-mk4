# MK4 workspace

## Commits

Commit each completed, verified implementation step directly to `main` before
starting the next step. Treat a step as a coherent change with its relevant
validation, and keep separate steps in separate commits.

Use a clear commit subject and a detailed description explaining the problem
or request, what changed, why the approach was chosen, and how it was verified.
Record relevant test results and any remaining limitations. Stage only the
files belonging to that step, preserving unrelated work.

## Scope and execution efficiency

Implement the requested end state through the existing shared runtime first.
For replacement/removal work, switch the shared entry or dispatch boundary,
then delete obsolete callers, configuration and generators as they become
unused. Do not turn a mechanical migration into routine-by-routine rewrites
or a broader fidelity project.

Separate actual blockers from optional improvements. Extend runtime/native
support only when required by the requested behavior or a concrete integration
failure. Reuse existing implementations and research; do not re-derive retail
script logic. Keep unrelated native accuracy, naming and presentation work out
of the migration unless needed to complete it. Report remaining limitations
without silently expanding scope.

Batch changes that share one implementation and validation boundary into a
coherent step. Commit each verified step, but do not create a separate research,
audit and commit cycle for every move, address or wrapper. Avoid temporary
fighter-specific adapters, origin contexts and parallel APIs; when a bridge is
necessary, identify and complete its removal within the same migration.

Choose validation from the changed contract. Run focused checks while working;
run all-roster audits and the full suite at meaningful shared boundaries and
completion. Repeat broad checks only after changes that could invalidate their
results, a relevant failure or an explicit repository requirement. Retire tests
of intentionally removed architecture in the step that removes it, preserving
checks for engine/runtime behavior we still own. Do not rewrite production code
to satisfy obsolete authored-script timing or damage expectations.

Keep documentation proportional: update the current architecture/workflow and
record evidence, validation and limitations in commit descriptions. Do not append
a chronological migration essay for every small change. If research, scaffolding
or verification starts dominating implementation, reassess the approach before
continuing; give the user a concise explanation of any unavoidable scope growth.

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

Test the shared runtime and native integration we implement. Keep fixtures
small and focused on opcode/control flow, typed native boundaries, data loading,
interruption and cleanup. Trust the retail scripts for authored character logic;
do not add per-fighter timing, damage or balance matrices. Existing regressions
may be adapted to the runtime API without adding character-specific coverage.

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
end-to-end research, shared-runtime implementation, validation and commit procedure.

The target is one shared retail runtime for every fighter, with retail-loaded
roster, input, action, stance and CPU tables. Do not add per-character C
architecture or hand-authored fighter configuration. Retail native routines
must have one shared implementation rather than copied fighter variants.
Do not add fighter directories or coverage manifests; native services live in
shared capability modules. See `docs/research/shared-retail-runtime.md`.

All 18 fighters run original retail words through the shared C11 interpreter.
Do not translate branches, waits, loops or animation commands into generated C,
frame rows or handwritten move logic.
Extend a missing opcode once and reuse the existing native engine helpers for
retail native calls. Script data must remain the source of script behavior.
Generated script/frame C and per-fighter C/header/include modules are forbidden.
Native services live once in shared capability modules under game/native/.

Start research with `tools/compile_retail.py --character NAME --inventory` and
`--audit --summary`. Use bounded `tools/disassemble_retail.py --character NAME
--move MOVE`, `--script`, `--native` and `--attack` queries for unresolved
boundaries; consult the existing `build/retail-disassembly/` material first.
Full bundle generation requires explicit `--bundle`; do not regenerate bulk
dumps for routine move research. Consult native listings and reference source
for unresolved helpers. See `docs/research/retail-vm.md` and
`docs/script-decoder.md`.

Keep native evidence beside shared implementations and in typed research bindings,
not per-fighter coverage manifests. All catalogued roots must remain interpreted.
Run `make scripts-check` and the strict `--require-implemented` audit. Neither
runtime registration nor reviewed-root coverage alone proves native gameplay
parity. Trust retail script control flow and data. Heavily rely on mk4decomp interpreter/handler sources and matching assembly,
checking local EXE addresses rather than copying upstream VAs. Test only the
shared runtime we implement, including opcode and native/lifetime contracts;
reuse existing integration coverage instead of adding per-move timing/damage matrices.
Run focused checks during migration and the full suite at meaningful shared
runtime boundaries, not repeatedly for each move.

Preserve move timing, command gates, branches, damage and balance intent while
using our engine's own implementation and state boundaries. Binary identity,
exact retail scheduler emulation and per-tick trace identity are not goals.
Do not build new retail-execution harnesses or record large JSON traces for
character work. Add engine features only when retail execution needs them,
and keep new tests focused on our shared runtime and native integration.
