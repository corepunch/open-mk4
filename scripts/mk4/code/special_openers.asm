; Fire, invisibility and wind entry/recovery streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: scorpion.breathe_fire (attacker).
fire_script_startup:
  open_clip 0, clip(448)
  cursor_set 1
  forward_to 4
  call 0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_DECOR
  forward_to 8
  call 0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_FLAME
  forward_to 11
  tail 0, MK4_NATIVE_FIRE_CONTACT
  .zero 4  ; non-executable guard between independent streams


fire_script_recovery:
  forward_to 30
  tail 0, MK4_NATIVE_FIRE_RECOVERY
  .zero 4  ; non-executable guard between independent streams


; Catalog: cage.groin_punch (owner reverse recovery).
groin_punch_groin_recover:
  backward 0
  ready 0

; Catalog: reptile.invisibility (toggle startup).
reptile_invisibility:
  play_clip 24, clip(12)
  sound_c 75
  sound_c 74
  tail 0, MK4_NATIVE_REPTILE_SCRIPT_INVISIBILITY
  .zero 4  ; non-executable guard between independent streams


; Catalog: kai.fatality_2 (victim fall).
kai_second_victim_fall:
  prefixed_to WITH_CLIP + 20, clip(206)  ; bind clip prefix; inline sample/count 20
  sound_special 0
  forward_to 90
  sound_special 0
  forward 0
  park 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.wind_slam (slam owner).
fujin_wind_slam:
  native_return 97
  prefixed_to WITH_CLIP + 6, clip(99)  ; bind clip prefix; inline sample/count 6
  forward 0
  damage 0, 0x147a
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: fujin.wind (caster recovery).
fujin_wind_recovery:
  forward 0
  playback_pair 34
  forward_to 40
  playback_pair 34
  forward_to 40
  playback_pair 30
  ready 0

fujin_wind_recovery_open_clip_1:
  open_clip 0, clip(98)
  double_to 16
  jump 0, word(fujin_wind_query_d_1)

; Catalog: fujin.wind (wind caster).
fujin_wind:
  sound_c 63
  fast_start 0, word(fujin_wind_recovery_open_clip_1)
  prefixed_to WITH_CLIP + 16, clip(98)  ; bind clip prefix; inline sample/count 16

fujin_wind_query_d_1:
  query_d 8
  tail 0, MK4_NATIVE_FUJIN_WIND_RECOVERY
