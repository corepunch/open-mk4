; Finisher performers, victims and body-part task streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: fujin.fatality_2 (owner wind).
fujin_second_fatality:
  wait 60
  sound_indexed 60
  native_release 55
  prefixed_finish WITH_CLIP + 0, clip(547)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0, clip(548)
  wait 8
  scene_active 0
  spawn 0, 0x26, MK4_NATIVE_FUJIN_SECOND_BLOOD
  native_return 181
  park 0

; Catalog: reptile.fatality_2 (owner acid).
reptile_fatality_2:
  native_release 53
  spawn 0, 0x26, MK4_NATIVE_SCENE_HOVER
  bind_cursor 0, clip(493)
  wait 150
  scene_active 0
  native_return 180
  spawn 0, 0x0, MK4_NATIVE_REPTILE_SECOND_ACID
  sound_indexed 61
  prefixed_finish WITH_CLIP + 0, clip(544)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(545)  ; bind clip prefix; inline sample/count 0
  clip_operation 0, clip(544)
  bind_cursor 0, clip(493)
  park 0

; Catalog: kai.fatality_2 (owner levitation).
kai_fatality_2:
  bind_cursor 0, clip(493)
  scene_event 45
  wait 40
  spawn 0, 0x26, MK4_NATIVE_SCENE_HOVER
  native_release 46
  wait 60
  sound_indexed 58
  scene_active 0
  wait 30
  call 0, MK4_NATIVE_KAI_SECOND_CHILD_WALK
  prefixed_finish WITH_CLIP + 0, clip(486)  ; bind clip prefix; inline sample/count 0
  sound_c 51
  spawn 0, 0x26, MK4_NATIVE_KAI_SECOND_SKULL
  bind_cursor 0, clip(487)
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.fatality_2 (setup events).
liu_kang_start_fatality:
  scene_event 42
  wait 15
  scene_event 43
  wait 20
  scene_event 44
  wait 60
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.fatality_2 (paired opening/projectile).
liu_kang_fireball_fatality_paired_opening:
  call 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN
  sound_b 10
  open_peer_clip 0, clip(539)
  open_clip 0, clip(537)
  play_pair 37
  scene_active 0
  native_return 176
  forward 0
  wait 18
  sound_a 33
  sound_indexed 57
  prefixed_finish WITH_CLIP + 0, clip(538)  ; bind clip prefix; inline sample/count 0
  spawn 0, 0x26, MK4_NATIVE_LIU_FINISHER_PROJECTILE
  playback_pair 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_2 (camera intro).
shinnok_fatality_2:
  scene_event 48
  wait 15
  scene_event 49
  wait 20
  scene_event 50
  wait 20
  native_release 51
  wait 20
  native_return 179
  tail 0, MK4_NATIVE_SHINNOK_SECOND_VANISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_2 (first hand).
shinnok_fatality_2_first_hand:
  prefixed_to WITH_CLIP + 120, clip(541)  ; bind clip prefix; inline sample/count 120
  wait 120
  call 0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
  call 0, MK4_NATIVE_SHINNOK_SECOND_GORE_7_PIECES
  forward 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_2 (second hand).
shinnok_fatality_2_second_hand:
  prefixed_to WITH_CLIP + 120, clip(541)  ; bind clip prefix; inline sample/count 120
  spawn_effect 6
  sound_c 105
  tail 0, MK4_NATIVE_SHINNOK_SECOND_CLAP

; Catalog: shinnok.fatality_2 (second hand tail).
shinnok_second_clap:
  wait 120
  call 0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
  call 0, MK4_NATIVE_SHINNOK_SECOND_GORE_8_PIECES
  forward 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fan_fatality (performer); raiden.fan_fatality (performer); sonya.fan_fatality (performer) (also shared by other catalog entries).
jax_fan_fatality_performer:
  prefixed_to WITH_CLIP + 69, clip(415)  ; bind clip prefix; inline sample/count 69
  tail 0, MK4_NATIVE_FAN_TAIL

; Catalog: jarek.fan_fatality (male-victim owner cycle).
jarek_fan_fatality_male_victim_owner_cycle:
  forward_to 8
  cursor_set 0
  tail 0, MK4_NATIVE_FAN_MALE_VICTIM_OWNER_CYCLE

; Catalog: scorpion.fan_fatality (owner cycle); raiden.fan_fatality (owner cycle); sonya.fan_fatality (owner cycle) (also shared by other catalog entries).
jax_fan_fatality_owner_cycle:
  forward_to 77
  cursor_set 69
  tail 0, MK4_NATIVE_FAN_FEMALE_VICTIM_OWNER_CYCLE

; Catalog: scorpion.fan_fatality (owner release); raiden.fan_fatality (owner release); sonya.fan_fatality (owner release) (also shared by other catalog entries).
jax_fan_fatality_owner_release:
  cursor_set 78
  forward_to 107
  tail 0, MK4_NATIVE_FAN_VICTORY

; Catalog: scorpion.fan_fatality (victory); raiden.fan_fatality (victory); sonya.fan_fatality (victory) (also shared by other catalog entries).
fan_victory:
  forward 0
  prefixed_finish WITH_CLIP + 0, clip(412)  ; bind clip prefix; inline sample/count 0

fan_victory_cursor_set_1:
  cursor_set 18
  forward 0
  jump 0, word(fan_victory_cursor_set_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fan_fatality (victim); raiden.fan_fatality (victim); sonya.fan_fatality (victim) (also shared by other catalog entries).
jax_fan_fatality_victim:
  prefixed_to WITH_CLIP + 69, clip(416)  ; bind clip prefix; inline sample/count 69

jax_fan_fatality_victim_forward_to_1:
  forward_to 77
  cursor_set 69
  jump 0, word(jax_fan_fatality_victim_forward_to_1)

; Catalog: scorpion.fan_fatality (victim release); raiden.fan_fatality (victim release); sonya.fan_fatality (victim release) (also shared by other catalog entries).
jax_fan_fatality_victim_release:
  cursor_set 78
  forward_to 107
  tail 0, MK4_NATIVE_FAN_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.fatality_1 (performer); kitana.fatality_1 (performer).
sonya_first_fatality:
  prefixed_to WITH_CLIP + 42, clip(388)  ; bind clip prefix; inline sample/count 42
  tail 0, MK4_NATIVE_SONYA_SCRIPT_FIRST_RELEASE

; Catalog: sonya.fatality_1 (owner loop); kitana.fatality_1 (owner loop).
sonya_fatality_1_owner_loop:
  forward 0
  cursor_set 84
  jump 0, word(sonya_fatality_1_owner_loop)
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.fatality_1 (performer).
raiden_first_fatality:
  prefixed_finish WITH_CLIP + 0, clip(392)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE

; Catalog: raiden.fatality_1 (owner release).
raiden_fatality_1_owner_release:
  playback_pair 20
  forward 0
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (performer).
burn_start:
  prefixed_to WITH_CLIP + 43, clip(390)  ; bind clip prefix; inline sample/count 43
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_BEGIN

; Catalog: scorpion.fatality_1 (owner cue).
scorpion_burn_begin:
  forward_to 105
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (owner flame).
scorpion_fatality_1_owner_flame:
  forward_to 123
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (owner repeat).
scorpion_burn_repeat1:
  cursor_set 105
  forward_to 123
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2

; Catalog: scorpion.fatality_1 (owner recovery).
scorpion_burn_repeat2:
  cursor_set 105
  forward_to 123
  forward 0
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTORY
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (victory).
scorpion_burn_victory:
  prefixed_finish WITH_CLIP + 0, clip(115)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(116)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(117)  ; bind clip prefix; inline sample/count 0
  bind_cursor 0x200, clip(118)
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (victim).
scorpion_fatality_1_victim:
  prefixed_to WITH_CLIP + 12, clip(391)  ; bind clip prefix; inline sample/count 12
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE

; Catalog: scorpion.fatality_1 (victim cue).
scorpion_burn_victim_cue:
  forward_to 14
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (victim repeat).
scorpion_burn_victim_repeat:
  forward 0
  cursor_set 63
  forward 0
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (victim collapse).
scorpion_burn_victim_blend:
  prefixed_to WITH_CLIP + 19, clip(206)  ; bind clip prefix; inline sample/count 19
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT

; Catalog: scorpion.fatality_1 (victim end).
scorpion_burn_victim_impact:
  forward_to 89
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.fatality_1 (victim remaining clip).
scorpion_burn_victim_settle:
  forward 0
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (setup).
sub_zero_fatality_1_setup:
  wait 50
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (first pull).
sub_zero_fatality_1_first_pull:
  prefixed_to WITH_CLIP + 17, clip(385)  ; bind clip prefix; inline sample/count 17
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL

; Catalog: sub_zero.fatality_1 (pull forward).
sub_zero_fatality_1_pull_forward:
  forward_to 42
  cursor_set 41
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1

; Catalog: sub_zero.fatality_1 (pull reverse).
sub_zero_fatality_1_pull_reverse:
  playback_pair 31
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (second pull).
sub_zero_fatality_1_second_pull:
  forward_to 42
  cursor_set 41
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3

; Catalog: sub_zero.fatality_1 (second reverse).
sub_zero_fatality_1_second_reverse:
  playback_pair 31
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (head release).
sub_zero_fatality_1_head_release:
  forward_to 44
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (owner loop).
sub_zero_fatality_1_owner_loop:
  forward_to 60

sub_zero_fatality_1_owner_loop_forward_1:
  forward 0
  cursor_set 96
  jump 0, word(sub_zero_fatality_1_owner_loop_forward_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (victim).
sub_zero_fatality_1_victim:
  prefixed_to WITH_CLIP + 15, clip(387)  ; bind clip prefix; inline sample/count 15
  cursor_set 14
  playback_pair 4
  forward_to 15
  cursor_set 14
  playback_pair 4
  forward_to 17
  forward_to 48
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (victim finish).
sub_zero_fatality_1_victim_finish:
  forward_to 63
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.fatality_1 (victim cleanup).
sub_zero_fatality_1_victim_cleanup:
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_1 (female-victim owner).
jarek_fatality_1_female_victim_owner:
  prefixed_to WITH_CLIP + 43, clip(403)  ; bind clip prefix; inline sample/count 43
  jump 0, word(jarek_fatality_1_male_victim_owner_tail_1)

; Catalog: jarek.fatality_1 (male-victim owner).
jarek_fatality_1_male_victim_owner:
  prefixed_to WITH_CLIP + 43, clip(402)  ; bind clip prefix; inline sample/count 43

jarek_fatality_1_male_victim_owner_tail_1:
  tail 0, MK4_NATIVE_JAREK_FIRST_HEART
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_1 (owner hold).
jarek_first_heart:
  forward 0
  bind_cursor 0x200, clip(405)
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_1 (victim reach).
victim_reach:
  prefixed_to WITH_CLIP + 43, clip(404)  ; bind clip prefix; inline sample/count 43
  tail 0, MK4_NATIVE_JAREK_FIRST_VICTIM_RIP

; Catalog: jarek.fatality_1 (victim rip).
jarek_first_victim_rip:
  forward_to 147
  tail 0, MK4_NATIVE_JAREK_FIRST_VICTIM_IMPACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_1 (victim fall).
jarek_first_victim_impact:
  forward_to 194
  tail 0, MK4_NATIVE_JAREK_FIRST_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.fatality_1 (victim finish).
jarek_first_victim_end:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (scene setup).
cage_fatality_1_scene_setup:
  forward 0
  tail 0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (scene loop).
cage_fatality_1_scene_loop:
  prefixed_finish WITH_CLIP + 0, clip(443)  ; bind clip prefix; inline sample/count 0
  jump 0, word(cage_fatality_1_scene_loop)

; Catalog: cage.fatality_1 (continuation 4e12c0).
cage_torso_rip_male_victim:
  prefixed_to WITH_CLIP + 58, clip(410)  ; bind clip prefix; inline sample/count 58
  jump 0, word(cage_torso_rip_female_victim_tail_1)

; Catalog: cage.fatality_1 (continuation 4e12d0).
cage_torso_rip_female_victim:
  prefixed_to WITH_CLIP + 58, clip(411)  ; bind clip prefix; inline sample/count 58

cage_torso_rip_female_victim_tail_1:
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_59

; Catalog: cage.fatality_1 (continuation 4e12e0).
cage_first_victim_59:
  forward_to 130
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_131
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e12f0).
cage_first_victim_131:
  forward_to 147
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_148
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1300).
cage_first_victim_148:
  forward_to 165
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_166
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1310).
cage_first_victim_166:
  forward_to 184
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_185
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1320).
cage_first_victim_185:
  forward_to 205
  tail 0, MK4_NATIVE_CAGE_FIRST_VICTIM_206
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1330).
cage_first_victim_206:
  forward_to 224
  tail 0, MK4_NATIVE_CAGE_FIRST_SEVER
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1340).
cage_torso_rip_lower_finish:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1350).
cage_first_sever:
  prefixed_to WITH_CLIP + 58, clip(408)  ; bind clip prefix; inline sample/count 58
  tail 0, MK4_NATIVE_CAGE_FIRST_TORSO_HALF

; Catalog: cage.fatality_1 (continuation 4e1360).
cage_first_torso_half:
  forward_to 122
  tail 0, MK4_NATIVE_CAGE_FIRST_TORSO_123
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1370).
cage_first_torso_123:
  forward_to 138
  tail 0, MK4_NATIVE_CAGE_FIRST_TORSO_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_1 (continuation 4e1380).
cage_first_torso_finish:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.fatality_1 (owner opening).
jax_fatality_1_owner_opening:
  prefixed_finish WITH_CLIP + 0, clip(384)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_JAX_FATALITY_1_VICTORY

; Catalog: jax.fatality_1 (owner loop).
jax_fatality_1_owner_loop:
  prefixed_finish WITH_CLIP + 0, clip(443)  ; bind clip prefix; inline sample/count 0
  jump 0, word(jax_fatality_1_owner_loop)

; Catalog: jax.fatality_1 (victim listed roster).
arm_tear_victim_clip_204:
  prefixed_to WITH_CLIP + 49, clip(204)  ; bind clip prefix; inline sample/count 49
  jump 0, word(arm_tear_victim_clip_400_tail_1)

; Catalog: jax.fatality_1 (victim other roster).
arm_tear_victim_clip_400:
  prefixed_to WITH_CLIP + 49, clip(400)  ; bind clip prefix; inline sample/count 49

arm_tear_victim_clip_400_tail_1:
  tail 0, MK4_NATIVE_JAX_FATALITY_1_GRAB

; Catalog: jax.fatality_1 (victim grab).
jax_fatality_1_grab:
  forward_to 78
  tail 0, MK4_NATIVE_JAX_FATALITY_1_FIRST_ARM
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.fatality_1 (victim first tear).
jax_fatality_1_first_arm:
  forward_to 128
  tail 0, MK4_NATIVE_JAX_FATALITY_1_SECOND_ARM
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.fatality_1 (victim finish).
jax_fatality_1_second_arm:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (owner opening).
kai_fatality_1_owner_opening:
  prefixed_to WITH_CLIP + 45, clip(440)  ; bind clip prefix; inline sample/count 45
  tail 0, MK4_NATIVE_KAI_FIRST_OWNER_GRAB

; Catalog: kai.fatality_1 (owner grab).
kai_first_owner_grab:
  forward_to 123
  tail 0, MK4_NATIVE_KAI_FIRST_OWNER_SPIN
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (owner spin).
kai_first_owner_spin:
  prefixed_finish WITH_CLIP + 0, clip(445)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(445)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_KAI_FIRST_OWNER_TEAR

; Catalog: kai.fatality_1 (owner victory).
kai_fatality_1_owner_victory:
  forward 0

kai_fatality_1_owner_victory_prefixed_finish_1:
  prefixed_finish WITH_CLIP + 0, clip(443)  ; bind clip prefix; inline sample/count 0
  jump 0, word(kai_fatality_1_owner_victory_prefixed_finish_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (victim opening).
kai_fatality_1_victim_opening:
  prefixed_to WITH_CLIP + 70, clip(441)  ; bind clip prefix; inline sample/count 70
  tail 0, MK4_NATIVE_KAI_FIRST_VICTIM_71

; Catalog: kai.fatality_1 (victim turn).
kai_first_victim_71:
  forward_to 123
  prefixed_finish WITH_CLIP + 0, clip(444)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_KAI_FIRST_VICTIM_TURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (victim second turn).
kai_first_victim_turn:
  prefixed_finish WITH_CLIP + 0, clip(444)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_KAI_FIRST_VICTIM_SEVER

; Catalog: kai.fatality_1 (victim finish).
kai_fatality_1_victim_finish:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (torso opening).
kai_fatality_1_torso_opening:
  prefixed_to WITH_CLIP + 202, clip(442)  ; bind clip prefix; inline sample/count 202
  tail 0, MK4_NATIVE_KAI_FIRST_TORSO_203

; Catalog: kai.fatality_1 (torso fall).
kai_first_torso_203:
  forward_to 242
  tail 0, MK4_NATIVE_KAI_FIRST_TORSO_IMPACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_1 (torso finish).
kai_first_torso_impact:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (owner opening).
quan_chi_fatality_1_owner_opening:
  prefixed_to WITH_CLIP + 40, clip(420)  ; bind clip prefix; inline sample/count 40
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_CUE_40

; Catalog: quan_chi.fatality_1 (owner 117).
quan_chi_fatality_cue_40:
  forward_to 117
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_LEG
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (owner 179).
quan_chi_fatality_leg:
  forward_to 179
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_HIT_179
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (owner 241).
quan_chi_fatality_hit_179:
  forward_to 241
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_HIT_241
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (owner finish).
quan_chi_fatality_hit_241:
  forward 0
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_1_OWNER_BEAT
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (owner beat).
quan_chi_fatality_1_owner_beat:
  prefixed_to WITH_CLIP + 18, clip(421)  ; bind clip prefix; inline sample/count 18
  tail 0, MK4_NATIVE_QUAN_CHI_FATALITY_BEAT

; Catalog: quan_chi.fatality_1 (owner beat end).
quan_chi_fatality_beat:
  forward 0
  jump 0, word(quan_chi_fatality_1_owner_beat)
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.fatality_1 (victim).
victim_stream:
  prefixed_finish WITH_CLIP + 0, clip(424)  ; bind clip prefix; inline sample/count 0

victim_stream_prefixed_finish_1:
  prefixed_finish WITH_CLIP + 0, clip(417)  ; bind clip prefix; inline sample/count 0
  jump 0, word(victim_stream_prefixed_finish_1)

; Catalog: quan_chi.fatality_1 (torn leg).
leg_entry:
  prefixed_finish WITH_CLIP + 0, clip(422)  ; bind clip prefix; inline sample/count 0

leg_entry_prefixed_finish_1:
  prefixed_finish WITH_CLIP + 0, clip(423)  ; bind clip prefix; inline sample/count 0
  jump 0, word(leg_entry_prefixed_finish_1)

; Catalog: tanya.fatality_1 (owner kiss).
owner_kiss_start:
  prefixed_to WITH_CLIP + 52, clip(395)  ; bind clip prefix; inline sample/count 52
  tail 0, MK4_NATIVE_TANYA_FIRST_OWNER_53

; Catalog: tanya.fatality_1 (owner 53).
tanya_first_owner_53:
  forward_to 104
  tail 0, MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_1 (owner 396).
tanya_first_owner_drift:
  forward 0
  prefixed_finish WITH_CLIP + 0, clip(396)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_1 (owner replay).
tanya_first_owner_loop:
  prefixed_finish 0
  tail 0, MK4_NATIVE_TANYA_FIRST_OWNER_LOOP
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_1 (victim swell).
victim_scene:
  prefixed_finish WITH_CLIP + 0, clip(397)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL

; Catalog: tanya.fatality_1 (victim pulse).
tanya_fatality_1_victim_pulse:
  playback_pair 188
  forward 0
  tail 0, MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT

; Catalog: tanya.fatality_1 (victim shake).
tanya_fatality_1_victim_shake:
  prefixed_to WITH_CLIP + 9, clip(409)  ; bind clip prefix; inline sample/count 9
  forward_to 11
  playback_pair 9
  forward_to 11
  playback_pair 9
  forward_to 11
  playback_pair 9
  forward_to 11
  playback_pair 9
  forward_to 21
  forward_to 23
  playback_pair 21
  forward_to 23
  playback_pair 21
  forward_to 23
  playback_pair 21
  forward_to 23
  playback_pair 21
  forward_to 38
  forward_to 40
  playback_pair 38
  forward_to 40
  playback_pair 38
  forward_to 40
  playback_pair 38
  forward_to 40
  playback_pair 38
  forward_to 55
  forward_to 57
  playback_pair 55
  forward_to 57
  playback_pair 55
  forward_to 57
  playback_pair 55
  forward_to 57
  playback_pair 55
  forward_to 64
  forward_to 66
  playback_pair 64
  forward_to 66
  playback_pair 64
  forward_to 66
  playback_pair 64
  forward_to 72
  forward_to 74
  playback_pair 72
  forward_to 74
  playback_pair 72
  tail 0, MK4_NATIVE_TANYA_FIRST_VICTIM_BURST
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_1 (victim loop).
tanya_first_victim_burst:
  forward 0
  playback_pair 72
  jump 0, word(tanya_first_victim_burst)
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.fatality_1 (wind opening).
fujin_first_fatality:
  half_restart WITH_CLIP + 0, clip(398)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_FUJIN_FIRST_LOOP_START

; Catalog: fujin.fatality_1 (wind cycle).
wind_finish_cycle_script:
  prefixed_finish WITH_CLIP + 0, clip(399)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_FUJIN_FIRST_CYCLE
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.fatality_1 (crossbow draw).
wind_finish_exit_script:
  prefixed_finish WITH_CLIP + 0, clip(210)  ; bind clip prefix; inline sample/count 0
  prefixed_finish WITH_CLIP + 0, clip(212)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_FUJIN_FIRST_SHOOT

; Catalog: fujin.fatality_1 (crossbow shot).
fujin_fatality_1_crossbow_shot:
  prefixed_finish WITH_CLIP + 0, clip(213)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_TASK_RETIRE

dragon_victim_script:
  prefixed_finish WITH_CLIP + 0, clip(419)  ; bind clip prefix; inline sample/count 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (owner tear).
owner_tear:
  half_count 0x220, clip(425)
  forward_to 43
  playback_pair 41
  forward_to 43
  playback_pair 41
  forward 0
  tail 0, MK4_NATIVE_REIKO_FIRST_VICTORY
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (owner victory).
owner_victory_loop:
  prefixed_finish WITH_CLIP + 0, clip(443)  ; bind clip prefix; inline sample/count 0
  jump 0, word(owner_victory_loop)

; Catalog: reiko.fatality_1 (victim).
victim_place:
  prefixed_to WITH_CLIP + 37, clip(428)  ; bind clip prefix; inline sample/count 37
  tail 0, MK4_NATIVE_REIKO_FIRST_TEAR

; Catalog: reiko.fatality_1 (victim after tear).
reiko_first_tear:
  cursor_set 38
  forward_to 117
  tail 0, MK4_NATIVE_REIKO_FIRST_VICTIM_118

; Catalog: reiko.fatality_1 (victim 118).
reiko_first_victim_118:
  forward_to 165
  tail 0, MK4_NATIVE_REIKO_FIRST_VICTIM_165
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (victim finish).
reiko_first_victim_165:
  forward 0
  tail 0, MK4_NATIVE_REIKO_LEG_TEAR_VICTIM_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (torso).
torso_start:
  prefixed_to WITH_CLIP + 29, clip(429)  ; bind clip prefix; inline sample/count 29
  tail 0, MK4_NATIVE_REIKO_FIRST_TORSO_29

; Catalog: reiko.fatality_1 (torso finish).
reiko_first_torso_29:
  forward 0
  tail 0, MK4_NATIVE_REIKO_FIRST_TORSO_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (head).
head_start:
  prefixed_to WITH_CLIP + 41, clip(430)  ; bind clip prefix; inline sample/count 41
  cursor_set 21
  forward_to 41
  cursor_set 21
  forward_to 41
  cursor_set 21
  forward_to 41
  cursor_set 21
  forward_to 41
  cursor_set 21
  forward_to 58
  tail 0, MK4_NATIVE_REIKO_FIRST_HEAD_59

; Catalog: reiko.fatality_1 (head 59).
reiko_first_head_59:
  forward_to 82
  tail 0, MK4_NATIVE_REIKO_FIRST_HEAD_83
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (head finish).
reiko_first_head_83:
  forward 0
  tail 0, MK4_NATIVE_REIKO_LEG_TEAR_HEAD_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (left arm).
left_arm_start:
  prefixed_to WITH_CLIP + 25, clip(427)  ; bind clip prefix; inline sample/count 25
  cursor_set 4
  forward_to 55
  tail 0, MK4_NATIVE_REIKO_FIRST_LEFT_ARM_56

; Catalog: reiko.fatality_1 (left arm 56).
reiko_first_left_arm_56:
  forward_to 66
  tail 0, MK4_NATIVE_REIKO_FIRST_LEFT_ARM_LAND
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (left arm finish).
reiko_first_left_arm_land:
  forward 0
  tail 0, MK4_NATIVE_REIKO_LEG_TEAR_LEFT_ARM_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (right arm).
right_arm_start:
  prefixed_to WITH_CLIP + 29, clip(426)  ; bind clip prefix; inline sample/count 29
  cursor_set 4
  forward_to 29
  cursor_set 4
  forward_to 29
  cursor_set 4
  forward_to 43
  tail 0, MK4_NATIVE_REIKO_FIRST_RIGHT_ARM_44

; Catalog: reiko.fatality_1 (right arm 44).
reiko_first_right_arm_44:
  forward_to 68
  tail 0, MK4_NATIVE_REIKO_FIRST_RIGHT_ARM_LAND
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.fatality_1 (right arm finish).
reiko_first_right_arm_land:
  forward 0
  tail 0, MK4_NATIVE_REIKO_LEG_TEAR_RIGHT_ARM_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.fatality_1 (owner startup).
owner_entry:
  prefixed_to WITH_CLIP + 10, clip(432)  ; bind clip prefix; inline sample/count 10
  tail 0, MK4_NATIVE_REPTILE_FIRST_OWNER_CYCLE

; Catalog: reptile.fatality_1 (owner cycles).
reptile_first_owner_cycle:
  forward_to 138
  cursor_set 99
  forward_to 138
  cursor_set 99
  forward_to 138
  cursor_set 99
  forward_to 168
  tail 0, MK4_NATIVE_REPTILE_FIRST_OWNER_BITE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.fatality_1 (owner play-out).
reptile_first_owner_bite:
  forward 0
  tail 0, MK4_NATIVE_REPTILE_FIRST_OWNER_LOOP
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.fatality_1 (owner loop).
reptile_first_owner_loop:
  jump 0, word(reptile_first_owner_bite)

; Catalog: reptile.fatality_1 (victim bite).
victim_entry:
  prefixed_to WITH_CLIP + 85, clip(431)  ; bind clip prefix; inline sample/count 85
  tail 0, MK4_NATIVE_REPTILE_FIRST_VICTIM_BITE

; Catalog: reptile.fatality_1 (victim struggle).
reptile_first_victim_bite:
  forward_to 124
  tail 0, MK4_NATIVE_REPTILE_FIRST_VICTIM_SHAKE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.fatality_1 (victim cycles).
reptile_first_victim_shake:
  cursor_set 85
  forward_to 124
  cursor_set 85
  forward_to 124
  cursor_set 85
  tail 0, MK4_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (owner vanish).
shinnok_fatality_1_owner_vanish:
  prefixed_to WITH_CLIP + 32, clip(12)  ; bind clip prefix; inline sample/count 32
  tail 0, MK4_NATIVE_SHINNOK_FIRST_VANISHED

; Catalog: shinnok.fatality_1 (owner hidden).
shinnok_first_vanished:
  forward 0
  tail 0, MK4_NATIVE_SHINNOK_FIRST_HIDDEN
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (owner return).
shinnok_fatality_return:
  prefixed_to WITH_CLIP + 18, clip(436)  ; bind clip prefix; inline sample/count 18
  tail 0, MK4_NATIVE_SHINNOK_FATALITY_REAPPEAR

; Catalog: shinnok.fatality_1 (owner landing).
shinnok_fatality_reappear:
  forward 0
  tail 0, MK4_NATIVE_SHINNOK_FATALITY_VICTORY
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (victory).
victory_loop:
  prefixed_finish WITH_CLIP + 0, clip(443)  ; bind clip prefix; inline sample/count 0
  jump 0, word(victory_loop)

; Catalog: shinnok.fatality_1 (hand grab).
hand_entry:
  prefixed_to WITH_CLIP + 164, clip(434)  ; bind clip prefix; inline sample/count 164
  cursor_set 159
  tail 0, MK4_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (hand squeeze).
shinnok_fatality_1_hand_squeeze:
  forward_to 164
  cursor_set 159
  tail 0, MK4_NATIVE_SHINNOK_HAND_SQUEEZE_CYCLE

; Catalog: shinnok.fatality_1 (hand release).
shinnok_fatality_1_hand_release:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (victim lift).
victim_lift:
  prefixed_to WITH_CLIP + 64, clip(435)  ; bind clip prefix; inline sample/count 64
  tail 0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_HELD

; Catalog: shinnok.fatality_1 (victim hold).
shinnok_first_victim_held:
  cursor_set 59
  tail 0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (victim squeeze).
shinnok_fatality_1_victim_squeeze:
  forward_to 64
  cursor_set 59
  tail 0, MK4_NATIVE_SHINNOK_VICTIM_SQUEEZE_CYCLE

; Catalog: shinnok.fatality_1 (victim crush).
shinnok_fatality_1_victim_crush:
  forward_to 138
  tail 0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_1 (victim end).
shinnok_first_victim_crush:
  forward 0
  tail 0, MK4_NATIVE_TASK_RETIRE
  .zero 4  ; non-executable guard between independent streams


; Catalog: shinnok.fatality_2 (owner vanish).
shinnok_second_vanish:
  prefixed_to WITH_CLIP + 32, clip(12)  ; bind clip prefix; inline sample/count 32
  tail 0, MK4_NATIVE_SHINNOK_SECOND_VANISHED

; Catalog: shinnok.fatality_2 (owner hidden).
shinnok_second_vanished:
  forward 0
  tail 0, MK4_NATIVE_SHINNOK_SECOND_HANDS
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.fatality_2 (scene entry).
raiden_second_fatality:
  native_release 75
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.fatality_2 (first playback).
raiden_fatality_2_first_playback:
  call 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES
  playback_pair 94
  forward 0
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT

; Catalog: raiden.fatality_2 (repeated playback).
raiden_fatality_2_repeated_playback:
  native_release 76
  playback_pair 94
  forward 0
  playback_pair 94
  forward 0
  playback_pair 80
  scene_active 0
  blend 0, clip(570)
  native_return 187
  clip 0, clip(570)
  tail 0, MK4_NATIVE_RAIDEN_FATALITY_2_HELD_POSE

; Catalog: raiden.fatality_2 (held pose).
raiden_fatality_2_held_pose:
  bind_cursor 0, clip(571)
  wait 211
  native_release 77
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE

; Catalog: raiden.fatality_2 (ending).
raiden_fatality_2_ending:
  spawn 0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
  clip 0, clip(572)
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.fatality_2 (owner hold).
raiden_fatality_2_owner_hold:
  clip 0, clip(573)
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.fatality_2 (victim).
raiden_fatality_2_victim:
  clip 0, clip(574)
  bind_cursor 0, clip(575)
  tail 0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END

; Catalog: raiden.fatality_2 (victim hold).
raiden_fatality_2_victim_hold:
  wait 211
  clip 0, clip(576)
  park 0

; Catalog: tanya.fatality_2 (owner opening).
tanya_fatality_2:
  native_release 78
  wait 30
  wait 30
  scene_active 0
  sound_indexed 18
  native_return 186
  clip 0, clip(564)
  tail 0, MK4_NATIVE_TANYA_SECOND_HEIGHT

; Catalog: tanya.fatality_2 (owner cycles).
tanya_second_height:
  spawn 0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0, clip(565)
  scene_event 69
  wait 10
  spawn 0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0, clip(565)
  scene_event 70
  wait 10
  spawn 0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0, clip(565)
  clip 0, clip(566)
  tail 0, MK4_NATIVE_TANYA_SECOND_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_2 (victim scene).
tanya_fatality_2_victim_scene:
  clip 0, clip(567)
  clip 0, clip(568)
  wait 10
  clip 0, clip(568)
  wait 10
  clip 0, clip(568)
  spawn 0, 0x26b, MK4_NATIVE_TANYA_SECOND_VICTIM_CUE
  tail 0, MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fatality_2 (victim collapse).
tanya_fatality_2_victim_collapse:
  native_release 71
  clip 0, clip(569)
  park 0

; Catalog: cage.fatality_2 (scene entry).
cage_fatality_2_scene_entry:
  native_release 68
  wait 30
  tail 0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE

; Catalog: cage.fatality_2 (uppercut).
cage_fatality_2_uppercut:
  wait 5
  scene_active 0
  sound_c 2
  wait 6
  spawn 0, 0x26b, MK4_NATIVE_CAGE_SECOND_CHARGE
  wait 6
  play_clip 1, clip(293)
  half_count 4
  spawn_effect 6
  sound_context 2
  spawn 0, 0x26b, MK4_NATIVE_CAGE_SECOND_FLASH
  half_count 1
  sound_c 93
  sound_b 1
  native_return 185
  tail 0, MK4_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS

; Catalog: cage.fatality_2 (victory).
cage_second_repeated_uppercuts:
  prefixed_to WITH_CLIP + 60, clip(111)  ; bind clip prefix; inline sample/count 60
  native_release 80
  forward 0
  cursor_set 42
  forward 0
  jump 0, word(raiden_victory_clip_111_cursor_set_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.fatality_2 (victim hit).
cage_second_victim_hit:
  clip 0, clip(130)
  blend 0, clip(206)
  play_clip 20, clip(206)
  sound_special 0
  forward_to 90
  sound_special 0
  tail 0, MK4_NATIVE_CAGE_SECOND_VICTIM_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.fatality_2 (scene entry); kitana.fatality_2 (scene entry).
sonya_second_fatality:
  native_release 79
  wait 40
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_OPEN

; Catalog: sonya.fatality_2 (opening); kitana.fatality_2 (opening).
sonya_fatality_2_opening:
  prefixed_finish WITH_CLIP + 0, clip(558)  ; bind clip prefix; inline sample/count 0
  native_return 184
  scene_active 0
  clip 0, clip(559)
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_REPEAT

; Catalog: sonya.fatality_2 (cycle); kitana.fatality_2 (cycle).
sonya_fatality_2_cycle:
  spawn 0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
  play_clip 24, clip(560)
  wait 6
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_BRANCH

; Catalog: sonya.fatality_2 (first cycle); kitana.fatality_2 (first cycle).
sonya_fatality_2_first_cycle:
  playback_tail 0
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_PAUSE
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.fatality_2 (second cycle); kitana.fatality_2 (second cycle).
sonya_fatality_2_second_cycle:
  playback_tail 0
  wait 15
  clip 0, clip(560)
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END

; Catalog: sonya.fatality_2 (owner ending); kitana.fatality_2 (owner ending).
sonya_fatality_2_owner_ending:
  forward 0
  prefixed_finish WITH_CLIP + 0, clip(114)  ; bind clip prefix; inline sample/count 0

sonya_fatality_2_owner_ending_playback_pair_1:
  playback_pair 69
  forward_to 99
  jump 0, word(sonya_fatality_2_owner_ending_playback_pair_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.fatality_2 (victim); kitana.fatality_2 (victim).
sonya_fatality_2_victim:
  clip 0, clip(561)
  tail 0, MK4_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.fatality_2 (victim end); kitana.fatality_2 (victim end).
sonya_fatality_2_victim_end:
  forward 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.fatality_2 (owner opening).
jax_fatality_2:
  native_release 67
  bind_cursor 0, clip(284)
  wait 20
  tail 0, MK4_NATIVE_JAX_FATALITY_2_APPROACH

; Catalog: jax.fatality_2 (clap).
jax_fatality_2_approach:
  spawn 0, 0x26b, MK4_NATIVE_JAX_FATALITY_2_SPARKS
  native_release 67
  sound_c 70
  wait 15
  scene_active 0
  sound_c 70
  wait 15
  sound_c 2
  prefixed_finish WITH_CLIP + 0, clip(555)  ; bind clip prefix; inline sample/count 0
  spawn 0, 0x26b, MK4_NATIVE_JAX_FATALITY_2_FLASH
  spawn_effect 6
  sound_c 105
  sound_c 44
  native_return 183
  wait 15
  clip 0, clip(556)
  native_release 80
  prefixed_finish WITH_CLIP + 0, clip(114)  ; bind clip prefix; inline sample/count 0

jax_fatality_2_approach_playback_pair_1:
  playback_pair 69
  forward_to 99
  jump 0, word(jax_fatality_2_approach_playback_pair_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.fatality_2 (head victim 183).
jax_fatality_2_head:
  sound_b 8
  clip 0, clip(557)
  spawn 0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
  clip 0, clip(206)
  tail 0, MK4_NATIVE_JAX_FATALITY_2_FINISH

; Catalog: tanya.fatality_2 (victim dizzy); jax.fatality_2 (victim finish); cage.fatality_2 (victim start).
cage_fatality_2_victim_start:
  forward 0
  tail 0, MK4_NATIVE_JAX_FATALITY_2_VICTIM_PAUSE
  .zero 4  ; non-executable guard
