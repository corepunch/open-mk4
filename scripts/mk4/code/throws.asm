; Grounded low-kick and low-punch throws, including roster selectors.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: jarek.lk_throw (native-selected script); jax.lk_throw (native-selected script).
jarek_lk_throw_startup:
  native_release 32
  sound_indexed 7
  open_peer_clip 0, clip(164)
  open_clip 0, clip(30)
  playback_single 22
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x1999
  playback_single 61
  native_step 153
  tail 0, MK4_NATIVE_JAREK_LK_THROW_OWNER_RETURN

; Catalog: tanya.lk_throw (native-selected script).
tanya_lk_throw_startup:
  native_release 24
  sound_indexed 29
  open_peer_clip 0, clip(162)
  open_clip 0, clip(42)
  playback_single 30
  tail 0, MK4_NATIVE_LK_BRANCH_CONTINUE
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.lk_throw (native continuation).
lk_branch_continue:
  playback_single 47
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x1999
  playback_single 127
  native_step 135
  tail 0, MK4_NATIVE_LK_RECOIL

; Catalog: shinnok.lk_throw (native-selected script).
shinnok_lk_throw_startup:
  sound_indexed 55
  native_release 21
  open_peer_clip 0, clip(146)
  open_clip 0, clip(31)
  playback_single 59
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x1999
  playback_single 127
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x23d7
  playback_single 157
  native_step 134
  tail 0, MK4_NATIVE_LK_JUMP_BACK
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.lk_throw (native-selected script).
reptile_lk_throw_startup:
  native_release 18
  sound_indexed 27
  open_peer_clip 0, clip(147)
  open_clip 0, clip(32)
  playback_single 85
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x35c2
  playback_single 105
  native_release 19
  playback_single 123
  native_step 133
  tail 0, MK4_NATIVE_LK_PAUSE_LONG
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.lk_throw (native-selected script).
reiko_lk_throw_startup:
  open_peer_clip 0, clip(148)
  open_clip 0, clip(40)
  playback_single 38
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 1
  damage 0, 0x35c2
  tail 0, MK4_NATIVE_LK_THROW_ROCK_REPEAT_ONE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reiko.lk_throw (native continuation).
lk_throw_rock_repeat_one:
  playback_single 43
  playback_pair_reverse 42
  tail 0, MK4_NATIVE_LK_THROW_ROCK_REPEAT_TWO

; Catalog: reiko.lk_throw (native continuation).
lk_throw_rock_repeat_two:
  playback_single 43
  playback_pair_reverse 42
  tail 0, MK4_NATIVE_LK_THROW_ROCK_RELEASE

; Catalog: reiko.lk_throw (native continuation).
lk_throw_rock_release:
  playback_single 43
  playback_pair_reverse 42
  playback_single 43
  playback_pair_reverse 42
  playback_single 59
  native_step 132
  forward 0
  ready 0

; Catalog: quan_chi.lk_throw (native-selected script).
quan_chi_lk_throw_startup:
  native_release 16
  open_peer_clip 0, clip(160)
  open_clip 0, clip(38)
  playback_single 40
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 8
  damage 0, 0x35c2
  playback_single 99
  native_release 17
  native_step 131
  native_word 0, 0x8000
  motion 0, 0xfffff334, 0xffffe148, 0x28f
  tail 0, MK4_NATIVE_LK_VICTIM_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lk_throw (native-selected script).
raiden_lk_throw_startup:
  sound_indexed 25
  native_release 14
  open_peer_clip 0, clip(149)
  open_clip 0, clip(33)
  playback_single 41
  disarm_peer 0
  spawn_effect 0
  sound_c 58
  sound_roster 1
  damage 0, 0x35c2
  native_step 111
  forward 0
  ready 0

; Catalog: kai.lk_throw (native-selected script).
kai_lk_throw_startup:
  native_release 13
  open_peer_clip 0, clip(159)
  open_clip 0, clip(34)
  playback_single 72
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 8
  damage 0, 0x3333
  native_step 110
  native_word 0, 0x1999
  forward 0
  tail 0, MK4_NATIVE_LK_PAUSE_SHORT

; Catalog: fujin.lk_throw (native-selected script).
fujin_lk_throw_startup:
  native_release 7
  open_peer_clip 0, clip(155)
  open_clip 0, clip(36)
  playback_single 49
  disarm_peer 0
  spawn_effect 0
  sound_a 36
  sound_roster 8
  damage 0, 0x2e14
  playback_single 46
  native_release 8
  playback_single 86
  clear_handler 0
  native_step 91
  forward_to 89
  wait 12
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.lk_throw (native-selected script).
scorpion_lk_throw_startup:
  native_release 4
  open_peer_clip 0, clip(157)
  open_clip 0, clip(37)
  playback_single 18
  tail 0, MK4_NATIVE_CAGE_LK_THROW_PAIRED_CONTINUATION

; Catalog: scorpion.lk_throw (native continuation).
cage_lk_throw_damage:
  playback_single 42
  tail 0, MK4_NATIVE_CAGE_LK_THROW_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.lk_throw (native continuation).
cage_lk_throw_release:
  damage 0, 0x2e14
  disarm_peer 0
  sound_a 36
  sound_roster 8
  spawn_effect 0
  playback_single 65
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  native_step 77
  tail 0, MK4_NATIVE_MMOVE_LK_OWNER_READY
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.lk_throw (native-selected script); kitana.lk_throw (native-selected script).
sonya_lk_throw_startup:
  sound_indexed 19
  open_peer_clip 0, clip(163)
  open_clip 0, clip(43)
  playback_single 6
  playback_single 24
  native_release 3
  playback_single 44
  damage 0, 0x2e14
  spawn_effect 0
  disarm_peer 0
  sound_a 36
  sound_roster 8
  playback_single 75
  native_step 78
  tail 0, MK4_NATIVE_MMOVE_LK_BLEND_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.lk_throw (native-selected script).
liu_kang_lk_throw_startup:
  open_peer_clip 0, clip(133)
  open_clip 0, clip(35)
  play_pair 10
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.lk_throw (native-selected script).
cage_lk_throw_startup:
  native_release 39
  sound_indexed 56
  open_peer_clip 0, clip(535)
  open_clip 0, clip(534)
  playback_single 23
  damage 0, 0x1999
  native_step 174
  forward_count 3
  wait 16
  forward 0
  tail 0, MK4_NATIVE_CAGE_LK_THROW_OWNER_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.lk_throw (native continuation).
liu_kang_lk_continue:
  native_release 1
  play_pair 20
  forward_to 38
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.lk_throw (native continuation).
liu_kang_lk_throw_damage_release:
  disarm_peer 0
  sound_a 36
  sound_roster 8
  play_pair 2
  native_return 80
  forward_to 43
  playback_pair 39
  forward_to 43
  playback_pair 39
  forward_to 43
  playback_pair 39
  forward_to 43
  playback_pair 39
  playback_pair 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.lk_throw (native continuation).
liu_kang_lk_throw_retreat_entry:
  speed_negative 0, 0x11eb
  tail 0, MK4_NATIVE_LIU_KANG_LK_THROW_RETREAT

; Catalog: liu_kang.lk_throw (native continuation).
liu_kang_lk_throw_retreat:
  open_clip 0, clip(182)
  double_to 28
  landing 0
  native_sound 0
  ready 0

; Catalog: sub_zero.lk_throw (native-selected script); noob_saibot.lk_throw (native-selected script).
sub_zero_lk_throw_startup:
  open_peer_clip 0, clip(165)
  open_clip 0, clip(41)
  forward_count 10
  native_release 2
  sound_a 3
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.lk_throw (native continuation); noob_saibot.lk_throw (native continuation).
sub_zero_lk_rebase:
  play_pair 29
  disarm_peer 0
  sound_a 36
  play_pair 5
  sound_roster 8
  play_pair 8
  handler 0, MK4_NATIVE_VELOCITY_FRICTION
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE

; Catalog: sub_zero.lk_throw (native continuation); noob_saibot.lk_throw (native continuation).
sub_zero_lk_damage:
  play_pair 5
  tail 0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.lp_throw (first slam).
jax_lp_throw_first_slam:
  open_peer_clip 0, clip(145)
  open_clip 0, clip(83)
  playback_single 55
  spawn_effect 0
  call 0, MK4_NATIVE_THROW_GRUNT
  tail 0, MK4_NATIVE_JAX_LP_SLAM_FIRST

; Catalog: jax.lp_throw (second slam).
slam_first_script:
  playback_single 82
  damage 0, 0xccc
  spawn_effect 0
  call 0, MK4_NATIVE_THROW_GRUNT
  tail 0, MK4_NATIVE_JAX_LP_SLAM_SECOND

; Catalog: jax.lp_throw (third slam).
slam_second_script:
  playback_single 116
  spawn_effect 0
  damage 0, 0xccc
  call 0, MK4_NATIVE_THROW_GRUNT
  tail 0, MK4_NATIVE_JAX_LP_SLAM_THIRD

; Catalog: jax.lp_throw (release 125).
slam_second_release_script:
  native_return 125
  jump 0, word(slam_first_release_script_native_word_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.lp_throw (release 100).
slam_first_release_script:
  native_return 100

slam_first_release_script_native_word_1:
  native_word 0, 0xf5c
  damage 0, 0xccc
  tail 0, MK4_NATIVE_JAX_LP_THROW_RETURN
  .zero 4  ; non-executable guard between independent streams


shared_tail_entry:
  tail 0, MK4_NATIVE_LP_SELECT_SHORT

; Catalog: fujin.lp_throw (extended throw); kai.lp_throw (extended throw).
lp_select_short:
  native_release 11
  open_peer_clip 0, clip(158)
  open_clip 0, clip(81)
  playback_single 46
  native_return 109
  damage 0, 0x2e14
  forward 0
  tail 0, MK4_NATIVE_LP_THROW_OWNER_READY

; Catalog: raiden.lp_throw (extended throw); reiko.lp_throw (extended throw); jarek.lp_throw (extended throw) (also shared by other catalog entries).
lp_select_long:
  native_release 12
  open_peer_clip 0, clip(154)
  open_clip 0, clip(80)
  playback_single 57
  native_return 107
  tail 0, MK4_NATIVE_LP_END
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lp_throw (extended owner recovery); reiko.lp_throw (throw owner continuation); jarek.lp_throw (owner return) (also shared by other catalog entries).
lp_end:
  damage 0, 0x2e14
  forward 0
  ready 0

lp_end_native_release_1:
  native_release 36
  open_peer_clip 0, clip(151)
  open_clip 0, clip(79)
  playback_single 45
  native_return 104
  native_word 0, 0xccc
  damage 0, 0x2666
  forward 0
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lp_throw (short throw); fujin.lp_throw (short throw); reiko.lp_throw (short throw) (also shared by other catalog entries).
lp_select:
  native_release 22
  open_peer_clip 0, clip(153)
  open_clip 0, clip(76)
  playback_single 25
  native_return 102
  native_word 0, 0x1eb8
  jump 0, word(shared_damage_entry)

; Catalog: goro.lp_throw (final damage).
goro_lp_throw_final_damage:
  native_release 22
  open_peer_clip 0, clip(152)
  open_clip 0, clip(75)
  playback_single 42
  native_return 100
  native_word 0, 0x1eb8
  damage 0, 0x2666
  forward 0
  tail 0, MK4_NATIVE_LP_THROW_VICTIM_ADJUST_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.lk_throw (native-selected script).
goro_lk_throw_startup:
  call 0, MK4_NATIVE_THROW_PAIR_START
  native_release 82
  open_peer_clip 0, clip(610)
  open_clip 0, clip(609)
  playback_single 83
  spawn_effect 0
  sound_a 36
  sound_roster 8
  damage 0, 0x3333
  playback_single 140
  tail 0, MK4_NATIVE_LP_ALIGN_AND_RELEASE

goro_lk_throw_startup_call_1:
  call 0, MK4_NATIVE_THROW_PAIR_START
  native_release 81
  open_peer_clip 0, clip(606)
  open_clip 0, clip(604)
  playback_single 47
  sound_context 1
  spawn_effect 1
  effect 21
  effect 21
  tail 0, MK4_NATIVE_GORO_LP_THROW_FIVE_PHASE_SEQUENCE

; Catalog: scorpion.lp_throw (roster-selected attacker/victim); raiden.lp_throw (roster selector); sonya.lp_throw (roster selector) (also shared by other catalog entries).
lp_throw_script:
  sound_b 7

lp_throw_script_select_roster_1:
  select_roster 0, \
    word(goro_lp_throw_final_damage), \  ; scorpion
    word(shared_tail_entry_2), \  ; raiden
    word(shared_tail_entry_2_native_release_1), \  ; sonya
    word(lp_end_native_release_1), \  ; liu_kang
    word(lp_select), \  ; sub_zero
    word(shared_tail_entry), \  ; fujin
    word(goro_lp_throw_final_damage), \  ; shinnok
    word(shared_tail_entry_2), \  ; reiko
    word(shared_tail_entry_2_native_release_1), \  ; quan_chi
    word(lp_end_native_release_1), \  ; tanya
    word(lp_select), \  ; reptile
    word(shared_tail_entry), \  ; kai
    word(shared_tail_entry_2), \  ; jarek
    word(lp_throw_script_select_roster_1_operand_16), \  ; jax
    word(goro_lp_throw_final_damage), \  ; cage
    word(goro_lk_throw_startup_call_1)  ; goro
  tail 0, MK4_NATIVE_JAX_LP_THROW_THROW_CONTINUATION

; Catalog: goro.lp_throw (repeated damage).
goro_lp_throw_repeated_damage:
  open_peer_clip 0, clip(607)
  open_clip 0, clip(605)
  playback_single 11
  sound_context 1
  spawn_effect 1
  effect 21
  effect 21
  effect 21
  damage 0, 0x7ae
  playback_single 23
  sound_context 1
  spawn_effect 1
  effect 21
  effect 21
  damage 0, 0x7ae
  tail 0, MK4_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.lk_throw (native continuation).
lp_shared_release:
  jump 0, word(goro_lp_throw_final_damage)
  .zero 4  ; non-executable guard between independent streams


shared_tail_entry_2:
  tail 0, MK4_NATIVE_LP_SELECT_LONG

shared_tail_entry_2_native_release_1:
  native_release 22
  open_peer_clip 0, clip(156)
  open_clip 0, clip(78)
  playback_single 21
  native_return 103
  damage 0, 0x2666
  forward_count 2
  wait 10
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


shared_damage_entry:
  damage 0, 0x2666
  forward 0
  tail 0, MK4_NATIVE_SUB_ZERO_LP_THROW_OWNER_RETURN
  .zero 4  ; non-executable guard
