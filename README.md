# MK4 package tool, viewer, and game

The local Git repository uses `main`. `reference/MortalKombat.Tools` is a pinned
submodule; restore it after cloning with `git submodule update --init`.
Clone [mk4decomp](https://github.com/nathan-casabieille/mk4decomp) into
`reference/mk4decomp` when you need that research tree. `reference/` is ignored.
Supply the original game files separately in `MK4/`; game binaries, extracted
assets, generated images, and build outputs are ignored by Git.

`fsys.cpp` is a dependency-free C++17 tool for inspecting, extracting, and polling
the PC versions of Mortal Kombat 4's `FILESYS.DAT` and War Gods' `WARGODS.DAT`.
Builds on macOS and Linux; archive format detection is automatic.

```sh
make
./fsys MK4/FILESYS.DAT --list
./fsys MK4/FILESYS.DAT --exe MK4/MK-4.EXE -o extracted
./fsys MK4/FILESYS.DAT --exe MK4/MK-4.EXE -o extracted --watch 1
make test
```

`make extract` builds the tool and extracts the supplied archive with filename
recovery. `--exe` is optional; numeric sound filename candidates are built in,
and executable strings supply the geometry and menu-image candidates.
`--list` prints TSV to standard output and does not create extracted files.

Extraction preserves every payload byte. Files go into `sound/`, `geogfx/`,
and `other/` inside the chosen output directory. Each filename starts with its
archive hash to avoid filename collisions. `manifest.tsv` records the entry
index, hash, offset, size, detected type, output path, and original path candidate.
The hash is weak: a matching name is a candidate, not proof of the original name.
Conflicting candidates fall back to hash-only filenames and are marked ambiguous.

For War Gods, supply `Wargods/WARGODS.DAT` and run:

```sh
make extract-wargods
./fsys Wargods/WARGODS.DAT --list
```

This extracts to `extracted/wargods/files/`, preserving the directory's actual
filenames, including DOS short names. The manifest marks these as `stored`.
No executable is needed. The hash column is our convenience hash of the stored
name, not an on-disk War Gods identifier; named lookup handles collisions.
The engine's shared C package layer loads either archive with `package_load`;
`package_named` retrieves War Gods payloads by case-insensitive stored filename.
Existing MK4 full-path hash lookup is unchanged. War Gods model, animation and
arena decoding is separate work; accepting its archive does not make those
assets playable yet.

The supplied War Gods archive has 559 packed directory records: 558 files and
an empty-name EOF sentinel. A little-endian count precedes 17-byte records
(`u32 absolute offset`, `char name[13]`). The first payload starts immediately
after the directory; adjacent offsets determine sizes. The loader validates
bounds, ordering, safe unique names and the EOF sentinel before extraction.
There are 112 TGA images, 219 `.W` files, 181 `.SBK` files, 44 `.D00/.H00/.D01/.H01`
files, `HISCORE.BIN`, and a file named `0` containing a DOS directory listing.
These extensions are inventory evidence, not yet decoded format definitions.

Polling requires two consecutive, identical, valid snapshots before publishing
an update, including the initial extraction. It compares archive bytes, so it
also detects in-place edits with unchanged file size and modification time.
Missing or invalid archives are retried; Ctrl-C or SIGTERM stops the watcher.
Output failures stop the tool with an error. Polling intervals range from 0.05
to 86400 seconds. Each changed output file is replaced atomically; unchanged
files keep their timestamps. An update is **not** atomic across the whole
directory. A writer that pauses longer than the polling interval may appear
stable; atomically replacing the input archive is preferable when possible.
Files removed from the archive are retained, and the latest manifest describes
the current contents. Existing output files with generated names are updated.
Output roots and generated subdirectories must not be symlinks.

## Language and architecture

The game and engine use C11: plain state structs, explicit context functions,
const tables, and caller-owned pose buffers. Fighters are rows in one roster.
Each row points at motion, move, and command tables; spawn stores the
row index on a fighter, and one step function reads those tables. Keyboard,
shell keys, and the CPU each produce the same input frame. The PC retail reference is
reconstructed in C using MSVC 5.0; the compiler product name does not make the
game C++. Our supplied executable confirms an MSVC-era toolchain but retains
no source filenames that distinguish `.c` from `.cpp`. See
[ARCHITECTURE.md](ARCHITECTURE.md) for the evidence, retail subsystem map and
behavioral gaps. This portable implementation is not a matching retail rebuild.

## Shared code and executables

All three executables include `common/mk4_package.h` and link the same
`common/mk4_package.c`. There is one filename hash and one archive index
validator, including overlap, padding, ordering, and payload bounds checks.
The package tool keeps its stable-snapshot watcher and extraction code separate.

| Code | Used by |
| --- | --- |
| `common/mk4_package.*`, `common/binary.*` | Package tool, viewer, game |
| `common/mk4_assets.*` | Viewer and game: GEO, RGB555 textures, rigs, AP0/AP1, poses, executable data |
| `common/renderer.*` | Viewer and game: canvas, depth buffer, textured triangles, text, sprite blits, screenshots |
| `common/lighting.*` | Shared point light for model/arena shading and matching shadow projection |
| `common/shadow.*` | Homogeneous planar shadow matrices for directional and point lights |
| `common/sprite_font.*` | Game: original MK4FONT glyphs, bearings, timer, and text layout |
| `common/sdl_app.*` | Viewer and game: window, SDL presentation, logical coordinates, cleanup |
| `viewer/main.c` | Character gallery and inspection controls |
| `fsys.cpp` | Package listing, extraction, filename recovery, polling |
| `game/game.h`, `game/roster.c`, `game/battle.c` | Roster state tables and SDL-independent combat simulation |
| `game/input.c` | Keyboard bindings, shell keys, and CPU input |
| `game/draw.c`, `game/main.c` | Battle presentation and game loop |

`make` builds all three tools. `make fsys` builds only the dependency-free
package tool. Viewer and game require SDL2 and a C11 compiler. The standalone
package tool uses a C++17 frontend linked to the same C archive module.

Decoded assets, canvas buffers and SDL handles have explicit load/free APIs.
Asset loading fails the command-line application with a diagnostic on invalid
input; the package validator returns errors so the watcher can retry. Initialize
asset owners to zero, free them before reloading, and keep them alive through
rendering. Pose output belongs to the caller.

## Battle game

```sh
make game
make play
./mk4-game --list
./mk4-game --p1 3 --p2 15 --arena 2
./mk4-game --two-player
./mk4-game --headless --demo --frames 360 --screenshot build/battle.bmp
make game-test
make check
```

The separate `mk4-game` target starts Scorpion versus a CPU Sub Zero in The
Prison. It loads all 18 fighters, 11 arena GEOs and their executable scene
hierarchies, three texture pages per arena, `mk4font_.geo` for original name and
health-bar artwork and all other game text, and `shared_g.geo` for original
ROUND/round-number sprites and fight/impact graphics. The game uses no built-in
or system font. Goro uses his
own 21-channel animation bank. Other characters use compatible original action
clips. All resources load directly from the read-only archive and executable.

All 1,137 catalogued fighter script roots across 18 roster slots execute original
retail words through one shared C11 interpreter. Roster, command/action, CPU
special, native throw/combo selection, close-attack button, reaction and scene-selection tables load from retail data. Generated fighter
C/frame output and its emitter are removed. Remaining native controllers and
state adapters are being consolidated; see the
[migration ledger](docs/research/shared-retail-runtime.md).
Follow the [shared runtime workflow](docs/character-script-port.md#workflow):
check mk4decomp and matching local native listings, implement a missing runtime
or native boundary once, and keep validation focused on our data/lifetime
contracts. Authored script behavior stays in the original words.

Combat runs at a fixed 60 ticks per second. It implements walking/running,
jumping, crouching, punches, kicks, sweeps (crouch + kick), uppercuts (crouch +
punch or U), throws, high/low guard, startup/active/recovery windows, range and
height checks, one hit per attack, hitstun, launch/knockdown, body collisions,
arena boundaries, a 99-second timer, draws, best-of-three rounds, and rematches.
The camera follows the fight and adjusts to fighter separation. A point light sits five
world units above the current camera, following its position and zoom. Original
GEO normals are decoded in Z/X/Y order, rotated with the arena hierarchy and
fighter bones, and shaded with ambient plus Lambert diffuse lighting. Vertex
brightness is clipped and interpolated with perspective correction. The exact
same point-light position drives the 4-by-4 planar shadow matrix for animated
fighter meshes. The shared software
renderer uses an 8-bit stencil buffer to mark visible floor receivers and union
shadow coverage, then darkens each covered pixel once. Depth tests prevent
shadows from painting over foreground props; transparent caster texels remain
transparent. Shadows follow attacks, movement, knockdowns, and jump height.

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Move | A / D | Left / Right |
| Jump / crouch | W / S | Up / Down |
| Punch / kick | J / K | Keypad 1 / 2 |
| Block | L | Keypad 3 |
| Uppercut / throw | U / O | Keypad 4 / 5 |
| Run | Hold H | Hold Right Shift |

F1/F2 cycle fighters and reset the match; F3 switches CPU/local player two;
F4 cycles arenas; R resets; Enter rematches after a match; Space pauses; Esc
quits. Attack buttons trigger on key presses. Losing window focus pauses play.
On compact keyboards, keypad inputs require an external keypad; the default
CPU mode needs only Player 1's keys.

This is a new native combat implementation using the original assets, with
its own move timings and damage values. It does not yet reproduce MK4's full
combat rules, character-specific specials, projectiles, combo chains, fatalities,
weapons, sound, or scripted arena effects/hazards. Arena hierarchies are loaded
in their static bind poses; original stage callbacks are not executed. The
Skull scene includes an open floor area. No original executable code runs.

`make game-test` checks the battle simulation, camera lighting, matching shadow rays, perspective brightness, projection and
stencil behavior, every arena and HUD resource,
animation compatibility for all 18 rigs, and a deterministic rendered CPU fight.
`make check` also checks code formatting, runs the package and viewer regressions,
and checks malformed archives against all three executables.

Run `make format` to apply `.clang-format` to project-owned C/C++ sources,
headers, and C include fragments. `make format-check` reports formatting errors
without editing files. Both targets leave `reference/` untouched. Install
`clang-format` on `PATH` or use the version supplied by Xcode on macOS; select
a specific installation with `make format CLANG_FORMAT=/path/to/clang-format`.

## SDL2 character gallery

The C11 viewer reads the archive and executable directly; extraction is optional.
It loads all 18 roster slots, their textured GEO meshes and executable skeletons,
and loops the game's per-character idle animations in a 6-by-3 grid. Each view
faces the character's head toward the camera. Kitana and Noob use the game's
special-ID stance remapping; Goro retains all four arms and 21 animation channels.

Install SDL2 and a C11 compiler, then run from this directory:

```sh
# macOS dependency, if needed: brew install sdl2
make viewer
make run
make viewer-test
```

Space pauses; dragging or Left/Right rotates the gallery; R restores the front
view. W toggles wireframe, B shows the joints, +/- adjusts speed, and Esc quits.
The window is resizable. Fonts and rendering are built in; SDL2 is the only
external runtime dependency.

```sh
./mk4-viewer --headless --time 0.2 --screenshot build/gallery.bmp
./mk4-viewer --archive MK4/FILESYS.DAT --exe MK4/MK-4.EXE
./mk4-viewer --help
```

The executable supplies the skeletons, roster, pointer fixups, animation frame
counts and idle selection. The current profile supports the supplied June 1998
`MK-4.EXE`; another revision is rejected if its animation-table signature differs.
The archive stays read-only. No game executable code runs inside the viewer.

`make viewer-test` checks original x86 rotation fixtures, complete bone-channel
mapping, upright poses throughout all 865 roster idle frames, loop wrapping and
valid joint matrices. An optional independent verifier requires Python's
`unicorn` package and checks every animation word against the original x86
routines in an isolated emulator:

```sh
python3 tests/verify_original.py
```

See [viewer/FORMATS.md](viewer/FORMATS.md) for the verified loading path and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for source attribution.

## Supplied archive

The inspected `MK4/FILESYS.DAT` is 24,096,506 bytes:

| Contents | Entries |
| --- | ---: |
| ESF sounds (`ESF` followed by byte `06`) | 674 |
| Geometry containers (`0.1v`) | 122 |
| Menu TGA image | 1 |
| Animation directory and animation pack | 2 |
| Total | 799 |

Filename recovery using `MK4/MK-4.EXE` produces 797 candidates. The animation entries have hashes `2da2df95` (2,964 bytes) and `2da3df95`
(1,138,516 bytes), now identified as `ANIM.AP0` and `ANIM.AP1`.
The extractor preserves them as `.bin` files. Extraction does not
decode geometry, convert sound to WAV, or decompress data inside resources.

The index occupies the first `0x3000` bytes: up to 1024 consecutive 12-byte
records, each containing three little-endian `uint32_t` values:

```text
filename_hash, absolute_payload_offset, payload_size
```

Unused records are zero-filled. Entries are strictly sorted by unsigned hash;
payload order is independent of index order. In this archive the payloads
cover all bytes from `0x3000` to EOF with no gaps or overlaps. The parser checks
table size, index ordering, zero padding, payload bounds, and overlaps before
writing anything. It does not hardcode the observed entry count of 799.

The game's hash uppercases the full DOS source path, adds each character into
one of four rotating byte positions with 32-bit addition, and adds the path
length. Filename recovery was checked against the FSYS routines in the supplied
executable (`0x004b13e0` normalization, `0x004b1470` hashing).

## References

Both projects are shallow Git checkouts, with their original history pointers
and license information retained:

- [MortalKombat.Tools](https://github.com/ermaccer/MortalKombat.Tools), checked out
  at `reference/MortalKombat.Tools/`: archive extraction/rebuilding and audio tools.
- [mk4decomp](https://github.com/nathan-casabieille/mk4decomp), checked out at
  `reference/mk4decomp/`: reconstructed game code and format research.

The package module and extractor are independent implementations; upstream projects are
research references. `AGENTS.md` identifies the most useful source files.
