; Generated kitana bytecode. Do not edit; run make scripts-export.
; Native symbols are linked to typed C services.


; Branches use linked names.
kitana_combo_high_punch_attacker:
  open_clip 0x2, clip(533)
  forward_to 0xb
  tail 0x0, native_combo_high_punch_tail
  .zero 4 ; non-executable guard
