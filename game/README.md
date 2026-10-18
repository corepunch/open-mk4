# Character source layout

Each fighter lives directly under `game/` and owns its native moves, generated
output and reviewed compiler profile:

```text
scorpion/
  character.c          roster definition and generated script integration
  character.h          character-private interfaces
  moves/
    spear.c
    teleport.c
    uppercut.c
    weapon.c
    ...
  generated/
    scripts.inc        compiler-owned VM bodies and frame phases
    frames.inc         compiler-owned clip frame maps
  port.json            reviewed roots, native boundaries and coverage
```

Move files are independent C11 translation units. A move owns its callbacks and
immutable records whenever they are used only there; those symbols remain
`static`. The private `character.h` declares only the entries, continuations and
shared records needed across the character's files. Existing game-facing
character declarations remain in the shared game headers.

Generated bodies remain private to `character.c`. Small typed entry functions
let native moves start those bodies, and phase accessors keep generated frame
tables private. Private aliases retain the reviewed native binding names while
namespace-qualified external symbols prevent collisions between fighters.

Edit native behavior and move-owned records in `moves/`. Add a private interface
only when another file needs it. Edit `port.json` when reviewing compiler
coverage or new generated roots. Regenerate with `make scripts` and
`make mmove-frames`; generated files must not be edited by hand. The Makefile
compiles every move source and tracks and formats character-private headers.

Source-query tools search each character's move modules and link to the actual
file and line. Generated code remains a separate query boundary. See the
[fighter port workflow](../docs/character-script-port.md).
