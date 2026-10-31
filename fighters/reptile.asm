; Generated reptile bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE 0x40eb80
.export_equ MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN 0x40ec40
.export_equ MK4_NATIVE_REPTILE_SCRIPT_HEAD_WIDE 0x40ed20
.export_equ MK4_NATIVE_REPTILE_SCRIPT_ACID_DROOL 0x40ee00
.export_equ MK4_NATIVE_REPTILE_SECOND_ACID 0x41e490
.export_equ MK4_NATIVE_REPTILE_LK_THROW_ENTRY 0x42bb50
.export_equ MK4_NATIVE_REPTILE_FATALITY_2 0x43bf40
.export_equ MK4_NATIVE_REPTILE_FATALITY_1 0x4539b0
.export_equ MK4_NATIVE_REPTILE_FIRST_OWNER_CYCLE 0x453ae0
.export_equ MK4_NATIVE_REPTILE_FIRST_OWNER_BITE 0x453b20
.export_equ MK4_NATIVE_REPTILE_FIRST_OWNER_LOOP 0x453ba0
.export_equ MK4_NATIVE_REPTILE_FIRST_VICTIM_BITE 0x454130
.export_equ MK4_NATIVE_REPTILE_FIRST_VICTIM_SHAKE 0x4541b0
.export_equ MK4_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE 0x4541e0
.export_equ MK4_NATIVE_REPTILE_CAN_FIRST_FATALITY 0x45e250
.export_equ MK4_NATIVE_REPTILE_CAN_DASH 0x45e950
.export_equ MK4_NATIVE_REPTILE_CAN_SPIT 0x45e9d0
.export_equ MK4_NATIVE_REPTILE_CAN_INVISIBILITY 0x45f260
.export_equ MK4_NATIVE_REPTILE_INVISIBILITY 0x4698a0
.export_equ MK4_NATIVE_REPTILE_SCRIPT_INVISIBILITY 0x4698b0
.export_equ MK4_NATIVE_REPTILE_DASH_ENTRY 0x47a690
.export_equ MK4_NATIVE_REPTILE_SCRIPT_DASH_CONTACT 0x47a6d0
.export_equ MK4_NATIVE_REPTILE_ROLL_ENTRY 0x47afe0
.export_equ MK4_NATIVE_REPTILE_SCRIPT_ROLL_TAIL 0x47b010
.export_equ MK4_NATIVE_REPTILE_SECOND_VICTIM 0x481530
.export_equ MK4_NATIVE_REPTILE_SECOND_MELT 0x481590
.export_equ MK4_NATIVE_REPTILE_WEAPON_REACTION_ENTRY 0x4821c0
.export_equ MK4_NATIVE_REPTILE_WEAPON_VICTIM_KEEP_WEAPON 0x482270
.export_equ MK4_NATIVE_REPTILE_WEAPON_STAGGER_TAIL 0x482280
.export_equ MK4_NATIVE_REPTILE_DASH_VICTIM_SCALED 0x483500
.export_equ MK4_NATIVE_REPTILE_REACTION_40 0x483550
.export_equ MK4_NATIVE_REPTILE_WEAPON_ALTERNATE_HIGH_PUNCH 0x495d20
.export_equ MK4_NATIVE_REPTILE_WEAPON_HP_ENTRY 0x495d40
.export_equ MK4_NATIVE_REPTILE_WEAPON_BACK_HP_ENTRY 0x495d70
.export_equ MK4_NATIVE_REPTILE_WEAPON_FLURRY 0x495db0
.export_equ MK4_NATIVE_REPTILE_WEAPON_LP_ENTRY 0x496100
.export_equ MK4_NATIVE_REPTILE_WEAPON_BACK_LP_ENTRY 0x496140
.export_equ MK4_NATIVE_REPTILE_ACID_SPIT 0x4988d0
.export_equ MK4_NATIVE_REPTILE_ACID_SPAWN 0x498920

; Retail source 0x004de0e8; branches use linked names.
reptile_lk_throw_startup:
  native_release 0x12
  sound_indexed 0x1b
  open_peer_clip 0x0, clip(147)
  open_clip 0x0, clip(32)
  playback_single 0x55
  disarm_peer 0x0
  spawn_effect 0x0
  sound_a 0x24
  sound_roster 0x1
  damage 0x0, 0x35c2
  playback_single 0x69
  native_release 0x13
  playback_single 0x7b
  native_step 0x85
  tail 0x0, MK4_NATIVE_LK_PAUSE_LONG
  .zero 4 ; non-executable guard

; Retail source 0x004dff08; branches use linked names.
reptile_fatality_2:
  native_release 0x35
  spawn 0x0, 0x26, MK4_NATIVE_SCENE_HOVER
  bind_cursor 0x0, clip(493)
  wait 0x96
  scene_active 0x0
  native_return 0xb4
  spawn 0x0, 0x0, MK4_NATIVE_REPTILE_SECOND_ACID
  sound_indexed 0x3d
  prefixed_finish 0x200, clip(544)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  prefixed_finish 0x200, clip(545)
  clip_operation 0x0, clip(544)
  bind_cursor 0x0, clip(493)
  park 0x0
  jump 0, word(kai_fatality_2)
  .zero 4 ; non-executable guard

; Retail source 0x004e2e60; branches use linked names.
owner_entry:
  prefixed_to 0x20a, clip(432)
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_CYCLE

; Retail source 0x004e2e70; branches use linked names.
reptile_first_owner_cycle:
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0x8a
  cursor_set 0x63
  forward_to 0xa8
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_BITE
  .zero 4 ; non-executable guard

; Retail source 0x004e2e98; branches use linked names.
reptile_first_owner_bite:
  forward 0x0
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_LOOP
  .zero 4 ; non-executable guard

; Retail source 0x004e2ea8; branches use linked names.
reptile_first_owner_loop:
  jump 0x0, word(reptile_first_owner_bite)

; Retail source 0x004e2eb0; branches use linked names.
victim_entry:
  prefixed_to 0x255, clip(431)
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_BITE

; Retail source 0x004e2ec0; branches use linked names.
reptile_first_victim_bite:
  forward_to 0x7c
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_SHAKE
  .zero 4 ; non-executable guard

; Retail source 0x004e2ed0; branches use linked names.
reptile_first_victim_shake:
  cursor_set 0x55
  forward_to 0x7c
  cursor_set 0x55
  forward_to 0x7c
  cursor_set 0x55
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE
  .zero 4 ; non-executable guard

; Retail source 0x004e5bb0; branches use linked names.
reptile_invisibility:
  play_clip 0x18, clip(12)
  sound_c 0x4b
  sound_c 0x4a
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_INVISIBILITY
  .zero 4 ; non-executable guard

; Retail source 0x004e8128; branches use linked names.
reptile_dash:
  prefixed_finish 0x200, clip(302)
  bind_cursor 0x0, clip(303)
  speed 0x0, 0x2e14
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_DASH_CONTACT
  .zero 4 ; non-executable guard

; Retail source 0x004e8178; branches use linked names.
reptile_roll:
  sound_indexed 0x23
  sound_c 0x46
  speed 0x0, 0x1eb8
  prefixed_to 0x20d, clip(106)
  query_d 0x15
  forward_to 0x11
  landing 0x0
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_ROLL_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e8c98; branches use linked names.
reptile_weapon_reaction_reaction_137:
  effect 0x8
  effect 0x9
  sound_context 0x6
  speed_negative 0x0, 0xccc
  open_clip 0x0, clip(311)
  tail 0x0, MK4_NATIVE_REPTILE_WEAPON_STAGGER_TAIL
  .zero 4 ; non-executable guard

; Retail source 0x004e8cc0; branches use linked names.
reptile_weapon_reaction_reaction_144:
  effect 0x0
  sound_b 0x1
  sound_context 0x6
  motion 0x0, 0xfffff70b, 0xffffc000, 0x28f
  task_callback 0x0
  effect 0x0
  open_clip 0x0, clip(189)
  forward_to 0x9
  half_finish 0x0
  native_context 0x0
  forward_count 0x10
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4 ; non-executable guard

; Retail source 0x004e95f0; branches use linked names.
reptile_dash_victim:
  sound_b 0x0
  motion 0x0, 0xa3d, 0xffffcf5d, 0x28f
  task_callback 0x0
  play_clip 0x27, clip(179)
  native_context 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP

; Retail source 0x004e9620; branches use linked names.
reptile_reaction_40:
  effect 0x11
  spawn_effect 0x0
  sound_context 0x1
  motion 0x0, 0xffffeb86, 0xffffe667, 0x24d
  task_callback 0x0
  clip 0x0, clip(183)
  native_context 0x0
  forward 0x0
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  jump 0, word(jarek_ball_victim)
  .zero 4 ; non-executable guard

; Retail source 0x004ea290; branches use linked names.
reptile_fatality_2_victim_melt:
  bind_cursor 0x0, clip(546)
  speed_negative 0x0, 0x28f
  tail 0x0, MK4_NATIVE_REPTILE_SECOND_MELT
  jump 0, word(fujin_fatality_2_victim_fall)
  .zero 4 ; non-executable guard

; Retail source 0x004ec640; branches use linked names.
reptile_weapon_back_hp_flurry_startup:
  open_clip 0x0, clip(248)
  forward_to 0x8
  tail 0x0, MK4_NATIVE_REPTILE_WEAPON_FLURRY
  .zero 4 ; non-executable guard

; Retail source 0x004ed1e8; branches use linked names.
reptile_acid_spit:
  play_clip 0xb, clip(25)
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
  forward_count 0x5
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_WIDE
  forward_count 0x5
  spawn 0x0, 0x4c, MK4_NATIVE_REPTILE_ACID_SPAWN
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_ACID_DROOL
  forward_count 0x8
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
  action_followup 0x0
  forward_to 0x1e
  playback_pair 0x1b
  forward_to 0x1e
  playback_pair 0x1b
  forward_to 0x1e
  playback_pair 0x1b
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE
  forward 0x0
  ready 0x0
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_LK_THROW_STARTUP reptile_lk_throw_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FATALITY_2 reptile_fatality_2
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_OWNER_ENTRY owner_entry
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FIRST_OWNER_CYCLE reptile_first_owner_cycle
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FIRST_OWNER_BITE reptile_first_owner_bite
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FIRST_OWNER_LOOP reptile_first_owner_loop
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_VICTIM_ENTRY victim_entry
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FIRST_VICTIM_BITE reptile_first_victim_bite
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FIRST_VICTIM_SHAKE reptile_first_victim_shake
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_INVISIBILITY reptile_invisibility
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_DASH reptile_dash
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_ROLL reptile_roll
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_137 reptile_weapon_reaction_reaction_137
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_144 reptile_weapon_reaction_reaction_144
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_DASH_VICTIM reptile_dash_victim
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_REACTION_40 reptile_reaction_40
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_FATALITY_2_VICTIM_MELT reptile_fatality_2_victim_melt
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_WEAPON_BACK_HP_FLURRY_STARTUP reptile_weapon_back_hp_flurry_startup
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_REPTILE_ACID_SPIT reptile_acid_spit
