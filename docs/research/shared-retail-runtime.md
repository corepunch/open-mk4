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
formatting, game build and game self-test (all18 rigs and11 arenas) pass.
Battle integration stops at the established ground-throw assertion. The broader
asset suite passes the snapshot/rig binding checks but later stops at its
existing air-throw attacker/victim clip expectation; that native throw family
has not yet been consolidated. No new fighter tests were added.
