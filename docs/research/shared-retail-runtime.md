# Shared retail runtime migration

Target: all fighters use one interpreter, retail-loaded roster/command/action/
stance/CPU tables, and shared native implementations. No fighter C definitions
or per-fighter script registrations remain in the target architecture. The
existing fighter modules are migration inputs, not the desired organization.

## Retail table reader

`game/retail_data.c` follows mk4decomp's `EventPacketDecoder` matching assembly
in `src/engine/input_check_cluster.c` (upstream 45de60, local 45de00): each
record consists of pattern, extra word and action word, terminated by a zero
pattern. An all-ones pattern is unconditional. The action word carries the
predicate index, action flags, input window and action index. Action flags
select the first matching dispatch table. Player-specific command-list roots
remain distinct; hidden characters keep their own input lists.

`StateDispatchTable` in `src/engine/state_dispatch_table.c` supplies the separate
16->2 / 17->7 remapping for stance/CPU selection. All addresses are checked
against the supplied EXE; upstream virtual addresses differ. Retail indices
are data, never host code pointers.

One shared fixture modifies source records and dispatch data in memory to
verify player selection, live reads, predicate/action decoding, termination
and the hidden-roster remap. It does not test individual character behavior.

## Input integration

`fighter_press` and `fighter_release` now walk the live retail record list for
all 18 roster slots. Pattern order, windows, action flags, predicate selection,
and native targets come from the image on each input event. Compiled per-fighter
command arrays are removed. A runtime-derived compatibility view remains for
UI/test callers that inspect `Controls`; input matching does not use that cache.

`retail_native.c` is the shared native-address boundary. Its input bridge still
points to existing native implementations while those implementations are
consolidated. Character masks in this temporary bridge are not the target
architecture and do not claim shared native completion. Known native entry
points remain callable even where a particular character's input list does not
reference them. Thirty-three redundant C command wrappers and five identical sidestep wrappers
are removed; sidestep uses one shared native implementation.

The shared source-mutation fixture also routes a real player input through a
modified retail list/action table. Static source checks retain native entry
reachability and canonical CPU binding output; they no longer require compiled
command arrays. Character definitions, stances, CPU bindings, other fighters'
generated script bodies and native duplication still require migration.

## CPU table integration

CPU near/medium/far/projectile actions now read the four retail action tables
on dispatch. The compatibility roster view derives those slots and both finish
distances from the image; compiled special/projectile rows and finish-distance
copies are removed. A zero source row does not substitute another special.
Native ABI registrations also cover CPU-only entry points for hidden fighters.

Reference: mk4decomp `CmpDualPatchScaledRangeJmp` in
`src/engine/cmp_dual_patch_scaled_range_jmp.c` (upstream 4399c0, local 439960),
with `triple_stub_push_call.c` selecting the source table. It reads task +0x34,
remaps 16/17 to 2/7, checks for a zero row and passes the action word to
`HitReactionDispatcher`. Local 438e50 supplies table 4df8e8. The existing
portable CPU planner remains engine code; this step replaces its fighter rows.

The shared mutation fixture verifies live CPU dispatch and zero-row behavior,
and resolves the runtime command/CPU ABI for every roster slot. Runtime suite,
C11 game build, formatting and all 18 canonical script/source checks pass.
