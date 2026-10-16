# Fighting-game architecture options

Research note for this workspace. It collects language, fighter-object, and
netplay choices. Written against commit `117a19b` (2026-10-16), then updated
when the match code took the data-table option.

The game now implements that option. `game/game.h` is the only match header.
A fighter is a roster index plus runtime fields. The roster row points at
shared motion, state, and command tables and at the fighter's own clip table.
Spawn writes the index. `battle_step` is the shared update. Keyboard, shell keys,
and the CPU are input sources in `game/input.c` and never share a code path
with the rules. `GAME_WARGODS` is reserved so another Eurocom/Midway roster
can be appended without a second battle type. Netplay is still local only.
The asset containers in `common/` are unchanged.

The sections below are the survey that led to that choice. "What the last
commit did" describes `117a19b`, not the roster change.

The short version:

- The last commit is behavior-preserving and stylistically noisier. `ARRAY_TYPE`
  is a hand-rolled `std::vector` typedef for every container, including several
  whose sizes are already known.
- Production fighters do not define Sub-Zero by subclassing `Character`. They
  run one shared simulator and point each fighter at data, a script, or a few
  function pointers installed at spawn. That is the Quake 2 shape, and it is
  close to the retail MK4 node pool. C can express it. C++ can express it too.
  The inheritance feature is not what decides the language.
- Online play does need a latency design. It does not require LAN ping if the
  design is rollback. Delay-only lockstep is the design that feels bad unless
  the ping is small and steady. A Quake-style authoritative server is a third
  design, and it is a poor default for a 1v1 fighter.

## What the last commit did

`117a19b` replaced the C++ asset, math, renderer, SDL, font, and frontend
modules with C11 structs and explicit context arguments. `fsys.cpp` stayed
C++17 and calls the shared C archive validator. The commit message records
`make check`, a strict C11 rebuild, ASan/UBSan, Unicorn agreement on all
36,723 animation frames and 16 rotation fixtures, byte-identical gallery and
fight BMPs, and the same 360-tick fight ending at health 40/100.

The combat simulation had already moved to C in `6028e78`. `Battle` is a
plain struct: two `Fighter` records, a `Move` table, and `battle_step`. That
part did not grow an `ARRAY_TYPE` layer.

What got worse is the asset and utility surface that used to be
`std::vector` and `std::string`.

Before, `common/mk4_assets.h` was a short list of structs. A mesh owned a
`std::vector<Triangle>`. A character owned a `std::string` name and a
`std::vector<Bone>`. `pose` returned a `std::vector<Pose>`.

After, `common/binary.h` introduces one macro and three others:

```c
#define ARRAY_TYPE(Name, Type) typedef struct Name { Type *data; size_t count, capacity; } Name
#define RESIZE(a, n) (a).data = array_resize(...)
#define PUSH(a, ...) do { RESIZE(a, (a).count + 1); (a).data[(a).count - 1] = (__VA_ARGS__); } while (0)
#define FREE_ARRAY(a) do { free((a).data); memset(&(a), 0, sizeof(a)); } while (0)
```

`ARRAY_TYPE` is then instantiated as `Bytes`, `Triangles`, `Meshes`,
`Pixels`, `Textures`, `Bones`, `Angles`, `Frames`, `Clips`, `Characters`,
`Sprites`, and `Arenas`. Every call site says `.data[i]` and `.count`.
Names and filenames became `char[1024]`. Several of those collections have
fixed bounds the header already publishes: 18 roster slots, 612 clips, 11
arenas, at most 128 bones. Heap vectors are useful for triangle and pixel
buffers, whose sizes really do vary. They are ceremony for the roster.

The same commit also compressed a lot of the new C into one line per
statement (`game/battle.c`, `viewer/main.c`, `common/mk4_assets.h`). The
logic is the same. The files are harder to read than the C++ they replaced,
and harder to read than the retail reconstruction, which uses ordinary named
structs and functions.

`Character` in this header is an asset: name, meshes, texture, bones, idle
clip, scale. `Fighter` in `game/battle.h` is the match participant: health,
position, action, facing. That split is sound. A class hierarchy hanging
gameplay off the asset struct would mix a rig with a health bar.

So the commit is a verified port with an unpleasant container encoding. The
encoding can be shrunk without leaving C, and the asset containers can move
back to C++ without putting virtual fighters in the simulator. Those are
separate decisions from "should a fighter be a subclass".

## How a fighting game is usually built

A match has a handful of entities, a fixed tick (here 60 Hz, one tick =
16.67 ms), and a rule that both sides must agree on which frame a hit
happened. The character is a large pile of timed data: startup, active,
recovery, hitboxes, hurtboxes, cancels, animation, audio. The engine is one
function that advances whoever is in play.

Published descriptions of that shape:

- Elecbyte's M.U.G.E.N puts every move in a text `[Statedef]`. The engine is
  shared. A character is a directory of `.def`, `.cns`, `.cmd`, `.air`, and
  sprites. `common1.cns` supplies walk, jump, guard, and hit reactions;
  a character file overrides the states it wants to change. Hundreds of
  states per character is normal, and none of them is a C++ class.
  <https://www.mugen-net.work/wiki/index.php?title=M.U.G.E.N_Documentation:CNS_Format>
- The long GameDev.net thread "How to handle states in a fighting game?"
  argues the same thing from the simulator side: one general attack path,
  with the animation and hitbox table saying what this attack is. Adding a
  combo is another row, not another state class.
  <https://www.gamedev.net/forums/topic/639005-how-to-handle-states-in-a-fighting-game/>
- Andrea Jens, "I wanna make a fighting game", part 3, describes the
  character as one state machine whose transitions are inputs and hit
  events. Part 5 then separates that logic tick from rendering, locks the
  timestep, and warns that gameplay floats desync across CPUs.
  <https://andrea-jens.medium.com/i-wanna-make-a-fighting-game-a-practical-guide-for-beginners-part-3-2021-update-12a04e48717a>
  <https://andrea-jens.medium.com/i-wanna-make-a-fighting-game-a-practical-guide-for-beginners-part-5-f049a78ddc5b>

Tutorial code often does use a class per state (`Enter` / `Tick` / `Exit`)
or a class per character. That organises a small prototype. It scales badly
once a roster has hundreds of timed frames, projectiles, and cancels,
because the interesting difference is data and the shared step still has to
know about hitboxes, blockstun, and pushback.

C++ is still the language of many shipped engines (Unreal for a lot of
modern fighters, RE Engine for Street Fighter 6, Elecbyte's own engine for
M.U.G.E.N). The language is carrying modules, containers, editors, and
platform code. The roster is still data plus a shared tick.

## Object models

These are interchangeable with the language section. Any of the first five
can be written in C or in C++.

### 1. One action enum, one move table

What we have. `Action` is idle, walk, crouch, jump, punch, kick, sweep,
uppercut, throw, block, hit, knockdown, victory. `battle_move` returns one
of five shared `Move` rows. Both fighters run through `battle_step`. The
character index selects a model and an idle clip, not a moveset.

This is enough for the current approximation: movement, guard, a few
normals, throws, hitstun, launches, a CPU, rounds. It cannot represent
Sub-Zero's ice clone and Kitana's fan as different rows, because the rows
are the actions themselves.

Keep it while the game is still a single shared moveset. Replace it when
the first character-specific move arrives.

### 2. Per-character tables, one fighter struct

```c
typedef struct MoveDef {
    int state;
    int startup, active, recovery, damage, stun;
    /* hitboxes keyed by frame live beside this, or in a parallel array */
} MoveDef;

typedef struct CharacterDef {
    int asset;          /* index into the loaded rig/clips */
    const MoveDef *moves;
    int move_count;
} CharacterDef;

typedef struct Fighter {
    const CharacterDef *def;
    int state, ticks, health, facing;
    /* position, velocity, stun, ... */
} Fighter;
```

Spawn copies a pointer (or an index) to that character's table into a
fighter. The step function stays one function. A new fighter is a new
table and a new set of clips. M.U.G.E.N's CNS is this idea with a text
syntax and a tiny controller language on top. The GameDev.net advice
("the data tells the code what to do") is this option.

Fits this repo. `Fighter` is already separate from the asset `Character`.
The next concrete step is a `CharacterDef` with its own move rows, still
advanced by `battle_step`. No new language feature required.

### 3. Quake 2 entity plus spawn callbacks

Quake 2 is C. Every monster, item, door, and player is the same `edict_t`
in a fixed pool. The spawn function for that classname fills fields and
installs function pointers. Fabien Sanglard's review of the source walks
through the kernel loading `gamex86.dll` through one exported `GetGameAPI`,
then calling `ge->RunFrame` each server frame:

<https://fabiensanglard.net/quake2/>
<https://fabiensanglard.net/quake2/quake2Polymorphism.php>

The game-side commentary at Gamers.org shows the berserk spawn writing
`self->monsterinfo.stand`, `.walk`, `.run`, `.melee`, `.sight` to ordinary
C functions, and notes that a C++ virtual would be the same pointer stored
one indirection deeper, in a vtable:

<https://www.gamers.org/dEngine/quake2/Q2DP/Q2DP_GameDLL/Q2DP_GameDLL.shtml>

The shared header is `game/g_local.h` in the id release:

<https://github.com/id-Software/Quake-2/blob/master/game/g_local.h>

The pointers on an edict are `think`, `touch`, `use`, `pain`, `die`,
`blocked`, and a `monsterinfo` block of further pointers. `G_Spawn` hands
back a slot from the pool. `SP_monster_berserk` specialises that slot.
Nothing subclasses `edict_t`.

A fighter version:

```c
typedef struct Fighter Fighter;
typedef void (*FighterFn)(Fighter *self, Battle *battle);

struct Fighter {
    int def, state, ticks, health, facing;
    float x, height, velocity;
    FighterFn think;     /* per tick while this state runs */
    FighterFn on_hit;
    FighterFn on_landed;
};

void spawn_subzero(Fighter *f) {
    f->def = CHAR_SUBZERO;
    f->think = fighter_think_shared;   /* or subzero_think, if it must differ */
    f->on_hit = fighter_on_hit;
}
```

Use a callback where a character really has unique code (a puppet, a
stance that replaces the neutral rules, a projectile that lives on its
own). Leave punch, kick, walk, and block in the shared step, driven by
the move table from option 2. A function pointer per frame of a normal
attack is how this model gets noisy.

Quake 1 did the same job in QuakeC: the entity has `.think` and `.touch`
fields, and the spawn function assigns them in a bytecode language. Quake 2
compiled that to a native DLL. Quake 3 went back to a bytecode VM (QVM) so
mods stayed portable. Sanglard notes that trade in the same review. A
fighter script VM is that story again. See option 5.

This is a good fit if we want character-specific code without a class
tree, and we accept that the pointers are installed by a spawn function
the match calls by roster id.

### 4. Retail MK4: a node pool, a callback, and a script decoder

The PC reconstruction is already this, in C, on top of a 32-bit packed
pointer model we should not copy.

`reference/mk4decomp/include/engine/scenegraph.h` describes a pool of 63
nodes, 232 bytes each. The same memory is viewed as a scene node, a fight
group node, or an audio source. A function pointer at offset `+0x10` is
the draw/tick callback the render walker calls. Nine transform kinds are
selected through `g_nodeDispatchTable`. Positions in this structure are
signed 32-bit fixed point. An FSM word lives at `+0x74`.

`reference/mk4decomp/analysis/notes/events.md` describes the behaviour
layer. Fatalities, intros, specials, stage hits, and timed state changes
go through a 20-slot event ring. Each node also carries a script cursor
(`entity[+0x64]` / `entity[+0x68]`) into an `(opcode, arg)` stream.
`EventPacketDecoder` reads opcodes and `Event_InvokeHandler` indirect-calls
`g_eventHandlerTable`. That is a spawn-time-installed program, not a
subclass per ninja.

`ARCHITECTURE.md` already records the boundary: our two fighter records
are a stand-in for that callback and script system. Copying the packed
pointer pool into the portable game would import a 32-bit address trick
the host does not need. Rebuilding the idea in ordinary C is option 3
plus option 5: a small entity array, a per-entity function pointer, and
later a bytecode cursor for specials and fatalities.

Choose this when the goal is structural fidelity to retail. It is the
largest gameplay change in this document. It is not required to delete
`ARRAY_TYPE`.

### 5. An external state script

The engine stays C or C++. Characters are programs in a small language.

| Host | Script | What varies per fighter |
| --- | --- | --- |
| M.U.G.E.N (C++ engine) | CNS / CMD text | states, hit definitions, commands |
| UnityMugen | Lua, with CNS compatibility as a goal | the same, in Lua |
| Quake 1 | QuakeC bytecode | entity spawn and think |
| Quake 3 | QVM bytecode | game module, portable across CPUs |
| Retail MK4 | `(opcode, arg)` stream on the node | specials, intros, fatalities, stage events |
| IKEMEN Go | MUGEN-like text on a Go engine | another reminder that the engine language and the character language diverge |

A script is justified once moves need conditionals that a flat table
expresses badly: "if the opponent is airborne and this is frame 4, spawn
ice". It is also the piece that makes a fatality authorable without a
recompile. It costs a decoder, a saved cursor, and a rule for what the
script is allowed to do to the match state.

For rollback, the script cursor and any script locals have to live inside
the saved match state. The program text is an asset and stays outside it.

### 6. One class per state

The state pattern: an `IState` with `Enter`, `Tick`, `Exit`, and a machine
that swaps the current object. A Japanese writeup of 2D action and fighter
structure argues for this so each new move is a new class rather than
another branch in a large update:

<https://zenn.dev/platypus2000jp/articles/9fd077742ddb62>

Fishfolk Punchy (Rust, Bevy) does a related split: one ECS component per
state, and systems that query "fighters that currently have `Attacking`":

<https://github.com/fishfolk/punchy/wiki/Fighter-State-Machine>

This stays readable for a few dozen hand-written states. A retail-sized
MK move list is hundreds of timed frames of data, and the classes either
become thin wrappers over that data or they duplicate it. Punchy's authors
wanted mods to add states without editing the core, which is the same
desire as option 5, satisfied with components instead of a script.

### 7. A general entity-component system

An ECS pays off when thousands of entities share a few behaviours and the
work is "every entity with a velocity". A fight is two characters, a few
projectiles, and a strict order: input, state, movement, hit test, damage,
push, facing. Save/restore for rollback wants that state in one contiguous
block. An ECS scatters it across component stores unless the whole store
is snapshotted on purpose.

Punchy shows an ECS fighter can be built. It is a poor match for a
two-player game whose reference architecture is a 63-slot node pool and
an event ring.

### 8. A C++ subclass per fighter

```cpp
struct Character { virtual void tick(Battle&); };
struct SubZero : Character { void tick(Battle&) override; };
```

This is the option that looks like the reason to pick C++. Shipped
fighters, M.U.G.E.N, Quake, and the MK4 node pool all avoid it. A virtual
`tick` on the whole character recreates `think` with a vtable. A virtual
per move recreates the move table with no place to put per-frame hitboxes
unless those virtuals start reading tables anyway. Shared rules (block,
pushback, round state, combo scaling) still have to live somewhere both
subclasses can see, which turns into a base class that knows every rule
and leaves the overrides as exceptions.

C++ remains a reasonable engine language for other reasons, listed next.
Those reasons are containers, RAII, and tooling. They are not fighter
inheritance.

## Language options

### Stay in C11 and shrink the containers

The simulator is already C, and it is the clean part: `Battle` has no
pointers, no macros, and a single step function. The retail guide we are
following is C. The noise is confined to asset loading.

A C-only cleanup, in increasing order of change:

- Format the compressed C so a function is more than one line. No design
  change.
- Stop minting a named typedef per element. One growable-buffer struct is
  enough, used at the two places whose length is data-dependent (triangles,
  pixels).
- Store the fixed collections in arrays: 18 characters, 612 clips, 11
  arenas, 128 bones, 128 poses. The bounds are already `#define`s in
  `common/mk4_assets.h`. `pose` already writes into a caller-owned
  `Pose[MK4_MAX_BONES]`.
- Keep `char name[N]` only if N is a real format limit. 1024 is a
  substitute for `std::string`, not a property of the archive.

This keeps one language for the game and the decomp, and it removes the
macro surface that prompted this note. `fsys.cpp` can stay the C++17
extraction tool described in `AGENTS.md`.

### C for the match, C++ for assets and tools

`Battle`, `Fighter`, and `battle_step` stay POD C. The archive decoder,
mesh buffers, and image buffers go back to `std::vector` and
`std::string`, or to a single C++ container module with RAII. The public
simulation header does not include `<vector>`.

This is how the code looked between `6028e78` and `117a19b`, and the
behaviour tests already treat the two as equivalent. It is the direct
answer to `ARRAY_TYPE` if the macro zoo is the part that feels wrong and
the C simulator is the part that feels right.

The cost is two languages again, plus `extern "C"` on the simulation API.
The game binary would link the C++ runtime, which `117a19b` deliberately
dropped. That is a real cost only if a C-only game binary is a goal on
its own.

### C++ throughout, with a POD match state

Write the engine as C++17. Ban, for the match state: virtual fighters,
exceptions as control flow, `std::string` and `std::vector` inside
`Battle`, `std::function`, and STL containers whose iteration order or
allocation would change a tick. Allow them in the loader, the viewer, and
the renderer.

`Battle` remains a trivially copyable struct so a later rollback
implementation can `memcpy` it. Character differences stay in tables and
spawn functions. Templates, if used, stay in math and containers.

This is the usual "C++ fighter" in practice. The extra language features
over option "stay in C" are RAII for assets, `std::vector` instead of
`ARRAY_TYPE`, and a stricter type system around the loader. They do not
change the fighter model.

### C++ with a fighter hierarchy

Covered as object model 8. Available, and a poor fit for this roster and
for the retail architecture. Choosing C++ in order to inherit `Character`
spends the complexity on the feature the genre does not use.

### A different language, as a different project

Rust (Punchy / Bevy), C# (a Unity or Godot fighter, or the small
deterministic-lockstep experiments on GitHub), and Go (IKEMEN) are all
real hosts for a fighter. They bring their own containers, so nobody
writes `ARRAY_TYPE`. They also discard the C reconstruction, the Unicorn
fixtures against retail x86, and the current SDL renderer. That is a new
game that happens to load MK4 assets, not a cleanup of this tree.

A scripting language beside C or C++ is option 5 and is not a rewrite.

## Netplay options

Local play today calls `battle_step` once per 60 Hz tick with two `Input`
blocks. Nothing is on the network. Any online design starts from that
function, because the tick is already fixed and already separated from SDL.

There are three places a design can put latency. Input can wait. The
simulation can guess and correct. Or a server can be the only copy of the
simulation and everyone else draws what it says. Fighting games converged
on the second, with a little of the first mixed in.

### What the ping actually costs

At 60 Hz a frame is 16.67 ms. Players quote delay in frames. A rough
conversion from round-trip time, which is what "ping" usually means:

| Round trip | Frames at 60 Hz | Typical path |
| --- | --- | --- |
| under 5 ms | under 1 | Wired LAN |
| 15–30 ms | 1–2 | Same city, good route |
| 30–50 ms | 2–3 | Same region |
| 80–120 ms | 5–7 | Cross-country |
| 150–220 ms | 9–13 | Another continent, or a bad route |

Wi-Fi and bufferbloat move these numbers around from frame to frame. A
steady 3-frame delay is more playable than a delay that hunts between 2
and 8. Infil's notes from people who shipped rollback say a steady 1–2
frames is already uncommon for players half an hour apart, and that
delay-based Guilty Gear sessions rarely held 1 frame:

<https://words.infil.net/w02-netcode-p6.html>

LAN does not have an internet-ping problem. A wired switch is usually
under a frame. Wi-Fi LAN can still jitter. The hard case is the public
internet, and the architecture decides whether that RTT becomes input lag,
a misprediction, or a wait for the server.

### Delay lockstep

Each peer sends its input and advances a tick only after the other side's
input for that tick has arrived. Both games then run the same inputs and
stay identical. Bandwidth is tiny: two input bitmasks, sixty times a
second.

Glenn Fiedler, "Deterministic Lockstep":

<https://gafferongames.com/post/deterministic_lockstep/>

Jay Mattis, SnapNet, "Netcode Architectures Part 1: Lockstep":

<https://www.snapnet.dev/blog/netcode-architectures-part-1-lockstep>

The whole RTT shows up as delay before a button does anything, because
the tick cannot start until the remote input exists. Implementations hide
some of that by delaying both players a fixed amount and buffering, but
the delay has to be at least large enough for the slower direction plus
jitter. Ars Technica's 2019 walkthrough describes Mortal Kombat X hunting
between 5 and 20 frames under this model, and holding the inflated delay
for seconds after a short spike:

<https://arstechnica.com/gaming/2019/10/explaining-how-fighting-games-use-delay-based-and-rollback-netcode/>

This is the design that needs "good ping" in the sense players mean it.
Under about 50 ms and steady, casual play is acceptable and competitive
play is already softened (3 frames is 50 ms off every reaction). Past
that, links and punishes change. A stall while a packet is late freezes
both games.

It is also the simplest correct online design, and it is what rollback is
built on top of. Infil's interview with krazhier: turn the prediction off,
and a rollback game is a delay-based game.

Reasonable for a first network test on a LAN, or as the bottom layer that
rollback later wraps. Poor as the only internet mode.

### Rollback, peer to peer

Both peers simulate every tick. Each one applies its local input
immediately and assumes the remote player repeated their previous input.
When the real remote input arrives and differs, the match rewinds to the
first wrong tick and resimulates forward. The local player never waits
for the network to see their own move. The remote character is what might
be wrong, for a few frames, and then it corrects.

Tony Cannon wrote GGPO for this around 2005–2006 and described it in Game
Developer, September 2012. The library is MIT licensed.

<https://www.gamedeveloper.com/programming/the-lag-fighting-techniques-behind-ggpo-s-netcode>
<https://github.com/pond3r/ggpo>

SnapNet's part 2 is a clear modern restatement, including the usual
compromise: a few frames of fixed input delay so that nearby matches
rarely roll back at all. Their default eats the first 50 ms (3 frames at
60 Hz) as delay and rolls back whatever remains.

<https://www.snapnet.dev/blog/netcode-architectures-part-2-rollback>

Ars (the Infil article above) is the long public explanation of why the
genre treated this as the repair for delay-based play. Shipped examples
with public confirmation include Street Fighter 6 (Capcom, a new
implementation, not the SFV one; Nakayama, reported by IGN and GameSpot
at Summer Game Fest 2022), Guilty Gear Strive (Arc System Works, announced
as rollback from the start), and Mortal Kombat 1 (WB Games / NetherRealm,
stated in their pre-release Q&A). Tekken 8 shipped rollback as well.
SFV had a rollback implementation with a bad reputation; having the word
in the feature list is not the same as the GGPO behaviour.

What rollback demands from this codebase:

- The match state is a value that can be copied and restored. `Battle`
  already is. `memcpy` in, `memcpy` out. Projectiles, script cursors, and
  RNG state have to join that struct when they exist.
- Assets, SDL, the framebuffer, and decoded clips stay outside it. They
  are not functions of the last 8 inputs. `ARRAY_TYPE` buffers full of
  heap pointers would be a bad thing to put inside the saved state.
  They are currently outside `Battle`, which is the right side of the line.
- `battle_step` is deterministic given `(Battle, Input[2])`. Same inputs,
  same bytes out. The resimulation budget is harsh: SnapNet's 8-frame
  catch-up inside a 16.67 ms render frame leaves about a millisecond per
  resimulated tick if the renderer also has to run. Our step is a few
  dozen arithmetic operations. The renderer is the part that must not run
  again during a rollback.
- Fixed point, or a float discipline. See below.

LAN under rollback with 0 or 1 frame of delay is indistinguishable from
the offline game, because the prediction is almost always confirmed before
it is shown. Internet play stays honest further out than delay-only,
and it still falls apart when the RTT exceeds the rollback window or
packets drop in bursts. One-sided rollback (one player smooth, the other
eating every correction) is a clock-sync bug; Cannon has described it as
a failure mode of uneven delays, SFV included.

Rollback does not need a machine in the middle to simulate the match.
It needs the two peers, and a way for UDP packets to meet.

### A relay or a matchmaking server that does not simulate

Peer-to-peer UDP fails when a NAT cannot be punched. The usual repair is
a server that forwards the same input packets and does not run
`battle_step`. Spectators can sit on that relay a few seconds behind, the
way Fightcade does, so they do not add their ping to the players' delay.

Ars notes SFV falling back to a server when a direct connection fails, and
treats Brawlhalla as a game that puts a server between players. Forwarding
adds a hop. It does not change the lockstep-or-rollback choice. It is
operationally simpler than an authoritative simulation and it avoids host
advantage.

This is the right "we have a server" design for a 1v1 fighter unless cheat
resistance becomes a requirement.

### Authoritative server, Quake style

Quake's internet play, and most shooters after it, keep one true
simulation. Clients send inputs. The server sends snapshots of positions,
health, and events. Remote entities are interpolated between snapshots.
The local player is often predicted so movement feels immediate, then
corrected if the server disagrees.

Quake 2's specific form, from Sanglard's review: one executable contains
both halves. `SV_Frame` runs the game DLL. `CL_Frame` renders. A
dedicated server skips rendering. A listen server runs both. Single player
still runs both, and the client talks to the server through a loopback
buffer (`NET_SendLoopPacket` / `NET_GetLoopPacket`) instead of a socket.
So "client/server" in that engine is a code boundary first and a network
only when the transport is a socket.

Fiedler, "Snapshot Interpolation", recommends this once the simulation
cannot be made deterministic or the player count leaves the 2–4 range
where lockstep still works:

<https://gafferongames.com/post/snapshot_interpolation/>

SnapNet part 3 covers the same architecture:

<https://www.snapnet.dev/blog/netcode-architectures-part-3-snapshot-interpolation>

Mapped onto a fighter, the button-to-confirmed-hit time is a round trip
to the server unless the client also predicts. Both players must see a
3-frame normal connect or whiff on the same visual frame. Interpolation
draws the opponent in the past, so a throw range and a poke range lie
about themselves. A listen server gives the hosting player zero ping into
the true simulation and gives the joiner the full RTT. That host advantage
is accepted in many shooters and rejected in fighting games.

The loopback lesson is still useful locally. Calling the input side a
client and `battle_step` a server, even in one process, keeps the later
network transport from growing into the rules. The transport for offline
play is a function call. The transport for a LAN experiment can be UDP
carrying `Input`. Neither forces an authoritative remote server.

Choose a real authoritative server if the match grows past what two
deterministic peers can host (many independent actors, physics that will
not stay deterministic) or if clients cannot be trusted with the rules.
A 1v1 MK-style match is the case lockstep and rollback were built for.

### Authoritative server plus rollback

SnapNet's product is this combination: the server owns the true
simulation, clients predict and roll back, and cheats that depend on
being the authority get smaller. The Unreal integration even uses a
separate world for the simulation and a separate world for presentation,
because a rollback tick must not spawn particles or play audio.

<https://www.snapnet.dev/docs/overview/>
<https://www.snapnet.dev/docs/core-concepts/simulation-vs-presentation>

This is the large, commercial version of online play. It wants a server
process, a prediction policy, lag compensation, and a simulation that can
be rewound on both sides. It is justified by ranked integrity and by
engine systems (physics, navigation) that are not deterministic. It is
far more machinery than this repo needs in order to play a lagged match
across a LAN or a region.

### Determinism, because lockstep and rollback both require it

Both peers run the full rules, so the only data on the wire is input.
Fiedler and Mattis state the prerequisite the same way: bitwise
determinism, and a shared fixed tick. Our tick size is already
`MK4_TICK_SECONDS (1.f / 60.f)` and the host already steps the battle on
that clock, separate from how fast SDL presents a frame.

Floats are the hazard. `Fighter.x`, `height`, and `velocity` are `float`.
Gravity is `15.f`. Same compiler, same ISA, same flags, no fast-math: this
often reproduces. Across macOS ARM and Windows x86, libm and fused
multiply-add choices do not have to agree. Fiedler's floating-point
article is the long version:

<https://gafferongames.com/post/floating_point_determinism/>

Jens's part 5 says the practical answer for a PC fighter is fixed point
or integers inside the rules, and floats only in the picture. Retail MK4
already stores node positions as signed fixed point (`s32 position_x` and
neighbours in `ScenegraphNode`). Our pose math is float on purpose, and
it is checked against the retail x86 routines by Unicorn. That check says
the renderer matches one x86 binary. It does not say two different CPUs
will compute the same gameplay float.

A rollback-ready cleanup can keep floats in the renderer and move the
match to integers or a 16.16 type when online play is actually started.
Doing it earlier is cheap inside `Battle` because the struct is small.
Doing it to the pose pipeline is a different project and is only required
if gameplay starts depending on bone positions (throw attachment, hurtboxes
on the mesh) rather than on the fighter's own x and height.

RNG, if a fatality or a stage ever needs it, has to be a counter and a
table inside `Battle`, seeded at the round. Unordered maps and pointer
identity cannot affect a tick.

### What "good ping" means for each option

| Design | LAN, wired | Same region, ~40 ms | Cross-continent, ~180 ms |
| --- | --- | --- | --- |
| Offline / loopback | The current game | n/a | n/a |
| Delay lockstep | Fine, often 0–1 frame | Playable, softened, about 3 frames if the delay tracks the RTT | A different game; reactions and links move |
| Rollback, 0–3 frames of fixed delay | Identical to offline | Usually close to offline; occasional one- or two-frame corrections | Playable only inside the rollback window; corrections become visible; past the window it stalls or warps |
| Input relay, peers still simulate | One extra millisecond | Adds the relay's detour on top of the row above | Worth it only when the detour beats a failed direct path |
| Authoritative snapshots | Joiner feels the RTT; host does not, on a listen server | Confirmed hits lag by the RTT; interpolation shows the opponent late | Remote play is several frames behind the server |
| Server-authoritative rollback | Close to offline if the server is in the room | Good if the prediction window covers the RTT, at the cost of the server | Same window problem as peer rollback, plus the server's own tick |

## Combinations that fit this repository

These are packages, not a choice already made.

**Clean C, tables later, rollback-ready state.** Delete the per-type
array typedefs. Use plain arrays where the count is a roster constant and
one growable buffer where the file actually varies. Keep `battle_step` as
the only rules function. When characters diverge, add `CharacterDef`
tables (object model 2) and a spawn function that points a `Fighter` at
one table (object model 3, used lightly). Leave the door open for an
opcode cursor (object model 4 and 5) when a special or a fatality needs
it. Keep `Battle` free of pointers so a future GGPO-style layer can copy
it. Stay delay-free and offline until that layer exists. This matches the
decomp's language and the current combat code, and it answers the
`ARRAY_TYPE` complaint without a second language.

**C match, C++ assets.** Restore `std::vector` / `std::string` (or one RAII
container) to the loader and the viewer. Leave `game/battle.h` as the C
API. Same fighter and netplay story as the package above. Pick this if
reading the asset code matters more than a C-only game binary.

**C or C++ engine, Quake-shaped entities.** Introduce a small fighter and
projectile pool, spawn functions per roster id, and `think` / `on_hit`
pointers only where a character leaves the shared table. Local play uses
the loopback shape from Quake 2: the rules run in a function the host
calls, and a socket is a later replacement for that call. First network
test is delay lockstep on a LAN, because it is rollback with the
prediction turned off. Rollback is the internet mode, peer to peer, with
a relay only for NAT. Fixed point in `Battle` before any machine other
than the developer's is expected to agree.

**Retail-shaped rules.** A node pool, a per-node callback, an event ring,
and an `(opcode, arg)` decoder, rebuilt with ordinary pointers and
indexes. This is the long fidelity project. It wants its own plan, its
own tests against the retail executable, and it should not be mixed into
a container cleanup. Netplay stays the same constraint: whatever the
decoder writes has to live in the copied match state.

Packages that this repo has little reason to take: a subclass per fighter,
an ECS, an authoritative Quake server as the first online mode, and a
rewrite in Rust, C#, or Go.

## Sources

Retail structure, in this workspace:

- `ARCHITECTURE.md` — current language decision and the retail-to-portable
  boundary
- `reference/mk4decomp/include/engine/scenegraph.h` — 63-node pool, callback
  at `+0x10`, fixed-point positions, FSM word
- `reference/mk4decomp/analysis/notes/events.md` — event ring and
  `(opcode, arg)` decoder
- `reference/mk4decomp/analysis/notes/architecture.md` — index of the
  reconstruction's subsystem notes

Fighter structure:

- M.U.G.E.N CNS format, Elecbyte 1.1 docs, mirrored at
  <https://www.mugen-net.work/wiki/index.php?title=M.U.G.E.N_Documentation:CNS_Format>
- "How to handle states in a fighting game?", GameDev.net
  <https://www.gamedev.net/forums/topic/639005-how-to-handle-states-in-a-fighting-game/>
- Andrea Jens, "I wanna make a fighting game", parts 3 and 5
  <https://andrea-jens.medium.com/i-wanna-make-a-fighting-game-a-practical-guide-for-beginners-part-3-2021-update-12a04e48717a>
  <https://andrea-jens.medium.com/i-wanna-make-a-fighting-game-a-practical-guide-for-beginners-part-5-f049a78ddc5b>
- State-pattern writeup (classes per state):
  <https://zenn.dev/platypus2000jp/articles/9fd077742ddb62>
- Fishfolk Punchy, one component per state:
  <https://github.com/fishfolk/punchy/wiki/Fighter-State-Machine>

Quake, as the C entity-and-spawn reference:

- Fabien Sanglard, Quake 2 source review (kernel, client/server loopback,
  `GetGameAPI`, and the note that Quake 3 returned to a VM)
  <https://fabiensanglard.net/quake2/>
  <https://fabiensanglard.net/quake2/quake2Polymorphism.php>
- id Software, `game/g_local.h` (the edict and its function pointers)
  <https://github.com/id-Software/Quake-2/blob/master/game/g_local.h>
- Quake 2 Game DLL commentary (spawn fills function pointers; a vtable is
  the same mechanism with another indirection)
  <https://www.gamers.org/dEngine/quake2/Q2DP/Q2DP_GameDLL/Q2DP_GameDLL.shtml>

Netplay:

- Tony Cannon, "The lag-fighting techniques behind GGPO's netcode",
  Game Developer, September 2012
  <https://www.gamedeveloper.com/programming/the-lag-fighting-techniques-behind-ggpo-s-netcode>
- GGPO, MIT license <https://github.com/pond3r/ggpo>
- Infil, "Explaining how fighting games use delay-based and rollback
  netcode", Ars Technica, 16 October 2019, and the developer interview
  <https://arstechnica.com/gaming/2019/10/explaining-how-fighting-games-use-delay-based-and-rollback-netcode/>
  <https://words.infil.net/w02-netcode-p6.html>
- Glenn Fiedler, "Deterministic Lockstep", "Snapshot Interpolation",
  "Floating Point Determinism"
  <https://gafferongames.com/post/deterministic_lockstep/>
  <https://gafferongames.com/post/snapshot_interpolation/>
  <https://gafferongames.com/post/floating_point_determinism/>
- Jay Mattis, SnapNet, "Netcode Architectures" parts 1–3 (lockstep,
  rollback, snapshot interpolation)
  <https://www.snapnet.dev/blog/netcode-architectures-part-1-lockstep>
  <https://www.snapnet.dev/blog/netcode-architectures-part-2-rollback>
  <https://www.snapnet.dev/blog/netcode-architectures-part-3-snapshot-interpolation>
