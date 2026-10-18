# Character source layout

Each character folder owns its native implementation, generated output and
reviewed compiler profile:

```text
scorpion/
  character.c          shared records, move tables and CharacterDef
  character.h          private callback and entry declarations
  moves/
    spear.inc
    teleport.inc
    uppercut.inc
    weapon.inc
    ...
  generated/
    scripts.inc        compiler-owned VM bodies and frame phases
    frames.inc         compiler-owned clip frame maps
  port.json            reviewed roots, native boundaries and coverage
```

Move files are C11 include fragments compiled through `character.c`. This keeps
callbacks and shared character data private without exposing them across the
game. Related helpers live with their move; shared ground, air, control and
weapon helpers have their own files. Existing external character entry points
remain declared in the shared game headers.

Edit native behavior in `moves/` and shared immutable records in `character.c`.
Add private declarations to `character.h`. Edit the local `port.json` when
reviewing compiler coverage or new generated roots. Regenerate with `make
scripts` and `make mmove-frames`; generated files must not be edited by hand.
The Makefile tracks private headers and move fragments as dependencies and
formats them with the rest of the project.

The source-query tools follow private fragments and link to the actual move
file and line. Generated code stays a separate query boundary. See the
[fighter port workflow](../../docs/character-script-port.md).
