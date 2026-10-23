; Fighter resource/rig records, in roster order.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


; Roster records (game/data/tables.c, common/mk4_assets.c): 8 words per slot.
; asset slot, alternate slot, native ID, body rig, alternate rig, cut rig, alternate cut rig, scale Q16.
roster_record_table:
  .word word(asset_slot_sc_geo_geo), word(asset_slot_sc_geo_geo), 0x48d0b0, word(scorpion_body_rig), word(scorpion_body_alternate_rig), word(scorpion_cut_rig), word(scorpion_cut_alternate_rig), 0xe666  ; 0: roster_scorpion

roster_raiden:
  .word word(asset_slot_ra_geo_geo), word(asset_slot_ra_geo_geo), 0x48d0e0, word(raiden_body_rig), word(raiden_body_alternate_rig), word(raiden_cut_rig), word(raiden_cut_alternate_rig), 0xc000  ; 1: roster_raiden

roster_sonya:
  .word word(asset_slot_sb_geo_geo), word(asset_slot_sb_geo_geo), 0x48d120, word(sonya_body_rig), word(sonya_body_alternate_rig), word(sonya_cut_rig), word(sonya_cut_alternate_rig), 0xb333  ; 2: roster_sonya

roster_liu_kang:
  .word word(asset_slot_lk_geo_geo), word(asset_slot_lk_geo_geo), 0x48d150, word(liu_kang_body_rig), word(liu_kang_body_alternate_rig), word(liu_kang_cut_rig), word(liu_kang_cut_alternate_rig), 0xcccc  ; 3: roster_liu_kang

roster_sub_zero:
  .word word(spine_asset_slot), word(spine_asset_slot), 0x48d170, word(sub_zero_body_rig), word(sub_zero_body_alternate_rig), word(sub_zero_cut_rig), word(sub_zero_cut_alternate_rig), 0xe666  ; 4: roster_sub_zero

roster_fujin:
  .word word(asset_slot_wg_geo_geo), word(asset_slot_wg_geo_geo), 0x48d1b0, word(fujin_body_rig), word(fujin_body_alternate_rig), word(fujin_cut_rig), word(fujin_cut_alternate_rig), 0xe666  ; 5: roster_fujin

roster_shinnok:
  .word word(asset_slot_sh_geo_geo), word(asset_slot_sh_geo_geo), 0x48d1e0, word(shinnok_body_rig), word(shinnok_body_alternate_rig), word(shinnok_cut_alternate_rig), word(shinnok_cut_alternate_rig), 0xfd70  ; 6: roster_shinnok

roster_reiko:
  .word word(asset_slot_ns_geo_geo), word(asset_slot_ns_geo_geo), 0x48d200, word(reiko_body_rig), word(reiko_body_alternate_rig), word(reiko_cut_rig), word(reiko_cut_alternate_rig), 0xf333  ; 7: roster_reiko

roster_quan_chi:
  .word word(asset_slot_qc_geo_geo), word(asset_slot_qc_geo_geo), 0x48d230, word(quan_chi_body_rig), word(quan_chi_body_alternate_rig), word(quan_chi_cut_alternate_rig), word(quan_chi_cut_alternate_rig), 0xe666  ; 8: roster_quan_chi

roster_tanya:
  .word word(asset_slot_ta_geo_geo), word(asset_slot_ta_geo_geo), 0x48d2b0, word(tanya_body_rig), word(tanya_body_alternate_rig), word(tanya_cut_rig), word(tanya_cut_alternate_rig), 0xcccc  ; 9: roster_tanya

roster_reptile:
  .word word(asset_slot_re_geo_geo), word(asset_slot_re_geo_geo), 0x48d250, word(reptile_body_rig), word(reptile_body_alternate_rig), word(reptile_cut_rig), word(reptile_cut_alternate_rig), 0xf333  ; 10: roster_reptile

roster_kai:
  .word word(asset_slot_me_geo_geo), word(asset_slot_me_geo_geo), 0x48d280, word(kai_body_rig), word(kai_body_alternate_rig), word(kai_cut_rig), word(kai_cut_alternate_rig), 0xcccc  ; 11: roster_kai

roster_jarek:
  .word word(asset_slot_ja_geo_geo), word(asset_slot_ja_geo_geo), 0x48d2e0, word(jarek_body_rig), word(jarek_body_alternate_rig), word(jarek_cut_rig), word(jarek_cut_alternate_rig), 0xd999  ; 12: roster_jarek

roster_jax:
  .word word(asset_slot_jx_geo_geo), word(asset_slot_jx_geo_geo), 0x48d310, word(jax_body_rig), word(jax_body_alternate_rig), word(jax_cut_rig), word(jax_cut_alternate_rig), 0xfd70  ; 13: roster_jax

roster_cage:
  .word word(asset_slot_jc_geo_geo), word(asset_slot_jc_geo_geo), 0x48d340, word(cage_body_rig), word(cage_body_alternate_rig), word(cage_cut_rig), word(cage_cut_alternate_rig), 0xd999  ; 14: roster_cage

roster_goro:
  .word word(asset_slot_go_geo_geo), word(asset_slot_go_geo_geo), 0x48d3b0, word(goro_body_alternate_rig), word(goro_body_alternate_rig), word(goro_cut_alternate_rig), word(goro_cut_alternate_rig), 0xd999  ; 15: roster_goro

roster_kitana:
  .word word(asset_slot_ki_geo_geo), word(asset_slot_ki_geo_geo), 0x48d370, word(kitana_body_alternate_rig), word(kitana_body_alternate_rig), word(tanya_cut_rig), word(tanya_cut_alternate_rig), 0xd999  ; 16: roster_kitana

roster_noob_saibot:
  .word word(asset_slot_nns_geo_geo), word(asset_slot_nns_geo_geo), 0x48d390, word(noob_saibot_body_alternate_rig), word(noob_saibot_body_alternate_rig), word(noob_saibot_cut_alternate_rig), word(noob_saibot_cut_alternate_rig), 0xd999  ; 17: roster_noob_saibot
