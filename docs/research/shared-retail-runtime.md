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

## One native fan scene

All roster slots now use `retail_scene.c` for native entries 467e00, 43d460,
43d510/43d530, 43d6f0 and 43db50. The eighteen per-fighter fan modules, generated
fan bodies and generated-phase accessors are removed. Native entry masks for
467e00 collapse to a single roster-wide implementation. The remaining script
entry wrappers for this scene are removed as well; the native coordinator
enters original addresses directly.

The native countdown remains native code, while retail 4e0118/0128/0138/0148,
0158/0188/01a0 supply playback and their branches. The native peer test selects
414 for non-female victims and the 415 continuation for female slots 2/9
(including hidden-roster remapping). Earlier copies generally omitted this
native branch. The shared implementation follows the local 43d4b0 and
43d540/43d559 listings, with mk4decomp's `CountdownStoreCallChain` and the
matching dual-entry source as reference. Descriptor lengths come from the
loaded retail image, including native rebinds, rather than copied C counts.

Validation reuses shared VM checks, strict coverage/canonical audits and the
existing battle regressions for native scenes. No scene-specific tests added.
Other native subsystems and generated script roots remain migration work.

The standalone generated clip-frame files also had zero remaining symbol
references across game sources and tests. All seventeen remaining files and
their includes are removed rather than regenerating unused arrays after fan
migration. Shared VM checks, all strict script audits and source-navigation
checks pass. Existing battle regressions pass the fan/stage scenes, then fail
at the old exact-tick Scorpion throw assertion. An isolated build of pre-change
4b234fe fails at the identical assertion; it is a pre-existing test limitation,
not evidence of a new fan regression.

## Roster-wide typed native boundary

The shared VM native boundary now covers the reviewed native contracts required
by every catalogued retail script graph. The bridge uses real C signatures and
keeps call/tail, predicate, spawn, physics and handler roles separate. Physics
entries install callbacks rather than executing them; spawn task types are
checked before the call. Script-only native links re-enter the VM with its
continuation. Scene-event and facing opcodes now use the same handler boundary.

This bridge is explicitly transitional: roster masks select existing native
services until their duplicate implementations and configuration are replaced.
It does not translate or embed script words, timing, branches or frame rows.
The original typed contracts and C signatures supply registrations; native
implementations remain the existing reviewed engine adaptations. It is not a
claim of completed native gameplay parity or removal of per-character modules.

The focused native ABI fixture checks callback installation, wrong-role and
wrong-task rejection, unknown addresses and invalid owners. Existing runtime
and malformed-stream checks continue to validate native handoff and cleanup.

## All roster script roots execute directly

All eighteen fighter integrations now enter original words through the shared
VM. All remaining generated script C includes are deleted; no script/frame
include remains in any fighter source. The last native frame-expansion helper,
`native_repeat`, and its generated-phase accessors are removed. A bounded
native counter re-enters a script with its original image, typed callback and
successor; cancellation retires that counter. Caller-returning Liu throw
playback retains its explicit native successor instead of falling through to a
controller return.

Native bridge selection temporarily preserves the originating integration
context independently of a victim/scene actor's model roster. This corrects
cross-actor native dispatch during migration; it does not make native masks or
per-character entry adapters part of the target architecture. Existing native
modules, C state definitions and configuration still need consolidation.

The shared playback runtime now clamps counted/double-step rendering while
retaining the requested count, following mk4decomp `anim_frame_advance.c` and
the existing checked decoder contracts. A forward target may reach the
one-past-end completion cursor without rendering an invalid pose. Contact
record bounds include the retail D bank through 4ec290 (end 4ec2a8), rather
than the earlier Scorpion-only extent.

Validation: shared VM/native ABI/lifetime/repetition and malformed-stream
checks; all eighteen strict require-implemented audits and canonical checks;
source-navigation/helper checks (7/6); format and native-frame gates; game
build and self-test (all eighteen rigs). The existing battle suite passes the
migrated earlier integration scenarios and stops at the same pre-existing
exact-tick Scorpion throw assertion reproduced on 4b234fe. The added fixtures
exercise shared native repetition and native/model context separation, not
authored fighter timing or balance.

## Shared native actor helpers

Duplicate fighter-local controller returns, mutable peer lookup, victim action
queries and body-stop implementations now use the existing shared controller
and body services plus common typed actor accessors. Their old definitions,
private aliases and declarations are removed; native callback/profile callers
refer directly to the shared APIs. This removes native duplication without
renaming fighter directories or copying their logic into another layout.

Validation uses the existing shared VM/native and lifetime checks, strict
roster audits, formatting and source-navigation checks. No tests were added
for these small equivalent helpers.

## One retail-loaded high-kick contact runner

Native entries 46a6b0/46a760/46a790 now share `retail_combat.c`. Their original
parameter blocks 4e5d00/4e5d30/4e6978 provide the query selector, native query,
pose rows and terminating continuation. The runner consumes those words live,
latches the first contact, skips a crouching peer as the original does, and
resumes the indicated retail script. Seventeen fighter-local implementations,
copied attack records/pose rows and the old row-copy helpers are removed.
No roster mask is involved in these shared native entries.

Evidence: mk4decomp `PackedLoadAdvanceJmp` (upstream 470cc0, local 470c60),
matching `InstallSelfBranchIndirect` (470d10/local 470cb0), and the checked local
470dc0 latch, 470e80 recovery action and 46a6c0/46a770/46a7a0 continuations.
Local matching assembly is authoritative where portable comments disagree.
This also replaces the previous Jax shortcut that skipped the native runner.

A single shared runtime fixture mutates the native parameter source and checks
live pose/continuation reads and interruption without generated frame arrays.
Existing shared runtime/native tests and strict roster audits pass. The battle
integration run again reaches its pre-existing exact-tick Scorpion throw
assertion; no new character damage or timing cases were introduced.

## Shared native high/low kick entries

Native 46a680 and the post-gate low-kick block 46a81a now have one
implementation each. They establish the engine action context, store
0100/0102 and enter the original retail selector
4e5ca0/4e5d90. Thirty-six duplicate entry adapters and unused private clip
seeds are removed. Descriptor selection and playback come from retail words;
the engine supplies only a shared locked/record-driven action context.

The high-kick entry is verified against mk4decomp's matching
`PendingMatch_ArgSarStoreJmp` container and local 46a680. Existing controller
branches still decide close/roundhouse/weapon/throw choices until their own
native consolidation. The full 46a7b0 includes throw gates before 46a81a;
it is not registered as a completed shared native entry by this step. Existing low-kick/runtime integration coverage is reused; its
entry assertion now names the original selector rather than a former direct
selected-root adapter. No new tests were added.

## One native standing high-punch controller

All eighteen standing high-punch integrations now call `retail_high_punch`.
The fighter-local entry/profile adapters, copied HP attack records and
296/297 clip seeds are removed, along with the public `NativeHPProfile` API.
The existing shared native repeat/contact phase implementation remains native
code because 46d290 is a native state machine, not a retail script program.

Its source contact comes from live A[0]; descriptor 4fbb5c/4fbb68 lengths are
loaded through the checked image binding helper. Native recovery uses the
loaded last sample and rendering clamps within it. The checked local 46d9b3
entry stores action 010a, clears busy, emits A[10] and binds 4fbb5c; its native
helper links include mk4decomp `TriCounterReinitChain` (local 46dd30).

One shared native fixture changes the source record to zero damage, verifies
that contact still occurs, checks descriptor bounds and normal controller
return. It tests our native data boundary rather than copied fighter balance.
Shared VM checks, strict roster audits and helper/navigation checks (6/7)
pass. The existing battle run again stops at the documented baseline throw
tick assertion.

## Shared stance callbacks

Sixty-seven fighter-local wrappers that only called `weapon_stance` are
removed. Four shared callbacks now select idle, walking, crouching and block
states through the same weapon/state service. Runtime-loaded idle/walk data
and actor weapon selection retain their ownership. Four private C modules
that became include-only are also deleted. No stance or timing behavior is
reimplemented by this consolidation, and no tests were added.

Validation reuses the shared VM/native/lifetime checks, formatting and strict
roster/source audits.

## Shared jab playback and unused adapter removal

Nineteen jab playback copies now use `retail_jab`, which establishes the shared
native action context and enters 4e62f0 with action 010b and live commands.
The original LP throw gates remain in the controllers; this is the post-gate
46dcbc playback block, not a claim that the entire 46dca0 native entry is
finished. No bypassing full-entry registration was added.

Native 46ddd0 now reads contact A[1] live rather than a C attack record. The
local entry/gate/contact listings and checked mk4decomp
`TriCounterReinitChain` correspondence establish these boundaries. Kick, jab
and native HP setup share one engine action-context primitive.

Sixty-eight mechanical VM wrappers with zero incoming C/table references are
removed, including their declarations. Retail jumps and native row
continuations enter original addresses directly and do not need C thunks.
Research navigation points at the shared native entries. Coverage remains
1,109 interpreted catalog roots across eighteen slots.

Validation reuses shared VM/native/lifetime checks, all strict roster audits,
formatting, helper/navigation checks (6/7) and native source gates. No new tests
added. The battle integration run stops at the established baseline throw
assertion.

## Shared sweep native cursor and recovery

All eighteen sweep entries use native 46ef90 once: action 010c, independent
B[28] cue after fourteen visits and direct selector 4e6798. Both contact tails
46efd0/46f080 advance the currently bound cursor and read live B[36]/B[35].
The native recovery marks 1006, waits one visit and plays the loaded clip to
its end. Copied records, fixed 23/24 frame rows and fighter-local sweep
modules/adapters are removed. The original script still chooses descriptors
and authored control flow.

Evidence is the checked local native entry/contact/recovery listings and
mk4decomp's matching cursor-submit/native-state handlers. Unused legacy
navigation aliases that previously shared a sweep adapter retain their
mapping; this does not register additional native ABI addresses.

One small runtime fixture uses an arbitrary bound clip/cursor to verify
relative advancement, loaded end bounds and exactly one native return.
Shared VM checks, strict audits for all eighteen slots and native source
gates pass. Existing advancing-sweep integration passes; the battle suite
then stops at the already established baseline ground-throw tick assertion.
No per-fighter tests were added. Remaining native controllers and roster
configuration still require consolidation.

## Shared roundhouse native family

All eighteen roundhouse modules and their fixed native profiles are removed.
Native 46f1f0 stores010d and enters retail selector4e6800. Shared46f220/46f390
select the original fast/normal streams from the saved follow-up flag.
Shared46f270/46f3e0 advance/query at most three times with live B[33]/B[34].
No C roster clip choice, copied contact record or absolute-tick frame rows
remain for this family. Six unused roundhouse rows are removed from the
remaining ground profiles, whose API no longer carries roundhouse fields.
Fifty-four unused VM thunks are also retired.

Evidence: local46f1f0..46f644 and contact48d3f0, matched to mk4decomp's
`PrefixThunkInstallSelf3State` embedded bit-9/contact handlers and
`Install3StateMStackPopDispatch`. The latter saves the contact result across
playback: result above one (hit/air hit) installs its28-visit native recovery;
miss/block return after the bound clip. This replaces the previous C profile's
inverted hit/miss recovery approximation with the checked native branch.

Shared VM/native checks, all eighteen strict audits, formatting and native
source gates pass. Existing roundhouse hit/miss and Goro integrations pass;
the battle suite subsequently reaches the documented baseline throw tick
assertion. No new fighter tests or timing matrices were added. Other native
families and roster/controller configuration remain transitional.

## Shared uppercut playback and contact service

All eighteen uppercut modules are removed. Shared playback46cdb4 establishes
0109 and enters original4e62a0. The pre-existing finish-window branches in
46ccd0 are a separate controller boundary; this step does not register that
full native entry. Native46cde0 uses the existing shared recovery state
machine once, queries live A[2]/A[6] instead of copied fighter records and
finishes through the loaded clip bound. The six private contact variants and
Sub-Zero's duplicate native phase are retired, together with eighteen unused
startup thunks and the now-unused hold-phase adapter.

The entry/VM boundary follows checked local code and mk4decomp ArgSarStoreJmp;
the local46cde0 listing confirms selector2/6, crouch2001 exclusion, contact
cycles and distinct result recovery. Existing native phase implementation is
reused rather than re-authoring retail script control flow.

Shared runtime checks, eighteen strict audits and native source gates pass.
The existing battle run reaches the established baseline throw assertion.
No new fighter tests were added. Native families and roster/controller
configuration outside these consolidated attacks remain transitional.

## Shared engine attack-state entries

The four generic attack states (punch, kick, sweep and uppercut) now bind their
shared native/script entries in the roster runtime. Seventy-two fighter-local
MoveDef seeds and duplicate entry assignments are removed. An unstarted state
has no authored clip, timing or balance; original playback binds its descriptor
before rendering. Other legacy state rows remain migration inputs.

Existing render-snapshot coverage now enters attacks through their runtime
API. Static attack-clip assertions are retired rather than preserving obsolete
C defaults. Existing asset checks read Shinnok commands through the live roster
view and preserve the actual displayed idle clip across freeze, replacing
assumptions about the old private command table and idle sentinel.

Validation: shared VM checks, all strict roster audits, native source gates,
formatting, game build and game self-test (all 18 rigs and11 arenas) pass.
Battle integration stops at the established ground-throw assertion. The broader
asset suite passes the snapshot/rig binding checks but later stops at its
existing air-throw attacker/victim clip expectation; that native throw family
has not yet been consolidated. No new fighter tests were added.

## Native identity is a typed retail address

293 typed native registrations with exactly one implementation no longer
restrict callers by a fighter mask. Original opcode kind plus native address
identifies the routine; actor identity remains runtime data. This is needed
when loaded tables select a native family for another model, including hidden
roster remaps. Registrations with multiple legacy implementations remain
explicitly transitional until their context dependencies are consolidated.
No script behavior or native implementation is generated by this change.

The checked throw selector48e490 demonstrates why model/roster identity is
not native implementation identity: its hidden17 remap is4, whereas script
character selection uses7. A small native-ABI fixture verifies the shared
48f6e0/48f750 momentum service on a caller outside its former registration
mask. Typed roles and argument guards remain enforced.

Validation reuses shared runtime/native lifetime and rejection checks, strict
audits for all slots, formatting and native source gates. No fighter timing
or balance coverage was added.

## Retail-loaded fatality native dispatch

First/second fatality dispatch now reads4e0498/4dfe90 at execution and resolves
the actual native scene address. Thirty fighter-specific dispatcher rows are
replaced by the two shared dispatchers and a native-address registry: the
existing native scene implementations remain registered once at their real
entry addresses. CPU finish entries are populated from the same live tables,
including supported hidden-roster selections; zero native targets stay absent.

Evidence is local467ce0/43cbb0 and467cd0/43bd70, including the scene-specific
hidden remaps16->2 and17->7, matched to mk4decomp's
`PendingMatch_CallPauseScaledStoreCopyJmp`. These remaps are intentionally
distinct from48e490's native action-table remap. Scene bodies and their
existing lifetime contracts are reused.

Existing command/scene regressions now request shared dispatch rather than
private C function pointers. One source-data fixture swaps a scene-table word
and checks native resolution. Shared VM checks, eighteen strict audits,
formatting and native source gates pass. Existing battle finishers/CPU scenes
pass and the run reaches the known ground-throw assertion. No fighter timing
or balance tests were added. Private CPU minimum-range configuration and
other native/controller families remain transitional.

## One shared spike stage native

All eighteen spike modules and eighteen startup thunks are removed. Native
467e70 now acquires the shared stage context, waits the original30 visits and
enters4e6950. Native467fc0 uses the existing shared impalement/recovery service.
The old adapters mixed immediate and delayed starts; the shared entry follows
the checked native delay. Liu Kang's private end adapter is equivalent to the
shared roster victory view and is retired. Three unused private arena dispatch
functions and dead fan/spike branches in the remaining scene setup are removed.

Evidence is local467e70/467fc0, original4e6950 and mk4decomp ArgSarStoreJmp;
no stage playback is translated to C. Existing native choreography remains
implemented once in the shared scene/script services.

Validation reuses shared VM/lifetime checks, all eighteen strict audits,
formatting, native source gates and existing CPU/stage integration. The battle
run passes the stage scenes and reaches the known ground-throw assertion.
No new fighter or timing tests were added.

## No generated fighter runtime includes

The final twelve generated CPU includes and their generator are removed.
There are no generated fighter C/include outputs or generated fighter folders
left. Runtime source checks reject their reintroduction. The shared roster
binds all four finisher slots itself; first/second support and target distances
come from retail tables, and fan/spike use their shared native implementations.
Two unused Kitana fatality entry copies lose their final private CPU callers
and are removed. Other native callbacks remain migration work.

Hand-authored CPU finish minima and their manifest fields are retired.
Local4345d1..43464b reads a single target distance;43460d maps boss15 to14
for approach data. CPU code reads that live row and approaches when outside
it. There is no authored minimum/retreat gate. The native scene body is called
directly after CPU selection, as in retail. Three duplicate human range checks
in native scene entries (Fujin, Reiko, Reptile second scenes) are removed;
original command predicates remain in human input matching. Checked local
43bea0,41c360,43bf40 and mk4decomp's matching scene handlers establish that
boundary.

Obsolete C-table generation tests and static CPU table reachability assertions
are removed. Two small source-registration audit checks remain; the shared data
fixture verifies a mutated distance word. Shared VM checks, eighteen strict
audits, native source gates, formatting and helper/navigation checks pass.
Existing CPU near/far and stage/scene integrations pass before the known throw
assertion. No fighter timing/balance tests were added. Remaining per-fighter
normal-controller aliases, states and native implementations still require
consolidation; this is not yet removal of the entire per-fighter architecture.

## Script-to-C/frame translation retired

The offline C emitter, frame-program lowering backend and translation test
suite are deleted. Only read-only inventory and runtime audits remain behind
the historical `compile_retail.py` command. `make scripts` and
`make scripts-check` audit all 18 slots in one process, sharing the source
reachability index. No generated files are written or compared.

Coverage profiles no longer contain compiler outputs, generated roots, entry
contexts or compiler return targets. Audits require interpreted roots and
check native presence/role in each decoded program; translated/legacy
configuration is rejected. Tiny decoder-boundary classification and native
fixture formatting utilities remain separate from code emission. Native-frame
fixture generation is retained for the engine scheduler we implement.

Obsolete generated-C/CFG reachability tests are retired. Two small audit
fixtures check forbidden translation configuration and missing/wrong native
roles. Shared-runtime coverage, native source/reachability checks, native-frame
fixture canonical checks, helper/navigation checks (6/7) and existing decoder
query tests (33) pass. Inventory fingerprints and 1,109 interpreted roots remain
unchanged. The workflow and compiler-era documentation now describe direct
retail execution. This tooling cleanup does not claim remaining per-fighter
native/controller architecture is finished.

## Shared native scene animation binding

Three private fatal-pose helpers and47 copied clip/length rows are removed.
Native scene setup now shares one actor-state/ownership helper and the same
checked descriptor binding as the interpreter. The other native scene call
sites no longer pass authored clip lengths; the original descriptors supply
them. Pose selection inside an actual native routine remains native work;
script animation commands continue to execute original words.

This reuses the existing checked animation binding boundary (including native
428750's descriptor-load path) rather than adding a second scene decoder.
State, untouchable/busy flags and actor-owned playback storage retain their
existing engine boundaries.

Shared VM checks, all strict roster audits, native source gates and formatting
pass. Existing native scene/CPU/stage integrations pass before the known throw
assertion. No additional tests or fighter timing expectations were added.

## Shared native throw utilities

The native addresses 42be10, 42bf00, 42cb80 and 42cc00 now resolve once,
without fighter masks. Six duplicate ground-throw predicates and the copied
LP branch selectors are removed. LK return reads its speed from 4de358 and
animation descriptor from 53c540, matching the local executable and mk4decomp
native paths. 4de358 is speed data; two obsolete adapters that treated it as a
script are removed. Sixteen unused VM entry adapters are retired; the existing
branch regression enters original addresses through the shared interpreter.

The 42bf00 return preserves the engine's existing 20-visit stance recovery
approximation; visual parity with the original 0xccc stance blend remains a
limitation. Shared VM checks, strict audits for all 1,109 roots, native source
gates and formatting pass. The battle integration builds and reaches its
previously reproduced ground-throw damage assertion. No new character tests
were added. Remaining throw starts and native families still require migration.

## Shared native facing callback

45b380 and physics callback 490bd0 now use one native-facing adaptation.
Five identical private callbacks and their owner-mask branches are removed;
Kai's native callers share the same service. The local listing confirms
45b380's facing service and 490bd0's pair-heading refresh. Our world pump
continues to own the simulation heading, preserving the existing axis-facing
adaptation. Shared runtime, all-roster strict audits, native source gates and
formatting pass; no tests were added.

## Shared crouching low-kick native boundary

Native entry46b710 sets0106 and enters original4e6158. Native46b740 now queries
live B23 once unless the peer is2011, marks1006, waits1/4 visits for miss/contact,
and enters original4e6180 for its authored rewind. Six private startup and tail
callbacks are removed. The entry is available to every retail context; existing
crouch controls use it. Original local listings match the action106 block and
recovery in mk4decomp's five_block_dispatch_variants.c (upstream46b7a0 tail).

Runtime checks, all strict roster audits, source gates and formatting pass.
The existing battle integration builds and reaches the known ground-throw
assertion. No tests were added. Other ground-attack profile rows and controller
tables remain transitional and are not evidence of completed consolidation.

## Retired compiler-only C support

Nine uncalled generated-program APIs are removed: inherited clip checks,
paired cursor stepping, generated blend setup, generated landing/wait wrappers,
and obsolete fan callbacks. Live interpreter opcodes already own these control
flows; native engine blending and other called services remain. Research aliases
now point to the actual VM, common landing or shared native scene implementation.
The header describes native services rather than generated script primitives.
Runtime checks, all strict roster audits, source gates and formatting pass.
No new tests or behavior paths were introduced.

## Shared crouching high-kick entry

Native46b5a0 now sets0104 and enters original selector4e6020 once for every
context. The selector owns each authored launcher variant. Six private startup
callbacks and twelve duplicated crouch kick press wrappers are removed. The
existing VM button-entry regression now expects the selector root rather than
its Scorpion-only destination. Matching local listing and mk4decomp's action104
router establish the entry; no authored timing or contact logic was added to C.
Runtime checks, all strict roster audits, source gates and formatting pass.

## Shared LP capture and original selector

The ground LP entry now uses a single native42c590 capture boundary and enters
original4de798, with unbound shared owner/victim state views. Retail scripts
bind their own animations; seventeen fighter setup functions and their dead
LP branches are removed. Jax's slam counter and generic native phase initialize
at this boundary. Raiden's existing combined air/ground adapter retains its air
branch and calls the common ground path. Existing controller throw gates and
engine pair geometry remain their separate boundaries; this does not implement
original camera-side pair swapping or claim full42c560 parity.

Formatting, runtime checks, all strict roster audits, native source gates and
six existing helper-navigation checks pass. Existing battle integration builds
and reaches the independently reproduced ground-throw assertion, after its
native scene and branch/interrupt coverage. No tests were added.

## Retail-selected native throw-victim entries

A checked4ea4e8 reaction-table reader now selects five missing native victim
entries by original address:485860,4857b0,485810,4854e0 and485480. Their shared
implementation retains inherited animation state, applies native descriptor /
physics setup and executes original victim words. Native4854c0/4854d0 continue
through original4ea3c0/4ea3d0. Native485510 now uses the checked floor/controller
return rather than Raiden's extra approximate landing/finish phase.

The entries match local listings and mk4decomp pending_match_variants.c. One
small runtime fixture edits a loaded reaction target, checks native selection
independently of the actor model, supplies its required inherited descriptor,
and checks continuation retirement. Runtime checks, all strict roster audits,
native source gates and formatting pass. Battle integration reaches the known
ground-throw assertion. This is preparation for replacing the copied LK starts;
the catalogue's existing1,109 roots still omit those correct LK selections.

## Retail-loaded native get-up records

All copied NativeGetupDef profiles are removed. Shared4914c0 recovery now reads
standing/crouching speed and packed descriptor pairs from original4ea210 or
4ea220, and obtains lengths through checked animation binding. Deferred recovery
stores a typed retail record address rather than a pointer to fighter-owned C
configuration. Actor state views are unbound until the descriptor is loaded.

The local488ba0/488c00 callers and4914c0 listing establish the records. Reading
the checked EXE confirms both records exactly match the retired profile values,
including descriptor-derived lengths. Runtime checks, all strict roster audits,
native source gates and formatting pass. No new tests were added.

## Shared native LK victim recovery phases

The same retail reaction resolver now implements4851d0,485530,4856a0 and
4852e0 once. Native descriptors53cf7c/53cf80 supply clip lengths; native waits,
impact, retained-cursor completion, friction and stance blending remain native
work. Recovery uses the original get-up records rather than fighter profiles.
The phases follow the local listings and matching pending_match_variants.c
entries. One extension to the existing native-table fixture checks that a
clip/hold/get-up chain retires playback ownership; it adds no fighter timing or
balance expectations. Runtime checks, strict roster audits, source gates and
formatting pass. These complete the missing nine LK victim entries needed
before selecting the correct LK scripts.

## Retail-selected LK throws for every roster slot

LK startup now reads native table4ddfb8, including native48e490's16->2 and
17->4 remaps. A shared native boundary supplies capture and each original
routine's literal script-enter operand. The twelve copied Scorpion selections
are replaced by actual retail selections. Original words supply animation,
paired progression, authored branches and damage. Native continuation entries,
impact voice list4ea8a0, retained-cursor victim reactions and source get-up
record4ea200 are shared once. The optional42b930 pair-shift task uses our circular
arena/camera heading adaptation and retains its independent native lifetime;
original camera-side swapping remains the existing engine geometry adaptation.

Eighteen private LK startups, all eighteen C throw state seeds, thirty-six
uncalled VM entry adapters and seven empty throw modules are removed. Shared
unbound capture views replace fighter configuration. The remaining per-fighter
modules still contain other native/controller migration inputs.

The research catalogue now verifies its native selections against the EXE table
and indexes the correct roots rather than counting copied Scorpion fragments.
The resulting count is1,101 interpreted roots across18 slots (the earlier1,109
count included those wrong selections). Runtime manifests and fingerprints are
reviewed against these original native/script queries. No script C is generated.

Shared VM checks, all strict roster audits, native source gates, formatting,
six helper-navigation checks and seven port-navigation checks pass. A small
runtime fixture edits the loaded LK target and verifies model-independent
selection plus pair/victim continuation retirement. The obsolete twelve-fighter
Scorpion-recovery matrix and exact-tick ground-throw damage assertions are
retired; existing capture, interruption and KO lifetime checks remain and pass.
Battle integration proceeds to its next exact-tick air-throw damage assertion,
so the broad battle suite is not reported as passing. The SDL self-test passes
all18 rigs and11 arenas. No new fighter timing/damage matrices were added.

## Shared native crouching low punch

Native46b7f0 now runs once, reading the armed/unarmed descriptor pair53ce58 and
A3 contact record from the retail image. Its native opening, sound, forward
completion, contact-dependent wait and reverse completion use the shared native
runner. Native4610d0's sword-type exception stays at the shared press boundary.
Twelve private entry/button wrappers and eighteen uncalled VM adapters are
removed. All24 copied rows for the four already migrated low/crouch families
are retired; native ground profiles now own only the remaining close attacks.
The old native ground runner's unreachable crouch branches are removed too.

The local46b7f0/4940e0/428ce0 sequence matches mk4decomp's phase controller in
five_block_dispatch_variants.c. Runtime checks, all strict roster audits,
native source gates and formatting pass. No tests were added. Close-attack
profiles and their follow-up tables remain the next native migration inputs.

## Retail-loaded inherited close-punch animation

Native46be40 now has one shared entry for every roster slot. It binds the
packed descriptor from4e6c10, applies the native16->2/17->4 aliases and checks
peer action class4000 before entering original inherited-animation script4e61e0.
This matches mk4decomp's close-entry sequence and scaled_chain_and_f000_dirty_toggle.c
(upstream48e740, local48e6e0), verified against the local entry listing.
Twelve private native startups, six ground-profile startup branches and eighteen
uncalled script adapters are removed. Existing close-contact recovery profiles
remain migration inputs; this step does not claim their copied tails are gone.

One small runtime contract mutates the loaded descriptor table and verifies that
the shared entry inherits the replacement independently of the model profile.
Shared VM checks, all eighteen strict script audits, native source gates and
formatting pass. No fighter timing or damage tests were added.

## Shared close-attack controllers and loaded follow-up buttons

Native46bf10,46c2c0 and46bb60 now have one implementation each, using live
A16/B38 records, native clip completion/waits/reversal and the original knee
miss script4e61d0. The knee contact drains attacker run energy via48fae0's
0x6666/40 contract; this is not extra victim damage. Native46ba60 also owns
its peer-reaction opening once. Native46c210 reads4e6c50 with native aliases
and selects original4e69b0,4e69c8 or4e6a28 (clips261,532 or533). The previous
copied profiles incorrectly selected the first combo variant for most slots.

The three follow-up ButtonTable views contain only retail addresses. Input
reads the six original controller pointers on each press;46c210..46c740 are
shared native controllers, including follow-up crouch/uppercut gates and the
existing equipped-weapon native service boundary. The script words retain
all authored startup and miss-recovery logic. Normal stance/air tables and
weapon frontends remain later migration inputs. The existing native idle
blend uses the shared armed stance convention until weapon data migration.

All54 copied close-attack rows,18 NativeGroundProfiles, copied follow-up tables,
private startup/tail adapters,18 unused follow-up press wrappers and36 uncalled
script adapters are removed. GroundAttack,NativeGroundProfile and the obsolete
native ground frame-profile runner are retired. Remaining air/native modules
still exist; this is not completion of the full fighter architecture migration.

Research roots now reflect the actual combo variants and two native-entered
recovery/follow-up scripts;1,137 reviewed roots are interpreted across18 slots.
The audit checks its combo index against loaded4e6c50. Newly exposed reaction
27 retains the preexisting generic native integration, without a parity claim.
The source graph includes the actual fighter_press entry for loaded controllers.

Validation: shared VM/rejection checks, all18 strict audits, native source gates,
format checks,6 helper-navigation checks and7 port-navigation checks pass. Two
small runtime contracts verify loaded-button target mutation and native/script
completion ownership; no character timing/damage matrices were added. The SDL
self-test passes18 rigs/11 arenas. The broad battle suite still stops at the
previously recorded exact-tick air-throw damage assertion.

## Loaded ground controls for every fighter

All eighteen standing/crouching controls now use shared address-only views of
4e5058/4e5070. Grounded air-contact follow-ups use original4e70c0 and native46c3f0,
which can force a close punch against a reacting grounded peer. Normal native
selectors read action table4e6ef8 and crouch table4e7020; targets are resolved
through the shared typed registry. Equipped weapon native frontends remain the
existing service boundary until their descriptor/button migration.

Native46dbb0 and46a7b0 now share their actual common exclusions while retaining
their differences: LP has attack-action exclusions and an unarmed condition;
LK uses strict distance and allows the armed path. Native48ebc0's swapped
context checks whether the victim holds back. Both use exact retail0xb333 range.
Native input controllers are implemented once rather than copied per fighter.
Held block remains owned by the simulation state pump, and engine command
matching handles finish requests before crouching-HP fallback; this step does
not add retail optional automatic-finish/cheat flags.

The54 copied ground/follow-up button tables,126 unused private normal-input
functions,40 empty native input modules, four now-unused shared press wrappers
and the unused approximate mk4_controls fallback are removed. Legacy CPU callers
still retain some native selector wrappers; air input/profiles remain the next
migration family. The audit exposed an old Goro manifest entry pointing at his
jumping table instead of the actual grounded follow-up table; the jumping table
is preserved and the correct native follow-up is now indexed.

Shared VM/rejection checks, eighteen strict audits, native source gates, format
checks, six helper-navigation checks and seven port-navigation checks pass. The
existing runtime fixtures exercise the loaded input path; no tests were added.
SDL self-test passes all18 rigs/11 arenas. The broad battle test's previously
recorded air-throw assertion has not been reclassified as passing.

## Preserve press-time direction modifiers

The first broad integration run after loaded ground controls exposed a real
bridge regression: Back+LK was selected inside the deferred native entry after
the direction could be released. LK and HK controller selection now resolves
the retail action target at the press boundary, then queues the selected native
entry. The existing advancing-sweep integration check passes again and the
battle suite proceeds to its previously recorded air-throw damage assertion.
Compilation and formatting pass. No new fighter timing tests were added.

## Shared air startup without fighter animation seeds

The five normal air native entries now supply their original action/script
operands once in retail_air.c. All rosters enter canonical selectors, including
Liu Kang's4e61f8 ->4e6df0 path. Armed HP/LP use original4e66d0/4e66f0.
Native470980's descriptor-binding phase reads its original parameter rows for
HP/LP, diagonal-HK variants and weapon-air playback. Several of these scripts
contain selectors and native calls without animation commands, so removing the
C seeds required implementing that native binding rather than inventing script
commands. Descriptor validation remains in the shared VM.

Local46e6c0/46ee30/46c7a0/46e380/46df50,4704a0 and470980 listings were checked
alongside mk4decomp's FiveThunkMStackDispatcher/FiveEntryAlarmInstallChain and
matching phase-dispatch assembly. Upstream script addresses differ; local
operands and parameter rows are used. Existing native recovery profiles retain
their contact/wait/landing adaptation until that next family migrates.

Ninety private air-entry wrappers, eighteen startup selectors, ninety fighter
MoveDef seeds, the two weapon-air seeds, the old weapon_air_frames adapter and
124 uncalled script-entry adapters are removed. No authored animation/branch
program is translated into C. Air press tables and native recovery profiles
remain migration inputs.

Shared VM/rejection and strict roster audits, native source gates and formatting
pass. A small native ownership contract covers armed/unarmed descriptor binding
and interruption without authored timing/damage assertions. Six helper and seven
port-navigation checks pass. SDL self-test passes18 rigs/11 arenas. Broad battle
integration exposed the separately committed direction-selection correction;
after that correction it reaches the known air-throw damage assertion again.

## Dispatch borrowed fatalities through the victim's retail table

Native43be30 now reads the victim's4dfe90 target and invokes that native entry
with the current actor. The generic CPU_FINISH_2 view is not a native identity:
calling it recursively reselected the actor's own borrowed-fatality dispatcher.
The original16->2/17->7 aliases and peer8 -> generic first-fatality selection
are retained, as checked against the local native listing and shared selectors.
Existing borrowed-fatality checks pass for both actor slots and both target
classes. The bounded broad suite now advances to an obsolete cloned262/261
close-combo matrix. Build/format checks pass; no tests were added for this fix.
