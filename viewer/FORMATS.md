# Verified PC asset path

Reference: `reference/mk4decomp` at `af88007` and the supplied June-1998 EXE.
Addresses below belong to the supplied EXE, and can differ from upstream symbols.

## Archive, meshes and textures

The FILESYS index holds little-endian `{hash, offset, size}` records in its first
0x3000 bytes. GEO names use the uppercase full DOS source-path hash. The viewer
reads assets directly, without altering or extracting the archive.

GEO begins with `0.1v`, texture offset and file size. Its 16-byte mesh entries
hold type, triangle count and three signed field-relative offsets. Strip
records contain flags and a signed count; a negative count ends the list. A
count N consumes N+3 vertices and produces N+1 triangles. Each vertex has XYZ
s16 positions (Q8) and Z/X/Y s16 normals (Q12), reordered into XYZ before use. Each triangle has eight UV/material
bytes. The last four UV bytes can overlap the texture-offset prefix.

The texture count lives four bytes after the header's texture offset. Each
texture has width, height and stream size. RGB555 literals use two bytes; bit
15 marks a repeated color followed by an 8-bit run length. Zero is transparent.
Some files omit the terminal alignment byte included in the declared size.

## Skeleton and animation

The roster at 0x4eb2e8 has 18 records of 32 bytes. Its primary rig pointer is
at +12, asset slot at +0, and scale at +28. Character names come from 0x4ee1f0.
The startup fixup tables store source VA/4 at each destination VA; these are data
fixups, distinct from PE relocation entries.

Rig records are 28 bytes. Signed top 12 bits of their first word encode bone
ID; low 20 bits contain flags. Translation at +12/+16/+20 is signed Q16, and
+24 selects a mesh. Bit 3 marks a leaf; bit 4 ends the sibling list. Bone-ID
lookup orders the animation channels, rather than mesh order or preorder.
Standard IDs 2..16 consume 15 channels; Goro adds six arm channels. Unanimated
nodes and empty lookup slots do not consume an angle word.

**The tree builder is only the first step.** `MStackPushTwoEntryChainCall`
(0x4058c0) and its continuation (0x405960) replace flag bits 24..27: the root
gets 9 and all descendants get 10. Render dispatch selects the C matrix builder
for the root and B for the children. Keeping the initial kind 8 selects A and
produces incorrect airborne/twisted poses even when decoding is exact.

AP0 (hash 2da2df95) contains a 512-byte nibble-prefix table followed by four-byte
`{id|compression-bit, byte-size}` entries, terminated by FFFF. AP1 (2da3df95)
concatenates those payloads. The descriptor table at 0x4fad7c has 612 12-byte
entries: data pointer, frame count and flags/word stride. There are 571 compressed
and 41 uncompressed clips, totaling 36,723 frames.

Each decoded frame starts with three signed Q16 root-motion values, followed
by one packed word per animated bone. Fields are 11/11/10 bits, expanded to BAM
angles by 2/2/4; 4096 is a full circle. Direct matrix handlers negate the angles.
The table named `g_sinTable` is actually built with FCOS in `AppInit_PostJoy`.
The viewer uses floating-point equivalents of the original B/C formulas,
shortest-angle interpolation, parent/local matrix composition and parent-space
bind offsets. It preserves the root motion and projects positive Y downward.

Idle selection reads the game's default descriptor table at 0x53d408. Special
character IDs 16 and 17 remap to 2 and 7. Camera heading is fixed from each idle's
initial head direction; animation does not rotate the camera to follow it.

## Independent checks

`tests/fixtures/original_matrices.*` records Q12 results from the original
0x4b2980 (C) and 0x4b2c00 (B) routines, with the runtime cosine table initialized.
The float matrices allow a 0.002 tolerance for original fixed-point truncation.
`tests/verify_original.py` rechecks those fixtures and every decoded frame against
0x4013a0 (compressed decoder) or 0x401000 (uncompressed frame acquisition).
Unicorn is a development-only dependency. The SDL2 app never executes PE code.


## Shared package and battle resources

Package validation and DOS-path hashing now live in `common/mk4_package.*`;
`fsys`, `mk4-viewer` and `mk4-game` all link that implementation. Asset decoding
and transforms live in `common/mk4_assets.*`.

In this executable the stage table starts at **0x4dd960**, with eleven 64-byte
records. The packed slot at +0 leads to the GEO asset descriptor; +12 is the
packed scene hierarchy. The root/child flags and 28-byte records follow the
same structural walk as character rigs. Static stage nodes use bind offsets;
the separate animation callbacks at +8 are not evaluated by this game.

Arena GEOs have three RGB555 pages. Mesh kinds include flag bits 0x40 and 0x100;
their field-relative streams follow the character layout. The first byte of
each triangle's 8-byte material/UV record selects the texture page. UI blocks
whose kind's low byte is 0x80 are 16-byte sprite records: page at +2, logical
width/height at +4/+6, origin at +8/+10, UV start and UV extent at +12..15.
The final sprite, like the final mesh UV record, can overlap the four bytes
before the texture count. `mk4font_.geo` contains 143 sprites on one page,
including original roster names and the health bar at sprite 25. The renderer
uses the long bar's top eight texture rows; the right player's bar is mirrored.
`shared_g.geo` contains four texture pages for fight text and effects.

The battle simulation uses native move rules. Scorpion and Sub-Zero high kicks
now follow original no-contact playback sequences, including Scorpion's reverse
recovery, end callbacks and contact-query steps. See
[the script/oracle evidence](../docs/scorpion-retail-moves.md). Other moves
still use approximate timings and one default clip per state; combo strings,
special attacks and victim animations are not reproduced. Move routines store an action code at player +0x74 and start a script through
0x459490. The VM fetches a word, advances, and dispatches its top byte through
the handler table at 0x4e3988; a packed pointer (top byte 0) is a no-op. Op
0x47 sets a clip and 0x3c indexes the 16-entry table that follows it by
character id, reading Sonya's entry for Kitana (16) and Reiko's for Noob (17).
Each fighter has its own moves in `game/c_*.c`, registered by `game/roster.c`:

| State | Evidence | Clips |
| --- | --- | --- |
| Walk | forward table 0x53d388 (0x53d3c8 is the slower walk back) | 265..269 by fighter |
| Crouch | action 0x2001 plays 259, then loops 258 | 259 |
| Jump | action 0x2007 plays the full jump | 304 |
| Punch | action 0x10b; 295 follows a repeated press | 294 |
| Kick | action 0x100, character table 0x4e5ca4 | 65, 66 or 71 |
| Sweep | action 0x10c, character table 0x4e679c | 62 or 63 |
| Uppercut | action 0x109 | 293 |
| Throw | character table 0x4de7a0; op 0x0e sets the victim, 0x0f the thrower | 75, 76, 78, 79, 81; Jax 83 |
| Block | action 0x2005 loads slot 0x53ce30, or 595 for Goro | 322 |
| Hit | the 0x2005 reaction script starts with 247 | 247 |
| Knockdown | action 0x4003 staggers with 311, then falls | 186 |
| Victory | round-end scripts call 0x4712e0; character table 0x4e6a44, held | 111..125 by fighter; Goro 594 |

Clips 210..250 are started from the weapon routines at 0x4955c0..0x496d10 and
hold a weapon grip. The 0x4e5efc table (44..51) sits in an AI move list and
holds katas, not win poses. Each win script opens one clip and then waits on
its last frame, so victory moves carry SF_HOLD. Jax's throw entry starts with
op 0x32, calling 0x42c7f0, which switches to script 0x4de4f0 (83, victim 145);
the 607/605 pair after it is Goro's pummel throw started from 0x42c7a0.

Goro (character 15) is driven by AI choice lists at 0x4dfa70..0x4dfacc and his
own handlers. Walk 590, block 595 and throw 604 come from the shared tables.
His punch 597 (0x43ae70, script 0x4dfb40), kick 598 (0x43b170), uppercut 600
(slot 0x53c6bc at 0x43b75f), leap and stomp 601 (action 0x409, used as his
low knockdown in place of a sweep), jump 602 (0x43b7e0) and win roar 594 come
from his code. His duck plays 603, a rise from the crouch, backwards through
the move's frame range. His hit (593) and knockdown (581) remain visual.
Battle placement keeps each frame's root-motion Y and drops root-motion X/Z.
That Y channel is the ground plant (positive Y is down). Subtracting a frame's
Y lifts the pose by that amount. On the clips the roster plays, uppercut 293
opens near +0.41, crouch 259 settles near +0.44, and knockdown 186 settles
near +0.97. Win poses open near +0.05 to +0.11; the held frame reaches about
+0.39 for Liu Kang (119) and about +0.47 for Quan Chi and Jarek (125). The
roster value at +28 is stored on the character and is not part of this plant.

Fujin's mesh on the shared fall 186 ends about 0.21 below the floor. The same
clip settles the other human meshes within 0.06, so his row keeps 186. Goro's
visual knockdown 581 keeps a root Y near +0.87 and its last frame meets the
floor; the first frame is still about 0.49 below it. No other clip in his bank
lies flat on the floor, so that opening stays.

Game text uses MK4FONT white digits 28..37, letters 38..63, and punctuation
64..95, preserving the +8/+10 signed sprite bearings. Shared sprites 138..141
supply the original ROUND word and digits 1..3 for the round intro; later round
numbers (possible after draws) use MK4FONT digits. All other game labels use the
same glyph atlas without a built-in or system-font fallback.


## Planar shadows and stencil

`common/shadow.*` builds the homogeneous plane projection
`M = dot(plane, light) * I - light * transpose(plane)`, supporting directional
(w=0) and point lights (w=1). The battle renderer reuses each fighter's posed
world-space triangles for both the visible mesh and its projected shadow on
the arena's floor plane, measured from visible near-ground geometry. The same point light used by surface shading is placed five world units above
the current camera (positive Z is the camera side, and world Y increases downward).

The software framebuffer has an 8-bit stencil alongside its reciprocal-depth
buffer. Visible arena triangles on y=0 (within 0.05 world units for quantized
GEO vertices) write receiver bit 1; other visible geometry clears it. Projected
caster fragments test scene depth with a small bias, respect texture alpha,
and OR shadow bit 2 only into receiver fragments. A separate resolve multiplies
floor color once for the union of both fighters' shadows, consumes shadow bits,
and leaves depth unchanged. Fighters and HUD are drawn afterward. This is a
planar floor technique; shadows do not wrap onto walls or stepped surfaces.


## Surface lighting

`common/lighting.*` supplies a single point light for the game. Normals are
rotated with their mesh/bone and then normalized for ambient + Lambert diffuse
shading. The point light follows the camera in X/Z and stays five units above
its Y coordinate. The game passes its homogeneous position (w=1) directly to
the floor-shadow matrix, so visible shading and cast shadows cannot use separate
light directions. Vertex brightness survives near-plane clipping and is
perspective-correct in the software rasterizer. HUD sprites remain unlit.
