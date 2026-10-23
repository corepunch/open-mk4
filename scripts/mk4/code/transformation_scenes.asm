; Transformation/second-finisher entry streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: sub_zero.fatality_2 (victim).
sub_zero_fatality_2_victim:
  prefixed_to WITH_CLIP + 20, clip(206)  ; bind clip prefix; inline sample/count 20
  sound_special 0
  forward_to 90
  sound_special 0
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END

; Catalog: sub_zero.fatality_2 (breath).
sub_zero_fatality_2_breath:
  sound_indexed 4
  play_clip 10, clip(552)
  spawn 0, 0x0, MK4_NATIVE_BREATH_SCENE_DECORATION
  sound_c 63
  native_release 56
  loop 3
  cursor_set 10
  forward_to 29
  repeat 0
  native_release 57
  sound_c 64
  forward_to 83
  scene_active 0
  spawn 0, 0x0, MK4_NATIVE_BREATH_SHATTER_DECORATION
  loop 5
  cursor_set 83
  forward_to 93
  repeat 0
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH

; Catalog: sub_zero.fatality_2 (approach).
sub_zero_breath_approach:
  native_release 58
  wait 20
  speed_negative 0, 0xffffd99a
  sound_c 15
  open_clip 0, clip(95)
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_2 (uppercut).
breath_resume:
  open_clip 0, clip(258)
  cursor_set 25
  forward 0
  sound_c 28
  play_clip 5, clip(293)
  sound_c 59
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER

; Catalog: sub_zero.fatality_2 (finish).
sub_zero_breath_shatter:
  loop 8
  forward_to 8
  playback_pair 6
  repeat 0
  forward 0
  wait 60
  tail 0, MK4_NATIVE_VICTORY_CONTINUE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_2 (owner barrage).
reiko_second_fatality:
  sound_indexed 62
  native_release 64
  wait 0
  scene_active 0
  play_clip 29, clip(553)
  spawn 0, 0x0, MK4_NATIVE_REIKO_SECOND_STARS
  loop 9
  cursor_set 29
  forward_to 38
  repeat 0
  cursor_set 29
  forward_to 33
  wait 20
  forward 0
  tail 0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_2 (victim stagger).
victim_stagger:
  blend 0, clip(554)
  open_clip 0, clip(554)
  loop 4
  cursor_set 0
  forward_to 20
  repeat 0
  native_release 65
  forward_to 135
  tail 0, MK4_NATIVE_REIKO_SECOND_VICTIM_FALL

; Catalog: reiko.fatality_2 (victim fall).
reiko_second_victim_fall:
  forward_to 145
  sound_special 0
  forward 0
  sound_special 0
  native_release 66
  tail 0, MK4_NATIVE_REIKO_SECOND_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_2 (scene owner).
jarek_fatality_2:
  native_release 59
  play_clip 1, clip(577)
  scene_active 0
  half_count 6
  call 0, MK4_NATIVE_JAREK_SECOND_GLOW
  half_count 19
  sound_b 1
  tail 0, MK4_NATIVE_JAREK_SECOND_HANDOFF
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_2 (laser owner).
jarek_second_handoff:
  half_count 12
  sound_c 11
  half_count 20
  sound_c 11
  half_count 20
  sound_c 11
  half_count 20
  sound_c 11
  half_count 20
  sound_c 11
  tail 0, MK4_NATIVE_VICTORY_CONTINUE

; Catalog: jarek.fatality_2 (victim).
victim_start:
  sound_c 11
  play_clip 16, clip(182)
  backward_count 15
  forward 0
  sound_b 12
  spawn 0, 0x0, MK4_NATIVE_JAREK_SECOND_HEAD_FALL
  prefixed_to WITH_CLIP + 20, clip(206)  ; bind clip prefix; inline sample/count 20
  sound_special 0
  forward_to 90
  forward 0
  tail 0, MK4_NATIVE_JAREK_SECOND_VICTIM_HOLD
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_2 (transformation).
beast_start:
  native_release 60
  play_clip 24, clip(12)
  sound_c 75
  sound_c 74
  spawn 0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_TRANSFORM_EFFECT
  forward 0
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_2 (creature approach).
scorpion_beast_begin:
  scene_active 0
  clip 0, clip(578)
  native_release 61
  clip 0, clip(579)
  sound_roster 1
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_2 (creature attack).
scorpion_fatality_2_creature_attack:
  clip 0, clip(580)
  loop 5
  call 0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VOICE
  backward_count 8
  forward_count 8
  repeat 0
  spawn_effect 6
  sound_context 18
  sound_roster 8
  clip 0, clip(581)
  native_release 62
  bind_cursor 0, clip(578)
  tail 0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP

; Catalog: scorpion.fatality_2 (victim).
scorpion_fatality_2_victim:
  play_clip 54, clip(584)
  sound_b 1
  sound_context 7
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES

; Catalog: scorpion.fatality_2 (victim release).
scorpion_beast_victim_pieces:
  forward 0
  loop 5
  backward_count 8
  forward_count 8
  repeat 0
  spawn 0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_GORE
  clip 0, clip(585)
  tail 0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
  .zero 4  ; non-executable guard
