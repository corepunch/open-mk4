# MK4 actor scripts in Lua

`scripts/lua/` holds generated fighter scripts as Lua 5.4 source: one function
per retail script label, grouped by shared capability.
`program.lua` lists the modules in link order; `mk4.lua` holds the shared
helpers and is loaded first. The game and every test
that runs a match open this program next to the linked data package.

The runtime is `game/lua/script_lua.c`. It owns one Lua state, runs each
actor script as a coroutine and resumes it from the shared `script_vm_step`
after the engine-owned playback prologue. Playback, waits, contact results,
pairing, native continuations and interruption stay in `ScriptVm` and the
engine; Lua decides which operation runs next. A script ends with
`return mk4.ready()`, `return mk4.finish()` or a native `mk4.tail(...)`
that takes the actor over. Falling off the end of a function is an error.

`make scripts` regenerates `scripts/mk4/program.asm` (one linked label per
script function) and `build/mk4_symbols.lua` (label and native IDs for the
runtime). `make lua-test` runs every scenario family in
`tests/test_lua_vm.c` twice and requires identical simulation state after
every tick; it also reports which functions and operations the families
reached.

## Regenerating scripts

```sh
make scripts-export       # decode MK4/MK-4.EXE and write the Lua modules
make lua-scripts-check    # compare without writing; also in scripts-check
make scripts             # link checked-in Lua labels and data; no EXE needed
```

Do not hand-write fighter behavior. `tools/import_mk4_data.py --lua` uses the
existing retail decoder and `tools/mk4_asm_to_lua.py` translator. All waits,
loops, branches, clips and native calls come from the hash-checked executable.
`tools/mk4_lua_layout.json` stores only the established entry names, original
addresses and module boundaries, including embedded no-op entries. Generated
comments identify source addresses. New decoded entries need names in this
map and roots in the import layout; implement any missing shared operation or
native service once in C, then regenerate. `mk4.lua` remains the shared helper
source, authored once for the runtime API.

To inspect a fresh export or another executable path:

```sh
python3 tools/import_mk4_data.py --lua --exe MK4/MK-4.EXE --output /tmp/mk4-lua
```

Normal builds use the checked-in generated files and restore the linked data
package and symbol map automatically. Exporting is explicit and needs the
supported retail EXE; it is never a normal build dependency.

## Reading a generated script

```lua
local mk4 = mk4
local native, labels = mk4.native_ids, mk4.labels
local S = mk4.module()

-- Retail script 0x004e5c80 in MK4/MK-4.EXE.
function S.high_kick_clip_71()
  mk4.attack_selector(0x2)
  mk4.sound_a(1)
  mk4.prefixed_to(10, 71)
  mk4.tail(native.KICK_CONTACT_NARROW)
end

return S
```

- `S` is the module's function table; `S.other` also resolves functions in
  other modules. Continue in another stream with `return S.other()`.
- Counted loops are `for` loops; branches are `if mk4.blocked() then return
  S.recovery() end`; roster selection is `return mk4.select_roster {
  scorpion = S.a, raiden = S.b, ... }` keyed by roster name (Kitana and Noob
  select the Sonya and Reiko entries).
- `native.NAME` is `MK4_NATIVE_NAME` from `scripts/mk4/native.inc`;
  `labels.name` is a linked data address for operations that pass one.
- Clips are numbers (animation descriptor indices); samples, ticks and
  selectors are integers; Q16 motion values keep their `0x...` spelling.
- Scripts run in a sandbox: integer arithmetic, `table`, `string` and
  `math` without `random`; no files, no `load`. Use `ipairs` or explicit
  keys for anything that affects the simulation.
- A function must call at least one yielding operation per 1,024
  operations and 1,000,000 Lua instructions, or the runtime reports it.

Data tables, command lists, reaction tables and natives reach a script by
its linked address: `tools/mk4_script_labels.py` gives every function defined
here a label, `tools/mk4_lua_symbols.py` maps the labels to the addresses the
linker assigned, and the runtime resolves entries through that map. Tables
and exports refer to generated function names after `make scripts`.

## Operations

Each operation keeps the semantics of the wordcode handler it replaced in
`game/script/vm.c`; the opcode is given for cross-reference. Operations that
start playback, wait or hand the actor to a native suspend the coroutine
until the engine is ready for the next operation.

| Operation | Opcode | Effect |
| --- | --- | --- |
| `clip(clip)` | 01 | Bind `clip` at sample 0 and play to its end |
| `play_clip(sample, clip)` | 02 | Bind `clip` and play to `sample` |
| `forward()` | 03 | Play to the last sample |
| `forward_count(n)` | 04 | Advance `n` samples, one per tick, clamped at the end |
| `forward_to(sample[, clip])` | 05, 47 | Play to `sample` when it lies ahead; optionally bind `clip` first |
| `prefixed_to(sample[, clip])` | 47 | Same as `forward_to` |
| `backward_count(n)` | 06 | Play back `n` samples, not before sample 0 |
| `backward()` | 07 | Play back to sample 0 |
| `clip_alt(sample, clip)` | 08 | Bind `clip` at `sample` and play to the end |
| `clip_operation(clip)` | 09 | Bind `clip` at its last sample and play back to the start |
| `cursor_advance(n)` | 0a | Submit the sample `n` after the cursor |
| `bind_submit(sample, clip)` | 0c | Bind `clip` at `sample` and advance one sample |
| `playback_native()` | 0d | Submit the next sample |
| `open_peer_clip(clip)` | 0e | Bind `clip` on the opponent at sample 0 |
| `open_clip(sample, clip)` | 0f | Bind `clip` at `sample` without playing |
| `play_pair(n)` | 10 | Advance `n` samples while driving the captured peer |
| `bind_cursor(sample, clip)` | 11 | Bind `clip` at `sample` |
| `cursor_set(sample)`, `playback_tail(sample)`, `prefixed_sample(sample[, clip])` | 12, 1d, 49 | Submit one sample; 49 may bind `clip` first |
| `cursor_last()`, `submit_last()` | 13, 14 | Submit the last sample |
| `play_double(clip)` | 17 | Bind `clip`, start at sample 1, play to the end two samples per tick |
| `double_count(n)` | 19 | Advance `n` samples two per tick |
| `double_to(sample)` | 4d | Play to `sample` two samples per tick |
| `play_interleaved(clip)` | 1a | Bind `clip` and play with half-weight interpolated poses |
| `inherited_interleaved(n)` | 1c | Interpolated playback of up to `n` samples |
| `half_count(n[, clip])` | 46 | Half-speed playback of `n` samples (`0`: to the end) |
| `half_restart([clip])` | 68 | Half-speed playback from sample 0 to the end |
| `half_finish()` | 69 | Half-speed playback to the end |
| `prefixed_finish([clip])` | 48 | Optionally bind `clip`, then play to the end |
| `playback_pair(sample)` | 4a | Rewind to `sample` when it lies behind |
| `playback_single(sample)`, `playback_pair_reverse(sample)` | 4b, 4c | Paired playback to `sample` |
| `native_sound()` | 6a | Play to the last sample |
| `blend(clip)` | 71 | Quarter-weight blend from the current pose into sample 1 of `clip` |
| `wait(ticks)` | 34 | Suspend for `ticks` simulation ticks |
| `wait_branch(miss, hit)`, `wait_result(miss, hit)` | 37, 54 | Wait `hit` ticks after a landed contact, else `miss` |
| `park()` | 3e | Suspend until a native resumes the actor |
| `ready()`, `finish()` | 3f, 30 | Return through the entry's continuation |
| `return_word(limit, word)` | 1b | Return with a playback limit and a word for the native caller |
| `call(native)` | 35 | Dispatch a native call |
| `tail(native)` | 32 | Dispatch a native continuation that takes the actor over |
| `spawn(type, native)` | 40 | Spawn an independent task |
| `handler(native)`, `clear_handler()` | 38, 39 | Install or clear the body physics callback |
| `scene_event(n)`, `face(n)` | 56, 64 | Scene context and facing handlers |
| `native_return(n)`, `native_step(n)` | 41, 1e | Reaction dispatch handlers |
| `predicate_flags(native)`, `predicate_bit(native)`, `predicate_argument(n, native)` | 36, 43, 55 | Run a native predicate; true when the wordcode branch would be taken |
| `airborne()`, `down()`, `fast_start()`, `blocked()`, `contact()`, `edge()` | 2f, 44, 45, 3d, 42, 6f | Branch conditions |
| `query_branch(n)` | 50 | Contact query B, true when it landed |
| `query_b(n)`, `query_a(n)`, `query_d(n)`, `weapon_query(n)` | 51, 52, 53, 5e | Contact queries against the loaded record tables |
| `reaction_distance(n, limit)` | 70 | Within `limit`, dispatch a reaction and record contact |
| `attack_selector(word)` | 1f | Set the attack selector |
| `sound_a(n)`, `sound_b(n)`, `sound_c(n)` | 20, 2b, 27 | Sound banks |
| `sound_indexed(n)`, `sound_context(n)`, `sound_roster(n)` | 25, 26, 6b | Table-selected sounds |
| `sound_special()`, `sound_variant()`, `sound_random()`, `sound_token()` | 28, 2c, 73, 74 | Impact, voice and token sounds |
| `block_sound()`, `block_sound_alt()` | 75, 76 | Guard block sounds |
| `task_callback()` | 22 | Impact callback |
| `release_attack()` | 24 | Attack return |
| `action_recovery()`, `action_followup()` | 29, 2d | Action codes 1006 and 1007 |
| `spawn_effect(n)`, `effect(n)` | 2a, 62 | Shake and visual effects |
| `no_guard()`, `allow_commands()` | 2e, 61 | Flag changes |
| `scene_active()` | 16 | Scene activation |
| `native_release(n)` | 4f | Paired camera context |
| `damage(q16)`, `native_word(q16)` | 63, 6d | Damage and pair adjustment |
| `disarm_peer()` | 6e | Disarm |
| `motion(speed, up, gravity)` | 58 | Set speed, vertical velocity and gravity |
| `motion_block(left, right, up, gravity)` | 59 | Reaction motion |
| `peer_motion(speed, up, gravity)` | 65 | Directional motion |
| `speed(q16)`, `speed_negative(q16)`, `gravity(q16)` | 5f, 5a, 5c | Motion parameters |
| `position(forward, up)` | 5d | Move the body |
| `schedule_motion(delay, duration, speed)` | 6c | Scheduled motion |
| `landing()` | 5b | Stop |
| `native_context()` | 60 | Wait until grounded |
| `edge_push()` | 72 | Arena edge push |
| `roster_name()`, `result()`, `current_sample()` | | Queries: remapped roster name, contact result bits, current sample |

Opcodes `00`, `0b`, `15`, `18`, `21`, `23`, `4e`, `57`, `66` and `67` are
no-ops and have no operation.
