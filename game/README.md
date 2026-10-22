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

`native/*.c` groups native implementations by capability: attacks, motion,
projectiles, throws, weapon attacks, contexts and cinematic bodies. Local
callbacks and records stay static. No C source or header lives in fighter
folders. Some native services retain historical names and engine adaptations;
source registration alone does not establish their native parity. Do not add
fighter configuration, generated frame paths or copied native variants.

Research evidence lives in shared native source comments and typed Python
bindings. Audits inspect decoded EXE roots and shared runtime registrations
directly; no per-fighter manifest is needed. Use bounded queries and the
[runtime workflow](../docs/character-script-port.md).

Arcade goes from fighter selection to the five retail destiny towers. The
frontend loads column names and packed opponent lists from the selected EXE
and draws the original `ladder_g.geo` artwork. Left/right chooses a column;
Enter begins. A match win returns to the tower at the next floor; a loss offers
a retry of the same opponent. The next fight starts after a short tower
preview (Enter skips the wait). The final win completes the run. Escape leaves
the run. Progress lives in the frontend session, outside the resettable battle.
Column rank and completed floors feed the existing CPU difficulty context.
The portable screen adapts retail presentation; camera choreography and fighter
ending movies are not implemented. Arena rotation uses the host's existing arena
list, with Goro in his lair, rather than a recovered retail arena progression.
