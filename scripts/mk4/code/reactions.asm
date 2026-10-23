; Shared hit, block, fall, capture and get-up streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


guard_airborne_fall:
  sound_context 5
  sound_b 3
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  wait 2
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

; Catalog: jax.dash_punch (airborne victim 168).
jax_dash_punch_airborne_victim_168:
  effect 9
  sound_b 1
  spawn_effect 0
  sound_context 0
  effect 4
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(189)
  native_context 0
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4  ; non-executable guard between independent streams


react_kick_air:
  effect 4
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(189)
  native_context 0
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.flip_kick (reaction 160 victim).
quan_chi_flip_reaction:
  sound_b 3
  sound_context 11
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(189)
  native_context 0
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP

reaction_fall_return:
  motion 0, 0xfffff0a4, 0xffffd47b, 0x28f
  task_callback 0
  open_clip 0, clip(131)
  double_to 13
  forward_to 41
  nop 0x13ece8
  native_context 0
  forward_count 4
  prefixed_finish WITH_CLIP + 0, clip(185)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_VM_RETURN

impact_reaction:
  effect 12
  sound_context 11
  sound_b 3
  peer_motion 0, 0xfffff0a4, 0xffffd47b, 0x28f
  task_callback 0
  open_clip 0, clip(131)
  double_to 13
  forward_to 41
  nop 0x13ece8
  native_context 0
  forward_count 4
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


react_jab_air:
  effect 6
  sound_context 15
  motion 0, 0xfffff334, 0xffffdc29, 0x28f
  task_callback 0
  play_clip 30, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.weapon_hp (victim); kitana.weapon_hp (victim).
sonya_weapon_hp_victim:
  sound_c 8
  sound_b 1
  effect 5
  clip 0, clip(136)
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

; Catalog: kai.draw_weapon (HP victim).
kai_draw_weapon_hp_victim:
  sound_b 1
  effect 5
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.weapon_reaction (reaction 170 victim).
tanya_weapon_reaction_reaction_170_victim:
  effect 16
  sound_context 1
  jump 0, word(kai_draw_weapon_back_victim_spawn_effect_1)

; Catalog: reptile.weapon_reaction (reaction 96); kai.draw_weapon (back victim).
kai_draw_weapon_back_victim:
  sound_context 6
  effect 1

kai_draw_weapon_back_victim_spawn_effect_1:
  spawn_effect 0
  sound_b 1
  reaction_context 0
  motion_block 0, 0xfffff5c3, 0xccc, 0x0, 0x0
  handler 0, MK4_NATIVE_FACE_PEER_ORIENT_GROUP
  open_clip 0, clip(182)
  native_sound 0
  landing 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.weapon_reaction (knife victim).
jarek_weapon_reaction_knife_victim:
  effect 1
  spawn_effect 0
  sound_context 6
  sound_b 1
  speed_negative 0, 0xccc
  prefixed_finish WITH_CLIP + 0, clip(130)  ; bind clip prefix; inline sample/count 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


weapon_clip_189_launch_back_getup:
  effect 2
  sound_b 1
  sound_context 6
  motion 0, 0xfffff99a, 0xffffc000, 0x28f
  task_callback 0
  effect 0
  open_clip 0, clip(189)
  forward_to 9
  half_finish 0
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


reaction_clip_311_short_slide:
  effect 16
  sound_context 6
  speed_negative 0, 0xccc
  clip 0, clip(311)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.weapon_reaction (reaction 137).
reptile_weapon_reaction_reaction_137:
  effect 8
  effect 9
  sound_context 6
  speed_negative 0, 0xccc
  open_clip 0, clip(311)
  tail 0, MK4_NATIVE_REPTILE_WEAPON_STAGGER_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.weapon_reaction (reaction 144); cage.weapon_reaction (back HP victim).
reptile_weapon_reaction_reaction_144:
  effect 0
  sound_b 1
  sound_context 6
  motion 0, 0xfffff70b, 0xffffc000, 0x28f
  task_callback 0
  effect 0
  open_clip 0, clip(189)
  forward_to 9
  half_finish 0
  native_context 0
  forward_count 16
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.weapon_reaction (LP victim).
shinnok_weapon_reaction_lp_victim:
  effect 0
  sound_context 6
  sound_b 1
  speed_negative 0, 0xa3d
  open_clip 0, clip(132)
  double_to 14
  landing 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK

; Catalog: cage.weapon_reaction (LP victim).
cage_weapon_reaction_lp_victim:
  effect 0
  sound_context 6
  sound_b 1
  motion 0, 0xfffff852, 0xffffc7af, 0x28f
  task_callback 0
  clip 0, clip(189)
  native_context 0
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4  ; non-executable guard between independent streams


weapon_clip_189_launch_roll_getup:
  effect 0
  sound_context 6
  sound_b 1
  motion 0, 0xfffffc29, 0xffffc7af, 0x28f
  task_callback 0
  clip 0, clip(189)
  native_context 0
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4  ; non-executable guard between independent streams


reaction_clip_326_short_slide:
  effect 18
  sound_context 6
  speed_negative 0, 0xccc
  clip 0, clip(326)
  call 0, 0x433c00
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.special_punch (189 victim).
goro_special_punch_189_victim:
  effect 16
  spawn_effect 0
  sound_b 1
  sound_context 2
  reaction_context 0
  task_callback 0
  motion_block 0, 0x0, 0x1999, 0xffffdeb9, 0x20c
  play_clip 34, clip(319)
  native_context 0
  forward_count 8
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.staff_hp (weapon victim).
raiden_staff_hp_weapon_victim:
  spawn_effect 0
  sound_context 2
  effect 0
  sound_b 1
  speed_negative 0, 0xa3d
  open_clip 0, clip(132)
  double_to 14
  landing 0
  forward_count 10
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.special_uppercut (192 victim).
goro_special_uppercut_192_victim:
  effect 3
  effect 8
  effect 8
  sound_b 1
  sound_context 0
  motion 0, 0x0, 0xffffc7af, 0x20c
  tail 0, MK4_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.special_uppercut (192 victim slide).
goro_uppercut_victim_slide:
  sound_a 0
  effect 0
  play_clip 51, clip(181)
  native_context 0
  sound_a 23
  forward 0
  tail 0, MK4_NATIVE_GORO_VICTIM_GETUP
  .zero 4  ; non-executable guard between independent streams


launch_clip_131_back_getup:
  sound_context 6
  sound_b 1
  motion 0, 0xfffff334, 0xffffdc29, 0x20c
  task_callback 0
  play_clip 40, clip(131)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


reaction_clip_180_slide_hold:
  effect 8
  effect 2
  sound_context 6
  sound_b 1
  speed_negative 0, 0x3333
  bind_submit 0, clip(180)
  wait 6
  clip 0, clip(180)
  ready 0

launch_clip_131_native_finish_back_getup:
  effect 8
  effect 2
  sound_context 6
  sound_b 1
  motion 0, 0xfffff5c3, 0xffffdc29, 0x24d
  task_callback 0
  play_clip 40, clip(131)
  native_context 0
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.weapon_lp (victim); kai.draw_weapon (LP victim); kitana.weapon_lp (victim).
sonya_weapon_lp_victim:
  effect 0
  sound_b 1
  speed_negative 0, 0x11eb
  play_clip 14, clip(132)
  native_context 0
  sound_a 4
  landing 0
  forward_count 14
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK

weapon_tumble_back_getup:
  effect 8
  effect 2
  spawn_effect 6
  sound_b 1
  task_callback 0
  motion_block 0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0x1ca
  task_callback 0
  handler 0, MK4_NATIVE_LANDING_MOTION
  clip 0, clip(142)
  bind_cursor 0, clip(141)
  native_context 0
  clear_handler 0
  open_clip 0, clip(143)
  cursor_set 12
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


grounded_tumble_hold_return:
  effect 0
  reaction_context 0
  spawn_effect 0
  sound_context 6
  sound_b 1
  task_callback 0
  motion_block 0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0xde
  handler 0, MK4_NATIVE_LANDING_MOTION
  clip 0, clip(142)
  clip 0, clip(141)
  play_clip 12, clip(143)
  clear_handler 0
  forward 0
  wait 20
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL

tumble_native_finish_back_getup:
  reaction_context 0
  sound_c 8
  sound_b 1
  task_callback 0
  motion_block 0, 0xfffff5c3, 0xfffff5c3, 0xffffeb86, 0xde
  effect 2
  effect 8
  effect 8
  handler 0, MK4_NATIVE_LANDING_MOTION
  clip 0, clip(142)
  clip 0, clip(141)
  play_clip 12, clip(143)
  clear_handler 0
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


reaction_clip_132_native_finish:
  effect 0
  sound_context 6
  sound_b 1
  speed_negative 0, 0x11eb
  play_clip 14, clip(132)
  sound_a 4
  landing 0
  tail 0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


stagger_clip_311_hold:
  speed_negative 0, 0x2666
  sound_context 17
  sound_b 3
  bind_submit 4, clip(311)
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  wait 13
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.crouch_hk (victim); kitana.crouch_hk (victim).
sonya_crouch_hk_victim:
  speed_negative 0, 0x3333
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  clip 0, clip(134)
  ready 0
  .zero 4  ; non-executable guard between independent streams


stagger_clip_134_friction:
  speed_negative 0, 0x1eb8
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  clip 0, clip(134)
  ready 0
  .zero 4  ; non-executable guard between independent streams


stagger_clip_134_friction_impact:
  sound_b 9
  speed_negative 0, 0x1eb8
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  clip 0, clip(134)
  ready 0

crouch_stagger_clip_254:
  speed_negative 0, 0x1eb8
  sound_b 9
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  prefixed_finish WITH_CLIP + 0, clip(254)  ; bind clip prefix; inline sample/count 0
  wait 4
  playback_pair 0
  tail 0, MK4_NATIVE_CROUCH_RETURN
  .zero 4  ; non-executable guard between independent streams


rear_hit_launch_clip_129:
  sound_a 22
  motion 0, 0xffffd99a, 0xffffcccd, 0x418
  tail 0, MK4_NATIVE_REACTION_CLIP_129_STARTUP
  .zero 4  ; non-executable guard between independent streams


reaction_clip_129_startup:
  play_clip 34, clip(129)
  tail 0, MK4_NATIVE_REACTION_CLIP_129_WAIT_LANDING

reaction_clip_129_wait_landing:
  native_context 0
  sound_a 23
  landing 0

reaction_clip_129_wait_landing_tail_1:
  tail 0, MK4_NATIVE_REACTION_CLIP_129_GETUP
  nop 0
  forward 0
  wait 16
  call 0, MK4_NATIVE_FACE_PEER_ORIENT_GROUP
  tail 0, MK4_NATIVE_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


kick_hit_standing_stagger:
  effect 4
  sound_b 3
  sound_context 1
  speed_negative 0, 0x2666
  bind_submit 4, clip(311)
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  wait 13
  forward 0
  ready 0

kick_hit_crouching_stagger:
  no_guard 0
  effect 4
  spawn_effect 1
  sound_b 3
  bind_submit 4, clip(311)
  wait 13
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


stagger_clip_134_shake:
  no_guard 0
  speed_negative 0, 0x51e
  spawn_effect 1
  sound_context 3
  sound_b 2
  open_clip 0, clip(134)
  forward_count 6
  sound_indexed 10
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_reaction (standing stagger).
kai_handstand_reaction_standing_stagger:
  effect 10
  sound_b 2
  sound_context 3
  speed_negative 0, 0x2666
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  open_clip 0, clip(134)
  forward_count 6
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  playback_native 0
  wait 2
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_reaction (crouching stagger).
crouch_reaction_script:
  sound_a 20
  speed_negative 0, 0x2b85
  prefixed_to WITH_CLIP + 4, clip(254)  ; bind clip prefix; inline sample/count 4
  playback_pair 0
  landing 0
  tail 0, MK4_NATIVE_CROUCH_RETURN
  .zero 4  ; non-executable guard between independent streams


knockdown_stagger_clip_311:
  effect 4
  no_guard 0
  spawn_effect 1
  bind_submit 4, clip(311)
  wait 13
  forward 0
  ready 0

forward_fall_grounded:
  effect 11
  sound_b 3
  peer_motion 0, 0xffffe667, 0x0, 0x0
  clip 0, clip(186)
  sound_special 0
  landing 0
  tail 0, MK4_NATIVE_GETUP_FRONT

weapon_clip_132_directional_fall:
  sound_b 1
  effect 3
  effect 8
  effect 8
  airborne 0, word(weapon_clip_132_directional_fall_task_callback_1)
  peer_motion 0, 0xffffe667, 0x0, 0x0

weapon_clip_132_directional_fall_task_callback_1:
  task_callback 0
  peer_motion 0, 0xffffe667, 0x666, 0x147
  play_clip 13, clip(132)
  native_context 0
  tail 0, 0x482f90
  .zero 4  ; non-executable guard between independent streams


forward_fall_airborne:
  effect 10
  motion 0, 0xffffe667, 0x0, 0x28f
  play_clip 14, clip(186)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_FRONT
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.weapon_reaction (sweep victim).
shinnok_weapon_reaction_sweep_victim:
  sound_b 0
  reaction_context 0
  motion_block 0, 0xfffff0a4, 0xfffff334, 0x0, 0x0
  clip 0, clip(195)
  sound_special 0
  landing 0
  tail 0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.weapon_reaction (sweep getup).
shinnok_weapon_sweep_getup:
  down 0, word(shared_clip_entry)
  open_clip 0, clip(197)
  tail 0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH

sweep_victim_recovery:
  sound_b 0
  reaction_context 0
  motion_block 0, 0xfffff0a4, 0xccc, 0x0, 0x0
  play_clip 24, clip(192)
  sound_special 0
  landing 0
  forward 0
  down 0, word(sweep_victim_recovery_clip_1)
  open_clip 0, clip(194)
  tail 0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH

sweep_victim_recovery_clip_1:
  clip 0, clip(193)
  tail 0, MK4_NATIVE_CROUCH_RETURN

tumble_camera_side_positive:
  reaction_context 0
  sound_context 1
  spawn_effect 0
  sound_b 3
  effect 0
  task_callback 0
  motion_block 0, 0xfffff334, 0xccc, 0xffffeb86, 0xde
  handler 0, MK4_NATIVE_LANDING_MOTION
  clip 0, clip(142)
  clip 0, clip(141)
  play_clip 12, clip(143)
  clear_handler 0
  forward 0
  wait 8
  tail 0, MK4_NATIVE_GETUP_BACK

tumble_camera_side_negative:
  reaction_context 0
  sound_context 1
  spawn_effect 0
  sound_b 3
  effect 0
  task_callback 0
  motion_block 0, 0xfffff334, 0xfffff334, 0xffffeb86, 0xde
  handler 0, MK4_NATIVE_LANDING_MOTION
  clip 0, clip(142)
  clip 0, clip(141)
  play_clip 12, clip(143)
  clear_handler 0
  forward 0
  wait 8
  tail 0, MK4_NATIVE_GETUP_BACK

; Catalog: goro.quake_stomp (136 victim).
goro_quake_victim:
  sound_b 1
  speed_negative 0, 0x1999
  open_clip 0, clip(182)
  double_to 38
  landing 0
  double_to 50
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.input_47a690 (victim reaction 158).
reptile_dash_victim:
  sound_b 0
  motion 0, 0xa3d, 0xffffcf5d, 0x28f
  task_callback 0
  play_clip 39, clip(179)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP

; Catalog: reptile.reaction_40 (reaction continuation).
reptile_reaction_40:
  effect 17
  spawn_effect 0
  sound_context 1
  motion 0, 0xffffeb86, 0xffffe667, 0x24d
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

; Catalog: jarek.cannonball (victim knockdown).
jarek_ball_victim:
  sound_context 0
  spawn_effect 6
  motion 0, 0xffffe667, 0xffffe667, 0x24d

jarek_ball_victim_task_callback_1:
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


launch_clip_131_shake_impact:
  sound_context 2
  spawn_effect 6
  sound_b 3
  motion 0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0
  play_clip 40, clip(131)
  native_context 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


fall_reaction_effects:
  sound_context 6
  spawn_effect 0
  effect 8
  tail 0, MK4_NATIVE_REACTION_FALL
  .zero 4  ; non-executable guard between independent streams


reaction_fall:
  effect 12
  sound_context 0
  sound_b 3
  motion 0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0
  play_clip 40, clip(131)
  native_context 0
  tail 0, MK4_NATIVE_REACTION_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.cpu_kick (victim).
cage_shadow_victim:
  effect 17
  sound_b 3
  spawn_effect 0
  sound_context 0
  motion 0, 0xffffe667, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


uppercut_victim_recovery:
  sound_b 1
  spawn_effect 0
  sound_context 0
  effect 0
  effect 8
  motion 0, 0xfffff852, 0xffffcccd, 0x20c
  play_clip 53, clip(128)
  native_context 0
  sound_a 23
  sound_special 0
  tail 0, MK4_NATIVE_UPPERCUT_RECOVER

jab_hit_short_recovery:
  effect 6
  open_clip 0, clip(311)
  cursor_set 4
  forward_to 12
  prefixed_sample 14
  wait 1
  prefixed_sample 16
  wait 1
  prefixed_sample 18
  wait 1
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4  ; non-executable guard between independent streams


jab_hit_damage_stagger:
  speed_negative 0, 0x1999
  open_clip 0, clip(182)
  double_to 38
  landing 0
  double_to 50
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.breathe_fire (grounded victim); reiko.star (grounded victim); reptile.acid_spit (acid victim).
stagger_entry_2:
  sound_b 1
  speed_negative 0, 0x3333
  open_clip 0, clip(182)
  tail 0, MK4_NATIVE_STAGGER_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.weapon_steal (grounded victim).
quan_chi_weapon_steal_grounded_victim:
  speed_negative 0, 0xccc
  prefixed_to WITH_CLIP + 25, clip(182)  ; bind clip prefix; inline sample/count 25
  landing 0
  forward 0
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL

; Catalog: quan_chi.weapon_steal (airborne victim).
quan_chi_weapon_steal_airborne_victim:
  motion 0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  wait 2
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

; Catalog: quan_chi.teleport_stomp (reaction 106 victim).
quan_chi_stomp_reaction:
  sound_context 13
  sound_b 1
  prefixed_finish WITH_CLIP + 0, clip(178)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_QUAN_CHI_SCRIPT_STOMP_VICTIM

; Catalog: reiko.teleport (slam victim); quan_chi.air_throw (released victim); noob_saibot.teleport (slam victim) (also shared by other catalog entries).
slam_victim:
  speed_negative 0, 0xffffeb86
  sound_special 0
  prefixed_finish WITH_CLIP + 0, clip(175)  ; bind clip prefix; inline sample/count 0
  wait 8
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.air_throw (victim release); kitana.air_throw (victim release).
air_victim_flip:
  prefixed_sample WITH_CLIP + 0, clip(198)  ; bind clip prefix; inline sample/count 0
  motion 0, 0x0, 0x1999, 0x28f
  native_context 0
  call 0, MK4_NATIVE_TOUCHABLE
  call 0, MK4_NATIVE_DISARM_SELF
  sound_special 0
  prefixed_to WITH_CLIP + 12, clip(198)  ; bind clip prefix; inline sample/count 12
  speed_negative 0, 0xccc
  tail 0, MK4_NATIVE_AIR_FLIP_GETUP

; Catalog: jax.air_throw (victim release).
air_victim_lift:
  motion 0, 0xffffe667, 0xffffd99a, 0x28f
  task_callback 0
  prefixed_to WITH_CLIP + 35, clip(140)  ; bind clip prefix; inline sample/count 35
  native_context 0
  tail 0, MK4_NATIVE_AIR_LIFT_GETUP

; Catalog: scorpion.air_throw (victim release); raiden.air_throw (victim release); sub_zero.air_throw (victim release) (also shared by other catalog entries).
air_victim_default:
  motion 0, 0x0, 0x3333, 0x28f
  native_context 0
  sound_special 0
  forward 0
  wait 4
  tail 0, MK4_NATIVE_MMOVE_AIR_GETUP

; Catalog: scorpion.teleport_punch (grounded victim).
scorpion_teleport_punch_grounded_victim:
  effect 9
  effect 8
  spawn_effect 0
  sound_context 1
  sound_b 3
  speed_negative 0, 0x2666
  bind_submit 4, clip(311)
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.teleport_punch (victim fall).
scorpion_teleport_victim_tail:
  wait 13
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


teleport_punch_airborne_victim:
  effect 9
  effect 8
  sound_context 1
  sound_b 3
  motion 0, 0xfffff852, 0xffffdc29, 0x28f
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lk_throw (native continuation); jarek.lk_throw (native continuation); jax.lk_throw (native continuation).
victim_disarm_fall:
  forward_count 8
  speed_negative 0, 0x147a
  forward_count 23
  landing 0
  forward 0
  tail 0, MK4_NATIVE_THROW_VICTIM_STOP_AND_RETURN

; Catalog: quan_chi.lk_throw (native continuation).
victim_friction_impact:
  prefixed_finish WITH_CLIP + 0, clip(168)  ; bind clip prefix; inline sample/count 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.lk_throw (native continuation).
victim_saved_motion:
  spawn_effect 0
  prefixed_to WITH_CLIP + 11, clip(177)  ; bind clip prefix; inline sample/count 11
  call 0, MK4_NATIVE_RESTORE_MOTION
  forward_to 34
  sound_special 0
  landing 0
  forward 0
  wait 6
  tail 0, MK4_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN

; Catalog: fujin.lk_throw (native continuation).
victim_ground_turn:
  speed_negative 0, 0x2666
  spawn_effect 6
  sound_a 3
  prefixed_finish WITH_CLIP + 0, clip(170)  ; bind clip prefix; inline sample/count 0
  call 0, MK4_NATIVE_IMPACT_VOICE
  prefixed_finish WITH_CLIP + 0, clip(138)  ; bind clip prefix; inline sample/count 0
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  prefixed_finish WITH_CLIP + 0, clip(139)  ; bind clip prefix; inline sample/count 0
  wait 10
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.leg_grab (victim recovery); kitana.leg_grab (victim recovery).
sonya_leg_grab_victim_recovery:
  motion 0, 0x1c28, 0xffffe667, 0x24d
  sound_special 0
  task_callback 0
  forward 0
  tail 0, MK4_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.lk_throw (native continuation).
liu_kang_lk_throw_victim:
  sound_a 37
  forward_to 35
  loop 3
  playback_pair 31
  forward_to 35
  repeat 0
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.torpedo (ground release); liu_kang.bicycle_kick (released ground victim).
raiden_torpedo_ground_release:
  speed_negative 0, 0x4ccc
  clip 0, clip(182)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.torpedo (air release); liu_kang.bicycle_kick (released air victim).
raiden_torpedo_air_release:
  motion 0, 0xfffff334, 0xfffff0a4, 0x312
  task_callback 0
  clip 0, clip(183)
  native_context 0
  forward 0
  wait 2
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

lp_victim_slide_release:
  prefixed_to WITH_CLIP + 5, clip(171)  ; bind clip prefix; inline sample/count 5
  speed_negative 0, 0x1999
  forward_to 24
  landing 0
  forward 0
  tail 0, 0x485a70
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lp_throw (extended victim); reiko.lp_throw (throw victim recovery); noob_saibot.lp_throw (throw victim recovery).
lp_victim_sliding_impact_release:
  speed_negative 0, 0x2666
  prefixed_to WITH_CLIP + 12, clip(172)  ; bind clip prefix; inline sample/count 12
  sound_special 0
  forward_count 6
  landing 0
  forward 0
  tail 0, MK4_NATIVE_LP_SLIDING_IMPACT_FLIP_GETUP

; Catalog: liu_kang.lp_throw (victim release).
lp_victim_impact_release:
  prefixed_to WITH_CLIP + 17, clip(174)  ; bind clip prefix; inline sample/count 17
  sound_special 0
  forward 0
  landing 0
  tail 0, MK4_NATIVE_GETUP_ROLL_FLIP
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.lp_throw (victim release); kitana.lp_throw (victim release).
lp_victim_two_speed_slide_release:
  prefixed_to WITH_CLIP + 4, clip(176)  ; bind clip prefix; inline sample/count 4
  speed_negative 0, 0x3333
  forward_count 8
  speed_negative 0, 0x1999
  forward_count 8
  landing 0
  forward 0
  tail 0, MK4_NATIVE_LP_TWO_SPEED_SLIDE_DOUBLE_FLIP_ROLL_GETUP
  .zero 4  ; non-executable guard between independent streams


hit_clip_324_recovery:
  effect 9
  sound_a 20
  clip 0, clip(324)
  ready 0
  .zero 4  ; non-executable guard between independent streams


hit_clip_325_recovery:
  effect 9
  sound_a 20
  clip 0, clip(325)
  ready 0
  .zero 4  ; non-executable guard between independent streams


launch_clip_131_no_impact:
  motion 0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0
  play_clip 40, clip(131)
  native_context 0
  forward 0
  ready 0

grounded_hit_clip_132_back_getup:
  sound_b 1
  speed_negative 0, 0xa3d
  open_clip 0, clip(132)
  double_to 14
  landing 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK

launch_clip_179_flip_getup:
  sound_b 3
  motion 0, 0x51e, 0xffffb334, 0x353
  task_callback 0
  play_clip 39, clip(179)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP

; Catalog: quan_chi.skull (reaction 128 victim).
quan_chi_skull_reaction:
  sound_c 47
  sound_b 3
  speed_negative 0, 0x11eb
  open_clip 0, clip(182)
  native_sound 0
  landing 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.spear (victim hit).
scorpion_spear_victim_hit:
  effect 5
  sound_b 3
  sound_context 10
  play_clip 5, clip(199)
  tail 0, MK4_NATIVE_SHAKE_STUN
  .zero 4  ; non-executable guard between independent streams


guard_grounded_stagger:
  speed_negative 0, 0xccc
  prefixed_finish WITH_CLIP + 0, clip(182)  ; bind clip prefix; inline sample/count 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


airborne_thaw_script:
  gravity 0, 0x147
  native_context 0
  tail 0, MK4_NATIVE_THAW_LANDING
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lightning (victim).
raiden_lightning_victim:
  sound_a 45
  speed_negative 0, 0x5999
  play_clip 27, clip(127)
  clear_handler 0
  speed_negative 0, 0xffffeb86
  forward_count 5
  landing 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


reaction_clip_127_friction:
  sound_a 38
  speed_negative 0, 0x5999
  play_clip 27, clip(127)
  clear_handler 0
  speed_negative 0, 0xffffeb86
  forward_count 5
  landing 0
  forward 0
  ready 0

; Catalog: tanya.weapon_reaction (reaction 169 victim).
tanya_weapon_reaction_reaction_169_victim:
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  effect 4
  spawn_effect 0
  sound_context 1
  sound_b 1
  speed_negative 0, 0x4000
  clip 0, clip(130)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.ring (victim); liu_kang.fireball_reaction (native C0 victim); kitana.ring (victim).
sonya_ring_victim:
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  sound_b 1
  speed_negative 0, 0x4000
  clip 0, clip(130)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.crossbow_reaction (bolt victim).
fujin_bolt_victim:
  speed_negative 0, 0x2666
  bind_submit 4, clip(311)
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  wait 4
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.dash_punch (grounded victim 168).
jax_dash_punch_grounded_victim_168:
  effect 9
  sound_b 1
  speed_negative 0, 0x6666
  clip 0, clip(130)
  ready 0
  .zero 4  ; non-executable guard between independent streams


block_unarmed_pose_cycle:
  bind_submit 3, clip(321)
  wait 1
  bind_submit 5, clip(321)
  wait 1
  bind_submit 7, clip(321)
  wait 1
  bind_submit 9, clip(321)
  wait 1
  bind_submit 7, clip(321)
  wait 1
  bind_submit 5, clip(321)
  wait 1
  bind_submit 3, clip(321)
  wait 1
  tail 0, MK4_NATIVE_BLOCK_CYCLE
  .zero 4  ; non-executable guard between independent streams


block_armed_recovery:
  prefixed_finish WITH_CLIP + 0, clip(247)  ; bind clip prefix; inline sample/count 0
  playback_pair 0
  landing 0
  tail 0, MK4_NATIVE_STANDING_BLOCK_HOLD

; Catalog: sub_zero.lk_throw (native continuation); noob_saibot.lk_throw (native continuation).
lk_thrown_release:
  cursor_set 45
  playback_pair 41
  forward_to 46
  playback_pair 41
  forward_to 46
  playback_pair 41
  forward_to 46
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_VICTIM_RISE
  .zero 4  ; non-executable guard between independent streams


snare_reaction:
  open_clip 6, clip(199)

snare_reaction_forward_to_1:
  forward_to 11
  playback_pair 7
  jump 0, word(snare_reaction_forward_to_1)

; Catalog: scorpion.spear (victim stun).
spear_arrival:
  bind_cursor 0, clip(13)
  tail 0, MK4_NATIVE_SCORPION_SPEAR_STUN_SCRIPT
  .zero 4  ; non-executable guard between independent streams


pair_release:
  motion 0, 0xfffffae2, 0x0, 0x28f
  task_callback 0
  prefixed_finish WITH_CLIP + 0, clip(183)  ; bind clip prefix; inline sample/count 0
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.groin_punch (victim stun).
groin_punch_groin_victim:
  speed_negative 0, 0x28f5
  spawn_effect 0
  sound_context 18
  sound_b 3
  prefixed_to WITH_CLIP + 14, clip(126)  ; bind clip prefix; inline sample/count 14
  playback_pair 10
  forward_to 14
  playback_pair 10
  forward_to 14
  playback_pair 10
  forward_to 14
  ready 0

; Catalog: fujin.wind_grab (victim release).
orbit_release:
  motion 0, 0xffffe667, 0xffffd99a, 0x353
  task_callback 0
  prefixed_finish WITH_CLIP + 0, clip(183)  ; bind clip prefix; inline sample/count 0
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.wind_victim (vortex landing).
vortex_landed:
  sound_special 0
  jump 0, word(shared_clip_entry)
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.wind_slam (victim bounce).
slam_landed:
  sound_special 0
  wait 4
  motion 0, 0x11eb, 0xffffcf5d, 0x28f
  task_callback 0
  play_clip 39, clip(179)
  native_context 0
  forward 0
  tail 0, MK4_NATIVE_GETUP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.dash_punch (hit victim 157).
jax_dash_victim_hit:
  effect 9
  sound_context 1
  prefixed_finish WITH_CLIP + 0, clip(311)  ; bind clip prefix; inline sample/count 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.triple_kick (first victim); reiko.axe_back_lp (first victim); jax.axe_back_lp (first victim) (also shared by other catalog entries).
cage_power_victim:
  sound_context 3
  play_clip 27, clip(127)
  ready 0

; Catalog: fujin.triple_kick (middle victim).
fujin_triple_victim:
  effect 10
  sound_context 3
  bind_submit 4, clip(311)
  wait 13
  ready 0

; Catalog: raiden.staff_hp (staff victim).
raiden_staff_hp_staff_victim:
  sound_b 1
  sound_context 2
  prefixed_to WITH_CLIP + 5, clip(205)  ; bind clip prefix; inline sample/count 5
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.cartwheel (victim); fujin.triple_kick (final victim); reiko.axe_back_lp (follow-up victim) (also shared by other catalog entries).
sonya_cartwheel_victim:
  sound_context 1
  sound_b 3
  motion 0, 0xffffe667, 0xffffd47b, 0x28f
  task_callback 0
  open_clip 0, clip(131)
  double_to 13
  forward_to 41
  nop 0x13ece8
  native_context 0
  forward_count 4
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP

bicycle_capture:
  clip 0, clip(136)
  tail 0, MK4_NATIVE_BICYCLE_CYCLE

; Catalog: raiden.torpedo (held victim).
raiden_torpedo_contact:
  effect 19
  handler 0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION
  prefixed_finish WITH_CLIP + 0, clip(135)  ; bind clip prefix; inline sample/count 0

raiden_torpedo_contact_playback_pair_1:
  playback_pair 4
  forward_to 9
  jump 0, word(raiden_torpedo_contact_playback_pair_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.bicycle (victim); kitana.bicycle (victim).
sonya_bicycle_victim:
  effect 8
  effect 9
  sound_context 2
  spawn_effect 0
  task_callback 0
  motion 0, 0xffffe667, 0x666, 0x147
  clip 0, clip(186)
  native_context 0
  sound_special 0
  landing 0
  tail 0, MK4_NATIVE_GETUP_FRONT

; Catalog: kai.handstand_reaction (knockdown).
kai_handstand_reaction_knockdown:
  effect 4
  sound_b 3
  sound_context 1
  spawn_effect 0
  motion 0, 0xffffe667, 0xffffd47b, 0x28f
  task_callback 0
  open_clip 0, clip(131)
  double_to 13
  forward_to 41
  nop 0x13ece8
  native_context 0
  forward_count 4
  tail 0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_reaction (rear stagger).
kai_handstand_reaction_rear_stagger:
  effect 10
  sound_b 2
  sound_context 3
  speed_negative 0, 0x3333
  open_clip 0, clip(127)
  forward_count 20
  tail 0, MK4_NATIVE_KAI_HANDSTAND_VICTIM_TAIL

mmove_side_getup:
  open_clip 0, clip(131)
  cursor_set 45
  forward 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_2 (victim reaction 178).
kai_second_victim:
  effect 0
  effect 8
  sound_c 105
  spawn_effect 6
  sound_b 8
  sound_c 52
  prefixed_to WITH_CLIP + 10, clip(130)  ; bind clip prefix; inline sample/count 10
  half_finish 0
  native_release 47
  tail 0, MK4_NATIVE_KAI_SECOND_VICTIM_FALL

; Catalog: shinnok.fatality_2 (victim).
shinnok_second_victim:
  prefixed_finish WITH_CLIP + 0, clip(542)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(543)
  wait 50
  sound_b 8
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.fatality_2 (victim melt).
reptile_fatality_2_victim_melt:
  bind_cursor 0, clip(546)
  speed_negative 0, 0x28f
  tail 0, MK4_NATIVE_REPTILE_SECOND_MELT

; Catalog: fujin.fatality_2 (victim fall); reptile.fatality_2 (victim fall).
fujin_fatality_2_victim_fall:
  prefixed_to WITH_CLIP + 40, clip(550)  ; bind clip prefix; inline sample/count 40
  sound_special 0
  forward 0
  wait 10
  face 0
  scene_event 63
  tail 0, MK4_NATIVE_SCENE_BODY_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.fatality_2 (victim wind hold).
fujin_second_victim:
  speed_negative 0, 0x28f
  bind_cursor 0, clip(549)
  wait 90
  sound_b 8
  tail 0, MK4_NATIVE_FUJIN_SECOND_BURST

reaction_park_task:
  park 0
  .zero 4  ; non-executable guard between independent streams


stage_victim:
  spawn_effect 0
  sound_context 0
  motion_block 0, 0xfffffd71, 0x28f5, 0xffffcccd, 0x0
  open_clip 0, clip(128)
  double_to 31
  tail 0, MK4_NATIVE_STAGE_IMPALE

stage_impaled:
  landing 0
  effect 8
  effect 8
  effect 20
  sound_context 7
  spawn_effect 6
  sound_b 12
  wait 128
  native_release 41
  wait 50
  tail 0, MK4_NATIVE_STAGE_COMPLETE
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.lk_throw (native continuation).
victim_disarm_raise:
  speed_negative 0, 0x1c28
  sound_b 1
  spawn_effect 0
  sound_context 0
  open_clip 0, clip(536)
  half_count 7
  tail 0, MK4_NATIVE_THROW_VICTIM_CONTINUE

; Catalog: cage.lk_throw (native continuation).
throw_victim_continue:
  forward_to 14
  landing 0
  tail 0, MK4_NATIVE_THROW_VICTIM_IMPACT

; Catalog: cage.lk_throw (native continuation).
throw_victim_impact:
  sound_special 0
  native_sound 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.special_kick (190 victim).
goro_kick_victim:
  sound_context 0
  spawn_effect 6
  motion 0, 0xffffe148, 0xffffd47b, 0x24d
  jump 0, word(jarek_ball_victim_task_callback_1)

; Catalog: goro.leap_stomp (191 victim).
goro_stomp_victim:
  sound_context 13
  sound_b 1
  prefixed_finish WITH_CLIP + 0, clip(178)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD
  .zero 4  ; non-executable guard between independent streams


shared_clip_entry:
  clip 0, clip(196)
  tail 0, MK4_NATIVE_CROUCH_RETURN
  .zero 4  ; non-executable guard
