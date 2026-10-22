# Shared retail runtime workflow

All catalogued fighter programs execute original retail words through one
C11 interpreter. Script-to-C/frame translation is retired. The remaining
native services and controllers live in shared capability modules under
`game/native/`. Evidence belongs beside shared implementations or in typed
research bindings; there are no per-fighter coverage manifests.

## Workflow

1. Inspect the existing inventory and unresolved boundary:

   ```sh
   python3 tools/compile_retail.py --character NAME --inventory
   python3 tools/compile_retail.py --character NAME --audit --summary
   python3 tools/disassemble_retail.py --character NAME --move MOVE
   ```

   Consult existing research before querying a bounded `--script`, `--native`
   or `--attack` listing. Do not generate bulk bundles for routine work.

2. Start native/interpreter research with `reference/mk4decomp` handler sources
   and matching assembly. Verify the supplied EXE address and data; upstream
   addresses belong to a different revision. Respect upstream licenses.

3. Extend an opcode once, or implement a retail native routine once at its
   typed native address. Load roster, input, action, stance, CPU, animation
   and contact data from retail. Native table selection can have different
   hidden-roster remaps from script selection; check its own source path.
   Keep actor/model identity separate from native implementation identity.

4. Run original script words. Do not translate authored waits, loops,
   branches, animation commands or balance into C rows or handwritten move
   logic. Native routines may contain their own original control flow; keep
   their engine adaptation shared and within existing state/context boundaries.
   Do not add per-fighter C folders, configuration or copied native variants.

5. Remove obsolete C adapters, copied data and private modules when their last
   callers migrate. Do not add per-fighter coverage declarations or fighter
   definitions. Generated fighter C/include outputs are forbidden.

6. Run `make format`, focused shared-runtime checks and `make scripts-check`.
   Test only runtime/native features we implement: opcode/control flow, typed
   native roles, loaded data, interruption, actor ownership and cleanup. Trust
   authored retail scripts. Reuse existing integration coverage; do not add
   fighter timing, damage or balance matrices or execution trace harnesses.
   Broaden verification at meaningful shared boundaries, including the game
   self-test when asset/animation integration changes. Record failures honestly.

7. Commit each coherent verified step directly to `main` before the next step.
   Explain the problem, implementation, retail/reference evidence, validation
   and remaining limitations. Preserve unrelated work.

See [shared-retail-runtime.md](research/shared-retail-runtime.md) for migration
status and checked native boundaries, [retail-vm.md](research/retail-vm.md) for
interpreter evidence, [script-decoder.md](script-decoder.md) for formats and
bounded queries, and [ARCHITECTURE.md](../ARCHITECTURE.md) for engine boundaries.
