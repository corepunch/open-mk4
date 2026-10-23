; Command predicates, input dispatch and shared native motion/contact parameters.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


command_next:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_001:
  .word MK4_NATIVE_SUB_ZERO_CAN_SLIDE, MK4_NATIVE_WEAPON_CAN_PICKUP, MK4_NATIVE_WEAPON_CAN_DRAW, MK4_NATIVE_CAN_PLAYER_PAIR_AIR_THROW, MK4_NATIVE_SONYA_CAN_LEG_GRAB, MK4_NATIVE_JAX_AIR_THROW_COMMAND_GATE
  .word MK4_NATIVE_SCORPION_CAN_TELEPORT, MK4_NATIVE_SUB_ZERO_CAN_CLONE, MK4_NATIVE_FUJIN_CAN_WIND, MK4_NATIVE_FUJIN_CAN_WIND_SLAM, 0x45eba0, MK4_NATIVE_SUB_ZERO_CAN_ICE
  .word MK4_NATIVE_KAI_CAN_HANDSTAND, MK4_NATIVE_FUJIN_CAN_DIVE, MK4_NATIVE_KAI_CAN_GROUND_SKULL, 0x45ea50, MK4_NATIVE_LIU_KANG_CAN_BICYCLE, MK4_NATIVE_HAS_GRAVITY
  .word MK4_NATIVE_KAI_CAN_RISING_SKULL, MK4_NATIVE_FUJIN_CAN_WIND_GRAB, MK4_NATIVE_SCORPION_CAN_SPEAR, MK4_NATIVE_RAIDEN_CAN_TELEPORT, MK4_NATIVE_REPTILE_CAN_INVISIBILITY, MK4_NATIVE_REPTILE_CAN_SPIT
  .word MK4_NATIVE_SONYA_CAN_CARTWHEEL, MK4_NATIVE_CAN_FAN_FINISH, MK4_NATIVE_CAN_FINISH_CLOSE, MK4_NATIVE_SUB_ZERO_CAN_FIRST_FATALITY, MK4_NATIVE_CAN_SIDESTEP, MK4_NATIVE_REPTILE_CAN_DASH
  .word MK4_NATIVE_JAX_CAN_DASH_PUNCH, MK4_NATIVE_CAGE_CAN_GROIN_PUNCH, MK4_NATIVE_TANYA_CAN_SPIN_KICK, MK4_NATIVE_JAX_CAN_MISSILE, MK4_NATIVE_REIKO_CAN_FLIP_KICK, MK4_NATIVE_REIKO_CAN_ORBIT
  .word MK4_NATIVE_CAN_FINISH_RANGE, MK4_NATIVE_LIU_KANG_CAN_FIRST_FATALITY, MK4_NATIVE_FUJIN_CAN_FIRST_FATALITY
  .zero 4
  .word MK4_NATIVE_REIKO_CAN_FIRST_FATALITY, MK4_NATIVE_QUAN_CHI_CAN_FIRST_FATALITY, MK4_NATIVE_TANYA_CAN_FIRST_FATALITY, MK4_NATIVE_REPTILE_CAN_FIRST_FATALITY, MK4_NATIVE_KAI_CAN_FIRST_FATALITY, MK4_NATIVE_JAX_CAN_FATALITY_1
  .word MK4_NATIVE_SONYA_CAN_FIRST_FATALITY
  .zero 4
  .word MK4_NATIVE_REIKO_CAN_STAR, MK4_NATIVE_SHINNOK_CAN_MORPH, MK4_NATIVE_CAN_SPIKE_FINISH, clip(317), 0x4609f0
  .zero 4
  .word 0x460c00, 0x45fc90
  .zero 8
  .word 0x460c70
  .zero 12
  .word 0x460c40
  .zero 28
  .word 0x460c00, 0x45fc90
  .zero 8
  .word 0x460c70
  .zero 12
  .word 0x460c40
  .zero 24
  .word 0x48e070, 0x460490, 0x4604b0, 0x48e070, 0x4604d0, 0x4604f0
  .word 0x48e070, 0x48e070, 0x460510, 0x460530, 0x48e070, 0x48e070
  .word 0x48e070, 0x48e070, 0x48e070, 0x48e070

input_table_standing:
  .word MK4_NATIVE_PRESS_STANDING_HIGH_PUNCH, MK4_NATIVE_PRESS_STANDING_LOW_PUNCH, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

input_table_crouching:
  .word MK4_NATIVE_PRESS_CROUCHING_HIGH_PUNCH, MK4_NATIVE_PRESS_CROUCHING_LOW_PUNCH, MK4_NATIVE_PRESS_CROUCHING_BLOCK, MK4_NATIVE_PRESS_CROUCHING_HIGH_KICK, MK4_NATIVE_PRESS_CROUCHING_LOW_KICK
  .zero 12
  .word 0x50000, 0x1
  .zero 4
  .word 0xfffd0000, 0x8, 0x40000, 0x50000, 0x3, 0xfffc0000
  .word 0x50000, 0x4, 0xfffc0000, 0xfffc0000, 0x5, 0x40000
  .word 0xfffc0000, 0x6, 0x50000, 0xfffb0000, 0x7, 0xfffb0000
  .word 0xfffb0000, 0xffffffff
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_1_free_weapon:
  .string "KODE 1: FREE WEAPON"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_2_throwing_disabled:
  .string "KODE 2: THROWING DISABLED"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_3_armed_and_dangerous:
  .string "KODE 3: ARMED AND DANGEROUS"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_4_silent_kombat:
  .string "KODE 4: SILENT KOMBAT"

; Literal text; no additional runtime role is inferred.
text_kode_5_explosive_kombat:
  .string "KODE 5: EXPLOSIVE KOMBAT"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_6_random_weapons:
  .string "KODE 6: RANDOM WEAPONS"

; Literal text; no additional runtime role is inferred.
text_kode_7_no_power:
  .string "KODE 7: NO POWER ??"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_8_many_weapons:
  .string "KODE 8: MANY WEAPONS"

; Literal text; no additional runtime role is inferred.
text_kode_9_randper_kombat:
  .string "KODE 9: RANDPER KOMBAT"

; Literal text; no additional runtime role is inferred.
text_kode_10_no_rain:
  .string "KODE 10: NO RAIN"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_11_weapon_kombat:
  .string "KODE 11: WEAPON KOMBAT"
  .word 0x45444f4b, 0x3a323120, 0x50203420, 0x4559414c, 0x41522052, 0x4550444e
  .word 0x4c492052, 0x4147454c, 0x4c
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_13_noob_saibot_mode:
  .string "KODE 13: NOOB SAIBOT MODE"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_14_red_rain:
  .string "KODE 14: RED RAIN"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_15_maximum_damage_disabled:
  .string "KODE 15: MAXIMUM DAMAGE DISABLED"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_16_throwing_and_max_damage_disabled:
  .string "KODE 16: THROWING AND MAX DAMAGE DISABLED"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_17_kombat_zone_goro_s_lair:
  .string "KODE 17: KOMBAT ZONE: GORO'S LAIR"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_18_kombat_zone_the_well:
  .string "KODE 18: KOMBAT ZONE: THE WELL"

; Literal text; no additional runtime role is inferred.
text_kode_19_kombat_zone_elder_gods:
  .string "KODE 19: KOMBAT ZONE: ELDER GODS"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_20_kombat_zone_the_tomb:
  .string "KODE 20: KOMBAT ZONE: THE TOMB"

; Literal text; no additional runtime role is inferred.
text_kode_21_kombat_zone_wind_world:
  .string "KODE 21: KOMBAT ZONE: WIND WORLD"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_22_kombat_zone_reptile_s_lair:
  .string "KODE 22: KOMBAT ZONE: REPTILE'S LAIR"

; Literal text; no additional runtime role is inferred.
text_kode_23_kombat_zone_shaolin_temple:
  .string "KODE 23: KOMBAT ZONE: SHAOLIN TEMPLE"

; Literal text; no additional runtime role is inferred.
text_kode_24_kombat_zone_living_forest:
  .string "KODE 24: KOMBAT ZONE: LIVING FOREST"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_25_kombat_zone_the_prison:
  .string "KODE 25: KOMBAT ZONE: THE PRISON"
  .zero 4

; Literal text; no additional runtime role is inferred.
text_kode_26_unlimited_run:
  .string "KODE 26: UNLIMITED RUN"

; Literal text; no additional runtime role is inferred.
text_kode_27_kombat_zone_ice_pit:
  .string "KODE 27: KOMBAT ZONE: ICE PIT"
  .word 0x321321, 0x461c30, 0x100100, 0x4612e0, 0x10010, 0x461230
  .word 0x110110, 0x4612c0, 0x1001, 0x461220, 0x111111, 0x4612f0
  .word 0x222222, 0x461330, 0x444444, 0x461300, 0x555555, 0x461340
  .word 0x666666, 0x461310, 0x50050, 0x461320, 0x123123, 0x4612b0
  .word 0x60060, 0x4612a0, 0x2002, 0x461280, 0x12012, 0x461260
  .word 0x20020, 0x461240, 0x11011, 0x4616a0, 0x22022, 0x4616c0
  .word 0x33033, 0x4616e0, 0x44044, 0x461700, 0x55055, 0x461720
  .word 0x66066, 0x461740, 0x101101, 0x461760, 0x202202, 0x461780
  .word 0x303303, 0x4617a0, 0x313313, 0x4617c0, 0xffffffff
  .zero 4
  .word 0x4, 0x462720, 0x8, 0x4627d0, 0x400, 0x462740
  .word 0x800, 0x4627f0, 0x1, MK4_NATIVE_TASK_RETIRE, 0x2, MK4_NATIVE_TASK_RETIRE
  .word 0x100, MK4_NATIVE_TASK_RETIRE, 0x200, MK4_NATIVE_TASK_RETIRE

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_002:
  .word 0x252, 0xfffdd99a, 0xfffe0000

tower_columns:
  .word word(unresolved_dispatch_and_motion_003), 0x253, 0xffff0000, 0xfffdc000, word(unresolved_dispatch_and_motion_004), 0x254
  .zero 4
  .word 0xfffd8000, word(unresolved_dispatch_and_motion_005), 0x255, 0x10000, 0xfffd4000, word(unresolved_dispatch_and_motion_006)
  .word 0x256, 0x20000, 0xfffd0000, word(unresolved_dispatch_and_motion_007)
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_003:
  .word 0x3020400, 0x403020b, 0x90a0707, 0xc0b0a09, 0x20e0e0c, 0xf0f0f0f
  .word 0x6060606
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_004:
  .word 0x8070503, 0x90e0704, 0xd0c0b05, 0x20e0a, 0x100010d, 0x403020e
  .word 0xf0f0f0f, 0x6060606
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_005:
  .word 0xc0b0a09, 0xd0c0b0a, 0x1000e0e, 0x4030201, 0x90e0705, 0xa090807
  .word 0x5040302, 0xf0f0f0f, 0x6060606
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_006:
  .word 0x20e0d, 0x100020e, 0x5040302, 0x90e0705, 0xd0c0b0a, 0xe0d0c0b
  .word 0xa090807, 0xf0f0f0f, 0x6060606
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_007:
  .word 0x1000201, 0x2010002, 0x7050403, 0xa090e07, 0xe0d0c0b, 0x30e0d0c
  .word 0xb0a090e, 0xf0f0f0f, 0x6060606
  .zero 4

tower_labels:
  .word word(text_novice), 0xff580000, 0x890000, word(text_beginner), 0xffb10000, 0x890000
  .word word(text_warrior)
  .zero 4
  .word 0x890000, word(text_master), 0x5a0000, 0x930000, word(text_master_ii), 0xb40000
  .word 0x990000
  .zero 564
  .word 0x3e000000
  .zero 200
  .word 0x3e000000
  .zero 16
  .word 0x2000018, clip(562), 0x34000006, 0x32000000, 0x4662e0
  .zero 4
  .word 0x14000000, 0x32000000, 0x466310
  .zero 4
  .word 0x3400000f, 0x32000000, 0x4663a0
  .zero 4
  .word 0x2000018, clip(562), 0x32000000, 0x466400
  .zero 8
  .word 0x2000018, clip(563), 0x34000006, 0x32000000, 0x4665f0
  .zero 4
  .word 0x1d000000, 0x32000000, 0x466600
  .zero 4
  .word 0x3400000f, 0x32000000, 0x4666a0
  .zero 4
  .word 0x2000018, clip(563), 0x32000000, 0x466710, 0x3000000, 0x32000000
  .word 0x466780
  .zero 144
  .word 0x3e000000
  .zero 56
  .word 0x4a, 0x49, 0x48
  .zero 52
  .word 0x72, 0x4689b0, 0x72, 0x4689b0
  .zero 48
  .word 0x2000021, clip(137), 0x28000000, 0x3000000, 0x3e000000
  .zero 292

kick_contact_default:
  .zero 4
  .word MK4_NATIVE_CONTACT_QUERY_B, 0xa, 0xa, 0x9, 0xffffffff, MK4_NATIVE_FUJIN_HIGH_KICK_CONTACT_RECOVERY
  .zero 20

kick_contact_wide:
  .word 0x1, MK4_NATIVE_CONTACT_QUERY_B, 0x11, 0x11, 0x10, 0xffffffff
  .word MK4_NATIVE_JAX_HIGH_KICK_CONTACT_RECOVERY
  .zero 4
  .word 0x6000002, 0x24000000, 0x7000000, 0x3f000000, 0x1f000000, 0x2
  .word 0x20000001, 0x4700020a, clip(71), 0x67003002, 0x29000000, 0x4000002
  .word 0x24000000, 0x3000000, 0x3f000000
  .zero 308

air_throw_slam_release_speed:
  .word 0x8f5

air_throw_slam_release_y_velocity:
  .word 0xffffcf5d

air_throw_slam_release_gravity:
  .word 0x28f
  .zero 44
  .word 0x3c000000, word(unresolved_dispatch_and_motion_009), word(unresolved_dispatch_and_motion_010), word(unresolved_dispatch_and_motion_008), word(unresolved_dispatch_and_motion_011), word(unresolved_dispatch_and_motion_010)
  .word word(unresolved_dispatch_and_motion_011), word(unresolved_dispatch_and_motion_015), word(unresolved_dispatch_and_motion_012), word(unresolved_dispatch_and_motion_014), word(unresolved_dispatch_and_motion_008), word(unresolved_dispatch_and_motion_013)
  .word word(unresolved_dispatch_and_motion_009), word(unresolved_dispatch_and_motion_012), word(unresolved_dispatch_and_motion_015), word(unresolved_dispatch_and_motion_015)

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_008:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(46), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_009:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(45), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_010:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(47), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_011:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(48), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_012:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(49), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_013:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(50), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_014:
  .word word(unresolved_dispatch_and_motion_016), 0x48000200, clip(51), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_015:
  .word word(unresolved_dispatch_and_motion_016), 0x1000000, clip(44), 0x6000032, clip(44), 0x33000000

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_016:
  .word word(unresolved_dispatch_and_motion_016), 0x32000000, 0x46b480
  .zero 320
  .word 0x1f000000, 0x1a, 0x20000001, 0x2000007, clip(52), 0x5100001a
  .word 0x29000000, 0x5b000000, 0x3000000, 0x3a000005, 0x1d00000a, 0x34000002
  .word 0x1d000009, 0x34000002, 0x3b000000, 0x9000000, clip(52), 0x30000000
  .zero 260

air_straight_tail:
  .word 0x4, 0x28, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_GORO_AIR_KICK_RECOVERY_CALLBACK, MK4_NATIVE_AIR_STRAIGHT_KICK_LAND, MK4_NATIVE_AIR_STRAIGHT_KICK_MISS_REVERSE
  .zero 456

air_lk_tail:
  .word 0xa, 0x1b, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_LOW_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_LOW_KICK_CLIP_58_REVERSE
  .word 0x46e300
  .zero 28

air_lk_alt_tail:
  .word 0xa, 0x1c, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_LOW_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_LOW_KICK_CLIP_59_REVERSE
  .word 0x46e300
  .zero 28

air_lk_wide_tail:
  .word 0xa, 0x1d, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_LOW_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_LOW_KICK_CLIP_60_REVERSE
  .word 0x46e300
  .zero 148

air_diagonal_tail:
  .word clip(55)
  .zero 4
  .word MK4_NATIVE_AIR_DIAGONAL_KICK_QUERY_30
  .zero 4

air_diagonal_kick_query_30_row:
  .word 0xa, 0x1e, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_DIAGONAL_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_DIAGONAL_KICK_CLIP_55_REVERSE
  .word 0x46e300
  .zero 28

air_diagonal_alt_tail:
  .word clip(56)
  .zero 4
  .word MK4_NATIVE_AIR_DIAGONAL_KICK_QUERY_31
  .zero 4

air_diagonal_kick_query_31_row:
  .word 0xa, 0x1f, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_DIAGONAL_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_DIAGONAL_KICK_CLIP_56_REVERSE
  .word 0x46e300
  .zero 28

air_diagonal_wide_tail:
  .word clip(57)
  .zero 4
  .word MK4_NATIVE_AIR_DIAGONAL_KICK_QUERY_32
  .zero 4

air_diagonal_kick_query_32_row:
  .word 0xa, 0x20, MK4_NATIVE_CONTACT_QUERY_B, MK4_NATIVE_AIR_DIAGONAL_KICK_CONTACT_HOLD, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_DIAGONAL_KICK_CLIP_57_REVERSE
  .word 0x46e300
  .zero 92

air_hp_tail:
  .word clip(97), 0x5, MK4_NATIVE_AIR_HIGH_PUNCH_CONTACT_WINDOW
  .zero 4

air_high_punch_contact_row:
  .word 0xa, 0x4, MK4_NATIVE_CONTACT_QUERY_A, MK4_NATIVE_GORO_AIR_HP_RECOVERY_CALLBACK, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_017:
  .word MK4_NATIVE_AIR_HIGH_PUNCH_MISS_REVERSE, 0x46e300
  .zero 68

weapon_air_high_punch_parameters:
  .word clip(252), 0x6, 0x46eae0
  .zero 20

weapon_air_low_punch_parameters:
  .word clip(251), 0x7, 0x46ec40, 0x46e300
  .zero 24

air_lp_tail:
  .word clip(96), 0x9, MK4_NATIVE_AIR_CONTACT_BEGIN
  .zero 4

air_low_punch_contact_row:
  .word 0xa, 0x5, MK4_NATIVE_CONTACT_QUERY_A, MK4_NATIVE_GORO_AIR_LP_RECOVERY_CALLBACK, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH, MK4_NATIVE_AIR_LOW_PUNCH_MISS_REVERSE
  .word 0x46e300
  .zero 60
  .word 0x26, 0x46ef30
  .zero 300
  .word 0x460c00, 0x45fc90
  .zero 4
  .word 0x460710, 0x460c70
  .zero 8
  .word 0x460570, 0x460c40
  .zero 24
  .word 0xffffcf5d, 0x28f, MK4_NATIVE_LANDING_MOTION, 0x46fcd0, 0x4706f0
  .zero 4
  .word 0xe14, 0xffffcf5d, 0x24d, 0x470650, 0xfffff1ec, 0xffffd1ec
  .word 0x24d
  .zero 4
  .word 0xfffff1ec, 0xffffd1ec, 0x24d
  .zero 8
  .word 0x4681a0
  .zero 32
  .word 0x4f000028, 0x3e000000

kick_contact_narrow:
  .word 0x2, MK4_NATIVE_CONTACT_QUERY_B, 0xa, 0xb, 0xc, 0xffffffff
  .word MK4_NATIVE_HIGH_KICK_NARROW_CONTACT_RECOVERY
  .zero 636

close_high_punch:
  .word clip(262)

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_018:
  .word clip(263), clip(264), clip(262), clip(263), clip(264), clip(262)
  .word clip(263), clip(264), clip(262), clip(263), clip(264), clip(262)
  .word clip(263), clip(264), clip(263)

combo_high_punch:
  .word MK4_NATIVE_JAX_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY, MK4_NATIVE_KITANA_COMBO_HP_ENTRY, MK4_NATIVE_JAX_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY, MK4_NATIVE_KITANA_COMBO_HP_ENTRY
  .word MK4_NATIVE_JAX_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY, MK4_NATIVE_KITANA_COMBO_HP_ENTRY, MK4_NATIVE_JAX_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY, MK4_NATIVE_KITANA_COMBO_HP_ENTRY
  .word MK4_NATIVE_JAX_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY, MK4_NATIVE_KITANA_COMBO_HP_ENTRY, MK4_NATIVE_COMBO_HP_ENTRY
  .zero 536
  .word 0x8, 0x20, 0x2, 0x21
  .zero 4
  .word 0x22, 0xffffffff
  .zero 4

input_table_straight_air:
  .word MK4_NATIVE_PRESS_STRAIGHT_AIR_HIGH_PUNCH, MK4_NATIVE_PRESS_STRAIGHT_AIR_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK, MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK
  .zero 4

input_table_air_combo:
  .zero 4
  .word MK4_NATIVE_PRESS_STRAIGHT_AIR_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK, MK4_NATIVE_PRESS_STRAIGHT_AIR_KICK
  .zero 4

action_table_1:
  .word MK4_NATIVE_TASK_PARK_40_TICK_LOOP, MK4_NATIVE_UPPERCUT_ENTRY, 0x42c560, MK4_NATIVE_WEAPON_PICKUP, MK4_NATIVE_NORMAL_LOW_KICK, MK4_NATIVE_HIGH_KICK
  .word MK4_NATIVE_SWEEP, MK4_NATIVE_ROUNDHOUSE, MK4_NATIVE_NORMAL_LOW_PUNCH, MK4_NATIVE_CLOSE_HIGH_PUNCH, MK4_NATIVE_AIR_HIGH_PUNCH, MK4_NATIVE_HIGH_PUNCH
  .word MK4_NATIVE_CLOSE_HIGH_KICK, MK4_NATIVE_AIR_KICK, MK4_NATIVE_AIR_LOW_PUNCH, MK4_NATIVE_SIDESTEP, MK4_NATIVE_AIR_DIAGONAL_HIGH_KICK, MK4_NATIVE_AIR_DIAGONAL_LOW_KICK
  .word MK4_NATIVE_QUAN_CHI_WEAPON_STEAL, MK4_NATIVE_SHINNOK_MORPH_SCORPION, MK4_NATIVE_SHINNOK_MORPH_RAIDEN, MK4_NATIVE_SHINNOK_MORPH_SONYA, MK4_NATIVE_SHINNOK_MORPH_LIU_KANG, MK4_NATIVE_SHINNOK_MORPH_SUB_ZERO
  .word MK4_NATIVE_SHINNOK_MORPH_FUJIN, MK4_NATIVE_SHINNOK_MORPH_JAREK, MK4_NATIVE_SHINNOK_MORPH_QUAN_CHI, MK4_NATIVE_SHINNOK_MORPH_TANYA, MK4_NATIVE_SHINNOK_MORPH_REPTILE, MK4_NATIVE_SHINNOK_MORPH_KAI
  .word MK4_NATIVE_SHINNOK_MORPH_JAX, MK4_NATIVE_REPTILE_INVISIBILITY, MK4_NATIVE_AIR_THROW_SLAM, MK4_NATIVE_AIR_THROW_FLIP, MK4_NATIVE_AIR_THROW, MK4_NATIVE_AIR_THROW_LIFT
  .word MK4_NATIVE_SHINNOK_MORPH_REIKO, MK4_NATIVE_SHINNOK_MORPH_CAGE
  .zero 8
  .word MK4_NATIVE_SCORPION_FIRE, MK4_NATIVE_WEAPON_DRAW, MK4_NATIVE_CAGE_GROIN_PUNCH, MK4_NATIVE_FUJIN_WIND_SLAM, MK4_NATIVE_JAX_GROUND_POUND, MK4_NATIVE_FUJIN_WIND
  .word 0x469810, 0x46b430, MK4_NATIVE_GORO_LEAP_STOMP, MK4_NATIVE_GORO_SPECIAL_PUNCH, MK4_NATIVE_GORO_SPECIAL_KICK, MK4_NATIVE_GORO_SPECIAL_UPPERCUT
  .word MK4_NATIVE_GORO_QUAKE_STOMP
  .zero 36
  .word 0x46a1e0, 0x46a470, MK4_NATIVE_FINISH_FIRST, MK4_NATIVE_FAN_FINISH, MK4_NATIVE_SPIKE_FINISH, MK4_NATIVE_FINISH_SECOND

action_table_8:
  .word MK4_NATIVE_AIR_HIGH_PUNCH, MK4_NATIVE_AIR_LOW_PUNCH, MK4_NATIVE_AIR_DIAGONAL_HIGH_KICK, MK4_NATIVE_AIR_DIAGONAL_LOW_KICK, MK4_NATIVE_AIR_KICK
  .zero 4

crouch_entry:
  .word 0x46b5e0, 0x46b1b0, 0x46f7e0, 0x46fdf0, 0x46fe80, 0x46afa0
  .word MK4_NATIVE_CROUCH_LOW_KICK, MK4_NATIVE_CROUCH_HIGH_KICK, MK4_NATIVE_CROUCH_LOW_PUNCH, 0x46b350, 0x4692d0
  .zero 4
  .word 0x46b1f0
  .zero 4
  .word 0x443, 0xfffff334
  .zero 8
  .word 0x443, 0xccc
  .zero 8

input_table_close_primary:
  .word MK4_NATIVE_PRESS_CLOSE_PUNCH_COMBO_HIGH_PUNCH, MK4_NATIVE_PRESS_FOLLOWUP_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_FOLLOWUP_HIGH_KICK, MK4_NATIVE_PRESS_FOLLOWUP_LOW_KICK
  .zero 4

input_table_close_secondary:
  .word MK4_NATIVE_PRESS_COMBO_PUNCH_HIGH_PUNCH, MK4_NATIVE_PRESS_FOLLOWUP_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_COMBO_PUNCH_HIGH_KICK, MK4_NATIVE_PRESS_FOLLOWUP_LOW_KICK
  .zero 4

input_table_close_tertiary:
  .word MK4_NATIVE_PRESS_CLOSE_KICK_HIGH_PUNCH, MK4_NATIVE_PRESS_FOLLOWUP_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_CLOSE_KICK_HIGH_KICK, MK4_NATIVE_PRESS_FOLLOWUP_LOW_KICK
  .zero 4

input_table_close_followup:
  .word MK4_NATIVE_PRESS_AIR_CONTACT_HIGH_PUNCH, MK4_NATIVE_PRESS_FOLLOWUP_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_FOLLOWUP_HIGH_KICK, MK4_NATIVE_PRESS_FOLLOWUP_LOW_KICK
  .zero 4

input_table_diagonal_air:
  .word MK4_NATIVE_PRESS_DIAGONAL_AIR_HIGH_PUNCH, MK4_NATIVE_PRESS_DIAGONAL_AIR_LOW_PUNCH
  .zero 4
  .word MK4_NATIVE_PRESS_DIAGONAL_AIR_HIGH_KICK, MK4_NATIVE_PRESS_DIAGONAL_AIR_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_019:
  .word word(unresolved_dispatch_and_motion_020), 0x471730, 0x3
  .zero 24
  .word 0x3243f, 0x3243f
  .zero 8
  .word 0xfffff334
  .zero 8
  .word 0x7ae
  .zero 4
  .word word(unresolved_dispatch_and_motion_019), word(unresolved_dispatch_and_motion_019)
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_020:
  .word 0x350ef5c, 0x350ef78, 0x350ef94, 0x350efb0, 0x450efcc, 0x450efe8
  .word 0x8950efe8, 0x408dc0, 0x8050efe8, 0x405e20

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_021:
  .word word(unresolved_dispatch_and_motion_022), 0x4718e0, 0x3
  .zero 24
  .word 0x3243f, 0x3243f
  .zero 8
  .word 0xffffe667
  .zero 8
  .word 0x7ae
  .zero 4
  .word word(unresolved_dispatch_and_motion_021), word(unresolved_dispatch_and_motion_021)
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_022:
  .word 0x250ef5c, 0x250ef78, 0x250ef94, 0x250efb0, 0x350efcc, 0x350efe8
  .word 0x8450efe8, 0x408dc0, 0x8050efe8, 0x405e20

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_023:
  .word word(unresolved_dispatch_and_motion_029), 0x471ea0, 0xf
  .zero 12
  .word 0xffff0000, 0x4ccc
  .zero 4
  .word 0xb333, 0xb333
  .zero 8
  .word 0xffffe667, 0x3333
  .zero 4
  .word 0x28f
  .zero 4
  .word word(unresolved_dispatch_and_motion_023)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_024:
  .word word(unresolved_dispatch_and_motion_029), 0x471ea0, 0xc
  .zero 12
  .word 0xffff0000, 0x4ccc
  .zero 4
  .word 0xb333, 0xb333
  .zero 8
  .word 0xffffe667, 0x3333
  .zero 4
  .word 0x28f
  .zero 4
  .word word(unresolved_dispatch_and_motion_024)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_025:
  .word word(unresolved_dispatch_and_motion_029), 0x471ea0, 0x8
  .zero 12
  .word 0xffff0000, 0x4ccc
  .zero 4
  .word 0xb333, 0xb333
  .zero 8
  .word 0xffffe667, 0x3333
  .zero 4
  .word 0x28f
  .zero 4
  .word word(unresolved_dispatch_and_motion_025)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_026:
  .word word(unresolved_dispatch_and_motion_029), 0x472120, 0x2
  .zero 24
  .word 0x9999, 0x3243f
  .zero 8
  .word 0xfffff852
  .zero 8
  .word 0x41
  .zero 4
  .word word(unresolved_dispatch_and_motion_026)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_027:
  .word word(unresolved_dispatch_and_motion_029), 0x472120, 0x2
  .zero 24
  .word 0x9999, 0x3243f
  .zero 8
  .word 0xfffff852
  .zero 8
  .word 0x41
  .zero 4
  .word word(unresolved_dispatch_and_motion_027)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_028:
  .word word(unresolved_dispatch_and_motion_029), 0x472120, 0x2
  .zero 24
  .word 0x9999, 0x3243f
  .zero 8
  .word 0xfffff852
  .zero 8
  .word 0x41
  .zero 4
  .word word(unresolved_dispatch_and_motion_028)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_029:
  .word 0x250d66c, 0x250d688, 0x250d6a4, 0x250d6c0, 0x250d6dc, 0x250d6f8
  .word 0x250d66c, 0x250d688, 0x250d6a4, 0x250d6c0, 0x250d6dc, 0x250d6f8
  .word 0x250d6f8, 0x550d6f8, 0x550d6f8, 0x550d6f8, 0x550d6f8, 0x550d6f8
  .word 0x550d6f8, 0x550d6f8, 0x8550d6f8, 0x405e20

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_030:
  .word word(unresolved_dispatch_and_motion_034), 0x472450, 0x19
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_031:
  .word word(unresolved_dispatch_and_motion_035), 0x4723a0, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_032:
  .word word(unresolved_dispatch_and_motion_035), 0x4723a0, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_033:
  .word word(unresolved_dispatch_and_motion_036), 0x4723a0, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4
  .word word(unresolved_dispatch_and_motion_030), word(unresolved_dispatch_and_motion_031), word(unresolved_dispatch_and_motion_030), word(unresolved_dispatch_and_motion_033), word(unresolved_dispatch_and_motion_032), word(unresolved_dispatch_and_motion_030)
  .word word(unresolved_dispatch_and_motion_033), word(unresolved_dispatch_and_motion_031), word(unresolved_dispatch_and_motion_030), word(unresolved_dispatch_and_motion_030), word(unresolved_dispatch_and_motion_032), word(unresolved_dispatch_and_motion_033)
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_034:
  .word 0x250f234, 0x250f250, 0x250f26c, 0x250f288, 0x250f2a4, 0x250f2c0
  .word 0x250f2dc, 0x250f2f8, 0x250f314, 0x250f330, 0x250f34c, 0x250f368
  .word 0x250f384, 0x250f3a0, 0x8950f3bc, 0x408dc0, 0x8050f3bc, 0x405e20

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_035:
  .word 0x1c506380, 0x89506380, 0x408dc0, 0x80506380, 0x405e20
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_036:
  .word 0x1c506380, 0x89506380, 0x408dc0, 0x80506380, 0x405e20
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_037:
  .word word(unresolved_dispatch_and_motion_040), 0x472710, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_038:
  .word word(unresolved_dispatch_and_motion_041), 0x472710, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_039:
  .word word(unresolved_dispatch_and_motion_042), 0x472710, 0xa
  .zero 24
  .word 0x1921f, 0x3243f
  .zero 8
  .word 0xffffcf5d
  .zero 8
  .word 0x7ae
  .zero 4
  .word word(unresolved_dispatch_and_motion_037), word(unresolved_dispatch_and_motion_038), word(unresolved_dispatch_and_motion_039)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_040:
  .word 0x1c50639c, 0x8950639c, 0x408dc0, 0x8050639c, 0x405e20
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_041:
  .word 0x1c506380, 0x89506380, 0x408dc0, 0x80506380, 0x405e20
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_042:
  .word 0x1c5063b8, 0x895063b8, 0x408dc0, 0x805063b8, 0x405e20
  .zero 4
  .word 0x50, 0x1b, 0x17, 0x15, 0x14, 0x13
  .word 0x14, 0x15, 0x14, 0x14, 0x14, 0x14
  .word 0x14, 0x13, 0x13, 0x13, 0x13, 0x13
  .word 0x13, 0x13, 0x13, 0x13, 0x13, 0x13
  .word 0x13, 0x13, 0x13, 0x13, 0x13, 0x13
  .word 0x13, 0x13, 0x13, 0x13, 0x13
  .zero 4
