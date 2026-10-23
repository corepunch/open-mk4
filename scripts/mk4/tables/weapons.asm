; Weapon playback parameters, contact follow-ups and inventory descriptors.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


weapon_parameters_begin:
  .word 0x15b0000, 0x1e0000
  .zero 16
  .word 0x490140
  .zero 4

weapon_clip_229_query_42_parameters:
  .word clip(229), 0x4070301, 0x20002a, MK4_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY, MK4_NATIVE_TANYA_WEAPON_BACK_HP_ENTRY
  .zero 4

weapon_clip_231_query_43_motion:
  .word 0x3, 0x147a, 0x8
  .zero 4

weapon_clip_231_query_43_parameters:
  .word clip(231), 0xa100401, 0x20062b, MK4_NATIVE_TANYA_WEAPON_LP_ENTRY

scepter_high_punch_parameters:
  .word clip(231), 0xa100401, 0x20061a, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_HP_ENTRY, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_HP_ENTRY
  .zero 4

scepter_back_high_punch_motion:
  .word 0x9, 0x147a, 0x8
  .zero 4

scepter_back_high_punch_parameters:
  .word clip(237), 0x9100400, 0x220006, MK4_NATIVE_SUB_ZERO_SCEPTER_LP_ENTRY

weapon_clip_250_query_38_parameters:
  .word clip(250), 0x40c0209, 0x220026, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_LP_ENTRY, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_LP_ENTRY
  .zero 4

weapon_clip_228_query_39_motion:
  .word 0x1, 0x2666, 0xe
  .zero 4

weapon_clip_228_query_39_parameters:
  .word clip(228), 0xa100505, 0x220027, MK4_NATIVE_JAREK_WEAPON_HP_ENTRY

weapon_clip_239_query_37_parameters:
  .word clip(239), 0x3090205, 0x220025, MK4_NATIVE_CAGE_WEAPON_UPPER_ENTRY, MK4_NATIVE_CAGE_WEAPON_UPPER_ENTRY
  .zero 4
  .word 0x8, 0x1999, 0xd
  .zero 36

weapon_clip_229_query_36_parameters:
  .word clip(229), 0x4070301, 0x200024, MK4_NATIVE_JAREK_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_JAREK_WEAPON_BACK_LP_ENTRY
  .zero 4

weapon_clip_228_query_35_motion:
  .word 0x1, 0x2666, 0xe
  .zero 4

weapon_clip_228_query_35_parameters:
  .word clip(228), 0xa100505, 0x220023, MK4_NATIVE_KAI_WEAPON_HIGH_PUNCH_FRONTEND

kai_weapon_high_punch_motion:
  .word 0x1, 0x2147, 0xa
  .zero 4

kai_weapon_high_punch_parameters:
  .word clip(230), 0x80b0301, 0x22001f, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_FRONTEND, MK4_NATIVE_KAI_WEAPON_LOW_PUNCH_FRONTEND
  .zero 4

weapon_clip_229_query_32_motion:
  .word 0x1, 0x1999, 0x8
  .zero 4

weapon_clip_229_query_32_parameters:
  .word clip(229), 0x4070301, 0x200020, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_FRONTEND, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_FRONTEND
  .zero 4
  .word 0x8, 0x1999, 0xd
  .zero 36

weapon_clip_237_query_28_motion:
  .word 0x9, 0x147a, 0xb
  .zero 4

weapon_clip_237_query_28_parameters:
  .word clip(237), 0x9100400, 0x22001c, MK4_NATIVE_LIU_KANG_SWORD_BACK_HP_ENTRY, MK4_NATIVE_SHINNOK_WEAPON_LP_ENTRY
  .zero 4

weapon_clip_238_query_29_motion:
  .word 0x1, 0x1999, 0x10
  .zero 4

weapon_clip_238_query_29_parameters:
  .word clip(238), 0x9190405, 0x22001d, MK4_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY
  .zero 4

weapon_clip_243_query_30_parameters:
  .word clip(243), 0x30f0209, 0x22001e, MK4_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY

reptile_weapon_high_punch_parameters:
  .word clip(230), 0x80a0305, 0x220012, MK4_NATIVE_REPTILE_WEAPON_BACK_HP_ENTRY, MK4_NATIVE_REPTILE_WEAPON_BACK_HP_ENTRY
  .zero 24
  .word 0x3333

weapon_clip_240_query_21_parameters:
  .word clip(240), 0x80d0306, 0x220015, MK4_NATIVE_REPTILE_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_REPTILE_WEAPON_BACK_LP_ENTRY
  .zero 4
  .word 0x1, 0x3333, 0x7
  .zero 140

sword_low_punch_open_parameters:
  .word clip(240), 0x80d0304, 0x22000d, MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY, 0x496500
  .zero 4

sword_low_punch_repeat_motion:
  .word 0x9, 0x147a, 0xb
  .zero 4

sword_low_punch_repeat_parameters:
  .word clip(237), 0x9100400, 0x220010, 0x4965d0

sword_back_high_punch_motion:
  .word 0x2, 0x28f5, 0x12
  .zero 4

sword_back_high_punch_parameters:
  .word clip(242), 0x90b0308, 0x220011, MK4_NATIVE_SONYA_WEAPON_HP_ENTRY

weapon_clip_230_query_8_parameters:
  .word clip(230), 0x80b0301, 0x220008, MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_SONYA_WEAPON_LP_ENTRY
  .zero 4

weapon_clip_229_query_9_parameters:
  .word clip(229), 0x4070301, 0x200009, MK4_NATIVE_SONYA_WEAPON_BACK_HP_ENTRY, MK4_NATIVE_SONYA_WEAPON_BACK_HP_ENTRY
  .zero 4
  .word 0x3, 0x1999, 0xc
  .zero 68

sword_back_low_punch_speed_delay:
  .word 0x8

sword_back_low_punch_speed:
  .word 0x1999

sword_back_low_punch_speed_duration:
  .word 0xd
  .zero 36

weapon_clip_233_query_4_parameters:
  .word clip(233), 0x8100500, 0x220004, MK4_NATIVE_QUAN_CHI_WEAPON_HP_ENTRY, MK4_NATIVE_QUAN_CHI_WEAPON_HP_ENTRY
  .zero 4

weapon_clip_238_query_7_motion:
  .word 0x8, 0x11eb, 0x11
  .zero 4

weapon_clip_238_query_7_parameters:
  .word clip(238), 0x8190300, 0x220007, 0x496ad0

weapon_clip_234_query_5_parameters:
  .word clip(234), 0x8100400, 0x220005, MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY, MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY
  .zero 4

staff_back_low_punch_motion:
  .word 0x9, 0x147a, 0xb
  .zero 4

staff_back_low_punch_parameters:
  .word clip(237), 0x9100400, 0x220006, MK4_NATIVE_SONYA_DRAW_WEAPON_HP_SLASH

weapon_clip_235_query_1_parameters:
  .word clip(235), 0x8100400, 0x220001

; Retained referenced block; semantic role is not established.
unresolved_weapons_001:
  .word MK4_NATIVE_SWORD_BACK_HP, MK4_NATIVE_SONYA_DRAW_WEAPON_LP_SLASH
  .zero 4

weapon_clip_236_query_0_parameters:
  .word clip(236), 0x8100400, 0x220000, MK4_NATIVE_SWORD_BACK_LP, MK4_NATIVE_SWORD_BACK_LP
  .zero 220

axe_high_punch_parameters:
  .word clip(231), 0xa100401, 0x20061a, MK4_NATIVE_JAX_AXE_BACK_HP_ENTRY, MK4_NATIVE_JAX_AXE_BACK_HP_ENTRY
  .zero 4

weapon_clip_229_query_23_parameters:
  .word clip(229), 0x4070301, 0x200017, MK4_NATIVE_JAX_AXE_LP_ENTRY

weapon_clip_232_query_24_parameters:
  .word clip(232), 0x40d0401, 0x200018, MK4_NATIVE_JAX_AXE_BACK_LP_ENTRY, MK4_NATIVE_JAX_AXE_BACK_LP_ENTRY
  .zero 4

strike_query:
  .word clip(230), 0x50a0307, 0x200019, 0x496d20

weapon_clip_231_parameters:
  .word clip(231), 0xa100401, 0x20061a
  .zero 4

weapon_clip_239_parameters:
  .word clip(239), 0x3090205, 0x220025
  .zero 4

cage_weapon_low_punch_parameters:
  .word clip(230), 0x80a0305, 0x220012, MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY

kai_weapon_followup_motion:
  .word 0x1, 0x2147, 0xa
  .zero 4

kai_weapon_followup_parameters:
  .word clip(230), 0x80b0301, 0x22001f
  .zero 4

reptile_weapon_back_high_punch_parameters:
  .word clip(230), 0x80a0305, 0x220012
  .zero 4
  .word 0x15, 0x16, 0x15, 0x15, 0x15, 0x15
  .word 0x15, 0x15, 0x15, 0x6e, 0x15, 0x15
  .word 0x16, 0x15, 0x15, 0x6e, 0x15, 0x15

; Retained referenced block; semantic role is not established.
unresolved_weapons_002:
  .word 0xffffcccd, 0x1999, 0xffffe667
  .zero 4
  .word 0xfffe0000, 0x494510, 0x147a, 0xfffff334
  .zero 4
  .word 0xffff0000

; Retained referenced block; semantic role is not established.
unresolved_weapons_003:
  .word 0xffffe667, 0x23d7, 0xffffe667, 0x191eb
  .zero 4
  .word 0xffffeb86, 0x191eb, 0xffff0000

; Retained referenced block; semantic role is not established.
unresolved_weapons_004:
  .word 0xffff199a, 0x170a, 0xffffd99a, 0x3248e, 0xfffe0000, 0x494510
  .zero 4
  .word 0xffffe667, 0x3248e, 0xffff0000

; Retained referenced block; semantic role is not established.
unresolved_weapons_005:
  .word 0xffffe667, 0x20c4, 0xffffd99a, 0x4b5c2

weapon_parameters_end:
  .word 0x23d7, 0xffffe667, 0x191eb
  .zero 4
  .word 0xffffeb86, 0x191eb, 0xffff0000
  .zero 4
  .word 0xffffe667, 0x23d7, 0xffffe667, 0x191eb
  .zero 4
  .word 0xffffeb86, 0x191eb, 0xffff0000

weapon_draw_followup_entries:
  .word MK4_NATIVE_SWORD_BACK_LP, MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY, MK4_NATIVE_SONYA_WEAPON_HP_ENTRY, MK4_NATIVE_KAI_WEAPON_FOLLOWUP_ENTRY, MK4_NATIVE_SWORD_BACK_LP, MK4_NATIVE_SWORD_BACK_LP
  .word MK4_NATIVE_LIU_WEAPON_LOW_PUNCH, MK4_NATIVE_REPTILE_WEAPON_ALTERNATE_HIGH_PUNCH, MK4_NATIVE_FUJIN_CROSSBOW_FOLLOWUP_ENTRY, MK4_NATIVE_JAX_AXE_BACK_LP_ENTRY, MK4_NATIVE_JAREK_WEAPON_LP_ENTRY, MK4_NATIVE_SUB_ZERO_DRAW_WEAPON_SCEPTER_FOLLOW_UP
  .word 0x496da0, MK4_NATIVE_TANYA_WEAPON_HP_ENTRY, MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY, 0x494d00, 0x495110
  .word 0x4951b0, 0x4951b0, 0x4951c0, 0x495280, 0x4951e0, 0x494f30
  .word 0x494e10, 0x495300
  .zero 16
  .word MK4_NATIVE_SONYA_DRAW_WEAPON_HP_SLASH, MK4_NATIVE_QUAN_CHI_WEAPON_HP_ENTRY, MK4_NATIVE_SONYA_WEAPON_LP_ENTRY, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_FRONTEND, MK4_NATIVE_SHINNOK_WEAPON_HP_ENTRY, MK4_NATIVE_SWORD_BACK_LP
  .word MK4_NATIVE_LIU_WEAPON_LOW_PUNCH, MK4_NATIVE_REPTILE_WEAPON_LP_ENTRY, 0x495cc0, MK4_NATIVE_JAX_AXE_BACK_HP_ENTRY, MK4_NATIVE_JAREK_WEAPON_HP_ENTRY, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_HP_ENTRY
  .word 0x496da0, 0x496da0, 0x496da0, 0x496da0, 0x496da0
  .zero 4
  .word MK4_NATIVE_SWORD_BACK_LP, MK4_NATIVE_RAIDEN_STAFF_BACK_LP_ENTRY, MK4_NATIVE_CAGE_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_KAI_WEAPON_LOW_PUNCH_FRONTEND, MK4_NATIVE_SHINNOK_WEAPON_BACK_LP_ENTRY, MK4_NATIVE_SWORD_BACK_LP
  .word MK4_NATIVE_LIU_KANG_SWORD_BACK_HP_ENTRY, MK4_NATIVE_REPTILE_WEAPON_HP_ENTRY, 0x495c90, MK4_NATIVE_JAX_AXE_BACK_LP_ENTRY, MK4_NATIVE_CAGE_WEAPON_UPPER_ENTRY, MK4_NATIVE_SUB_ZERO_SCEPTER_BACK_LP_ENTRY
  .word 0x496da0, 0x496da0, 0x496da0, 0x496da0, 0x496da0
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_006:
  .word MK4_NATIVE_QUAN_CHI_WEAPON_BACK_HP_ENTRY, MK4_NATIVE_RAIDEN_STAFF_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_007:
  .word MK4_NATIVE_CAGE_WEAPON_BACK_HP_ENTRY, MK4_NATIVE_CAGE_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_008:
  .word MK4_NATIVE_TANYA_WEAPON_HP_ENTRY, MK4_NATIVE_TANYA_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_009:
  .word MK4_NATIVE_SUB_ZERO_SCEPTER_HP_ENTRY, MK4_NATIVE_SUB_ZERO_SCEPTER_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_010:
  .word MK4_NATIVE_JAREK_WEAPON_HP_ENTRY, MK4_NATIVE_JAREK_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_011:
  .word MK4_NATIVE_KAI_WEAPON_HIGH_PUNCH_FRONTEND, MK4_NATIVE_KAI_WEAPON_LOW_PUNCH_FRONTEND, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_012:
  .word MK4_NATIVE_SHINNOK_WEAPON_HP_ENTRY, MK4_NATIVE_SHINNOK_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_013:
  .word MK4_NATIVE_JAX_AXE_HP_ENTRY, MK4_NATIVE_JAX_AXE_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_014:
  .word MK4_NATIVE_SONYA_DRAW_WEAPON_HP_SLASH, MK4_NATIVE_SONYA_DRAW_WEAPON_LP_SLASH, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_015:
  .word MK4_NATIVE_RAIDEN_STAFF_HP_ENTRY, MK4_NATIVE_RAIDEN_STAFF_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_016:
  .word MK4_NATIVE_SONYA_WEAPON_HP_ENTRY, MK4_NATIVE_SONYA_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_017:
  .word MK4_NATIVE_LIU_WEAPON_HIGH_PUNCH, MK4_NATIVE_LIU_WEAPON_LOW_PUNCH, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_018:
  .word MK4_NATIVE_REPTILE_WEAPON_HP_ENTRY, MK4_NATIVE_REPTILE_WEAPON_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_weapons_019:
  .word MK4_NATIVE_FUJIN_CROSSBOW_HP_ENTRY, MK4_NATIVE_FUJIN_CROSSBOW_LP_ENTRY, MK4_NATIVE_PRESS_STANDING_BLOCK, MK4_NATIVE_PRESS_STANDING_HIGH_KICK, MK4_NATIVE_PRESS_STANDING_LOW_KICK
  .zero 4

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_tanya:
  .word 0x6d, word(weapon_descriptor_tanya_rig), word(unresolved_scene_assets_450), word(unresolved_weapons_008), clip(285), clip(275), clip(276), 0xffffcccd

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_scepter:
  .word 0x6b, word(weapon_descriptor_scepter_rig), word(unresolved_scene_assets_448), word(unresolved_weapons_009), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_jarek:
  .word 0x6a, word(weapon_descriptor_jarek_rig), word(unresolved_scene_assets_447), word(unresolved_weapons_010), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_sword:
  .word 0x60, word(weapon_descriptor_sword_rig), word(unresolved_scene_assets_440), word(unresolved_weapons_014), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_raiden_staff:
  .word 0x61, word(weapon_descriptor_raiden_staff_rig), word(unresolved_scene_assets_443), word(unresolved_weapons_015), clip(286), clip(277), clip(278), 0xffff8000

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_sonya:
  .word 0x62, word(weapon_descriptor_sonya_rig), word(unresolved_scene_assets_442), word(unresolved_weapons_016), clip(285), clip(275), clip(276), 0xffffa667

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_dragon_sword:
  .word 0x66, word(weapon_descriptor_dragon_sword_rig), word(unresolved_scene_assets_438), word(unresolved_weapons_017), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_reptile:
  .word 0x67, word(weapon_descriptor_reptile_rig), word(unresolved_scene_assets_444), word(unresolved_weapons_018), clip(286), clip(277), clip(278), 0xffff999a

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_fujin:
  .word 0x68, word(weapon_descriptor_fujin_rig), word(unresolved_scene_assets_445), word(unresolved_weapons_019), clip(210), clip(279), clip(280), 0xffffcccd

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_shinnok:
  .word 0x64, word(weapon_descriptor_shinnok_rig), word(unresolved_scene_assets_441), word(unresolved_weapons_012), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_axe:
  .word 0x69, word(weapon_descriptor_axe_rig), word(unresolved_scene_assets_446), word(unresolved_weapons_013), clip(285), clip(275), clip(276), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_kai:
  .word 0x63, word(weapon_descriptor_kai_rig), word(unresolved_scene_assets_439), word(unresolved_weapons_011), clip(285), clip(275), clip(276), 0xffffc000

; Retained referenced block; semantic role is not established.
unresolved_weapons_020:
  .word 0x6c  ; numeric; printable bytes do not establish text
  .word word(goro_weapon_rig), word(unresolved_scene_assets_449), word(unresolved_weapons_011), clip(285), clip(275), clip(276)
  .zero 4

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_quan_chi:
  .word 0x6f, word(weapon_descriptor_quan_chi_rig), word(unresolved_body_assets_117), word(unresolved_weapons_006), clip(286), clip(277), clip(278), 0xffffb334

; Weapon inventory: type, packed rig, hand binding, buttons, idle, forward, backward, reserved.
; Reader: game/data/tables.c:mk4_weapon_read; 8 words.
weapon_descriptor_cage:
  .word 0x6e, word(weapon_descriptor_cage_rig), word(unresolved_body_assets_118), word(unresolved_weapons_007), clip(286), clip(277), clip(278), 0xffffb334

; Retained referenced block; semantic role is not established.
unresolved_weapons_021:
  .word 0x290, word(unresolved_body_assets_004), word(unresolved_body_assets_119), word(unresolved_weapons_014), clip(286), clip(277)
  .word clip(278), 0xffffb334

; Retained referenced block; semantic role is not established.
unresolved_weapons_022:
  .word 0x6a  ; numeric; printable bytes do not establish text
  .word word(unresolved_body_assets_004), word(unresolved_body_assets_119), word(unresolved_weapons_010), clip(286), clip(277), clip(278)
  .word 0xffffb334

; Retained referenced block; semantic role is not established.
unresolved_weapons_023:
  .word 0x68  ; numeric; printable bytes do not establish text
  .word word(unresolved_scene_assets_267), word(unresolved_scene_assets_451), word(unresolved_weapons_019), clip(210), clip(279), clip(280)
  .word 0xffffcccd

; Retained referenced block; semantic role is not established.
unresolved_weapons_024:
  .word 0x6d  ; numeric; printable bytes do not establish text
  .word word(weapon_descriptor_kai_rig), word(unresolved_scene_assets_439), word(unresolved_weapons_008), clip(285), clip(275), clip(276)
  .zero 4
  .word 0x6a, word(weapon_descriptor_kai_rig), word(unresolved_scene_assets_439), word(unresolved_weapons_010), clip(286), clip(277)
  .word clip(278), 0xffffb334

; Retained referenced block; semantic role is not established.
unresolved_weapons_025:
  .word 0x6a  ; numeric; printable bytes do not establish text
  .word word(noob_saibot_weapon_rig), word(unresolved_scene_assets_452), word(unresolved_weapons_010), clip(286), clip(277), clip(278)
  .word 0xffffb334
  .zero 72
  .word 0x15, 0x496f10, 0x496f40, 0x496f60
  .zero 40
  .word word(unresolved_scene_assets_262), 0x7333, 0xffffe667, word(unresolved_scene_assets_193), 0x4, 0x49bbd0
  .word 0x49bb00, 0x49bb30
  .zero 52
  .word 0x497340, 0x4973a0, 0x4973f0
  .zero 88
  .word word(unresolved_body_assets_007), 0x6666, 0xffffc7af
  .zero 4
  .word 0x4, 0x4976b0, 0x4978e0, 0x497ab0
  .zero 24
  .word 0x14, 0x497de0, 0x497e60, 0x497e60
  .zero 4
  .word 0xfffff0a4, 0x51e
  .zero 4
  .word 0x12, 0x4986d0, 0x498730, 0x498710, 0x13, 0x4986d0
  .word 0x498730, 0x498710
  .zero 112
  .word 0x11, 0x498a90, 0x498ac0, 0x498b50
  .zero 72
