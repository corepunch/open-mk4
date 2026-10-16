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
| Ground normals/combos | Far HP repetition, close HP/HK and their distinct cancel tables, combo HP, LK, crouching punches/kicks and roundhouse |
| Throws | Paired LP/LK throws, air throw, damage/release phases and victim recovery |
| Victim reactions | Explicit record-selected hitstun, combo scaling, crouch branches, launches, tumbles and getups |
| Fatalities | Both character fatalities and both stage fatalities; winner/range/stage gates, paired phases, creature asset and match completion |

Spear sources: scripts `0x4ed448`, `0x4ed4a8`; native `0x49a6a0`,
`0x49b170`, `0x487350`, `0x487420`; contact record `0x4ebeb8`.
Teleport: scripts `0x4e87b0`, `0x4e8818`; helpers `0x480040`, `0x47fd90`,
`0x47fec0`; record `0x4ebf48`. Fire: `0x4e5b58`, `0x468640`, `0x4689b0`;
record `0x4ebf90`.

Intentional engine choices: fixed standing/crouching collision hulls; a ten-tick
teleport vanish replacing the retail camera-bearing trigger, with distance and
wall exits retained; simple rope/flame/blade rendering. Sword drops use a single settling arc rather
than retail cosmetic bounces; thrown swords have a six-unit flight budget. Spear pull uses explicit C
state instead of a rope-node task chain. Audio, original camera cuts and particle/gore
rendering are not implemented. Finisher presentation uses our fixed camera, flame
lines, darkened burn victim and stage removal/impalement. Script coverage does not
claim identical retail rendering, collision hulls or scheduler timing.

Air attack sources: scripts `0x4e6658`, `0x4e6710`, `0x4e61f8`, `0x4e6500`,
`0x4e63d8`; native contact continuations `0x46e7c0`, `0x46cb10`, `0x46ef10`,
`0x46e6b0`, `0x46e230`. Low HP/straight-kick contacts expose the current ground
button table; its close-range combo selection uses the ground entry predicates. Diagonal
jump classification uses horizontal motion in our current jump controller.

Air throw now follows scripts `0x4e5e28/0x4e5e50`: paired clips 82/202,
28-tick capture, 13% release damage, separate falls and recovery. Attachment
only suppresses body separation; interruption releases the victim. Getup uses
clip 131:46–79 and returns control instead of reproducing retail task parking.

Scorpion's normal, air, sword, teleport and fire contact records now select
explicit shared reaction phases in `game/reactions.c`, including crouch branches,
no-guard combo windows, scaling, friction, launches, landing and getup. Sources
are the reaction table at `0x4ea4e8`, each entry's targeted native query, and its
referenced script. The port expresses tumbles along the gameplay pair axis;
camera roll, blood/sound helpers and retail wall-contact task dispatch are not
recreated. The world still uses our gameplay hulls. Goro retains his separate
animation-bank fallback. Focused tests exercise reaction recovery from ground
and air, combo flags/scaling, crouch selection, and replacing an active reaction.

Ground LP and LK throws now use paired clips 75/152 and 37/157, respectively.
Both deal their script damage at sample 42 (15% LP, 18% LK). LP releases into
reaction 100; LK retains the pair through sample 65, then plays the victim's
586:65–130 continuation. Root alignment uses our paired world origin. The
retail CPU probability-based throw reversal is not a human command branch and
is left to a future CPU policy.

The match now opens a winner-only finish window after the deciding KO. It uses
our explicit five-second window, freezes the round timer, allows positioning and
command history, and holds the loser in clip 13/action 0x1009. Timeout wins skip
this window. Starting a finisher locks both controllers and removes projectiles;
completion awards no extra round. Arena selection is now available to simulation
for the two stage-fatality predicates.


## Finisher commands and sources

After the deciding KO, enter a command during the five-second finish window.
Directions are relative to the opponent; press the attack with the last direction.

| Finisher | Command | Gate | Script/native sources |
| --- | --- | --- | --- |
| Burn | B,F,F,B + BLOCK | 1.8–3 units; 50 ticks | `467ce0`, `443840`, owner `4e0278..2f0`, victim `4e0300..35c` |
| Creature | B,F,D,U + HP | At most 1 unit; 50 ticks | `41e960`, `4d2e58/88/b0`, victim `4d2ef8/2f10` |
| Fan | F,D,D + LK | Prison (8), at most 1 unit; 40 ticks | `43d040`, `4e0118..1a8`, 31 repeats initialized at `43d3d0` |
| Spikes | B,F,F + LK | Goro's Lair (0), at most 1 unit; 40 ticks | `467e70`, `4e6950`, reaction 175 `4ea300/330` |

The creature uses `sc_fat2_.geo`, slot `50c2e8`, rig `50cb28`, clips 578–581.
Its animated IDs start at 1 (the human rigs start at 2), and its native root
placement is Y=-0.9. Victim clips 584/585 stay on the defeated character's rig.
Finite scene completion replaces retail parked tasks. Blends and stage camera
travel are presentation choices; the owner/victim clips, repeated bite phases,
fan repetitions and spike wait phases come from the listed scripts. The match
result retains the final poses and clears every finisher field on rematch.
