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

The battle simulation uses native move rules. Its clip selections were checked
visually for compatibility, and are not a reconstruction of the original FSM's
complete per-character action mapping. Goro's separate action bank is retained.
Move routines store an action code at player +0x74 and start a script through
0x459490; script opcode 0x47 sets a clip, and 0x3c branches on a 16-entry
character table. Action 0x10b plays the jab 294, with 295 on a repeated press.
Action 0x109 starts with the uppercut 293. Clips 210..250 are started from the
weapon routines at 0x4955c0..0x496d10 and hold a weapon grip. The punch and
uppercut rows use 294 and 293. The other rows remain visual choices.
Battle placement keeps each frame's root-motion Y and drops root-motion X/Z.
That Y channel is the ground plant: idle, crouch, hit, victory and a settled
knockdown put the sole on Y=0 (positive Y is down) only while it is applied.
Subtracting the first frame's Y lifts a victory by about 0.4 and a knockdown
by about 1.0. The roster scale at +28 is stored on the character and is not
part of this plant.

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
