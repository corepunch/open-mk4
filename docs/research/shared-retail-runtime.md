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

## Roster identity and base stances

The shared reader now loads roster names from 4ee1f0 and ground offsets from
the referenced rig root in each 32-byte row at 4eb2e8. Idle/walk descriptors
come from the original 53d408/53d388 tables. Name, asset identity and ground
constants are removed from all 18 native integration records, along with their
idle/walk MoveDef copies. Generic runtime state views supply these two stances.
Other state entries and native parameter blocks are still legacy integration.

The layout is the same independently decoded by `common/mk4_assets.c` and the
mk4decomp rig/state routines cited in AGENTS.md. The asset self-test verifies
all 18 runtime names and ground offsets against the loaded meshes/rig roots,
and validates all state clips. This corrects Kitana's old copied Sonya ground
offset by loading Kitana's actual rig value. The two old morph assertions now
use the runtime roster API rather than reading a C definition's ground field.

Verification: shared runtime suite, C11 game build, format-check, all 18
canonical script/source checks and the existing game asset self-test pass.

## Shared script entry

`retail_vm_enter` now starts an original address inside the checked retail data
window, with an explicit native return continuation. The per-fighter 70-root
switch and its graph-bound constants are removed. Offline inventory checks
still verify that decoded roots lie inside the shared window and use supported
operations. Individual entries do not need runtime C enrollment. VM state keeps
the source entry separately from the checked data bounds for diagnostics.

The native ABI glue is now a runtime module, `retail_native_vm.c`. It remains a
transitional adapter to legacy native routines, not a completed native port.
This change does not enable all other fighters' generated roots automatically:
their remaining opcode/native semantics must still be migrated and verified.
Existing shared-runtime and rejection checks pass with the generic entry path;
the small explicit-window fixture retains cursor-boundary rejection coverage.

## Native predicate consolidation

Local native addresses 45e530, 45e570 and 45ef20 now each have one shared
implementation. Remove their 20 fighter variants and redirect existing native
callers to the shared routines. The predicate ABI is keyed only by native
address; it no longer selects an implementation through a character mask.
The input adapter still has legacy action implementations to consolidate.

Bounded local listings establish the ranged finish limit, close finish gate,
and group +4c gravity test. The finish paths join 45e5e0, whose checked
mk4decomp correspondence is `PendingMatch_SwapOrPassSet` in
`pending_match_variants.c` / matching assembly. These bodies preserve the
existing engine finish-window abstraction while removing duplicated C logic.

Existing predicate assertions now refer to shared APIs, with no new character
matrices. Runtime checks, C11 game and existing battle-test builds, formatting,
all 18 canonical script/source audits, and helper/source navigation checks pass.
The source tools now link removed weapon, stage, sidestep and predicate wrappers
to the actual shared native implementations.

## Shared playback and control-flow completion

The interpreter now covers the additional counted, paired, reverse-to and
half-speed playback modes used outside Scorpion, optional descriptor prefixes
for sample assignment, and blend-to-clip/resume-at-one. Blend state belongs to
the VM and is retired on interruption. Playback uses the shared animation
clock without generating frame programs. Added branch forms use the native
result bit selected by the opcode, live down input, or airborne state.

Evidence is mk4decomp's `PauseTestCmp2CallStore`, paired forward/reverse matching
assembly in `pending_match_variants.c`, and the supplied EXE handler table at
4e3988. Bounded local listings verify half-speed wrappers 45b800/45b6d0,
which call 429040/428ea0, and blend handler 45bc70. Existing reviewed decoder
and native contracts supply the shared motion, contact D, callback and sound
boundaries. This expands runtime capability; other fighters still require
native binding and entry migration before their generated outputs can retire.

One synthetic runtime fixture covers optional operand consumption, half-speed
holding, reverse playback, blend resumption and cancellation. No authored
fighter timing or damage cases were added.
