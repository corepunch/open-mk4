; Stance/animation selectors, contact-query tables and weapon inventories.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


roster_def:
  .word clip(322), clip(245), clip(257), clip(220)

animation_descriptor_weapon_stance:
  .word clip(259), clip(217), clip(218)
  .zero 4

crouch_hold_descriptors:
  .word clip(258), clip(219)

crouch_low_punch:
  .word clip(260), clip(225)

animation_descriptor_close_high_kick:
  .word clip(314), clip(304), clip(304), clip(301), clip(301)
  .zero 4
  .word word(unresolved_fighter_assets_050), word(unresolved_fighter_assets_073), word(unresolved_fighter_assets_051), word(unresolved_fighter_assets_052), word(unresolved_fighter_assets_063), word(unresolved_fighter_assets_064)
  .word word(unresolved_fighter_assets_065), word(unresolved_fighter_assets_066), word(unresolved_fighter_assets_067), word(unresolved_fighter_assets_068), word(unresolved_fighter_assets_057), word(unresolved_fighter_assets_058)
  .word word(unresolved_fighter_assets_059), word(unresolved_fighter_assets_060), word(unresolved_fighter_assets_061), word(unresolved_fighter_assets_062), word(unresolved_fighter_assets_069)
  .zero 4
  .word word(unresolved_fighter_assets_070), word(unresolved_fighter_assets_071), word(unresolved_fighter_assets_072), word(unresolved_fighter_assets_053), word(unresolved_fighter_assets_054), word(unresolved_fighter_assets_055)
  .word word(unresolved_fighter_assets_056)
  .zero 4
  .word word(unresolved_body_assets_013), word(unresolved_body_assets_011), word(unresolved_body_assets_012), word(unresolved_body_assets_010), word(unresolved_body_assets_014), word(unresolved_body_assets_015)
  .word word(unresolved_body_assets_022), word(unresolved_body_assets_025), word(unresolved_body_assets_020), word(unresolved_body_assets_018), word(unresolved_body_assets_021), word(unresolved_body_assets_017)
  .word word(unresolved_body_assets_016), word(unresolved_body_assets_023), word(unresolved_body_assets_024), word(unresolved_body_assets_026), word(unresolved_body_assets_027), word(unresolved_body_assets_019)
  .word word(unresolved_reaction_parameters_001), word(unresolved_reaction_parameters_002), word(unresolved_reaction_parameters_003), word(unresolved_reaction_parameters_004), clip(306), clip(110)
  .word clip(109), clip(301), clip(302), clip(301), clip(4), clip(87)
  .word clip(89), clip(301), clip(102), clip(101), clip(301), clip(13)

animation_descriptor_air_getup_rise:
  .word clip(197), clip(197)

animation_descriptor_lk_victim_slide:
  .word clip(169)

animation_descriptor_victim_finish_flip_getup:
  .word clip(166)

animation_descriptor_victim_impact_roll_getup:
  .word clip(167)
  .zero 4

crouch_block_animation_descriptors:
  .word clip(255), clip(222), clip(200)

animation_descriptor_square_victim:
  .word clip(136), clip(8), clip(8)

animation_descriptor_bicycle_cycle:
  .word clip(136), clip(132)

animation_descriptor_scene_victim_hold:
  .word clip(540)
  .zero 4
  .word word(unresolved_fighter_assets_044)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_001:
  .zero 4
  .word word(unresolved_reactions_and_audio_020), word(unresolved_reactions_and_audio_021), word(unresolved_reactions_and_audio_022), word(unresolved_reactions_and_audio_023), word(unresolved_reactions_and_audio_024), word(unresolved_reactions_and_audio_025)
  .word word(unresolved_reactions_and_audio_011)

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_002:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_003:
  .zero 4
  .word word(unresolved_reactions_and_audio_044), word(unresolved_reactions_and_audio_043), word(unresolved_reactions_and_audio_042), word(unresolved_reactions_and_audio_041), word(unresolved_reactions_and_audio_040), word(unresolved_reactions_and_audio_039)
  .word word(unresolved_stance_and_queries_002)

roster_sound_selector_table:
  .word word(unresolved_reactions_and_audio_005), word(unresolved_stance_and_queries_003), word(unresolved_reactions_and_audio_006), word(unresolved_stance_and_queries_005), word(unresolved_reactions_and_audio_004), word(unresolved_reactions_and_audio_003)

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_004:
  .word word(unresolved_stance_and_queries_008), word(unresolved_stance_and_queries_007), word(unresolved_reactions_and_audio_010), word(unresolved_stance_and_queries_001), word(unresolved_reactions_and_audio_007), word(unresolved_reactions_and_audio_008)
  .word word(unresolved_reactions_and_audio_009)
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_005:
  .zero 4
  .word word(unresolved_reactions_and_audio_014), word(unresolved_reactions_and_audio_015), word(unresolved_reactions_and_audio_016), word(unresolved_reactions_and_audio_017), word(unresolved_reactions_and_audio_018), word(unresolved_reactions_and_audio_019)
  .word word(unresolved_reactions_and_audio_012)

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_006:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_007:
  .zero 4
  .word word(unresolved_reactions_and_audio_026), word(unresolved_reactions_and_audio_027), word(unresolved_reactions_and_audio_028), word(unresolved_reactions_and_audio_029), word(unresolved_reactions_and_audio_030), word(unresolved_reactions_and_audio_031)
  .word word(unresolved_stance_and_queries_006)

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_008:
  .zero 4
  .word word(unresolved_reactions_and_audio_032), word(unresolved_reactions_and_audio_033), word(unresolved_reactions_and_audio_034), word(unresolved_reactions_and_audio_035), word(unresolved_reactions_and_audio_036), word(unresolved_reactions_and_audio_037)
  .word word(unresolved_reactions_and_audio_038)

sound_draw_voice_pointer:
  .word word(unresolved_reactions_and_audio_045)

sound_impact_voice_pointer:
  .word word(unresolved_reactions_and_audio_013), word(attack_records_begin), word(unresolved_reactions_and_audio_046), word(unresolved_reactions_and_audio_047), word(unresolved_reactions_and_audio_046), word(unresolved_reactions_and_audio_047)
  .zero 4

; Packed attack-record references, indexed by query selector; game/script/vm.c and game/native/contact.c.
weapon_attack_table:
  .word word(contact_weapon_00_record), word(contact_weapon_00_record), word(contact_weapon_02_record), word(contact_weapon_03_record), word(contact_weapon_04_record), word(contact_weapon_05_record)
  .word word(contact_weapon_06_record), word(contact_weapon_07_record), word(contact_weapon_08_record), word(contact_weapon_09_record), word(contact_weapon_10_record), word(contact_weapon_11_record)
  .word word(contact_weapon_12_record), word(contact_weapon_13_record), word(contact_weapon_14_record), word(contact_weapon_15_record), word(contact_weapon_16_record), word(contact_weapon_17_record)
  .word word(contact_weapon_18_record), word(sweep_attack_record), word(flurry_attack_record), word(contact_weapon_21_record), word(contact_weapon_22_record), word(contact_weapon_23_record)
  .word word(contact_weapon_24_record), word(contact_weapon_25_record), word(contact_weapon_26_record), word(axe_followup_attack_record), word(contact_weapon_28_record), word(contact_weapon_29_record)
  .word word(contact_weapon_30_record), word(contact_weapon_31_record), word(contact_weapon_32_record), word(contact_weapon_33_record), word(contact_weapon_34_record), word(contact_weapon_35_record)
  .word word(contact_weapon_36_record), word(contact_weapon_37_record), word(contact_weapon_38_record), word(contact_weapon_39_record), word(contact_weapon_40_record), word(contact_weapon_41_record)
  .word word(contact_weapon_42_record), word(contact_weapon_43_record), word(contact_weapon_42_record)
  .zero 4

; Packed attack-record references, indexed by query selector; game/script/vm.c and game/native/contact.c.
contact_query_d_table:
  .word word(torpedo_attack_record), word(aerial_bicycle_attack_record), word(square_attack_record), word(leg_attack_record), word(teleport_attack_record), word(ice_clone_attack_record)
  .word word(bicycle_attack_record), word(slide_attack_record), word(contact_d_08_record), word(triple_attack_record), word(stomp_attack_record), word(weapon_steal_attack_record)
  .word word(flying_attack_record), word(handstand_spin_attack_record), word(teleport_slam_attack_record), word(dive_attack_record), word(aerial_attack_record), word(drill_attack_record)
  .word word(cartwheel_attack_record), word(ball_attack_record), word(contact_d_20_record), word(roll_attack_record), word(dash_attack_record_648), word(dash_attack_record)
  .word word(flip_attack_record), word(shadow_attack_record), word(power_attack_record), word(groin_punch_attack_record), word(spin_attack_record), word(flip_kick_attack_record)
  .word word(fire_attack_record), word(contact_d_31_record), word(punch_attack_record), word(kick_attack_record), word(stomp_attack_record_666), word(uppercut_attack_record)

; Packed attack-record references, indexed by query selector; game/script/vm.c and game/native/contact.c.
projectile_contact_table:
  .word word(orb_attack_record), word(fireball_low_attack_record), word(generic_projectile_attack_record), word(ice_projectile_attack_record), word(blade_attack_record), word(contact_a_09_record)
  .word word(spear_projectile), word(skull_attack_record), word(drop_attack_record), word(rise_attack_record), word(contact_a_09_record), word(contact_a_09_record)
  .word word(contact_a_09_record), word(contact_a_09_record), word(contact_a_09_record), word(contact_a_09_record), word(bolt_attack_record), word(acid_attack_record)
  .word word(weapon_throw_attack_record), word(contact_projectile_19_record), word(boomerang_attack_record), word(star_attack_record), word(fireball_attack_record)
  .zero 4

; Packed attack-record references, indexed by query selector; game/script/vm.c and game/native/contact.c.
contact_query_a_table:
  .word word(contact_a_00_record)

jab_contact:
  .word word(contact_a_01_record), word(contact_a_02_record), word(contact_a_03_record), word(contact_a_04_record), word(contact_a_05_record), word(contact_a_06_record)
  .word word(weapon_air_hp_attack_record), word(weapon_air_lp_attack_record), word(contact_a_09_record), word(contact_a_09_record), word(contact_a_09_record), word(contact_a_09_record)
  .word word(contact_a_09_record), word(contact_a_09_record), word(contact_a_09_record), word(contact_a_16_record), word(contact_a_17_record), word(contact_a_18_record)
  .word word(contact_a_19_record)

; Packed attack-record references, indexed by query selector; game/script/vm.c and game/native/contact.c.
contact_query_b_table:
  .word word(contact_b_00_record), word(contact_b_01_record), word(contact_b_02_record)
  .zero 8
  .word word(contact_b_05_record), word(contact_b_06_record), word(contact_b_07_record)
  .zero 4
  .word word(lunge_attack_record), word(handstand_front_attack_record), word(handstand_rear_attack_record)
  .zero 20
  .word word(contact_b_17_record)
  .zero 20
  .word word(contact_b_23_record), word(contact_b_24_record), word(contact_b_25_record), word(contact_b_26_record), word(contact_b_27_record), word(contact_b_28_record)
  .word word(contact_b_29_record), word(contact_b_30_record), word(contact_b_31_record), word(contact_b_32_record), word(contact_b_33_record), word(contact_b_34_record)
  .word word(contact_b_35_record), word(contact_b_36_record)
  .zero 4
  .word word(contact_b_38_record), word(contact_b_39_record), word(contact_b_40_record)
  .zero 4
  .word word(unresolved_fighter_assets_081), word(unresolved_fighter_assets_085), word(unresolved_fighter_assets_042), word(unresolved_fighter_assets_084)
  .zero 24

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
walk_animation_table:
  .word clip(269)  ; row 0

walk_animation_table_row_01:
  .word clip(267)  ; row 1

walk_animation_table_row_02:
  .word clip(267)  ; row 2

walk_animation_table_row_03:
  .word clip(266)  ; row 3

walk_animation_table_row_04:
  .word clip(267)  ; row 4

walk_animation_table_row_05:
  .word clip(267)  ; row 5

walk_animation_table_row_06:
  .word clip(267)  ; row 6

walk_animation_table_row_07:
  .word clip(267)  ; row 7

walk_animation_table_row_08:
  .word clip(267)  ; row 8

walk_animation_table_row_09:
  .word clip(267)  ; row 9

walk_animation_table_row_10:
  .word clip(268)  ; row 10

walk_animation_table_row_11:
  .word clip(265)  ; row 11

walk_animation_table_row_12:
  .word clip(267)  ; row 12

walk_animation_table_row_13:
  .word clip(265)  ; row 13

walk_animation_table_row_14:
  .word clip(267)  ; row 14

walk_animation_table_row_15:
  .word clip(590)  ; row 15
  .word clip(274), clip(272), clip(272), clip(271), clip(272), clip(272)
  .word clip(272), clip(272), clip(272), clip(272), clip(273), clip(270)
  .word clip(272), clip(270), clip(272), clip(589)

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
idle_animation_table:
  .word clip(290)  ; row 0

idle_animation_table_row_01:
  .word clip(291)  ; row 1

idle_animation_table_row_02:
  .word clip(439)  ; row 2

idle_animation_table_row_03:
  .word clip(287)  ; row 3

idle_animation_table_row_04:
  .word clip(292)  ; row 4

idle_animation_table_row_05:
  .word clip(291)  ; row 5

idle_animation_table_row_06:
  .word clip(283)  ; row 6

idle_animation_table_row_07:
  .word clip(284)  ; row 7

idle_animation_table_row_08:
  .word clip(283)  ; row 8

idle_animation_table_row_09:
  .word clip(439)  ; row 9

idle_animation_table_row_10:
  .word clip(289)  ; row 10

idle_animation_table_row_11:
  .word clip(288)  ; row 11

idle_animation_table_row_12:
  .word clip(284)  ; row 12

idle_animation_table_row_13:
  .word clip(284)  ; row 13

idle_animation_table_row_14:
  .word clip(291)  ; row 14

idle_animation_table_row_15:
  .word clip(587)  ; row 15
  .word word(unresolved_scene_assets_240), word(unresolved_scene_assets_240), word(unresolved_scene_assets_242), word(unresolved_scene_assets_242), word(unresolved_scene_assets_243), word(unresolved_scene_assets_243)
  .word word(unresolved_scene_assets_244), word(unresolved_scene_assets_244), word(unresolved_scene_assets_245), word(unresolved_scene_assets_245), word(unresolved_scene_assets_246), word(unresolved_scene_assets_246)
  .word word(unresolved_scene_assets_247), word(unresolved_scene_assets_247), word(unresolved_scene_assets_254), word(unresolved_scene_assets_254), word(unresolved_scene_assets_248), word(unresolved_scene_assets_248)
  .word word(unresolved_scene_assets_249), word(unresolved_scene_assets_249), word(unresolved_scene_assets_250), word(unresolved_scene_assets_250), word(unresolved_scene_assets_251), word(unresolved_scene_assets_251)
  .word word(unresolved_scene_assets_252), word(unresolved_scene_assets_252), word(unresolved_scene_assets_241), word(unresolved_scene_assets_241), word(unresolved_scene_assets_253), word(unresolved_scene_assets_253)
  .word word(unresolved_scene_assets_357), word(unresolved_scene_assets_358), word(unresolved_scene_assets_359), word(unresolved_scene_assets_360), word(unresolved_scene_assets_361), word(unresolved_scene_assets_362)
  .word word(unresolved_scene_assets_363), word(unresolved_scene_assets_364), word(unresolved_scene_assets_365), word(unresolved_scene_assets_366), word(unresolved_scene_assets_367), word(unresolved_scene_assets_368)
  .word word(unresolved_scene_assets_369), word(unresolved_scene_assets_370)
  .zero 8
  .word word(unresolved_scene_assets_388), word(unresolved_scene_assets_389), word(unresolved_scene_assets_390), word(unresolved_scene_assets_391), word(unresolved_scene_assets_392), word(unresolved_scene_assets_393)
  .word word(unresolved_scene_assets_394), word(unresolved_scene_assets_395), word(unresolved_scene_assets_396), word(unresolved_scene_assets_397), word(unresolved_scene_assets_398), word(unresolved_scene_assets_399)
  .word word(unresolved_scene_assets_400), word(unresolved_scene_assets_401), word(unresolved_scene_assets_402), word(unresolved_scene_assets_403), word(unresolved_scene_assets_404), word(unresolved_scene_assets_405)
  .word word(unresolved_scene_assets_406), word(unresolved_scene_assets_407)
  .zero 8
  .word word(unresolved_scene_assets_375), word(unresolved_scene_assets_371), word(unresolved_scene_assets_372), word(unresolved_scene_assets_373), word(unresolved_scene_assets_374), word(unresolved_scene_assets_376)
  .word word(unresolved_scene_assets_377)
  .zero 4
  .word clip(249), clip(233), word(unresolved_weapons_005), word(unresolved_weapons_004), word(unresolved_weapons_003), word(unresolved_weapons_005)
  .word word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_003)
  .word word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_002), word(unresolved_weapons_005), word(unresolved_weapons_005), word(unresolved_weapons_005)

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
weapon_inventory_table:
  .word word(weapon_descriptor_sword)  ; row 0

weapon_inventory_table_row_01:
  .word word(weapon_descriptor_raiden_staff)  ; row 1

weapon_inventory_table_row_02:
  .word word(weapon_descriptor_sonya)  ; row 2

weapon_inventory_table_row_03:
  .word word(weapon_descriptor_dragon_sword)  ; row 3

weapon_inventory_table_row_04:
  .word word(weapon_descriptor_scepter)  ; row 4

weapon_inventory_table_row_05:
  .word word(weapon_descriptor_fujin)  ; row 5

weapon_inventory_table_row_06:
  .word word(weapon_descriptor_shinnok)  ; row 6

weapon_inventory_table_row_07:
  .word word(weapon_descriptor_axe)  ; row 7

weapon_inventory_table_row_08:
  .word word(weapon_descriptor_quan_chi)  ; row 8

weapon_inventory_table_row_09:
  .word word(weapon_descriptor_tanya)  ; row 9

weapon_inventory_table_row_10:
  .word word(weapon_descriptor_reptile)  ; row 10

weapon_inventory_table_row_11:
  .word word(weapon_descriptor_kai)  ; row 11

weapon_inventory_table_row_12:
  .word word(weapon_descriptor_jarek)  ; row 12

weapon_inventory_table_row_13:
  .word word(weapon_descriptor_axe)  ; row 13

weapon_inventory_table_row_14:
  .word word(weapon_descriptor_cage)  ; row 14

weapon_inventory_table_row_15:
  .word word(unresolved_weapons_020)  ; row 15

weapon_inventory_table_row_16:
  .word word(unresolved_weapons_024)  ; row 16

weapon_inventory_table_row_17:
  .word word(unresolved_weapons_025)  ; row 17
  .word word(unresolved_weapons_021), word(unresolved_weapons_021), word(weapon_descriptor_quan_chi), word(weapon_descriptor_sonya), word(weapon_descriptor_kai), word(weapon_descriptor_dragon_sword)
  .word word(weapon_descriptor_reptile), word(weapon_descriptor_axe), word(weapon_descriptor_sword), word(weapon_descriptor_dragon_sword), word(weapon_descriptor_cage), word(weapon_descriptor_scepter)
  .word word(weapon_descriptor_jarek), word(unresolved_weapons_022), word(weapon_descriptor_raiden_staff), word(weapon_descriptor_shinnok), word(weapon_descriptor_kai), word(unresolved_weapons_020)
  .word word(unresolved_weapons_024), word(unresolved_weapons_025)
  .zero 4
  .word word(unresolved_weapons_021), word(weapon_descriptor_jarek), word(weapon_descriptor_kai), word(weapon_descriptor_quan_chi), word(weapon_descriptor_dragon_sword), word(weapon_descriptor_dragon_sword)
  .word word(weapon_descriptor_shinnok), word(weapon_descriptor_tanya), word(weapon_descriptor_fujin), word(weapon_descriptor_fujin), word(weapon_descriptor_raiden_staff), word(weapon_descriptor_axe)
  .word word(weapon_descriptor_axe), word(weapon_descriptor_raiden_staff), word(weapon_descriptor_kai), word(unresolved_weapons_023), word(unresolved_weapons_020), word(unresolved_weapons_024)
  .word word(unresolved_weapons_025), word(unresolved_weapons_021), word(unresolved_weapons_021), clip(208)
  .zero 4
  .word word(unresolved_body_assets_005)
  .zero 12
  .word clip(214), clip(215), clip(212), clip(213), clip(211), clip(211)
  .zero 4
  .word word(unresolved_projectile_parameters_002), word(unresolved_projectile_parameters_003), word(unresolved_projectile_parameters_004), word(unresolved_projectile_parameters_005), word(unresolved_projectile_parameters_006), word(unresolved_projectile_parameters_007)
  .word word(unresolved_projectile_parameters_008), word(unresolved_projectile_parameters_009), word(unresolved_projectile_parameters_010), word(unresolved_projectile_parameters_011), word(unresolved_projectile_parameters_012), word(unresolved_projectile_parameters_013)
  .word word(unresolved_projectile_parameters_014), word(unresolved_projectile_parameters_015), word(unresolved_projectile_parameters_016)

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_009:
  .word word(unresolved_projectile_parameters_017), word(unresolved_projectile_parameters_018)
  .zero 207064

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_010:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_011:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_012:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_013:
  .zero 236

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_014:
  .zero 128

; Retained referenced block; semantic role is not established.
unresolved_stance_and_queries_015:
  .zero 65152
