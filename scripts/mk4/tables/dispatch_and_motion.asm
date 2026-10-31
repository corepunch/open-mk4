; Command predicates, input dispatch and shared native motion/contact parameters.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


command_next:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_001:
  .word native_sub_zero_can_slide, native_weapon_can_pickup, native_weapon_can_draw, native_can_player_pair_air_throw, native_sonya_can_leg_grab, native_jax_air_throw_command_gate
  .word native_scorpion_can_teleport, native_sub_zero_can_clone, native_fujin_can_wind, native_fujin_can_wind_slam, 0x45eba0, native_sub_zero_can_ice
  .word native_kai_can_handstand, native_fujin_can_dive, native_kai_can_ground_skull, 0x45ea50, native_liu_kang_can_bicycle, native_has_gravity
  .word native_kai_can_rising_skull, native_fujin_can_wind_grab, native_scorpion_can_spear, native_raiden_can_teleport, native_reptile_can_invisibility, native_reptile_can_spit
  .word native_sonya_can_cartwheel, native_can_fan_finish, native_can_finish_close, native_sub_zero_can_first_fatality, native_can_sidestep, native_reptile_can_dash
  .word native_jax_can_dash_punch, native_cage_can_groin_punch, native_tanya_can_spin_kick, native_jax_can_missile, native_reiko_can_flip_kick, native_reiko_can_orbit
  .word native_can_finish_range, native_liu_kang_can_first_fatality, native_fujin_can_first_fatality
  .zero 4
  .word native_reiko_can_first_fatality, native_quan_chi_can_first_fatality, native_tanya_can_first_fatality, native_reptile_can_first_fatality, native_kai_can_first_fatality, native_jax_can_fatality_1
  .word native_sonya_can_first_fatality
  .zero 4
  .word native_reiko_can_star, native_shinnok_can_morph, native_can_spike_finish, clip(317), 0x4609f0
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
  .word native_press_standing_high_punch, native_press_standing_low_punch, native_press_standing_block, native_press_standing_high_kick, native_press_standing_low_kick
  .zero 4

input_table_crouching:
  .word native_press_crouching_high_punch, native_press_crouching_low_punch, native_press_crouching_block, native_press_crouching_high_kick, native_press_crouching_low_kick
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
  .word 0x800, 0x4627f0, 0x1, native_task_retire, 0x2, native_task_retire
  .word 0x100, native_task_retire, 0x200, native_task_retire

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
  .word native_contact_query_b, 0xa, 0xa, 0x9, 0xffffffff, native_fujin_high_kick_contact_recovery
  .zero 20

kick_contact_wide:
  .word 0x1, native_contact_query_b, 0x11, 0x11, 0x10, 0xffffffff
  .word native_jax_high_kick_contact_recovery
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
  .word 0x4, 0x28, native_contact_query_b, native_goro_air_kick_recovery_callback, native_air_straight_kick_land, native_air_straight_kick_miss_reverse
  .zero 456

air_lk_tail:
  .word 0xa, 0x1b, native_contact_query_b, native_air_low_kick_contact_hold, native_air_attack_land_or_finish, native_air_low_kick_clip_58_reverse
  .word 0x46e300
  .zero 28

air_lk_alt_tail:
  .word 0xa, 0x1c, native_contact_query_b, native_air_low_kick_contact_hold, native_air_attack_land_or_finish, native_air_low_kick_clip_59_reverse
  .word 0x46e300
  .zero 28

air_lk_wide_tail:
  .word 0xa, 0x1d, native_contact_query_b, native_air_low_kick_contact_hold, native_air_attack_land_or_finish, native_air_low_kick_clip_60_reverse
  .word 0x46e300
  .zero 148

air_diagonal_tail:
  .word clip(55)
  .zero 4
  .word native_air_diagonal_kick_query_30
  .zero 4

air_diagonal_kick_query_30_row:
  .word 0xa, 0x1e, native_contact_query_b, native_air_diagonal_kick_contact_hold, native_air_attack_land_or_finish, native_air_diagonal_kick_clip_55_reverse
  .word 0x46e300
  .zero 28

air_diagonal_alt_tail:
  .word clip(56)
  .zero 4
  .word native_air_diagonal_kick_query_31
  .zero 4

air_diagonal_kick_query_31_row:
  .word 0xa, 0x1f, native_contact_query_b, native_air_diagonal_kick_contact_hold, native_air_attack_land_or_finish, native_air_diagonal_kick_clip_56_reverse
  .word 0x46e300
  .zero 28

air_diagonal_wide_tail:
  .word clip(57)
  .zero 4
  .word native_air_diagonal_kick_query_32
  .zero 4

air_diagonal_kick_query_32_row:
  .word 0xa, 0x20, native_contact_query_b, native_air_diagonal_kick_contact_hold, native_air_attack_land_or_finish, native_air_diagonal_kick_clip_57_reverse
  .word 0x46e300
  .zero 92

air_hp_tail:
  .word clip(97), 0x5, native_air_high_punch_contact_window
  .zero 4

air_high_punch_contact_row:
  .word 0xa, 0x4, native_contact_query_a, native_goro_air_hp_recovery_callback, native_air_attack_land_or_finish

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_017:
  .word native_air_high_punch_miss_reverse, 0x46e300
  .zero 68

weapon_air_high_punch_parameters:
  .word clip(252), 0x6, 0x46eae0
  .zero 20

weapon_air_low_punch_parameters:
  .word clip(251), 0x7, 0x46ec40, 0x46e300
  .zero 24

air_lp_tail:
  .word clip(96), 0x9, native_air_contact_begin
  .zero 4

air_low_punch_contact_row:
  .word 0xa, 0x5, native_contact_query_a, native_goro_air_lp_recovery_callback, native_air_attack_land_or_finish, native_air_low_punch_miss_reverse
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
  .word 0xffffcf5d, 0x28f, native_landing_motion, 0x46fcd0, 0x4706f0
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
  .word 0x2, native_contact_query_b, 0xa, 0xb, 0xc, 0xffffffff
  .word native_high_kick_narrow_contact_recovery
  .zero 636

close_high_punch:
  .word clip(262)

; Retained referenced block; semantic role is not established.
unresolved_dispatch_and_motion_018:
  .word clip(263), clip(264), clip(262), clip(263), clip(264), clip(262)
  .word clip(263), clip(264), clip(262), clip(263), clip(264), clip(262)
  .word clip(263), clip(264), clip(263)

combo_high_punch:
  .word native_jax_combo_hp_entry, native_combo_hp_entry, native_kitana_combo_hp_entry, native_jax_combo_hp_entry, native_combo_hp_entry, native_kitana_combo_hp_entry
  .word native_jax_combo_hp_entry, native_combo_hp_entry, native_kitana_combo_hp_entry, native_jax_combo_hp_entry, native_combo_hp_entry, native_kitana_combo_hp_entry
  .word native_jax_combo_hp_entry, native_combo_hp_entry, native_kitana_combo_hp_entry, native_combo_hp_entry
  .zero 536
  .word 0x8, 0x20, 0x2, 0x21
  .zero 4
  .word 0x22, 0xffffffff
  .zero 4

input_table_straight_air:
  .word native_press_straight_air_high_punch, native_press_straight_air_low_punch
  .zero 4
  .word native_press_straight_air_kick, native_press_straight_air_kick
  .zero 4

input_table_air_combo:
  .zero 4
  .word native_press_straight_air_low_punch
  .zero 4
  .word native_press_straight_air_kick, native_press_straight_air_kick
  .zero 4

action_table_1:
  .word native_task_park_40_tick_loop, native_uppercut_entry, 0x42c560, native_weapon_pickup, native_normal_low_kick, native_high_kick
  .word native_sweep, native_roundhouse, native_normal_low_punch, native_close_high_punch, native_air_high_punch, native_high_punch
  .word native_close_high_kick, native_air_kick, native_air_low_punch, native_sidestep, native_air_diagonal_high_kick, native_air_diagonal_low_kick
  .word native_quan_chi_weapon_steal, native_shinnok_morph_scorpion, native_shinnok_morph_raiden, native_shinnok_morph_sonya, native_shinnok_morph_liu_kang, native_shinnok_morph_sub_zero
  .word native_shinnok_morph_fujin, native_shinnok_morph_jarek, native_shinnok_morph_quan_chi, native_shinnok_morph_tanya, native_shinnok_morph_reptile, native_shinnok_morph_kai
  .word native_shinnok_morph_jax, native_reptile_invisibility, native_air_throw_slam, native_air_throw_flip, native_air_throw, native_air_throw_lift
  .word native_shinnok_morph_reiko, native_shinnok_morph_cage
  .zero 8
  .word native_scorpion_fire, native_weapon_draw, native_cage_groin_punch, native_fujin_wind_slam, native_jax_ground_pound, native_fujin_wind
  .word 0x469810, 0x46b430, native_goro_leap_stomp, native_goro_special_punch, native_goro_special_kick, native_goro_special_uppercut
  .word native_goro_quake_stomp
  .zero 36
  .word 0x46a1e0, 0x46a470, native_finish_first, native_fan_finish, native_spike_finish, native_finish_second

action_table_8:
  .word native_air_high_punch, native_air_low_punch, native_air_diagonal_high_kick, native_air_diagonal_low_kick, native_air_kick
  .zero 4

crouch_entry:
  .word 0x46b5e0, 0x46b1b0, 0x46f7e0, 0x46fdf0, 0x46fe80, 0x46afa0
  .word native_crouch_low_kick, native_crouch_high_kick, native_crouch_low_punch, 0x46b350, 0x4692d0
  .zero 4
  .word 0x46b1f0
  .zero 4
  .word 0x443, 0xfffff334
  .zero 8
  .word 0x443, 0xccc
  .zero 8

input_table_close_primary:
  .word native_press_close_punch_combo_high_punch, native_press_followup_low_punch
  .zero 4
  .word native_press_followup_high_kick, native_press_followup_low_kick
  .zero 4

input_table_close_secondary:
  .word native_press_combo_punch_high_punch, native_press_followup_low_punch
  .zero 4
  .word native_press_combo_punch_high_kick, native_press_followup_low_kick
  .zero 4

input_table_close_tertiary:
  .word native_press_close_kick_high_punch, native_press_followup_low_punch
  .zero 4
  .word native_press_close_kick_high_kick, native_press_followup_low_kick
  .zero 4

input_table_close_followup:
  .word native_press_air_contact_high_punch, native_press_followup_low_punch
  .zero 4
  .word native_press_followup_high_kick, native_press_followup_low_kick
  .zero 4

input_table_diagonal_air:
  .word native_press_diagonal_air_high_punch, native_press_diagonal_air_low_punch
  .zero 4
  .word native_press_diagonal_air_high_kick, native_press_diagonal_air_low_kick
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
