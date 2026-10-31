; Generated tanya bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.equ MK4_NATIVE_TANYA_LK_THROW_ENTRY 0x42b7e0
.export MK4_NATIVE_TANYA_LK_THROW_ENTRY MK4_NATIVE_TANYA_LK_THROW_ENTRY
.equ MK4_NATIVE_TANYA_FATALITY_1 0x44d340
.export MK4_NATIVE_TANYA_FATALITY_1 MK4_NATIVE_TANYA_FATALITY_1
.equ MK4_NATIVE_TANYA_FIRST_OWNER_53 0x44d4a0
.export MK4_NATIVE_TANYA_FIRST_OWNER_53 MK4_NATIVE_TANYA_FIRST_OWNER_53
.equ MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT 0x44d4e0
.export MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT
.equ MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS 0x44d510
.export MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS
.equ MK4_NATIVE_TANYA_FIRST_OWNER_LOOP 0x44d530
.export MK4_NATIVE_TANYA_FIRST_OWNER_LOOP MK4_NATIVE_TANYA_FIRST_OWNER_LOOP
.equ MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL 0x44d6e0
.export MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL
.equ MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT 0x44d750
.export MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT
.equ MK4_NATIVE_TANYA_FIRST_VICTIM_BURST 0x44d7a0
.export MK4_NATIVE_TANYA_FIRST_VICTIM_BURST MK4_NATIVE_TANYA_FIRST_VICTIM_BURST
.equ MK4_NATIVE_TANYA_CAN_FIRST_FATALITY 0x45e2a0
.export MK4_NATIVE_TANYA_CAN_FIRST_FATALITY MK4_NATIVE_TANYA_CAN_FIRST_FATALITY
.equ MK4_NATIVE_TANYA_CAN_SPIN_KICK 0x45e8d0
.export MK4_NATIVE_TANYA_CAN_SPIN_KICK MK4_NATIVE_TANYA_CAN_SPIN_KICK
.equ MK4_NATIVE_TANYA_FATALITY_2 0x464f30
.export MK4_NATIVE_TANYA_FATALITY_2 MK4_NATIVE_TANYA_FATALITY_2
.equ MK4_NATIVE_TANYA_SECOND_HEIGHT 0x4650b0
.export MK4_NATIVE_TANYA_SECOND_HEIGHT MK4_NATIVE_TANYA_SECOND_HEIGHT
.equ MK4_NATIVE_TANYA_SECOND_FINISH 0x465130
.export MK4_NATIVE_TANYA_SECOND_FINISH MK4_NATIVE_TANYA_SECOND_FINISH
.equ MK4_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186 0x465200
.export MK4_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186 MK4_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186
.equ MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE 0x465250
.export MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE
.equ MK4_NATIVE_TANYA_SECOND_OWNER_CUE 0x465270
.export MK4_NATIVE_TANYA_SECOND_OWNER_CUE MK4_NATIVE_TANYA_SECOND_OWNER_CUE
.equ MK4_NATIVE_TANYA_SECOND_VICTIM_CUE 0x465390
.export MK4_NATIVE_TANYA_SECOND_VICTIM_CUE MK4_NATIVE_TANYA_SECOND_VICTIM_CUE
.equ MK4_NATIVE_TANYA_SPIN_KICK 0x479520
.export MK4_NATIVE_TANYA_SPIN_KICK MK4_NATIVE_TANYA_SPIN_KICK
.equ MK4_NATIVE_TANYA_DRILL_KICK 0x47b9f0
.export MK4_NATIVE_TANYA_DRILL_KICK MK4_NATIVE_TANYA_DRILL_KICK
.equ MK4_NATIVE_TANYA_DRILL_KICK_CONTACT 0x47ba20
.export MK4_NATIVE_TANYA_DRILL_KICK_CONTACT MK4_NATIVE_TANYA_DRILL_KICK_CONTACT
.equ MK4_NATIVE_TANYA_WEAPON_REACTION_REACTION_170_ENTRY 0x4821a0
.export MK4_NATIVE_TANYA_WEAPON_REACTION_REACTION_170_ENTRY MK4_NATIVE_TANYA_WEAPON_REACTION_REACTION_170_ENTRY
.equ MK4_NATIVE_TANYA_WEAPON_REACTION_ENTRY 0x486710
.export MK4_NATIVE_TANYA_WEAPON_REACTION_ENTRY MK4_NATIVE_TANYA_WEAPON_REACTION_ENTRY
.equ MK4_NATIVE_TANYA_WEAPON_HP_ENTRY 0x4955b0
.export MK4_NATIVE_TANYA_WEAPON_HP_ENTRY MK4_NATIVE_TANYA_WEAPON_HP_ENTRY
.equ MK4_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY 0x4955e0
.export MK4_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY MK4_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY
.equ MK4_NATIVE_TANYA_WEAPON_LP_ENTRY 0x495610
.export MK4_NATIVE_TANYA_WEAPON_LP_ENTRY MK4_NATIVE_TANYA_WEAPON_LP_ENTRY
.equ MK4_NATIVE_TANYA_BOOMERANG_RELEASE 0x497b60
.export MK4_NATIVE_TANYA_BOOMERANG_RELEASE MK4_NATIVE_TANYA_BOOMERANG_RELEASE

; Retail source 0x004de038; branches use linked names.
tanya_lk_throw_startup:
  native_release 0x18
  sound_indexed 0x1d
  open_peer_clip 0x0, clip(162)
  open_clip 0x0, clip(42)
  playback_single 0x1e
  tail 0x0, MK4_NATIVE_LK_BRANCH_CONTINUE
  .zero 4 ; non-executable guard

; Retail source 0x004de060; branches use linked names.
lk_branch_continue:
  playback_single 0x2f
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x1999
  playback_single 0x7f
  native_step 0x87
  tail 0x0, MK4_NATIVE_LK_RECOIL
  jump 0, word(shinnok_lk_throw_startup)
  .zero 4 ; non-executable guard

; Retail source 0x004e1598; branches use linked names.
owner_kiss_start:
  prefixed_to 0x234, clip(395)
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_53

; Retail source 0x004e15a8; branches use linked names.
tanya_first_owner_53:
  forward_to 0x68
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT
  .zero 4 ; non-executable guard

; Retail source 0x004e15b8; branches use linked names.
tanya_first_owner_drift:
  forward 0x0
  prefixed_finish 0x200, clip(396)
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS
  .zero 4 ; non-executable guard

; Retail source 0x004e15d0; branches use linked names.
tanya_first_owner_loop:
  prefixed_finish 0x0
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_LOOP
  .zero 4 ; non-executable guard

; Retail source 0x004e15e0; branches use linked names.
victim_scene:
  prefixed_finish 0x200, clip(397)
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL

; Retail source 0x004e15f0; branches use linked names.
tanya_fatality_1_victim_pulse:
  playback_pair 0xbc
  forward 0x0
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT

; Retail source 0x004e1600; branches use linked names.
tanya_fatality_1_victim_shake:
  prefixed_to 0x209, clip(409)
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0xb
  playback_pair 0x9
  forward_to 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x17
  playback_pair 0x15
  forward_to 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x28
  playback_pair 0x26
  forward_to 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x39
  playback_pair 0x37
  forward_to 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x42
  playback_pair 0x40
  forward_to 0x48
  forward_to 0x4a
  playback_pair 0x48
  forward_to 0x4a
  playback_pair 0x48
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_BURST
  .zero 4 ; non-executable guard

; Retail source 0x004e16d0; branches use linked names.
tanya_first_victim_burst:
  forward 0x0
  playback_pair 0x48
  jump 0x0, word(tanya_first_victim_burst)
  .zero 4 ; non-executable guard

; Retail source 0x004e5798; branches use linked names.
tanya_fatality_2:
  native_release 0x4e
  wait 0x1e
  wait 0x1e
  scene_active 0x0
  sound_indexed 0x12
  native_return 0xba
  clip 0x0, clip(564)
  tail 0x0, MK4_NATIVE_TANYA_SECOND_HEIGHT

; Retail source 0x004e57c0; branches use linked names.
tanya_second_height:
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0x0, clip(565)
  scene_event 0x45
  wait 0xa
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0x0, clip(565)
  scene_event 0x46
  wait 0xa
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
  clip 0x0, clip(565)
  clip 0x0, clip(566)
  tail 0x0, MK4_NATIVE_TANYA_SECOND_FINISH
  .zero 4 ; non-executable guard

; Retail source 0x004e5820; branches use linked names.
tanya_fatality_2_victim_scene:
  clip 0x0, clip(567)
  clip 0x0, clip(568)
  wait 0xa
  clip 0x0, clip(568)
  wait 0xa
  clip 0x0, clip(568)
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_VICTIM_CUE
  tail 0x0, MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE
  .zero 4 ; non-executable guard

; Retail source 0x004e5860; branches use linked names.
tanya_fatality_2_victim_collapse:
  native_release 0x47
  clip 0x0, clip(569)
  park 0x0
  jump 0, word(cage_fatality_2_scene_entry)
  .zero 4 ; non-executable guard

; Retail source 0x004e81f0; branches use linked names.
tanya_drill_kick:
  sound_indexed 0x1f
  speed 0x0, 0x23d7
  prefixed_to 0x20c, clip(103)
  tail 0x0, MK4_NATIVE_TANYA_DRILL_KICK_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004e8bb0; branches use linked names.
tanya_weapon_reaction_reaction_170_victim:
  effect 0x10
  sound_context 0x1
  jump 0x0, word(kai_draw_weapon_back_victim_spawn_effect_1)
  .zero 4 ; non-executable guard

; Retail source 0x004e9db8; branches use linked names.
tanya_weapon_reaction_reaction_169_victim:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
  effect 0x4
  spawn_effect 0x0
  sound_context 0x1
  sound_b 0x1
  speed_negative 0x0, 0x4000
  clip 0x0, clip(130)
  ready 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004ed190; branches use linked names.
boomerang_start:
  sound_c 0x21
  play_clip 0x7, clip(229)
  tail 0x0, MK4_NATIVE_TANYA_BOOMERANG_RELEASE
  .zero 4 ; non-executable guard

; Retail source 0x004ed268; branches use linked names.
tanya_fireball_caster:
  prefixed_finish 0x200, clip(26)
  sound_a 0x21
  block_sound 0x0
  spawn 0x0, 0x4b, MK4_NATIVE_FIREBALL_SPAWN
  playback_pair 0xb
  forward_to 0xd
  action_followup 0x0
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0xb
  forward_to 0xd
  playback_pair 0x0
  ready 0x0
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_LK_THROW_STARTUP tanya_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_LK_BRANCH_CONTINUE lk_branch_continue
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_OWNER_KISS_START owner_kiss_start
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FIRST_OWNER_53 tanya_first_owner_53
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FIRST_OWNER_DRIFT tanya_first_owner_drift
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FIRST_OWNER_LOOP tanya_first_owner_loop
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_VICTIM_SCENE victim_scene
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FATALITY_1_VICTIM_PULSE tanya_fatality_1_victim_pulse
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FATALITY_1_VICTIM_SHAKE tanya_fatality_1_victim_shake
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FIRST_VICTIM_BURST tanya_first_victim_burst
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FATALITY_2 tanya_fatality_2
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_SECOND_HEIGHT tanya_second_height
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FATALITY_2_VICTIM_SCENE tanya_fatality_2_victim_scene
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FATALITY_2_VICTIM_COLLAPSE tanya_fatality_2_victim_collapse
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_DRILL_KICK tanya_drill_kick
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_WEAPON_REACTION_REACTION_170_VICTIM tanya_weapon_reaction_reaction_170_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_WEAPON_REACTION_REACTION_169_VICTIM tanya_weapon_reaction_reaction_169_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_BOOMERANG_START boomerang_start
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_TANYA_FIREBALL_CASTER tanya_fireball_caster
