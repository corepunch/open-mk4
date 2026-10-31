# Shared runtime workflow

All catalogued fighter programs run as named wordcode from
[`fighters/`](../fighters/README.md) through the shared C interpreter in
`game/script/vm.c`. It preserves retail opcode numbers, packed arguments,
branches, loop state and cooperative waits. Actor-owned playback, contact,
pairing, native continuations and cleanup stay in the existing engine.
Native C services live once under `game/native/`.

`make scripts-export` regenerates wordcode from the supported EXE through the
existing decoder. `tools/mk4_script_layout.json` preserves established entry
names/addresses and native IDs. Tooling groups streams by existing
names and catalog usage; ambiguous/shared streams remain single definitions.
Each fighter file carries its bytecode and relevant native ABI declarations,
with symbolic cross-file references. Bare labels and executable embedded
aliases become C constants named `scr_<label>` directly, without separate
script export mappings or intermediate enums.
Explicit jumps preserve original fall-through when blocks move.

Data tables stay in `scripts/mk4/tables/`. Normal builds link the checked-in
sources into one validated package and compile exported addresses and diagnostic
names into C consumers. Build/startup needs no EXE or Lua. See
[source format](../scripts/README.md) and [architecture](../ARCHITECTURE.md).

## Workflow

1. Inspect the existing inventory and unresolved boundary:

   ```sh
   python3 tools/compile_retail.py --character NAME --inventory
   python3 tools/compile_retail.py --character NAME --audit --summary
   python3 tools/disassemble_retail.py --character NAME --move MOVE
   ```

   Consult existing research before bounded script/native/attack queries.
   Do not regenerate bulk bundles for routine work.

2. Use `reference/mk4decomp` interpreter/handler sources and matching assembly.
   Verify local EXE addresses and data; upstream addresses belong to a different
   revision. Preserve established semantic names and respect upstream licenses.

3. Implement missing runtime/native behavior once at the shared boundary.
   Generate opcode names from the existing schema; implement shared operations
   in `game/script/vm.c`, and native services in the relevant capability module.
   Load roster, input, action, stance, CPU, animation and contact records from
   the package. Selection remaps follow each original table's own path.

4. Keep authored waits, loops, branches, animation commands and balance in
   wordcode. Do not lower scripts to C frame rows or handwritten move logic.
   Name new decoded entries in the metadata and add roots to the import layout,
   then run `make scripts-export`. Do not add per-fighter C dispatchers,
   duplicated native implementations, handwritten fighter definitions or
   coverage manifests. File ownership is organization, not runtime identity.

5. Remove superseded interpreters, configuration and generators at the same
   shared migration boundary. Keep source exports and typed continuations
   consistent; a native callback is never executable script data.

6. Run `make format`, focused shared-runtime checks and `make scripts-check`.
   Import/link tests compare every retained instruction and typed relocation.
   Test operation/native contracts, data loading, interruption and cleanup;
   trust retail fighter logic rather than adding timing/damage matrices.
   At shared boundaries, `make bytecode-test` runs all existing scenario families
   twice and compares the complete simulation each tick. Run `make check` at
   completion and record failures honestly.

7. Commit each coherent verified change to `main`, preserving unrelated work.
   Record the request, shared implementation, evidence, validation and limits.

The earlier Lua experiment and its comparison evidence remain in the historical
[Lua notes](research/lua-vm.md); current execution uses the C interpreter.
