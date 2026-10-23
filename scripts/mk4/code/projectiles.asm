; Projectile casting streams; native services own projectile lifetimes.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: reiko.star (throw); noob_saibot.star (throw).
reiko_star:
  sound_c 100
  prefixed_to WITH_CLIP + 13, clip(437)  ; bind clip prefix; inline sample/count 13
  spawn 0, 0x52, MK4_NATIVE_REIKO_STAR_SPAWN
  forward_to 18
  playback_pair 16
  forward_to 18
  playback_pair 16
  forward_to 18
  playback_pair 16
  forward_to 18
  playback_pair 16
  forward_to 18
  playback_pair 16
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jax.missile (cast).
jax_missile:
  prefixed_to WITH_CLIP + 10, clip(29)  ; bind clip prefix; inline sample/count 10
  sound_c 93
  spawn 0, 0x50, MK4_NATIVE_JAX_MISSILE_SPAWN
  forward_to 25
  half_finish 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.high_orb (caster); cage.low_orb (caster).
cast_orb:
  sound_c 95
  open_clip 0, clip(28)
  double_to 8
  forward_to 20
  spawn 0, 0x4f, MK4_NATIVE_CAGE_ORB_SPAWN
  forward 0
  action_followup 0
  wait 22
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: jarek.blade (throw).
jarek_blade:
  sound_c 103
  fast_start 0, word(shared_open_clip_entry)
  prefixed_to WITH_CLIP + 17, clip(27)  ; bind clip prefix; inline sample/count 17

jarek_blade_spawn_1:
  spawn 0, 0x4e, MK4_NATIVE_JAREK_BLADE_SPAWN
  forward_to 17
  playback_pair 15
  forward_to 17
  playback_pair 15
  forward_to 17
  playback_pair 15
  forward_to 17
  playback_pair 15
  forward_to 17
  playback_pair 15
  forward_to 17
  playback_pair 15
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.weapon_lp (boomerang throw).
boomerang_start:
  sound_c 33
  play_clip 7, clip(229)
  tail 0, MK4_NATIVE_TANYA_BOOMERANG_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: reptile.acid_spit (selected stream).
reptile_acid_spit:
  play_clip 11, clip(25)
  call 0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
  forward_count 5
  call 0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_WIDE
  forward_count 5
  spawn 0, 0x4c, MK4_NATIVE_REPTILE_ACID_SPAWN
  call 0, MK4_NATIVE_REPTILE_SCRIPT_ACID_DROOL
  forward_count 8
  call 0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
  action_followup 0
  forward_to 30
  playback_pair 27
  forward_to 30
  playback_pair 27
  forward_to 30
  playback_pair 27
  call 0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.fireball (caster); kitana.shadow_low (shadow low playback); noob_saibot.shadow_low (shadow low playback).
tanya_fireball_caster:
  prefixed_finish WITH_CLIP + 0, clip(26)  ; bind clip prefix; inline sample/count 0
  sound_a 33
  block_sound 0
  spawn 0, 0x4b, MK4_NATIVE_FIREBALL_SPAWN
  playback_pair 11
  forward_to 13
  action_followup 0
  playback_pair 11
  forward_to 13
  playback_pair 11
  forward_to 13
  playback_pair 11
  forward_to 13
  playback_pair 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: tanya.air_fireball (caster); kitana.shadow_up (shadow up playback); noob_saibot.shadow_up (shadow up playback).
air_fireball:
  sound_a 33
  block_sound_alt 0
  prefixed_to WITH_CLIP + 9, clip(24)  ; bind clip prefix; inline sample/count 9
  spawn 0, 0x4b, MK4_NATIVE_AIR_FIREBALL_SPAWN
  backward_count 5
  tail 0, MK4_NATIVE_FIREBALL_FALL
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.ground_skull (cast hop).
kai_ground_skull:
  prefixed_finish WITH_CLIP + 0, clip(22)  ; bind clip prefix; inline sample/count 0
  sound_c 51
  motion 0, 0x0, 0xffffcccd, 0x28f
  tail 0, MK4_NATIVE_KAI_GROUND_SKULL_SPAWN
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.rising_skull (cast).
kai_rising_skull:
  prefixed_to WITH_CLIP + 7, clip(21)  ; bind clip prefix; inline sample/count 7
  sound_c 51
  tail 0, MK4_NATIVE_KAI_RISING_SKULL_SPAWN
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.rising_skull (cast loop).
kai_rising_skull_spawn:
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  playback_pair 4
  forward_to 6
  tail 0, MK4_NATIVE_KAI_RISING_SKULL_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.skull (caster).
quan_chi_skull:
  open_clip 0, clip(23)
  fast_start 0, word(quan_chi_skull_release_cursor_set_1)
  prefixed_to 17
  tail 0, MK4_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: quan_chi.skull (release recovery).
quan_chi_skull_release:
  sound_c 56
  forward_to 23
  playback_pair 19
  forward_to 23
  playback_pair 19
  forward_to 23
  playback_pair 19
  forward 0
  ready 0

quan_chi_skull_release_cursor_set_1:
  cursor_set 5
  double_to 17
  tail 0, MK4_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.crossbow_lp (shot recovery).
fujin_crossbow_lp_shot_recovery:
  forward_to 8
  playback_pair 4
  forward_to 8
  playback_pair 4
  forward_to 8
  playback_pair 4
  tail 0, MK4_NATIVE_FUJIN_BOW_FINISH
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.spear (attacker).
scorpion_spear:
  sound_c 5
  fast_start 0, word(scorpion_spear_open_clip_1)
  open_clip 0, clip(14)
  double_to 20
  spawn 0, 0x43, MK4_NATIVE_SPEAR_TASK
  double_to 30
  action_followup 0
  wait 45
  ready 0

scorpion_spear_open_clip_1:
  open_clip 9, clip(14)
  double_count 16
  spawn 0, 0x43, MK4_NATIVE_SPEAR_TASK
  double_to 30
  action_followup 0
  wait 45
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: scorpion.spear (return).
scorpion_spear_return:
  forward_count 19
  native_return 71
  sound_indexed 5
  forward 0
  tail 0, MK4_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN

; Catalog: sonya.ring (startup and recovery); kitana.ring (startup and recovery).
sonya_ring:
  sound_indexed 17
  open_clip 0, clip(15)
  fast_start 0, word(sonya_ring_double_to_1)
  double_to 10
  forward_to 15

sonya_ring_spawn_1:
  spawn 0, 0x44, MK4_NATIVE_SONYA_RING_OBJECT
  spawn 0, 0x26, MK4_NATIVE_SONYA_SCRIPT_RING_FLASH
  sound_c 4
  forward_to 19
  action_followup 0
  playback_pair 15
  forward_to 19
  playback_pair 15
  forward_to 19
  playback_pair 15
  forward_to 19
  playback_pair 0
  ready 0

sonya_ring_double_to_1:
  double_to 15
  jump 0, word(sonya_ring_spawn_1)
  .zero 4  ; non-executable guard between independent streams


; Catalog: sub_zero.ice (attacker).
sub_zero_ice:
  sound_indexed 4
  open_clip 0, clip(16)
  fast_start 0, word(shared_double_to_entry)
  forward_to 15

sub_zero_ice_sound_c_1:
  sound_c 0
  spawn 0, 0x42, MK4_NATIVE_ICE_PROJECTILE_TASK
  wait 1
  action_followup 0
  loop 2
  forward_to 23
  playback_pair 18
  repeat 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: raiden.lightning (startup and recovery).
raiden_lightning:
  sound_indexed 20
  sound_c 43
  fast_start 0, word(raiden_lightning_open_clip_1)
  play_clip 4, clip(17)
  forward 0
  jump 0, word(raiden_lightning_spawn_1)

raiden_lightning_open_clip_1:
  open_clip 0, clip(17)
  double_to 4
  double_count 10

raiden_lightning_spawn_1:
  spawn 0, 0x41, MK4_NATIVE_RAIDEN_LIGHTNING_OBJECT
  wait 1
  action_followup 0
  loop 3
  clip 4, clip(17)
  repeat 0
  clip_alt 4, clip(17)
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: liu_kang.low_fireball (low).
liu_kang_low_fireball_low:
  open_clip 0, clip(19)
  double_to 20
  sound_a 33
  sound_indexed 2
  spawn 0, 0x40, MK4_NATIVE_LIU_LOW_FIREBALL_OBJECT
  forward_to 23
  action_followup 0
  playback_pair 18
  forward_to 23
  playback_pair 18
  forward_to 23
  playback_pair 18
  forward_to 23
  playback_pair 0
  ready 0

; Catalog: liu_kang.high_fireball (high).
liu_kang_high_fireball_high:
  open_clip 0, clip(18)
  double_to 20
  sound_a 33
  sound_indexed 2
  spawn 0, 0x40, MK4_NATIVE_LIU_HIGH_FIREBALL_OBJECT
  forward_to 27
  action_followup 0
  forward_to 18
  forward_to 27
  playback_pair 0
  ready 0

; Catalog: liu_kang.air_fireball (air).
liu_kang_air_fireball_air:
  clip 0, clip(20)
  sound_a 33
  sound_indexed 3
  spawn 0, 0x40, MK4_NATIVE_LIU_AIR_FIREBALL_OBJECT
  wait 16
  clip_operation 0, clip(20)
  tail 0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.crossbow_followup (short shot).
fujin_crossbow_followup_short_shot:
  forward_to 8
  return 0

; Catalog: goro.fireball (clip 592 launch program).
goro_fireball:
  open_clip 0, clip(592)
  forward_to 14
  sound_a 33
  spawn 0, 0x53, MK4_NATIVE_GORO_FIREBALL_SPAWN
  playback_pair 12
  forward_to 14
  playback_pair 12
  forward_to 14
  playback_pair 12
  forward_to 14
  playback_pair 12
  forward 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


shared_open_clip_entry:
  open_clip 0, clip(27)
  double_to 0x211
  jump 0, word(jarek_blade_spawn_1)
  .zero 4  ; non-executable guard between independent streams


shared_double_to_entry:
  double_to 15
  jump 0, word(sub_zero_ice_sound_c_1)
