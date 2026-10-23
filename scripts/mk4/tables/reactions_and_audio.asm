; Reaction native dispatch and sound-event selector lists.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


reaction_table:
  .word MK4_NATIVE_BLOCK_SINGLE_NO_PUSH, MK4_NATIVE_BLOCK_SINGLE_PUSH_0_24, MK4_NATIVE_BLOCK_PUSH_0_13_NO_WALL_RECOIL, MK4_NATIVE_BLOCK_STANDING_PUSH_0_06, MK4_NATIVE_BLOCK_PUSH_0_15, MK4_NATIVE_BLOCK_PUSH_0_18
  .word MK4_NATIVE_BLOCK_STANDING_PUSH_0_20, MK4_NATIVE_BLOCK_STANDING_PUSH_0_25_TRIPLE, MK4_NATIVE_BLOCK_SINGLE_NO_PUSH_NO_WALL_RECOIL, MK4_NATIVE_BLOCK_STANDING_PUSH_0_20_NO_WALL_RECOIL, MK4_NATIVE_DISARM_WALL_RECOIL_FALL, MK4_NATIVE_REACT_JAB_AIR
  .word MK4_NATIVE_LAUNCH_CLIP_131_BACK_GETUP, MK4_NATIVE_REACT_KICK_AIR, MK4_NATIVE_FALL_WITHOUT_DISARM, MK4_NATIVE_TUMBLE_CAMERA_SIDE_POSITIVE, MK4_NATIVE_REACT_KICK_HIT, MK4_NATIVE_STAGGER_STANCE_SELECT_DISARM
  .word MK4_NATIVE_WEAPON_SWEEP_GROUNDED_VICTIM, MK4_NATIVE_SWEEP_KNOCKDOWN_VICTIM, MK4_NATIVE_TUMBLE_CAMERA_SIDE_NEGATIVE, MK4_NATIVE_KNOCKDOWN_STAGGER_HALF_DAMAGE_ALIAS, MK4_NATIVE_FORWARD_FALL_CONTEXT_SOUND, MK4_NATIVE_IMPACT_WALL_COUNTER_60_PERCENT_DAMAGE
  .word MK4_NATIVE_SONYA_CROUCH_HIGH_KICK_VICTIM_KEEP_WEAPON, MK4_NATIVE_STAGGER_CLIP_311_KEEP_WEAPON, MK4_NATIVE_UPPERCUT_DELAYED_VOICE_40_PERCENT_DAMAGE, MK4_NATIVE_STAGGER_SHAKE_60_PERCENT_DAMAGE, MK4_NATIVE_STAGGER_CLIP_134, MK4_NATIVE_STAGGER_CLIP_134_IMPACT
  .word MK4_NATIVE_REAR_HIT_TURN_AND_LAUNCH, MK4_NATIVE_REACTION_FALL, MK4_NATIVE_HIT_CLIP_324_WITH_BOSS_STAGGER, MK4_NATIVE_HIT_CLIP_325_WITH_DAMAGE_STAGGER, MK4_NATIVE_UPPERCUT_DELAYED_VOICE_40_PERCENT_DAMAGE, MK4_NATIVE_TASK_PARK_40_TICK_LOOP
  .word MK4_NATIVE_LAUNCH_CLIP_131_NO_IMPACT, MK4_NATIVE_REACT_JAB_HIT, MK4_NATIVE_KNOCKDOWN_STAGGER_HALF_DAMAGE, MK4_NATIVE_FORWARD_FALL_CONTEXT_SOUND_ALIAS, MK4_NATIVE_REPTILE_REACTION_40, MK4_NATIVE_HIT_STAGGER_ACTION_1013
  .word MK4_NATIVE_JAREK_BALL_VICTIM, MK4_NATIVE_KICK_STAGGER_60_PERCENT_DAMAGE, MK4_NATIVE_SONYA_WEAPON_LOW_PUNCH_VICTIM_CONTEXT_SOUND, MK4_NATIVE_REIKO_STAR_REACTION_45, MK4_NATIVE_REACTION_FALL_RETURN, MK4_NATIVE_BATTLE_REACT
  .word MK4_NATIVE_REACTION_CLIP_180_SLIDE_HOLD, MK4_NATIVE_LAUNCH_CLIP_131_NATIVE_FINISH_BACK_GETUP, MK4_NATIVE_REACTION_CLIP_132_NATIVE_FINISH, MK4_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY, MK4_NATIVE_UPPERCUT_40_PERCENT_DAMAGE, MK4_NATIVE_STAFF_HIGH_PUNCH_WEAPON_VICTIM
  .word MK4_NATIVE_KAI_WEAPON_HIGH_PUNCH_VICTIM_IMPACT_SOUND, MK4_NATIVE_SONYA_WEAPON_HIGH_PUNCH_VICTIM_POSITION, MK4_NATIVE_SONYA_WEAPON_LOW_PUNCH_VICTIM_IMPACT_SOUND, MK4_NATIVE_TUMBLE_NATIVE_FINISH_BACK_GETUP, MK4_NATIVE_WEAPON_CLIP_189_LAUNCH_BACK_GETUP, MK4_NATIVE_WEAPON_CLIP_189_LAUNCH_HALF_DAMAGE
  .word MK4_NATIVE_REACTION_CLIP_311_SHORT_SLIDE, MK4_NATIVE_GROUNDED_TUMBLE_HOLD_RETURN, MK4_NATIVE_JAREK_WEAPON_REACTION_ENTRY, MK4_NATIVE_WEAPON_LOW_PUNCH_VICTIM_ROLL_GETUP, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_REACTION_CLIP_127_FRICTION
  .word MK4_NATIVE_LIGHTNING_VICTIM_FRICTION, MK4_NATIVE_REACT_FREEZE, MK4_NATIVE_RING_PROJECTILE_VICTIM, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_SPEAR_HIT_VICTIM_60_PERCENT_DAMAGE, MK4_NATIVE_SNARE_REACTION
  .word MK4_NATIVE_SPEAR_WAIT, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_LIU_KANG_FIREBALL_REACTION_ENTRY, MK4_NATIVE_VICTIM_FINISH_CAPTURED_CLIP
  .word MK4_NATIVE_LK_VICTIM_CLIP_203_RECOVERY, MK4_NATIVE_SONYA_LEG_GRAB_VICTIM_RECOVERY, MK4_NATIVE_LIU_KANG_LK_THROW_VICTIM_REACTION_80, MK4_NATIVE_BICYCLE_RELEASE, MK4_NATIVE_TORPEDO_VICTIM_FACE_PEER, MK4_NATIVE_TORPEDO_VICTIM_RELEASE_SOUND_AND_SHAKE
  .word MK4_NATIVE_BICYCLE_CAPTURE, MK4_NATIVE_AIR_VICTIM_FLIP, MK4_NATIVE_BICYCLE_VICTIM_FRONT_GETUP, MK4_NATIVE_SQUARE_WAVE_VICTIM_FACE_PEER, MK4_NATIVE_PAIR_WAIT, MK4_NATIVE_AIR_VICTIM_DEFAULT
  .word MK4_NATIVE_TELEPORT_PUNCH_GROUND_OR_AIR_VICTIM, MK4_NATIVE_VICTIM_GROUND_TURN, MK4_NATIVE_FUJIN_WIND_VICTIM, MK4_NATIVE_FUJIN_TRIPLE_FIRST_VICTIM, MK4_NATIVE_FUJIN_TRIPLE_MIDDLE_VICTIM, MK4_NATIVE_CARTWHEEL_VICTIM_RECOIL
  .word MK4_NATIVE_REPTILE_WEAPON_REACTION_ENTRY, MK4_NATIVE_FUJIN_WIND_SLAM_VICTIM, MK4_NATIVE_FUJIN_GRAB_VICTIM, MK4_NATIVE_REACTION_CLIP_326_SHORT_SLIDE, MK4_NATIVE_LP_VICTIM_WEAPON_SCATTER_RELEASE, MK4_NATIVE_FUJIN_BOLT_VICTIM
  .word MK4_NATIVE_LP_VICTIM_TABLE_RELEASE, MK4_NATIVE_LP_VICTIM_TWO_SPEED_SLIDE_RELEASE, MK4_NATIVE_LP_VICTIM_IMPACT_RELEASE, MK4_NATIVE_SLAM_REACTION, MK4_NATIVE_QUAN_CHI_STOMP_REACTION, MK4_NATIVE_LP_VICTIM_SLIDING_IMPACT_RELEASE
  .word MK4_NATIVE_QUAN_CHI_STEAL_REACTION, MK4_NATIVE_LP_VICTIM_SLIDE_RELEASE, MK4_NATIVE_VICTIM_SAVED_MOTION, MK4_NATIVE_LK_VICTIM_EFFECT_SLIDE, MK4_NATIVE_BLOCK_WEAPON_SINGLE_NO_PUSH
  .zero 16
  .word MK4_NATIVE_BLOCK_WEAPON_PUSH_0_18, MK4_NATIVE_BLOCK_WEAPON_STANDING_PUSH_0_20, MK4_NATIVE_BLOCK_WEAPON_STANDING_PUSH_0_25_TRIPLE, MK4_NATIVE_REACT_WEAPON_FOLLOWUP, MK4_NATIVE_CARTWHEEL_VICTIM_SCALED_IMPACT, MK4_NATIVE_REPTILE_DASH_VICTIM_SCALED
  .word MK4_NATIVE_REACT_KICK_AIR_IMPACT, MK4_NATIVE_GUARD_FALL, MK4_NATIVE_LP_VICTIM_REBASED_TABLE_RELEASE, MK4_NATIVE_KAI_HANDSTAND_REACTION_ENTRY, MK4_NATIVE_HANDSTAND_VICTIM_KNOCKDOWN, MK4_NATIVE_QUAN_CHI_SKULL_REACTION
  .word MK4_NATIVE_HIT_CLIP_132_OR_AIR_FORWARD_FALL, MK4_NATIVE_LAUNCH_CLIP_179_ACTION_100A, MK4_NATIVE_VICTIM_FRICTION_IMPACT, MK4_NATIVE_VICTIM_PAUSE_SLIDE, MK4_NATIVE_VICTIM_IMPACT_ROLL_GETUP, MK4_NATIVE_VICTIM_FINISH_FLIP_GETUP
  .word MK4_NATIVE_VICTIM_SLIDE_TO_STANCE, MK4_NATIVE_GORO_QUAKE_VICTIM, MK4_NATIVE_REPTILE_WEAPON_VICTIM_KEEP_WEAPON, MK4_NATIVE_CARTWHEEL_VICTIM_IMPACT, MK4_NATIVE_WEAPON_LOW_PUNCH_VICTIM_ROLL_GETUP, MK4_NATIVE_SHINNOK_WEAPON_REACTION_ENTRY
  .word MK4_NATIVE_WEAPON_SWEEP_GROUNDED_VICTIM, MK4_NATIVE_KAI_WEAPON_HIGH_PUNCH_VICTIM_CONTEXT_SOUND, MK4_NATIVE_BLOCK_STANDING_PUSH_0_06_NO_WALL_RECOIL, MK4_NATIVE_CAGE_WEAPON_REACTION_ENTRY, MK4_NATIVE_SONYA_WEAPON_LOW_PUNCH_VICTIM_CONTEXT_SOUND, MK4_NATIVE_REPTILE_WEAPON_REACTION_ENTRY
  .word MK4_NATIVE_REACTION_FALL, MK4_NATIVE_JAREK_WEAPON_REACTION_ENTRY, MK4_NATIVE_CAGE_WEAPON_REACTION_ENTRY, MK4_NATIVE_SWEEP_KNOCKDOWN_VICTIM, MK4_NATIVE_REACT_SCEPTER_FREEZE, MK4_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY
  .word MK4_NATIVE_VICTIM_DISARM_FALL, MK4_NATIVE_FALL_REACTION_WITH_EFFECTS, MK4_NATIVE_WEAPON_CLIP_132_DIRECTIONAL_FALL, MK4_NATIVE_WEAPON_TUMBLE_BACK_GETUP, MK4_NATIVE_JAX_DASH_VICTIM_HIT, MK4_NATIVE_REPTILE_DASH_VICTIM_SCALED
  .word MK4_NATIVE_LAUNCH_CLIP_131_SHAKE_IMPACT, MK4_NATIVE_QUAN_CHI_FLIP_REACTION, MK4_NATIVE_TORPEDO_RETURN, MK4_NATIVE_CAGE_SHADOW_VICTIM, MK4_NATIVE_CAGE_POWER_VICTIM, MK4_NATIVE_CAGE_GROIN_PUNCH_VICTIM_REACTION
  .word MK4_NATIVE_BLOCK_STANDING_NO_PHYSICS, MK4_NATIVE_AIR_VICTIM_LIFT, MK4_NATIVE_HANDSTAND_VICTIM_REAR_STAGGER, MK4_NATIVE_JAX_DASH_VICTIM_THROW, MK4_NATIVE_TANYA_WEAPON_REACTION_ENTRY, MK4_NATIVE_TANYA_WEAPON_REACTION_REACTION_170_ENTRY
  .word MK4_NATIVE_UPPERCUT_DELAYED_VOICE_40_PERCENT_DAMAGE, MK4_NATIVE_SCENE_PARK, MK4_NATIVE_HIT_STAGGER_WITH_PARTICLES, MK4_NATIVE_VICTIM_DISARM_RAISE, MK4_NATIVE_STAGE_VICTIM, MK4_NATIVE_SCENE_VICTIM_HOLD
  .word MK4_NATIVE_SCENE_EXPLODE, MK4_NATIVE_KAI_SECOND_VICTIM, MK4_NATIVE_SHINNOK_SECOND_VICTIM, MK4_NATIVE_REPTILE_SECOND_VICTIM, MK4_NATIVE_FUJIN_SECOND_VICTIM, MK4_NATIVE_REACTION_PARK_TASK
  .word MK4_NATIVE_JAX_FATALITY_2_HEAD, MK4_NATIVE_SONYA_FATALITY_2_SCENE_VICTIM, MK4_NATIVE_CAGE_SECOND_VICTIM_HIT, MK4_NATIVE_TANYA_FATALITY_2_VICTIM_REACTION_186, MK4_NATIVE_RAIDEN_FATALITY_2_SCENE_VICTIM, MK4_NATIVE_GORO_KICK_VICTIM_DISARM
  .word MK4_NATIVE_GORO_SPECIAL_PUNCH_189_ENTRY, MK4_NATIVE_GORO_KICK_VICTIM, MK4_NATIVE_GORO_STOMP_VICTIM, MK4_NATIVE_GORO_SPECIAL_UPPERCUT_192_ENTRY
  .zero 4

reaction_table_end:
  .word 0x4876b0, 0x487860, 0x487530, 0x4874f0, 0x4874b0, 0x4878b0
  .zero 4
  .word 0x4871e0, MK4_NATIVE_PAIR_WAIT
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
