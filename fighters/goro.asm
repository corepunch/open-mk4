; Generated goro bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_GORO_LK_THROW_ENTRY 0x42c750
.export_equ MK4_NATIVE_GORO_LP_THROW_FIVE_PHASE_SEQUENCE 0x42c770
.export_equ MK4_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT 0x42c7b0
.export_equ MK4_NATIVE_GORO_SPECIAL_PUNCH 0x43af90
.export_equ MK4_NATIVE_GORO_SCRIPT_PUNCH_TAIL 0x43afc0
.export_equ MK4_NATIVE_GORO_SPECIAL_KICK 0x43b170
.export_equ MK4_NATIVE_GORO_SCRIPT_KICK_TAIL 0x43b1a0
.export_equ MK4_NATIVE_GORO_LEAP_STOMP 0x43b320
.export_equ MK4_NATIVE_GORO_SCRIPT_LEAP_CONTINUE 0x43b370
.export_equ MK4_NATIVE_GORO_SCRIPT_STOMP_RETURN 0x43b590
.export_equ MK4_NATIVE_GORO_SPECIAL_UPPERCUT 0x43b640
.export_equ MK4_NATIVE_GORO_QUAKE_STOMP 0x43b7e0
.export_equ MK4_NATIVE_GORO_SCRIPT_QUAKE_LAND 0x43b8c0
.export_equ MK4_NATIVE_GORO_AIR_KICK_RECOVERY_CALLBACK 0x46cb10
.export_equ MK4_NATIVE_GORO_AIR_HP_RECOVERY_CALLBACK 0x46e7c0
.export_equ MK4_NATIVE_GORO_AIR_LP_RECOVERY_CALLBACK 0x46ef10
.export_equ MK4_NATIVE_GORO_STOMP_HOLD_SEPARATION 0x480f40
.export_equ MK4_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY 0x482420
.export_equ MK4_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY 0x482480
.export_equ MK4_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE 0x4824f0
.export_equ MK4_NATIVE_GORO_VICTIM_GETUP 0x482540
.export_equ MK4_NATIVE_GORO_QUAKE_VICTIM 0x4834d0
.export_equ MK4_NATIVE_GORO_KICK_VICTIM 0x483570
.export_equ MK4_NATIVE_GORO_KICK_VICTIM_DISARM 0x483580
.export_equ MK4_NATIVE_GORO_STOMP_VICTIM 0x4842d0
.export_equ MK4_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD 0x484330
.export_equ MK4_NATIVE_GORO_FIREBALL 0x49ca70
.export_equ MK4_NATIVE_GORO_FIREBALL_SPAWN 0x49ca90

; Retail source 0x004de6e4; branches use linked names.
goro_lp_throw_final_damage:
  native_release 0x16
  open_peer_clip 0x0, clip(152)
  open_clip 0x0, clip(75)
  playback_single 0x2a
  native_return 0x64
  native_word 0x0, 0x1eb8
  damage 0x0, 0x2666
  forward 0x0
  tail 0x0, MK4_NATIVE_LP_THROW_VICTIM_ADJUST_RETURN
  .zero 4 ; non-executable guard

; Retail source 0x004de720; branches use linked names.
goro_lk_throw_startup:
  call 0x0, MK4_NATIVE_THROW_PAIR_START
  native_release 0x52
  open_peer_clip 0x0, clip(610)
  open_clip 0x0, clip(609)
  playback_single 0x53
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x8
  damage 0x0, 0x3333
  playback_single 0x8c
  tail 0x0, MK4_NATIVE_LP_ALIGN_AND_RELEASE

; Retail source 0x004de760; branches use linked names.
goro_lk_throw_startup_call_1:
  call 0x0, MK4_NATIVE_THROW_PAIR_START
  native_release 0x51
  open_peer_clip 0x0, clip(606)
  open_clip 0x0, clip(604)
  playback_single 0x2f
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  tail 0x0, MK4_NATIVE_GORO_LP_THROW_FIVE_PHASE_SEQUENCE
  jump 0, word(lp_throw_script)
  .zero 4 ; non-executable guard

; Retail source 0x004de7e8; branches use linked names.
goro_lp_throw_repeated_damage:
  open_peer_clip 0x0, clip(607)
  open_clip 0x0, clip(605)
  playback_single 0xb
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  effect 0x15
  damage 0x0, 0x7ae
  playback_single 0x17
  sound_context 0x1
  spawn_effect 0x1
  effect 0x15
  effect 0x15
  damage 0x0, 0x7ae
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT
  .zero 4 ; non-executable guard

; Retail source 0x004de840; branches use linked names.
lp_shared_release:
  jump 0x0, word(goro_lp_throw_final_damage)
  .zero 4 ; non-executable guard

; Retail source 0x004dfb60; branches use linked names.
goro_special_punch:
  prefixed_to 0x20e, clip(599)
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_PUNCH_TAIL

; Retail source 0x004dfb70; branches use linked names.
goro_special_kick:
  prefixed_to 0x208, clip(598)
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_KICK_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004dfbb8; branches use linked names.
goro_leap_stomp:
  open_clip 0x0, clip(601)
  double_to 0x17
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_LEAP_CONTINUE
  .zero 4 ; non-executable guard

; Retail source 0x004dfbd0; branches use linked names.
goro_leap_stomp_stomp_hit:
  handler 0x0, MK4_NATIVE_GORO_STOMP_HOLD_SEPARATION
  prefixed_sample 0x2e
  native_context 0x0
  forward 0x0
  prefixed_to 0x219, clip(602)
  sound_special 0x0
  forward 0x0
  prefixed_to 0x19
  sound_special 0x0
  forward 0x0
  prefixed_to 0x8
  motion 0x0, 0xffffe148, 0xffffe148, 0x28f
  forward_to 0x19
  native_context 0x0
  call 0x0, MK4_NATIVE_DISTURB_RESTING_WEAPONS
  sound_special 0x0
  landing 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_STOMP_RETURN

; Retail source 0x004dfc38; branches use linked names.
goro_leap_stomp_miss_landing:
  sound_special 0x0
  forward 0x0
  prefixed_finish 0x200, clip(603)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004dfc50; branches use linked names.
quake_jump:
  prefixed_to 0x215, clip(602)
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_QUAKE_LAND

; Retail source 0x004dfc60; branches use linked names.
goro_quake_land:
  sound_special 0x0
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e8dd0; branches use linked names.
goro_special_punch_189_victim:
  effect 0x10
  spawn_effect 0x0
  sound_b 0x1
  sound_context 0x2
  reaction_context 0x0
  task_callback 0x0
  motion_block 0x0, 0x0, 0x1999, 0xffffdeb9, 0x20c
  play_clip 0x22, clip(319)
  native_context 0x0
  forward_count 0x8
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4 ; non-executable guard

; Retail source 0x004e8e50; branches use linked names.
goro_special_uppercut_192_victim:
  effect 0x3
  effect 0x8
  effect 0x8
  sound_b 0x1
  sound_context 0x0
  motion 0x0, 0x0, 0xffffc7af, 0x20c
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE
  .zero 4 ; non-executable guard

; Retail source 0x004e8e80; branches use linked names.
goro_uppercut_victim_slide:
  sound_a 0x0
  effect 0x0
  play_clip 0x33, clip(181)
  native_context 0x0
  sound_a 0x17
  forward 0x0
  tail 0x0, MK4_NATIVE_GORO_VICTIM_GETUP
  .zero 4 ; non-executable guard

; Retail source 0x004e95c8; branches use linked names.
goro_quake_victim:
  sound_b 0x1
  speed_negative 0x0, 0x1999
  open_clip 0x0, clip(182)
  double_to 0x26
  landing 0x0
  double_to 0x32
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ea3e0; branches use linked names.
goro_kick_victim:
  sound_context 0x0
  spawn_effect 0x6
  motion 0x0, 0xffffe148, 0xffffd47b, 0x24d
  jump 0x0, word(jarek_ball_victim_task_callback_1)

; Retail source 0x004ea400; branches use linked names.
goro_stomp_victim:
  sound_context 0xd
  sound_b 0x1
  prefixed_finish 0x200, clip(178)
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD
  .zero 4 ; non-executable guard

; Retail source 0x004ed790; branches use linked names.
goro_fireball:
  open_clip 0x0, clip(592)
  forward_to 0xe
  sound_a 0x21
  spawn 0x0, 0x53, MK4_NATIVE_GORO_FIREBALL_SPAWN
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward_to 0xe
  playback_pair 0xc
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE goro_lp_throw_final_damage
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LK_THROW_STARTUP goro_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LP_THROW_REPEATED_DAMAGE goro_lp_throw_repeated_damage
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_LP_SHARED_RELEASE lp_shared_release
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_SPECIAL_PUNCH goro_special_punch
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_SPECIAL_KICK goro_special_kick
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LEAP_STOMP goro_leap_stomp
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LEAP_STOMP_STOMP_HIT goro_leap_stomp_stomp_hit
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_LEAP_STOMP_MISS_LANDING goro_leap_stomp_miss_landing
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_QUAKE_JUMP quake_jump
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_QUAKE_LAND goro_quake_land
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_SPECIAL_PUNCH_189_VICTIM goro_special_punch_189_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_SPECIAL_UPPERCUT_192_VICTIM goro_special_uppercut_192_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_UPPERCUT_VICTIM_SLIDE goro_uppercut_victim_slide
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_QUAKE_VICTIM goro_quake_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_KICK_VICTIM goro_kick_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_STOMP_VICTIM goro_stomp_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_GORO_FIREBALL goro_fireball
