# Shared runtime workflow

All catalogued fighter programs run as Lua scripts from
[`scripts/lua/`](../scripts/lua/README.md) on the embedded Lua 5.4 runtime
(`game/lua/script_lua.c`); `game/script/vm.c` owns playback, waits, contact
results, pairing and native continuations. The scripts were translated
mechanically from the original retail words and proved identical to them tick
for tick before the word interpreter was removed
([lua-vm.md](research/lua-vm.md)). Native services and controllers live in
shared capability modules under `game/native/`. Evidence belongs beside shared
implementations or in typed research bindings; there are no per-fighter
coverage manifests.

Data tables stay in `scripts/mk4/tables/`; `scripts/mk4/program.asm` is
generated from the Lua modules so every script keeps a linked address for
tables, command lists, reaction tables and natives. `make game` rebuilds the
package, the symbol map and native label consumers. The EXE is needed for
research/import evidence only. See [source format](../scripts/README.md).

## Workflow

1. Build edited scripts and tables with `make scripts`; normal builds do this
   automatically. Inspect the existing inventory and unresolved boundary:

   ```sh
   python3 tools/compile_retail.py --character NAME --inventory
   python3 tools/compile_retail.py --character NAME --audit --summary
   python3 tools/disassemble_retail.py --character NAME --move MOVE
   ```

   Consult existing research before querying a bounded `--script`, `--native`
   or `--attack` listing. Do not generate bulk bundles for routine work.

2. Start native research with `reference/mk4decomp` handler sources and
   matching assembly. Verify the supplied EXE address and data; upstream
   addresses belong to a different revision. Respect upstream licenses.

3. Implement a retail native routine once at its typed native address, or add
   a shared script operation once in `game/lua/script_lua.c` and document it
   in `scripts/lua/README.md`. Load roster, input, action, stance, CPU,
   animation and contact data from the linked tables. Native table selection
   can have different hidden-roster remaps from script selection; check its
   own source path. Keep actor/model identity separate from native
   implementation identity.

4. Keep script behaviour in the Lua scripts. Do not translate authored waits,
   loops, branches, animation commands or balance into C rows or handwritten
   move logic. Native routines may contain their own original control flow;
   keep their engine adaptation shared and within existing state/context
   boundaries. Do not add per-fighter C folders, configuration or copied
   native variants. A new script is a new function in a module; a table or
   export refers to it by name after `make scripts`.

5. Remove obsolete C adapters, copied data and private modules when their last
   callers migrate. Do not add per-fighter coverage declarations or fighter
   definitions. Generated fighter C/include outputs are forbidden.

6. Run `make format`, focused shared-runtime checks and `make scripts-check`.
   Test only runtime/native features we implement: operations, typed native
   roles, loaded data, interruption, actor ownership and cleanup. Trust the
   retail-derived scripts. Reuse existing integration coverage; do not add
   fighter timing, damage or balance matrices or execution trace harnesses.
   `make lua-test` runs every scenario family twice and requires identical
   simulation state each tick; run it at meaningful shared boundaries, and
   the game self-test when asset/animation integration changes. Record
   failures honestly.

7. Commit each coherent verified step directly to `main` before the next step.
   Explain the problem, implementation, retail/reference evidence, validation
   and remaining limitations. Preserve unrelated work.

See [shared-retail-runtime.md](research/shared-retail-runtime.md) for migration
status and checked native boundaries, [lua-vm.md](research/lua-vm.md) for the
runtime design and the equivalence evidence, [retail-vm.md](research/retail-vm.md)
for the retired interpreter's research notes, [script-decoder.md](script-decoder.md)
for formats and bounded queries, and [ARCHITECTURE.md](../ARCHITECTURE.md) for
engine boundaries.
