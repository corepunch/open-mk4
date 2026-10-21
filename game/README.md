# Retail runtime and native services

Every fighter uses the shared C11 interpreter in `retail_vm.c`. Roster,
input, action, stance, CPU and weapon selection come from retail tables.
Script branches, waits, loops and animation instructions execute from original
words; no generated script C or frame output belongs in the game.

`native_services.h` declares engine implementations of retail native routines.
There are no character-private C headers or C fighter definitions. A native
service receives explicit Fighter/body/scene state and reuses the engine's
combat, physics and lifetime boundaries. A routine shared by several roster
slots has one implementation.

Remaining `*/moves/*.c` paths are transitional native implementations awaiting
consolidation into shared services. They do not define fighter registration or
script programs. Keep local callbacks and records static. Do not add fighter
configuration, generated frame paths or copied native variants there.

Per-fighter `port.json` files are research inventories and source navigation,
not runtime configuration or evidence of complete native parity. Historical
source names are resolved by Python research tools only. Use bounded queries
and the [runtime migration workflow](../docs/character-script-port.md).
