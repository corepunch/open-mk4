; Generated shinnok bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES 0x41db70
.export_equ MK4_NATIVE_SHINNOK_SECOND_GORE_8_PIECES 0x41dba0
.export_equ MK4_NATIVE_SHINNOK_SECOND_GORE_7_PIECES 0x41dbd0
.export_equ MK4_NATIVE_SHINNOK_LK_THROW_ENTRY 0x42bad0
.export_equ MK4_NATIVE_SHINNOK_FATALITY_2 0x43c6d0
.export_equ MK4_NATIVE_SHINNOK_SECOND_VANISH 0x43c820
.export_equ MK4_NATIVE_SHINNOK_SECOND_CLAP 0x43ca30
.export_equ MK4_NATIVE_SHINNOK_FATALITY_1 0x455000
.export_equ MK4_NATIVE_SHINNOK_FIRST_VANISHED 0x455110
.export_equ MK4_NATIVE_SHINNOK_FIRST_HIDDEN 0x455140
.export_equ MK4_NATIVE_SHINNOK_FATALITY_REAPPEAR 0x4551b0
.export_equ MK4_NATIVE_SHINNOK_FATALITY_VICTORY 0x455230
.export_equ MK4_NATIVE_SHINNOK_SECOND_VANISHED 0x455330
.export_equ MK4_NATIVE_SHINNOK_SECOND_HANDS 0x455360
.export_equ MK4_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE 0x4558b0
.export_equ MK4_NATIVE_SHINNOK_HAND_SQUEEZE_CYCLE 0x4558d0
.export_equ MK4_NATIVE_SHINNOK_FIRST_VICTIM_HELD 0x455ae0
.export_equ MK4_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE 0x455b60
.export_equ MK4_NATIVE_SHINNOK_VICTIM_SQUEEZE_CYCLE 0x455b80
.export_equ MK4_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH 0x455ba0
.export_equ MK4_NATIVE_SHINNOK_CAN_MORPH 0x45e6a0
.export_equ MK4_NATIVE_SHINNOK_SECOND_VICTIM 0x481510
.export_equ MK4_NATIVE_SHINNOK_WEAPON_REACTION_ENTRY 0x482350
.export_equ MK4_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP 0x483090
.export_equ MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH 0x4830a0
.export_equ MK4_NATIVE_SHINNOK_WEAPON_HP_ENTRY 0x495bb0
.export_equ MK4_NATIVE_SHINNOK_WEAPON_LP_ENTRY 0x495be0
.export_equ MK4_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY 0x495c20

; Retail source 0x004de088; branches use linked names.
shinnok_lk_throw_startup:
  sound_indexed 0x37
  native_release 0x15
  open_peer_clip 0x0, clip(146)
  open_clip 0x0, clip(31)
  playback_single 0x3b
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x1999
  playback_single 0x7f
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x23d7
  playback_single 0x9d
  native_step 0x86
  tail 0x0, MK4_NATIVE_LK_JUMP_BACK
  .zero 4 ; non-executable guard

; Retail source 0x004e0088; branches use linked names.
shinnok_fatality_2:
  scene_event 0x30
  wait 0xf
  scene_event 0x31
  wait 0x14
  scene_event 0x32
  wait 0x14
  native_release 0x33
  wait 0x14
  native_return 0xb3
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_VANISH
  .zero 4 ; non-executable guard

; Retail source 0x004e00b8; branches use linked names.
shinnok_fatality_2_first_hand:
  prefixed_to 0x278, clip(541)
  wait 0x78
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_7_PIECES
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e00e0; branches use linked names.
shinnok_fatality_2_second_hand:
  prefixed_to 0x278, clip(541)
  spawn_effect 0x6
  sound_c 0x69
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_CLAP

; Retail source 0x004e00f8; branches use linked names.
shinnok_second_clap:
  wait 0x78
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_8_PIECES
  forward 0x0
  park 0x0
  .zero 4 ; non-executable guard

; Retail source 0x004e2ef0; branches use linked names.
shinnok_fatality_1_owner_vanish:
  prefixed_to 0x220, clip(12)
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VANISHED

; Retail source 0x004e2f00; branches use linked names.
shinnok_first_vanished:
  forward 0x0
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_HIDDEN
  .zero 4 ; non-executable guard

; Retail source 0x004e2f10; branches use linked names.
shinnok_fatality_return:
  prefixed_to 0x212, clip(436)
  tail 0x0, MK4_NATIVE_SHINNOK_FATALITY_REAPPEAR

; Retail source 0x004e2f20; branches use linked names.
shinnok_fatality_reappear:
  forward 0x0
  tail 0x0, MK4_NATIVE_SHINNOK_FATALITY_VICTORY
  .zero 4 ; non-executable guard

; Retail source 0x004e2f30; branches use linked names.
victory_loop:
  prefixed_finish 0x200, clip(443)
  jump 0x0, word(victory_loop)

; Retail source 0x004e2f40; branches use linked names.
hand_entry:
  prefixed_to 0x2a4, clip(434)
  cursor_set 0x9f
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE
  .zero 4 ; non-executable guard

; Retail source 0x004e2f58; branches use linked names.
shinnok_fatality_1_hand_squeeze:
  forward_to 0xa4
  cursor_set 0x9f
  tail 0x0, MK4_NATIVE_SHINNOK_HAND_SQUEEZE_CYCLE

; Retail source 0x004e2f68; branches use linked names.
shinnok_fatality_1_hand_release:
  forward 0x0
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4 ; non-executable guard

; Retail source 0x004e2f78; branches use linked names.
victim_lift:
  prefixed_to 0x240, clip(435)
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_HELD

; Retail source 0x004e2f88; branches use linked names.
shinnok_first_victim_held:
  cursor_set 0x3b
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE
  .zero 4 ; non-executable guard

; Retail source 0x004e2f98; branches use linked names.
shinnok_fatality_1_victim_squeeze:
  forward_to 0x40
  cursor_set 0x3b
  tail 0x0, MK4_NATIVE_SHINNOK_VICTIM_SQUEEZE_CYCLE

; Retail source 0x004e2fa8; branches use linked names.
shinnok_fatality_1_victim_crush:
  forward_to 0x8a
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH
  .zero 4 ; non-executable guard

; Retail source 0x004e2fb8; branches use linked names.
shinnok_first_victim_crush:
  forward 0x0
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4 ; non-executable guard

; Retail source 0x004e2fc8; branches use linked names.
shinnok_second_vanish:
  prefixed_to 0x220, clip(12)
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_VANISHED

; Retail source 0x004e2fd8; branches use linked names.
shinnok_second_vanished:
  forward 0x0
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_HANDS
  .zero 4 ; non-executable guard

; Retail source 0x004e62d8; branches use linked names.
shinnok_draw_weapon_equip:
  forward_count 0xd
  sound_c 0x12
  forward 0x0
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4 ; non-executable guard

; Retail source 0x004e8d08; branches use linked names.
shinnok_weapon_reaction_lp_victim:
  effect 0x0
  sound_context 0x6
  sound_b 0x1
  speed_negative 0x0, 0xa3d
  open_clip 0x0, clip(132)
  double_to 0xe
  landing 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_GETUP_BACK
  jump 0, word(cage_weapon_reaction_lp_victim)
  .zero 4 ; non-executable guard

; Retail source 0x004e9460; branches use linked names.
shinnok_weapon_reaction_sweep_victim:
  sound_b 0x0
  reaction_context 0x0
  motion_block 0x0, 0xfffff0a4, 0xfffff334, 0x0, 0x0
  clip 0x0, clip(195)
  sound_special 0x0
  landing 0x0
  tail 0x0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP
  .zero 4 ; non-executable guard

; Retail source 0x004e9498; branches use linked names.
shinnok_weapon_sweep_getup:
  down 0x0, word(shared_clip_entry)
  open_clip 0x0, clip(197)
  tail 0x0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH
  jump 0, word(sweep_victim_recovery)
  .zero 4 ; non-executable guard

; Retail source 0x004ea270; branches use linked names.
shinnok_second_victim:
  prefixed_finish 0x200, clip(542)
  bind_cursor 0x0, clip(543)
  wait 0x32
  sound_b 0x8
  park 0x0
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_LK_THROW_STARTUP shinnok_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_2 shinnok_fatality_2
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_2_FIRST_HAND shinnok_fatality_2_first_hand
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_2_SECOND_HAND shinnok_fatality_2_second_hand
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_SECOND_CLAP shinnok_second_clap
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_1_OWNER_VANISH shinnok_fatality_1_owner_vanish
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FIRST_VANISHED shinnok_first_vanished
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_RETURN shinnok_fatality_return
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_REAPPEAR shinnok_fatality_reappear
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_VICTORY_LOOP victory_loop
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_HAND_ENTRY hand_entry
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_1_HAND_SQUEEZE shinnok_fatality_1_hand_squeeze
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_1_HAND_RELEASE shinnok_fatality_1_hand_release
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_VICTIM_LIFT victim_lift
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FIRST_VICTIM_HELD shinnok_first_victim_held
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_1_VICTIM_SQUEEZE shinnok_fatality_1_victim_squeeze
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FATALITY_1_VICTIM_CRUSH shinnok_fatality_1_victim_crush
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_FIRST_VICTIM_CRUSH shinnok_first_victim_crush
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_SECOND_VANISH shinnok_second_vanish
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_SECOND_VANISHED shinnok_second_vanished
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_DRAW_WEAPON_EQUIP shinnok_draw_weapon_equip
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_WEAPON_REACTION_LP_VICTIM shinnok_weapon_reaction_lp_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_WEAPON_REACTION_SWEEP_VICTIM shinnok_weapon_reaction_sweep_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_WEAPON_SWEEP_GETUP shinnok_weapon_sweep_getup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SHINNOK_SECOND_VICTIM shinnok_second_victim
