"""Reusable contracts recovered from local retail listings and character ports.

Names are semantic aliases for our EXE, not upstream symbols or inferred C
signatures. `port` identifies the existing engine adaptation, never an original
source recovery. Related addresses are explicit, bounded navigation, not a
recursive native disassembly closure. Add new knowledge here once for all ports.
"""
from dataclasses import dataclass


@dataclass(frozen=True)
class Helper:
    name: str
    inputs: str
    effects: str
    flow: str
    port: tuple = ()
    related: tuple = ()
    tables: tuple = ()
    call: str = ''  # optional VM-call template, using decoded inline/word values


H = Helper
SZ = 'game/sub_zero/character.c'
LK = 'game/liu_kang/character.c'
HELPERS = {
    0x45bc70: H('blend_clip_resume_one', 'initialized source pose and destination descriptor', '45bd20/428b10 blend to destination sample zero; step 0xa3d, first weight 0x4000; resume at sample one. Opcode 71 supplies the descriptor.', 'Yield through the shared pose blend, then continue generated playback.', ('game/script.c', 'mk4_mmove_blend_bind'), (0x45bd20, 0x428b10)),
    0x45bbc0: H('reverse_pair_to_sample', 'current paired owner/victim and low-byte target', 'Reverse both pose contexts until the owner reaches target, retaining the initial cursor offset.', 'One-tick visits and final handoff.', ('game/script.c', 'mk4_mmove_pair_back_step')),
    0x49be10: H('raiden_lightning_object', 'task type 65, owner and captured heading', 'Independent mesh-41 projectile; speed 0x3333, C[2] contact, 0x23d7 damage and reaction 66.', 'Object survives owner interruption; bounded pool owns its lifetime.', ('game/raiden/character.c', 'script_lightning_spawn')),
    0x49ba50: H('sonya_ring_object', 'task type 68, owner and captured heading', 'Independent mesh-42 projectile; speed 0x3333, C[4] contact, 0x1eb8 damage and reaction 68.', 'Shared object movement/contact and independent lifetime.', ('game/sonya/character.c', 'script_ring_spawn')),
    0x46aed0: H('sonya_air_throw_release', 'owner/victim clips 77/161 at paired count 24', 'Release reaction 85, apply 0x2666 damage, finish owner; owner gravity 0x20c, vertical -0x51e and landing.', 'Independent victim 4e98a0/4847e0; native 53cf70 getup.', ('game/sonya/character.c', 'script_air_release'), (0x4847e0,)),
    0x47dee0: H('sonya_leg_grab', 'Down+Block+LP chord and cooldown gate', 'Clip 102 eleven-sample opening, D[3] hit; generated 4e8578 paired reversal or 4e85a8 miss/block. Owner 11/victim 0 offset leaves victim at 23 on release.', 'Shared pair cleanup and independent 4e9a68 recovery.', ('game/sonya/character.c', 'sonya_leg_grab')),
    0x47cd00: H('raiden_teleport', 'Down,Up direction press and actor cooldown', 'Sink, relocate behind victim, rise and land through engine body physics. Untouchable during transit.', 'Cooperative owner fiber and actor-owned cooldown.', ('game/raiden/character.c', 'raiden_teleport')),
    0x464bd0: H('scene_keepalive_task', 'scene task type 619', 'Native task only retains a slot through one-tick reschedules. Portable bounded effect object lives through PHASE_FATALITY.', 'Independent lifetime without combat writes.', ('game/script.c', 'mk4_script_scene_task')),
    0x45eab0: H('liu_bicycle_charge_gate', 'LK release; per-player last LK press tick', 'Reject opponent action 100b via 45f540, then require tick minus LK press >=120. P1/P2 latches are 53c1fc/524e44.', 'Immediate predicate.', (LK, 'can_bicycle')),
    0x47eaf0: H('liu_bicycle_entry', 'grounded release command or direct CPU entry', 'Action 206, D[6], clip 68 six stopped samples; restore speed 0x2147/Y -0xa3d/gravity 0xc4, indexed sound 1 and cycle 7..22.', 'Resumable owner and independent victim recovery.', (LK, 'liu_kang_bicycle_kick'), (0x47eb50, 0x4859b0)),
    0x47eb50: H('liu_bicycle_hit', 'D[6] outcome', 'Block selects 4e8688 recoil; hit clamps owner Y <=-0x15999, halves horizontal speed, stops Y/gravity, holds pair 58 ticks at 0xe666, then release reaction 81 and ground owner through 4906c0.', 'Only initial D[6] damage; visual repetition is not repeated health loss.', (LK, 'bicycle_script')),
    0x488870: H('liu_bicycle_victim_hold', 'reaction 84 victim', 'Disarm, clip 136, then repeat from sample 4 while owner action is 206. Interruption exits through 487980.', 'Victim-owned hold/recovery fiber.', (LK, 'bicycle_victim_script')),
    0x4859b0: H('liu_bicycle_victim_release', 'released victim; current height', 'Action 100b; height >0x3333 selects airborne 4e9ae8, otherwise clip 182 at -0x4ccc with friction. Air uses 183 at -0xccc/Y -0xf5c/gravity 0x312 then getup.', 'Independent of owner recovery and KO.', (LK, 'bicycle_victim_script')),
    0x4315b0: H('dispatch_scene_event', 'inline event selector in 53c42c; active scene actor', 'Opcode 56 masks low byte, dispatches through scene task 4dea40 and writes selector to task +18 with actor context +24. Liu Kang second fatality queues events 42/43/44 around waits 15/20/60.', 'Immediate scene event request; presentation task owns any continuation.', (LK, 'liu_script_scene_event')),
    0x450930: H('liu_dragon_fatality', '4e0498 roster slot 3; actor/victim scene contexts', 'Align 0x20ccc; setup waits 16/85, spawn rig 508204 (lk_fat_g.geo), wait 20; clip 418 samples 0..159, 160..518, loop 446..518. Victim starts after 269, blends 389/419 then plays 419.', 'Independent actor, victim and decoration clocks.', (LK, 'dragon_fatality_script'), (0x451190, 0x451300)),
    0x451190: H('liu_dragon_victim', 'scene victim', 'Wait 269; common blend 475190 then bind 419 through 475340 and play 4e1fa8. Decorations independently remove limbs.', 'Independent victim clock.', (LK, 'dragon_fatality_script')),
    0x451300: H('liu_dragon_blood_and_cuts', 'victim scene context', 'B[37], wait 4, blood loops 23 four-visit and 26 six-visit phases, wait 8; final joint removals 5/6 through 4514f0. Portable bounded blood/piece pools adapt the native per-joint tasks.', 'Independent decoration task.', (LK, 'dragon_fatality_script')),
    0x43c3d0: H('liu_fireball_fatality', 'second-fatality roster slot 3; close scene', '4dfff8 events 42/43/44, waits 15/20/60, then 4e0020 paired clips 537/539 through 37; victim reaction 176; finish 537, wait 18, A[33]/indexed[57], clip 538, spawn 43c540, reverse and park.', 'Owner fiber plus independent finisher projectile.', ('game/liu_kang/generated/scripts.inc', 'retail_script_004dfff8'), (0x43c540, 0x481360, 0x481420)),
    0x43c540: H('liu_finisher_projectile', 'task type 38; captured heading and victim context', '4e0078 spawn uses mesh 50, forward 0x1999 and dy -0x7333. Travel toward victim, dispatch reaction 177 when within 2.5 native scene-Z units, then free object.', 'Portable pair-axis distance replaces fixed scene Z.', (LK, 'fatal_fireball_object')),
    0x481360: H('liu_finisher_victim_hold', 'victim after paired sample 37', 'Flip heading, hold clip 539 and install camera tracking; native task parks while projectile approaches.', 'Victim presentation held until reaction 177.', (LK, 'liu_script_reaction')),
    0x481420: H('liu_finisher_victim_explosion', 'victim after finisher projectile arrival', '4891c0 spawns dismemberment tasks; 489110 waits 160. Portable generic flesh pieces and blood use bounded scene pools.', 'Independent explosion tail.', (LK, 'fatal_fireball_object')),
    0x45b050: H('schedule_horizontal_motion', 'packed low/high 12-bit delay/duration; one following signed Q16 speed word', 'Stop motion; unpack counts through 45c020 into task +5c/+64, fetch speed through 45bff0 into +60, install 494860 as group callback. Opcode 6c consumes one following word.', 'Independent body callback while script playback continues.', (LK, 'sword_travel'), (0x494860,)),
    0x4963e0: H('liu_weapon_high_punch', 'armed type 66; HP press latch', '4ec680: clip 233 startup double steps to 18; clip 216 weapon[15] at 0/10. Buffered HP repeats the two strikes. Each reached 6c schedules delay 6/duration 6/speed 0x11eb. Recovery blends to armed stance.', 'Sequential repeat fiber with independent motion callback.', (LK, 'sword_hp')),
    0x4964d0: H('liu_weapon_low_punch', 'armed type 66; LP entry, HP continuation latch', '4ec708: clip 240, weapon[13], sound C[34]; back alternate 496920. Contact continuation 496500 may select 4ec720/730 clip 237 weapon[16] when HP buffered.', 'Shared contact/playback boundaries; character owns follow-up decisions.', (LK, 'sword_lp')),
    0x496670: H('liu_weapon_back_high_punch', 'armed type 66', '4ec740 travel 2/18/0x28f5, 4ec750 clip 242 from 8, weapon[17], fixed armed-stance blend after contact.', 'Cooperative strike.', (LK, 'sword_back_hp')),
    0x496920: H('liu_weapon_back_low_punch', 'armed type 66', '4ec7e0 travel 8/13/0x1999; 4ec7f0 clip 227 double steps to 16/26 then 29; two weapon[11] queries; finish through 496840.', 'Cooperative strike.', (LK, 'sword_back_lp')),
    0x49bf50: H('liu_low_fireball_entry', 'grounded active fighter', 'Action 30d; snapshot input context, then clip 19/script 4ed678. Double-step startup to 20, independent low projectile, three 18..23 recovery cycles.', 'Owner fiber and independent projectile callback.', (LK, 'liu_kang_low_fireball'), (0x49c110,)),
    0x49bf90: H('liu_high_fireball_entry', 'ground or airborne active fighter', 'Ground action 30c and script 4ed6c0; airborne branch 49bfe0/action 30e/script 4ed6f8.', 'Owner and projectile have independent lifetimes.', (LK, 'liu_kang_high_fireball'), (0x49bfe0, 0x49c160)),
    0x49bfe0: H('liu_air_fireball_entry', 'airborne active fighter', 'Clip 20 six samples, indexed sound 3, spawn 49c0d0, hold 16 and reverse; gravity 0x20c until shared landing.', 'Cooperative owner fiber.', (LK, 'liu_kang_high_fireball'), (0x49c0d0,)),
    0x49c110: H('liu_low_fireball_object', 'independent task type 64, captured owner heading', 'Spawn from 4ed740, speed 0x3333 along heading, life 48 ticks, C[1] including crouched target; rig mesh 50.', 'Contact/expiry frees only this object.', (LK, 'fireball_spawn'), tables=('liu_low_fireball_spawn',)),
    0x49c160: H('liu_high_fireball_object', 'independent task type 64, captured owner heading', 'Spawn from 4ed760, speed 0x3333, life 48 ticks, C[0]; skip victim action 2001.', 'Independent of owner interruption.', (LK, 'fireball_spawn'), tables=('liu_high_fireball_spawn',)),
    0x49c0d0: H('liu_air_fireball_object', 'independent task type 64, captured owner heading', 'Spawn from 4ed730; high C[0] record and 48-tick life.', 'Independent object.', (LK, 'fireball_spawn'), tables=('liu_fireball_spawn',)),
    0x47edc0: H('liu_flying_kick_entry', 'grounded active fighter', 'Action 207, clip 92; 4e86a8 motion 0x23d7/-0x170a/0x106 and indexed sound 0. Six opening visits then D[12] until landing/contact.', 'Hit/block/miss recover separately.', (LK, 'liu_kang_flying_kick'), (0x47ee30,)),
    0x47ee30: H('liu_flying_kick_contact', 'airborne kicking owner', 'D[12] hit plays 93 then hold 20/gravity 0x51e/reverse 92. Block holds 12+12 then gravity 0x28f/shared landing. Miss reverses 92 after landing.', 'Sequential owner fiber.', (LK, 'flying_kick_script')),
    0x46d290: H('standing_high_punch', 'standing actor, LP/HP press latches and victim action', 'Native repeat/contact/recovery graph; action 10a writer is at 46d9b8. Portable graph retains live HP repeats and ducking low-kick gate.', 'Cooperative attack phases.', (SZ, 'sub_zero_hp'), (0x46d9b3,)),
    0x42c560: H('ground_lp_throw', 'paired grounded fighters; throw predicate 42c3d0', 'Roster selector 4de798 chooses Scorpion 4de6e4 (75/152, release 42) or Sub-Zero 4de6b8 (76/153, release 25, reaction 102). Sub-Zero now uses 76/153, damage 0x2666 at 25 and owner return at 49. Its victim tumble 142/141/143, twenty-tick hold and reversed getup 328 are independent fibers.', 'Independent victim continuation; interruption before release cancels damage.', (SZ, 'enter_throw'), (0x42c3d0, 0x42ceb0)),
    0x42b6b0: H('ground_lk_throw', 'paired grounded fighters; throw predicate 42c3d0', 'Scorpion uses 37/157 and release 4de328 (damage 0x2e14, pose 65). Sub-Zero native table 4ddfb8[4] selects 42c180 -> 4de490 (41/165), then native 42c200. The Sub-Zero fiber now disarms at 39, damages 0x2e14 at 52, plays five more paired samples, then holds owner idle for sixty ticks. Victim recovery is 4e9ee0 then clip 200.', 'Cooperative paired opening, damage and release boundaries.', (SZ, 'enter_lk_throw'), (0x42bdf0, 0x42be00, 0x42be10)),
    # VM wrappers: checked at their exact local entries, not guessed from names.
    0x45ac70: H('save_reaction_context', 'current group', 'Save current group in 53c144 for the reaction motion/impact path.', 'Immediate bookkeeping; engine owns an explicit victim body context.', call='save_reaction_context();'),
    0x45af60: H('reaction_motion_block', 'four fixed signed words', '490910 combines heading and camera-oriented perpendicular motion from the first two words, then assigns vertical velocity and gravity. The horizontal operands are independent; Kai weapon victim146 supplies -0xa3d,0xccc.', 'Motion runs independently of clip playback.', ('game/script.c', 'mk4_script_reaction_motion'), (0x490910,), call='reaction_motion_block({w0}, {w1}, {w2}, {w3});'),
    0x42c200: H('sub_zero_lk_pair_rebase', 'owner after its ten-sample opening', '48f9f0 stops motion and aligns origin/facing; continue script 4de4b8 with paired 29,5,8 counts.', 'Owner cursor 10; victim cursor 0.', (SZ, 'script_lk_rebase'), (0x48f9f0,)),
    0x48f6e0: H('save_throw_momentum', 'current group velocity', 'Copy group velocity +6c/+70/+74 to task +5c/+60/+64, then stop motion. Sub-Zero LK saves -0x1999 along its entry pair axis.', 'Saved vector survives setup until 42c220 restores it.', (SZ, 'script_lk_rebase')),
    0x42c220: H('sub_zero_lk_throw_damage', 'saved entry velocity and paired cursor 52', 'Apply 0x2e14 damage, restore saved velocity on owner and its negation on victim; enter 4de4e0 for five more paired samples.', 'Damage precedes independent victim release.', (SZ, 'script_lk_rebase'), (0x42c300, 0x48f6e0)),
    0x42c300: H('sub_zero_lk_throw_return', 'owner after paired cursor 57', 'Select opponent continuation table 4ea7f0[7] (4871e0), bind owner idle, wait 0x3c ticks, then controller return.', 'Victim recovery continues independently through a lethal throw.', (SZ, 'script_lk_rebase'), (0x4871e0,)),
    0x485b70: H('sub_zero_lp_throw_reaction', 'victim reaction 102', 'Setup/disarm, descriptor 4e9bb8 contains VM body 4e9bd0: 142/141/143 playback, vertical -0x147a, gravity 0xde, twenty-tick hold; 488b80 reverses facing and selects getup 328.', 'Independent victim fiber; dead victims settle without standing.', ('game/script.c', 'mk4_script_reaction_motion'), (0x485c60, 0x488b80)),
    0x4871e0: H('sub_zero_lk_victim_release', 'opponent continuation table 4ea7f0[7]', 'Install friction and script 4e9ee0: cursor 45, reverse to 41 and forward to 46 three times, then 487220.', 'Independent recovery.', (SZ, 'lk_thrown_script'), (0x487220,)),
    0x487220: H('sub_zero_lk_victim_finish', 'victim after three recovery cursor cycles', 'Bind clip 200 from descriptor 4fb6dc and 53cf90, play all 41 stored samples, restore idle pose and return through controller.', 'Living victim returns to its own roster controls; dead victim parks for defeat handling.', (SZ, 'lk_thrown_script')),
    0x4913a0: H('velocity_friction', 'group X/Z velocity', 'Multiply horizontal velocity by 0xe666 each world pump; stop below squared speed 0x41.', 'Independent group physics callback.', ('game/reactions.c', 'fighter_friction')),
    0x4596e0: H('indexed_sound_event', 'low byte of inline; table 4ead30', 'Lookup a 16-bit sampled/control token; index <= 0x41, zero tokens do nothing. This is NOT bank B.', 'Immediate; dispatch resumes. Queue battle_sound(SOUND_INDEXED, selector); the common resolver maps sampled tokens through /5.', ('common/mk4_audio.c', 'sound_event'), related=(0x48a0d0,), tables=('indexed_sounds',), call='indexed_sound_event({arg});'),
    0x4595e0: H('roster_voice_event', 'low-byte selector and active task roster', 'Select 53d0a0 row and a roster-specific 16-bit token; Kitana/Noob remap to Sonya/Reiko.', 'Immediate; dispatch resumes.', related=(0x48a230,), call='roster_voice_event({arg});'),
    0x45a4d0: H('submit_sample', 'low-byte stored sample', '429970 submits the requested pose without a scheduler wait.', 'Immediate; the following wait is a separate operation.', related=(0x429970,), call='show_sample({arg});'),
    0x45ba40: H('rewind_to_sample', 'low-byte target sample; current clip/cursor', '4292f0 decrements playback toward the target; retain the bound clip.', 'Cooperative playback; resumes script after completion.', ('game/native.c', 'native_play_rows'), (0x4292f0,), call='rewind_to_sample({arg});'),
    0x45bb10: H('advance_pair_to_sample', 'low-byte target, current fighter and task opponent', '45bde0 advances both pose contexts; compare the active cursor with target.', 'Cooperative: one-tick visits until target, then one final reschedule.', (SZ, 'script_lk_rebase'), (0x45bde0,), call='advance_pair_to_sample({arg});'),
    0x45b1f0: H('dispatch_current_reaction', 'low-byte reaction selector', '480f80 selects reaction table 4ea4e8; 48f880/48fd00 installs the selected continuation on the task peer (+38/+3c), then 480f80 also enters 4e8998. The historical current-context label does not mean the executing portable Fighter is the victim.', 'May suspend through reaction setup, then resumes VM.', related=(0x480f80,), call='dispatch_current_reaction({arg});'),
    0x45b230: H('dispatch_opponent_reaction', 'low-byte reaction selector', '480f50 uses reaction table 4ea4e8 in the opponent context. LK throw release selects 77; engine victim recovery is an explicit adaptation.', 'May replace victim continuation; owner resumes VM.', related=(0x480f50,), call='dispatch_opponent_reaction({arg});'),
    0x45ae50: H('throw_pair_adjustment_native', 'one fixed word; LP throw supplies 0x1eb8', 'Fetch word via 45bff0, call 42ceb0. That helper compares group points and writes heading-scaled group +6c/+74. Group +74 here is NOT the controller action field. Native math 42d000 remains a boundary; portable throw uses shared origin/reaction physics.', 'Calls honor cooperative pause before VM dispatch. This is a native boundary, not a complete math translation.', related=(0x42ceb0, 0x42d000), call='throw_pair_adjustment_native({w0});'),
    0x459720: H('roster_sound_list_event', 'low-byte selector and current roster', '48fd00 invokes 489ff0, using roster remap 4ead18 and selector table 53d000, including random lists. This is distinct from direct A/B/C and indexed sounds. Upstream equal-address sound names do not prove a match.', 'Immediate/cooperative audio wrapper.', related=(0x489ff0,), call='roster_sound_list_event({arg});'),
    0x459ac0: H('rewind_clip', 'one packed animation descriptor operand', 'Bind descriptor; 429460 starts from its last stored sample and decrements.', 'Cooperative playback to clip start.', related=(0x429460,), call='rewind_clip({clip});'),
    0x45aef0: H('set_motion_q16', 'three signed Q16 words: heading speed, Y velocity, gravity', '490390 resolves speed along the pair axis; store Y velocity at group +70 and gravity at +4c.', 'Immediate; motion continues in world stepping.', ('game/moves.c', 'fighter_set_speed'), (0x490390,), call='set_motion_q16({w0}, {w1}, {w2});'),
    0x45b010: H('stop_motion', 'current fighter group', '4906e0 clears X/Y/Z velocity, gravity and all three angular velocities.', 'Immediate.', (SZ, 'stop_body'), (0x4906e0,), call='stop_motion();'),
    0x45baf0: H('mark_fatality_scene_active', 'none', 'Set scene flag 5322c4 to one.', 'Immediate.', call='mark_fatality_scene_active();'),
    0x45b1b0: H('scene_impact', 'current fighter group and player context', '457cb0 selects roster voice 1, records X/Z impact, starts camera shake record 0 and sound A[19].', 'Immediate setup; camera shake continues independently. Portable impact keeps normal fight framing.', ('game/presentation.c', 'battle_scene_impact'), related=(0x457cb0, 0x457b50), tables=('impact_voices', 'camera_shakes'), call='scene_impact();'),
    0x45abf0: H('enqueue_context_task', 'low-byte context selector and current fighter group', '431520 conditionally creates task script 4de9f0; stores selector at task +18 and group at +24.', 'Spawn is independent; context modes may suppress it. Task behavior remains a separate boundary.', related=(0x431520,), call='enqueue_context_task({arg});'),
    0x45a780: H('playback_current_cursor_continuation', 'current clip and cursor', '429200 enters 429590 playback and polls native result flag 1 via 429850.', 'Cooperative; do not treat this as an immediate pose assignment. Endpoint behavior remains in 429850.', related=(0x429200, 0x429850), call='playback_current_cursor_continuation();'),
    0x45b2f0: H('wait_landing_continuation', 'current fighter, landing callback', 'Enter 48f390 landing/callback dispatcher.', 'Cooperative; continue only after landing path completes.', ('game/mk4_common.c', 'mk4_land'), (0x48f390,), call='wait_landing_continuation();'),
    0x459990: H('spawn_script_task', 'two fixed words: task type, native callback; inline byte ignored', '49cb00 -> 41f290 allocates an independent task with inherited context.', 'Parent script continues; target is not called inline.', ('game/combat.c', 'battle_spawn_projectile'), (0x49cb00, 0x41f290)),
    # Shared primitives and gates.
    0x45ed70: H('can_air_throw_command', 'both absolute group Y positions and Q16 X/Y/Z deltas', '45ed80/45eef0 require both Y <= -0x23333 and sum of three Q16 squared deltas <= 0x20000. Portable entry also rejects unavailable victims.', 'Press gate; entry rechecks before taking control. Retail Sub-Zero BLOCK list has no air-throw record; its new binding is an explicit extension.', ('game/mk4_common.c', 'mk4_can_air_throw'), (0x45ed80, 0x45eef0)),
    0x46aa80: H('air_throw_entry', 'eligible owner and victim', 'Action 502; take/disarm victim, stop and upright both bodies, align facing/origin. Clips owner 82/victim 202; indexed sound 13 (token 2100).', 'Paired playback to sample 28; context-task camera 23 is adapted by normal portable fight framing.', ('game/mk4_common.c', 'mk4_air_throw_frames'), (0x45ed70, 0x46aae0, 0x4878c0), ('indexed_sounds',)),
    0x46aae0: H('air_throw_release', 'paired owner at sample 28; victim still attached', '480f50[89] releases victim; apply 0x2147 damage once. Then one sample, owner speed 0x7ae, Y velocity -0x1999, gravity 0x28f; wait ten ticks, finish clip and wait landing.', 'Portable runner releases before world physics for either player and cancels damage if the pair was interrupted.', ('game/mk4_common.c', 'mk4_script_air_throw_release'), (0x484b40, 0x46ab20)),
    0x46ab20: H('air_throw_landing', 'owner after clip completion and floor contact', '46fd90 landing samples 18..27, then 46f650 stance return.', 'Use the shared landing continuation.', ('game/mk4_common.c', 'mk4_land')),
    0x4878c0: H('air_throw_pair_wait', 'captured victim and owner action', 'Hold victim clip 202 with owner; release the wait when owner action changes. Interruption must not leave a suspended body.', 'Victim recovery is independent after release.', ('game/mk4_common.c', 'air_frame_victim_tick'), (0x484b40,)),
    0x484b40: H('air_throw_victim_release', 'captured victim; reaction 89 via 4ea4e8', '4e9910: speed 0, Y velocity 0x3333, gravity 0x28f; wait landing, scene impact, finish clip 202, wait four, then 484b60.', 'Fall begins in the same world update as damage.', ('game/mk4_common.c', 'air_throw_drop'), (0x484b60, 0x457cb0)),
    0x484b60: H('air_throw_victim_getup', 'victim after landing playback and four-tick wait', 'Turn for reverse getup, clip 131 samples 46..79. Portable recovery returns a living victim to its own stance instead of parking a retail task.', 'Root alignment and task parking use portable engine recovery.', ('game/mk4_common.c', 'mk4_mmove_air_getup')),
    0x45b650: H('fast_startup_allowed', 'attacker follow-up flag; victim airborne/reacting flags', 'Grounded target: attacker flag 0x200. Air target: victim flag 4. These are different owners.', 'Branch to operand script when allowed; otherwise fall through.', ('game/moves.c', 'fighter_fast_start')),
    0x45f1f0: H('can_ice_command', 'match phase, victim hit count, clone task types 45/46', 'Reject an active clone; do not reject another ice projectile (task 42).', 'Command predicate before move entry.', (SZ, 'can_ice')),
    0x45ece0: H('can_clone_command', 'clone cooldown, victim action and existing clone', 'Require cooldown elapsed; victim not frozen (action 1003); no owned clone.', 'Command predicate.', (SZ, 'can_clone')),
    0x45f270: H('can_slide_chord', 'held LP+BLOCK+LK, cooldown and phase', 'Require the exact permitted button mask; slide precedes ordinary button actions.', 'Command predicate.', (SZ, 'can_slide')),
    0x45ef50: H('can_draw_or_throw_weapon', 'weapon-used/lost flags and armed state', 'An armed fighter may throw; an unarmed fighter needs an unused available weapon.', 'Command predicate.', (SZ, 'can_weapon')),
    0x45e820: H('can_run_double_tap', 'previous RUN latch, grounded state, cooldown', 'Previous press within eight ticks; current latch is written after matching. Sidestep takes priority over pickup.', 'Command predicate.', (SZ, 'can_sidestep')),
    0x45e460: H('can_first_fatality_chord', 'winner, finish window, range and held mask', 'Require HP+BLOCK+RUN, <=1 unit; reject Up/HK/LP/LK. Finish timing is shared.', 'Command predicate.', (SZ, 'can_first_fatality')),
    0x45e530: H('can_second_fatality_range', 'winner, finish window, horizontal range', 'Require range 0x1cccc..3 Q16 units.', 'Command predicate.', (SZ, 'can_second_fatality')),
    0x45e5a0: H('can_fan_stage_fatality', 'winner, window, stage and range', 'Use the fan-stage gate, not an ordinary fatality range alone.', 'Command predicate.', (SZ, 'can_fan_fatality')),
    0x45e4f0: H('can_spike_stage_fatality', 'winner, window, stage and range', 'Use the spike-stage gate.', 'Command predicate.', (SZ, 'can_spike_fatality')),
    0x48d3d0: H('contact_query_a', 'selector, active fighter pose, pair axis', 'Read table A; apply grounded/air/block reaction, record damage and contact result.', 'Immediate contact; reaction may replace victim continuation.', ('game/combat.c', 'battle_contact')),
    0x48d3f0: H('contact_query_b', 'selector, active fighter pose, pair axis', 'Read table B; preserve distinct hit/air/block reactions and contact-selected recovery.', 'Immediate; result flags are context-dependent.', ('game/combat.c', 'battle_contact')),
    0x490390: H('set_horizontal_speed_q16', 'signed Q16 speed in native argument scratch', 'Resolve velocity along the X/Z pair axis; retain the sign.', 'Immediate.', ('game/moves.c', 'fighter_set_speed')),
    0x490140: H('set_no_guard', 'current fighter group', 'Set the no-guard flag; interruption/recovery must clear it at the reached source path.', 'Immediate.', related=()),
    0x490330: H('clear_busy', 'current fighter group', 'Clear busy flag; does not itself restore all controls.', 'Immediate.'),
    0x46f650: H('fighter_controller_return', 'fighter state, held input, landing status', 'Select stance/crouch/block/air return and root-aware landing; standing blend is interruptible.', 'Return through shared controller.', ('game/mk4_common.c', 'mk4_return_to_stance')),
    0x46a950: H('task_slot_5c_clear', 'current task', 'Clear task +5c (attack avoidance window).', 'Immediate.'),
    0x46a970: H('task_slot_5c_set_one', 'current task', 'Set task +5c (attack avoidance window). Clear on interruption.', 'Immediate.'),
    # Ice and clone: all numerical values are from this EXE/its data tables.
    0x49bc60: H('sub_zero_ice_entry', 'active fighter; command and fast-start gates', 'Set action 304, stop movement and enter 4ed588. Clip 16 startup takes 15 or eight ticks.', 'Starts script; projectile has independent lifetime.', (SZ, 'sub_zero_ice'), (0x45b650, 0x49bcb0, 0x486230), ('ice_spawn', 'ice_meshes', 'indexed_sounds')),
    0x49bcb0: H('ice_projectile_task', 'owner group position/heading and descriptor 4ed5d0', 'Spawn forward 0xccc, Y -0x28f; X/Z speed 0x3333. Initial rig mesh 74; track 75..82, one tick each, looping without the spawn rig. Contact C[3], reaction 67, zero damage. Skip action 2001; expire on contact/bounds.', 'Independent of owner interruption; native contact waiter 49c690.', (SZ, 'ice_projectile'), (0x49c300, 0x49c690, 0x40d720), ('ice_spawn', 'ice_meshes')),
    0x49c300: H('spawn_heading_object', 'descriptor: packed rig, forward offset, Y offset, initial animation track', 'Copy owner origin; add heading-relative offset; set object orientation and callbacks.', 'Allocation failure is a real branch, never overwrite another object.', ('game/combat.c', 'battle_spawn_projectile'), tables=('ice_spawn',)),
    0x49c690: H('projectile_contact_waiter', 'independent object and contact descriptor', 'Probe contacts and bounds while the owner can run a different move.', 'Independent continuation.', ('game/combat.c', 'battle_projectile_contact')),
    0x486210: H('scepter_freeze_hit_gate', 'scepter victim hit count and frozen state', 'Reaction 151 uses freeze/hit-cap behavior rather than unconditional damage.', 'May replace reaction controller.', ('game/reactions.c', 'react_scepter_freeze')),
    0x486230: H('freeze_reaction', 'victim current rig/pose; already-frozen state', 'Capture pose and closed-surface rig. Hold 142 ticks without direct damage. Double freeze backfires onto the attacker; Goro is not excluded.', 'Independent victim controller; thaw restores controls or launches the air thaw path.', ('game/reactions.c', 'react_freeze'), (0x486310, 0x486430, 0x486530, 0x48ce00)),
    0x486310: H('freeze_shake_clock', 'frozen victim heading and captured pose', 'Six 3+3-tick +/-0xccc visual cycles, 88-tick hold, then three 3+3 cycles. Keep combat origin unchanged.', '142-tick victim continuation.', ('game/reactions.c', 'frozen_play')),
    0x486430: H('freeze_backfire', 'already frozen victim and opponent', 'Switch context to the opponent before applying freeze again.', 'Reaction context transfer.', ('game/reactions.c', 'react_freeze')),
    0x486530: H('freeze_air_thaw', 'airborne victim at thaw', 'Restore normal presentation, gravity 0x147, then land before restoring controller.', 'Cooperative landing.', ('game/reactions.c', 'thaw_landing_start')),
    0x47f4d0: H('sub_zero_clone_entry', 'current pose, pair range, victim human/CPU ownership', 'Snapshot BEFORE retreat. Range <1 omits statue. Lifetime 74 ticks versus human /53 versus CPU; cooldown 180. Retreat clip 301.', 'Owner retreat and statue object have independent clocks.', (SZ, 'sub_zero_clone'), (0x47f6d0, 0x47f880, 0x47f9d0, 0x47fa90), ('clone_rig',)),
    0x47f6d0: H('clone_retreat', 'retreat fighter clip 301 and body', 'Advance samples 0..8; adjust root/group Y by opposite 0x3333 and spin -0x6147 on ascent. Speed -0x170a, Y velocity -0x28f5, gravity 0x28f.', 'Wait until apex; independent of the statue lifetime.', (SZ, 'clone_spin'), (0x47f880,)),
    0x47f880: H('clone_apex_return', 'fighter at apex', 'Stop angular motion, restore orientation, retract clip while descending; complete retraction and shared landing.', 'Cooperative return.', (SZ, 'clone_turn')),
    0x47f9d0: H('clone_contact_object', 'captured pose/origin; contact D[5]', 'Use shared statue meshes 58..72 and freeze reaction on contact; remove on contact or scene cleanup.', 'Independent object; no following the owner.', (SZ, 'clone_object'), (0x47fa90,), ('clone_rig',)),
    0x47fa90: H('clone_orbit_light', 'statue decoration light', 'World-origin orbit radius 25, Y -25, phase +=0x18000 radians Q16, strength -=0xccc each tick.', 'Independent presentation clock.', ('game/presentation.c', 'battle_object_light')),
    # Slide, weapons and movement.
    0x47dc00: H('sub_zero_slide_entry', 'LP+BLOCK+LK chord and cooldown gate', 'Clip 95; speed 0x2666, sound C[15], action 204; then 4e84d8/47dc40.', 'Owner continuation.', (SZ, 'sub_zero_slide'), (0x47dc40,)),
    0x47dc40: H('slide_contact_recovery', 'slide fighter; hit/blocked/miss contact', 'Reactions 122/165. Unblocked contact stamps 64-tick cooldown; block stops motion, sets no-guard and uses longer recoil. Miss does not stamp cooldown.', 'Branch into 4e84f8 hit /4e8530 block; then retract and restore controls.', (SZ, 'slide_script')),
    0x469250: H('run_double_tap_entry', 'previous RUN latch and Down state', 'Shared clip 300, 80-tick cooldown; Down selects forward playback and reverses lateral direction; air victim reduces speed to 30%.', 'Owner sidestep continuation.', (SZ, 'sub_zero_sidestep'), (0x4692e0,)),
    0x4692e0: H('run_sidestep_motion', 'pair axis and Down state', 'Retain both heading and perpendicular X/Z components; do not confuse it with weapon pickup.', '18-tick playback and return.', ('game/mk4_common.c', 'mk4_sidestep')),
    0x46d1d0: H('draw_or_throw_weapon', 'armed state and follow-up flag', 'Armed -> throw. Follow-up -> equip and paired reaction 120, direct back-HP setup without ordinary travel. Otherwise draw.', 'Shared weapon lifecycle.', (SZ, 'sub_zero_weapon'), (0x494610, 0x4946f0, 0x495710)),
    0x494610: H('followup_weapon_draw', 'follow-up attacker and victim', 'Equip and retain no-guard through paired reaction; >3 victim hits uses 30% damage scaling.', 'Owner and victim continuations.', ('game/weapons.c', 'weapon_followup_victim'), (0x4946f0,)),
    0x4946f0: H('followup_weapon_victim', 'victim paired with drawing attacker', 'Reaction 120 controls paired damage/release; do not clear attack restrictions prematurely.', 'Independent victim recovery.', ('game/reactions.c', 'react_weapon_followup')),
    0x494a00: H('shared_weapon_strike', 'WeaponStrike record; travel enabled/disabled', 'Translate startup/contact/recovery phases from the weapon record; keep contact-selected recovery.', 'Cooperative strike runner.', ('game/weapons.c', 'weapon_start_strike')),
    0x495110: H('weapon_hit_recovery', 'strike contact and recovery record', 'Hit/block/miss choose distinct recovery paths; retain paired victim behavior.', 'Owner recovery.', ('game/weapons.c', 'strike_recovery')),
    0x495300: H('weapon_stance_blend', 'current weapon stance and source pose', 'Blend to stance after LP rather than jumping immediately.', 'Cooperative pose blend.', ('game/weapons.c', 'blend_tick')),
    0x4956c0: H('scepter_high_punch', 'armed HP', 'Clip 231, 18% damage; shared strike parameters 4ec458.', 'Shared strike runner.', (SZ, 'scepter_hp'), (0x494a00,)),
    0x495720: H('scepter_low_punch', 'armed LP', 'Clip 250, 15% low damage; params 4ec490; stance-blend recovery.', 'Shared strike runner.', (SZ, 'scepter_lp'), (0x494a00, 0x495300)),
    0x4956f0: H('scepter_back_high_punch', 'armed back+HP', 'Clip 237, 15% damage; params 4ec480.', 'Strike setup then shared runner.', (SZ, 'scepter_back_hp'), (0x495710,)),
    0x495710: H('scepter_back_hp_direct', 'equipped scepter; follow-up context', 'Direct strike entry bypasses ordinary movement setup.', 'Shared strike runner.', (SZ, 'scepter_attack')),
    0x495740: H('scepter_back_low_punch', 'armed back+LP', 'Clip 228; reaction 151 freeze, zero direct damage; params 4ec4b8.', 'Shared strike runner.', (SZ, 'scepter_back_lp'), (0x486210,)),
    0x498290: H('throw_weapon_entry', 'equipped weapon identity and heading', 'Throw origin .75 along heading. Preserve actual weapon type through foreign pickup; scepter does not gain sword-only spin attacks.', 'Independent weapon projectile after release.', ('game/weapons.c', 'weapon_throw'), tables=('weapon_descriptor', 'weapon_bounces')),
    0x4941b0: H('weapon_settling', 'weapon object and bounce table', 'Three arcs, floor -0x1999, gravity 0x28f; each impact sound C[21]. Set final rest angle, then allow pickup.', 'Independent object continuation.', ('game/weapons.c', 'object_tick'), tables=('weapon_bounces',)),
    # Fatalities and shared presentation mechanics.
    0x467ce0: H('first_fatality_dispatch', 'roster-selected fatality entry, validated chord/range', 'Select 4e0498[roster]; Sub-Zero goes to 445100.', 'Scene entry dispatch.', (SZ, 'sub_zero_first_fatality'), (0x445100,)),
    0x467cd0: H('second_fatality_dispatch', 'roster-selected fatality entry and range', 'Select 4dfe90[roster]; Sub-Zero goes to 41b3a0.', 'Scene entry dispatch.', (SZ, 'sub_zero_second_fatality'), (0x41b3a0,)),
    0x445100: H('sub_zero_spine_fatality', 'owner/victim and scene alignment', 'Align 1.45 units; independent clips owner 385, victim 389/387; head/spine accessory 386. Camera starts at tick 8; actor playback after setup wait.', 'Independent actor, victim, camera and decoration clocks.', (SZ, 'first_fatality_script'), (0x445510, 0x445740, 0x445920, 0x445c40)),
    0x445510: H('spine_victim_pose_blends', 'victim clip 387', 'Blend to samples 46/49/50 using steps 0x666/0x43f/0x43f and initial weights 0x666/0x43f/0x886.', 'Victim clock independent of owner sample count.', ('game/scene.c', 'scene_step')),
    0x445740: H('detach_head_and_animate_spine', 'victim closed head, owner scene origin', 'Detach at owner sample 44 continuation; clip 386 samples 0..96 then loop 52. Spine uses five meshes 36..40, channel 0 reserved for head.', 'Independent accessory, not a head rigidly following hand pose.', ('game/presentation.c', 'battle_attach_piece_model'), (0x446620, 0x445c40), ('spine_rig',)),
    0x445920: H('spine_fatality_camera', 'scene reference and victim joint 3', 'First path step 0xccc, hold 50; orbit step 0x45a. Native coordinates are X,Z,Y. Eighty-tick target transition on second shot.', 'Independent camera clock; never actor sample indexed.', ('game/presentation.c', 'battle_camera_sequence'), related=(0x48b030,), tables=('camera_first', 'camera_orbit')),
    0x445c40: H('neck_blood_stream', 'victim joints 2 and 3', '203-tick schedule: 11 double sprays, 11 singles, 13 pairs with 2/1 waits, then 71 singles with 2-tick waits. Aim from joint 2 away from 3.', 'Independent emitter; speed 0x23d7 + random[0,0x7ae), jitter +/-0x7ae, gravity 0x51e.', ('game/presentation.c', 'battle_joint_spray'), (0x474330, 0x4745d0)),
    0x4456c0: H('spine_victim_collapse', 'victim finishing scene', 'Sound B[24] and camera shake selector 5. This helper does NOT emit a blood burst.', 'Enter victim cleanup script 4e0430.', (SZ, 'script_first_victim_collapse'), (0x457b50,), ('camera_shakes',)),
    0x457b50: H('camera_shake_start', 'selector in native scratch 53c43c', 'Start 457b90 using a row of 4e3638: pulse count, half-cycle wait, vertical offset.', 'Independent shared camera task.', ('game/presentation.c', 'battle_camera_shake'), (0x457b90,), ('camera_shakes',)),
    0x457b90: H('camera_shake_pulses', 'shake row selected by starter', 'Add Y offset; wait interval; subtract it; wait interval; decrement pulse count. Selector 5 = two pulses, three ticks, amplitude 0x2e14.', 'Does not alter camera look direction or accumulate drift.', ('game/presentation.c', 'battle_step_presentation'), tables=('camera_shakes',)),
    0x446620: H('spine_accessory_rig', 'detached head parent, clip 386', 'Rig 5145bc in sz_geo.geo; reserve head channel then five spine channels. Root Z offset 0x13b6; inverse parent orientation keeps spine hanging.', 'Independent accessory pose.', ('game/presentation.c', 'battle_attach_piece_model'), tables=('spine_rig',)),
    0x41b3a0: H('sub_zero_breath_fatality', 'owner/victim; validated range', 'Owner 552 repeated breath, independent victim 206. Current-range approach, uppercut/shatter, eight cursor passes, wait 60 and victory.', 'Independent owner and victim scene clocks.', (SZ, 'second_fatality_script'), (0x41b450, 0x41b610, 0x41b860, 0x41bbb0)),
    0x41b450: H('breath_fatality_approach', 'current owner/victim distance after setup', 'Wait 20, slide clip 95 through sample 8, then calculate approach delay from CURRENT range.', 'Do not use the command-entry range for this delay.', (SZ, 'script_breath_slide')),
    0x41b610: H('breath_fatality_shatter', 'victim frozen closed-surface rig and pose', 'Detach visible named and unnamed mesh pieces; skip invisible control root, then hide intact victim.', 'Independent launched pieces.', ('game/presentation.c', 'battle_shatter')),
    0x40d720: H('ice_trail_emit', 'ice object origin and shared sprite 339', 'Ten trail nodes, position spread +/-0x6666, Y velocity -0x7ae +/-0x28f, initial scale 0x4000; ten-tick decoration track.', 'Independent; 405430 always succeeds in this EXE, not a random probability gate.', ('game/presentation.c', 'battle_spray'), (0x40d920, 0x405430), ('ice_trail',)),
    0x40d920: H('ice_trail_shrink', 'trail scale', 'Subtract 0x51e scale per tick; retain independent position integration.', 'Presentation callback.', ('game/presentation.c', 'battle_step_presentation')),
    0x405430: H('always_success_gate', 'nominal probability argument (ignored by this EXE)', 'Set result flag 4 and result scratch zero unconditionally.', 'Immediate; do not manufacture RNG calls.'),
    0x474330: H('emit_directional_blood', 'joint-derived direction, origin and blood type', 'Normalize direction; speed 0x23d7 + random 0x7ae; jitter all velocity axes +/-0x7ae; shared blood mesh 0.', 'Independent blood object.', ('game/presentation.c', 'battle_spray')),
    0x4745d0: H('blood_fall_and_splash', 'live blood objects and floor contact', 'Gravity 0x51e; floor contact replaces drop with shared meshes 4..12, two ticks each plus final tick; floor Y -0x7ae.', 'Nineteen-tick splash decoration; impact sound rate-limited per tick.', ('game/presentation.c', 'battle_step_presentation'), tables=('blood_splash',)),
    0x433630: H('cpu_distance_policy', 'distance plan, difficulty, action changes and adaptive state', 'Use shared weighted action rows and strict per-thousand gates; distant opposing clone causes a bounded wait.', 'Shared simulation policy, not host input generation.', ('game/cpu.c', 'battle_cpu_step')),
    0x48fc20: H('finish_window_clock', 'defeat recovery, dizzy state, first contact', 'Preserve recovery before dizzy. Sixty-tick grace clears contact latch, then 240 ticks or first contact. Accepted entry can start on expiry tick.', 'Shared match phase clock.', ('game/battle.c', 'battle_step')),
}

# Related continuations already translated into the same phase-controlled C.
# Keep their local entry names distinct; do not imply they are independent moves.
for address, name, parent in (
    (0x445150, 'spine_first_pull', 0x445100), (0x445220, 'spine_pull_repeat', 0x445100),
    (0x445700, 'spine_victim_end', 0x445100), (0x41b080, 'breath_victim_end', 0x41b3a0),
    (0x41b440, 'breath_approach_transition', 0x41b3a0), (0x41b570, 'breath_uppercut_transition', 0x41b3a0),
    (0x41b860, 'breath_shatter_decoration', 0x41b3a0), (0x41bbb0, 'breath_scene_decoration', 0x41b3a0),
):
    base = HELPERS[parent]
    HELPERS[address] = H(name, base.inputs, 'Continuation within: ' + base.effects, base.flow, base.port, (parent,))

# Shared normal entry points select roster-owned clips/records. These links
# expose the already translated Sub-Zero decisions instead of researching the
# same selectors again; other rosters must supply their own data.
for address, name, function, effects in (
    (0x46dca0, 'jab_entry', 'sub_zero_jab', 'Shared jab selector; Sub-Zero clips 294/295, A[1], repeat/contact-selected recovery.'),
    (0x46a680, 'high_kick_entry', 'sub_zero_kick', 'Sub-Zero clip 71, B[2], sparse 29-step playback; contact at steps 11..13.'),
    (0x46a7b0, 'low_kick_entry', 'sub_zero_lk', 'Sub-Zero clip 70, avoidance samples 6..11, B[7] and contact-selected recovery.'),
    (0x46ef90, 'sweep_entry', 'enter_sweep', 'Sub-Zero clip 63 and B[35]; keep low-contact and recovery branches.'),
    (0x46ccd0, 'uppercut_entry', 'enter_uppercut', 'Sub-Zero clip 293; A[2]/A[6] branch, repeated contact and distinct hit/miss endings.'),
    (0x46f1f0, 'roundhouse_entry', 'sub_zero_roundhouse', 'Character-selected roundhouse; keep its own command and record.'),
    (0x46be40, 'close_hp_entry', 'sub_zero_hp', 'Sub-Zero close HP and follow-up tables; do not substitute ordinary jab.'),
    (0x46c2a0, 'combo_hp_entry', 'sub_zero_combo_hp', 'Sub-Zero clip 532 and its own follow-up contact.'),
    (0x46ba60, 'close_hk_entry', 'sub_zero_hk', 'Sub-Zero close HK and contact-enabled follow-up buttons.'),
    (0x46b7f0, 'crouch_lp_entry', 'sub_zero_crouch_lp', 'Character-owned crouching LP data; preserve contact-selected endings.'),
    (0x46b5a0, 'crouch_hk_entry', 'sub_zero_crouch_hk', 'Sub-Zero clip 52 launcher and its own victim reaction.'),
    (0x46b710, 'crouch_lk_entry', 'sub_zero_crouch_lk', 'Character-owned crouching LK and low-contact data.'),
    (0x46e6c0, 'air_hp_entry', 'air_hp', 'Sub-Zero air HP data; contact, missed landing and follow-up controls are separate branches.'),
    (0x46ee30, 'air_lp_entry', 'air_lp', 'Sub-Zero air LP data and contact/recovery branches.'),
    (0x46c7a0, 'air_straight_kick_entry', 'air_kick', 'Sub-Zero straight air kick; retain hit/low-contact follow-up selection.'),
    (0x46e380, 'air_diagonal_hk_entry', 'air_hk_diagonal', 'Sub-Zero diagonal HK data and landing branches.'),
    (0x46df50, 'air_diagonal_lk_entry', 'air_lk_diagonal', 'Sub-Zero diagonal LK data and landing branches.'),
    (0x467e00, 'fan_fatality_dispatch', 'sub_zero_fan_fatality', 'Shared fan scene; paired clips 415/416, 31 repetitions and stage-specific gate.'),
    (0x467e70, 'spike_fatality_dispatch', 'sub_zero_spike_fatality', 'Shared spike scene; reaction 175, impaled Y -0x59999 and timed victim cleanup.'),
):
    HELPERS[address] = H(name, 'active fighter and roster-owned move data', effects,
                         'Character adapter enters a resumable move; shared helpers own contact/landing.', (SZ, function))

HELPERS[0x498270] = H('throw_equipped_weapon', 'equipped weapon identity',
    'Dispatch through the actual equipped WeaponDef, including foreign pickups.',
    'Owner throw animation releases an independent weapon object.', ('game/weapons.c', 'weapon_throw'), (0x498290,))

# Shared leaves reached by the wrappers above, with native scratch inputs.
for address, parent, inputs in (
    (0x48a0d0, 0x4596e0, 'native argument selector in 53c42c; table 4ead30'),
    (0x48a230, 0x4595e0, 'native argument selector and current task roster'),
    (0x4292f0, 0x45ba40, 'target sample in 53c440; current clip/cursor'),
    (0x4906e0, 0x45b010, 'current fighter group'),
    (0x457cb0, 0x45b1b0, 'current fighter and player context'),
    (0x431520, 0x45abf0, 'native argument context selector and current group'),
):
    base = HELPERS[parent]
    HELPERS[address] = H(base.name + '_native', inputs, base.effects, base.flow, base.port, tables=base.tables)

HELPERS[0x41f290] = H('task_allocate', 'callback, type, inherited owner/context',
    'Allocate/link a cooperative task. Engine ports use explicit continuation state and bounded object pools instead of reproducing task slots.',
    'Independent continuation; never execute the callback as an ordinary inline call.', ('game/combat.c', 'battle_spawn_projectile'))
HELPERS[0x49cb00] = H('task_allocate_wrapper', 'callback in 53c40c; type in 53c434',
    'Pass the callback to 41f290. Opcode 40 does not forward its inline byte.',
    'Allocation wrapper, not callback execution.', related=(0x41f290,))
HELPERS[0x48d430] = H('contact_query_weapon', 'weapon selector and current fighter/object',
    'Read the weapon contact pointer table 53d0c0 and preserve hit/air/block reactions.',
    'Immediate contact, reaction continuation may replace victim state.', ('game/combat.c', 'battle_contact'))
HELPERS[0x48ce00] = H('select_frozen_cut_rig', 'roster asset slot and current pose',
    'Select roster +14 closed-surface rig for frozen bodies and cut pieces. Kitana uses Tanya cut geometry; preserve unnamed visible pieces.',
    'Presentation resource selection, not a new animation or combat origin.', ('game/pose.c', 'fighter_model'))
HELPERS[0x48b030] = H('camera_compact_spline', 'Q16 track parameter and four neighboring control points',
    'Kernel .5*(1-.75*d*d+.25*abs(d*d*d)); repeat initial camera for control indices <=2; retail track coordinates are X,Z,Y.',
    'Sample on the independent camera clock.', ('game/presentation.c', 'camera_sample'))
