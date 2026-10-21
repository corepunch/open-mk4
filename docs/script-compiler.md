# Runtime audits; the script-to-C compiler is retired

Retail scripts run as original words through the shared C11 interpreter.
The former C/frame emitter, generated fighter includes, compiler manifests
and translation tests are removed. `compile_retail.py` retains its historical
filename for these read-only research commands:

```sh
python3 tools/compile_retail.py --character NAME --inventory
python3 tools/compile_retail.py --character NAME --audit --summary
python3 tools/compile_retail.py --all --audit --strict --summary --require-implemented
```

`make scripts-check` uses the last command. One process shares its C source
reachability index across all roster slots. No C compilation or regeneration
is involved. `--output` saves the JSON inventory/audit or textual summary.
Inspect individual scripts/native routines with `disassemble_retail.py`.

The audit checks reviewed inventory fingerprints, interpreter registration for
every catalogued root, opcode support, typed native roles, data-window bounds
and real source/reachability for native and engine boundaries. It also checks
retail input/CPU registrations, stance/weapon data and reaction dispatch.
Translated or legacy script configuration is rejected.

These gates check the runtime integration we implement. Authored timing,
damage and balance remain in the retail words. Native implementation work
still needs source evidence and proportional lifetime/data-boundary validation;
registering an address alone does not establish gameplay parity.

Follow [the shared runtime workflow](character-script-port.md#workflow) and
[the migration ledger](research/shared-retail-runtime.md). Do not recreate
per-fighter generated code or handwritten fighter configuration.
