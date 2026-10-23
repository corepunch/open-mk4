; Special attack startup, contact branches and recovery.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: cage.cpu_kick (attack).
cage_cpu_kick:
  speed 0, 0x2666
  sound_c 81
  open_clip 0, clip(308)
  double_to 22
  tail 0, MK4_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.powered_punch (attack).
cage_powered_punch:
  sound_c 81
  open_clip 0, clip(85)
  double_to 14
  speed 0, 0x2b85
  double_to 24
  tail 0, MK4_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.flip_kick (startup).
quan_chi_flip_kick:
  sound_c 86
  open_clip 0, clip(105)
  prefixed_to 10
  speed 0, 0x547a
  forward_to 15
  tail 0, MK4_NATIVE_QUAN_CHI_SCRIPT_FLIP_KICK_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.input_47a690 (selected stream).
reptile_dash:
  prefixed_finish WITH_CLIP + 0, clip(302)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(303)
  speed 0, 0x2e14
  tail 0, MK4_NATIVE_REPTILE_SCRIPT_DASH_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.dash_punch (dash).
jax_dash_punch:
  sound_indexed 37
  sound_c 78
  speed 0, 0x2666
  open_clip 0, clip(299)
  double_to 18
  tail 0, MK4_NATIVE_JAX_DASH_PUNCH_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.input_47afe0 (selected stream).
reptile_roll:
  sound_indexed 35
  sound_c 70
  speed 0, 0x1eb8
  prefixed_to WITH_CLIP + 13, clip(106)  ; bind clip prefix; inline sample/count 13
  query_d 21
  forward_to 17
  landing 0
  tail 0, MK4_NATIVE_REPTILE_SCRIPT_ROLL_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.quake (stomp).
jarek_quake:
  sound_indexed 33
  prefixed_to WITH_CLIP + 20, clip(107)  ; bind clip prefix; inline sample/count 20
  sound_c 68
  spawn_effect 3
  tail 0, MK4_NATIVE_JAREK_QUAKE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.quake (recovery).
jarek_quake_tail:
  forward 0
  ready 0

; Catalog: sonya.cartwheel (startup); kitana.cartwheel (startup).
sonya_cartwheel:
  sound_indexed 32
  sound_c 6
  speed 0, 0x1999
  prefixed_to WITH_CLIP + 14, clip(104)  ; bind clip prefix; inline sample/count 14
  tail 0, MK4_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL

; Catalog: tanya.drill_kick (attacker).
tanya_drill_kick:
  sound_indexed 31
  speed 0, 0x23d7
  prefixed_to WITH_CLIP + 12, clip(103)  ; bind clip prefix; inline sample/count 12
  tail 0, MK4_NATIVE_TANYA_DRILL_KICK_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_47bd10 (selected continuation).
kai_aerial_punch:
  sound_c 60
  motion 0, 0x2e14, 0xffffe667, 0x41
  prefixed_to WITH_CLIP + 9, clip(298)  ; bind clip prefix; inline sample/count 9
  tail 0, MK4_NATIVE_KAI_AERIAL_PUNCH_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.aerial_dive (startup).
fujin_dive:
  sound_c 57
  motion 0, 0x1eb8, 0xf5c, 0x41
  prefixed_finish WITH_CLIP + 0, clip(58)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_FUJIN_DIVE_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.aerial_dive (recoil).
dive_recoil:
  landing 0
  wait 8
  allow_commands 0
  gravity 0, 0x51e
  native_context 0
  tail 0, MK4_NATIVE_FUJIN_DIVE_LAND

; Catalog: reiko.teleport (rise); noob_saibot.teleport (rise).
teleport:
  sound_c 54
  prefixed_to WITH_CLIP + 3, clip(86)  ; bind clip prefix; inline sample/count 3
  motion 0, 0x0, 0xffffcccd, 0x0
  forward 0
  tail 0, MK4_NATIVE_TELEPORT_VANISH

; Catalog: reiko.teleport (slam opener); noob_saibot.teleport (slam opener).
teleport_slam:
  sound_indexed 24
  native_release 23
  landing 0
  open_peer_clip 0, clip(150)
  open_clip 0, clip(74)
  playback_single 18
  tail 0, MK4_NATIVE_TELEPORT_SLAM_FALL
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.teleport (slam fall); noob_saibot.teleport (slam fall).
teleport_slam_fall:
  playback_single 27
  tail 0, MK4_NATIVE_TELEPORT_SLAM_DROP
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_47c520 (selected continuation).
kai_handstand_enter:
  sound_indexed 23
  prefixed_finish WITH_CLIP + 0, clip(1)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_spin (spin startup).
handstand_spin:
  prefixed_finish WITH_CLIP + 0, clip(2)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(3)
  tail 0, MK4_NATIVE_KAI_HANDSTAND_SPIN_BEGIN
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_47cc10 (selected continuation).
handstand_front:
  sound_a 1
  prefixed_to WITH_CLIP + 9, clip(0)  ; bind clip prefix; inline sample/count 9
  tail 0, MK4_NATIVE_KAI_SCRIPT_CONTACT_B10_CONTINUATION
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_47cc10 (scheduler continuation).
kai_contact_b10_continuation:
  wait 8
  playback_pair 0
  tail 0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT

; Catalog: kai.handstand_rear (rear startup).
handstand_rear:
  sound_a 1
  prefixed_to WITH_CLIP + 9, clip(6)  ; bind clip prefix; inline sample/count 9
  tail 0, MK4_NATIVE_KAI_HANDSTAND_REAR_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_rear (rear recovery).
kai_handstand_rear_contact:
  playback_pair 0
  tail 0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.handstand_exit (reverse intro).
handstand_exit:
  sound_indexed 23
  clip_operation 0, clip(1)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.torpedo (fast startup).
raiden_torpedo_fast_startup:
  open_clip 0, clip(94)
  playback_tail 14
  jump 0, word(raiden_torpedo_ordinary_startup_call_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.torpedo (ordinary startup).
raiden_torpedo_ordinary_startup:
  open_clip 0, clip(94)
  double_to 14

raiden_torpedo_ordinary_startup_call_1:
  call 0, MK4_NATIVE_RESTORE_MOTION
  handler 0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MOTION
  sound_a 43
  sound_c 11
  loop 29
  predicate_argument 0, MK4_NATIVE_CONTACT_QUERY_D, word(shared_tail_entry_3)
  call 0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP
  wait 1
  repeat 0
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.spin_kick (blocked rewind); jarek.cannonball (contact rewind); jarek.upward_ball (contact rewind) (also shared by other catalog entries).
power_rewind:
  clip_operation 0, clip(301)
  native_context 0
  clear_handler 0
  tail 0, MK4_NATIVE_RECOIL_LAND
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.slide (entry).
sub_zero_slide:
  speed_negative 0, 0xffffd99a
  sound_c 15
  open_clip 0, clip(95)
  tail 0, MK4_NATIVE_SUB_ZERO_SLIDE_SCRIPT
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.slide (hit).
sub_zero_slide_hit:
  loop 4
  playback_tail 10
  wait 1
  playback_tail 9
  wait 1
  playback_tail 8
  wait 1
  playback_tail 9
  wait 1
  repeat 0
  landing 0
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.slide (block).
sub_zero_slide_block:
  no_guard 0
  landing 0
  loop 6
  playback_tail 10
  wait 1
  playback_tail 9
  wait 1
  playback_tail 8
  wait 1
  playback_tail 9
  wait 1
  repeat 0
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.leg_grab (paired grab); kitana.leg_grab (paired grab).
sonya_leg_grab_paired_grab:
  native_release 37
  sound_roster 1
  open_peer_clip 0, clip(201)
  playback_single 22
  playback_pair_reverse 19
  playback_single 22
  playback_pair_reverse 19
  playback_single 34
  native_step 79
  tail 0, MK4_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL

; Catalog: sonya.leg_grab (miss or block); kitana.leg_grab (miss or block).
sonya_leg_grab_miss_or_block:
  forward_to 22
  playback_pair 19
  forward_to 22
  playback_pair 19
  forward 0
  ready 0

; Catalog: sonya.square_wave (launch); kitana.square_wave (launch).
sonya_square_wave:
  speed 0, 0x1999
  prefixed_finish WITH_CLIP + 0, clip(72)  ; bind clip prefix; inline sample/count 0
  sound_indexed 14
  bind_cursor 0, clip(73)
  motion_block 0, 0x1999, 0x0, 0xffffe667, 0xffffff7d
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.bicycle_kick (startup).
liu_kang_bicycle_kick:
  motion 0, 0x2147, 0xfffff5c3, 0x0
  call 0, MK4_NATIVE_SAVE_MOTION
  play_clip 6, clip(68)
  handler 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION
  sound_indexed 1
  call 0, MK4_NATIVE_RESTORE_MOTION
  gravity 0, 0xc4
  tail 0, MK4_NATIVE_LIU_KANG_BICYCLE_SCRIPT
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.bicycle_kick (finish).
bicycle_land:
  clear_handler 0
  clip_alt 6, clip(68)
  ready 0

; Catalog: liu_kang.bicycle_kick (blocked).
bicycle_contact_end:
  motion 0, 0x0, 0xffffeb86, 0xe5
  native_context 0
  clear_handler 0
  tail 0, MK4_NATIVE_LIU_KANG_BICYCLE_LAND

; Catalog: liu_kang.flying_kick (launch).
liu_kang_flying_kick:
  sound_indexed 0
  open_clip 0, clip(92)
  motion 0, 0x23d7, 0xffffe8f6, 0x106
  tail 0, MK4_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.flying_kick (miss).
liu_kang_flying_kick_miss:
  clip_operation 0, clip(92)
  bind_submit 0, clip(92)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.flying_kick (hit).
liu_kang_flying_kick_hit:
  landing 0
  speed_negative 0, 0xfffffae2
  clip 0, clip(93)
  wait 20
  gravity 0, 0x51e
  clip_operation 0, clip(92)
  native_context 0
  ready 0

; Catalog: liu_kang.flying_kick (block).
liu_kang_flying_kick_block:
  landing 0
  wait 12
  landing 0
  wait 12
  gravity 0, 0x28f
  native_context 0
  tail 0, MK4_NATIVE_LIU_KANG_FLYING_KICK_LAND
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.triple_kick (startup).
fujin_triple_kick:
  fast_start 0, word(fujin_triple_kick_tail_1)
  prefixed_to WITH_CLIP + 6, clip(100)  ; bind clip prefix; inline sample/count 6
  speed 0, 0x2b85
  forward_to 13
  tail 0, MK4_NATIVE_FUJIN_TRIPLE_CONTACT

fujin_triple_kick_tail_1:
  tail 0, MK4_NATIVE_FUJIN_TRIPLE_FAST
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.triple_kick (fast startup).
fujin_triple_fast:
  open_clip 0, clip(100)
  double_to 6
  speed 0, 0x2b85
  double_to 13
  tail 0, MK4_NATIVE_FUJIN_TRIPLE_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.teleport_punch (attacker).
scorpion_teleport:
  motion 0, 0xffffd99a, 0xffffe8f6, 0x1ca
  open_clip 0, clip(312)
  sound_c 35
  handler 0, MK4_NATIVE_TELEPORT_GROUP_CALLBACK
  tail 0, MK4_NATIVE_SCORPION_TELEPORT_SCRIPT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.teleport_punch (landing); reiko.teleport (blocked landing); quan_chi.teleport_stomp (blocked landing) (also shared by other catalog entries).
teleport_block_return:
  clip_operation 0, clip(301)
  native_context 0
  clear_handler 0
  tail 0, MK4_NATIVE_LAND

; Catalog: fujin.wind_grab (opening).
fujin_wind_grab:
  sound_indexed 30
  sound_c 61
  prefixed_finish WITH_CLIP + 0, clip(10)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(11)
  tail 0, MK4_NATIVE_FUJIN_GRAB_CONTROL_START

; Catalog: scorpion.sword_spin (startup).
sword_spin_script:
  prefixed_finish WITH_CLIP + 0, clip(223)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(224)
  tail 0, MK4_NATIVE_SCORPION_SPIN_SCRIPT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_478df0 (selected continuation).
kai_lunge_kick:
  sound_indexed 59
  open_clip 0, clip(551)
  fast_start 0, word(kai_armed_handstand_enter_cursor_set_1)

kai_lunge_kick_double_to_1:
  double_to 26
  sound_c 31
  speed 0, 0x2666
  forward_to 29
  tail 0, MK4_NATIVE_KAI_LUNGE_KICK_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.input_47cbf0 (selected continuation).
kai_armed_handstand_enter:
  sound_indexed 23
  open_clip 0, clip(1)
  native_sound 0
  tail 0, MK4_NATIVE_HANDSTAND_FRONT

kai_armed_handstand_enter_cursor_set_1:
  cursor_set 14
  jump 0, word(kai_lunge_kick_double_to_1)
  .zero 4  ; non-executable guard between independent streams


shared_tail_entry_3:
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT
  .zero 4  ; non-executable guard
