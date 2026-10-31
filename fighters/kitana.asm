; Generated kitana bytecode and native IDs. Do not edit; run make scripts-export.
; The shared C VM executes these words; native IDs select shared C services.

.export_equ MK4_NATIVE_KITANA_COMBO_HP_ENTRY 0x46c2b0

; Retail source 0x004e6a28; branches use linked names.
kitana_combo_high_punch_attacker:
  open_clip 0x2, clip(533)
  forward_to 0xb
  tail 0x0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  .zero 4 ; non-executable guard
.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_KITANA_COMBO_HIGH_PUNCH_ATTACKER kitana_combo_high_punch_attacker
