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

## Shared retail air throws and native reactions

The four original air-throw entry addresses46aa80,46a990,46aba0 and46ae60 now
select one shared capture implementation and their literal original script
operands. Both captured views are unbound until the retail words bind them.
Quan Chi's actual46aba0 input now runs4e5e78 and its slam continuations instead
of the copied Scorpion throw. Research inventories reflect that selection,
bringing the reviewed root count to1,138 across18 slots.

Release/fall/get-up natives are actor-independent services. The duplicate
Reiko/Noob slam and reaction implementations are shared once; the hop consumes
original4e5ec0. Reaction85/89/105/166 native selection reads4ea4e8. Native484b60
now follows its actual descriptor4fb628 and side-getup service4ea200; the old
living-victim clip131 profile had copied the KO path. Flip get-up reads53cf70.
Local listings were checked against mk4decomp's pending-match/native recovery
structure, retaining our independent captured-victim interruption policy.

Private capture/get-up animation seeds, duplicate release/slam helpers, five
empty modules,27 unused VM adapters and the with-moves capture adapter are
removed. Air button tables still include transitional C fallbacks; their
retail-authoritative migration is the next step.

Shared runtime tests cover the four native capture classes and KO ownership
without authored timing/damage matrices. All strict audits, native source gates,
formatting, six helper and seven port-navigation checks pass; SDL self-test
passes18 rigs/11 arenas. The broad suite now passes air capture/interruption
and the repaired borrowed-fatality dispatch. Obsolete Scorpion/Quan/Noob throw,
cloned262/261 combo, C-roster-seed and per-tick kick/Scorpion frame assertions
were retired or reduced to runtime ownership checks. The two unused C trace
fixtures and their C-output paths were removed; JSON research evidence remains.
The broad suite currently stops at a legacy fixed-visit LP throw balance assertion. It is not reported as
passing. The fixture drives LP input; its S_THROW argument is a test-helper
label, not direct state entry.

### Loaded air controls and shared teleport native

All roster stance views now use the same loaded standing, crouching, straight-air
and diagonal-air tables. Air contact and teleport controls read4e6ee0/4e88f0.
Thin4712b0/c0/d0 and46c1d0/e0/f0/46c200 controllers resolve flag8 action
entries directly, without the ground-controller floor operation. CPU fallback
also consumes these tables rather than the retired C callbacks. Removed120
private air callbacks and the empty Goro throw module.

Reiko/Noob47c070/47c0a0/47c2a0/47c360/47c390 natives were identical after
actor-name normalization and now have one implementation in retail_teleport.c.
Original scripts bind their capture views. Shared capture accepts an original
script VA instead of fighter-specific root adapters. Native47ffc0 now calls
shared landing46fd90 (mk4_land); bounded local listings and mk4decomp native
control/landing sources supplied the contracts. Kai handstand still owns the
remaining private special button table; air recovery and weapon profiles remain
migration inputs.

Verified: shared VM/rejection checks,18 strict script audits, native source and
frame checks, formatting. Extended the existing loaded-button fixture to
change an air action target and check dispatch plus airborne ownership; no
fighter timing/damage matrices were added. The broad legacy battle-suite
limitation recorded above remains.

### All button tables come from retail

The handstand controller47c580 installs4e83f8, confirmed against the local EXE
and mk4decomp StageEventComplexFsm/StageEventExitCluster. Its four nonzero
entries resolve once through the typed native bridge. The native handstand
service now lives in shared retail_handstand.c. Removed ButtonTable.press and
the last human/CPU C callback fallback: every installed button view names an
original table. Existing authored scripts remain unchanged.

Verified make format, retail-vm-test including rejection checks,18 strict
script audits, native source/frame checks and format-check. No new tests were
needed for this use of the already-tested loaded-table boundary. The remaining
native handstand implementation still owns its native animation controller;
this change does not claim all native profiles have been migrated.

### Shared air native parameter controllers

Removed all18 air-attack modules,90 copied attack records and their recovery
budgets, NativeAirProfile/AirAttack and54 private callbacks. Native470980 now
reads the original opening descriptor, budget and callback;4704d0 reads its
original query budget, attack-bank selector and hit/ground/miss callbacks.
The remaining straight-wide controller is implemented once. The typed VM
bridge resolves these services independently of native_owner.

Miss/contact recovery enters original4e6270/6280,4e6458/6490/64c8/64e0,
4e6588/65d0/6618/6630,4e66a0/66b8 and4e6758/6770. Their waits, gravity,
reverse clips and return branches execute as words in the shared interpreter.
Landing46e240/46dff0/46c8f0/46c820 uses the common native controller. Original
HP contact permits an air follow-up when the peer is airborne even if the
attacker is low. Evidence: bounded native listings, original rows and pinned
mk4decomp FiveThunkMStackDispatcher/FiveEntryAlarmInstallChain in
five_block_dispatch_variants.c, plus matching ComboSpecialEventCluster.
Fourteen uncalled VM root adapters and the unused Scorpion landing seed were
removed. Liu projectile-only helpers moved beside the projectile service.

Verified shared VM/rejection checks,18 strict script audits, native source/frame
gates and formatting. The existing loaded-row fixture changes a miss callback,
checks entry into the original recovery script, then interrupts the frozen
owner and checks playback cleanup. Broad battle integration passes the shared
air paths and still stops at the previously recorded fixed-visit LP throw balance
assertion. Removed the retired per-fighter air timing/damage matrices; retained
short-descriptor rendering, real input, landing and follow-up ownership checks.
The short-descriptor fixture now installs the actual air stance rather than
using standing controls on a manually positioned actor. SDL self-test passes
all18 rigs and11 arenas. Weapon native recovery remains separate migration
work, as do the remaining roster definitions and special native modules.

### Shared CPU action integration

Removed the five remaining per-fighter CpuMoves configurations, ten trivial
close-attack adapters, five duplicate HK selectors and seven empty modules.
CPU close HP/HK resolve original action slots9/12. Normal fallback uses loaded
stance controls; jab/low kick/forced LP throw remain shared engine action
entries. Weapon draw uses original slot41 only when its target appears in the
loaded input lists, including slots formerly missing C CPU tables and respecting
boss slots without that command. Specials/finishers and finish distances retain
their original loaded tables. No character CPU configuration supplies behavior.

Verified formatting, shared VM/rejection tests,18 strict audits and native
source/frame gates. Existing broad integration exercises CPU and weapon dispatch
before reaching the unchanged fixed-visit LP throw balance assertion. No new tests
or per-fighter CPU matrices were added.

### Loaded roster views; no C fighter definitions

Removed all18 CharacterDef templates, their copied state/entry arrays and
handwritten move lists. roster_def now builds compatibility views from loaded
identity, rig ground, idle/walk, commands and CPU data plus shared engine state
interfaces. Original46b300 selects the loaded53ce30 guard descriptor or its
literal boss descriptor4fc960. The common crouch native binds53ce40 on entry.
Unstarted jump, attack, reaction and victory views are unbound; they do not
invent fighter-specific animation seeds. Native/script takeover owns binding.
Approximate reaction fallback remains an engine integration limitation, rather
than a per-fighter C pose table. Private default throw/crouch/victory adapters
and three unused script-root wrappers were removed.

Victory enters original4e6a40 for every slot. Its selector supplies authored
clips and loops, including the actual boss victory instead of a C pose. The
existing native finisher-completion monitor is retained where needed; raw4712e0
entry itself is shared. The move-list UI formats original direction/button
records and uses shared labels (SPECIAL/WEAPON/finishers), since these command
lists contain no display names. C no longer maintains a second command list.

Goro's victory exposed missing opcode73 (native45c070): choose sampled token64a
or66d using bit0 of the original random word, preserving script execution.
Implemented once using SOUND_TOKEN. Evidence matches mk4decomp
HitReactionCluster's matching assembly in five_block_dispatch_variants.c.
Original45abf0 masks its context operand to one byte; replaced the arbitrary
previous81 limit with that actual boundary. The portable camera still follows
live scene actors. Source-navigation mappings and graph fixtures now root at
the shared roster/native registry, not removed CharacterDef registrations.

Verified make format/format-check, shared VM and rejection checks,18 strict
script audits, native/source graph gates, six helper and seven port checks.
SDL self-test passes18 rigs/11 arenas. Existing broad integration passes the
loaded boss victory/context/sound path, CPU stage/finish dispatch and all air
checks, then reaches the unchanged fixed-visit LP throw balance assertion.
KO lifecycle checks now allow the original victory program after attack cleanup.
An earlier ledger diagnosis calling that last throw assertion a direct state
entry was wrong: do_attack(S_THROW) drives LP input; its remaining failure is
an authored timing/damage assertion, not proof about an unstarted state view.
Weapon tables/profiles,446 remaining script-root adapters and unique/duplicate
native modules are still migration inputs; this is not native gameplay parity.

### Retire direct script-root thunks

Replaced387 direct calls with their exact retail_vm_enter_owned address,
completion callback and integration context. Removed377 uncalled root thunks
and dead aliases. Authored script behavior is unchanged; native literal script
operands now reach the interpreter directly. The remaining72 names include
69 true deferred/selected script callbacks and three mislabeled native callback
tables, not generated script bodies.

Source navigation now follows native frontends or the shared interpreter for
script-only boundaries. Removed the dead generated-path index and the rule
that invented a C root thunk for every runtime script. Updated existing
navigation assertions to require the actual VM call rather than a wrapper.

Verified make format/format-check, shared VM/rejection tests,18 strict script
audits, native/source graph gates, six helper and seven navigation checks. No
new fighter tests or broad timing-matrix runs were needed for this equivalent
adapter removal. Deferred callback migration is the next native lifetime step.

### Native completion owns a script address

Added RetailScriptContinuation as actor-owned native phase data. A native phase
can install its original script VA, completion callback and typed integration
context with native_script_end. Completion consumes that record once and enters
the shared VM; replacing/canceling the phase clears it. Moved23 deferred native
completion callbacks onto this boundary and deleted their C root thunks.

This retains the original native hold/blend/frame controllers and script words;
no script behavior or callback sequence is translated. One focused shared
runtime fixture covers completion context plus interruption without resume.
Verified formatting, VM/rejection checks,18 strict audits, native/source gates
and seven navigation checks. No per-fighter timing/damage coverage was added.

### No C script-root adapters remain

Removed the remaining46 selected/immediate script thunks. Native selectors
carry RetailScriptContinuation values or raw original VAs instead of C function
pointers. The shared axe victim service consumes that data directly. Scene
actor allocation now has a shared raw-script entry path; the actor keeps the
source model while entering its own native integration context.

The original4de428 return followed by4de468 is represented as VM-owned return
data, separate from native frame completion. That data follows compatible VM
handoffs and is consumed once or canceled. Existing native return semantics
are preserved; no authored branch/wait/animation is translated.

Three remaining names falsely labeled4de664 a script. It is a native490350
callback word; all callers now load that word and use one facing-flip service.
Verified local490350 against mk4decomp scaled_xor_store.c (upstream4903b0),
which toggles the group state-mask facing bit. No script entry adapters or
associated declarations/macros remain anywhere in game C/headers.

Verified formatting, shared VM/rejection checks,18 strict audits, native/source
gates and seven navigation checks. Extended the existing continuation fixture
for VM/native ownership separation, raw scene-actor context and reset cleanup;
no fighter timing/damage matrices were added. Character.c files now contain
only residual native data or are empty; their source-index/data migration and
removal are next, along with consolidation of the remaining native services.

### Original default weapon inventory loader

Added one typed loader for all eight words of the original53d5c0 inventory
record, including object/hand bindings, button table and stance descriptors.
Native493080 indexes this table with the raw roster id: no script/native action
alias applies. Local493080 and mk4decomp ScaledLoadCmp0fJmp/StrikeAnim4Picker
confirm that choice and the boss equip exclusion.

Removed ten copied inventory arrays and assertions comparing those copies to
other C constants. The resting-object native reads its inventory type from the
image. Inventory auditing now names the loader rather than requiring a duplicate
C table. Corrected the research inventory's hidden-slot alias error: slot16
is4ecfd8/type6d and slot17 is4ed018/type6a, rather than inherited62/69. Reviewed
and updated only those two inventory fingerprints against the original words.
The remaining WeaponDef/native profiles still require migration to this data;
this loader step alone does not claim hidden-weapon gameplay parity.

Verified formatting, shared VM/rejections,18 strict audits, native/source gates
and seven navigation checks. Extended the existing data-loading fixture by
changing a raw inventory slot independently of script/CPU aliases; no authored
fighter behavior matrices were added. Alternative kombat-code inventory tables
are not selected by the current engine settings.

### Remove fighter definition translation units

Removed all 18 `character.c` units after retail roster loading replaced their
definitions. Surviving native descriptor data now lives with its native caller;
unused descriptors and declarations were removed. Research navigation resolves
legacy scopes to actual native source files, and reviewed profile references
point at their current implementations. The native frame gate now includes shared retail
native sources rather than requiring character translation units. Per-fighter native
modules and headers remain migration work.

Validated formatting, shared VM/rejection checks, all 18 strict script audits,
native frame checks, and existing navigation and port audit tests.

### Shared getup native entries

488ba0 and 488c00 now dispatch once for every caller, passing the original
4ea210/4ea220 records to the existing 4914c0 adaptation. Removed six copied
fighter wrappers and their private declarations. 488b80 also performs its
490350 facing toggle before the shared recovery, as the local executable does;
the previous Reptile wrapper omitted it. Compared the local bounded listings
with mk4decomp's Cascade5StageInit (upstream491520, local4914c0); upstream
addresses differ. Renderer pause gates stay outside combat simulation.

Verified formatting, shared VM/rejections, all strict script audits and native
frame gates. Reused existing runtime getup/lifetime checks without new authored
fighter timing fixtures.

### Shared throw recovery natives

Removed four more copied getup bodies. Local485ad0 flips then enters488ba0;
485b10 flips then enters488c00;485b50 flips twice through488bd0 and enters
488c00. The old Sonya/Kitana body incorrectly used a backwards standing clip,
and Reiko/Noob bypassed recovery entirely. These addresses now use the same
shared data-driven recovery regardless of native-owner context. Evidence comes
from bounded local listings and mk4decomp SfxBlockCluster's matching assembly.

Verified formatting, shared VM/rejections, strict script audits and native frame
gates. Added one shared native-boundary fixture for facing parity, crouch-row
selection, owner independence and interruption; no character timing assertions.

### Load native weapon strike parameters

Removed ten copied WeaponStrike arrays (28 rows). The shared494990/494a00
adaptation now reads the original animation descriptor and count, packed phase
bytes, start cursor, sound, weapon contact selector and optional4947d0 motion
row into actor-owned native state. Source data no longer comes from the old
C copies, including Shinnok's omitted start cursor in shared4ec750. Native
frontends still supply their original row addresses and action classes.

Checked bounded local494990/494a00/48d430 listings against mk4decomp
MoveDispatch4StateFsm and its matching assembly. Verified formatting, shared
VM/rejection checks, all strict audits and native frame gates. One shared loader
fixture mutates phase, contact selector and motion data and checks actor-owned
lifetime. Three remaining strike arrays and fighter equipment/routing adapters
still need migration; this does not claim complete native recovery parity.

### Remove remaining native strike arrays

All WeaponStrike C arrays are removed. Axe and blade native controllers now
consume original parameter rows through the shared loader. Native strike
identity uses its retail parameter address rather than a pointer into a C
configuration table. LP repeat recovery retains actor-owned data across native
playback; the back-LP script reads its motion and contact data directly rather
than using a synthetic fourth strike record. Authored back-LP control flow
continues to execute original4ec7f0 words.

Verified formatting, shared VM/rejections, strict audits and native frame gates.
The existing Liu Kang suite builds but stops at its old current_mmove-only
normal-playback assertion before weapons: direct VM ownership is not accepted by
that legacy generated-program assertion. It is not reported as a passing weapon
integration check. No new fighter matrices or execution harnesses were added.

### Shared inventory-driven weapon lifecycle and input

Removed all 17 authored WeaponDef configurations and fighter draw/equip
adapters. WeaponDef is now an address/model view without C callbacks; one
shared lifecycle reads the raw roster inventory, original button targets and
4ecb40 followup table. Shared494990 dispatch follows the original parameter's
back alternative. Native frontend operands are indexed by original VA, with
action writes checked against bounded listings rather than inherited C tags.
Complex native services remain single address exports. Three emptied weapon
translation units were deleted; remaining weapon modules contain native
services and are still pending relocation/consolidation.

Pickup, catch and theft retain the actual world object's inventory instead of
re-equipping the recipient's own weapon. Armed stance/walk descriptors load from
that inventory. Kitana now uses raw type6d with its original object template;
Noob uses raw type6a and its separately loaded original rig, rather than the
old Sonya/axe aliases. Reference checks include mk4decomp StrikeAnim4Picker,
MoveDispatch4StateFsm, Event112Cluster and local46d1d0/46d260/493080/494610
listings. Default inventory settings remain the supported engine mode.

Verified formatting, shared VM/rejections, all18 strict audits, native frame
gates, six helper-navigation checks, seven port/source checks, and the SDL
asset self-test (all18 combat rigs and11 arenas). Extended the existing shared
loader fixture by mutating raw inventory, button target and back alternative;
no fighter timing or damage matrix was added. Catalogued root coverage remains
a research index, not a claim of exhaustive native gameplay parity.

### Own delayed native contact records

Fixed a lifetime error in the migrated back-LP tail: a local decoded contact
record was retained by delayed native playback. Both native_query_rows and the
weapon contact runner now copy their input into actor-owned storage before
yielding. This applies to every caller rather than extending record lifetimes
in individual native services.

Verified formatting, shared VM/rejection checks and native frame gates. One
shared ABI fixture changes caller-local data after handoff and checks that both
yielding entry points retain their own copy, then interrupts playback.

### Shared weapon contact tails

4958b0/495ae0/496770/496970 now dispatch once with loaded weapon contact
records, independent of the actor's native-owner mask. Removed nine fighter
wrappers, their copied contact records and the unused alternate query runner.
Local4968e0 now enters the original4ec7c0 or4ec7d8 contact branch instead of
finishing through the old handwritten approximation. Query callbacks also stop
when contact interrupts or replaces the current action.

Checked bounded local native listings (selectors33/10/11 and query counts)
and the original contact scripts; mk4decomp Event112Cluster/source references
remain the native research base. Verified formatting, shared VM/rejections,
all strict audits, native frame gates and existing helper/port navigation.
Extended the shared lifetime fixture to mutate the loaded contact bank and
invoke the same native under a different owner context, then cancel playback.

### Remove duplicate hidden-fighter native implementations

Removed59 duplicate native bodies and12 emptied C units from the hidden-fighter
folders. Complete function comparisons found identical native operations after
removing diagnostic text and the old explicit script/native context aliases
(16->2,17->7); raw roster/model/inventory selection remains independent.
Consumers now call the existing native implementation. Twelve duplicated owner
dispatch cases collapsed to direct calls, and repeated input registrations
merged. The remaining shadow-task adaptations are separate research work.

Historical names survive only in a Python research navigation alias index;
unique native definitions resolve to their actual source file. This is not game
configuration or a character callback layer. Arbitrary engine helpers remain
outside character source-excerpt expansion. Reviewed source references point to
canonical native definitions.

Verified formatting, shared VM/rejection checks, all strict audits, native frame
gates, six helper checks, seven port/source checks and six reachability checks.
No new character behavior tests were added.

### Use the original shared projectile task for hidden fighters

Local498b70/498e70 cast original4ed268/4ed2c0. Their498bb0/498ed0 native
spawns call49c540 and498be0, which always select the same4d1718 mesh template
and4ed2b0/4ed2e8 waiter data.49c810 queries the C contact bank. These native
paths contain no roster-specific cloned-actor branch. Removed the unsupported
shadow-actor event/renderer subsystem and the remaining Kitana/Noob C units and
private headers. One shared retail_fireball service now implements these native
addresses for all actors.

Contact selectors, origin offsets and the falling descriptor now read original
data. Cast timing stays in original script words; native480cf0 recovery binds
4fbc04 and its stored length. Research names resolve through the shared native
index rather than requiring deleted fighter headers. Source evidence includes
mk4decomp MstackPushPackChainInit, PendingMatch_MStackCall and
TwoPhaseInstallScaledPackedPtr, checked against bounded local listings.

Verified formatting, shared VM/rejections, strict script audits, native frame
gates, helper/port/reachability checks and SDL self-test for18 rigs/11 arenas.
One shared ABI fixture checks typed task rejection, owner-independent spawning
and reset cleanup; no fighter timing/damage matrices were added. The existing
projectile burst/trail presentation omissions remain native integration work.

### Shared throw and crouch controller returns

Removed19 copied endpoint bodies. Native42cbc0/42ce60/42ce80 now use one
KO-aware engine controller boundary that releases owned pair/motion state;
45fc40 reuses the existing shared crouch-return service. Native42cc40 runs the
peer callback cell4de664 before entering original4de668, including the facing
callback omitted by the old Raiden variant. Actor-based return dispatch is
removed at these addresses. Engine pair-space coordinates remain the simulation
boundary rather than emulating retail render-coordinate rebasing.

Checked local return listings,48fe80 and42d070 against mk4decomp
GameSectionSwitcher/controller references. Verified formatting, shared
VM/rejections, strict audits, native frame gates and helper/port navigation.
One shared fixture checks context-independent pair/motion cleanup and prevents
KO revival; no authored throw timing/damage matrices were added.

### Decode the shared native roster sound lists

Native489ff0 now reads the original roster-remap byte, selector pointer and
direct half-word token/per-roster random-list formats. This replaces the old
two-selector audio alias and supplies selector6 required by native485c60.
The raw16->10 sound remap follows the native code independently of script and
weapon aliases. The reviewed selector pointer block is53d000..53d054.

Native48a220's throw grunt moved out of the Jax module and reads original
word-sized token list4ea828 through the same list helper as impact voices.
Compared bounded local489ff0/48a220 listings with mk4decomp
ByteWordTableTaggedDispatch (upstream48a050) and the relocated token wrapper.
Upstream comments contain a reversed branch description; the actual matching
assembly and local EXE establish the remap.

Verified formatting, shared VM/rejections, strict audits and native frame gates.
One shared data-format fixture mutates the remap and checks direct tokens,
per-roster half-word lists and word-sized native lists. No character audio or
move matrices were added.

### Typed native animation-table completions

Implemented the shared4287a0 boundary: load the packed animation descriptor and
unpacked native completion address from the original table, then retain the
completion in actor-owned state. Native and script completion addresses have
separate types and replace/cancel each other. This is required by the LP victim
tables; treating their second word as a script root was incorrect.

Compared the bounded local4287a0 listing with the mk4decomp animation runner.
Verified formatting, shared VM/rejection checks and native frame gates. A small
runtime fixture checks table mutation after capture, completion, cancellation
and script/native type rejection. Production LP callers migrate next.

### Load the shared LP victim entries

Reaction-table targets485b70/485ba0/485bf0 now use one485c60 prelude and
original4287a0 tables4e9bb8/4e9bc0/4e9bc8. Their packed descriptors and native
callback485c40 come from retail data. Direct-entry variants485b30/485af0/
485a90/485a30 enter original4e9b88/4e9b68/4e9b40/4e9b18 respectively.
Removed copied LP victim frame programs and the redundant Jax slam victim.
Known loaded native reactions take precedence over the old Goro approximation.

The old reaction102 root4e9bd0 was a false hypothesis: native485b70 calls an
animation/native-callback table, not that script. Removed it from the five
inherited research inventories and reviewed their fingerprints. Compared
bounded local entries and4287a0 against mk4decomp SfxBlockCluster matching
assembly. Engine pair/world coordinates replace485d00 render rebasing;
485bf0's Goro-specific43a5c0 object-task presentation remains unimplemented.

Verified make format, shared VM/rejections, strict audits and native frame
gates. One integration fixture mutates the reaction target and animation-table
words to verify loaded dispatch independently of roster/selector. No authored
LP timing or damage matrices were added. Existing broad battle/timing-suite
limitations recorded above remain; these checks do not establish native parity.

### Route known reaction natives independently of script owner

The45b1f0 bridge now checks the loaded shared throw-native registry before
legacy owner dispatch, matching480f80's reaction-table selection and48f880/
48fd00 peer switch. Removed five forwarding wrappers and their owner branches.
The existing mutated-table fixture now exercises this handler from an unrelated
script context and verifies pair release, rather than calling battle_react
alone. Both native reaction handlers were checked against bounded local
listings; their context labels do not make the executing Fighter the victim.

Resolved the previous485bf0 Goro presentation limitation:43a5c0/43a610 visits
resting object descriptors of type6c. Its existing weapon-hop adaptation is
now one shared service used by LP, stomp and quake callers, without a roster
inventory wrapper. Verified formatting, shared VM/rejections, strict audits,
native frame gates and helper/port navigation (6/7 tests). Other native scene
and reaction services remain migration work; no authored matrices added.

### Remove character-private C interfaces

Removed all16 remaining character-private headers. Expanded their old object
aliases in native source, retained only used native constants/macros locally,
and declared actual exported native services through one shared interface.
The remaining source files contain native implementations, not fighter
registration/configuration. Historical spelling exceptions remain solely in
Python research navigation; they do not select game behavior.

Updated architecture/source-layout documentation to remove obsolete generated
C/frame instructions. Native file consolidation remains the next structural
step; this interface change preserves existing native behavior and its recorded
limitations. Verified make format, shared VM/rejections, strict audits, native
frame gates and helper/port navigation (6/7 tests). All native units compile
through the shared C11 interface. No tests added for this interface cleanup.

### Consolidate native C into shared capability modules

Removed every fighter C source path:125 native units now compile as eight
shared modules undergame/native/ (attacks, motion, projectiles, throws, weapon
attacks, contexts, severing and cinematics). Their local symbols remain static
and have distinct native-cluster namespaces. This is native C consolidation,
not script translation. Removed mk4_characters.h and the VM bridge's duplicate
native declarations in favor of the single native_services.h interface.

The build no longer enumerates fighter C folders. The architecture gate rejects
character C/header/moves paths and checks all shared native modules. Reachability
now covers those modules globally rather than silently skipping relocated
implementations. Normalized reviewed source links; historical path/local-name
relocations live only in Python navigation. Cached source text/masks by file
modification time to avoid repeatedly parsing consolidated units during audits.

Verified make format, shared VM/rejections, strict all18 inventories, native
frame gates, helper/port navigation (6/7 tests), and the SDL game build. The
linker reports the existing SDL library/newer macOS deployment-version warning.
No new gameplay tests. Old actor-based native bridge specializations and copied
reaction data still need consolidation against actual native semantics; moving
native files does not establish their parity. Recorded broad timing-suite
limitations remain and were not rerun for this layout/interface change.

### Remove copied LK links and victim release variants

Native42bdf0/42be00 only enter4de318/4de328 in the actual EXE. Removed their
owner-specialized C bodies (including the invented Scorpion peer rebase).
Reaction77 now selects native484f10 through the loaded table, binds original
descriptor4fc8f4 at65 and uses its loaded length. Removed the copied131-pose
range, separate Scorpion victim program and12 opponent-reaction wrappers.
Both reaction handlers can use the shared release; legacy stage175 remains an
explicit coordinator-owned boundary. Other non-shared reaction entries remain.

Compared bounded42bdf0/42be00/484f10 listings and matching mk4decomp
PendingMatch variants. Engine continuation ownership replaces484f10's internal
task84 resume flag; completion still uses the shared KO-aware return.
Verified formatting, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). The existing loaded-table fixture also
mutates the release descriptor length through45b230; no authored timing matrix.

### Execute stagger reaction scripts instead of copied C rows

Added the shared loaded-native reaction entry boundary and migrated native
482e00/482830/482870/4828d0/482be0/482e80/482e90/482b10. Their C implementations
only perform original native recoil/disarm, incoming-action selection,
scale and sound preludes, then enter original4e9308/4e9360/4e90e0/4e9130/
4e9150/4e9170/4e9260/4e9388/4e9238 words. Removed eight ReactionSpec rows and
unused copied crouch/slow-playback branches. Native sound lists reuse the
existing shared decoder; no per-fighter handler registrations were added.

Checked each bounded native/helper/stream against the local EXE and matching
mk4decomp StageGameProgressCluster/CrouchFsmCluster assembly. In particular,
482aa0 supplies impact sounds, not the no-guard flag; scripts own that flag.
The broader ReactionSpec/tumble paths still contain legacy copies to migrate.
Catalogue root counts do not enumerate every newly entered native-frontier stream.

Verified make format, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). One small native-boundary fixture swaps
loaded reaction selection, checks the incoming-action branch, completion and
interruption. No authored frame timing, damage or per-fighter matrices added.

### Load launch streams and share their native completions

Native481320/482560/4825c0/4835e0 now enter original4e89a0/4e8ea8/4e8f10/
4e96c8, with only checked native disarm/recoil preludes. Removed their legacy
reaction-table playback entries and three copied spec rows. The old row10
remains solely because unmigrated airborne90/173 adapters still borrow it;
its native entry no longer uses that copy.

Native482790 is one shared inherited-descriptor completion into loaded back
getup4ea210. Removed four duplicate wrappers and all seven owner branches.
This corrects the old Raiden/Sonya-specific flip/roll variants at that address;
their distinct staff/488c10 services remain. Native4835f0 enters original
4e96e0 and483610 finishes the inherited descriptor before controller return.
Checked bounded entries,428810 and streams against mk4decomp
StageGameProgressCluster/BattleEndCluster matching assembly. Engine frame
continuations replace internal task84 resume bookkeeping.

Verified formatting, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). Existing runtime/native/lifetime
fixtures cover the integration; no new timing or per-fighter tests added.

### Replace the copied tumble loop and ordinary controller variants

Native483450/483490/482750/482730/4826a0 now perform their checked native
prelude and execute original4e9508/4e9568/4e9048/4e8fe8/4e8f80. Removed the
hand-authored tumble phase loop, its MoveDefs and legacy dispatch entries.
Native4826e0's sound selection follows the peer's weapon descriptor/type,
with the loaded inventory as the unequipped fallback. Original words own
motion, landing boundaries and waits. Corrected reaction100's stale review
reference to the already-shared loaded LP native.

Native470420 uses one controller boundary instead of five owner branches.
Removed six obsolete return/getup wrappers and the now-unused Sub-Zero LP
frame program. Checked470420/48e080 with mk4decomp SlotPhaseResetInstallChain:
callback release must preserve airborne velocity/gravity. The shared return
releases pair/native callbacks, faces the peer and preserves KO ownership;
ordinary engine neutral control handles grounded reset. This differs from
native throw helpers that explicitly stop and floor the body.

Verified formatting, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). Extended the existing controller
contract fixture for airborne preservation and KO; no authored timing matrix.
Checked bounded tumble/helpers/streams against StageGameProgressCluster,
matching assembly and the supplied EXE. Other legacy reaction copies remain.

### Retire native tasks through caller-owned scene completions

Native49cb60 now has one implementation. Its portable scene-completion callback
comes from the script caller, not six roster/context branches. Nine terminal
streams (including the repeated hand stream's entry) now retain their explicit
scene joins through the existing typed VM completion field. Renamed those joins
by native scene identity and moved common playback retirement to the shared
boundary. A live controller callback is rejected as an invalid task completion.

Native41f1b0/466df0 now share final-pose pause handling; removed five copied
pause/park bodies. Removed identical owner-gated projectile registrations at
496e00/49b9e0/49ba50 while retaining their typed task arguments. Checked bounded
41f1b0/466df0/49cb60 entries, original task_finish_and_pause/task_mark_finished
and matching upstream task-lifetime structure. Scene joins remain engine
adaptations, not retail scheduler emulation.

Verified formatting, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). One runtime fixture checks native task
retirement consumes completion/cleanup once and interruption suppresses the
completion, in an unrelated script context. No scene timing/damage matrices.

### Cover cursor operations and dispatcher-alias opcode slots

Added opcode0a's low-byte cursor increment and13/14's final-sample cursor/
submission operations using the loaded descriptor. The engine owns sampled
poses directly rather than emitting a separate retail render submission.
Slots0b/15/18/21/23/57/66/67 are genuine no-ops: their loaded handler pointers
return directly to dispatcher4594b0. Decoder operand counts, native-boundary
classification and read-only C-style views now recognize these operations.
Slot31 is null in the original table and remains invalid, not a no-op.

Checked local45a490/45aa80/45aab0/429970 and the dispatch table against
mk4decomp ComboMoveSelectFsmCluster/matching animation assembly. One runtime
fixture checks no operand consumption, low-byte masking and a mutated loaded
descriptor length. Verified formatting, shared VM/rejections, strict audits,
native frame gates, existing decoder checks and helper/port navigation (6/7
tests). Fourteen non-null opcode handlers still require completion; this is
not a claim of full interpreter/native parity. No character tests added.

### Complete conditional and small engine opcode boundaries

Implemented3d's numeric block-result branch,43's predicate bit-zero branch,
50's loaded B-contact/conditional script jump,54's packed conditional wait,
5d's signed heading/Y position adjustment and74's native token627. Reused the
shared contact, pose, heading and sound boundaries. ContactResult values1/2/3
match the original native48d4a0 output, independently of predicate flag bits.
Native predicates now preserve opcode55's masked inline argument and stop the
old stream after an ownership transfer; the previous bridge discarded it.

Checked bounded45b500/45b5f0/45ac80/45acf0/45b0d0/45c100/45ad90 and their
helpers, including490b10 and48d4a0, against the supplied EXE and mk4decomp
MoveListCursorCluster/VM matching structure. The decoder follows both new
branch edges and exposes exact operand counts; C-style views remain read-only.

Verified formatting, shared VM/rejections, strict audits, native frame gates,
existing decoder checks and helper/port navigation (6/7 tests). One combined
runtime fixture checks numeric contact versus flag branches, inline native
arguments, signed operands, token dispatch and packed wait selection. It uses
a locked synthetic runtime action so the controller cannot interrupt the wait.
No authored character tests. Eight non-null opcode handlers remain to complete.

### Add directional motion and arena opcode services

Implemented65's three-word directional motion,6f's outer-edge branch,70's
world-distance-gated peer reaction and72's independent edge-push task.
Directional490460/490580/490c00 behavior uses the captured peer's facing and
relative order, adapted to the engine's pair/world coordinates. Distance70
reads the existing world distance field and preserves unrelated result bits.

Checked local handlers and native48f100/48f240/42b8c0/42b930/4ab290. The latter
is a Q16 square root, not a random helper: edge push is deterministic when the
farther body's radius exceeds arena radius minus two. The shared native task
applies the original radial push and ten decay visits through the existing
world-event frame player, then stops both motion contexts. No fighter script
or per-roster callback owns that event. mk4decomp's
bit_dispatch_dual_call_m_stack_push.c matches490460's bit-driven cascade;
local addresses establish the VM wrapper and geometry operands.

Verified formatting, shared VM/rejections, strict audits, native frame gates,
existing decoder checks and helper/port navigation (6/7 tests). Small runtime
fixtures check signed operands/directional branches, typed distance dispatch,
result-bit preservation and independent event cleanup/reset. No authored move
or balance matrices. Four non-null animation-controller slots remain.

### Cover the remaining animation-controller dispatch slots

Added17's double-source-pose playback and1a/1c's actor-owned interleaved
full/half-weight playback with the original200/inline source budget. The
shared VM owns the cursor, target, blend and cancellation; no frame rows are
generated. Slot1b returns its fetched word and masked count to the typed native
caller as data. It does not bind or execute that word. Decoder control flow
recognizes the counted return as terminal.

Checked45a3e0/4283a0/4285b0,45a070/45a0c0/45a0f0/45a140/45a260,
428550/4285d0 and407d00/407d50 against mk4decomp
ComboMoveSelectFsmCluster's dual-entry machine, matching assembly and the
separate animation cursor/completion model. The engine represents sampled
poses and blends directly; exact retail tagged-scheduler trace identity is
not the integration contract.

Verified make format, shared VM/rejections, strict audits, native frame gates,
existing decoder checks, helper/port navigation (6/7 tests), SDL game build
and resource self-test (all18 rigs and11 arenas). Existing SDL deployment-
version linker warning remains. A compact runtime fixture checks loaded clip
bounds, inline budgets, counted data return and blend cleanup on interruption.

A direct comparison with the supplied dispatch table now finds runtime and
decoder cases for all118 non-null slots; null31 remains invalid. Slot coverage
does not establish native gameplay parity. Legacy reaction copies and native
owner specializations remain the next migration work, not completed parity.

### Share camera selector and avoidance native boundaries

Removed six camera-event adapters and their owner masks at45ac30. Local
45ac30 only masks the selector and calls4315b0, which queues camera director
4dea40 with the actor context. The existing engine camera service handles all
selectors; battle_begin_finisher already owns both actors' input and
invulnerability flags. Camera requests no longer duplicate those combat writes
or enforce invented per-fighter selector ranges. The portable follow-camera
policy remains; retail camera-shot reproduction is not added by this cleanup.

Native46a950/46a970 are plain task5c clear/set operations. Removed two identical
Jarek wrappers and owner gates, using the existing shared avoidance/lifetime
helpers. Checked bounded local entries and mk4decomp matching clear/set
assembly. Historical names remain research aliases only.

Verified formatting, shared VM/rejections, strict audits, native frame gates
and helper/port navigation (6/7 tests). Updated the existing navigation check
to assert canonical alias names instead of requiring removed legacy spellings
inside current C bodies. No new tests for identical helper bodies.

### Share body scene completion

Removed duplicate Fujin/Reptile4817c0 native bodies and their context-owner
switch. The shared scene boundary lowers the body by0x147a once and uses the
existing60-visit continuation before completing the finisher. Checked the
bounded local native phases; retained the engine's existing audio/orchestration
adaptation rather than inventing fighter-specific audio selectors.

Verified formatting, shared runtime, all strict fighter audits, native gates
and existing helper/port navigation. No new tests for identical native bodies.
This removes another owner specialization; remaining copied reaction playback
and other owner branches still require migration.

### Share recoil landing continuation

Removed Cage, Jarek and Tanya copies of47dae0 and its three owner gates.
The bounded local entry and mk4decomp MoveSelectorCluster agree: it runs
46fd90 landing, then46f650 controller return. Existing mk4_land owns the
portable landing/cancel controller. Direct native callers use the same service.
Verified formatting, runtime, all strict audits, native gates and existing
helper/port navigation. No additional tests for identical adapter bodies.

### One independent hover native

Replaced the Kai projectile-based driver and Reptile event driver with one
46a280 scene task. Checked local46a280/46a340/46a380 and mk4decomp
TripleEntryCountdownInstall matching assembly. Gravity starts at-0x28f,
eases through ten+0x41 updates, then alternates+/-0x83 using native vertical
velocity predicates. Ten-visit holds now precede reversal; the old Reptile
copy reversed before its delay. The shared task uses engine event ownership,
independent of fighter script replacement, and retires with the scene/reset.

Verified formatting, shared runtime, strict audits, native gates and helper/
port navigation. One compact native/lifetime fixture checks typed task rejection,
context-independent dispatch, interruption and scene/reset retirement; no
fighter timing or balance assertions were added.

### Shared inherited stagger completion

Removed three different483df0 adapters (star, acid and burn) and their owner
switch. Local483df0/483ed0/483fa0 runs20 then ten4-sample native blocks,
resets neutral context and completes the inherited descriptor. Checked429490
and4297a0: playback saturates at the loaded descriptor end. The common routine
uses those bounds rather than copied clip182/count65 or a fighter-specific
friction write. Existing CPU policy owns input/action response between native
blocks; exact tagged scheduler interleaving remains outside the engine contract.

Verified formatting, runtime/rejections, all strict audits, native gates and
helper/port navigation. Extended the existing inherited-descriptor fixture with
a shortened loaded descriptor and an unrelated actor context to catch fixed
fighter clip bounds and completion errors, without authored timing assertions.

### Keep victory native independent of scene orchestration

Local4712e0 only enters4e6a40 (mk4decomp ArgSarStoreJmp/script entry).
Removed the last roster branch at that native and its Sub-Zero adapter.
The portable50-visit result wait is now an independent scene-owned join,
installed by the existing scene caller. It recognizes the original victory
root rather than a hard-coded model clip118 and does not replace body physics.
This preserves the engine scene completion policy; it is not authored script
logic or an emulation of the original scene director.

Verified formatting, shared runtime, strict audits, native gates, helper/port
navigation, SDL build and resource self-test (all18 rigs and11 arenas).
The existing SDL deployment-version linker warning remains.

### Remove five copied reaction programs

Loaded native targets482260/482410/482420/482580/482770 now enter their
original4e8c78/4e8da8/4e8dd0/4e8ee0/4e90b0 words after only native preludes.
Removed ReactionSpec48/50/51/60/99 and the separate Goro189 punch adapter;
51 and189 resolve the same original482420. No fighter/model gate remains
for these reactions. Native482580 loads its physics callback from4e8edc
(currently4913a0) instead of copying friction behavior into the reaction row.

Checked bounded local entries, mk4decomp EightEntryAlarmDispatch and
StageGameProgressCluster matching assembly, and shared script-entry contracts.
Updated reviewed research metadata to the actual shared native source.
Verified formatting, shared runtime/rejections, all18 strict fighter audits,
native gates and existing helper/port navigation. Reused existing runtime
and callback fixtures; no new per-reaction tests.

### Share scaled native reaction handoffs

Removed copied ReactionSpec52/59/122/155 and the separate Reptile158 and
Goro192 adapters. Loaded482480/4824a0/4823c0/483500/482f60 now execute
original4e8e50/4e8d70/4e95f0/4e93d8 after their native preludes. Both122
and158 use the same483500 implementation. Native482480 adds shake6 before
the shared4824a0 path; native run drain and damage scaling remain native
engine effects. Native48a320 now reads original sound list4ea8f8 rather
than omitting it. Shared4826e0 weapon-dependent audio serves both native
callers once. Native4823c0 retains its typed roll-getup completion after VM
return.

Checked bounded local entries and482a50/48a320 dependencies, mk4decomp
EightEntryAlarmDispatch matching assembly and shared controller contracts.
Verified formatting, shared runtime/rejections, all strict fighter audits,
native gates and helper/port navigation. No new fighter-specific tests.

### Original ground/air reaction branches and front getup data

Replaced ReactionSpec22/39/90/129/130 with shared native branch entries.
482ef0/482f00 selects4e93a8 or4e9430,485f10 selects4e9c80 or4e9430,
485f50 sets native action100a and enters4e9ca8, and484bc0 selects4e9938 or
4e9980. Removed the Kai-named C branch and its copied sound/playback handling.
The old airborne90 approximation incorrectly borrowed reaction10 motion.

Native488c10 now selects original getup record4ea230 through the same shared
loaded-record controller as other getups. Removed the Sonya wrapper's extra
inherited-clip finish. Battle owns KO/round result orchestration rather than
reproducing488c40's result director inside this gameplay native.

Checked bounded local branches, mk4decomp OutroEventForwarderCluster,
PendingMatch_ArgSarStoreJmp and existing script/getup contracts. Verified
formatting, shared runtime/rejections, all strict audits, native gates and
helper/port navigation. Extended the existing loaded-getup fixture with the
new record/address, preserving context, crouch and interruption checks.

### Repair a stale freeze-to-fall continuation

Found the legacy scepter freeze hit-cap branch still selecting retired
ReactionSpec154. Local486210 and mk4decomp Phase3Packed3EntryDispatch branch
to4835f0 once hits exceed3. It now calls the shared disarm/fall native, entering
original4e96e0 without rewriting the reaction ID to select deleted C data.
Verified formatting, runtime/rejections, strict audits and native gates. Added
one transfer assertion to the existing shared native-boundary fixture to catch
retired-row calls; no authored timing/damage matrix.

### Load weapon victim scripts by native identity

Removed Cage/Jarek/Kai/Reptile/Shinnok weapon-reaction dispatchers, seven
copied victim MoveDefs, two ReactionSpec rows and the copied sweep program.
Sixteen reviewed reaction IDs now resolve ten actual native entries once.
This corrects conflicting per-fighter variants of4821c0 and482300 and lets
54/62 use their original482130/4821e0 script and placement rather than copied
fallback rows. Native preludes preserve sound bank distinctions, captured-peer
position, airborne motion, damage scaling and roll-getup continuation. Scripts
own animation, waits, branches, motion operands and recovery selection.

Checked bounded local entries482110/130/1c0/1e0/270/300/350/380/5e0 and
483050, mk4decomp GameModeAdvanceCluster/EightEntryAlarmDispatch and the
existing script/controller contracts. Updated research metadata and historical
source aliases. Verified formatting, shared runtime/rejections, all strict
fighter audits, native gates and existing helper/port navigation. No new tests
for authored weapon reactions.

### Original uppercut and impact scripts

Removed ReactionSpec23/26/32 and copied uppercut phase tables. Shared native
481fc0 preserves scaling and its outer-edge peer reaction14,4838e0 starts
original4e9758 with native scaling/recoil/disarm and independent30-visit audio
426ad0/426af0 (loaded list4ea960), and485e30 preserves hit decrement and the
retail damage/Goro stagger gate before4e97d0 or4e9c28. Added pure481350 to
the shared launch registry so native peer reaction14 enters4e89a0 directly.
Checked local native/dependency listings and mk4decomp OutroEventForwarderCluster
matching assembly. Verified formatting, runtime/rejections, all strict audits,
native gates and helper/port navigation. No authored reaction timing tests.

### Retire ReactionSpec playback entirely

Removed the last ReactionSpec rows, its animation/motion compiler helpers and
Scorpion-named generated-reaction adapter. Loaded483c50/483d40/483d60 share
native483d80 ground handling (loaded4e97f0 callback and original4e97f8).
Airborne483c50 runs its actual4e8a78 continuation; star/acid enter original
4e89a0. Native action distinctions remain2013/1013/0. Removed redundant
star/acid victim MoveDefs and handlers. Shared entries now own their disarm
preludes; the old reaction-ID disarm list applies only to legacy fallback
entries, preventing it from overriding native branch decisions.

Checked bounded local branches and mk4decomp MainTickChain/native task source.
416e20's bounded independent hit task uses the existing engine particle service
(16 emissions); its raw palette/mesh object system remains represented by the
portable generic particle artwork, consistent with existing effect boundaries.
Verified formatting, runtime/rejections, all strict audits, native gates and
helper/port navigation. No new authored reaction tests. Other legacy reaction
phase programs still remain; removing ReactionSpec alone is not completion.

### Original low-hit and recoil programs

Removed conflicting4831e0 low-hit implementations, the Liu-named record
player and their copied getup/slide/launch tables. Shared native preludes now
enter original4e94b0,4e9d88 and4e9de8;31 resolves native4835f0 directly.
4e94b0 owns its human/down branch and tails to existing native recovery;
its authored clip and getup control are no longer reconstructed in C.
4866d0 loads its original callback cell4e9d80. Native486730 keeps C-bank5c.
Verified bounded local native/script entries and mk4decomp shared contracts,
formatting, runtime/rejections, strict audits, native gates and existing helper/
port navigation. Updated bounded source navigation after retiring the old
record player; no new authored reaction tests.

### Shared remaining LK victim boundaries

Removed all roster branches from45b230. Its captured-peer dispatcher now uses
loaded native reaction entries78/79/111, alongside the already shared victims.
484d50 binds4fb700 at the inherited cursor rather than forcing sample75;
4858a0 runs original4e9a68;485040 loads53cf78 and4e99bc, advances through its
native impact phase and completes the descriptor. Removed Sonya/Raiden LK
wrappers and redundant mixed-handler branches. Stage175 remains an explicit
coordinator-owned boundary.

Checked bounded local entries and428d10/428d30/429040, plus mk4decomp
PendingMatch native/script contracts. Verified formatting, runtime/rejections,
strict audits, native gates and helper/port navigation. Extended the existing
inherited-descriptor fixture to catch fixed fighter starting poses. Camera/
render-space cleanup remains the engine's shared controller adaptation.

### Share captured-peer scene reaction setup

Loaded464e30/465200/466110 now share475690/4756f0 facing, orientation and
horizontal-origin setup, then original4e5770/4e5820/4e59c8 binds the victim
animation. Removed fixed561/574 bindings and Tanya's duplicate alignment
implementation/handler. The engine scene owns collision pairing and generic
presentation; raw retail decorative task rendering remains adapted.
Verified bounded native and script entries, formatting, runtime/rejections,
all strict audits, native gates and existing helper/port navigation. No new
authored scene tests.

### Run the stage victim scripts directly

Replaced the synthetic324-visit owner/victim clock and forced clip/height
updates with original4ea300 and4ea330. Loaded native4836e0 starts the victim;
483720 waits for absoluteY<=-0x59999 before entering the impalement words;
4837c0 joins the portable battle result after its native60-visit phase.
The owner's467fc0 now implements only its native three rewind/advance cycles,
clip completion and30-visit park. Removed the stage175 dispatcher bypass.

Checked bounded native phases (including467fc0's jump table), mk4decomp
BattleEndCluster and original victim words. Verified formatting, runtime/
rejections, strict audits, native gates and helper/port navigation. Added one
shared scene/native completion contract to the existing runtime fixture:
original script handoff, bounded completion and retirement of actor playback.
No character timing/damage assertions. Result/input direction and scene rendering
remain engine coordinator/presentation adaptations.

### Remove the last native owner-mask dispatcher

45b1f0 now resolves the captured peer and loaded native target once; all ten
remaining fighter-specific wrappers and both duplicate stage-contact holds are
gone. Throw natives release the pair through the shared boundary.487330 enters
original4e9f08 unless pull arrival already owns its independent1008 stun, in
which case the existing handoff is preserved. Native485910 enters4e9a90 after
the owner's original paired two-pose advance; removed Liu's premature native
two-visit victim player.481360 flips and binds loaded53cfa8 for the scene-held
victim, with camera/result orchestration in the existing scene director.

No owner bitmask or roster-dependent native handler remains in retail_native_vm.
Checked local45b1f0/480f80,487330,485910,481360,42c0d0 and original paired/victim
words. Verified formatting, runtime/rejections, all strict audits, native gates
and helper/port navigation. Reused the loaded-stagger fixture through45b1f0
with a different native context to check peer selection and caller ownership;
no authored move timing assertions.
