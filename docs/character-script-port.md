# Porting a character's scripts to C

Start with a compact move query:
`python3 tools/disassemble_retail.py --move spear` (or `--list-moves`).
Follow its `--script`, `--native` and `--attack` queries for details; see
[script-decoder.md](script-decoder.md). Use the existing
`build/retail-disassembly/` bundle for broader navigation. Regenerate with
`python3 tools/disassemble_retail.py --bundle --output build/retail-disassembly` only
when its inputs or decoder change. This is static decoding, not retail execution.

1. Start at the character's command table and stance dispatch tables. List each
   command, input window, predicate and entry. Include standing, crouching,
   jumping, armed and follow-up button tables; specials alone are not a character.
2. Open each decoded script. Translate clip selection, cursor changes, waits,
   contact queries and conditional branches into that character's C file.
   Use `MoveSequence` for fixed playback and `Fighter.script` for native loops.
   `sample_override` is the explicit animation cursor for those loops.
3. Follow calls and spawned tasks into `native/`. Use the included decompilation
   as an explanatory reference, checking revision-dependent constants against
   the supplied executable. `tools/retail_moves.py` and `tools/scorpion_moves.py`
   can read additional tables without running the game. Unknown operations are
   work to resolve, not permission to treat them as no-ops.
4. Preserve gameplay data: startup/active/recovery, damage and chip, launch/pull
   velocity, stun, cooldowns, range, command gates and allowed cancels. Name any
   intentional engine choice in the coverage notes. Do not reproduce scratch
   globals, packed pointers, CPU registers or cooperative-task implementation.
5. Add only the missing shared primitive that the current move needs: movement,
   an attack record, projectile, reaction or paired-fighter continuation. Keep
   character decisions in `game/c_<name>.c`, and rendering read-only.
6. Test our C behavior with a few meaningful scenarios: command acceptance and
   rejection, hit/block/miss, interrupted recovery and cleanup, both player
   sides, and relevant combo/cancel branches. Avoid raw per-tick dumps or new
   retail-emulation harnesses. Keep existing asset/decoder regression coverage.
7. Run `make format`, the relevant native checks, and required repository checks.
   Commit the coherent, verified step on main before starting the next one.

## Scorpion coverage

The working implementation is `game/c_scorpion.c` (the existing filename).

| Area | Current implementation |
| --- | --- |
| Jab, high kick, sweep, uppercut | Existing playback/branches plus script attack records |
| Spear | B,B,LP, startup alternative, projectile, chip, pull, 64-tick stun, 60% combo scaling, cooldown and interruption cleanup |
| Teleport | D,B,HP, air permission, cooldown/hit-count gate, vanish, cross-over, hit hang/drop, block recoil/landing, miss recovery |
| Fire | D,F,LP, normal/follow-up playback, one active query, 12% damage, recovery |
| Sword | Draw/follow-up draw, HP/LP and back slashes, held/steered spin with recoil, armed air punches, throw, disarm, resting pickup and round reset |
| Air normals | HP/LP, straight and diagonal kicks, extend/hold/retract, contact freezes, HP-to-LP/kick follow-ups and landing |
| Remaining ground normals/combos, air throw, fatalities | Not ported yet; do not label Scorpion complete |

Spear sources: scripts `0x4ed448`, `0x4ed4a8`; native `0x49a6a0`,
`0x49b170`, `0x487350`, `0x487420`; contact record `0x4ebeb8`.
Teleport: scripts `0x4e87b0`, `0x4e8818`; helpers `0x480040`, `0x47fd90`,
`0x47fec0`; record `0x4ebf48`. Fire: `0x4e5b58`, `0x468640`, `0x4689b0`;
record `0x4ebf90`.

Intentional engine choices: fixed standing/crouching collision hulls; a ten-tick
teleport vanish replacing the retail camera-bearing trigger, with distance and
wall exits retained; simple rope/flame/blade rendering. Sword drops use a single settling arc rather
than retail cosmetic bounces; thrown swords have a six-unit flight budget. Spear pull uses explicit C
state instead of a rope-node task chain. Air, weapon and special victim reactions other than
spear still use shared fallback reactions and need their own source translation
before balance equivalence can be claimed. Audio and original particle/gore
rendering are not implemented. The remaining coverage is explicit work, not
silently synthesized retail behavior.

Air attack sources: scripts `0x4e6658`, `0x4e6710`, `0x4e61f8`, `0x4e6500`,
`0x4e63d8`; native contact continuations `0x46e7c0`, `0x46cb10`, `0x46ef10`,
`0x46e6b0`, `0x46e230`. Low HP/straight-kick contacts expose the current ground
button table; its close-range combo selection still needs translation. Diagonal
jump classification uses horizontal motion in our current jump controller.
