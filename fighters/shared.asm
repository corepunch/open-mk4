; Generated shared bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
shared_tail_entry:
  tail 0x0, native_lp_select_short

; Branches use linked names.
lp_select_short:
  native_release 0xb
  open_peer_clip 0x0, clip(158)
  open_clip 0x0, clip(81)
  playback_single 0x2e
  native_return 0x6d
  damage 0x0, 0x2e14
  forward 0x0
  tail 0x0, native_lp_throw_owner_ready

; Branches use linked names.
lp_select_long:
  native_release 0xc
  open_peer_clip 0x0, clip(154)
  open_clip 0x0, clip(80)
  playback_single 0x39
  native_return 0x6b
  tail 0x0, native_lp_end
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_end:
  damage 0x0, 0x2e14
  forward 0x0
  ready 0x0

; Branches use linked names.
lp_end_native_release_1:
  native_release 0x24
  open_peer_clip 0x0, clip(151)
  open_clip 0x0, clip(79)
  playback_single 0x2d
  native_return 0x68
  native_word 0x0, 0xccc
  damage 0x0, 0x2666
  forward 0x0
  tail 0x0, native_return_to_player_or_cpu_control
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_select:
  native_release 0x16
  open_peer_clip 0x0, clip(153)
  open_clip 0x0, clip(76)
  playback_single 0x19
  native_return 0x66
  native_word 0x0, 0x1eb8
  jump 0x0, word(shared_damage_entry)
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_throw_script:
  sound_b 0x7

; Branches use linked names.
lp_throw_script_select_roster_1:
  select_roster 0x0, word(goro_lp_throw_final_damage), word(shared_tail_entry_2), word(shared_tail_entry_2_native_release_1), word(lp_end_native_release_1), word(lp_select), word(shared_tail_entry), word(goro_lp_throw_final_damage), word(shared_tail_entry_2), word(shared_tail_entry_2_native_release_1), word(lp_end_native_release_1), word(lp_select), word(shared_tail_entry), word(shared_tail_entry_2), word(lp_throw_script_select_roster_1_operand_16), word(goro_lp_throw_final_damage), word(goro_lk_throw_startup_call_1)
  tail 0x0, native_jax_lp_throw_throw_continuation
  jump 0, word(goro_lp_throw_repeated_damage)
  .zero 4 ; non-executable guard
.alias lp_throw_script_select_roster_1_operand_16 lp_throw_script_select_roster_1 + 64

; Branches use linked names.
shared_tail_entry_2:
  tail 0x0, native_lp_select_long

; Branches use linked names.
shared_tail_entry_2_native_release_1:
  native_release 0x16
  open_peer_clip 0x0, clip(156)
  open_clip 0x0, clip(78)
  playback_single 0x15
  native_return 0x67
  damage 0x0, 0x2666
  forward_count 0x2
  wait 0xa
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
shared_damage_entry:
  damage 0x0, 0x2666
  forward 0x0
  tail 0x0, native_sub_zero_lp_throw_owner_return
  .zero 4 ; non-executable guard

; Branches use linked names.
fan_victory:
  forward 0x0
  prefixed_finish 0x200, clip(412)

; Branches use linked names.
fan_victory_cursor_set_1:
  cursor_set 0x12
  forward 0x0
  jump 0x0, word(fan_victory_cursor_set_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
dragon_victim_script:
  prefixed_finish 0x200, clip(419)
  tail 0x0, native_task_retire
  .zero 4 ; non-executable guard

; Branches use linked names.
fire_script_recovery:
  forward_to 0x1e
  tail 0x0, native_fire_recovery
  .zero 4 ; non-executable guard

; Branches use linked names.
high_kick_clip_71:
  attack_selector 0x0, 0x2
  sound_a 0x1
  prefixed_to 0x20a, clip(71)
  tail 0x0, native_kick_contact_narrow
  .zero 4 ; non-executable guard

; Branches use linked names.
high_kick:
  select_roster 0x0, word(high_kick_clip_66), word(high_kick_clip_71), word(high_kick_sonya), word(high_kick_clip_66), word(high_kick_clip_71), word(high_kick_sonya), word(high_kick_clip_66), word(high_kick_clip_66), word(high_kick_clip_71), word(high_kick_clip_66), word(high_kick_clip_66), word(high_kick_clip_71), word(high_kick_sonya), word(high_kick_clip_66), word(high_kick_clip_71), word(high_kick_clip_66)
  attack_selector 0x0, 0x0
  sound_a 0x1
  prefixed_to 0x20a, clip(65)
  tail 0x0, native_kick_contact_default
  .zero 4 ; non-executable guard
.alias high_kick_sonya high_kick + 64

; Branches use linked names.
low_kick:
  select_roster 0x0, word(low_kick_clip_67), word(low_kick_clip_70), word(low_kick_clip_308), word(low_kick_clip_67), word(low_kick_clip_70), word(low_kick_clip_308), word(low_kick_clip_67), word(low_kick_clip_67), word(low_kick_clip_70), word(low_kick_clip_308), word(low_kick_clip_67), word(low_kick_clip_70), word(low_kick_clip_308), word(low_kick_clip_67), word(low_kick_clip_70), word(low_kick_clip_67)
  .zero 4 ; non-executable guard

; Branches use linked names.
air_throw:
  native_release 0x17
  sound_indexed 0xd
  open_peer_clip 0x0, clip(202)
  open_clip 0x0, clip(82)
  playback_single 0x1c
  tail 0x0, native_air_throw_release
  .zero 4 ; non-executable guard

; Branches use linked names.
air_throw_release:
  forward_count 0x1
  motion 0x0, 0x7ae, 0xffffe667, 0x28f
  wait 0xa
  forward 0x0
  native_context 0x0
  tail 0x0, native_air_throw_land

; Branches use linked names.
air_throw_slam:
  landing 0x0
  native_release 0x17
  sound_indexed 0x15
  open_peer_clip 0x0, clip(150)
  open_clip 0x0, clip(74)
  playback_single 0x12
  tail 0x0, native_air_slam_fall

; Branches use linked names.
air_slam_fall:
  playback_single 0x1b
  tail 0x0, native_noob_saibot_air_throw_landing_link
  .zero 4 ; non-executable guard

; Branches use linked names.
air_throw_flip:
  native_release 0x17
  sound_indexed 0x10
  open_peer_clip 0x0, clip(161)
  open_clip 0x0, clip(77)
  play_pair 0x18
  tail 0x0, native_air_flip_release
  .zero 4 ; non-executable guard

; Branches use linked names.
follow_crouch_hk:
  speed 0x0, 0x1999

; Branches use linked names.
crouch_high_kick_clip_52:
  attack_selector 0x0, 0x1a
  sound_a 0x1
  play_clip 0x7, clip(52)
  query_b 0x1a
  action_recovery 0x0
  landing 0x0
  forward 0x0
  loop 0x5
  playback_tail 0xa
  wait 0x2
  playback_tail 0x9
  wait 0x2
  repeat 0x0
  clip_operation 0x0, clip(52)
  return 0x0

; Branches use linked names.
crouch_high_kick:
  select_roster 0x0, word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_52), word(crouch_high_kick_sonya), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_52), word(crouch_high_kick_sonya), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_52), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_52), word(crouch_high_kick_clip_52), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_54), word(crouch_high_kick_clip_52), word(crouch_high_kick_clip_52)
  attack_selector 0x0, 0x18
  sound_a 0x1
  play_clip 0x9, clip(53)
  query_b 0x18
  action_recovery 0x0
  forward 0x0
  loop 0x2
  playback_tail 0xb
  wait 0x2
  playback_tail 0xa
  wait 0x2
  playback_tail 0xb
  wait 0x2
  playback_tail 0xc
  wait 0x2
  repeat 0x0
  clip_operation 0x0, clip(53)
  return 0x0
.alias crouch_high_kick_sonya crouch_high_kick + 64

; Branches use linked names.
crouch_high_kick_clip_54:
  attack_selector 0x0, 0x19
  sound_a 0x1
  play_clip 0x9, clip(54)
  query_b 0x19
  action_recovery 0x0
  forward 0x0

; Branches use linked names.
crouch_high_kick_clip_54_loop_1:
  loop 0x2
  playback_tail 0xb
  wait 0x2
  playback_tail 0xa
  wait 0x2
  playback_tail 0xb
  wait 0x2
  playback_tail 0xc
  wait 0x2
  repeat 0x0
  clip_operation 0x0, clip(54)
  return 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
crouch_low_kick:
  attack_selector 0x0, 0x17
  sound_c 0x1b
  open_clip 0x0, clip(315)
  cursor_set 0x1
  forward_to 0x9
  tail 0x0, native_crouch_low_kick_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
crouch_low_kick_recover:
  cursor_set 0x9
  playback_pair 0x1
  return 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
knee_script:
  open_clip 0x0, clip(314)
  playback_tail 0x5
  wait 0x1
  playback_tail 0x6
  wait 0x1
  playback_tail 0x7
  wait 0x1
  playback_tail 0x8
  wait 0x1
  playback_tail 0x9
  action_recovery 0x0
  tail 0x0, native_close_high_kick_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
knee_miss_finished:
  wait 0x4
  backward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
close_high_punch_2:
  attack_selector 0x0, 0x110
  forward_to 0x7
  tail 0x0, native_close_high_punch_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
air_straight_kick_startup:
  select_roster 0x0, word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_clip_61), word(air_straight_kick_startup_scorpion), word(air_straight_kick_startup_scorpion)
  attack_selector 0x0, 0x28
  sound_a 0x1f
  clip 0x0, clip(327)
  tail 0x0, native_air_straight_tail
  .zero 4 ; non-executable guard
.alias air_straight_kick_startup_scorpion air_straight_kick_startup + 64

; Branches use linked names.
air_punch_contact:
  allow_commands 0x0
  wait 0xc
  gravity 0x0, 0x28f
  native_context 0x0
  tail 0x0, native_air_attack_land_or_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
uppercut:
  attack_selector 0x0, 0x102
  prefixed_to 0x205, clip(293)
  sound_c 0x1c
  tail 0x0, native_uppercut_default
  .zero 4 ; non-executable guard

; Branches use linked names.
jab:
  call 0x0, native_jab_repeat_predicate
  sound_a 0xa
  play_clip 0x8, clip(294)
  call 0x0, native_jab_contact
  wait 0x1
  predicate_flags 0x0, native_jab_repeat_predicate, word(cage_jab_repeat_tail_1)
  sound_a 0xa
  open_clip 0x0, clip(295)
  tail 0x0, native_cage_jab_second_punch
  jump 0, word(cage_jab_second_punch)
  .zero 4 ; non-executable guard

; Branches use linked names.
air_diagonal_low_kick_startup:
  sound_a 0x19

; Branches use linked names.
air_diagonal_low_kick_startup_select_roster_1:
  select_roster 0x0, word(air_diagonal_low_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_clip_1), word(air_straight_kick_startup_clip_61_clip_2), word(air_diagonal_low_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_clip_1), word(air_straight_kick_startup_clip_61_clip_2), word(air_diagonal_low_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_clip_1), word(air_straight_kick_startup_clip_61_clip_2), word(air_diagonal_low_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_clip_1), word(air_straight_kick_startup_clip_61_clip_2), word(air_diagonal_low_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_clip_1), word(air_straight_kick_startup_clip_61_clip_2), word(air_straight_kick_startup_clip_61_clip_2)
  clip 0x0, clip(58)
  attack_selector 0x0, 0x1b
  tail 0x0, native_air_lk_tail
  .zero 4 ; non-executable guard
.alias air_diagonal_low_kick_startup_select_roster_1_operand_16 air_diagonal_low_kick_startup_select_roster_1 + 64

; Branches use linked names.
air_low_kick_clip_58_reverse:
  clip_operation 0x0, clip(58)
  native_context 0x0
  tail 0x0, native_air_low_kick_land
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_kick_clip_59_reverse:
  clip_operation 0x0, clip(59)
  native_context 0x0
  tail 0x0, native_unresolved_air_low_kick_clip_59_reverse
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_kick_clip_60_reverse:
  clip_operation 0x0, clip(60)
  native_context 0x0
  tail 0x0, native_unresolved_air_low_kick_clip_60_reverse
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_kick_contact_hold:
  landing 0x0
  wait 0x8
  allow_commands 0x0
  gravity 0x0, 0x51e
  native_context 0x0
  tail 0x0, native_air_attack_land_or_finish

; Branches use linked names.
air_diagonal_high_kick_startup:
  sound_a 0x19

; Branches use linked names.
air_diagonal_high_kick_startup_select_roster_1:
  select_roster 0x0, word(air_diagonal_high_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_attack_selector_1), word(air_straight_kick_startup_clip_61_attack_selector_2), word(air_diagonal_high_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_attack_selector_1), word(air_straight_kick_startup_clip_61_attack_selector_2), word(air_diagonal_high_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_attack_selector_1), word(air_straight_kick_startup_clip_61_attack_selector_2), word(air_diagonal_high_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_attack_selector_1), word(air_straight_kick_startup_clip_61_attack_selector_2), word(air_diagonal_high_kick_startup_select_roster_1_operand_16), word(air_straight_kick_startup_clip_61_attack_selector_1), word(air_straight_kick_startup_clip_61_attack_selector_2), word(air_straight_kick_startup_clip_61_attack_selector_1)
  attack_selector 0x0, 0x1e
  tail 0x0, native_air_diagonal_tail
  .zero 4 ; non-executable guard
.alias air_diagonal_high_kick_startup_select_roster_1_operand_16 air_diagonal_high_kick_startup_select_roster_1 + 64

; Branches use linked names.
air_diagonal_kick_clip_55_reverse:
  clip_operation 0x0, clip(55)
  native_context 0x0
  tail 0x0, native_unresolved_air_diagonal_kick_clip_55_reverse
  .zero 4 ; non-executable guard

; Branches use linked names.
air_diagonal_kick_clip_56_reverse:
  clip_operation 0x0, clip(56)
  native_context 0x0
  tail 0x0, native_unresolved_air_diagonal_kick_clip_56_reverse
  .zero 4 ; non-executable guard

; Branches use linked names.
air_diagonal_kick_clip_57_reverse:
  clip_operation 0x0, clip(57)
  native_context 0x0
  tail 0x0, native_unresolved_air_diagonal_kick_clip_57_reverse
  .zero 4 ; non-executable guard

; Branches use linked names.
air_diagonal_kick_contact_hold:
  landing 0x0
  wait 0x4
  allow_commands 0x0
  wait 0x4
  gravity 0x0, 0x51e
  native_context 0x0
  tail 0x0, native_air_attack_land_or_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
air_high_punch_startup:
  attack_selector 0x0, 0x104
  sound_a 0x19
  tail 0x0, native_air_hp_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
air_high_punch_miss_reverse:
  clip_operation 0x0, clip(97)
  tail 0x0, native_air_straight_land
  .zero 4 ; non-executable guard

; Branches use linked names.
air_high_contact_end:
  gravity 0x0, 0x28f
  native_context 0x0
  tail 0x0, native_air_attack_land_or_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_punch_startup:
  attack_selector 0x0, 0x105
  sound_a 0x19
  tail 0x0, native_air_lp_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_punch_miss_reverse:
  clip_operation 0x0, clip(96)
  tail 0x0, native_air_straight_land
  .zero 4 ; non-executable guard

; Branches use linked names.
air_low_punch_contact_hold:
  allow_commands 0x0
  landing 0x0
  wait 0xc
  gravity 0x0, 0x28f
  native_context 0x0
  tail 0x0, native_air_attack_land_or_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
sweep:
  select_roster 0x0, word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_raiden), word(sweep_clip_63), word(sweep_clip_63)
  attack_selector 0x0, 0x24
  open_clip 0x0, clip(62)
  double_to 0xa
  sound_a 0xaf
  forward_to 0x16
  tail 0x0, native_sweep_advancing_tail
.alias sweep_raiden sweep + 64

; Branches use linked names.
roundhouse:
  select_roster 0x0, word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_clip_64), word(roundhouse_scorpion), word(roundhouse_scorpion)
  jump 0, word(scorpion_roundhouse_attacker)
  .zero 4 ; non-executable guard
.alias roundhouse_scorpion roundhouse + 64

; Branches use linked names.
spike_enter:
  wait 0x1c
  prefixed_to 0x205, clip(293)
  sound_c 0x1c
  query_a 0x12
  tail 0x0, native_stage_owner
  .zero 4 ; non-executable guard

; Branches use linked names.
high_kick_clip_66:
  attack_selector 0x0, 0x1
  sound_a 0x1
  play_clip 0x3, clip(66)
  double_count 0x11
  tail 0x0, native_kick_contact_wide

; Branches use linked names.
low_kick_clip_308:
  call 0x0, native_avoid_clear
  attack_selector 0x0, 0x5
  sound_a 0x1
  open_clip 0x0, clip(308)
  cursor_set 0x4
  double_to 0x10
  call 0x0, native_avoid_set
  double_to 0x18
  query_b 0x5
  wait_branch 0x4002
  action_recovery 0x0
  backward_count 0x2
  call 0x0, native_avoid_clear
  release_attack 0x0
  tail 0x0, native_low_kick_reverse_return
  .zero 4 ; non-executable guard

; Branches use linked names.
low_kick_clip_67:
  call 0x0, native_avoid_clear
  attack_selector 0x0, 0x6
  sound_a 0x1
  open_clip 0x0, clip(67)
.alias low_kick_clip_67_operand_1 low_kick_clip_67 + 4

; Branches use linked names.
low_kick_clip_67_prefixed_sample_1:
  prefixed_sample 0x0
  wait 0x1
  prefixed_sample 0x2
  wait 0x1
  prefixed_sample 0x4
  wait 0x1
  prefixed_sample 0x6
  wait 0x1
  prefixed_sample 0x8
  wait 0x1
  prefixed_sample 0xa
  wait 0x1
  prefixed_sample 0xc
  wait 0x1
  call 0x0, native_avoid_set
  prefixed_sample 0xe
  wait 0x1
  prefixed_sample 0x10
  wait 0x1
  prefixed_sample 0x11
  wait 0x1
  call 0x0, native_avoid_clear
  query_b 0x6
  action_recovery 0x0
  release_attack 0x0
  wait_branch 0x8005
  tail 0x0, native_low_kick_narrow_reverse_return
  .zero 4 ; non-executable guard

; Branches use linked names.
low_kick_clip_70:
  call 0x0, native_avoid_clear
  attack_selector 0x0, 0x7
  sound_a 0x1
  open_clip 0x0, clip(70)
  prefixed_to 0x6
  call 0x0, native_avoid_set
  forward_to 0xb
  call 0x0, native_avoid_clear
  query_b 0x7
  action_recovery 0x0
  wait_branch 0x4002
  forward_count 0x2
  release_attack 0x0
  forward 0x0
  ready 0x0

; Branches use linked names.
air_straight_kick_startup_clip_61:
  attack_selector 0x0, 0x27
  open_clip 0x0, clip(61)
  cursor_set 0x4
  forward_to 0x7
  sound_a 0x1f
  forward_to 0xa
  tail 0x0, native_air_straight_wide_tail

; Branches use linked names.
air_straight_kick_startup_clip_61_clip_1:
  clip 0x0, clip(59)
  attack_selector 0x0, 0x1c
  tail 0x0, native_air_lk_alt_tail

; Branches use linked names.
air_straight_kick_startup_clip_61_clip_2:
  clip 0x0, clip(60)
  attack_selector 0x0, 0x1d
  tail 0x0, native_air_lk_wide_tail

; Branches use linked names.
air_straight_kick_startup_clip_61_attack_selector_1:
  attack_selector 0x0, 0x1f
  tail 0x0, native_air_diagonal_alt_tail

; Branches use linked names.
air_straight_kick_startup_clip_61_attack_selector_2:
  attack_selector 0x0, 0x20
  tail 0x0, native_air_diagonal_wide_tail

; Branches use linked names.
sweep_clip_63:
  attack_selector 0x0, 0x23
  open_clip 0x0, clip(63)
  double_to 0xa
  sound_a 0xaf
  forward_to 0x16
  tail 0x0, native_sweep_standard_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
roundhouse_clip_64:
  attack_selector 0x0, 0x22
  open_clip 0x0, clip(64)
  tail 0x0, native_roundhouse_wide_start
  .zero 4 ; non-executable guard

; Branches use linked names.
teleport:
  sound_c 0x36
  prefixed_to 0x203, clip(86)
  motion 0x0, 0x0, 0xffffcccd, 0x0
  forward 0x0
  tail 0x0, native_teleport_vanish

; Branches use linked names.
teleport_slam:
  sound_indexed 0x18
  native_release 0x17
  landing 0x0
  open_peer_clip 0x0, clip(150)
  open_clip 0x0, clip(74)
  playback_single 0x12
  tail 0x0, native_teleport_slam_fall
  .zero 4 ; non-executable guard

; Branches use linked names.
teleport_slam_fall:
  playback_single 0x1b
  tail 0x0, native_teleport_slam_drop
  .zero 4 ; non-executable guard

; Branches use linked names.
power_rewind:
  clip_operation 0x0, clip(301)
  native_context 0x0
  clear_handler 0x0
  tail 0x0, native_recoil_land
  .zero 4 ; non-executable guard

; Branches use linked names.
teleport_block_return:
  clip_operation 0x0, clip(301)
  native_context 0x0
  clear_handler 0x0
  tail 0x0, native_land
  jump 0, word(fujin_wind_grab)
  .zero 4 ; non-executable guard

; Branches use linked names.
guard_airborne_fall:
  sound_context 0x5
  sound_b 0x3
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  wait 0x2
  tail 0x0, native_mmove_side_getup
  jump 0, word(jax_dash_punch_airborne_victim_168)
  .zero 4 ; non-executable guard

; Branches use linked names.
react_kick_air:
  effect 0x4
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, native_jax_dash_punch_roll_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_fall_return:
  motion 0x0, 0xfffff0a4, 0xffffd47b, 0x28f
  task_callback 0x0
  open_clip 0x0, clip(131)
  double_to 0xd
  forward_to 0x29
  nop 0x13ece8
  native_context 0x0
  forward_count 0x4
  prefixed_finish 0x200, clip(185)
  tail 0x0, native_vm_return

; Branches use linked names.
impact_reaction:
  effect 0xc
  sound_context 0xb
  sound_b 0x3
  peer_motion 0x0, 0xfffff0a4, 0xffffd47b, 0x28f
  task_callback 0x0
  open_clip 0x0, clip(131)
  double_to 0xd
  forward_to 0x29
  nop 0x13ece8
  native_context 0x0
  forward_count 0x4
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
react_jab_air:
  effect 0x6
  sound_context 0xf
  motion 0x0, 0xfffff334, 0xffffdc29, 0x28f
  task_callback 0x0
  play_clip 0x1e, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
weapon_clip_189_launch_back_getup:
  effect 0x2
  sound_b 0x1
  sound_context 0x6
  motion 0x0, 0xfffff99a, 0xffffc000, 0x28f
  task_callback 0x0
  effect 0x0
  open_clip 0x0, clip(189)
  forward_to 0x9
  half_finish 0x0
  native_context 0x0
  forward 0x0
  tail 0x0, native_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_clip_311_short_slide:
  effect 0x10
  sound_context 0x6
  speed_negative 0x0, 0xccc
  clip 0x0, clip(311)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
weapon_clip_189_launch_roll_getup:
  effect 0x0
  sound_context 0x6
  sound_b 0x1
  motion 0x0, 0xfffffc29, 0xffffc7af, 0x28f
  task_callback 0x0
  clip 0x0, clip(189)
  native_context 0x0
  tail 0x0, native_jax_dash_punch_roll_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_clip_326_short_slide:
  effect 0x12
  sound_context 0x6
  speed_negative 0x0, 0xccc
  clip 0x0, clip(326)
  call 0x0, native_unresolved_reaction_clip_326_short_slide
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
launch_clip_131_back_getup:
  sound_context 0x6
  sound_b 0x1
  motion 0x0, 0xfffff334, 0xffffdc29, 0x20c
  task_callback 0x0
  play_clip 0x28, clip(131)
  native_context 0x0
  forward 0x0
  tail 0x0, native_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_clip_180_slide_hold:
  effect 0x8
  effect 0x2
  sound_context 0x6
  sound_b 0x1
  speed_negative 0x0, 0x3333
  bind_submit 0x0, clip(180)
  wait 0x6
  clip 0x0, clip(180)
  ready 0x0

; Branches use linked names.
launch_clip_131_native_finish_back_getup:
  effect 0x8
  effect 0x2
  sound_context 0x6
  sound_b 0x1
  motion 0x0, 0xfffff5c3, 0xffffdc29, 0x24d
  task_callback 0x0
  play_clip 0x28, clip(131)
  native_context 0x0
  tail 0x0, native_reaction_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
weapon_tumble_back_getup:
  effect 0x8
  effect 0x2
  spawn_effect 0x6
  sound_b 0x1
  task_callback 0x0
  motion_block 0x0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0x1ca
  task_callback 0x0
  handler 0x0, native_landing_motion
  clip 0x0, clip(142)
  bind_cursor 0x0, clip(141)
  native_context 0x0
  clear_handler 0x0
  open_clip 0x0, clip(143)
  cursor_set 0xc
  forward 0x0
  tail 0x0, native_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
grounded_tumble_hold_return:
  effect 0x0
  reaction_context 0x0
  spawn_effect 0x0
  sound_context 0x6
  sound_b 0x1
  task_callback 0x0
  motion_block 0x0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0xde
  handler 0x0, native_landing_motion
  clip 0x0, clip(142)
  clip 0x0, clip(141)
  play_clip 0xc, clip(143)
  clear_handler 0x0
  forward 0x0
  wait 0x14
  tail 0x0, native_return_to_player_or_cpu_control

; Branches use linked names.
tumble_native_finish_back_getup:
  reaction_context 0x0
  sound_c 0x8
  sound_b 0x1
  task_callback 0x0
  motion_block 0x0, 0xfffff5c3, 0xfffff5c3, 0xffffeb86, 0xde
  effect 0x2
  effect 0x8
  effect 0x8
  handler 0x0, native_landing_motion
  clip 0x0, clip(142)
  clip 0x0, clip(141)
  play_clip 0xc, clip(143)
  clear_handler 0x0
  tail 0x0, native_reaction_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_clip_132_native_finish:
  effect 0x0
  sound_context 0x6
  sound_b 0x1
  speed_negative 0x0, 0x11eb
  play_clip 0xe, clip(132)
  sound_a 0x4
  landing 0x0
  tail 0x0, native_reaction_getup_back
  .zero 4 ; non-executable guard

; Branches use linked names.
stagger_clip_311_hold:
  speed_negative 0x0, 0x2666
  sound_context 0x11
  sound_b 0x3
  bind_submit 0x4, clip(311)
  handler 0x0, native_velocity_friction
  wait 0xd
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
stagger_clip_134_friction:
  speed_negative 0x0, 0x1eb8
  handler 0x0, native_velocity_friction
  clip 0x0, clip(134)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
stagger_clip_134_friction_impact:
  sound_b 0x9
  speed_negative 0x0, 0x1eb8
  handler 0x0, native_velocity_friction
  clip 0x0, clip(134)
  ready 0x0

; Branches use linked names.
crouch_stagger_clip_254:
  speed_negative 0x0, 0x1eb8
  sound_b 0x9
  handler 0x0, native_velocity_friction
  prefixed_finish 0x200, clip(254)
  wait 0x4
  playback_pair 0x0
  tail 0x0, native_crouch_return
  .zero 4 ; non-executable guard

; Branches use linked names.
rear_hit_launch_clip_129:
  sound_a 0x16
  motion 0x0, 0xffffd99a, 0xffffcccd, 0x418
  tail 0x0, native_reaction_clip_129_startup
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_clip_129_startup:
  play_clip 0x22, clip(129)
  tail 0x0, native_reaction_clip_129_wait_landing

; Branches use linked names.
reaction_clip_129_wait_landing:
  native_context 0x0
  sound_a 0x17
  landing 0x0

; Branches use linked names.
reaction_clip_129_wait_landing_tail_1:
  tail 0x0, native_reaction_clip_129_getup
  nop 0x0
  forward 0x0
  wait 0x10
  call 0x0, native_face_peer_orient_group
  tail 0x0, native_getup_back
  .zero 4 ; non-executable guard
.alias reaction_clip_129_wait_landing_tail_1_operand_1 reaction_clip_129_wait_landing_tail_1 + 4

; Branches use linked names.
kick_hit_standing_stagger:
  effect 0x4
  sound_b 0x3
  sound_context 0x1
  speed_negative 0x0, 0x2666
  bind_submit 0x4, clip(311)
  handler 0x0, native_velocity_friction
  wait 0xd
  forward 0x0
  ready 0x0

; Branches use linked names.
kick_hit_crouching_stagger:
  no_guard 0x0
  effect 0x4
  spawn_effect 0x1
  sound_b 0x3
  bind_submit 0x4, clip(311)
  wait 0xd
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
stagger_clip_134_shake:
  no_guard 0x0
  speed_negative 0x0, 0x51e
  spawn_effect 0x1
  sound_context 0x3
  sound_b 0x2
  open_clip 0x0, clip(134)
  forward_count 0x6
  sound_indexed 0xa
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  playback_native 0x0
  wait 0x2
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
knockdown_stagger_clip_311:
  effect 0x4
  no_guard 0x0
  spawn_effect 0x1
  bind_submit 0x4, clip(311)
  wait 0xd
  forward 0x0
  ready 0x0

; Branches use linked names.
forward_fall_grounded:
  effect 0xb
  sound_b 0x3
  peer_motion 0x0, 0xffffe667, 0x0, 0x0
  clip 0x0, clip(186)
  sound_special 0x0
  landing 0x0
  tail 0x0, native_getup_front

; Branches use linked names.
weapon_clip_132_directional_fall:
  sound_b 0x1
  effect 0x3
  effect 0x8
  effect 0x8
  airborne 0x0, word(weapon_clip_132_directional_fall_task_callback_1)
  peer_motion 0x0, 0xffffe667, 0x0, 0x0

; Branches use linked names.
weapon_clip_132_directional_fall_task_callback_1:
  task_callback 0x0
  peer_motion 0x0, 0xffffe667, 0x666, 0x147
  play_clip 0xd, clip(132)
  native_context 0x0
  tail 0x0, native_weapon_fall_finish_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
forward_fall_airborne:
  effect 0xa
  motion 0x0, 0xffffe667, 0x0, 0x28f
  play_clip 0xe, clip(186)
  native_context 0x0
  forward 0x0
  tail 0x0, native_getup_front
  .zero 4 ; non-executable guard

; Branches use linked names.
sweep_victim_recovery:
  sound_b 0x0
  reaction_context 0x0
  motion_block 0x0, 0xfffff0a4, 0xccc, 0x0, 0x0
  play_clip 0x18, clip(192)
  sound_special 0x0
  landing 0x0
  forward 0x0
  down 0x0, word(sweep_victim_recovery_clip_1)
  open_clip 0x0, clip(194)
  tail 0x0, native_shinnok_weapon_sweep_finish

; Branches use linked names.
sweep_victim_recovery_clip_1:
  clip 0x0, clip(193)
  tail 0x0, native_crouch_return

; Branches use linked names.
tumble_camera_side_positive:
  reaction_context 0x0
  sound_context 0x1
  spawn_effect 0x0
  sound_b 0x3
  effect 0x0
  task_callback 0x0
  motion_block 0x0, 0xfffff334, 0xccc, 0xffffeb86, 0xde
  handler 0x0, native_landing_motion
  clip 0x0, clip(142)
  clip 0x0, clip(141)
  play_clip 0xc, clip(143)
  clear_handler 0x0
  forward 0x0
  wait 0x8
  tail 0x0, native_getup_back

; Branches use linked names.
tumble_camera_side_negative:
  reaction_context 0x0
  sound_context 0x1
  spawn_effect 0x0
  sound_b 0x3
  effect 0x0
  task_callback 0x0
  motion_block 0x0, 0xfffff334, 0xfffff334, 0xffffeb86, 0xde
  handler 0x0, native_landing_motion
  clip 0x0, clip(142)
  clip 0x0, clip(141)
  play_clip 0xc, clip(143)
  clear_handler 0x0
  forward 0x0
  wait 0x8
  tail 0x0, native_getup_back
  jump 0, word(goro_quake_victim)
  .zero 4 ; non-executable guard

; Branches use linked names.
launch_clip_131_shake_impact:
  sound_context 0x2
  spawn_effect 0x6
  sound_b 0x3
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0x0
  play_clip 0x28, clip(131)
  native_context 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
fall_reaction_effects:
  sound_context 0x6
  spawn_effect 0x0
  effect 0x8
  tail 0x0, native_reaction_fall
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_fall:
  effect 0xc
  sound_context 0x0
  sound_b 0x3
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0x0
  play_clip 0x28, clip(131)
  native_context 0x0
  tail 0x0, native_reaction_finish
  .zero 4 ; non-executable guard

; Branches use linked names.
uppercut_victim_recovery:
  sound_b 0x1
  spawn_effect 0x0
  sound_context 0x0
  effect 0x0
  effect 0x8
  motion 0x0, 0xfffff852, 0xffffcccd, 0x20c
  play_clip 0x35, clip(128)
  native_context 0x0
  sound_a 0x17
  sound_special 0x0
  tail 0x0, native_uppercut_recover

; Branches use linked names.
jab_hit_short_recovery:
  effect 0x6
  open_clip 0x0, clip(311)
  cursor_set 0x4
  forward_to 0xc
  prefixed_sample 0xe
  wait 0x1
  prefixed_sample 0x10
  wait 0x1
  prefixed_sample 0x12
  wait 0x1
  tail 0x0, native_return_to_player_or_cpu_control
  .zero 4 ; non-executable guard

; Branches use linked names.
jab_hit_damage_stagger:
  speed_negative 0x0, 0x1999
  open_clip 0x0, clip(182)
  double_to 0x26
  landing 0x0
  double_to 0x32
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
stagger_entry_2:
  sound_b 0x1
  speed_negative 0x0, 0x3333
  open_clip 0x0, clip(182)
  tail 0x0, native_stagger_tail
  .zero 4 ; non-executable guard

; Branches use linked names.
slam_victim:
  speed_negative 0x0, 0xffffeb86
  sound_special 0x0
  prefixed_finish 0x200, clip(175)
  wait 0x8
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
air_victim_flip:
  prefixed_sample 0x200, clip(198)
  motion 0x0, 0x0, 0x1999, 0x28f
  native_context 0x0
  call 0x0, native_touchable
  call 0x0, native_disarm_self
  sound_special 0x0
  prefixed_to 0x20c, clip(198)
  speed_negative 0x0, 0xccc
  tail 0x0, native_air_flip_getup
  jump 0, word(air_victim_lift)
  .zero 4 ; non-executable guard

; Branches use linked names.
air_victim_default:
  motion 0x0, 0x0, 0x3333, 0x28f
  native_context 0x0
  sound_special 0x0
  forward 0x0
  wait 0x4
  tail 0x0, native_mmove_air_getup
  jump 0, word(scorpion_teleport_punch_grounded_victim)
  .zero 4 ; non-executable guard

; Branches use linked names.
teleport_punch_airborne_victim:
  effect 0x9
  effect 0x8
  sound_context 0x1
  sound_b 0x3
  motion 0x0, 0xfffff852, 0xffffdc29, 0x28f
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
victim_disarm_fall:
  forward_count 0x8
  speed_negative 0x0, 0x147a
  forward_count 0x17
  landing 0x0
  forward 0x0
  tail 0x0, native_throw_victim_stop_and_return
  jump 0, word(victim_friction_impact)
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_victim_slide_release:
  prefixed_to 0x205, clip(171)
  speed_negative 0x0, 0x1999
  forward_to 0x18
  landing 0x0
  forward 0x0
  tail 0x0, native_unresolved_lp_victim_slide_release
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_victim_sliding_impact_release:
  speed_negative 0x0, 0x2666
  prefixed_to 0x20c, clip(172)
  sound_special 0x0
  forward_count 0x6
  landing 0x0
  forward 0x0
  tail 0x0, native_lp_sliding_impact_flip_getup
  jump 0, word(lp_victim_impact_release)
  .zero 4 ; non-executable guard

; Branches use linked names.
lp_victim_two_speed_slide_release:
  prefixed_to 0x204, clip(176)
  speed_negative 0x0, 0x3333
  forward_count 0x8
  speed_negative 0x0, 0x1999
  forward_count 0x8
  landing 0x0
  forward 0x0
  tail 0x0, native_lp_two_speed_slide_double_flip_roll_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
hit_clip_324_recovery:
  effect 0x9
  sound_a 0x14
  clip 0x0, clip(324)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
hit_clip_325_recovery:
  effect 0x9
  sound_a 0x14
  clip 0x0, clip(325)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
launch_clip_131_no_impact:
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
  task_callback 0x0
  play_clip 0x28, clip(131)
  native_context 0x0
  forward 0x0
  ready 0x0

; Branches use linked names.
grounded_hit_clip_132_back_getup:
  sound_b 0x1
  speed_negative 0x0, 0xa3d
  open_clip 0x0, clip(132)
  double_to 0xe
  landing 0x0
  forward 0x0
  tail 0x0, native_getup_back

; Branches use linked names.
launch_clip_179_flip_getup:
  sound_b 0x3
  motion 0x0, 0x51e, 0xffffb334, 0x353
  task_callback 0x0
  play_clip 0x27, clip(179)
  native_context 0x0
  forward 0x0
  tail 0x0, native_sub_zero_lp_throw_victim_getup
  jump 0, word(quan_chi_skull_reaction)
  .zero 4 ; non-executable guard

; Branches use linked names.
guard_grounded_stagger:
  speed_negative 0x0, 0xccc
  prefixed_finish 0x200, clip(182)
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
airborne_thaw_script:
  gravity 0x0, 0x147
  native_context 0x0
  tail 0x0, native_thaw_landing
  .zero 4 ; non-executable guard
.alias airborne_thaw_script_operand_1 airborne_thaw_script + 4

; Branches use linked names.
reaction_clip_127_friction:
  sound_a 0x26
  speed_negative 0x0, 0x5999
  play_clip 0x1b, clip(127)
  clear_handler 0x0
  speed_negative 0x0, 0xffffeb86
  forward_count 0x5
  landing 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
block_unarmed_pose_cycle:
  bind_submit 0x3, clip(321)
  wait 0x1
  bind_submit 0x5, clip(321)
  wait 0x1
  bind_submit 0x7, clip(321)
  wait 0x1
  bind_submit 0x9, clip(321)
  wait 0x1
  bind_submit 0x7, clip(321)
  wait 0x1
  bind_submit 0x5, clip(321)
  wait 0x1
  bind_submit 0x3, clip(321)
  wait 0x1
  tail 0x0, native_block_cycle
  .zero 4 ; non-executable guard

; Branches use linked names.
block_armed_recovery:
  prefixed_finish 0x200, clip(247)
  playback_pair 0x0
  landing 0x0
  tail 0x0, native_standing_block_hold

; Branches use linked names.
lk_thrown_release:
  cursor_set 0x2d
  playback_pair 0x29
  forward_to 0x2e
  playback_pair 0x29
  forward_to 0x2e
  playback_pair 0x29
  forward_to 0x2e
  tail 0x0, native_sub_zero_script_lk_victim_rise
  .zero 4 ; non-executable guard

; Branches use linked names.
snare_reaction:
  open_clip 0x6, clip(199)

; Branches use linked names.
snare_reaction_forward_to_1:
  forward_to 0xb
  playback_pair 0x7
  jump 0x0, word(snare_reaction_forward_to_1)
  .zero 4 ; non-executable guard

; Branches use linked names.
pair_release:
  motion 0x0, 0xfffffae2, 0x0, 0x28f
  task_callback 0x0
  prefixed_finish 0x200, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, native_mmove_side_getup
  .zero 4 ; non-executable guard

; Branches use linked names.
bicycle_capture:
  clip 0x0, clip(136)
  tail 0x0, native_bicycle_cycle
  jump 0, word(raiden_torpedo_contact)
  .zero 4 ; non-executable guard

; Branches use linked names.
mmove_side_getup:
  open_clip 0x0, clip(131)
  cursor_set 0x2d
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
reaction_park_task:
  park 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
stage_victim:
  spawn_effect 0x0
  sound_context 0x0
  motion_block 0x0, 0xfffffd71, 0x28f5, 0xffffcccd, 0x0
  open_clip 0x0, clip(128)
  double_to 0x1f
  tail 0x0, native_stage_impale

; Branches use linked names.
stage_impaled:
  landing 0x0
  effect 0x8
  effect 0x8
  effect 0x14
  sound_context 0x7
  spawn_effect 0x6
  sound_b 0xc
  wait 0x80
  native_release 0x29
  wait 0x32
  tail 0x0, native_stage_complete
  .zero 4 ; non-executable guard

; Branches use linked names.
shared_clip_entry:
  clip 0x0, clip(196)
  tail 0x0, native_crouch_return
  .zero 4 ; non-executable guard

; Branches use linked names.
sword_back_high_punch_hit_recovery:
  forward_to 0x18
  playback_pair 0xf
  forward_to 0x18
  playback_pair 0xf
  forward 0x0
  ready 0x0

; Branches use linked names.
sword_back_high_punch_block_recovery:
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Branches use linked names.
sword_attack:
  open_clip 0x0, clip(227)
  double_to 0x10
  sound_c 0x1f
  double_to 0x1a
  forward_to 0x1d
  tail 0x0, native_cage_weapon_back_lp_contact
  .zero 4 ; non-executable guard

; Branches use linked names.
sword_back_lp_script:
  play_clip 0x9, clip(237)
  sound_c 0x22
  speed 0x0, 0x1999
  forward_count 0x8
  weapon_query 0x2
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  forward_count 0x1
  weapon_query 0x2
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  forward_count 0x1
  weapon_query 0x2
  contact 0x0, word(sword_back_hp_script_action_recovery_1)
  landing 0x0
  forward_count 0x2
  wait 0x4
  action_recovery 0x0
  landing 0x0
  forward 0x0
  tail 0x0, native_return_to_player_or_cpu_control
  .zero 4 ; non-executable guard

; Branches use linked names.
sword_back_hp_script_action_recovery_1:
  action_recovery 0x0
  landing 0x0
  forward 0x0
  tail 0x0, native_return_to_player_or_cpu_control
  .zero 4 ; non-executable guard

; Branches use linked names.
air_fireball:
  sound_a 0x21
  block_sound_alt 0x0
  prefixed_to 0x209, clip(24)
  spawn 0x0, 0x4b, native_air_fireball_spawn
  backward_count 0x5
  tail 0x0, native_fireball_fall
  .zero 4 ; non-executable guard
