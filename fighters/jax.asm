; Generated jax bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ native_jax_fatality_2_sparks 0x412d80
.export_equ native_jax_fatality_2_flash 0x412ec0
.export_equ native_jax_lp_throw_throw_continuation 0x42c7f0
.export_equ native_jax_lp_slam_first 0x42c820
.export_equ native_jax_lp_slam_second 0x42c8c0
.export_equ native_jax_lp_slam_third 0x42c8f0
.export_equ native_jax_lp_throw_return 0x42c9f0
.export_equ native_jax_fatality_1 0x4494c0
.export_equ native_jax_fatality_1_victory 0x449620
.export_equ native_jax_fatality_1_grab 0x449890
.export_equ native_jax_fatality_1_first_arm 0x4498d0
.export_equ native_jax_fatality_1_second_arm 0x449940
.export_equ native_jax_can_fatality_1 0x45e1b0
.export_equ native_jax_can_missile 0x45e7e0
.export_equ native_jax_can_dash_punch 0x45e910
.export_equ native_jax_air_throw_command_gate 0x45ed70
.export_equ native_jax_fatality_2 0x4669c0
.export_equ native_jax_fatality_2_approach 0x466a90
.export_equ native_jax_fatality_2_head 0x466b40
.export_equ native_jax_fatality_2_finish 0x466bb0
.export_equ native_jax_fatality_2_victim_pause 0x466df0
.export_equ native_jax_ground_pound 0x468f00
.export_equ native_jax_high_kick_contact_recovery 0x46a770
.export_equ native_jax_combo_hp_entry 0x46c290
.export_equ native_jax_ground_pound_controller_return 0x46f650
.export_equ native_jax_dash_punch 0x47ab10
.export_equ native_jax_dash_punch_contact 0x47ab50
.export_equ native_jax_dash_victim_throw 0x4867b0
.export_equ native_jax_dash_victim_hit 0x4883f0
.export_equ native_jax_dash_punch_roll_getup 0x488c00
.export_equ native_jax_axe_hp_entry 0x496cd0
.export_equ native_jax_axe_back_hp_entry 0x496ce0
.export_equ native_jax_axe_lp_entry 0x496cf0
.export_equ native_jax_axe_back_lp_entry 0x496d10
.export_equ native_jax_missile 0x496f80
.export_equ native_jax_missile_spawn 0x496fc0

; Retail source 0x004de4f0; branches use linked names.
jax_lp_throw_first_slam:
  open_peer_clip 0x0, clip(145)
  open_clip 0x0, clip(83)
  playback_single 0x37
  spawn_effect 0x0
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_first

; Retail source 0x004de518; branches use linked names.
slam_first_script:
  playback_single 0x52
  damage 0x0, 0xccc
  spawn_effect 0x0
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_second

; Retail source 0x004de538; branches use linked names.
slam_second_script:
  playback_single 0x74
  spawn_effect 0x0
  damage 0x0, 0xccc
  call 0x0, native_throw_grunt
  tail 0x0, native_jax_lp_slam_third

; Retail source 0x004de558; branches use linked names.
slam_second_release_script:
  native_return 0x7d
  jump 0x0, word(slam_first_release_script_native_word_1)
  .zero 4 ; non-executable guard

; Retail source 0x004de568; branches use linked names.
slam_first_release_script:
  native_return 0x64

; Retail source 0x004de56c; branches use linked names.
slam_first_release_script_native_word_1:
  native_word 0x0, 0xf5c
  damage 0x0, 0xccc
  tail 0x0, native_jax_lp_throw_return
  .zero 4 ; non-executable guard

; Retail source 0x004e0118; branches use linked names.
jax_fan_fatality_performer:
  prefixed_to 0x245, clip(415)
  tail 0x0, native_fan_tail
  jump 0, word(jarek_fan_fatality_male_victim_owner_cycle)
  .zero 4 ; non-executable guard

; Retail source 0x004e0138; branches use linked names.
jax_fan_fatality_owner_cycle:
  forward_to 0x4d
  cursor_set 0x45
  tail 0x0, native_fan_female_victim_owner_cycle

; Retail source 0x004e0148; branches use linked names.
jax_fan_fatality_owner_release:
  cursor_set 0x4e
  forward_to 0x6b
  tail 0x0, native_fan_victory
  jump 0, word(fan_victory)
  .zero 4 ; non-executable guard

; Retail source 0x004e0188; branches use linked names.
jax_fan_fatality_victim:
  prefixed_to 0x245, clip(416)

; Retail source 0x004e0190; branches use linked names.
jax_fan_fatality_victim_forward_to_1:
  forward_to 0x4d
  cursor_set 0x45
  jump 0x0, word(jax_fan_fatality_victim_forward_to_1)

; Retail source 0x004e01a0; branches use linked names.
jax_fan_fatality_victim_release:
  cursor_set 0x4e
  forward_to 0x6b
  tail 0x0, native_fan_victim_end
  .zero 4 ; non-executable guard

; Retail source 0x004e1390; branches use linked names.
jax_fatality_1_owner_opening:
  prefixed_finish 0x200, clip(384)
  tail 0x0, native_jax_fatality_1_victory

; Retail source 0x004e13a0; branches use linked names.
jax_fatality_1_owner_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(jax_fatality_1_owner_loop)

; Retail source 0x004e13b0; branches use linked names.
arm_tear_victim_clip_204:
  prefixed_to 0x231, clip(204)
  jump 0x0, word(arm_tear_victim_clip_400_tail_1)

; Retail source 0x004e13c0; branches use linked names.
arm_tear_victim_clip_400:
  prefixed_to 0x231, clip(400)

; Retail source 0x004e13c8; branches use linked names.
arm_tear_victim_clip_400_tail_1:
  tail 0x0, native_jax_fatality_1_grab

; Retail source 0x004e13d0; branches use linked names.
jax_fatality_1_grab:
  forward_to 0x4e
  tail 0x0, native_jax_fatality_1_first_arm
  .zero 4 ; non-executable guard

; Retail source 0x004e13e0; branches use linked names.
jax_fatality_1_first_arm:
  forward_to 0x80
  tail 0x0, native_jax_fatality_1_second_arm
  .zero 4 ; non-executable guard

; Retail source 0x004e13f0; branches use linked names.
jax_fatality_1_second_arm:
  forward 0x0
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Retail source 0x004e5a80; branches use linked names.
jax_fatality_2:
  native_release 0x43
  bind_cursor 0x0, clip(284)
  wait 0x14
  tail 0x0, native_jax_fatality_2_approach

; Retail source 0x004e5a98; branches use linked names.
jax_fatality_2_approach:
  spawn 0x0, 0x26b, native_jax_fatality_2_sparks
  native_release 0x43
  sound_c 0x46
  wait 0xf
  scene_active 0x0
  sound_c 0x46
  wait 0xf
  sound_c 0x2
  prefixed_finish 0x200, clip(555)
  spawn 0x0, 0x26b, native_jax_fatality_2_flash
  spawn_effect 0x6
  sound_c 0x69
  sound_c 0x2c
  native_return 0xb7
  wait 0xf
  clip 0x0, clip(556)
  native_release 0x50
  prefixed_finish 0x200, clip(114)

; Retail source 0x004e5afc; branches use linked names.
jax_fatality_2_approach_playback_pair_1:
  playback_pair 0x45
  forward_to 0x63
  jump 0x0, word(jax_fatality_2_approach_playback_pair_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e5b10; branches use linked names.
jax_fatality_2_head:
  sound_b 0x8
  clip 0x0, clip(557)
  spawn 0x0, 0x26b, native_scene_keepalive_task
  clip 0x0, clip(206)
  tail 0x0, native_jax_fatality_2_finish
  jump 0, word(cage_fatality_2_victim_start)
  .zero 4 ; non-executable guard

; Retail source 0x004e5c70; branches use linked names.
jax_high_kick_recovery:
  backward_count 0x2
  release_attack 0x0
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e5dd8; branches use linked names.
air_throw_lift:
  sound_indexed 0x36
  open_peer_clip 0x0, clip(144)
  open_clip 0x0, clip(84)
  playback_single 0x14
  tail 0x0, native_air_lift_fall

; Retail source 0x004e5df8; branches use linked names.
air_lift_fall:
  playback_single 0x20
  sound_a 0x24
  sound_roster 0x1
  spawn_effect 0x6
  playback_single 0x2b
  disarm_peer 0x0
  tail 0x0, native_air_lift_release

; Retail source 0x004e5e18; branches use linked names.
air_lift_release:
  playback_single 0x30
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e66d0; branches use linked names.
jax_draw_weapon_air_hp:
  attack_selector 0x0, 0x104
  tail 0x0, native_weapon_air_high_punch_open
  .zero 4 ; non-executable guard

; Retail source 0x004e66f0; branches use linked names.
jax_draw_weapon_air_lp:
  attack_selector 0x0, 0x108
  tail 0x0, native_weapon_air_low_punch_open
  .zero 4 ; non-executable guard

; Retail source 0x004e6890; branches use linked names.
jax_roundhouse_fast_start:
  double_to 0xc
  sound_c 0x1c
  double_to 0x14
  tail 0x0, native_roundhouse_wide_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e68a8; branches use linked names.
jax_roundhouse_normal_start:
  double_to 0xc
  sound_c 0x1c
  forward_to 0x14
  tail 0x0, native_roundhouse_wide_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e69b0; branches use linked names.
jax_combo_hp_attacker:
  open_clip 0x2, clip(261)
  forward_to 0xb
  tail 0x0, native_combo_high_punch_tail
  .zero 4 ; non-executable guard

; Retail source 0x004e8150; branches use linked names.
jax_dash_punch:
  sound_indexed 0x25
  sound_c 0x4e
  speed 0x0, 0x2666
  open_clip 0x0, clip(299)
  double_to 0x12
  tail 0x0, native_jax_dash_punch_contact
  .zero 4 ; non-executable guard

; Retail source 0x004e89d8; branches use linked names.
jax_dash_punch_airborne_victim_168:
  effect 0x9
  sound_b 0x1
  spawn_effect 0x0
  sound_context 0x0
  effect 0x4
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, native_jax_dash_punch_roll_getup
  .zero 4 ; non-executable guard

; Retail source 0x004e98e8; branches use linked names.
air_victim_lift:
  motion 0x0, 0xffffe667, 0xffffd99a, 0x28f
  task_callback 0x0
  prefixed_to 0x223, clip(140)
  native_context 0x0
  tail 0x0, native_air_lift_getup
  jump 0, word(air_victim_default)
  .zero 4 ; non-executable guard

; Retail source 0x004e9e38; branches use linked names.
jax_dash_punch_grounded_victim_168:
  effect 0x9
  sound_b 0x1
  speed_negative 0x0, 0x6666
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea028; branches use linked names.
jax_dash_victim_hit:
  effect 0x9
  sound_context 0x1
  prefixed_finish 0x200, clip(311)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed090; branches use linked names.
jax_missile:
  prefixed_to 0x20a, clip(29)
  sound_c 0x5d
  spawn 0x0, 0x50, native_jax_missile_spawn
  forward_to 0x19
  half_finish 0x0
  ready 0x0
  .zero 4 ; non-executable guard
