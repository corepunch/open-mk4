; Reaction and playback parameter records, including the sound-B selector table.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


sound_b_table:
  .word 0x21c140a, 0x14820302, 0x2a81487, 0x2b202ad, 0x1db02b7, 0x1469145a
  .word 0x1473146e, 0x1491148c, 0x14a0149c, 0x14a500dc, 0x142314aa, 0xe60276
  .word 0x1b802f8, 0x13ab0299, 0x14af00d2, 0x2760267, 0x280027b, 0xe600e1
  .word 0x14c314be, 0x14cd14c8, 0x14d714d2, 0x14e114dc, 0x157c14e6, 0x15861581
  .word 0xbf4158b, 0x280027b, 0x1e0009b, 0x1ea01e5, 0xaa00a5, 0x7d00b9
  .word 0x8c0082, 0xe600be, 0x152202f8, 0xdc00d7, 0xe600e1, 0xcd00b4
  .word 0x154f154a, 0x15591554, 0x19821496, 0x198c1987, 0x19961991, 0x19a0199b
  .word 0x19aa19a5, 0x19b419af, 0x19be19b9, 0x1b580136, 0x1a541590, 0x1a5e1a59
  .word 0x1a681a63, 0x1a721a6d, 0x1a861a77, 0x1a901a8b, 0x1a9a1a95, 0x1aa91a9f
  .word 0x1ab31aae, 0x82007d, 0x8c0087, 0x18600f5, 0x190018b, 0x24e
  .word 0x475570
  .zero 4
  .word 0x450d0bc, 0x450d0d8, 0x450d0f4, 0x450d110, 0x450d12c, 0x450d148
  .word 0x550d164, 0x550d180, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c
  .word 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c
  .word 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x8050d19c, 0x405dd0
  .word 0x8c
  .zero 20
  .word 0x18000, 0x10000, 0x18000, 0x28000, 0x18000, 0x1b333
  .word 0x1b333, 0x3e666, 0x1b333, 0x2b333, 0x44ccc, 0x20000
  .word 0x10000, 0x1b333, 0x20000
  .zero 4
  .word 0x2, 0x18ce0000, 0xff
  .zero 4
  .word 0x1850f800, 0xc50f800, 0x250f81c, 0x250f838, 0x250f854, 0x250f870
  .word 0x250f88c, 0x250f8a8, 0x250f8c4, 0x8050f8c4, 0x405dd0
  .zero 4
  .word 0xc50f800, 0x250f81c, 0x250f838, 0x250f854, 0x250f870, 0x250f88c
  .word 0x250f8a8, 0x250f8c4, 0x8050f8c4, 0x405dd0
  .zero 8
  .word 0x250f838, 0x250f854, 0x250f870, 0x250f88c, 0x250f8a8, 0x250f8c4
  .word 0x8050f8c4, 0x405dd0
  .zero 8
  .word 0x1508f24, 0x2508f40, 0x1508f5c, 0x2508f78, 0x1508f94, 0x2508fb0
  .word 0x1508fcc, 0x2508fe8, 0x1509004, 0x2509020, 0x150903c, 0x2509058
  .word 0x1509074, 0x2509090, 0x15090ac, 0x15090c8, 0x25090e4, 0x1509100
  .word 0x150911c, 0x2509138, 0x1509154, 0x1509170, 0x80509170, 0x405dd0
  .zero 8
  .word 0x350d634, 0x350d650, 0x350d66c, 0x350d688, 0x350d6a4, 0x350d6c0
  .word 0x350d6dc, 0x350d6f8, 0x350d714, 0x350d730, 0x350d74c, 0x350d768
  .word 0x350d784, 0x350d7a0, 0x350d7bc, 0x350d7d8, 0x8050d7d8, 0x405dd0
  .word 0x2506268, 0x2506284, 0x25062a0, 0x25062bc, 0x25062d8, 0x25062f4
  .word 0x2506310, 0x250632c, 0x2506348, 0x81506348, 0x405dd0
  .zero 4
  .word 0x250cff8, 0x350d014, 0x350d030, 0x350d04c, 0x450d068, 0x450d084
  .word 0x550d0a0, 0x8150d0a0, 0x405dd0
  .zero 4

arm_tear_clip_204_rosters:
  .word 0x4
  .zero 4
  .word 0xc, 0x8, 0x7, 0xd, 0xe, 0xffffffff
  .word 0x9, 0x2, 0xffffffff
  .zero 4
  .word 0x8, 0xffffffff, 0x9, 0x2, 0xa, 0xd
  .word 0x6, 0xffffffff, 0xd, 0xffffffff, 0x9, 0x2
  .word 0xffffffff
  .zero 4
  .word 0x7, 0x9999, 0x9, 0x8000, 0x8, 0x9999
  .word 0xa, 0x8000, 0xd, 0xcccc, 0xf, 0xd999
  .word 0xe, 0xcccc, 0x10, 0xd999, 0x3, 0x11999
  .word 0x2, 0x11999, 0x7, 0x8ccc, 0x8, 0x8ccc
  .word 0xf, 0x9999, 0x10, 0x9999, 0xd, 0xb333
  .word 0xe, 0xb333
  .zero 8
  .word 0x7, 0x10000, 0x9, 0xd1eb, 0x8, 0x10000
  .word 0xa, 0xd1eb, 0xd, 0x12e14, 0xf, 0x14a3d
  .word 0xe, 0x12e14, 0x10, 0x14a3d, 0x3, 0xf0a3
  .word 0x2, 0xf0a3
  .zero 8
  .word 0x8150f934, 0x4735a0, 0x8150f950, 0x4735a0, 0x8150f96c, 0x4735a0
  .word 0x8150f988, 0x4735a0, 0x8150f9a4, 0x4735a0, 0x8150f9c0, 0x4735a0
  .word 0x8150f9dc, 0x4735a0, 0x8150f9f8, 0x4735a0, 0x8150fa14, 0x4735a0
  .word 0x8150fa30, 0x4735a0, 0x8150fa4c, 0x4735a0, 0x8150fa68, 0x4735a0
  .word 0x8150f8e0, 0x4735e0, 0x8150f8fc, 0x4735e0, 0x8150f918, 0x4735e0
  .word 0x8150f934, 0x4735e0, 0x8150f950, 0x4735e0, 0x8150f96c, 0x4735e0
  .word 0x8150f988, 0x4735e0, 0x8150f9a4, 0x4735e0, 0x8150f9c0, 0x4735e0
  .word 0x8150f9dc, 0x4735e0, 0x8150f9f8, 0x4735e0, 0x8150fa14, 0x4735e0
  .word 0x8150fa30, 0x4735e0, 0x8150fa4c, 0x4735e0, 0x8150fa68, 0x4735e0
  .word 0x8050fa68, 0x473810, 0x8150f8e0, 0x4735e0, 0x8150f8fc, 0x4735e0
  .word 0x8150f918, 0x4735e0, 0x8150f934, 0x4735e0, 0x8150f950, 0x4735e0
  .word 0x8150f96c, 0x4735e0, 0x8150f988, 0x4735e0, 0x8150f9a4, 0x4735e0
  .word 0x8150f9c0, 0x4735e0, 0x8150f9dc, 0x4735e0, 0x8150f9f8, 0x4735e0
  .word 0x8150fa14, 0x4735e0, 0x8150fa30, 0x4735e0, 0x8150fa4c, 0x4735e0
  .word 0x8150fa68, 0x4735e0, 0x8050fa68, 0x473810, 0x22, 0x23
  .word 0x24, 0x25, 0x26, 0x27, 0x28, 0x29
  .word 0x2a, 0x2b, 0x2c, 0x234, 0x235, 0x236
  .word 0x237, 0x23d, 0x232, 0x233
  .zero 8
  .word 0x450d0bc, 0x450d0d8, 0x450d0f4, 0x450d110, 0x7f50d110, 0x7f50d110
  .word 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110
  .word 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110, 0x7f50d110
  .word 0x8050d110, 0x405dd0, 0x150cf50, 0x150cf6c, 0x150cf88, 0x150cfa4
  .word 0x7f50cfc0, 0x7f50cfc0, 0x7f50cfc0, 0x7f50cfc0, 0x7f50cfc0, 0x7f50cfc0
  .word 0x8050cfc0, 0x405dd0, word(unresolved_body_assets_030)
  .zero 4
  .word 0x27, 0xff9b8000, 0xff9c0000, 0x10000, word(unresolved_body_assets_030), 0x1
  .word 0x28, 0x648000, 0xff9c0000, 0x10000
  .zero 8
  .word 0xffa60000, 0xff9f0000, 0x276
  .zero 4
  .word 0x1e0000, 0xff9f0000, 0x277
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_001:
  .word word(unresolved_fighter_assets_008), word(unresolved_fighter_assets_074), word(unresolved_retained_frontend_005), word(unresolved_retained_frontend_006), 0xfe68fd0c, 0xfe8cfd0c
  .word 0x2bc0000, 0x288, 0x1a1
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_002:
  .word word(unresolved_fighter_assets_041), word(unresolved_fighter_assets_026), word(unresolved_retained_frontend_009), word(unresolved_retained_frontend_012), 0xfe6802f4, 0xfe8c02f4
  .word 0x2bc0000, 0x288, 0x1a1, 0x1

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_003:
  .word word(unresolved_fighter_assets_013), word(unresolved_fighter_assets_042), word(unresolved_retained_frontend_015), word(unresolved_retained_frontend_016), 0xfe94fd0c, 0xfea0fd0c
  .word 0xb80000, 0xb8, 0x51e
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_004:
  .word word(unresolved_fighter_assets_024), word(unresolved_fighter_assets_084), word(unresolved_retained_frontend_019), word(unresolved_retained_frontend_022), 0xfe9402f4, 0xfea002f4
  .word 0xb80000, 0xb8, 0x51e, 0x1, word(unresolved_reaction_parameters_005), 0x234
  .word word(unresolved_reaction_parameters_006), 0x235, word(unresolved_reaction_parameters_007), 0x236, word(unresolved_reaction_parameters_008), 0x237
  .word word(unresolved_retained_frontend_004), 0xfe68fd0c, word(unresolved_retained_frontend_005), 0xfe68fd0c, word(unresolved_retained_frontend_006), 0xfe8cfd0c
  .word word(unresolved_retained_frontend_007), 0xfe8cfd0c, word(unresolved_retained_frontend_009), 0xfe6802f4, word(unresolved_retained_frontend_010), 0xfe6802f4
  .word word(unresolved_retained_frontend_011), 0xfe8c02f4, word(unresolved_retained_frontend_012), 0xfe8c02f4, word(unresolved_retained_frontend_014), 0xfe94fd0c
  .word word(unresolved_retained_frontend_015), 0xfe94fd0c, word(unresolved_retained_frontend_016), 0xfea0fd0c, word(unresolved_retained_frontend_017), 0xfea0fd0c
  .word word(unresolved_retained_frontend_019), 0xfe9402f4, word(unresolved_retained_frontend_020), 0xfe9402f4, word(unresolved_retained_frontend_021), 0xfea002f4
  .word word(unresolved_retained_frontend_022), 0xfea002f4

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_005:
  .word 0x38088
  .zero 20
  .word word(unresolved_retained_frontend_003), 0x1f8001

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_006:
  .word 0x38088
  .zero 20
  .word word(unresolved_retained_frontend_008), 0x1f8001

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_007:
  .word 0x38088
  .zero 20
  .word word(unresolved_retained_frontend_013), 0x2808001

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_008:
  .word 0x38088
  .zero 20
  .word word(unresolved_retained_frontend_018), 0x2808001, 0x47bf50, 0x4797c0, 0x47bf50
  .zero 36
  .word 0x47bf50
  .zero 40
  .word 0x47bf50, 0x47bf50
  .zero 76
  .word 0x47aab0
  .zero 40
  .word 0x47af90
  .zero 192
  .word 0xf, 0x10, MK4_NATIVE_CONTACT_QUERY_D, 0x47be50, 0x47bed0, 0x47bd60
  .word 0x47bf50
  .zero 44
  .word 0x3c, 0xf, MK4_NATIVE_CONTACT_QUERY_D, 0x47bfb0, MK4_NATIVE_FUJIN_DIVE_LAND, 0x47bfb0
  .zero 112
  .word 0x47c4d0
  .zero 68
  .word 0x6666
  .zero 28
  .word 0xa, MK4_NATIVE_CONTACT_QUERY_B, 0xa, 0xb, 0xb, 0xffffffff
  .word 0x47cc70
  .zero 44
  .word 0xb, MK4_NATIVE_CONTACT_QUERY_B, 0xa, 0xb, 0xc, 0xffffffff
  .word 0x47cce0
  .zero 36

input_table_handstand:
  .zero 4
  .word MK4_NATIVE_HANDSTAND_SPIN, MK4_NATIVE_HANDSTAND_EXIT, MK4_NATIVE_HANDSTAND_FRONT, MK4_NATIVE_HANDSTAND_REAR
  .zero 4
  .word 0x47d550, MK4_NATIVE_GORO_STOMP_HOLD_SEPARATION, clip(88), 0x47d1a0, clip(90), 0x47d320
  .word 0xffffe667, 0xffffcccd, 0x28f
  .zero 4
  .word clip(91), MK4_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN
  .zero 104
  .word 0xfffff334, 0xffffdc29, 0x168
  .zero 28
  .word MK4_NATIVE_LANDING_MOTION
  .zero 148
  .word 0x1, 0x3333, 0x7
  .zero 132
  .word 0x58000000, 0xfffffae2, 0xffffd1ec, 0x1ca, 0x32000000, 0x47e2b0
  .zero 72
  .word 0x3c, 0x6, MK4_NATIVE_CONTACT_QUERY_D, 0x47eb50, 0x47ed20, 0x47eb50
  .word 0x480ee0
  .zero 284
  .word 0xfffffae2, 0xffffe667, 0x147
  .zero 4
  .word 0xffffe8f6, 0xffffd70b, 0x28f
  .zero 52
  .word 0x3c, 0x4, MK4_NATIVE_CONTACT_QUERY_D, 0x47fd90, 0x47fe40, 0x47fd90
  .word 0xfffffd71, 0xffffe667, 0x1ca
  .zero 4
  .word 0xfffffd71, 0xffffdeb9, 0x1ca
  .zero 84
  .word 0x1999
  .zero 52
  .word 0x9, MK4_NATIVE_CONTACT_QUERY_B, 0x1e, 0x1f, 0x20, 0x21
  .word 0x22, 0xffffffff, 0x478e40
  .zero 44

input_table_teleport_air:
  .word MK4_NATIVE_PRESS_STRAIGHT_AIR_HIGH_PUNCH, MK4_NATIVE_PRESS_STRAIGHT_AIR_LOW_PUNCH, MK4_NATIVE_TELEPORT_SLAM, MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK, MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK
  .zero 12

action_table_4:
  .word MK4_NATIVE_SCORPION_TELEPORT, MK4_NATIVE_SUB_ZERO_CLONE, MK4_NATIVE_SUB_ZERO_SLIDE, MK4_NATIVE_LIU_KANG_FLYING_KICK, MK4_NATIVE_LIU_KANG_BICYCLE_KICK, MK4_NATIVE_SONYA_BICYCLE
  .word MK4_NATIVE_SONYA_SQUARE_WAVE, MK4_NATIVE_SONYA_LEG_GRAB, MK4_NATIVE_FUJIN_WIND_GRAB, MK4_NATIVE_FUJIN_TRIPLE_KICK, MK4_NATIVE_RAIDEN_TORPEDO, MK4_NATIVE_QUAN_CHI_TELEPORT_STOMP
  .word MK4_NATIVE_RAIDEN_TELEPORT, MK4_NATIVE_KAI_HANDSTAND_ENTRY, MK4_NATIVE_TELEPORT, 0x47c050, MK4_NATIVE_FUJIN_DIVE, MK4_NATIVE_KAI_AERIAL_PUNCH_ENTRY
  .word MK4_NATIVE_TANYA_DRILL_KICK, MK4_NATIVE_SONYA_CARTWHEEL, MK4_NATIVE_JAREK_QUAKE, MK4_NATIVE_JAREK_CANNONBALL, MK4_NATIVE_REPTILE_ROLL_ENTRY, MK4_NATIVE_JAX_DASH_PUNCH
  .word MK4_NATIVE_REPTILE_DASH_ENTRY, 0x480770, MK4_NATIVE_QUAN_CHI_FLIP_KICK, MK4_NATIVE_CAGE_CPU_KICK, MK4_NATIVE_CAGE_POWERED_PUNCH, MK4_NATIVE_JAREK_UPWARD_BALL
  .word MK4_NATIVE_TANYA_SPIN_KICK, MK4_NATIVE_REIKO_FLIP_KICK, MK4_NATIVE_REIKO_ORBIT, MK4_NATIVE_KAI_LUNGE_KICK_ENTRY, 0x481010
  .zero 1344

reaction_clip_180_friction_callback:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 800
  .word 0x482b60
  .zero 172
  .word 0x48000200, clip(180), 0x48000200, clip(180), 0x3f000000
  .zero 4
  .word 0x62000004, 0x2a000001, 0x2b000003, 0x26000001, 0x58000000, 0xffffee15
  .word 0xffffdc29, 0x28f, 0x22000000, 0x1000000, clip(183), 0x60000000
  .word 0x3000000, 0x34000002, 0x32000000, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 80

crouch_reaction_physics_word:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 1172

stagger_entry:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 456

lk_victim_slide_speed:
  .word 0xf5c
  .zero 240
  .word 0x5a000000, 0x3333, 0x38000000, MK4_NATIVE_VELOCITY_FRICTION, 0x4a000000, 0x32000000
  .word 0x485940

bicycle_release:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 232

animation_rows_lp_victim_table_release:
  .word clip(173)

; Retained referenced block; semantic role is not established.
unresolved_reaction_parameters_009:
  .word MK4_NATIVE_LP_TABLE_VICTIM_STOP_AND_FLIP_GETUP

animation_rows_lp_victim_rebased_release:
  .word clip(173), MK4_NATIVE_LP_TABLE_VICTIM_STOP_AND_FLIP_GETUP

animation_rows_lp_victim_weapon_scatter_release:
  .word clip(173), MK4_NATIVE_LP_TABLE_VICTIM_STOP_AND_FLIP_GETUP, 0x26000005, 0x2b000003, 0x4e000000, 0x22000000
  .word 0x59000000, 0xfffff5c3, 0xa3d, 0xffffeb86, 0xde, 0x38000000
  .word MK4_NATIVE_LANDING_MOTION, 0x1000000, clip(142), 0x1000000, clip(141), 0x200000c
  .word clip(143), 0x39000000, 0x3000000, 0x34000014, 0x32000000, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 292

record_reaction:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 48

reaction_clip_127_friction_row:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 132
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 40
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 40

block_reaction_friction_row:
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 204
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 4
  .word 0x5a000000, 0x4ccc, 0x32000000, 0x487570
  .zero 312
  .word 0x5b000000, 0x2a000000, 0x3000000, 0x34000030, 0x32000000, 0x488700
  .word 0x5b000000, 0x2a000000, 0x3000000, 0x34000010, 0x3f000000
  .zero 268
  .word MK4_NATIVE_VELOCITY_FRICTION
  .zero 68

getup_descriptor_side_getup:
  .word 0xffffee15, clip(185), 0xfffff5c3, clip(184)

getup_descriptor_flip_getup:
  .zero 4
  .word clip(328), 0xfffff5c3, clip(184)

getup_descriptor_roll_getup:
  .word 0xffffeb86, clip(191), 0xfffff5c3, clip(190)

getup_descriptor_front_getup:
  .word 0xfffff5c3, clip(188), 0xfffff5c3, clip(187)
  .zero 144
  .word 0x33000000, word(fujin_fatality_2_victim_fall)
  .zero 136
  .word 0x48eb40
  .zero 4
  .word 0x22000000, 0x3400000a, 0x35000000, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VOICE, 0x60000000, 0x32000000
  .word 0x4838d0
  .zero 4
  .word 0x34000014, 0x2500003f, 0x3e000000
  .zero 140

skin_sever_rows:
  .word 0x2

skin_sever_first_rig_word:
  .word word(flayed_body_rig_joint_2), 0x481aa0, 0xa, word(flayed_body_rig_joint_10), 0x481b80, 0x9
  .word word(flayed_body_rig_joint_9), 0x481b80, 0x8, word(flayed_body_rig_joint_8), 0x481b80, 0x7
  .word word(flayed_body_rig_joint_7), 0x481b80, 0x6, word(flayed_body_rig_joint_6), 0x481b80, 0x5
  .word word(flayed_body_rig_joint_5), 0x481b80, 0x3, word(flayed_body_rig_joint_3), 0x481b80, 0xc
  .word word(flayed_body_rig_joint_12), 0x481b80, 0xb, word(flayed_body_rig_joint_11), 0x481b80, 0xe
  .word word(flayed_body_rig_joint_14), 0x481b80, 0xd, word(flayed_body_rig_joint_13), 0x481b80, 0x10
  .word word(flayed_body_rig_joint_16), 0x481b80, 0xf, word(flayed_body_rig_joint_15), 0x481b80
  .zero 24
  .word 0x32000000, 0x484d40
