; Boss attack and landing streams.
; Mnemonics/operands: scripts/README.md. Native IDs select C services.


; Catalog: goro.special_punch (attacker).
goro_special_punch:
  prefixed_to WITH_CLIP + 14, clip(599)  ; bind clip prefix; inline sample/count 14
  tail 0, MK4_NATIVE_GORO_SCRIPT_PUNCH_TAIL

; Catalog: goro.special_kick (attacker).
goro_special_kick:
  prefixed_to WITH_CLIP + 8, clip(598)  ; bind clip prefix; inline sample/count 8
  tail 0, MK4_NATIVE_GORO_SCRIPT_KICK_TAIL
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.leap_stomp (rise).
goro_leap_stomp:
  open_clip 0, clip(601)
  double_to 23
  tail 0, MK4_NATIVE_GORO_SCRIPT_LEAP_CONTINUE
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.leap_stomp (stomp hit).
goro_leap_stomp_stomp_hit:
  handler 0, MK4_NATIVE_GORO_STOMP_HOLD_SEPARATION
  prefixed_sample 46
  native_context 0
  forward 0
  prefixed_to WITH_CLIP + 25, clip(602)  ; bind clip prefix; inline sample/count 25
  sound_special 0
  forward 0
  prefixed_to 25
  sound_special 0
  forward 0
  prefixed_to 8
  motion 0, 0xffffe148, 0xffffe148, 0x28f
  forward_to 25
  native_context 0
  call 0, MK4_NATIVE_DISTURB_RESTING_WEAPONS
  sound_special 0
  landing 0
  forward 0
  tail 0, MK4_NATIVE_GORO_SCRIPT_STOMP_RETURN

; Catalog: goro.leap_stomp (miss landing).
goro_leap_stomp_miss_landing:
  sound_special 0
  forward 0
  prefixed_finish WITH_CLIP + 0, clip(603)  ; bind clip prefix; inline sample/count 0
  ready 0
  .zero 4  ; non-executable guard between independent streams


; Catalog: goro.quake_stomp (jump).
quake_jump:
  prefixed_to WITH_CLIP + 21, clip(602)  ; bind clip prefix; inline sample/count 21
  tail 0, MK4_NATIVE_GORO_SCRIPT_QUAKE_LAND

; Catalog: goro.quake_stomp (landing).
goro_quake_land:
  sound_special 0
  forward 0
  ready 0
  .zero 4  ; non-executable guard
