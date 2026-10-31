; Reaction native dispatch and sound-event selector lists.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


reaction_table:
  .word native_block_single_no_push, native_block_single_push_0_24, native_block_push_0_13_no_wall_recoil, native_block_standing_push_0_06, native_block_push_0_15, native_block_push_0_18
  .word native_block_standing_push_0_20, native_block_standing_push_0_25_triple, native_block_single_no_push_no_wall_recoil, native_block_standing_push_0_20_no_wall_recoil, native_disarm_wall_recoil_fall, native_react_jab_air
  .word native_launch_clip_131_back_getup, native_react_kick_air, native_fall_without_disarm, native_tumble_camera_side_positive, native_react_kick_hit, native_stagger_stance_select_disarm
  .word native_weapon_sweep_grounded_victim, native_sweep_knockdown_victim, native_tumble_camera_side_negative, native_knockdown_stagger_half_damage_alias, native_forward_fall_context_sound, native_impact_wall_counter_60_percent_damage
  .word native_sonya_crouch_high_kick_victim_keep_weapon, native_stagger_clip_311_keep_weapon, native_uppercut_delayed_voice_40_percent_damage, native_stagger_shake_60_percent_damage, native_stagger_clip_134, native_stagger_clip_134_impact
  .word native_rear_hit_turn_and_launch, native_reaction_fall, native_hit_clip_324_with_boss_stagger, native_hit_clip_325_with_damage_stagger, native_uppercut_delayed_voice_40_percent_damage, native_task_park_40_tick_loop
  .word native_launch_clip_131_no_impact, native_react_jab_hit, native_knockdown_stagger_half_damage, native_forward_fall_context_sound_alias, native_reptile_reaction_40, native_hit_stagger_action_1013
  .word native_jarek_ball_victim, native_kick_stagger_60_percent_damage, native_sonya_weapon_low_punch_victim_context_sound, native_reiko_star_reaction_45, native_reaction_fall_return, native_battle_react
  .word native_reaction_clip_180_slide_hold, native_launch_clip_131_native_finish_back_getup, native_reaction_clip_132_native_finish, native_goro_special_punch_189_entry, native_uppercut_40_percent_damage, native_staff_high_punch_weapon_victim
  .word native_kai_weapon_high_punch_victim_impact_sound, native_sonya_weapon_high_punch_victim_position, native_sonya_weapon_low_punch_victim_impact_sound, native_tumble_native_finish_back_getup, native_weapon_clip_189_launch_back_getup, native_weapon_clip_189_launch_half_damage
  .word native_reaction_clip_311_short_slide, native_grounded_tumble_hold_return, native_jarek_weapon_reaction_entry, native_weapon_low_punch_victim_roll_getup, native_liu_kang_fireball_reaction_entry, native_reaction_clip_127_friction
  .word native_lightning_victim_friction, native_react_freeze, native_ring_projectile_victim, native_liu_kang_fireball_reaction_entry, native_spear_hit_victim_60_percent_damage, native_snare_reaction
  .word native_spear_wait, native_liu_kang_fireball_reaction_entry, native_liu_kang_fireball_reaction_entry, native_liu_kang_fireball_reaction_entry, native_liu_kang_fireball_reaction_entry, native_victim_finish_captured_clip
  .word native_lk_victim_clip_203_recovery, native_sonya_leg_grab_victim_recovery, native_liu_kang_lk_throw_victim_reaction_80, native_bicycle_release, native_torpedo_victim_face_peer, native_torpedo_victim_release_sound_and_shake
  .word native_bicycle_capture, native_air_victim_flip, native_bicycle_victim_front_getup, native_square_wave_victim_face_peer, native_pair_wait, native_air_victim_default
  .word native_teleport_punch_ground_or_air_victim, native_victim_ground_turn, native_fujin_wind_victim, native_fujin_triple_first_victim, native_fujin_triple_middle_victim, native_cartwheel_victim_recoil
  .word native_reptile_weapon_reaction_entry, native_fujin_wind_slam_victim, native_fujin_grab_victim, native_reaction_clip_326_short_slide, native_lp_victim_weapon_scatter_release, native_fujin_bolt_victim
  .word native_lp_victim_table_release, native_lp_victim_two_speed_slide_release, native_lp_victim_impact_release, native_slam_reaction, native_quan_chi_stomp_reaction, native_lp_victim_sliding_impact_release
  .word native_quan_chi_steal_reaction, native_lp_victim_slide_release, native_victim_saved_motion, native_lk_victim_effect_slide, native_block_weapon_single_no_push
  .zero 16
  .word native_block_weapon_push_0_18, native_block_weapon_standing_push_0_20, native_block_weapon_standing_push_0_25_triple, native_react_weapon_followup, native_cartwheel_victim_scaled_impact, native_reptile_dash_victim_scaled
  .word native_react_kick_air_impact, native_guard_fall, native_lp_victim_rebased_table_release, native_kai_handstand_reaction_entry, native_handstand_victim_knockdown, native_quan_chi_skull_reaction
  .word native_hit_clip_132_or_air_forward_fall, native_launch_clip_179_action_100a, native_victim_friction_impact, native_victim_pause_slide, native_victim_impact_roll_getup, native_victim_finish_flip_getup
  .word native_victim_slide_to_stance, native_goro_quake_victim, native_reptile_weapon_victim_keep_weapon, native_cartwheel_victim_impact, native_weapon_low_punch_victim_roll_getup, native_shinnok_weapon_reaction_entry
  .word native_weapon_sweep_grounded_victim, native_kai_weapon_high_punch_victim_context_sound, native_block_standing_push_0_06_no_wall_recoil, native_cage_weapon_reaction_entry, native_sonya_weapon_low_punch_victim_context_sound, native_reptile_weapon_reaction_entry
  .word native_reaction_fall, native_jarek_weapon_reaction_entry, native_cage_weapon_reaction_entry, native_sweep_knockdown_victim, native_react_scepter_freeze, native_goro_special_punch_189_entry
  .word native_victim_disarm_fall, native_fall_reaction_with_effects, native_weapon_clip_132_directional_fall, native_weapon_tumble_back_getup, native_jax_dash_victim_hit, native_reptile_dash_victim_scaled
  .word native_launch_clip_131_shake_impact, native_quan_chi_flip_reaction, native_torpedo_return, native_cage_shadow_victim, native_cage_power_victim, native_cage_groin_punch_victim_reaction
  .word native_block_standing_no_physics, native_air_victim_lift, native_handstand_victim_rear_stagger, native_jax_dash_victim_throw, native_tanya_weapon_reaction_entry, native_tanya_weapon_reaction_reaction_170_entry
  .word native_uppercut_delayed_voice_40_percent_damage, native_scene_park, native_hit_stagger_with_particles, native_victim_disarm_raise, native_stage_victim, native_scene_victim_hold
  .word native_scene_explode, native_kai_second_victim, native_shinnok_second_victim, native_reptile_second_victim, native_fujin_second_victim, native_reaction_park_task
  .word native_jax_fatality_2_head, native_sonya_fatality_2_scene_victim, native_cage_second_victim_hit, native_tanya_fatality_2_victim_reaction_186, native_raiden_fatality_2_scene_victim, native_goro_kick_victim_disarm
  .word native_goro_special_punch_189_entry, native_goro_kick_victim, native_goro_stomp_victim, native_goro_special_uppercut_192_entry
  .zero 4

reaction_table_end:
  .word 0x4876b0, 0x487860, 0x487530, 0x4874f0, 0x4874b0, 0x4878b0
  .zero 4
  .word 0x4871e0, native_pair_wait
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_001:
  .word 0x489850, 0x489990, 0x4899d0, 0x489b40

sound_throw_grunt_list:
  .word 0x5

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_002:
  .word 0x2f8, 0x2fd, 0x302, 0x307, 0x30c, 0x3
  .word 0x152c, 0x1531, 0x1536, 0x6, 0x1e5, 0x1e0
  .word 0x1ea, 0x177, 0x17c, 0x181
  .zero 4

sound_knockdown_voice_list:
  .word 0x3, 0x285, 0x28a, 0x28f

sound_list_weapon_impact_voice:
  .word 0x3, 0x1a9, 0x1ae, 0x1b3, 0x3, 0x13e7
  .word 0x13ec, 0x13f1

sound_impact_voice_list:
  .word 0x2, 0x311, 0x316
  .zero 4

sound_footstep_list:
  .word 0x4, 0x1b8, 0x1bd, 0x1c2, 0xcd
  .zero 4

sound_unarmed_impact_list:
  .word 0x4, 0x208, 0x24e, 0x253, 0x258
  .zero 4

sound_armed_impact_list:
  .word 0x4, 0x23a, 0x23f, 0x244, 0x249
  .zero 4

sound_random_grunt_list:
  .word 0x3, 0x78, 0x7d, 0x82, 0x2, 0x1f0000bf
  .word 0x1e0000d7
  .zero 4

sound_list_block_low_variant_impacts:
  .word 0x2, 0xf5, 0x8c
  .zero 4

sound_list_block_high_variant_impacts:
  .word 0x2, 0x7d, 0x82
  .zero 4

sound_list_stagger_reaction:
  .word 0x8, 0xfa, 0xff, 0x104, 0x109, 0x10e
  .word 0x113, 0x118, 0x11d
  .zero 4

sound_uppercut_voice_list:
  .word 0xd, 0xbf4, 0xbf9, 0xbfe, 0xcb7, 0xcbc
  .word 0xcc1, 0xcc6, 0xccb, 0xcd0, 0xcd5, 0xcda
  .word 0xce4, 0xce9, 0x4, 0x281, 0x14dc, 0x14e1
  .word 0x14d7
  .zero 4

sound_a_table:
  .word 0xaf0064, 0xb413ab, 0x13c900be, 0x17930807, 0x17841397, 0xa5009b
  .word 0x7d0000, 0x780433, 0x3f70802, 0xc300be, 0xd70069, 0xe600b9
  .word 0x9100cd, 0x7e9080c, 0x80700eb, 0x910834, 0x1220073, 0x4b00136
  .word 0x7df00d2, 0x8c012c, 0x6e082a, 0x8430140, 0x1400000, 0x145
  .word 0x1360bf4
  .zero 4

sound_c_table:
  .word 0x1d101cc, 0x1fe0122, 0x138d012c, 0x1f90294, 0x1db020d, 0x15913c4
  .word 0x15e0163, 0x1c701d1, 0x16d0168, 0x19f0172, 0x18601a4, 0x25d00eb
  .word 0x2620203, 0x91012c, 0x1e500af, 0x1ea01e0, 0x17c0177, 0x13880181
  .word 0x2cb13d8, 0x2c602bc, 0x13b500eb, 0x13b030c, 0xc0d0154, 0x12c0320
  .word 0x14f014a, 0x13f60320, 0x9113fb, 0x1c20294, 0x1c713dd, 0x26200b9
  .word 0x13c90294, 0x139213ce, 0x13a11397, 0x13ba13a6, 0x13881437, 0x14500159
  .word 0x144b1446, 0x1432142d, 0x13d81432, 0x144114b4, 0x151314b9, 0x151d1518
  .word 0x15091504, 0x14551428, 0x150e00a5, 0x15400aa, 0x154029e, 0x153b1527
  .word 0x15451540, 0x1b991b8f, 0x1572156d, 0x155e1577, 0x1b5813b0, 0x8d41b85
  .word 0x4601b5d, 0x9600d7

sound_context_table:
  .word 0xb900b4, 0xc80064, 0x6900eb, 0x1ae01a9, 0xeb01b3, 0x730087
  .word 0x21c00fa, 0x2850082, 0x28f028a, 0xd2

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_003:
  .word 0x54b054b, 0xe1a0889, 0x12571257
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_004:
  .word 0x5690433, 0x825082a, 0x11f31040
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_005:
  .word 0x500037a, 0xded080c, 0x11ee0ff5, 0x668

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_006:
  .word 0x519040b, 0xde307fd, 0x11d00fe6, 0x668

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_007:
  .word 0x56403cf, 0xe0b08a2, 0x1248102c
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_008:
  .word 0x3e303e3, 0xe1f08b1, 0x12521068, 0x62c

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_009:
  .word 0x57d0460, 0xe4c08d4, 0x124d0fff
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_010:
  .word 0x4fb044c, 0xdca0870, 0x124d0fc8
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_011:
  .word 0x61d0009, 0x6270622, 0x631062c, 0x63b0636, 0x6450640
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_012:
  .word 0x61d0009, 0x6270622, 0x631062c, 0x63b0636, 0x6450640
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_013:
  .word 0x8750398, 0x50a0000, 0xdfc039d, 0x87a0df7
  .zero 16

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_014:
  .word 0x3ed0004, 0x3f703f2, 0x3f7
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_015:
  .word 0x4d80005, 0x4f104dd, 0x4fb04f6
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_016:
  .word 0x7df0009, 0x7e907e4, 0x7f307ee, 0x7fd07f8, 0x8070802
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_017:
  .word 0xdac0003, 0xdb60db1

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_018:
  .word 0xfa50005, 0xfaf0faa, 0xfb90fb4
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_019:
  .word 0x11940005, 0x119e1199, 0x11a811a3
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_020:
  .word 0x3750006, 0x37f037a, 0x3890384, 0x38e

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_021:
  .word 0x56e0003, 0x5780573

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_022:
  .word 0x7e40006, 0x7ee07e9, 0x7f807f3, 0x7fd

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_023:
  .word 0xdac0003, 0xdb60db1

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_024:
  .word 0xfa50005, 0xfaf0faa, 0xfb90fb4
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_025:
  .word 0x11940005, 0x119e1199, 0x11a811a3
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_026:
  .word 0x3c00002, 0x3c5

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_027:
  .word 0x5280002, 0x52d

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_028:
  .word 0x87f0002, 0x884

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_029:
  .word 0x87f0002, 0x884

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_030:
  .word 0x10590002, 0x1063105e

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_031:
  .word 0x11580002, 0x11fd

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_032:
  .word 0x3980003, 0x3a2039d

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_033:
  .word 0x50a0003, 0x514050f

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_034:
  .word 0x8750002, 0x87a

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_035:
  .word 0xdf70002, 0xdfc

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_036:
  .word 0xfd70003, 0xfe10fdc

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_037:
  .word 0x11da0003, 0x11e411df

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_038:
  .word 0x61d0009, 0x6270622, 0x631062c, 0x63b0636, 0x6450640
  .zero 4

sound_roster_remap:
  .word 0x1040200, 0x30003, 0x30503, 0x6000302, 0x4
  .zero 4

sound_indexed_table:
  .word 0x4d304b0, 0x4c904ba, 0x4470852, 0xe3803de, 0x7d503fc, 0x80705dc
  .word 0x83407ee, 0x10090fa0, 0x1013100e, 0x1018101d, 0xe060848, 0x3cf0e0b
  .word 0x3d90e10, 0xe3d1234, 0x12431239, 0x12200438, 0x82f102c, 0xdbb08a7
  .word 0x3660c5d, 0xd3e0c21, 0xc3a0c4e, 0xd160c30, 0xd6b0c44, 0xd250c53
  .word 0xd200c26, 0xd5c0d48, 0x123e0d52, 0xe42045b, 0x55a0370, 0x8980893
  .word 0xe1f08ac, 0x15040e47, 0x1509
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_039:
  .word 0x11ad0006, 0x11a711b2, 0x11c111bc, 0x11c6

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_040:
  .word 0xfbe0005, 0xfc80fc3, 0xfd20fcd
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_041:
  .word 0xdbb0005, 0xdc50dc0, 0xdcf0dca
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_042:
  .word 0x8570006, 0x861085c, 0x86b0866, 0x870

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_043:
  .word 0x4e20006, 0x4ec04e7, 0x4f604f1, 0x4fb

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_044:
  .word 0x3930003, 0x3930393

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_045:
  .word 0x89d03ca, 0x5501027, 0xe01088e, 0x3de
  .zero 16

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_046:
  .word 0xf0000, 0xa0000, 0xfff60000, 0xfffd0000, 0xa0000, 0xa0000
  .word 0xfffc8000, 0xfff60000, 0xa0000, 0xfffc0000, 0xfff60000, 0xfff60000
  .word 0xfffc8000, 0xa0000, 0xfff60000, 0xfffd0000, 0xa0000, 0xa0000
  .word 0xfffd8000, 0xfff60000, 0xa0000, 0xfffe0000, 0xfff60000, 0xfff60000
  .word 0xfffe8000, 0xa0000, 0xfff60000, 0xfffe0000, 0xa0000, 0xa0000
  .word 0xfffd8000, 0xfff60000, 0xa0000, 0xfffd0000, 0xfff60000, 0xfff60000
  .word 0xfffc8000, 0xa0000, 0xfff60000, 0xfffc0000, 0xa0000, 0xa0000
  .word 0xfffc8000, 0xfff60000, 0xa0000, 0xfffd0000, 0xfff60000, 0xfff60000
  .word 0xfffd8000, 0xa0000, 0xfff60000, 0xfffe0000, 0xa0000, 0xa0000
  .word 0xfffe8000, 0xfff60000, 0xa0000, 0xffff0000

; Retained referenced block; semantic role is not established.
unresolved_reactions_and_audio_047:
  .word 0xf0000, 0xa0000, 0xfff60000, 0xfffd8000, 0xfff40000, 0xfff40000
  .word 0xfffd0000, 0xfff30000, 0xd0000, 0xfffc8000, 0xc0000, 0xc0000
  .word 0xfffd0000, 0x90000, 0xfff70000, 0xfffd8000, 0xfff20000, 0xfff20000
  .word 0xfffe0000, 0xffef0000, 0x110000, 0xfffe8000, 0xe0000, 0xe0000
  .word 0xfffe0000, 0xd0000, 0xfff30000, 0xfffd8000, 0xfff40000, 0xfff40000
  .word 0xfffd0000, 0xfff50000, 0xb0000, 0xfffc8000, 0xa0000, 0xa0000
  .word 0xfffc0000, 0xc0000, 0xfff40000, 0xfffb8000, 0xfff30000, 0xfff30000
  .word 0xfffb0000, 0xfff40000, 0xc0000, 0xfffb8000, 0xa0000, 0xa0000
  .word 0xfffc0000, 0xb0000, 0xfff50000, 0xfffc8000, 0xfff40000, 0xfff40000
  .word 0xfffd0000, 0xfff40000, 0xfff40000, 0xfffd0000
