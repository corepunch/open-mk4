; Weapon attack playback and contact recovery.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: jarek.weapon_back_hp (upper); cage.weapon_upper (upper strike).
jarek_weapon_back_hp_upper:
  open_clip 0, clip(226)
  double_to 16
  sound_c 31
  double_to 26
  forward_to 29
  tail 0, MK4_NATIVE_CAGE_WEAPON_UPPER_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.draw_weapon (back strike).
kai_draw_weapon_back_strike:
  open_clip 0, clip(226)
  double_to 16
  sound_c 31
  double_to 26
  forward_to 29
  tail 0, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.weapon_back_hp (flurry startup).
reptile_weapon_back_hp_flurry_startup:
  open_clip 0, clip(248)
  forward_to 8
  tail 0, MK4_NATIVE_REPTILE_WEAPON_FLURRY
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.sword_hp (repeat program).
liu_kang_sword_hp_repeat_program:
  attack_selector 0, 0x30f
  schedule_motion 0x6006, 0x11eb
  open_clip 0, clip(233)
  double_to 9
  sound_c 29
  double_to 18

liu_kang_sword_hp_repeat_program_prefixed_sample_1:
  prefixed_sample WITH_CLIP + 0, clip(216)  ; bind clip prefix; inline sample/count 0
  weapon_query 15
  predicate_flags 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(liu_kang_sword_hp_repeat_program_action_recovery_1)
  schedule_motion 0x6006, 0x11eb
  sound_c 31
  forward_to 10
  weapon_query 15
  predicate_flags 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(shared_action_recovery_entry)
  schedule_motion 0x6006, 0x11eb
  sound_c 29
  forward_to 21
  jump 0, word(liu_kang_sword_hp_repeat_program_prefixed_sample_1)

liu_kang_sword_hp_repeat_program_action_recovery_1:
  action_recovery 0
  forward_count 5
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.weapon_back_hp (attacker); kitana.weapon_back_hp (attacker).
sonya_weapon_back_hp_attacker:
  open_clip 0, clip(228)
  forward_to 6
  sound_c 30
  forward_to 14
  tail 0, MK4_NATIVE_LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT
  .zero 4  ; non-executable guard between independent streams


sword_back_high_punch_hit_recovery:
  forward_to 24
  playback_pair 15
  forward_to 24
  playback_pair 15
  forward 0
  ready 0

sword_back_high_punch_block_recovery:
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: sonya.weapon_back_lp (attacker); liu_kang.sword_back_lp (attacker); cage.weapon_back_lp (back LP) (also shared by other catalog entries).
sword_attack:
  open_clip 0, clip(227)
  double_to 16
  sound_c 31
  double_to 26
  forward_to 29
  tail 0, MK4_NATIVE_CAGE_WEAPON_BACK_LP_CONTACT
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.sword_back_lp (attacker); shinnok.weapon_followup (follow-up HP).
sword_back_lp_script:
  play_clip 9, clip(237)
  sound_c 34
  speed 0, 0x1999
  forward_count 8
  weapon_query 2
  contact 0, word(sword_back_hp_script_action_recovery_1)
  forward_count 1
  weapon_query 2
  contact 0, word(sword_back_hp_script_action_recovery_1)
  forward_count 1
  weapon_query 2
  contact 0, word(sword_back_hp_script_action_recovery_1)
  landing 0
  forward_count 2
  wait 4
  action_recovery 0
  landing 0
  forward 0
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.sword_back_hp (attacker).
sword_back_hp_script:
  play_clip 8, clip(238)
  sound_c 34
  forward_count 8
  speed 0, 0x1999
  forward_to 25
  weapon_query 3
  contact 0, word(sword_back_hp_script_action_recovery_1)
  forward_count 1
  weapon_query 3
  contact 0, word(sword_back_hp_script_action_recovery_1)
  forward_count 1
  weapon_query 3
  contact 0, word(sword_back_hp_script_action_recovery_1)
  action_recovery 0
  landing 0
  forward_count 2
  sound_c 19
  wait 4

sword_back_hp_script_action_recovery_1:
  action_recovery 0
  landing 0
  forward 0
  tail 0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4  ; non-executable guard between independent streams


shared_action_recovery_entry:
  action_recovery 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard
