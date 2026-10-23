; Projectile motion/contact records and action dispatch.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


fireball_standing_contact_row:
  .zero 4
  .word 0x498df0, 0x498e30, 0x498e50
  .zero 40

fireball_air_contact_row:
  .zero 4
  .word 0x498f20, 0x498e30, 0x498e50
  .zero 40
  .word word(unresolved_body_assets_009), 0x3333, 0xffffcccd, word(unresolved_scene_assets_471)
  .zero 96
  .word word(unresolved_body_assets_009), 0xffff999a, 0xfffffd71, word(unresolved_scene_assets_471)
  .zero 88
  .word 0x7, 0x499ae0, 0x499b40, 0x499c00
  .zero 32
  .word 0x10, 0x49bb50, 0x49a5c0, 0x49a5f0, word(unresolved_scene_assets_265), 0xccc
  .word 0xffffcccd, word(unresolved_scene_assets_205)
  .zero 228
  .word 0x4509d5c, 0x4509d40, 0x2509d24, 0x2509d08, 0x1509cec, 0x1509cd0
  .word 0x1509cb4, 0x80509cb4, 0x405dd0, 0x14ed8e0
  .zero 4
  .word word(unresolved_scene_assets_356), 0xcccc, 0xffffe667, word(unresolved_projectile_parameters_001), word(unresolved_scene_assets_262), 0xccc
  .word 0xffffe667, word(unresolved_scene_assets_193), 0x4, 0x49bbd0, 0x49bb00, 0x49bb30
  .zero 72
  .word word(unresolved_scene_assets_264), 0xccc, 0xfffffd71, word(unresolved_scene_assets_195), 0x3, 0x49bb90
  .word 0x49bd90, 0x49bdb0
  .zero 104
  .word word(unresolved_body_assets_008), 0x28f5
  .zero 4
  .word word(unresolved_scene_assets_470), 0x2, 0x49bed0, 0x49bf00, 0x49bf30
  .zero 180
  .word 0x20c, word(unresolved_scene_assets_263), 0x9999, 0xffffdeb9, word(unresolved_scene_assets_194), word(unresolved_scene_assets_263)
  .word 0x28f5, 0x35c2, word(unresolved_scene_assets_194), 0x1, 0x49c240, 0x49c2b0
  .word 0x49c2e0, word(unresolved_scene_assets_263), 0x1999, 0xffff8ccd, word(unresolved_scene_assets_194)
  .zero 4
  .word 0x49c240, 0x49c2b0, 0x49c2e0, 0x47, 0x49a520
  .zero 72
  .word word(unresolved_scene_assets_263), 0x2e14, 0xffffe667, word(unresolved_scene_assets_194), 0x16, 0x49c240
  .word 0x49c2b0, 0x49c2e0
  .zero 24

fireball_spawn_forward_offset:
  .word 0x3333  ; numeric; printable bytes do not establish text

fireball_spawn_y_offset:
  .word 0x1999
  .zero 24

action_table_2:
  .word MK4_NATIVE_GORO_FIREBALL, MK4_NATIVE_LIU_KANG_HIGH_FIREBALL, MK4_NATIVE_LIU_KANG_LOW_FIREBALL, MK4_NATIVE_RAIDEN_LIGHTNING, MK4_NATIVE_SUB_ZERO_ICE, MK4_NATIVE_SONYA_RING
  .word MK4_NATIVE_SCORPION_SPEAR, MK4_NATIVE_QUAN_CHI_SKULL, MK4_NATIVE_KAI_RISING_SKULL, MK4_NATIVE_KAI_GROUND_SKULL, MK4_NATIVE_FIREBALL, MK4_NATIVE_AIR_FIREBALL
  .word MK4_NATIVE_REPTILE_ACID_SPIT, 0x498270, MK4_NATIVE_JAREK_BLADE, 0x49a370, 0x499db0, 0x499c30
  .word 0x498250, MK4_NATIVE_CAGE_LOW_ORB, MK4_NATIVE_CAGE_HIGH_ORB, MK4_NATIVE_JAX_MISSILE, 0x497ae0, 0x497ad0
  .word MK4_NATIVE_REIKO_STAR, 0x49a190, MK4_NATIVE_GORO_FIREBALL
  .zero 4
  .word 0x81506a64, 0x40d720, 0x81506a80, 0x40d720, 0x81506a9c, 0x40d720
  .word 0x81506ab8, 0x40d720, 0x81506ad4, 0x40d720, 0x81506af0, 0x40d720
  .word 0x81506b0c, 0x40d720, 0x81506b28, 0x40d720
  .zero 8
  .word 0x4509d5c, 0x4509d40, 0x2509d24, 0x2509d08, 0x1509cec, 0x1509cd0
  .word 0x1509cb4, 0x80509cb4, 0x405dd0
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_001:
  .word 0x14ed8e0
  .zero 4
  .word 0x8250c364, 0x4997d0, 0x8450c364, 0x4997f0, 0x8250c364, 0x4997d0
  .zero 8
  .word 0x8250c364, 0x4997d0, 0x8450c364, 0x4997f0, 0x8450c364, 0x4997d0
  .word 0x8450c364, 0x4997f0, 0x8450c364, 0x4997d0, 0x8450c364, 0x4997f0
  .word 0x8450c364, 0x4997d0, 0x8250c364, 0x4997f0, 0x9e50c364, 0x499850
  .word 0x8050c364, 0x405dd0, 0x8350c380, 0x499810, 0x8350c380, 0x499830
  .zero 8
  .word 0x7, 0x49f870, 0x8, 0x49f880, 0x9, 0x49f890
  .word 0x11, 0x49f810, 0xffffffff
  .zero 4
  .word 0x8c
  .zero 20
  .word 0x1, 0x49f130, 0x2, 0x49f2e0, 0x4, 0x49f470
  .word 0x8, 0x49f5e0, 0x100, 0x49f160, 0x200, 0x49f310
  .word 0x400, 0x49f4a0, 0x800, 0x49f610

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_002:
  .word 0x9, 0x4, 0x5, 0xa, 0x9, 0x8
  .word 0xd, 0xe, 0x9, 0xa, 0x5, 0x4
  .word 0x3, 0x2, 0x1
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_003:
  .word 0xc, 0xd, 0x8, 0x3, 0x4, 0x5
  .word 0xa, 0xf, 0xe, 0xd, 0x8, 0x3
  .word 0x2, 0x1, 0x2
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_004:
  .word 0x7, 0x8, 0x9, 0xa, 0xf, 0xe
  .word 0xd, 0x8, 0x9, 0x4, 0x3, 0x2
  .word 0x7, 0x8, 0x3
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_005:
  .word 0x6, 0x1, 0x2, 0x3, 0x4, 0x5
  .word 0xa, 0xf, 0xe, 0xd, 0x8, 0x7
  .word 0x2, 0x3, 0x4
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_006:
  .word 0x1, 0x6, 0xb, 0xc, 0xd, 0x8
  .word 0x3, 0x2, 0x7, 0x8, 0x9, 0xe
  .word 0xf, 0xa, 0x5
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_007:
  .word 0xa, 0x9, 0x8, 0x7, 0x6, 0xb
  .word 0xc, 0xd, 0x8, 0x3, 0x2, 0x7
  .word 0xc, 0xb, 0x6
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_008:
  .word 0x5, 0x4, 0x3, 0x8, 0xd, 0xe
  .word 0x9, 0x4, 0x3, 0x2, 0x1, 0x6
  .word 0xb, 0xc, 0x7
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_009:
  .word 0x4, 0x9, 0xe, 0xd, 0xc, 0x7
  .word 0x2, 0x1, 0x6, 0xb, 0xc, 0xd
  .word 0xe, 0x9, 0x8
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_010:
  .word 0x3, 0x8, 0xd, 0xe, 0x9, 0x8
  .word 0x7, 0x6, 0x1, 0x2, 0x3, 0x4
  .word 0x5, 0xa, 0x9
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_011:
  .word 0xe, 0xf, 0xa, 0x9, 0x4, 0x3
  .word 0x2, 0x1, 0x6, 0x7, 0xc, 0xd
  .word 0xe, 0xf, 0xa
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_012:
  .word 0x3, 0x4, 0x9, 0xa, 0xf, 0xe
  .word 0x9, 0x8, 0x3, 0x2, 0x1, 0x6
  .word 0x7, 0xc, 0xb
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_013:
  .word 0x2, 0x3, 0x4, 0x9, 0xa, 0xf
  .word 0xe, 0x9, 0x8, 0x3, 0x2, 0x1
  .word 0x6, 0x7, 0xc
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_014:
  .word 0x7, 0x8, 0x9, 0xa, 0xf, 0xe
  .word 0xd, 0xc, 0xb, 0x6, 0x7, 0x2
  .word 0x3, 0x8, 0xd
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_015:
  .word 0xa, 0x9, 0x8, 0x7, 0x2, 0x1
  .word 0x6, 0x7, 0x8, 0xd, 0xe, 0x9
  .word 0xa, 0xf, 0xe
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_016:
  .word 0x3, 0x4, 0x9, 0xa, 0x5, 0x4
  .word 0x3, 0x2, 0x7, 0x6, 0xb, 0xc
  .word 0xd, 0xe, 0xf
  .zero 4
  .word 0x6, 0x6, 0x6, 0x6, 0x8, 0x8
  .word 0x8, 0x8, 0xa, 0xa, 0xc, 0xc
  .word 0x10, 0x12, 0x14, 0x19

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_017:
  .word word(unresolved_fighter_assets_028), 0x49fab0, word(unresolved_arenas_032), word(unresolved_fighter_assets_046), word(unresolved_fighter_assets_023), word(unresolved_fighter_assets_086)
  .word word(unresolved_fighter_assets_087), word(unresolved_fighter_assets_076), word(unresolved_scene_assets_268), word(unresolved_scene_assets_269), word(unresolved_scene_assets_276), word(unresolved_scene_assets_277)
  .word word(unresolved_scene_assets_272), word(unresolved_scene_assets_273), word(unresolved_scene_assets_280), word(unresolved_scene_assets_281), word(unresolved_fighter_assets_034)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_018:
  .word word(unresolved_fighter_assets_021), 0x49fae0, word(unresolved_arenas_033), word(unresolved_fighter_assets_020), word(unresolved_fighter_assets_048), word(unresolved_fighter_assets_022)
  .word word(unresolved_fighter_assets_040), word(unresolved_fighter_assets_031), word(unresolved_scene_assets_270), word(unresolved_scene_assets_271), word(unresolved_scene_assets_278), word(unresolved_scene_assets_279)
  .word word(unresolved_scene_assets_274), word(unresolved_scene_assets_275), word(unresolved_scene_assets_282), word(unresolved_scene_assets_283), word(unresolved_fighter_assets_043)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_019:
  .word 0xb, word(unresolved_arenas_026)
  .zero 4
  .word 0x300002, 0x30000020, 0x1, word(unresolved_arenas_007)
  .zero 4
  .word 0x600001, 0x60000010, 0x6, word(unresolved_arenas_012)
  .zero 4
  .word 0x300001, 0x30000010, 0x3, word(unresolved_arenas_011)
  .zero 4
  .word 0x600002, 0x60000020, 0xa, word(unresolved_arenas_009)
  .zero 4
  .word 0x200003, 0x20000030
  .zero 4
  .word word(unresolved_arenas_008)
  .zero 4
  .word 0x700000, 0x70000000, 0xd, word(unresolved_arenas_036)
  .zero 4
  .word 0x200005, 0x20000050, 0x7, word(unresolved_arenas_040)
  .zero 4
  .word 0x100006, 0x10000060, 0xe, word(unresolved_arenas_038)
  .zero 4
  .word 0x7, 0x70, 0xc, word(unresolved_arenas_034)
  .zero 4
  .word 0x500004, 0x50000040, 0x9, word(unresolved_arenas_028)
  .zero 4
  .word 0x400003, 0x40000030, 0x5, word(unresolved_arenas_030)
  .zero 4
  .word 0x500001, 0x50000010, 0x4, word(unresolved_arenas_010)
  .zero 4
  .word 0x300004, 0x30000040, 0x8, word(unresolved_arenas_014)
  .zero 4
  .word 0x500004, 0x50000040, 0x2, word(unresolved_arenas_013)
  .zero 4
  .word 0x100006, 0x10000060
  .zero 4
  .word 0x4ef2f4, 0x4ef2f0, 0x1
  .zero 4
  .word 0x53d728, 0x4d00bc, 0x4d00cc, 0x53d72c, 0x4d00b4, 0x4d00c4

; Retained referenced block; semantic role is not established.
unresolved_projectile_parameters_020:
  .word 0x53d730, 0x4d00b8, 0x4d00c8, 0x53d93c, 0x4d00c0, 0x4d00d0
  .word 0x4d00ac, 0x4d00dc, 0x4d00ec, 0x4d00a4, 0x4d00d4, 0x4d00e4
  .word 0x4d00a8, 0x4d00d8, 0x4d00e8, 0x4d00b0, 0x4d00e0, 0x4d00f0
  .word 0xa0000, 0x50000
  .zero 16
  .word 0xfff88000
  .zero 8
  .word 0xfffd0000, 0xfff4c000
  .zero 4
  .word 0xfffb0000, 0xfffe0000
  .zero 8
  .word 0x50000, 0xfffe0000
  .zero 8
  .word 0x105, 0x4a9c20, 0xe, 0x4ef2e8
  .zero 4
  .word 0xa0000, 0x10000, 0x200, 0x4a7580, 0xf, 0x4ef2e0
  .zero 4
  .word 0x200000, 0x10000, 0x301, 0x4a9930, 0x10, 0x4ef2d4
  .zero 4
  .word 0x360000, 0x10000, 0x402, 0x4a51f0, 0x12, 0x4ef2c8
  .zero 4
  .word 0x4c0000, 0x10000, 0x503, 0x4a26a0, 0x13, 0x4ef2bc
  .zero 4
  .word 0x620000, 0x10000, 0x4, 0x4a2730, 0x14, 0x4ef2b0
  .zero 4
  .word 0x780000, 0x10000, 0xffff
  .zero 4
  .word 0x15, 0x4ce270
  .zero 4
  .word 0xba0000, 0x1c000, 0xffff
  .zero 4
  .word 0x17, 0x4ce278
  .zero 4
  .word 0xd00000, 0x1c000, 0xffff
  .zero 4
  .word 0x18, 0x4ce298
  .zero 4
  .word 0xba0000, 0x1c000, 0xffff
  .zero 4
  .word 0x18, 0x4ce2a8
  .zero 4
  .word 0xd00000, 0x1c000, 0x1, 0x1, 0x4ef2a8, 0x4ef2a0
  .word 0x4ef298, 0x4ef290, 0x4ef288, 0x4ef280, 0x4ef278, 0x4ef270
  .word 0x4ef268, 0x4ef260, 0x4ef258, 0x2, 0x4
  .zero 4
