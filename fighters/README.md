# Fighter bytecode

Each fighter file contains named retail wordcode. `shared.asm` defines shared
or unclassified streams once. All files execute through the same C VM.

```sh
make scripts-export       # decode the hash-checked retail EXE
make scripts-source-check # compare without writing
make scripts              # link checked-in source; no EXE needed
make bytecode-test         # deterministic shared-runtime scenarios
```

Native operands are symbolic references, for example
`tail 0, native_quan_chi_script_skull_release`. The declaration in
`scripts/mk4/native.inc` names its typed C implementation:

```asm
.native native_quan_chi_script_skull_release, tail=quan_chi_script_skull_release
```

The linker assigns compact references and emits typed host function pointers.
No retail code address is compiled into native dispatch. The importer/research
metadata retains original addresses for verification. Different call roles
remain distinct; unsupported roles fail explicitly when invoked. Some retained
unreachable streams reference natives whose implementation is still unavailable.

Script-only native tails link directly to a script and its completion policy;
they require no C trampoline. Native services live once under `game/native/`.
Script labels and embedded aliases become `fn_<label>` in C automatically.
Cross-file branches use bare script names. Regrouping inserts explicit jumps
where needed to preserve original fall-through and operand entries.

See [source format](../scripts/README.md) and [architecture](../ARCHITECTURE.md).
