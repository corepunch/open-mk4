; Fighter resource slots and hierarchy records, plus retained resource data.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


spine_asset_slot:
  .word asset_sz_geo_geo
  .zero 8
  .word 0x4f16e8
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_wg_geo_geo:
  .word asset_wg_geo_geo
  .zero 8
  .word 0x4f1748
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_ns_geo_geo:
  .word asset_ns_geo_geo
  .zero 8
  .word 0x4f17a8
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_nns_geo_geo:
  .word asset_nns_geo_geo
  .zero 8
  .word 0x4f1808
  .zero 8

; scorpion: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
scorpion_body_rig:
  .word 0x400994, asset_slot_sc_geo_geo, 0, 0xffffff37, 0xfffed3b7, 0x142, 0  ; node ID 4, depth 0

scorpion_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_sc_geo_geo, 0, 0, 0xffffe20d, 0, 0  ; node ID 4095, depth 1

scorpion_body_rig_joint_3:
  .word 0x300980, asset_slot_sc_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

scorpion_body_rig_joint_2:
  .word 0x200988, asset_slot_sc_geo_geo, 0, 0, 0xffff75ea, 0x160a, 0x2  ; node ID 2, depth 3

scorpion_body_rig_joint_5:
  .word 0x500980, asset_slot_sc_geo_geo, 0, 0x3f5c, 0xffffae1f, 0x183e, 0x3  ; node ID 5, depth 3

scorpion_body_rig_joint_7:
  .word 0x700990, asset_slot_sc_geo_geo, 0, 0xfffff7f0, 0xffffaf1b, 0x2b0, 0x4  ; node ID 7, depth 4

scorpion_body_rig_joint_9:
  .word 0x900998, asset_slot_sc_geo_geo, 0, 0x45a, 0xffffbf5d, 0xffffff3c, 0x5  ; node ID 9, depth 5

scorpion_body_rig_joint_6:
  .word 0x600993, asset_slot_sc_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x6  ; node ID 6, depth 3

scorpion_body_rig_joint_8:
  .word 0x800991, asset_slot_sc_geo_geo, 0, 0x810, 0xffffaf1b, 0x2b0, 0x7  ; node ID 8, depth 4

scorpion_body_rig_joint_10:
  .word 0xa00999, asset_slot_sc_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffff3c, 0x8  ; node ID 10, depth 5

scorpion_body_rig_joint_17:
  .word 0x1100888, asset_slot_sc_geo_geo, 0, 0, 0, 0, 0x9  ; node ID 17, depth 2

scorpion_body_rig_joint_11:
  .word 0xb00980, asset_slot_sc_geo_geo, 0, 0x16f6, 0x336e, 0xfffffdc0, 0xf  ; node ID 11, depth 2

scorpion_body_rig_joint_13:
  .word 0xd00990, asset_slot_sc_geo_geo, 0, 0xfffff8c8, 0xffff887a, 0x5c9, 0x10  ; node ID 13, depth 3

scorpion_body_rig_joint_15:
  .word 0xf00998, asset_slot_sc_geo_geo, 0, 0x11eb, 0xffff80b1, 0x70a, 0x11  ; node ID 15, depth 4

scorpion_body_rig_joint_12:
  .word 0xc00993, asset_slot_sc_geo_geo, 0, 0xffffe90a, 0x336e, 0xfffffdc0, 0x12  ; node ID 12, depth 2

scorpion_body_rig_joint_14:
  .word 0xe00991, asset_slot_sc_geo_geo, 0, 0x738, 0xffff887a, 0x5c9, 0x13  ; node ID 14, depth 3

scorpion_body_rig_joint_16:
  .word 0x1000999, asset_slot_sc_geo_geo, 0, 0xffffee15, 0xffff80b5, 0x70a, 0x14  ; node ID 16, depth 4
  .word 0x1100888, 0x511cbc
  .zero 16
  .word 0xa, 0x1100888, 0x511cbc
  .zero 16
  .word 0xb, 0x1100888, 0x511cbc
  .zero 16
  .word 0xc, 0x1100888, 0x511cbc
  .zero 16
  .word 0xd, 0x1100888, 0x511cbc
  .zero 16
  .word 0xe

; scorpion: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
scorpion_cut_rig:
  .word 0x400994, asset_slot_sc_geo_geo, 0, 0, 0xfffed3b7, 0, 0  ; node ID 4, depth 0

scorpion_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_sc_geo_geo, 0, 0, 0xffffe20d, 0, 0x15  ; node ID 4095, depth 1

scorpion_cut_rig_joint_3:
  .word 0x300980, asset_slot_sc_geo_geo, 0, 0, 0, 0, 0x16  ; node ID 3, depth 2

scorpion_cut_rig_joint_2:
  .word 0x200988, asset_slot_sc_geo_geo, 0, 0, 0xffff75ea, 0x160a, 0x17  ; node ID 2, depth 3

scorpion_cut_rig_joint_5:
  .word 0x500980, asset_slot_sc_geo_geo, 0, 0x3f5c, 0xffffae1f, 0x183e, 0x18  ; node ID 5, depth 3

scorpion_cut_rig_joint_7:
  .word 0x700990, asset_slot_sc_geo_geo, 0, 0xfffff7f0, 0xffffaf1b, 0x2b0, 0x19  ; node ID 7, depth 4

scorpion_cut_rig_joint_9:
  .word 0x900998, asset_slot_sc_geo_geo, 0, 0x45a, 0xffffbf5d, 0xffffff3c, 0x1a  ; node ID 9, depth 5

scorpion_cut_rig_joint_6:
  .word 0x600993, asset_slot_sc_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x1b  ; node ID 6, depth 3

scorpion_cut_rig_joint_8:
  .word 0x800991, asset_slot_sc_geo_geo, 0, 0x810, 0xffffaf1b, 0x2b0, 0x1c  ; node ID 8, depth 4

scorpion_cut_rig_joint_10:
  .word 0xa00999, asset_slot_sc_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffff3c, 0x1d  ; node ID 10, depth 5

scorpion_cut_rig_joint_17:
  .word 0x1100888, asset_slot_sc_geo_geo, 0, 0, 0, 0, 0x1e  ; node ID 17, depth 2

scorpion_cut_rig_joint_11:
  .word 0xb00980, asset_slot_sc_geo_geo, 0, 0x16f6, 0x336e, 0xfffffdc0, 0x1f  ; node ID 11, depth 2

scorpion_cut_rig_joint_13:
  .word 0xd00990, asset_slot_sc_geo_geo, 0, 0xfffff8c8, 0xffff887a, 0x5c9, 0x20  ; node ID 13, depth 3

scorpion_cut_rig_joint_15:
  .word 0xf00998, asset_slot_sc_geo_geo, 0, 0x11eb, 0xffff80b1, 0x70a, 0x21  ; node ID 15, depth 4

scorpion_cut_rig_joint_12:
  .word 0xc00993, asset_slot_sc_geo_geo, 0, 0xffffe90a, 0x336e, 0xfffffdc0, 0x22  ; node ID 12, depth 2

scorpion_cut_rig_joint_14:
  .word 0xe00991, asset_slot_sc_geo_geo, 0, 0x738, 0xffff887a, 0x5c9, 0x23  ; node ID 14, depth 3

scorpion_cut_rig_joint_16:
  .word 0x1000999, asset_slot_sc_geo_geo, 0, 0xffffee15, 0xffff80b5, 0x70a, 0x24  ; node ID 16, depth 4

scorpion_body_alternate_rig:
  .word 0x400994, 0x511cc8
  .zero 8
  .word 0xfffed3b7
  .zero 8
  .word 0xfff00890, 0x511cc8
  .zero 8
  .word 0xffffe20d
  .zero 8
  .word 0x300980, 0x511cc8
  .zero 16
  .word 0x1, 0x200988, 0x511cc8
  .zero 8
  .word 0xffff75ea, 0x160a, 0x2, 0x500980, 0x511cc8
  .zero 4
  .word 0x3f5c, 0xffffae1f, 0x183e, 0x3, 0x700990, 0x511cc8
  .zero 4
  .word 0xfffff7f0, 0xffffaf1b, 0x2b0, 0x4, 0x900998, 0x511cc8
  .zero 4
  .word 0x45a, 0xffffbf5d, 0xffffff3c, 0x5, 0x600993, 0x511cc8
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x6, 0x800991, 0x511cc8
  .zero 4
  .word 0x810, 0xffffaf1b, 0x2b0, 0x7, 0xa00999, 0x511cc8
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffff3c, 0x8, 0x1100888, 0x511cc8
  .zero 16
  .word 0x9, 0xb00980, 0x511cc8
  .zero 4
  .word 0x16f6, 0x336e, 0xfffffdc0, 0xf, 0xd00990, 0x511cc8
  .zero 4
  .word 0xfffff8c8, 0xffff887a, 0x5c9, 0x10, 0xf00998, 0x511cc8
  .zero 4
  .word 0x11eb, 0xffff80b1, 0x70a, 0x11, 0xc00993, 0x511cc8
  .zero 4
  .word 0xffffe90a, 0x336e, 0xfffffdc0, 0x12, 0xe00991, 0x511cc8
  .zero 4
  .word 0x738, 0xffff887a, 0x5c9, 0x13, 0x1000999, 0x511cc8
  .zero 4
  .word 0xffffee15, 0xffff80b5, 0x70a, 0x14, 0x1100888, 0x511cc8
  .zero 16
  .word 0xa, 0x1100888, 0x511cc8
  .zero 16
  .word 0xb, 0x1100888, 0x511cc8
  .zero 16
  .word 0xc, 0x1100888, 0x511cc8
  .zero 16
  .word 0xd, 0x1100888, 0x511cc8
  .zero 16
  .word 0xe

scorpion_cut_alternate_rig:
  .word 0x400994, 0x511cc8
  .zero 8
  .word 0xfffed3b7
  .zero 8
  .word 0xfff00890, 0x511cc8
  .zero 8
  .word 0xffffe20d
  .zero 4
  .word 0x15, 0x300980, 0x511cc8
  .zero 16
  .word 0x16, 0x200988, 0x511cc8
  .zero 8
  .word 0xffff75ea, 0x160a, 0x17, 0x500980, 0x511cc8
  .zero 4
  .word 0x3f5c, 0xffffae1f, 0x183e, 0x18, 0x700990, 0x511cc8
  .zero 4
  .word 0xfffff7f0, 0xffffaf1b, 0x2b0, 0x19, 0x900998, 0x511cc8
  .zero 4
  .word 0x45a, 0xffffbf5d, 0xffffff3c, 0x1a, 0x600993, 0x511cc8
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x1b, 0x800991, 0x511cc8
  .zero 4
  .word 0x810, 0xffffaf1b, 0x2b0, 0x1c, 0xa00999, 0x511cc8
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffff3c, 0x1d, 0x1100888, 0x511cc8
  .zero 16
  .word 0x1e, 0xb00980, 0x511cc8
  .zero 4
  .word 0x16f6, 0x336e, 0xfffffdc0, 0x1f, 0xd00990, 0x511cc8
  .zero 4
  .word 0xfffff8c8, 0xffff887a, 0x5c9, 0x20, 0xf00998, 0x511cc8
  .zero 4
  .word 0x11eb, 0xffff80b1, 0x70a, 0x21, 0xc00993, 0x511cc8
  .zero 4
  .word 0xffffe90a, 0x336e, 0xfffffdc0, 0x22, 0xe00991, 0x511cc8
  .zero 4
  .word 0x738, 0xffff887a, 0x5c9, 0x23, 0x1000999, 0x511cc8
  .zero 4
  .word 0xffffee15, 0xffff80b5, 0x70a, 0x24

; raiden: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
raiden_body_rig:
  .word 0x400994, asset_slot_ra_geo_geo, 0, 0, 0xfffed917, 0, 0  ; node ID 4, depth 0

raiden_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_ra_geo_geo, 0, 0, 0xffffee36, 0, 0  ; node ID 4095, depth 1

raiden_body_rig_joint_3:
  .word 0x300980, asset_slot_ra_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

raiden_body_rig_joint_2:
  .word 0x200980, asset_slot_ra_geo_geo, 0, 0, 0xffff6ccd, 0x12d0, 0x2  ; node ID 2, depth 3

raiden_body_rig_joint_4094:
  .word 0xffe00898, asset_slot_ra_geo_geo, 0, 0, 0xffffe000, 0xfffffd71, 0x3  ; node ID 4094, depth 4

raiden_body_rig_joint_5:
  .word 0x500980, asset_slot_ra_geo_geo, 0, 0x3d0e, 0xffffb334, 0x1439, 0x4  ; node ID 5, depth 3

raiden_body_rig_joint_7:
  .word 0x700990, asset_slot_ra_geo_geo, 0, 0xfffffae2, 0xffffa8d5, 0xc4, 0x5  ; node ID 7, depth 4

raiden_body_rig_joint_9:
  .word 0x900998, asset_slot_ra_geo_geo, 0, 0x5c2, 0xffffb8f6, 0, 0x6  ; node ID 9, depth 5

raiden_body_rig_joint_6:
  .word 0x600993, asset_slot_ra_geo_geo, 0, 0xffffc2f2, 0xffffb334, 0x1439, 0x7  ; node ID 6, depth 3

raiden_body_rig_joint_8:
  .word 0x800991, asset_slot_ra_geo_geo, 0, 0x51e, 0xffffa8d5, 0xc4, 0x8  ; node ID 8, depth 4

raiden_body_rig_joint_10:
  .word 0xa00999, asset_slot_ra_geo_geo, 0, 0xfffffa3e, 0xffffb8f6, 0, 0x9  ; node ID 10, depth 5

raiden_body_rig_joint_11:
  .word 0xb00980, asset_slot_ra_geo_geo, 0, 0x1851, 0x2b85, 0xfffff959, 0xa  ; node ID 11, depth 2

raiden_body_rig_joint_13:
  .word 0xd00990, asset_slot_ra_geo_geo, 0, 0xfffffaa0, 0xffff876d, 0, 0xb  ; node ID 13, depth 3

raiden_body_rig_joint_15:
  .word 0xf00998, asset_slot_ra_geo_geo, 0, 0xf5c, 0xffff7eb9, 0x51e, 0xc  ; node ID 15, depth 4

raiden_body_rig_joint_12:
  .word 0xc00983, asset_slot_ra_geo_geo, 0, 0xffffe7af, 0x2b85, 0xfffff959, 0xd  ; node ID 12, depth 2

raiden_body_rig_joint_14:
  .word 0xe00991, asset_slot_ra_geo_geo, 0, 0x560, 0xffff876d, 0, 0xe  ; node ID 14, depth 3

raiden_body_rig_joint_16:
  .word 0x1000999, asset_slot_ra_geo_geo, 0, 0xfffff0a4, 0xffff7eb9, 0x51e, 0xf  ; node ID 16, depth 4

raiden_body_rig_joint_4093:
  .word 0xffd00898, asset_slot_ra_geo_geo, 0, 0, 0, 0, 0x10  ; node ID 4093, depth 2

; raiden: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
raiden_cut_rig:
  .word 0x400994, asset_slot_ra_geo_geo, 0, 0, 0xfffed917, 0, 0  ; node ID 4, depth 0

raiden_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_ra_geo_geo, 0, 0, 0xffffee36, 0, 0x11  ; node ID 4095, depth 1

raiden_cut_rig_joint_3:
  .word 0x300980, asset_slot_ra_geo_geo, 0, 0, 0, 0, 0x12  ; node ID 3, depth 2

raiden_cut_rig_joint_2:
  .word 0x200980, asset_slot_ra_geo_geo, 0, 0, 0xffff6ccd, 0x12d0, 0x13  ; node ID 2, depth 3

raiden_cut_rig_joint_4094:
  .word 0xffe00898, asset_slot_ra_geo_geo, 0, 0, 0xffffe000, 0xfffffd71, 0x14  ; node ID 4094, depth 4

raiden_cut_rig_joint_5:
  .word 0x500980, asset_slot_ra_geo_geo, 0, 0x3d0e, 0xffffb334, 0x1439, 0x15  ; node ID 5, depth 3

raiden_cut_rig_joint_7:
  .word 0x700990, asset_slot_ra_geo_geo, 0, 0xfffffae2, 0xffffa8d5, 0xc4, 0x16  ; node ID 7, depth 4

raiden_cut_rig_joint_9:
  .word 0x900998, asset_slot_ra_geo_geo, 0, 0x5c2, 0xffffb8f6, 0, 0x17  ; node ID 9, depth 5

raiden_cut_rig_joint_6:
  .word 0x600993, asset_slot_ra_geo_geo, 0, 0xffffc2f2, 0xffffb334, 0x1439, 0x18  ; node ID 6, depth 3

raiden_cut_rig_joint_8:
  .word 0x800991, asset_slot_ra_geo_geo, 0, 0x51e, 0xffffa8d5, 0xc4, 0x19  ; node ID 8, depth 4

raiden_cut_rig_joint_10:
  .word 0xa00999, asset_slot_ra_geo_geo, 0, 0xfffffa3e, 0xffffb8f6, 0, 0x1a  ; node ID 10, depth 5

raiden_cut_rig_joint_11:
  .word 0xb00980, asset_slot_ra_geo_geo, 0, 0x1851, 0x2b85, 0xfffff959, 0x1b  ; node ID 11, depth 2

raiden_cut_rig_joint_13:
  .word 0xd00990, asset_slot_ra_geo_geo, 0, 0xfffffaa0, 0xffff876d, 0, 0x1c  ; node ID 13, depth 3

raiden_cut_rig_joint_15:
  .word 0xf00998, asset_slot_ra_geo_geo, 0, 0xf5c, 0xffff7eb9, 0x51e, 0x1d  ; node ID 15, depth 4

raiden_cut_rig_joint_12:
  .word 0xc00983, asset_slot_ra_geo_geo, 0, 0xffffe7af, 0x2b85, 0xfffff959, 0x1e  ; node ID 12, depth 2

raiden_cut_rig_joint_14:
  .word 0xe00991, asset_slot_ra_geo_geo, 0, 0x560, 0xffff876d, 0, 0x1f  ; node ID 14, depth 3

raiden_cut_rig_joint_16:
  .word 0x1000999, asset_slot_ra_geo_geo, 0, 0xfffff0a4, 0xffff7eb9, 0x51e, 0x20  ; node ID 16, depth 4

raiden_cut_rig_joint_4093:
  .word 0xffd00898, asset_slot_ra_geo_geo, 0, 0, 0, 0, 0x21  ; node ID 4093, depth 2

raiden_body_alternate_rig:
  .word 0x400994, 0x511ce0
  .zero 8
  .word 0xfffed917
  .zero 8
  .word 0xfff00890, 0x511ce0
  .zero 8
  .word 0xffffee36
  .zero 8
  .word 0x300980, 0x511ce0
  .zero 16
  .word 0x1, 0x200980, 0x511ce0
  .zero 8
  .word 0xffff6ccd, 0x12d0, 0x2, 0xffe00898, 0x511ce0
  .zero 8
  .word 0xffffe000, 0xfffffd71, 0x3, 0x500980, 0x511ce0
  .zero 4
  .word 0x3d0e, 0xffffb334, 0x1439, 0x4, 0x700990, 0x511ce0
  .zero 4
  .word 0xfffffae2, 0xffffa8d5, 0xc4, 0x5, 0x900998, 0x511ce0
  .zero 4
  .word 0x5c2, 0xffffb8f6
  .zero 4
  .word 0x6, 0x600993, 0x511ce0
  .zero 4
  .word 0xffffc2f2, 0xffffb334, 0x1439, 0x7, 0x800991, 0x511ce0
  .zero 4
  .word 0x51e, 0xffffa8d5, 0xc4, 0x8, 0xa00999, 0x511ce0
  .zero 4
  .word 0xfffffa3e, 0xffffb8f6
  .zero 4
  .word 0x9, 0xb00980, 0x511ce0
  .zero 4
  .word 0x1851, 0x2b85, 0xfffff959, 0xa, 0xd00990, 0x511ce0
  .zero 4
  .word 0xfffffaa0, 0xffff876d
  .zero 4
  .word 0xb, 0xf00998, 0x511ce0
  .zero 4
  .word 0xf5c, 0xffff7eb9, 0x51e, 0xc, 0xc00983, 0x511ce0
  .zero 4
  .word 0xffffe7af, 0x2b85, 0xfffff959, 0xd, 0xe00991, 0x511ce0
  .zero 4
  .word 0x560, 0xffff876d
  .zero 4
  .word 0xe, 0x1000999, 0x511ce0
  .zero 4
  .word 0xfffff0a4, 0xffff7eb9, 0x51e, 0xf, 0xffd00898, 0x511ce0
  .zero 16
  .word 0x10

raiden_cut_alternate_rig:
  .word 0x400994, 0x511ce0
  .zero 8
  .word 0xfffed917
  .zero 8
  .word 0xfff00890, 0x511ce0
  .zero 8
  .word 0xffffee36
  .zero 4
  .word 0x11, 0x300980, 0x511ce0
  .zero 16
  .word 0x12, 0x200980, 0x511ce0
  .zero 8
  .word 0xffff6ccd, 0x12d0, 0x13, 0xffe00898, 0x511ce0
  .zero 8
  .word 0xffffe000, 0xfffffd71, 0x14, 0x500980, 0x511ce0
  .zero 4
  .word 0x3d0e, 0xffffb334, 0x1439, 0x15, 0x700990, 0x511ce0
  .zero 4
  .word 0xfffffae2, 0xffffa8d5, 0xc4, 0x16, 0x900998, 0x511ce0
  .zero 4
  .word 0x5c2, 0xffffb8f6
  .zero 4
  .word 0x17, 0x600993, 0x511ce0
  .zero 4
  .word 0xffffc2f2, 0xffffb334, 0x1439, 0x18, 0x800991, 0x511ce0
  .zero 4
  .word 0x51e, 0xffffa8d5, 0xc4, 0x19, 0xa00999, 0x511ce0
  .zero 4
  .word 0xfffffa3e, 0xffffb8f6
  .zero 4
  .word 0x1a, 0xb00980, 0x511ce0
  .zero 4
  .word 0x1851, 0x2b85, 0xfffff959, 0x1b, 0xd00990, 0x511ce0
  .zero 4
  .word 0xfffffaa0, 0xffff876d
  .zero 4
  .word 0x1c, 0xf00998, 0x511ce0
  .zero 4
  .word 0xf5c, 0xffff7eb9, 0x51e, 0x1d, 0xc00983, 0x511ce0
  .zero 4
  .word 0xffffe7af, 0x2b85, 0xfffff959, 0x1e, 0xe00991, 0x511ce0
  .zero 4
  .word 0x560, 0xffff876d
  .zero 4
  .word 0x1f, 0x1000999, 0x511ce0
  .zero 4
  .word 0xfffff0a4, 0xffff7eb9, 0x51e, 0x20, 0xffd00898, 0x511ce0
  .zero 16
  .word 0x21

; sonya: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
sonya_body_rig:
  .word 0x400994, asset_slot_sb_geo_geo, 0, 0, 0xfffee063, 0x9fb, 0  ; node ID 4, depth 0

sonya_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_sb_geo_geo, 0, 0, 0xffffeb4e, 0xfffff585, 0  ; node ID 4095, depth 1

sonya_body_rig_joint_3:
  .word 0x300980, asset_slot_sb_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

sonya_body_rig_joint_2:
  .word 0x200980, asset_slot_sb_geo_geo, 0, 0, 0xffff763e, 0xd70, 0x2  ; node ID 2, depth 3

sonya_body_rig_joint_17:
  .word 0x1100898, asset_slot_sb_geo_geo, 0, 0, 0xffffefe0, 0x18dd, 0x3  ; node ID 17, depth 4

sonya_body_rig_joint_5:
  .word 0x500980, asset_slot_sb_geo_geo, 0, 0x2b02, 0xffffa3ce, 0x1785, 0xf  ; node ID 5, depth 3

sonya_body_rig_joint_7:
  .word 0x700990, asset_slot_sb_geo_geo, 0, 0x204, 0xffffabbc, 0x158, 0x10  ; node ID 7, depth 4

sonya_body_rig_joint_9:
  .word 0x900998, asset_slot_sb_geo_geo, 0, 0, 0xffffbd12, 0x89, 0x11  ; node ID 9, depth 5

sonya_body_rig_joint_6:
  .word 0x600993, asset_slot_sb_geo_geo, 0, 0xffffd4fe, 0xffffa3ce, 0x1785, 0x12  ; node ID 6, depth 3

sonya_body_rig_joint_8:
  .word 0x800991, asset_slot_sb_geo_geo, 0, 0xfffffdfc, 0xffffabbc, 0x158, 0x13  ; node ID 8, depth 4

sonya_body_rig_joint_10:
  .word 0xa00999, asset_slot_sb_geo_geo, 0, 0, 0xffffbd12, 0x89, 0x14  ; node ID 10, depth 5

sonya_body_rig_joint_11:
  .word 0xb00980, asset_slot_sb_geo_geo, 0, 0x19f0, 0x1ec7, 0xa7b, 0x15  ; node ID 11, depth 2

sonya_body_rig_joint_13:
  .word 0xd00990, asset_slot_sb_geo_geo, 0, 0, 0xffff845b, 0x35c, 0x16  ; node ID 13, depth 3

sonya_body_rig_joint_15:
  .word 0xf00998, asset_slot_sb_geo_geo, 0, 0x10ef, 0xffff7fc9, 0xcc4, 0x17  ; node ID 15, depth 4

sonya_body_rig_joint_12:
  .word 0xc00993, asset_slot_sb_geo_geo, 0, 0xffffe610, 0x1ec7, 0xa7b, 0x18  ; node ID 12, depth 2

sonya_body_rig_joint_14:
  .word 0xe00991, asset_slot_sb_geo_geo, 0, 0, 0xffff845b, 0x35c, 0x19  ; node ID 14, depth 3

sonya_body_rig_joint_16:
  .word 0x1000999, asset_slot_sb_geo_geo, 0, 0xffffef11, 0xffff7fc9, 0xcc4, 0x1a  ; node ID 16, depth 4
  .word 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x4, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x5, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x6, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x7, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x8, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x9, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xa, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xb, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xc, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xd, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xe

; sonya: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
sonya_cut_rig:
  .word 0x400994, asset_slot_sb_geo_geo, 0, 0, 0xfffee063, 0x9fb, 0  ; node ID 4, depth 0

sonya_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_sb_geo_geo, 0, 0, 0xffffeb4e, 0xfffff585, 0x1b  ; node ID 4095, depth 1

sonya_cut_rig_joint_3:
  .word 0x300980, asset_slot_sb_geo_geo, 0, 0, 0, 0, 0x1c  ; node ID 3, depth 2

sonya_cut_rig_joint_2:
  .word 0x200980, asset_slot_sb_geo_geo, 0, 0, 0xffff763e, 0xd70, 0x1d  ; node ID 2, depth 3

sonya_cut_rig_joint_17:
  .word 0x1100898, asset_slot_sb_geo_geo, 0, 0, 0xffffefe0, 0x18dd, 0x1e  ; node ID 17, depth 4

sonya_cut_rig_joint_5:
  .word 0x500980, asset_slot_sb_geo_geo, 0, 0x2b02, 0xffffa3ce, 0x1785, 0x2a  ; node ID 5, depth 3

sonya_cut_rig_joint_7:
  .word 0x700990, asset_slot_sb_geo_geo, 0, 0x204, 0xffffabbc, 0x158, 0x2b  ; node ID 7, depth 4

sonya_cut_rig_joint_9:
  .word 0x900998, asset_slot_sb_geo_geo, 0, 0, 0xffffbd12, 0x89, 0x2c  ; node ID 9, depth 5

sonya_cut_rig_joint_6:
  .word 0x600993, asset_slot_sb_geo_geo, 0, 0xffffd4fe, 0xffffa3ce, 0x1785, 0x2d  ; node ID 6, depth 3

sonya_cut_rig_joint_8:
  .word 0x800991, asset_slot_sb_geo_geo, 0, 0xfffffdfc, 0xffffabbc, 0x158, 0x2e  ; node ID 8, depth 4

sonya_cut_rig_joint_10:
  .word 0xa00999, asset_slot_sb_geo_geo, 0, 0, 0xffffbd12, 0x89, 0x2f  ; node ID 10, depth 5

sonya_cut_rig_joint_11:
  .word 0xb00980, asset_slot_sb_geo_geo, 0, 0x19f0, 0x1ec7, 0xa7b, 0x30  ; node ID 11, depth 2

sonya_cut_rig_joint_13:
  .word 0xd00990, asset_slot_sb_geo_geo, 0, 0, 0xffff845b, 0x35c, 0x31  ; node ID 13, depth 3

sonya_cut_rig_joint_15:
  .word 0xf00998, asset_slot_sb_geo_geo, 0, 0x10ef, 0xffff7fc9, 0xcc4, 0x32  ; node ID 15, depth 4

sonya_cut_rig_joint_12:
  .word 0xc00993, asset_slot_sb_geo_geo, 0, 0xffffe610, 0x1ec7, 0xa7b, 0x33  ; node ID 12, depth 2

sonya_cut_rig_joint_14:
  .word 0xe00991, asset_slot_sb_geo_geo, 0, 0, 0xffff845b, 0x35c, 0x34  ; node ID 14, depth 3

sonya_cut_rig_joint_16:
  .word 0x1000999, asset_slot_sb_geo_geo, 0, 0xffffef11, 0xffff7fc9, 0xcc4, 0x35  ; node ID 16, depth 4
  .word 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x1f, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x20, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x21, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x22, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x23, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x24, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x25, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x26, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x27, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x28, 0x1100898, 0x511cec
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x29

sonya_body_alternate_rig:
  .word 0x400994, 0x511cf8
  .zero 8
  .word 0xfffee063, 0x9fb
  .zero 4
  .word 0xfff00890, 0x511cf8
  .zero 8
  .word 0xffffeb4e, 0xfffff585
  .zero 4
  .word 0x300980, 0x511cf8
  .zero 16
  .word 0x1, 0x200980, 0x511cf8
  .zero 8
  .word 0xffff763e, 0xd70, 0x2, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x3, 0x500980, 0x511cf8
  .zero 4
  .word 0x2b02, 0xffffa3ce, 0x1785, 0xf, 0x700990, 0x511cf8
  .zero 4
  .word 0x204, 0xffffabbc, 0x158, 0x10, 0x900998, 0x511cf8
  .zero 8
  .word 0xffffbd12, 0x89, 0x11, 0x600993, 0x511cf8
  .zero 4
  .word 0xffffd4fe, 0xffffa3ce, 0x1785, 0x12, 0x800991, 0x511cf8
  .zero 4
  .word 0xfffffdfc, 0xffffabbc, 0x158, 0x13, 0xa00999, 0x511cf8
  .zero 8
  .word 0xffffbd12, 0x89, 0x14, 0xb00980, 0x511cf8
  .zero 4
  .word 0x19f0, 0x1ec7, 0xa7b, 0x15, 0xd00990, 0x511cf8
  .zero 8
  .word 0xffff845b, 0x35c, 0x16, 0xf00998, 0x511cf8
  .zero 4
  .word 0x10ef, 0xffff7fc9, 0xcc4, 0x17, 0xc00993, 0x511cf8
  .zero 4
  .word 0xffffe610, 0x1ec7, 0xa7b, 0x18, 0xe00991, 0x511cf8
  .zero 8
  .word 0xffff845b, 0x35c, 0x19, 0x1000999, 0x511cf8
  .zero 4
  .word 0xffffef11, 0xffff7fc9, 0xcc4, 0x1a, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x4, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x5, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x6, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x7, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x8, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x9, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xa, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xb, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xc, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xd, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0xe

sonya_cut_alternate_rig:
  .word 0x400994, 0x511cf8
  .zero 8
  .word 0xfffee063, 0x9fb
  .zero 4
  .word 0xfff00890, 0x511cf8
  .zero 8
  .word 0xffffeb4e, 0xfffff585, 0x1b, 0x300980, 0x511cf8
  .zero 16
  .word 0x1c, 0x200980, 0x511cf8
  .zero 8
  .word 0xffff763e, 0xd70, 0x1d, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x1e, 0x500980, 0x511cf8
  .zero 4
  .word 0x2b02, 0xffffa3ce, 0x1785, 0x2a, 0x700990, 0x511cf8
  .zero 4
  .word 0x204, 0xffffabbc, 0x158, 0x2b, 0x900998, 0x511cf8
  .zero 8
  .word 0xffffbd12, 0x89, 0x2c, 0x600993, 0x511cf8
  .zero 4
  .word 0xffffd4fe, 0xffffa3ce, 0x1785, 0x2d, 0x800991, 0x511cf8
  .zero 4
  .word 0xfffffdfc, 0xffffabbc, 0x158, 0x2e, 0xa00999, 0x511cf8
  .zero 8
  .word 0xffffbd12, 0x89, 0x2f, 0xb00980, 0x511cf8
  .zero 4
  .word 0x19f0, 0x1ec7, 0xa7b, 0x30, 0xd00990, 0x511cf8
  .zero 8
  .word 0xffff845b, 0x35c, 0x31, 0xf00998, 0x511cf8
  .zero 4
  .word 0x10ef, 0xffff7fc9, 0xcc4, 0x32, 0xc00993, 0x511cf8
  .zero 4
  .word 0xffffe610, 0x1ec7, 0xa7b, 0x33, 0xe00991, 0x511cf8
  .zero 8
  .word 0xffff845b, 0x35c, 0x34, 0x1000999, 0x511cf8
  .zero 4
  .word 0xffffef11, 0xffff7fc9, 0xcc4, 0x35, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x1f, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x20, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x21, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x22, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x23, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x24, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x25, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x26, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x27, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x28, 0x1100898, 0x511cf8
  .zero 8
  .word 0xffffefe0, 0x18dd, 0x29

; liu_kang: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
liu_kang_body_rig:
  .word 0x400994, asset_slot_lk_geo_geo, 0, 0, 0xfffeed50, 0, 0  ; node ID 4, depth 0

liu_kang_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_lk_geo_geo, 0, 0, 0xffffe08d, 0, 0  ; node ID 4095, depth 1

liu_kang_body_rig_joint_3:
  .word 0x300980, asset_slot_lk_geo_geo, 0, 0, 0xfffff7ab, 0xfffffca9, 0x1  ; node ID 3, depth 2

liu_kang_body_rig_joint_2:
  .word 0x200980, asset_slot_lk_geo_geo, 0, 0, 0xffff6a48, 0x144b, 0x2  ; node ID 2, depth 3

liu_kang_body_rig_joint_4094:
  .word 0xffe00898, asset_slot_lk_geo_geo, 0, 0, 0, 0, 0x3  ; node ID 4094, depth 4

liu_kang_body_rig_joint_5:
  .word 0x500980, asset_slot_lk_geo_geo, 0, 0x3895, 0xffffa92e, 0x1428, 0x4  ; node ID 5, depth 3

liu_kang_body_rig_joint_7:
  .word 0x700990, asset_slot_lk_geo_geo, 0, 0x89, 0xffffaf3a, 0x35c, 0x5  ; node ID 7, depth 4

liu_kang_body_rig_joint_9:
  .word 0x900998, asset_slot_lk_geo_geo, 0, 0x158, 0xffffb1c8, 0xfffffe86, 0x6  ; node ID 9, depth 5

liu_kang_body_rig_joint_6:
  .word 0x600993, asset_slot_lk_geo_geo, 0, 0xffffc76b, 0xffffa92e, 0x1428, 0x7  ; node ID 6, depth 3

liu_kang_body_rig_joint_8:
  .word 0x800991, asset_slot_lk_geo_geo, 0, 0xffffff77, 0xffffaf3a, 0x35c, 0x8  ; node ID 8, depth 4

liu_kang_body_rig_joint_10:
  .word 0xa00999, asset_slot_lk_geo_geo, 0, 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x9  ; node ID 10, depth 5

liu_kang_body_rig_joint_11:
  .word 0xb00980, asset_slot_lk_geo_geo, 0, 0x1bf4, 0x24f5, 0xac, 0xa  ; node ID 11, depth 2

liu_kang_body_rig_joint_13:
  .word 0xd00990, asset_slot_lk_geo_geo, 0, 0xfffffe3f, 0xffff8bcc, 0x1a4, 0xb  ; node ID 13, depth 3

liu_kang_body_rig_joint_15:
  .word 0xf00998, asset_slot_lk_geo_geo, 0, 0xb27, 0xffff7b9e, 0x968, 0xc  ; node ID 15, depth 4

liu_kang_body_rig_joint_12:
  .word 0xc00993, asset_slot_lk_geo_geo, 0, 0xffffe40c, 0x24f5, 0xac, 0xd  ; node ID 12, depth 2

liu_kang_body_rig_joint_14:
  .word 0xe00991, asset_slot_lk_geo_geo, 0, 0x1c1, 0xffff8bcc, 0x1a4, 0xe  ; node ID 14, depth 3

liu_kang_body_rig_joint_16:
  .word 0x1000999, asset_slot_lk_geo_geo, 0, 0xfffff4d9, 0xffff7b9e, 0x968, 0xf  ; node ID 16, depth 4

; liu_kang: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
liu_kang_cut_rig:
  .word 0x400994, asset_slot_lk_geo_geo, 0, 0, 0xfffeed50, 0, 0  ; node ID 4, depth 0

liu_kang_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_lk_geo_geo, 0, 0, 0xffffe08d, 0, 0x10  ; node ID 4095, depth 1

liu_kang_cut_rig_joint_3:
  .word 0x300980, asset_slot_lk_geo_geo, 0, 0, 0xfffff7ab, 0xfffffca9, 0x11  ; node ID 3, depth 2

liu_kang_cut_rig_joint_2:
  .word 0x200980, asset_slot_lk_geo_geo, 0, 0, 0xffff6a48, 0x144b, 0x12  ; node ID 2, depth 3

liu_kang_cut_rig_joint_4094:
  .word 0xffe00898, asset_slot_lk_geo_geo, 0, 0, 0, 0, 0x13  ; node ID 4094, depth 4

liu_kang_cut_rig_joint_5:
  .word 0x500980, asset_slot_lk_geo_geo, 0, 0x3895, 0xffffa92e, 0x1428, 0x14  ; node ID 5, depth 3

liu_kang_cut_rig_joint_7:
  .word 0x700990, asset_slot_lk_geo_geo, 0, 0x89, 0xffffaf3a, 0x35c, 0x15  ; node ID 7, depth 4

liu_kang_cut_rig_joint_9:
  .word 0x900998, asset_slot_lk_geo_geo, 0, 0x158, 0xffffb1c8, 0xfffffe86, 0x16  ; node ID 9, depth 5

liu_kang_cut_rig_joint_6:
  .word 0x600993, asset_slot_lk_geo_geo, 0, 0xffffc76b, 0xffffa92e, 0x1428, 0x17  ; node ID 6, depth 3

liu_kang_cut_rig_joint_8:
  .word 0x800991, asset_slot_lk_geo_geo, 0, 0xffffff77, 0xffffaf3a, 0x35c, 0x18  ; node ID 8, depth 4

liu_kang_cut_rig_joint_10:
  .word 0xa00999, asset_slot_lk_geo_geo, 0, 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x19  ; node ID 10, depth 5

liu_kang_cut_rig_joint_11:
  .word 0xb00980, asset_slot_lk_geo_geo, 0, 0x1bf4, 0x24f5, 0xac, 0x1a  ; node ID 11, depth 2

liu_kang_cut_rig_joint_13:
  .word 0xd00990, asset_slot_lk_geo_geo, 0, 0xfffffe3f, 0xffff8bcc, 0x1a4, 0x1b  ; node ID 13, depth 3

liu_kang_cut_rig_joint_15:
  .word 0xf00998, asset_slot_lk_geo_geo, 0, 0xb27, 0xffff7b9e, 0x968, 0x1c  ; node ID 15, depth 4

liu_kang_cut_rig_joint_12:
  .word 0xc00993, asset_slot_lk_geo_geo, 0, 0xffffe40c, 0x24f5, 0xac, 0x1d  ; node ID 12, depth 2

liu_kang_cut_rig_joint_14:
  .word 0xe00991, asset_slot_lk_geo_geo, 0, 0x1c1, 0xffff8bcc, 0x1a4, 0x1e  ; node ID 14, depth 3

liu_kang_cut_rig_joint_16:
  .word 0x1000999, asset_slot_lk_geo_geo, 0, 0xfffff4d9, 0xffff7b9e, 0x968, 0x1f  ; node ID 16, depth 4

liu_kang_body_alternate_rig:
  .word 0x400994, 0x511d10
  .zero 8
  .word 0xfffeed50
  .zero 8
  .word 0xfff00890, 0x511d10
  .zero 8
  .word 0xffffe08d
  .zero 8
  .word 0x300980, 0x511d10
  .zero 8
  .word 0xfffff7ab, 0xfffffca9, 0x1, 0x200980, 0x511d10
  .zero 8
  .word 0xffff6a48, 0x144b, 0x2, 0xffe00898, 0x511d10
  .zero 16
  .word 0x3, 0x500980, 0x511d10
  .zero 4
  .word 0x3895, 0xffffa92e, 0x1428, 0x4, 0x700990, 0x511d10
  .zero 4
  .word 0x89, 0xffffaf3a, 0x35c, 0x5, 0x900998, 0x511d10
  .zero 4
  .word 0x158, 0xffffb1c8, 0xfffffe86, 0x6, 0x600993, 0x511d10
  .zero 4
  .word 0xffffc76b, 0xffffa92e, 0x1428, 0x7, 0x800991, 0x511d10
  .zero 4
  .word 0xffffff77, 0xffffaf3a, 0x35c, 0x8, 0xa00999, 0x511d10
  .zero 4
  .word 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x9, 0xb00980, 0x511d10
  .zero 4
  .word 0x1bf4, 0x24f5, 0xac, 0xa, 0xd00990, 0x511d10
  .zero 4
  .word 0xfffffe3f, 0xffff8bcc, 0x1a4, 0xb, 0xf00998, 0x511d10
  .zero 4
  .word 0xb27, 0xffff7b9e, 0x968, 0xc, 0xc00993, 0x511d10
  .zero 4
  .word 0xffffe40c, 0x24f5, 0xac, 0xd, 0xe00991, 0x511d10
  .zero 4
  .word 0x1c1, 0xffff8bcc, 0x1a4, 0xe, 0x1000999, 0x511d10
  .zero 4
  .word 0xfffff4d9, 0xffff7b9e, 0x968, 0xf

liu_kang_cut_alternate_rig:
  .word 0x400994, 0x511d10
  .zero 8
  .word 0xfffeed50
  .zero 8
  .word 0xfff00890, 0x511d10
  .zero 8
  .word 0xffffe08d
  .zero 4
  .word 0x10, 0x300980, 0x511d10
  .zero 8
  .word 0xfffff7ab, 0xfffffca9, 0x11, 0x200980, 0x511d10
  .zero 8
  .word 0xffff6a48, 0x144b, 0x12, 0xffe00898, 0x511d10
  .zero 16
  .word 0x13, 0x500980, 0x511d10
  .zero 4
  .word 0x3895, 0xffffa92e, 0x1428, 0x14, 0x700990, 0x511d10
  .zero 4
  .word 0x89, 0xffffaf3a, 0x35c, 0x15, 0x900998, 0x511d10
  .zero 4
  .word 0x158, 0xffffb1c8, 0xfffffe86, 0x16, 0x600993, 0x511d10
  .zero 4
  .word 0xffffc76b, 0xffffa92e, 0x1428, 0x17, 0x800991, 0x511d10
  .zero 4
  .word 0xffffff77, 0xffffaf3a, 0x35c, 0x18, 0xa00999, 0x511d10
  .zero 4
  .word 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x19, 0xb00980, 0x511d10
  .zero 4
  .word 0x1bf4, 0x24f5, 0xac, 0x1a, 0xd00990, 0x511d10
  .zero 4
  .word 0xfffffe3f, 0xffff8bcc, 0x1a4, 0x1b, 0xf00998, 0x511d10
  .zero 4
  .word 0xb27, 0xffff7b9e, 0x968, 0x1c, 0xc00993, 0x511d10
  .zero 4
  .word 0xffffe40c, 0x24f5, 0xac, 0x1d, 0xe00991, 0x511d10
  .zero 4
  .word 0x1c1, 0xffff8bcc, 0x1a4, 0x1e, 0x1000999, 0x511d10
  .zero 4
  .word 0xfffff4d9, 0xffff7b9e, 0x968, 0x1f

; sub_zero: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
sub_zero_body_rig:
  .word 0x400994, spine_asset_slot, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

sub_zero_body_rig_joint_4095:
  .word 0xfff00890, spine_asset_slot, 0, 0, 0xffffe20d, 0, 0  ; node ID 4095, depth 1

sub_zero_body_rig_joint_3:
  .word 0x300980, spine_asset_slot, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

sub_zero_body_rig_joint_2:
  .word 0x200988, spine_asset_slot, 0, 0, 0xffff75e3, 0x164f, 0x2  ; node ID 2, depth 3

sub_zero_body_rig_joint_5:
  .word 0x500980, spine_asset_slot, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x3  ; node ID 5, depth 3

sub_zero_body_rig_joint_7:
  .word 0x700990, spine_asset_slot, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x4  ; node ID 7, depth 4

sub_zero_body_rig_joint_9:
  .word 0x900998, spine_asset_slot, 0, 0x45a, 0xffffbf5d, 0, 0x5  ; node ID 9, depth 5

sub_zero_body_rig_joint_6:
  .word 0x600993, spine_asset_slot, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x6  ; node ID 6, depth 3

sub_zero_body_rig_joint_8:
  .word 0x800991, spine_asset_slot, 0, 0x831, 0xffffaf9e, 0, 0x7  ; node ID 8, depth 4

sub_zero_body_rig_joint_10:
  .word 0xa00999, spine_asset_slot, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x8  ; node ID 10, depth 5

sub_zero_body_rig_joint_4094:
  .word 0xffe00888, spine_asset_slot, 0, 0, 0, 0, 0x9  ; node ID 4094, depth 2

sub_zero_body_rig_joint_11:
  .word 0xb00980, spine_asset_slot, 0, 0x16f9, 0x336d, 0xffffff09, 0xa  ; node ID 11, depth 2

sub_zero_body_rig_joint_13:
  .word 0xd00990, spine_asset_slot, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0xb  ; node ID 13, depth 3

sub_zero_body_rig_joint_15:
  .word 0xf00988, spine_asset_slot, 0, 0x11eb, 0xffff7e57, 0x70a, 0xc  ; node ID 15, depth 4

sub_zero_body_rig_joint_4092:
  .word 0xffc00898, spine_asset_slot, 0, 0, 0, 0, 0xd  ; node ID 4092, depth 4

sub_zero_body_rig_joint_12:
  .word 0xc00993, spine_asset_slot, 0, 0xffffe903, 0x336e, 0xffffff07, 0xe  ; node ID 12, depth 2

sub_zero_body_rig_joint_14:
  .word 0xe00991, spine_asset_slot, 0, 0x737, 0xffff886b, 0x5cb, 0xf  ; node ID 14, depth 3

sub_zero_body_rig_joint_16:
  .word 0x1000989, spine_asset_slot, 0, 0xffffee15, 0xffff7e57, 0x70a, 0x10  ; node ID 16, depth 4

sub_zero_body_rig_joint_4093:
  .word 0xffd00899, spine_asset_slot, 0, 0, 0, 0, 0x11  ; node ID 4093, depth 4

; sub_zero: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
sub_zero_cut_rig:
  .word 0x400994, spine_asset_slot, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

sub_zero_cut_rig_joint_4095:
  .word 0xfff00890, spine_asset_slot, 0, 0, 0xffffe20d, 0, 0x12  ; node ID 4095, depth 1

sub_zero_cut_rig_joint_3:
  .word 0x300980, spine_asset_slot, 0, 0, 0, 0, 0x13  ; node ID 3, depth 2

sub_zero_cut_rig_joint_2:
  .word 0x200988, spine_asset_slot, 0, 0, 0xffff75e3, 0x164f, 0x14  ; node ID 2, depth 3

sub_zero_cut_rig_joint_5:
  .word 0x500980, spine_asset_slot, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x15  ; node ID 5, depth 3

sub_zero_cut_rig_joint_7:
  .word 0x700990, spine_asset_slot, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x16  ; node ID 7, depth 4

sub_zero_cut_rig_joint_9:
  .word 0x900998, spine_asset_slot, 0, 0x45a, 0xffffbf5d, 0, 0x17  ; node ID 9, depth 5

sub_zero_cut_rig_joint_6:
  .word 0x600993, spine_asset_slot, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x18  ; node ID 6, depth 3

sub_zero_cut_rig_joint_8:
  .word 0x800991, spine_asset_slot, 0, 0x831, 0xffffaf9e, 0, 0x19  ; node ID 8, depth 4

sub_zero_cut_rig_joint_10:
  .word 0xa00999, spine_asset_slot, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x1a  ; node ID 10, depth 5

sub_zero_cut_rig_joint_4094:
  .word 0xffe00888, spine_asset_slot, 0, 0, 0, 0, 0x1b  ; node ID 4094, depth 2

sub_zero_cut_rig_joint_11:
  .word 0xb00980, spine_asset_slot, 0, 0x16f9, 0x336d, 0xffffff09, 0x1c  ; node ID 11, depth 2

sub_zero_cut_rig_joint_13:
  .word 0xd00990, spine_asset_slot, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0x1d  ; node ID 13, depth 3

sub_zero_cut_rig_joint_15:
  .word 0xf00988, spine_asset_slot, 0, 0x11eb, 0xffff7e57, 0x70a, 0x1e  ; node ID 15, depth 4

sub_zero_cut_rig_joint_4092:
  .word 0xffc00898, spine_asset_slot, 0, 0, 0, 0, 0x1f  ; node ID 4092, depth 4

sub_zero_cut_rig_joint_12:
  .word 0xc00993, spine_asset_slot, 0, 0xffffe903, 0x336e, 0xffffff07, 0x20  ; node ID 12, depth 2

sub_zero_cut_rig_joint_14:
  .word 0xe00991, spine_asset_slot, 0, 0x737, 0xffff886b, 0x5cb, 0x21  ; node ID 14, depth 3

sub_zero_cut_rig_joint_16:
  .word 0x1000989, spine_asset_slot, 0, 0xffffee15, 0xffff7e57, 0x70a, 0x22  ; node ID 16, depth 4

sub_zero_cut_rig_joint_4093:
  .word 0xffd00899, spine_asset_slot, 0, 0, 0, 0, 0x23  ; node ID 4093, depth 4

spine_rig:
  .word 0x200990, spine_asset_slot, 0, 0, 0xc4, 0, 0x24  ; node ID 2, depth 0

spine_rig_joint_3:
  .word 0x300990, spine_asset_slot, 0, 0, 0x21eb, 0x24d, 0x25  ; node ID 3, depth 1

spine_rig_joint_4:
  .word 0x400990, spine_asset_slot, 0, 0, 0x226e, 0x24d, 0x26  ; node ID 4, depth 2

spine_rig_joint_5:
  .word 0x500990, spine_asset_slot, 0, 0, 0x226e, 0xfffffdb3, 0x27  ; node ID 5, depth 3

spine_rig_joint_6:
  .word 0x600998, spine_asset_slot, 0, 0, 0x22f1, 0xfffffd30, 0x28  ; node ID 6, depth 4

sub_zero_body_alternate_rig:
  .word 0x400994, 0x511d28
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d28
  .zero 8
  .word 0xffffe20d
  .zero 8
  .word 0x300980, 0x511d28
  .zero 16
  .word 0x1, 0x200988, 0x511d28
  .zero 8
  .word 0xffff75e3, 0x164f, 0x2, 0x500980, 0x511d28
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x3, 0x700990, 0x511d28
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x4, 0x900998, 0x511d28
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x5, 0x600993, 0x511d28
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x6, 0x800991, 0x511d28
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x7, 0xa00999, 0x511d28
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x8, 0xffe00888, 0x511d28
  .zero 16
  .word 0x9, 0xb00980, 0x511d28
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0xa, 0xd00990, 0x511d28
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0xb, 0xf00988, 0x511d28
  .zero 4
  .word 0x11eb, 0xffff7e57, 0x70a, 0xc, 0xffc00898, 0x511d28
  .zero 4
  .word 0xffffffab
  .zero 4
  .word 0x8b, 0xd, 0xc00993, 0x511d28
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0xe, 0xe00991, 0x511d28
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0xf, 0x1000989, 0x511d28
  .zero 4
  .word 0xffffee15, 0xffff7e57, 0x70a, 0x10, 0xffd00899, 0x511d28
  .zero 16
  .word 0x11

sub_zero_cut_alternate_rig:
  .word 0x400994, 0x511d28
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d28
  .zero 8
  .word 0xffffe20d
  .zero 4
  .word 0x12, 0x300980, 0x511d28
  .zero 16
  .word 0x13, 0x200988, 0x511d28
  .zero 8
  .word 0xffff75e3, 0x164f, 0x14, 0x500980, 0x511d28
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x15, 0x700990, 0x511d28
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x16, 0x900998, 0x511d28
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x17, 0x600993, 0x511d28
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x18, 0x800991, 0x511d28
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x19, 0xa00999, 0x511d28
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x1a, 0xffe00888, 0x511d28
  .zero 16
  .word 0x1b, 0xb00980, 0x511d28
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0x1c, 0xd00990, 0x511d28
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0x1d, 0xf00988, 0x511d28
  .zero 4
  .word 0x11eb, 0xffff7e57, 0x70a, 0x1e, 0xffc00898, 0x511d28
  .zero 16
  .word 0x1f, 0xc00993, 0x511d28
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0x20, 0xe00991, 0x511d28
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0x21, 0x1000989, 0x511d28
  .zero 4
  .word 0xffffee15, 0xffff7e57, 0x70a, 0x22, 0xffd00899, 0x511d28
  .zero 16
  .word 0x23, 0x200990, 0x511d28
  .zero 8
  .word 0xc4
  .zero 4
  .word 0x24, 0x300990, 0x511d28
  .zero 8
  .word 0x21eb, 0x24d, 0x25, 0x400990, 0x511d28
  .zero 8
  .word 0x226e, 0x24d, 0x26, 0x500990, 0x511d28
  .zero 8
  .word 0x226e, 0xfffffdb3, 0x27, 0x600998, 0x511d28
  .zero 8
  .word 0x22f1, 0xfffffd30, 0x28

; fujin: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
fujin_body_rig:
  .word 0x400994, asset_slot_wg_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

fujin_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_wg_geo_geo, 0, 0, 0xffffe1ec, 0, 0  ; node ID 4095, depth 1

fujin_body_rig_joint_3:
  .word 0x300980, asset_slot_wg_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

fujin_body_rig_joint_2:
  .word 0x200980, asset_slot_wg_geo_geo, 0, 0, 0xffff6a5f, 0x153f, 0x2  ; node ID 2, depth 3

fujin_body_rig_joint_17:
  .word 0x1100898, asset_slot_wg_geo_geo, 0, 0, 0, 0, 0x3  ; node ID 17, depth 4

fujin_body_rig_joint_5:
  .word 0x500980, asset_slot_wg_geo_geo, 0, 0x3ed9, 0xffffa8d5, 0x170a, 0xf  ; node ID 5, depth 3

fujin_body_rig_joint_7:
  .word 0x700990, asset_slot_wg_geo_geo, 0, 0x83, 0xffffb26f, 0x47a, 0x10  ; node ID 7, depth 4

fujin_body_rig_joint_9:
  .word 0x900998, asset_slot_wg_geo_geo, 0, 0x28f, 0xffffb582, 0xfffffe98, 0x11  ; node ID 9, depth 5

fujin_body_rig_joint_6:
  .word 0x600993, asset_slot_wg_geo_geo, 0, 0xffffc127, 0xffffa8d5, 0x170a, 0x12  ; node ID 6, depth 3

fujin_body_rig_joint_8:
  .word 0x800991, asset_slot_wg_geo_geo, 0, 0xffffff7d, 0xffffb26f, 0x47a, 0x13  ; node ID 8, depth 4

fujin_body_rig_joint_10:
  .word 0xa00999, asset_slot_wg_geo_geo, 0, 0xfffffd71, 0xffffb582, 0xfffffe98, 0x14  ; node ID 10, depth 5

fujin_body_rig_joint_11:
  .word 0xb00980, asset_slot_wg_geo_geo, 0, 0x1a9f, 0x2e14, 0xa3, 0x15  ; node ID 11, depth 2

fujin_body_rig_joint_13:
  .word 0xd00990, asset_slot_wg_geo_geo, 0, 0xfffffe57, 0xffff8d6b, 0x566, 0x16  ; node ID 13, depth 3

fujin_body_rig_joint_15:
  .word 0xf00998, asset_slot_wg_geo_geo, 0, 0xbe7, 0xffff7852, 0x8f5, 0x17  ; node ID 15, depth 4

fujin_body_rig_joint_12:
  .word 0xc00993, asset_slot_wg_geo_geo, 0, 0xffffe561, 0x2e14, 0xa3, 0x18  ; node ID 12, depth 2

fujin_body_rig_joint_14:
  .word 0xe00991, asset_slot_wg_geo_geo, 0, 0x1a9, 0xffff8d6b, 0x566, 0x19  ; node ID 14, depth 3

fujin_body_rig_joint_16:
  .word 0x1000999, asset_slot_wg_geo_geo, 0, 0xfffff419, 0xffff7852, 0x8f5, 0x1a  ; node ID 16, depth 4
  .word 0x1100898, 0x511d34
  .zero 16
  .word 0x4, 0x1100898, 0x511d34
  .zero 16
  .word 0x5, 0x1100898, 0x511d34
  .zero 16
  .word 0x6, 0x1100898, 0x511d34
  .zero 16
  .word 0x7, 0x1100898, 0x511d34
  .zero 16
  .word 0x8, 0x1100898, 0x511d34
  .zero 16
  .word 0x9, 0x1100898, 0x511d34
  .zero 16
  .word 0xa, 0x1100898, 0x511d34
  .zero 16
  .word 0xb, 0x1100898, 0x511d34
  .zero 16
  .word 0xc, 0x1100898, 0x511d34
  .zero 16
  .word 0xd, 0x1100898, 0x511d34
  .zero 16
  .word 0xe

; fujin: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
fujin_cut_rig:
  .word 0x400994, asset_slot_wg_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

fujin_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_wg_geo_geo, 0, 0, 0xffffe1ec, 0, 0x1b  ; node ID 4095, depth 1

fujin_cut_rig_joint_3:
  .word 0x300980, asset_slot_wg_geo_geo, 0, 0, 0, 0, 0x1c  ; node ID 3, depth 2

fujin_cut_rig_joint_2:
  .word 0x200980, asset_slot_wg_geo_geo, 0, 0, 0xffff6a5f, 0x153f, 0x1d  ; node ID 2, depth 3

fujin_cut_rig_joint_17:
  .word 0x1100898, asset_slot_wg_geo_geo, 0, 0, 0, 0, 0x1e  ; node ID 17, depth 4

fujin_cut_rig_joint_5:
  .word 0x500980, asset_slot_wg_geo_geo, 0, 0x3ed9, 0xffffa8d5, 0x170a, 0x2a  ; node ID 5, depth 3

fujin_cut_rig_joint_7:
  .word 0x700990, asset_slot_wg_geo_geo, 0, 0x83, 0xffffb26f, 0x47a, 0x2b  ; node ID 7, depth 4

fujin_cut_rig_joint_9:
  .word 0x900998, asset_slot_wg_geo_geo, 0, 0x28f, 0xffffb582, 0xfffffe98, 0x2c  ; node ID 9, depth 5

fujin_cut_rig_joint_6:
  .word 0x600993, asset_slot_wg_geo_geo, 0, 0xffffc127, 0xffffa8d5, 0x170a, 0x2d  ; node ID 6, depth 3

fujin_cut_rig_joint_8:
  .word 0x800991, asset_slot_wg_geo_geo, 0, 0xffffff7d, 0xffffb26f, 0x47a, 0x2e  ; node ID 8, depth 4

fujin_cut_rig_joint_10:
  .word 0xa00999, asset_slot_wg_geo_geo, 0, 0xfffffd71, 0xffffb582, 0xfffffe98, 0x2f  ; node ID 10, depth 5

fujin_cut_rig_joint_11:
  .word 0xb00980, asset_slot_wg_geo_geo, 0, 0x1a9f, 0x2e14, 0xa3, 0x30  ; node ID 11, depth 2

fujin_cut_rig_joint_13:
  .word 0xd00990, asset_slot_wg_geo_geo, 0, 0xfffffe57, 0xffff8d6b, 0x566, 0x31  ; node ID 13, depth 3

fujin_cut_rig_joint_15:
  .word 0xf00998, asset_slot_wg_geo_geo, 0, 0xbe7, 0xffff7852, 0x8f5, 0x32  ; node ID 15, depth 4

fujin_cut_rig_joint_12:
  .word 0xc00993, asset_slot_wg_geo_geo, 0, 0xffffe561, 0x2e14, 0xa3, 0x33  ; node ID 12, depth 2

fujin_cut_rig_joint_14:
  .word 0xe00991, asset_slot_wg_geo_geo, 0, 0x1a9, 0xffff8d6b, 0x566, 0x34  ; node ID 14, depth 3

fujin_cut_rig_joint_16:
  .word 0x1000999, asset_slot_wg_geo_geo, 0, 0xfffff419, 0xffff7852, 0x8f5, 0x35  ; node ID 16, depth 4
  .word 0x1100898, 0x511d34
  .zero 16
  .word 0x1f, 0x1100898, 0x511d34
  .zero 16
  .word 0x20, 0x1100898, 0x511d34
  .zero 16
  .word 0x21, 0x1100898, 0x511d34
  .zero 16
  .word 0x22, 0x1100898, 0x511d34
  .zero 16
  .word 0x23, 0x1100898, 0x511d34
  .zero 16
  .word 0x24, 0x1100898, 0x511d34
  .zero 16
  .word 0x25, 0x1100898, 0x511d34
  .zero 16
  .word 0x26, 0x1100898, 0x511d34
  .zero 16
  .word 0x27, 0x1100898, 0x511d34
  .zero 16
  .word 0x28, 0x1100898, 0x511d34
  .zero 16
  .word 0x29

fujin_body_alternate_rig:
  .word 0x400994, 0x511d40
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x511d40
  .zero 8
  .word 0xffffe1ec
  .zero 8
  .word 0x300980, 0x511d40
  .zero 16
  .word 0x1, 0x200980, 0x511d40
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x2, 0x1100898, 0x511d40
  .zero 16
  .word 0x3, 0x500980, 0x511d40
  .zero 4
  .word 0x3ed9, 0xffffa8d5, 0x170a, 0xf, 0x700990, 0x511d40
  .zero 4
  .word 0x83, 0xffffb26f, 0x47a, 0x10, 0x900998, 0x511d40
  .zero 4
  .word 0x28f, 0xffffb582, 0xfffffe98, 0x11, 0x600993, 0x511d40
  .zero 4
  .word 0xffffc127, 0xffffa8d5, 0x170a, 0x12, 0x800991, 0x511d40
  .zero 4
  .word 0xffffff7d, 0xffffb26f, 0x47a, 0x13, 0xa00999, 0x511d40
  .zero 4
  .word 0xfffffd71, 0xffffb582, 0xfffffe98, 0x14, 0xb00980, 0x511d40
  .zero 4
  .word 0x1a9f, 0x2e14, 0xa3, 0x15, 0xd00990, 0x511d40
  .zero 4
  .word 0xfffffe57, 0xffff8d6b, 0x566, 0x16, 0xf00998, 0x511d40
  .zero 4
  .word 0xbe7, 0xffff7852, 0x8f5, 0x17, 0xc00993, 0x511d40
  .zero 4
  .word 0xffffe561, 0x2e14, 0xa3, 0x18, 0xe00991, 0x511d40
  .zero 4
  .word 0x1a9, 0xffff8d6b, 0x566, 0x19, 0x1000999, 0x511d40
  .zero 4
  .word 0xfffff419, 0xffff7852, 0x8f5, 0x1a, 0x1100898, 0x511d40
  .zero 16
  .word 0x4, 0x1100898, 0x511d40
  .zero 16
  .word 0x5, 0x1100898, 0x511d40
  .zero 16
  .word 0x6, 0x1100898, 0x511d40
  .zero 16
  .word 0x7, 0x1100898, 0x511d40
  .zero 16
  .word 0x8, 0x1100898, 0x511d40
  .zero 16
  .word 0x9, 0x1100898, 0x511d40
  .zero 16
  .word 0xa, 0x1100898, 0x511d40
  .zero 16
  .word 0xb, 0x1100898, 0x511d40
  .zero 16
  .word 0xc, 0x1100898, 0x511d40
  .zero 16
  .word 0xd, 0x1100898, 0x511d40
  .zero 16
  .word 0xe

fujin_cut_alternate_rig:
  .word 0x400994, 0x511d40
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x511d40
  .zero 8
  .word 0xffffe1ec
  .zero 4
  .word 0x1b, 0x300980, 0x511d40
  .zero 16
  .word 0x1c, 0x200980, 0x511d40
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x1d, 0x1100898, 0x511d40
  .zero 16
  .word 0x1e, 0x500980, 0x511d40
  .zero 4
  .word 0x3ed9, 0xffffa8d5, 0x170a, 0x2a, 0x700990, 0x511d40
  .zero 4
  .word 0x83, 0xffffb26f, 0x47a, 0x2b, 0x900998, 0x511d40
  .zero 4
  .word 0x28f, 0xffffb582, 0xfffffe98, 0x2c, 0x600993, 0x511d40
  .zero 4
  .word 0xffffc127, 0xffffa8d5, 0x170a, 0x2d, 0x800991, 0x511d40
  .zero 4
  .word 0xffffff7d, 0xffffb26f, 0x47a, 0x2e, 0xa00999, 0x511d40
  .zero 4
  .word 0xfffffd71, 0xffffb582, 0xfffffe98, 0x2f, 0xb00980, 0x511d40
  .zero 4
  .word 0x1a9f, 0x2e14, 0xa3, 0x30, 0xd00990, 0x511d40
  .zero 4
  .word 0xfffffe57, 0xffff8d6b, 0x566, 0x31, 0xf00998, 0x511d40
  .zero 4
  .word 0xbe7, 0xffff7852, 0x8f5, 0x32, 0xc00993, 0x511d40
  .zero 4
  .word 0xffffe561, 0x2e14, 0xa3, 0x33, 0xe00991, 0x511d40
  .zero 4
  .word 0x1a9, 0xffff8d6b, 0x566, 0x34, 0x1000999, 0x511d40
  .zero 4
  .word 0xfffff419, 0xffff7852, 0x8f5, 0x35, 0x1100898, 0x511d40
  .zero 16
  .word 0x1f, 0x1100898, 0x511d40
  .zero 16
  .word 0x20, 0x1100898, 0x511d40
  .zero 16
  .word 0x21, 0x1100898, 0x511d40
  .zero 16
  .word 0x22, 0x1100898, 0x511d40
  .zero 16
  .word 0x23, 0x1100898, 0x511d40
  .zero 16
  .word 0x24, 0x1100898, 0x511d40
  .zero 16
  .word 0x25, 0x1100898, 0x511d40
  .zero 16
  .word 0x26, 0x1100898, 0x511d40
  .zero 16
  .word 0x27, 0x1100898, 0x511d40
  .zero 16
  .word 0x28, 0x1100898, 0x511d40
  .zero 16
  .word 0x29

; reiko: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
reiko_body_rig:
  .word 0x400994, asset_slot_ns_geo_geo, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

reiko_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_ns_geo_geo, 0, 0, 0xffffe20d, 0, 0  ; node ID 4095, depth 1

reiko_body_rig_joint_3:
  .word 0x300980, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

reiko_body_rig_joint_2:
  .word 0x200988, asset_slot_ns_geo_geo, 0, 0, 0xffff75e3, 0x164f, 0x2  ; node ID 2, depth 3

reiko_body_rig_joint_5:
  .word 0x500980, asset_slot_ns_geo_geo, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x3  ; node ID 5, depth 3

reiko_body_rig_joint_7:
  .word 0x700990, asset_slot_ns_geo_geo, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x4  ; node ID 7, depth 4

reiko_body_rig_joint_9:
  .word 0x900990, asset_slot_ns_geo_geo, 0, 0x45a, 0xffffbf5d, 0, 0x5  ; node ID 9, depth 5

reiko_body_rig_joint_4093:
  .word 0xffd00898, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x6  ; node ID 4093, depth 6

reiko_body_rig_joint_6:
  .word 0x600993, asset_slot_ns_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x7  ; node ID 6, depth 3

reiko_body_rig_joint_8:
  .word 0x800991, asset_slot_ns_geo_geo, 0, 0x831, 0xffffaf9e, 0, 0x8  ; node ID 8, depth 4

reiko_body_rig_joint_10:
  .word 0xa00991, asset_slot_ns_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x9  ; node ID 10, depth 5

reiko_body_rig_joint_4092:
  .word 0xffc00899, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0xa  ; node ID 4092, depth 6

reiko_body_rig_joint_4094:
  .word 0xffe00888, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0xb  ; node ID 4094, depth 2

reiko_body_rig_joint_11:
  .word 0xb00980, asset_slot_ns_geo_geo, 0, 0x16f9, 0x336d, 0xffffff09, 0xc  ; node ID 11, depth 2

reiko_body_rig_joint_13:
  .word 0xd00990, asset_slot_ns_geo_geo, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0xd  ; node ID 13, depth 3

reiko_body_rig_joint_15:
  .word 0xf00998, asset_slot_ns_geo_geo, 0, 0xeb8, 0xffff7e57, 0x70a, 0xe  ; node ID 15, depth 4

reiko_body_rig_joint_12:
  .word 0xc00993, asset_slot_ns_geo_geo, 0, 0xffffe903, 0x336e, 0xffffff07, 0xf  ; node ID 12, depth 2

reiko_body_rig_joint_14:
  .word 0xe00991, asset_slot_ns_geo_geo, 0, 0x737, 0xffff886b, 0x5cb, 0x10  ; node ID 14, depth 3

reiko_body_rig_joint_16:
  .word 0x1000999, asset_slot_ns_geo_geo, 0, 0xfffff148, 0xffff7e57, 0x70a, 0x11  ; node ID 16, depth 4

; reiko: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
reiko_cut_rig:
  .word 0x400994, asset_slot_ns_geo_geo, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

reiko_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_ns_geo_geo, 0, 0, 0xffffe20d, 0, 0x12  ; node ID 4095, depth 1

reiko_cut_rig_joint_3:
  .word 0x300980, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x13  ; node ID 3, depth 2

reiko_cut_rig_joint_2:
  .word 0x200988, asset_slot_ns_geo_geo, 0, 0, 0xffff75e3, 0x164f, 0x14  ; node ID 2, depth 3

reiko_cut_rig_joint_5:
  .word 0x500980, asset_slot_ns_geo_geo, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x15  ; node ID 5, depth 3

reiko_cut_rig_joint_7:
  .word 0x700990, asset_slot_ns_geo_geo, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x16  ; node ID 7, depth 4

reiko_cut_rig_joint_9:
  .word 0x900990, asset_slot_ns_geo_geo, 0, 0x45a, 0xffffbf5d, 0, 0x17  ; node ID 9, depth 5

reiko_cut_rig_joint_4093:
  .word 0xffd00898, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x18  ; node ID 4093, depth 6

reiko_cut_rig_joint_6:
  .word 0x600993, asset_slot_ns_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x19  ; node ID 6, depth 3

reiko_cut_rig_joint_8:
  .word 0x800991, asset_slot_ns_geo_geo, 0, 0x831, 0xffffaf9e, 0, 0x1a  ; node ID 8, depth 4

reiko_cut_rig_joint_10:
  .word 0xa00991, asset_slot_ns_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x1b  ; node ID 10, depth 5

reiko_cut_rig_joint_4092:
  .word 0xffc00899, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x1c  ; node ID 4092, depth 6

reiko_cut_rig_joint_4094:
  .word 0xffe00888, asset_slot_ns_geo_geo, 0, 0, 0, 0, 0x1d  ; node ID 4094, depth 2

reiko_cut_rig_joint_11:
  .word 0xb00980, asset_slot_ns_geo_geo, 0, 0x16f9, 0x336d, 0xffffff09, 0x1e  ; node ID 11, depth 2

reiko_cut_rig_joint_13:
  .word 0xd00990, asset_slot_ns_geo_geo, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0x1f  ; node ID 13, depth 3

reiko_cut_rig_joint_15:
  .word 0xf00998, asset_slot_ns_geo_geo, 0, 0xeb8, 0xffff7e57, 0x70a, 0x20  ; node ID 15, depth 4

reiko_cut_rig_joint_12:
  .word 0xc00993, asset_slot_ns_geo_geo, 0, 0xffffe903, 0x336e, 0xffffff07, 0x21  ; node ID 12, depth 2

reiko_cut_rig_joint_14:
  .word 0xe00991, asset_slot_ns_geo_geo, 0, 0x737, 0xffff886b, 0x5cb, 0x22  ; node ID 14, depth 3

reiko_cut_rig_joint_16:
  .word 0x1000999, asset_slot_ns_geo_geo, 0, 0xfffff148, 0xffff7e57, 0x70a, 0x23  ; node ID 16, depth 4

reiko_body_alternate_rig:
  .word 0x400994, 0x511d58
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d58
  .zero 8
  .word 0xffffe20d
  .zero 8
  .word 0x300980, 0x511d58
  .zero 16
  .word 0x1, 0x200988, 0x511d58
  .zero 8
  .word 0xffff75e3, 0x164f, 0x2, 0x500980, 0x511d58
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x3, 0x700990, 0x511d58
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x4, 0x900990, 0x511d58
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x5, 0xffd00898, 0x511d58
  .zero 16
  .word 0x6, 0x600993, 0x511d58
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x7, 0x800991, 0x511d58
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x8, 0xa00991, 0x511d58
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x9, 0xffc00899, 0x511d58
  .zero 16
  .word 0xa, 0xffe00888, 0x511d58
  .zero 16
  .word 0xb, 0xb00980, 0x511d58
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0xc, 0xd00990, 0x511d58
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0xd, 0xf00998, 0x511d58
  .zero 4
  .word 0xeb8, 0xffff7e57, 0x70a, 0xe, 0xc00993, 0x511d58
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0xf, 0xe00991, 0x511d58
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0x10, 0x1000999, 0x511d58
  .zero 4
  .word 0xfffff148, 0xffff7e57, 0x70a, 0x11

reiko_cut_alternate_rig:
  .word 0x400994, 0x511d58
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d58
  .zero 8
  .word 0xffffe20d
  .zero 4
  .word 0x12, 0x300980, 0x511d58
  .zero 16
  .word 0x13, 0x200988, 0x511d58
  .zero 8
  .word 0xffff75e3, 0x164f, 0x14, 0x500980, 0x511d58
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x15, 0x700990, 0x511d58
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x16, 0x900990, 0x511d58
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x17, 0xffd00898, 0x511d58
  .zero 16
  .word 0x18, 0x600993, 0x511d58
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x19, 0x800991, 0x511d58
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x1a, 0xa00991, 0x511d58
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x1b, 0xffc00899, 0x511d58
  .zero 16
  .word 0x1c, 0xffe00888, 0x511d58
  .zero 16
  .word 0x1d, 0xb00980, 0x511d58
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0x1e, 0xd00990, 0x511d58
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0x1f, 0xf00998, 0x511d58
  .zero 4
  .word 0xeb8, 0xffff7e57, 0x70a, 0x20, 0xc00993, 0x511d58
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0x21, 0xe00991, 0x511d58
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0x22, 0x1000999, 0x511d58
  .zero 4
  .word 0xfffff148, 0xffff7e57, 0x70a, 0x23

; noob_saibot: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
noob_saibot_body_alternate_rig:
  .word 0x400994, asset_slot_nns_geo_geo, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

noob_saibot_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_nns_geo_geo, 0, 0, 0xffffe20d, 0, 0  ; node ID 4095, depth 1

noob_saibot_body_rig_joint_3:
  .word 0x300980, asset_slot_nns_geo_geo, 0, 0x3d7, 0xfffff811, 0xa2, 0x1  ; node ID 3, depth 2

noob_saibot_body_rig_joint_2:
  .word 0x200988, asset_slot_nns_geo_geo, 0, 0, 0xffff75e3, 0x164f, 0x2  ; node ID 2, depth 3

noob_saibot_body_rig_joint_5:
  .word 0x500980, asset_slot_nns_geo_geo, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x3  ; node ID 5, depth 3

noob_saibot_body_rig_joint_7:
  .word 0x700990, asset_slot_nns_geo_geo, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x4  ; node ID 7, depth 4

noob_saibot_body_rig_joint_9:
  .word 0x900998, asset_slot_nns_geo_geo, 0, 0x45a, 0xffffbf5d, 0, 0x5  ; node ID 9, depth 5

noob_saibot_body_rig_joint_6:
  .word 0x600993, asset_slot_nns_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x6  ; node ID 6, depth 3

noob_saibot_body_rig_joint_8:
  .word 0x800991, asset_slot_nns_geo_geo, 0, 0x831, 0xffffaf9e, 0, 0x7  ; node ID 8, depth 4

noob_saibot_body_rig_joint_10:
  .word 0xa00999, asset_slot_nns_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x8  ; node ID 10, depth 5

noob_saibot_body_rig_joint_4094:
  .word 0xffe00888, asset_slot_nns_geo_geo, 0, 0, 0, 0, 0x9  ; node ID 4094, depth 2

noob_saibot_body_rig_joint_11:
  .word 0xb00980, asset_slot_nns_geo_geo, 0, 0x16f9, 0x336d, 0xffffff09, 0xa  ; node ID 11, depth 2

noob_saibot_body_rig_joint_13:
  .word 0xd00990, asset_slot_nns_geo_geo, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0xb  ; node ID 13, depth 3

noob_saibot_body_rig_joint_15:
  .word 0xf00998, asset_slot_nns_geo_geo, 0, 0xeb8, 0xffff7e57, 0x70a, 0xc  ; node ID 15, depth 4

noob_saibot_body_rig_joint_12:
  .word 0xc00993, asset_slot_nns_geo_geo, 0, 0xffffe903, 0x336e, 0xffffff07, 0xd  ; node ID 12, depth 2

noob_saibot_body_rig_joint_14:
  .word 0xe00991, asset_slot_nns_geo_geo, 0, 0x737, 0xffff886b, 0x5cb, 0xe  ; node ID 14, depth 3

noob_saibot_body_rig_joint_16:
  .word 0x1000999, asset_slot_nns_geo_geo, 0, 0xfffff148, 0xffff7e57, 0x70a, 0xf  ; node ID 16, depth 4

; noob_saibot: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
noob_saibot_cut_alternate_rig:
  .word 0x400994, asset_slot_nns_geo_geo, 0, 0, 0xfffed0a4, 0, 0  ; node ID 4, depth 0

noob_saibot_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_nns_geo_geo, 0, 0, 0xffffe20d, 0, 0x10  ; node ID 4095, depth 1

noob_saibot_cut_rig_joint_3:
  .word 0x300980, asset_slot_nns_geo_geo, 0, 0, 0, 0, 0x11  ; node ID 3, depth 2

noob_saibot_cut_rig_joint_2:
  .word 0x200988, asset_slot_nns_geo_geo, 0, 0, 0xffff75e3, 0x164f, 0x12  ; node ID 2, depth 3

noob_saibot_cut_rig_joint_5:
  .word 0x500980, asset_slot_nns_geo_geo, 0, 0x3f5c, 0xffffae1c, 0x183e, 0x13  ; node ID 5, depth 3

noob_saibot_cut_rig_joint_7:
  .word 0x700990, asset_slot_nns_geo_geo, 0, 0xfffff7cf, 0xffffaf9e, 0, 0x14  ; node ID 7, depth 4

noob_saibot_cut_rig_joint_9:
  .word 0x900998, asset_slot_nns_geo_geo, 0, 0x45a, 0xffffbf5d, 0, 0x15  ; node ID 9, depth 5

noob_saibot_cut_rig_joint_6:
  .word 0x600993, asset_slot_nns_geo_geo, 0, 0xffffc0a4, 0xffffae1c, 0x183e, 0x16  ; node ID 6, depth 3

noob_saibot_cut_rig_joint_8:
  .word 0x800991, asset_slot_nns_geo_geo, 0, 0x831, 0xffffaf9e, 0, 0x17  ; node ID 8, depth 4

noob_saibot_cut_rig_joint_10:
  .word 0xa00999, asset_slot_nns_geo_geo, 0, 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x18  ; node ID 10, depth 5

noob_saibot_cut_rig_joint_4094:
  .word 0xffe00888, asset_slot_nns_geo_geo, 0, 0, 0, 0, 0x19  ; node ID 4094, depth 2

noob_saibot_cut_rig_joint_11:
  .word 0xb00980, asset_slot_nns_geo_geo, 0, 0x16f9, 0x336d, 0xffffff09, 0x1a  ; node ID 11, depth 2

noob_saibot_cut_rig_joint_13:
  .word 0xd00990, asset_slot_nns_geo_geo, 0, 0xfffff8c8, 0xffff886d, 0x5c9, 0x1b  ; node ID 13, depth 3

noob_saibot_cut_rig_joint_15:
  .word 0xf00998, asset_slot_nns_geo_geo, 0, 0xeb8, 0xffff7e57, 0x70a, 0x1c  ; node ID 15, depth 4

noob_saibot_cut_rig_joint_12:
  .word 0xc00993, asset_slot_nns_geo_geo, 0, 0xffffe903, 0x336e, 0xffffff07, 0x1d  ; node ID 12, depth 2

noob_saibot_cut_rig_joint_14:
  .word 0xe00991, asset_slot_nns_geo_geo, 0, 0x737, 0xffff886b, 0x5cb, 0x1e  ; node ID 14, depth 3

noob_saibot_cut_rig_joint_16:
  .word 0x1000999, asset_slot_nns_geo_geo, 0, 0xfffff148, 0xffff7e57, 0x70a, 0x1f  ; node ID 16, depth 4
  .word 0x400994, 0x511d70
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d70
  .zero 8
  .word 0xffffe20d
  .zero 8
  .word 0x300980, 0x511d70
  .zero 16
  .word 0x1, 0x200988, 0x511d70
  .zero 8
  .word 0xffff75e3, 0x164f, 0x2, 0x500980, 0x511d70
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x3, 0x700990, 0x511d70
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x4, 0x900998, 0x511d70
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x5, 0x600993, 0x511d70
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x6, 0x800991, 0x511d70
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x7, 0xa00999, 0x511d70
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x8, 0xffe00888, 0x511d70
  .zero 16
  .word 0x9, 0xb00980, 0x511d70
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0xa, 0xd00990, 0x511d70
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0xb, 0xf00998, 0x511d70
  .zero 4
  .word 0x1333, 0xffff7e57, 0x47a, 0xc, 0xc00993, 0x511d70
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0xd, 0xe00991, 0x511d70
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0xe, 0x1000999, 0x511d70
  .zero 4
  .word 0xffffeccd, 0xffff7d71, 0x47a, 0xf, 0x400994, 0x511d70
  .zero 8
  .word 0xfffed0a4
  .zero 8
  .word 0xfff00890, 0x511d70
  .zero 8
  .word 0xffffe20d
  .zero 4
  .word 0x10, 0x300980, 0x511d70
  .zero 16
  .word 0x11, 0x200988, 0x511d70
  .zero 8
  .word 0xffff75e3, 0x164f, 0x12, 0x500980, 0x511d70
  .zero 4
  .word 0x3f5c, 0xffffae1c, 0x183e, 0x13, 0x700990, 0x511d70
  .zero 4
  .word 0xfffff7cf, 0xffffaf9e
  .zero 4
  .word 0x14, 0x900998, 0x511d70
  .zero 4
  .word 0x45a, 0xffffbf5d
  .zero 4
  .word 0x15, 0x600993, 0x511d70
  .zero 4
  .word 0xffffc0a4, 0xffffae1c, 0x183e, 0x16, 0x800991, 0x511d70
  .zero 4
  .word 0x831, 0xffffaf9e
  .zero 4
  .word 0x17, 0xa00999, 0x511d70
  .zero 4
  .word 0xfffffba6, 0xffffbf5d, 0xffffffe0, 0x18, 0xffe00888, 0x511d70
  .zero 16
  .word 0x19, 0xb00980, 0x511d70
  .zero 4
  .word 0x16f9, 0x336d, 0xffffff09, 0x1a, 0xd00990, 0x511d70
  .zero 4
  .word 0xfffff8c8, 0xffff886d, 0x5c9, 0x1b, 0xf00998, 0x511d70
  .zero 4
  .word 0xeb8, 0xffff7e57, 0x70a, 0x1c, 0xc00993, 0x511d70
  .zero 4
  .word 0xffffe903, 0x336e, 0xffffff07, 0x1d, 0xe00991, 0x511d70
  .zero 4
  .word 0x737, 0xffff886b, 0x5cb, 0x1e, 0x1000999, 0x511d70
  .zero 4
  .word 0xfffff148, 0xffff7e57, 0x70a, 0x1f
  .zero 272
  .word 0x3aa800
  .zero 8
  .word 0x7b7db, 0x23, 0xde800000, 0x387800
  .zero 4
  .word 0x25400000, 0x398000, 0x3d7800, 0x6025800
  .zero 4
  .word 0xfb02a000, 0xf9c0080b
  .zero 4
  .word 0xf582cffd
  .zero 20
  .word 0x23
  .zero 68
  .word 0x23
  .zero 68
  .word 0x23
  .zero 68
  .word 0x43
  .zero 32
  .word 0xff4187fb, 0xfec00000
  .zero 4
  .word 0xff4187fb
  .zero 16
  .word 0x43
  .zero 32
  .word 0xff4187fb, 0xfec00000
  .zero 4
  .word 0xff4187fb
  .zero 16
  .word 0x43
  .zero 32
  .word 0xff4187fb, 0xfec00000
  .zero 4
  .word 0xff4187fb
  .zero 16
  .word 0x43
  .zero 32
  .word 0xff4187fb, 0xfec00000
  .zero 4
  .word 0xff4187fb
  .zero 272
  .word 0x55000
  .zero 12
  .word 0xd5400000, 0x3bb800
  .zero 4
  .word 0x2a800000, 0x3bb800
  .zero 8
  .word 0xd000000
  .zero 4
  .word 0xf2017fed
  .zero 4
  .word 0xf2c00000
  .zero 4
  .word 0xf2c2c7de
  .zero 544
  .word 0x39c000
  .zero 12
  .word 0xd5400000, 0x3bb800
  .zero 8
  .word 0x2a800000, 0x3bb800
  .zero 12
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf2c00000
  .zero 4
  .word 0xf2c2c7de
  .zero 228
  .word 0x3c7000
  .zero 12
  .word 0xd5400000, 0x3bb800
  .zero 4
  .word 0x2a800000, 0x3bb800
  .zero 8
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf2c00000
  .zero 4
  .word 0xf2c2c7de
  .zero 68
  .word 0x3c7000
  .zero 12
  .word 0xd5400000, 0x3bb800
  .zero 4
  .word 0x2a800000, 0x3bb800
  .zero 8
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf2c00000
  .zero 4
  .word 0xf2c2c7de
  .zero 68
  .word 0x5166fc, 0x1, 0x110000, 0x516740, 0x1, 0x110000
  .word 0x516784, 0x1, 0x110000, 0x5167c8, 0x1, 0x110000
  .word 0x51680c, 0x1, 0x120000, 0x516854, 0x1, 0x120000
  .word 0x51689c, 0x1, 0x120000, 0x5168e4, 0x1, 0x120000
  .word 0x51692c, 0x1, 0x110000, 0x516970, 0x1, 0x110000
  .word 0x5169b4, 0x1, 0x110000, 0x5169f8, 0x1, 0x110000
  .word 0x516a3c, 0x1, 0x110000, 0x516a80, 0x1, 0x110000
  .word 0x516ac4, 0x1, 0x110000, 0x516b08, 0x1, 0x110000
  .word 0x516b4c, 0x1, 0x130000, 0x516b98, 0x1, 0x130000
  .word 0x516be4, 0x1, 0x50000, 0x516bf8, 0x1, 0x130000
  .word 0x516c44, 0x1, 0x130000, 0x516c90, 0x1, 0x50000
  .word 0x516ca4, 0x1, 0x110000, 0x516ce8, 0x1, 0x110000
  .word 0x516d2c, 0x1, 0x110000, 0x516d70, 0x1, 0x110000
  .word 0x516db4, 0x1, 0x130000, 0x516e00, 0x1, 0x130000
  .word 0x516e4c, 0x1, 0x130000, 0x516e98, 0x1, 0x130000
  .word 0x516ee4, 0x1, 0x110000, 0x516f28, 0x1, 0x110000
  .word 0x516f6c, 0x1, 0x110000, 0x516fb0, 0x1, 0x110000
  .zero 16
  .word 0xffffe953, 0xffffe624, 0xffffd0d9, 0x16b9, 0x2470, 0x222f
  .zero 4
  .word 0xffffe953, 0xffffe84a, 0xffffd401, 0x16b9, 0x255e, 0x1a9b
  .zero 4
  .word 0xffffe953, 0xffffe624, 0xffffd0d9, 0x16b9, 0x2470, 0x222f
  .zero 12
  .word 0xffffe953, 0xffffe624, 0xffffd0d9, 0x16b9, 0x2470, 0x222f
  .zero 4
  .word 0xffffe953, 0xffffe84a, 0xffffd401, 0x16b9, 0x255e, 0x1a9b
  .zero 4
  .word 0xffffe953, 0xffffe624, 0xffffd0d9, 0x16b9, 0x2470, 0x222f
  .zero 88

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_jx_geo_geo:
  .word asset_jx_geo_geo
  .zero 8
  .word 0x4f1868
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_re_geo_geo:
  .word asset_re_geo_geo
  .zero 8
  .word 0x4f18c8
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_me_geo_geo:
  .word asset_me_geo_geo
  .zero 8
  .word 0x4f1928
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_ta_geo_geo:
  .word asset_ta_geo_geo
  .zero 8
  .word 0x4f1988
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_ki_geo_geo:
  .word asset_ki_geo_geo
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_ja_geo_geo:
  .word asset_ja_geo_geo
  .zero 8
  .word 0x4f1a18
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_jc_geo_geo:
  .word asset_jc_geo_geo
  .zero 8
  .word 0x4f1a78
  .zero 8

; Asset slot; first word is a byte reference to its descriptor.
asset_slot_go_geo_geo:
  .word asset_go_geo_geo
  .zero 8

; jax: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
jax_body_rig:
  .word 0x400994, asset_slot_jx_geo_geo, 0, 0, 0xfffeda5f, 0, 0  ; node ID 4, depth 0

jax_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_jx_geo_geo, 0, 0, 0xffffe152, 0, 0  ; node ID 4095, depth 1

jax_body_rig_joint_3:
  .word 0x300980, asset_slot_jx_geo_geo, 0, 0, 0xfffff979, 0, 0x1  ; node ID 3, depth 2

jax_body_rig_joint_2:
  .word 0x200988, asset_slot_jx_geo_geo, 0, 0, 0xffff71b1, 0x1ecf, 0x2  ; node ID 2, depth 3

jax_body_rig_joint_5:
  .word 0x500980, asset_slot_jx_geo_geo, 0, 0x3ac0, 0xffffadc0, 0x1d60, 0x3  ; node ID 5, depth 3

jax_body_rig_joint_7:
  .word 0x700990, asset_slot_jx_geo_geo, 0, 0xfffff82b, 0xffffab23, 0x14e, 0x4  ; node ID 7, depth 4

jax_body_rig_joint_9:
  .word 0x900998, asset_slot_jx_geo_geo, 0, 0x29c, 0xffffb447, 0, 0x5  ; node ID 9, depth 5

jax_body_rig_joint_6:
  .word 0x600993, asset_slot_jx_geo_geo, 0, 0xffffc540, 0xffffadc0, 0x1d60, 0x6  ; node ID 6, depth 3

jax_body_rig_joint_8:
  .word 0x800991, asset_slot_jx_geo_geo, 0, 0x7d5, 0xffffab23, 0x14e, 0x7  ; node ID 8, depth 4

jax_body_rig_joint_10:
  .word 0xa00999, asset_slot_jx_geo_geo, 0, 0xfffffd64, 0xffffb447, 0, 0x8  ; node ID 10, depth 5

jax_body_rig_joint_11:
  .word 0xb00980, asset_slot_jx_geo_geo, 0, 0x1827, 0x2879, 0xbc0, 0x9  ; node ID 11, depth 2

jax_body_rig_joint_13:
  .word 0xd00990, asset_slot_jx_geo_geo, 0, 0xfffff8b1, 0xffff8589, 0x5e0, 0xa  ; node ID 13, depth 3

jax_body_rig_joint_15:
  .word 0xf00998, asset_slot_jx_geo_geo, 0, 0xf25, 0xffff76ea, 0xaf7, 0xb  ; node ID 15, depth 4

jax_body_rig_joint_12:
  .word 0xc00993, asset_slot_jx_geo_geo, 0, 0xffffe7d9, 0x2879, 0xbc0, 0xc  ; node ID 12, depth 2

jax_body_rig_joint_14:
  .word 0xe00991, asset_slot_jx_geo_geo, 0, 0x74f, 0xffff8589, 0x5e0, 0xd  ; node ID 14, depth 3

jax_body_rig_joint_16:
  .word 0x1000999, asset_slot_jx_geo_geo, 0, 0xfffff0db, 0xffff76ea, 0xaf7, 0xe  ; node ID 16, depth 4

; jax: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
jax_cut_rig:
  .word 0x400994, asset_slot_jx_geo_geo, 0, 0, 0xfffeda5f, 0, 0  ; node ID 4, depth 0

jax_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_jx_geo_geo, 0, 0, 0xffffe152, 0, 0xf  ; node ID 4095, depth 1

jax_cut_rig_joint_3:
  .word 0x300980, asset_slot_jx_geo_geo, 0, 0, 0xfffff979, 0, 0x10  ; node ID 3, depth 2

jax_cut_rig_joint_2:
  .word 0x200988, asset_slot_jx_geo_geo, 0, 0, 0xffff71b1, 0x1ecf, 0x11  ; node ID 2, depth 3

jax_cut_rig_joint_5:
  .word 0x500980, asset_slot_jx_geo_geo, 0, 0x3ac0, 0xffffadc0, 0x1d60, 0x12  ; node ID 5, depth 3

jax_cut_rig_joint_7:
  .word 0x700990, asset_slot_jx_geo_geo, 0, 0xfffff82b, 0xffffab23, 0x14e, 0x13  ; node ID 7, depth 4

jax_cut_rig_joint_9:
  .word 0x900998, asset_slot_jx_geo_geo, 0, 0x29c, 0xffffb447, 0, 0x14  ; node ID 9, depth 5

jax_cut_rig_joint_6:
  .word 0x600993, asset_slot_jx_geo_geo, 0, 0xffffc540, 0xffffadc0, 0x1d60, 0x15  ; node ID 6, depth 3

jax_cut_rig_joint_8:
  .word 0x800991, asset_slot_jx_geo_geo, 0, 0x7d5, 0xffffab23, 0x14e, 0x16  ; node ID 8, depth 4

jax_cut_rig_joint_10:
  .word 0xa00999, asset_slot_jx_geo_geo, 0, 0xfffffd64, 0xffffb447, 0, 0x17  ; node ID 10, depth 5

jax_cut_rig_joint_11:
  .word 0xb00980, asset_slot_jx_geo_geo, 0, 0x1827, 0x2879, 0xbc0, 0x18  ; node ID 11, depth 2

jax_cut_rig_joint_13:
  .word 0xd00990, asset_slot_jx_geo_geo, 0, 0xfffff8b1, 0xffff8589, 0x5e0, 0x19  ; node ID 13, depth 3

jax_cut_rig_joint_15:
  .word 0xf00998, asset_slot_jx_geo_geo, 0, 0xf25, 0xffff76ea, 0xaf7, 0x1a  ; node ID 15, depth 4

jax_cut_rig_joint_12:
  .word 0xc00993, asset_slot_jx_geo_geo, 0, 0xffffe7d9, 0x2879, 0xbc0, 0x1b  ; node ID 12, depth 2

jax_cut_rig_joint_14:
  .word 0xe00991, asset_slot_jx_geo_geo, 0, 0x74f, 0xffff8589, 0x5e0, 0x1c  ; node ID 14, depth 3

jax_cut_rig_joint_16:
  .word 0x1000999, asset_slot_jx_geo_geo, 0, 0xfffff0db, 0xffff76ea, 0xaf7, 0x1d  ; node ID 16, depth 4

jax_body_alternate_rig:
  .word 0x400994, 0x5172ac
  .zero 8
  .word 0xfffeda5f
  .zero 8
  .word 0xfff00890, 0x5172ac
  .zero 8
  .word 0xffffe152
  .zero 8
  .word 0x300980, 0x5172ac
  .zero 8
  .word 0xfffff979
  .zero 4
  .word 0x1, 0x200988, 0x5172ac
  .zero 8
  .word 0xffff71b1, 0x1ecf, 0x2, 0x500980, 0x5172ac
  .zero 4
  .word 0x3ac0, 0xffffadc0, 0x1d60, 0x3, 0x700990, 0x5172ac
  .zero 4
  .word 0xfffff82b, 0xffffab23, 0x14e, 0x4, 0x900998, 0x5172ac
  .zero 4
  .word 0x29c, 0xffffb447
  .zero 4
  .word 0x5, 0x600993, 0x5172ac
  .zero 4
  .word 0xffffc540, 0xffffadc0, 0x1d60, 0x6, 0x800991, 0x5172ac
  .zero 4
  .word 0x7d5, 0xffffab23, 0x14e, 0x7, 0xa00999, 0x5172ac
  .zero 4
  .word 0xfffffd64, 0xffffb447
  .zero 4
  .word 0x8, 0xb00980, 0x5172ac
  .zero 4
  .word 0x1827, 0x2879, 0xbc0, 0x9, 0xd00990, 0x5172ac
  .zero 4
  .word 0xfffff8b1, 0xffff8589, 0x5e0, 0xa, 0xf00998, 0x5172ac
  .zero 4
  .word 0xf25, 0xffff76ea, 0xaf7, 0xb, 0xc00993, 0x5172ac
  .zero 4
  .word 0xffffe7d9, 0x2879, 0xbc0, 0xc, 0xe00991, 0x5172ac
  .zero 4
  .word 0x74f, 0xffff8589, 0x5e0, 0xd, 0x1000999, 0x5172ac
  .zero 4
  .word 0xfffff0db, 0xffff76ea, 0xaf7, 0xe

jax_cut_alternate_rig:
  .word 0x400994, 0x5172ac
  .zero 8
  .word 0xfffeda5f
  .zero 8
  .word 0xfff00890, 0x5172ac
  .zero 8
  .word 0xffffe152
  .zero 4
  .word 0xf, 0x300980, 0x5172ac
  .zero 8
  .word 0xfffff979
  .zero 4
  .word 0x10, 0x200988, 0x5172ac
  .zero 8
  .word 0xffff71b1, 0x1ecf, 0x11, 0x500980, 0x5172ac
  .zero 4
  .word 0x3ac0, 0xffffadc0, 0x1d60, 0x12, 0x700990, 0x5172ac
  .zero 4
  .word 0xfffff82b, 0xffffab23, 0x14e, 0x13, 0x900998, 0x5172ac
  .zero 4
  .word 0x29c, 0xffffb447
  .zero 4
  .word 0x14, 0x600993, 0x5172ac
  .zero 4
  .word 0xffffc540, 0xffffadc0, 0x1d60, 0x15, 0x800991, 0x5172ac
  .zero 4
  .word 0x7d5, 0xffffab23, 0x14e, 0x16, 0xa00999, 0x5172ac
  .zero 4
  .word 0xfffffd64, 0xffffb447
  .zero 4
  .word 0x17, 0xb00980, 0x5172ac
  .zero 4
  .word 0x1827, 0x2879, 0xbc0, 0x18, 0xd00990, 0x5172ac
  .zero 4
  .word 0xfffff8b1, 0xffff8589, 0x5e0, 0x19, 0xf00998, 0x5172ac
  .zero 4
  .word 0xf25, 0xffff76ea, 0xaf7, 0x1a, 0xc00993, 0x5172ac
  .zero 4
  .word 0xffffe7d9, 0x2879, 0xbc0, 0x1b, 0xe00991, 0x5172ac
  .zero 4
  .word 0x74f, 0xffff8589, 0x5e0, 0x1c, 0x1000999, 0x5172ac
  .zero 4
  .word 0xfffff0db, 0xffff76ea, 0xaf7, 0x1d

; reptile: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
reptile_body_rig:
  .word 0x400994, asset_slot_re_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

reptile_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_re_geo_geo, 0, 0, 0xffffe1ec, 0, 0  ; node ID 4095, depth 1

reptile_body_rig_joint_3:
  .word 0x300980, asset_slot_re_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

reptile_body_rig_joint_2:
  .word 0x200988, asset_slot_re_geo_geo, 0, 0, 0xffff6a5f, 0x153f, 0x2  ; node ID 2, depth 3

reptile_body_rig_joint_5:
  .word 0x500980, asset_slot_re_geo_geo, 0, 0x3c28, 0xffffab86, 0x13d7, 0x6  ; node ID 5, depth 3

reptile_body_rig_joint_7:
  .word 0x700990, asset_slot_re_geo_geo, 0, 0, 0xffffae15, 0x51e, 0x7  ; node ID 7, depth 4

reptile_body_rig_joint_9:
  .word 0x900998, asset_slot_re_geo_geo, 0, 0x28f, 0xffffb0a4, 0, 0x8  ; node ID 9, depth 5

reptile_body_rig_joint_6:
  .word 0x600993, asset_slot_re_geo_geo, 0, 0xffffc3d8, 0xffffab86, 0x13d7, 0x9  ; node ID 6, depth 3

reptile_body_rig_joint_8:
  .word 0x800991, asset_slot_re_geo_geo, 0, 0, 0xffffae15, 0x51e, 0xa  ; node ID 8, depth 4

reptile_body_rig_joint_10:
  .word 0xa00999, asset_slot_re_geo_geo, 0, 0xfffffd71, 0xffffb0a4, 0, 0xb  ; node ID 10, depth 5

reptile_body_rig_joint_11:
  .word 0xb00980, asset_slot_re_geo_geo, 0, 0x1a3d, 0x27ae, 0xb85, 0xc  ; node ID 11, depth 2

reptile_body_rig_joint_13:
  .word 0xd00990, asset_slot_re_geo_geo, 0, 0x28f, 0xffff8667, 0x51e, 0xd  ; node ID 13, depth 3

reptile_body_rig_joint_15:
  .word 0xf00998, asset_slot_re_geo_geo, 0, 0xd70, 0xffff799a, 0x8f5, 0xe  ; node ID 15, depth 4

reptile_body_rig_joint_12:
  .word 0xc00993, asset_slot_re_geo_geo, 0, 0xffffe5c3, 0x27ae, 0xb85, 0xf  ; node ID 12, depth 2

reptile_body_rig_joint_14:
  .word 0xe00991, asset_slot_re_geo_geo, 0, 0xfffffd71, 0xffff8667, 0x51e, 0x10  ; node ID 14, depth 3

reptile_body_rig_joint_16:
  .word 0x1000999, asset_slot_re_geo_geo, 0, 0xfffff290, 0xffff799a, 0x8f5, 0x11  ; node ID 16, depth 4
  .word 0x200988, 0x5172b8
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x3, 0x200988, 0x5172b8
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x4, 0x200988, 0x5172b8
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x5

; reptile: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
reptile_cut_rig:
  .word 0x400994, asset_slot_re_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

reptile_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_re_geo_geo, 0, 0, 0xffffe1ec, 0, 0x12  ; node ID 4095, depth 1

reptile_cut_rig_joint_3:
  .word 0x300980, asset_slot_re_geo_geo, 0, 0, 0, 0, 0x13  ; node ID 3, depth 2

reptile_cut_rig_joint_2:
  .word 0x200988, asset_slot_re_geo_geo, 0, 0, 0xffff6a5f, 0x153f, 0x14  ; node ID 2, depth 3

reptile_cut_rig_joint_5:
  .word 0x500980, asset_slot_re_geo_geo, 0, 0x3c28, 0xffffab86, 0x13d7, 0x15  ; node ID 5, depth 3

reptile_cut_rig_joint_7:
  .word 0x700990, asset_slot_re_geo_geo, 0, 0, 0xffffae15, 0x51e, 0x16  ; node ID 7, depth 4

reptile_cut_rig_joint_9:
  .word 0x900998, asset_slot_re_geo_geo, 0, 0x28f, 0xffffb0a4, 0, 0x17  ; node ID 9, depth 5

reptile_cut_rig_joint_6:
  .word 0x600993, asset_slot_re_geo_geo, 0, 0xffffc3d8, 0xffffab86, 0x13d7, 0x18  ; node ID 6, depth 3

reptile_cut_rig_joint_8:
  .word 0x800991, asset_slot_re_geo_geo, 0, 0, 0xffffae15, 0x51e, 0x19  ; node ID 8, depth 4

reptile_cut_rig_joint_10:
  .word 0xa00999, asset_slot_re_geo_geo, 0, 0xfffffd71, 0xffffb0a4, 0, 0x1a  ; node ID 10, depth 5

reptile_cut_rig_joint_11:
  .word 0xb00980, asset_slot_re_geo_geo, 0, 0x1a3d, 0x27ae, 0xb85, 0x1b  ; node ID 11, depth 2

reptile_cut_rig_joint_13:
  .word 0xd00990, asset_slot_re_geo_geo, 0, 0x28f, 0xffff8667, 0x51e, 0x1c  ; node ID 13, depth 3

reptile_cut_rig_joint_15:
  .word 0xf00998, asset_slot_re_geo_geo, 0, 0xd70, 0xffff799a, 0x8f5, 0x1d  ; node ID 15, depth 4

reptile_cut_rig_joint_12:
  .word 0xc00993, asset_slot_re_geo_geo, 0, 0xffffe5c3, 0x27ae, 0xb85, 0x1e  ; node ID 12, depth 2

reptile_cut_rig_joint_14:
  .word 0xe00991, asset_slot_re_geo_geo, 0, 0xfffffd71, 0xffff8667, 0x51e, 0x1f  ; node ID 14, depth 3

reptile_cut_rig_joint_16:
  .word 0x1000999, asset_slot_re_geo_geo, 0, 0xfffff290, 0xffff799a, 0x8f5, 0x20  ; node ID 16, depth 4

reptile_body_alternate_rig:
  .word 0x400994, 0x5172c4
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x5172c4
  .zero 8
  .word 0xffffe1ec
  .zero 8
  .word 0x300980, 0x5172c4
  .zero 16
  .word 0x1, 0x200988, 0x5172c4
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x2, 0x500980, 0x5172c4
  .zero 4
  .word 0x3c28, 0xffffab86, 0x13d7, 0x6, 0x700990, 0x5172c4
  .zero 8
  .word 0xffffae15, 0x51e, 0x7, 0x900998, 0x5172c4
  .zero 4
  .word 0x28f, 0xffffb0a4
  .zero 4
  .word 0x8, 0x600993, 0x5172c4
  .zero 4
  .word 0xffffc3d8, 0xffffab86, 0x13d7, 0x9, 0x800991, 0x5172c4
  .zero 8
  .word 0xffffae15, 0x51e, 0xa, 0xa00999, 0x5172c4
  .zero 4
  .word 0xfffffd71, 0xffffb0a4
  .zero 4
  .word 0xb, 0xb00980, 0x5172c4
  .zero 4
  .word 0x1a3d, 0x27ae, 0xb85, 0xc, 0xd00990, 0x5172c4
  .zero 4
  .word 0x28f, 0xffff8667, 0x51e, 0xd, 0xf00998, 0x5172c4
  .zero 4
  .word 0xd70, 0xffff799a, 0x8f5, 0xe, 0xc00993, 0x5172c4
  .zero 4
  .word 0xffffe5c3, 0x27ae, 0xb85, 0xf, 0xe00991, 0x5172c4
  .zero 4
  .word 0xfffffd71, 0xffff8667, 0x51e, 0x10, 0x1000999, 0x5172c4
  .zero 4
  .word 0xfffff290, 0xffff799a, 0x8f5, 0x11, 0x200988, 0x5172c4
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x3, 0x200988, 0x5172c4
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x4, 0x200988, 0x5172c4
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x5

reptile_cut_alternate_rig:
  .word 0x400994, 0x5172c4
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x5172c4
  .zero 8
  .word 0xffffe1ec
  .zero 4
  .word 0x12, 0x300980, 0x5172c4
  .zero 16
  .word 0x13, 0x200988, 0x5172c4
  .zero 8
  .word 0xffff6a5f, 0x153f, 0x14, 0x500980, 0x5172c4
  .zero 4
  .word 0x3c28, 0xffffab86, 0x13d7, 0x15, 0x700990, 0x5172c4
  .zero 8
  .word 0xffffae15, 0x51e, 0x16, 0x900998, 0x5172c4
  .zero 4
  .word 0x28f, 0xffffb0a4
  .zero 4
  .word 0x17, 0x600993, 0x5172c4
  .zero 4
  .word 0xffffc3d8, 0xffffab86, 0x13d7, 0x18, 0x800991, 0x5172c4
  .zero 8
  .word 0xffffae15, 0x51e, 0x19, 0xa00999, 0x5172c4
  .zero 4
  .word 0xfffffd71, 0xffffb0a4
  .zero 4
  .word 0x1a, 0xb00980, 0x5172c4
  .zero 4
  .word 0x1a3d, 0x27ae, 0xb85, 0x1b, 0xd00990, 0x5172c4
  .zero 4
  .word 0x28f, 0xffff8667, 0x51e, 0x1c, 0xf00998, 0x5172c4
  .zero 4
  .word 0xd70, 0xffff799a, 0x8f5, 0x1d, 0xc00993, 0x5172c4
  .zero 4
  .word 0xffffe5c3, 0x27ae, 0xb85, 0x1e, 0xe00991, 0x5172c4
  .zero 4
  .word 0xfffffd71, 0xffff8667, 0x51e, 0x1f, 0x1000999, 0x5172c4
  .zero 4
  .word 0xfffff290, 0xffff799a, 0x8f5, 0x20

; kai: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
kai_body_rig:
  .word 0x400994, asset_slot_me_geo_geo, 0, 0, 0xfffedbb3, 0, 0  ; node ID 4, depth 0

kai_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_me_geo_geo, 0, 0, 0xfffff206, 0, 0  ; node ID 4095, depth 1

kai_body_rig_joint_3:
  .word 0x300980, asset_slot_me_geo_geo, 0, 0, 0xfffff7ab, 0, 0x1  ; node ID 3, depth 2

kai_body_rig_joint_2:
  .word 0x200980, asset_slot_me_geo_geo, 0, 0, 0xffff6a48, 0x144b, 0x2  ; node ID 2, depth 3

kai_body_rig_joint_4093:
  .word 0xffd00898, asset_slot_me_geo_geo, 0, 0, 0, 0, 0x3  ; node ID 4093, depth 4

kai_body_rig_joint_5:
  .word 0x500980, asset_slot_me_geo_geo, 0, 0x3895, 0xffffa92e, 0x1428, 0x4  ; node ID 5, depth 3

kai_body_rig_joint_7:
  .word 0x700990, asset_slot_me_geo_geo, 0, 0x89, 0xffffaf3a, 0x35c, 0x5  ; node ID 7, depth 4

kai_body_rig_joint_9:
  .word 0x900998, asset_slot_me_geo_geo, 0, 0x158, 0xffffb1c8, 0xfffffe86, 0x6  ; node ID 9, depth 5

kai_body_rig_joint_6:
  .word 0x600983, asset_slot_me_geo_geo, 0, 0xffffc76b, 0xffffa92e, 0x1428, 0x7  ; node ID 6, depth 3

kai_body_rig_joint_8:
  .word 0x800991, asset_slot_me_geo_geo, 0, 0xffffff77, 0xffffaf3a, 0x35c, 0x8  ; node ID 8, depth 4

kai_body_rig_joint_10:
  .word 0xa00999, asset_slot_me_geo_geo, 0, 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x9  ; node ID 10, depth 5

kai_body_rig_joint_4094:
  .word 0xffe00898, asset_slot_me_geo_geo, 0, 0, 0, 0, 0xa  ; node ID 4094, depth 3

kai_body_rig_joint_11:
  .word 0xb00980, asset_slot_me_geo_geo, 0, 0x17ec, 0x24f5, 0xac, 0xb  ; node ID 11, depth 2

kai_body_rig_joint_13:
  .word 0xd00990, asset_slot_me_geo_geo, 0, 0xfffffe3f, 0xffff8bcc, 0x1a4, 0xc  ; node ID 13, depth 3

kai_body_rig_joint_15:
  .word 0xf00998, asset_slot_me_geo_geo, 0, 0xd2b, 0xffff7b9e, 0x968, 0xd  ; node ID 15, depth 4

kai_body_rig_joint_12:
  .word 0xc00993, asset_slot_me_geo_geo, 0, 0xffffe814, 0x24f5, 0xac, 0xe  ; node ID 12, depth 2

kai_body_rig_joint_14:
  .word 0xe00991, asset_slot_me_geo_geo, 0, 0x1c1, 0xffff8bcc, 0x1a4, 0xf  ; node ID 14, depth 3

kai_body_rig_joint_16:
  .word 0x1000999, asset_slot_me_geo_geo, 0, 0xfffff2d5, 0xffff7b9e, 0x968, 0x10  ; node ID 16, depth 4

; kai: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
kai_cut_rig:
  .word 0x400994, asset_slot_me_geo_geo, 0, 0, 0xfffedbb3, 0, 0  ; node ID 4, depth 0

kai_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_me_geo_geo, 0, 0, 0xfffff206, 0, 0x11  ; node ID 4095, depth 1

kai_cut_rig_joint_3:
  .word 0x300980, asset_slot_me_geo_geo, 0, 0, 0xfffff7ab, 0, 0x12  ; node ID 3, depth 2

kai_cut_rig_joint_2:
  .word 0x200980, asset_slot_me_geo_geo, 0, 0, 0xffff6a48, 0x144b, 0x13  ; node ID 2, depth 3

kai_cut_rig_joint_4093:
  .word 0xffd00898, asset_slot_me_geo_geo, 0, 0, 0, 0, 0x14  ; node ID 4093, depth 4

kai_cut_rig_joint_5:
  .word 0x500980, asset_slot_me_geo_geo, 0, 0x3895, 0xffffa92e, 0x1428, 0x15  ; node ID 5, depth 3

kai_cut_rig_joint_7:
  .word 0x700990, asset_slot_me_geo_geo, 0, 0x89, 0xffffaf3a, 0x35c, 0x16  ; node ID 7, depth 4

kai_cut_rig_joint_9:
  .word 0x900998, asset_slot_me_geo_geo, 0, 0x158, 0xffffb1c8, 0xfffffe86, 0x17  ; node ID 9, depth 5

kai_cut_rig_joint_6:
  .word 0x600983, asset_slot_me_geo_geo, 0, 0xffffc76b, 0xffffa92e, 0x1428, 0x18  ; node ID 6, depth 3

kai_cut_rig_joint_8:
  .word 0x800991, asset_slot_me_geo_geo, 0, 0xffffff77, 0xffffaf3a, 0x35c, 0x19  ; node ID 8, depth 4

kai_cut_rig_joint_10:
  .word 0xa00999, asset_slot_me_geo_geo, 0, 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x1a  ; node ID 10, depth 5

kai_cut_rig_joint_4094:
  .word 0xffe00898, asset_slot_me_geo_geo, 0, 0, 0, 0, 0x1b  ; node ID 4094, depth 3

kai_cut_rig_joint_11:
  .word 0xb00980, asset_slot_me_geo_geo, 0, 0x17ec, 0x24f5, 0xac, 0x1c  ; node ID 11, depth 2

kai_cut_rig_joint_13:
  .word 0xd00990, asset_slot_me_geo_geo, 0, 0xfffffe3f, 0xffff8bcc, 0x1a4, 0x1d  ; node ID 13, depth 3

kai_cut_rig_joint_15:
  .word 0xf00998, asset_slot_me_geo_geo, 0, 0xd2b, 0xffff7b9e, 0x968, 0x1e  ; node ID 15, depth 4

kai_cut_rig_joint_12:
  .word 0xc00993, asset_slot_me_geo_geo, 0, 0xffffe814, 0x24f5, 0xac, 0x1f  ; node ID 12, depth 2

kai_cut_rig_joint_14:
  .word 0xe00991, asset_slot_me_geo_geo, 0, 0x1c1, 0xffff8bcc, 0x1a4, 0x20  ; node ID 14, depth 3

kai_cut_rig_joint_16:
  .word 0x1000999, asset_slot_me_geo_geo, 0, 0xfffff2d5, 0xffff7b9e, 0x968, 0x21  ; node ID 16, depth 4

kai_body_alternate_rig:
  .word 0x400994, 0x5172dc
  .zero 8
  .word 0xfffedbb3
  .zero 8
  .word 0xfff00890, 0x5172dc
  .zero 8
  .word 0xfffff206
  .zero 8
  .word 0x300980, 0x5172dc
  .zero 8
  .word 0xfffff7ab
  .zero 4
  .word 0x1, 0x200980, 0x5172dc
  .zero 8
  .word 0xffff6a48, 0x144b, 0x2, 0xffd00898, 0x5172dc
  .zero 16
  .word 0x3, 0x500980, 0x5172dc
  .zero 4
  .word 0x3895, 0xffffa92e, 0x1428, 0x4, 0x700990, 0x5172dc
  .zero 4
  .word 0x89, 0xffffaf3a, 0x35c, 0x5, 0x900998, 0x5172dc
  .zero 4
  .word 0x158, 0xffffb1c8, 0xfffffe86, 0x6, 0x600983, 0x5172dc
  .zero 4
  .word 0xffffc76b, 0xffffa92e, 0x1428, 0x7, 0x800991, 0x5172dc
  .zero 4
  .word 0xffffff77, 0xffffaf3a, 0x35c, 0x8, 0xa00999, 0x5172dc
  .zero 4
  .word 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x9, 0xffe00898, 0x5172dc
  .zero 16
  .word 0xa, 0xb00980, 0x5172dc
  .zero 4
  .word 0x17ec, 0x24f5, 0xac, 0xb, 0xd00990, 0x5172dc
  .zero 4
  .word 0xfffffe3f, 0xffff8bcc, 0x1a4, 0xc, 0xf00998, 0x5172dc
  .zero 4
  .word 0xd2b, 0xffff7b9e, 0x968, 0xd, 0xc00993, 0x5172dc
  .zero 4
  .word 0xffffe814, 0x24f5, 0xac, 0xe, 0xe00991, 0x5172dc
  .zero 4
  .word 0x1c1, 0xffff8bcc, 0x1a4, 0xf, 0x1000999, 0x5172dc
  .zero 4
  .word 0xfffff2d5, 0xffff7b9e, 0x968, 0x10

kai_cut_alternate_rig:
  .word 0x400994, 0x5172dc
  .zero 8
  .word 0xfffedbb3
  .zero 8
  .word 0xfff00890, 0x5172dc
  .zero 8
  .word 0xfffff206
  .zero 4
  .word 0x11, 0x300980, 0x5172dc
  .zero 8
  .word 0xfffff7ab
  .zero 4
  .word 0x12, 0x200980, 0x5172dc
  .zero 8
  .word 0xffff6a48, 0x144b, 0x13, 0xffd00898, 0x5172dc
  .zero 16
  .word 0x14, 0x500980, 0x5172dc
  .zero 4
  .word 0x3895, 0xffffa92e, 0x1428, 0x15, 0x700990, 0x5172dc
  .zero 4
  .word 0x89, 0xffffaf3a, 0x35c, 0x16, 0x900998, 0x5172dc
  .zero 4
  .word 0x158, 0xffffb1c8, 0xfffffe86, 0x17, 0x600983, 0x5172dc
  .zero 4
  .word 0xffffc76b, 0xffffa92e, 0x1428, 0x18, 0x800991, 0x5172dc
  .zero 4
  .word 0xffffff77, 0xffffaf3a, 0x35c, 0x19, 0xa00999, 0x5172dc
  .zero 4
  .word 0xfffffea8, 0xffffb1c8, 0xfffffe86, 0x1a, 0xffe00898, 0x5172dc
  .zero 16
  .word 0x1b, 0xb00980, 0x5172dc
  .zero 4
  .word 0x17ec, 0x24f5, 0xac, 0x1c, 0xd00990, 0x5172dc
  .zero 4
  .word 0xfffffe3f, 0xffff8bcc, 0x1a4, 0x1d, 0xf00998, 0x5172dc
  .zero 4
  .word 0xd2b, 0xffff7b9e, 0x968, 0x1e, 0xc00993, 0x5172dc
  .zero 4
  .word 0xffffe814, 0x24f5, 0xac, 0x1f, 0xe00991, 0x5172dc
  .zero 4
  .word 0x1c1, 0xffff8bcc, 0x1a4, 0x20, 0x1000999, 0x5172dc
  .zero 4
  .word 0xfffff2d5, 0xffff7b9e, 0x968, 0x21

; tanya: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
tanya_body_rig:
  .word 0x400994, asset_slot_ta_geo_geo, 0, 0, 0xfffeea44, 0, 0  ; node ID 4, depth 0

tanya_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_ta_geo_geo, 0, 0, 0xffffef1b, 0, 0  ; node ID 4095, depth 1

tanya_body_rig_joint_3:
  .word 0x300980, asset_slot_ta_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

tanya_body_rig_joint_5:
  .word 0x500980, asset_slot_ta_geo_geo, 0, 0x2999, 0xffffa667, 0x13d7, 0x2  ; node ID 5, depth 3

tanya_body_rig_joint_7:
  .word 0x700990, asset_slot_ta_geo_geo, 0, 0x3d7, 0xffffb7af, 0, 0x3  ; node ID 7, depth 4

tanya_body_rig_joint_9:
  .word 0x900998, asset_slot_ta_geo_geo, 0, 0, 0xffffbe15, 0, 0x4  ; node ID 9, depth 5

tanya_body_rig_joint_2:
  .word 0x200980, asset_slot_ta_geo_geo, 0, 0, 0xffff75c3, 0x51e, 0x5  ; node ID 2, depth 3

tanya_body_rig_joint_4094:
  .word 0xffe00898, asset_slot_ta_geo_geo, 0, 0, 0, 0, 0x6  ; node ID 4094, depth 4

tanya_body_rig_joint_6:
  .word 0x600993, asset_slot_ta_geo_geo, 0, 0xffffd667, 0xffffa667, 0x13d7, 0x7  ; node ID 6, depth 3

tanya_body_rig_joint_8:
  .word 0x800991, asset_slot_ta_geo_geo, 0, 0xfffffc29, 0xffffb7af, 0, 0x8  ; node ID 8, depth 4

tanya_body_rig_joint_10:
  .word 0xa00999, asset_slot_ta_geo_geo, 0, 0, 0xffffbe15, 0, 0x9  ; node ID 10, depth 5

tanya_body_rig_joint_11:
  .word 0xb00980, asset_slot_ta_geo_geo, 0, 0x1851, 0x253f, 0xae1, 0xa  ; node ID 11, depth 2

tanya_body_rig_joint_13:
  .word 0xd00990, asset_slot_ta_geo_geo, 0, 0x147, 0xffff8ae2, 0, 0xb  ; node ID 13, depth 3

tanya_body_rig_joint_15:
  .word 0xf00998, asset_slot_ta_geo_geo, 0, 0xb85, 0xffff87af, 0x851, 0xc  ; node ID 15, depth 4

tanya_body_rig_joint_12:
  .word 0xc00993, asset_slot_ta_geo_geo, 0, 0xffffe7af, 0x253f, 0xae1, 0xd  ; node ID 12, depth 2

tanya_body_rig_joint_14:
  .word 0xe00991, asset_slot_ta_geo_geo, 0, 0xfffffeb9, 0xffff8ae2, 0, 0xe  ; node ID 14, depth 3

tanya_body_rig_joint_16:
  .word 0x1000999, asset_slot_ta_geo_geo, 0, 0xfffff47b, 0xffff87af, 0x851, 0xf  ; node ID 16, depth 4

; tanya: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
; kitana: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
tanya_cut_rig:
  .word 0x400994, asset_slot_ta_geo_geo, 0, 0, 0xfffeea44, 0, 0  ; node ID 4, depth 0

tanya_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_ta_geo_geo, 0, 0, 0xffffef1b, 0, 0x10  ; node ID 4095, depth 1

tanya_cut_rig_joint_3:
  .word 0x300980, asset_slot_ta_geo_geo, 0, 0, 0, 0, 0x11  ; node ID 3, depth 2

tanya_cut_rig_joint_5:
  .word 0x500980, asset_slot_ta_geo_geo, 0, 0x2999, 0xffffa667, 0x13d7, 0x12  ; node ID 5, depth 3

tanya_cut_rig_joint_7:
  .word 0x700990, asset_slot_ta_geo_geo, 0, 0x3d7, 0xffffb7af, 0, 0x13  ; node ID 7, depth 4

tanya_cut_rig_joint_9:
  .word 0x900998, asset_slot_ta_geo_geo, 0, 0, 0xffffbe15, 0, 0x14  ; node ID 9, depth 5

tanya_cut_rig_joint_2:
  .word 0x200980, asset_slot_ta_geo_geo, 0, 0, 0xffff75c3, 0x51e, 0x15  ; node ID 2, depth 3

tanya_cut_rig_joint_4094:
  .word 0xffe00898, asset_slot_ta_geo_geo, 0, 0, 0, 0, 0x16  ; node ID 4094, depth 4

tanya_cut_rig_joint_6:
  .word 0x600993, asset_slot_ta_geo_geo, 0, 0xffffd667, 0xffffa667, 0x13d7, 0x17  ; node ID 6, depth 3

tanya_cut_rig_joint_8:
  .word 0x800991, asset_slot_ta_geo_geo, 0, 0xfffffc29, 0xffffb7af, 0, 0x18  ; node ID 8, depth 4

tanya_cut_rig_joint_10:
  .word 0xa00999, asset_slot_ta_geo_geo, 0, 0, 0xffffbe15, 0, 0x19  ; node ID 10, depth 5

tanya_cut_rig_joint_11:
  .word 0xb00980, asset_slot_ta_geo_geo, 0, 0x1851, 0x253f, 0xae1, 0x1a  ; node ID 11, depth 2

tanya_cut_rig_joint_13:
  .word 0xd00990, asset_slot_ta_geo_geo, 0, 0x147, 0xffff8ae2, 0, 0x1b  ; node ID 13, depth 3

tanya_cut_rig_joint_15:
  .word 0xf00998, asset_slot_ta_geo_geo, 0, 0xb85, 0xffff87af, 0x851, 0x1c  ; node ID 15, depth 4

tanya_cut_rig_joint_12:
  .word 0xc00993, asset_slot_ta_geo_geo, 0, 0xffffe7af, 0x253f, 0xae1, 0x1d  ; node ID 12, depth 2

tanya_cut_rig_joint_14:
  .word 0xe00991, asset_slot_ta_geo_geo, 0, 0xfffffeb9, 0xffff8ae2, 0, 0x1e  ; node ID 14, depth 3

tanya_cut_rig_joint_16:
  .word 0x1000999, asset_slot_ta_geo_geo, 0, 0xfffff47b, 0xffff87af, 0x851, 0x1f  ; node ID 16, depth 4

tanya_body_alternate_rig:
  .word 0x400994, 0x5172f4
  .zero 8
  .word 0xfffeea44
  .zero 8
  .word 0xfff00890, 0x5172f4
  .zero 8
  .word 0xffffef1b
  .zero 8
  .word 0x300980, 0x5172f4
  .zero 16
  .word 0x1, 0x500980, 0x5172f4
  .zero 4
  .word 0x2999, 0xffffa667, 0x13d7, 0x2, 0x700990, 0x5172f4
  .zero 4
  .word 0x3d7, 0xffffb7af
  .zero 4
  .word 0x3, 0x900998, 0x5172f4
  .zero 8
  .word 0xffffbe15
  .zero 4
  .word 0x4, 0x200980, 0x5172f4
  .zero 8
  .word 0xffff75c3, 0x51e, 0x5, 0xffe00898, 0x5172f4
  .zero 16
  .word 0x6, 0x600993, 0x5172f4
  .zero 4
  .word 0xffffd667, 0xffffa667, 0x13d7, 0x7, 0x800991, 0x5172f4
  .zero 4
  .word 0xfffffc29, 0xffffb7af
  .zero 4
  .word 0x8, 0xa00999, 0x5172f4
  .zero 8
  .word 0xffffbe15
  .zero 4
  .word 0x9, 0xb00980, 0x5172f4
  .zero 4
  .word 0x1851, 0x253f, 0xae1, 0xa, 0xd00990, 0x5172f4
  .zero 4
  .word 0x147, 0xffff8ae2
  .zero 4
  .word 0xb, 0xf00998, 0x5172f4
  .zero 4
  .word 0xb85, 0xffff87af, 0x851, 0xc, 0xc00993, 0x5172f4
  .zero 4
  .word 0xffffe7af, 0x253f, 0xae1, 0xd, 0xe00991, 0x5172f4
  .zero 4
  .word 0xfffffeb9, 0xffff8ae2
  .zero 4
  .word 0xe, 0x1000999, 0x5172f4
  .zero 4
  .word 0xfffff47b, 0xffff87af, 0x851, 0xf

tanya_cut_alternate_rig:
  .word 0x400994, 0x5172f4
  .zero 8
  .word 0xfffeea44
  .zero 8
  .word 0xfff00890, 0x5172f4
  .zero 8
  .word 0xffffef1b
  .zero 4
  .word 0x10, 0x300980, 0x5172f4
  .zero 16
  .word 0x11, 0x500980, 0x5172f4
  .zero 4
  .word 0x2999, 0xffffa667, 0x13d7, 0x12, 0x700990, 0x5172f4
  .zero 4
  .word 0x3d7, 0xffffb7af
  .zero 4
  .word 0x13, 0x900998, 0x5172f4
  .zero 8
  .word 0xffffbe15
  .zero 4
  .word 0x14, 0x200980, 0x5172f4
  .zero 8
  .word 0xffff75c3, 0x51e, 0x15, 0xffe00898, 0x5172f4
  .zero 16
  .word 0x16, 0x600993, 0x5172f4
  .zero 4
  .word 0xffffd667, 0xffffa667, 0x13d7, 0x17, 0x800991, 0x5172f4
  .zero 4
  .word 0xfffffc29, 0xffffb7af
  .zero 4
  .word 0x18, 0xa00999, 0x5172f4
  .zero 8
  .word 0xffffbe15
  .zero 4
  .word 0x19, 0xb00980, 0x5172f4
  .zero 4
  .word 0x1851, 0x253f, 0xae1, 0x1a, 0xd00990, 0x5172f4
  .zero 4
  .word 0x147, 0xffff8ae2
  .zero 4
  .word 0x1b, 0xf00998, 0x5172f4
  .zero 4
  .word 0xb85, 0xffff87af, 0x851, 0x1c, 0xc00993, 0x5172f4
  .zero 4
  .word 0xffffe7af, 0x253f, 0xae1, 0x1d, 0xe00991, 0x5172f4
  .zero 4
  .word 0xfffffeb9, 0xffff8ae2
  .zero 4
  .word 0x1e, 0x1000999, 0x5172f4
  .zero 4
  .word 0xfffff47b, 0xffff87af, 0x851, 0x1f

; kitana: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
kitana_body_alternate_rig:
  .word 0x400994, asset_slot_ki_geo_geo, 0, 0, 0xfffeea44, 0, 0  ; node ID 4, depth 0

kitana_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_ki_geo_geo, 0, 0, 0xffffef1b, 0, 0  ; node ID 4095, depth 1

kitana_body_rig_joint_3:
  .word 0x300980, asset_slot_ki_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

kitana_body_rig_joint_5:
  .word 0x500980, asset_slot_ki_geo_geo, 0, 0x2999, 0xffffa667, 0x13d7, 0x2  ; node ID 5, depth 3

kitana_body_rig_joint_7:
  .word 0x700990, asset_slot_ki_geo_geo, 0, 0x3d7, 0xffffb7af, 0, 0x3  ; node ID 7, depth 4

kitana_body_rig_joint_9:
  .word 0x900998, asset_slot_ki_geo_geo, 0, 0, 0xffffbe15, 0, 0x4  ; node ID 9, depth 5

kitana_body_rig_joint_2:
  .word 0x200980, asset_slot_ki_geo_geo, 0, 0, 0xffff75c3, 0x51e, 0x5  ; node ID 2, depth 3

kitana_body_rig_joint_4094:
  .word 0xffe00898, asset_slot_ki_geo_geo, 0, 0, 0, 0, 0x6  ; node ID 4094, depth 4

kitana_body_rig_joint_6:
  .word 0x600993, asset_slot_ki_geo_geo, 0, 0xffffd667, 0xffffa667, 0x13d7, 0x7  ; node ID 6, depth 3

kitana_body_rig_joint_8:
  .word 0x800991, asset_slot_ki_geo_geo, 0, 0xfffffc29, 0xffffb7af, 0, 0x8  ; node ID 8, depth 4

kitana_body_rig_joint_10:
  .word 0xa00999, asset_slot_ki_geo_geo, 0, 0, 0xffffbe15, 0, 0x9  ; node ID 10, depth 5

kitana_body_rig_joint_11:
  .word 0xb00980, asset_slot_ki_geo_geo, 0, 0x1851, 0x253f, 0xae1, 0xa  ; node ID 11, depth 2

kitana_body_rig_joint_13:
  .word 0xd00990, asset_slot_ki_geo_geo, 0, 0x147, 0xffff8ae2, 0, 0xb  ; node ID 13, depth 3

kitana_body_rig_joint_15:
  .word 0xf00998, asset_slot_ki_geo_geo, 0, 0xb85, 0xffff87af, 0x851, 0xc  ; node ID 15, depth 4

kitana_body_rig_joint_12:
  .word 0xc00993, asset_slot_ki_geo_geo, 0, 0xffffe7af, 0x253f, 0xae1, 0xd  ; node ID 12, depth 2

kitana_body_rig_joint_14:
  .word 0xe00991, asset_slot_ki_geo_geo, 0, 0xfffffeb9, 0xffff8ae2, 0, 0xe  ; node ID 14, depth 3

kitana_body_rig_joint_16:
  .word 0x1000999, asset_slot_ki_geo_geo, 0, 0xfffff47b, 0xffff87af, 0x851, 0xf  ; node ID 16, depth 4

; jarek: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
jarek_body_rig:
  .word 0x400994, asset_slot_ja_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

jarek_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_ja_geo_geo, 0, 0, 0xffffe1ec, 0, 0  ; node ID 4095, depth 1

jarek_body_rig_joint_3:
  .word 0x300980, asset_slot_ja_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

jarek_body_rig_joint_2:
  .word 0x200988, asset_slot_ja_geo_geo, 0, 0, 0xffff6a5f, 0x1ced, 0x2  ; node ID 2, depth 3

jarek_body_rig_joint_5:
  .word 0x500980, asset_slot_ja_geo_geo, 0, 0x3999, 0xffffaf5d, 0x1ccc, 0x3  ; node ID 5, depth 3

jarek_body_rig_joint_7:
  .word 0x700990, asset_slot_ja_geo_geo, 0, 0xfffff852, 0xffffaccd, 0x147, 0x4  ; node ID 7, depth 4

jarek_body_rig_joint_9:
  .word 0x900998, asset_slot_ja_geo_geo, 0, 0x28f, 0xffffb5c3, 0, 0x5  ; node ID 9, depth 5

jarek_body_rig_joint_6:
  .word 0x600993, asset_slot_ja_geo_geo, 0, 0xffffc667, 0xffffae15, 0x1ccc, 0x6  ; node ID 6, depth 3

jarek_body_rig_joint_8:
  .word 0x800991, asset_slot_ja_geo_geo, 0, 0x7ae, 0xffffaccd, 0x147, 0x7  ; node ID 8, depth 4

jarek_body_rig_joint_10:
  .word 0xa00999, asset_slot_ja_geo_geo, 0, 0xfffffd71, 0xffffb5c3, 0, 0x8  ; node ID 10, depth 5

jarek_body_rig_joint_11:
  .word 0xb00980, asset_slot_ja_geo_geo, 0, 0x1a3d, 0x27ae, 0xb85, 0x9  ; node ID 11, depth 2

jarek_body_rig_joint_13:
  .word 0xd00990, asset_slot_ja_geo_geo, 0, 0x28f, 0xffff8667, 0x51e, 0xa  ; node ID 13, depth 3

jarek_body_rig_joint_15:
  .word 0xf00998, asset_slot_ja_geo_geo, 0, 0xd70, 0xffff799a, 0x7ae, 0xb  ; node ID 15, depth 4

jarek_body_rig_joint_12:
  .word 0xc00993, asset_slot_ja_geo_geo, 0, 0xffffe5c3, 0x27ae, 0xb85, 0xc  ; node ID 12, depth 2

jarek_body_rig_joint_14:
  .word 0xe00991, asset_slot_ja_geo_geo, 0, 0xfffffd71, 0xffff8667, 0x51e, 0xd  ; node ID 14, depth 3

jarek_body_rig_joint_16:
  .word 0x1000999, asset_slot_ja_geo_geo, 0, 0xfffff290, 0xffff799a, 0x7ae, 0xe  ; node ID 16, depth 4

; jarek: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
jarek_cut_rig:
  .word 0x400994, asset_slot_ja_geo_geo, 0, 0, 0xfffee0c5, 0, 0  ; node ID 4, depth 0

jarek_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_ja_geo_geo, 0, 0, 0xffffe1ec, 0, 0xf  ; node ID 4095, depth 1

jarek_cut_rig_joint_3:
  .word 0x300980, asset_slot_ja_geo_geo, 0, 0, 0, 0, 0x10  ; node ID 3, depth 2

jarek_cut_rig_joint_2:
  .word 0x200988, asset_slot_ja_geo_geo, 0, 0, 0xffff6a5f, 0x1ced, 0x11  ; node ID 2, depth 3

jarek_cut_rig_joint_5:
  .word 0x500980, asset_slot_ja_geo_geo, 0, 0x3999, 0xffffaf5d, 0x1ccc, 0x12  ; node ID 5, depth 3

jarek_cut_rig_joint_7:
  .word 0x700990, asset_slot_ja_geo_geo, 0, 0xfffff852, 0xffffaccd, 0x147, 0x13  ; node ID 7, depth 4

jarek_cut_rig_joint_9:
  .word 0x900998, asset_slot_ja_geo_geo, 0, 0x28f, 0xffffb5c3, 0, 0x14  ; node ID 9, depth 5

jarek_cut_rig_joint_6:
  .word 0x600993, asset_slot_ja_geo_geo, 0, 0xffffc667, 0xffffae15, 0x1ccc, 0x15  ; node ID 6, depth 3

jarek_cut_rig_joint_8:
  .word 0x800991, asset_slot_ja_geo_geo, 0, 0x7ae, 0xffffaccd, 0x147, 0x16  ; node ID 8, depth 4

jarek_cut_rig_joint_10:
  .word 0xa00999, asset_slot_ja_geo_geo, 0, 0xfffffd71, 0xffffb5c3, 0, 0x17  ; node ID 10, depth 5

jarek_cut_rig_joint_11:
  .word 0xb00980, asset_slot_ja_geo_geo, 0, 0x1a3d, 0x27ae, 0xb85, 0x18  ; node ID 11, depth 2

jarek_cut_rig_joint_13:
  .word 0xd00990, asset_slot_ja_geo_geo, 0, 0x28f, 0xffff8667, 0x51e, 0x19  ; node ID 13, depth 3

jarek_cut_rig_joint_15:
  .word 0xf00998, asset_slot_ja_geo_geo, 0, 0xd70, 0xffff799a, 0x7ae, 0x1a  ; node ID 15, depth 4

jarek_cut_rig_joint_12:
  .word 0xc00993, asset_slot_ja_geo_geo, 0, 0xffffe5c3, 0x27ae, 0xb85, 0x1b  ; node ID 12, depth 2

jarek_cut_rig_joint_14:
  .word 0xe00991, asset_slot_ja_geo_geo, 0, 0xfffffd71, 0xffff8667, 0x51e, 0x1c  ; node ID 14, depth 3

jarek_cut_rig_joint_16:
  .word 0x1000999, asset_slot_ja_geo_geo, 0, 0xfffff290, 0xffff799a, 0x7ae, 0x1d  ; node ID 16, depth 4

jarek_body_alternate_rig:
  .word 0x400994, 0x517318
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x517318
  .zero 8
  .word 0xffffe1ec
  .zero 8
  .word 0x300980, 0x517318
  .zero 16
  .word 0x1, 0x200988, 0x517318
  .zero 8
  .word 0xffff6a5f, 0x1ced, 0x2, 0x500980, 0x517318
  .zero 4
  .word 0x3999, 0xffffaf5d, 0x1ccc, 0x3, 0x700990, 0x517318
  .zero 4
  .word 0xfffff852, 0xffffaccd, 0x147, 0x4, 0x900998, 0x517318
  .zero 4
  .word 0x28f, 0xffffb5c3
  .zero 4
  .word 0x5, 0x600993, 0x517318
  .zero 4
  .word 0xffffc667, 0xffffae15, 0x1ccc, 0x6, 0x800991, 0x517318
  .zero 4
  .word 0x7ae, 0xffffaccd, 0x147, 0x7, 0xa00999, 0x517318
  .zero 4
  .word 0xfffffd71, 0xffffb5c3
  .zero 4
  .word 0x8, 0xb00980, 0x517318
  .zero 4
  .word 0x1a3d, 0x27ae, 0xb85, 0x9, 0xd00990, 0x517318
  .zero 4
  .word 0x28f, 0xffff8667, 0x51e, 0xa, 0xf00998, 0x517318
  .zero 4
  .word 0xd70, 0xffff799a, 0x7ae, 0xb, 0xc00993, 0x517318
  .zero 4
  .word 0xffffe5c3, 0x27ae, 0xb85, 0xc, 0xe00991, 0x517318
  .zero 4
  .word 0xfffffd71, 0xffff8667, 0x51e, 0xd, 0x1000999, 0x517318
  .zero 4
  .word 0xfffff290, 0xffff799a, 0x7ae, 0xe

jarek_cut_alternate_rig:
  .word 0x400994, 0x517318
  .zero 8
  .word 0xfffee0c5
  .zero 8
  .word 0xfff00890, 0x517318
  .zero 8
  .word 0xffffe1ec
  .zero 4
  .word 0xf, 0x300980, 0x517318
  .zero 16
  .word 0x10, 0x200988, 0x517318
  .zero 8
  .word 0xffff6a5f, 0x1ced, 0x11, 0x500980, 0x517318
  .zero 4
  .word 0x3999, 0xffffaf5d, 0x1ccc, 0x12, 0x700990, 0x517318
  .zero 4
  .word 0xfffff852, 0xffffaccd, 0x147, 0x13, 0x900998, 0x517318
  .zero 4
  .word 0x28f, 0xffffb5c3
  .zero 4
  .word 0x14, 0x600993, 0x517318
  .zero 4
  .word 0xffffc667, 0xffffae15, 0x1ccc, 0x15, 0x800991, 0x517318
  .zero 4
  .word 0x7ae, 0xffffaccd, 0x147, 0x16, 0xa00999, 0x517318
  .zero 4
  .word 0xfffffd71, 0xffffb5c3
  .zero 4
  .word 0x17, 0xb00980, 0x517318
  .zero 4
  .word 0x1a3d, 0x27ae, 0xb85, 0x18, 0xd00990, 0x517318
  .zero 4
  .word 0x28f, 0xffff8667, 0x51e, 0x19, 0xf00998, 0x517318
  .zero 4
  .word 0xd70, 0xffff799a, 0x7ae, 0x1a, 0xc00993, 0x517318
  .zero 4
  .word 0xffffe5c3, 0x27ae, 0xb85, 0x1b, 0xe00991, 0x517318
  .zero 4
  .word 0xfffffd71, 0xffff8667, 0x51e, 0x1c, 0x1000999, 0x517318
  .zero 4
  .word 0xfffff290, 0xffff799a, 0x7ae, 0x1d

; cage: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
cage_body_rig:
  .word 0x400994, asset_slot_jc_geo_geo, 0, 0, 0xfffedc6b, 0, 0  ; node ID 4, depth 0

cage_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_jc_geo_geo, 0, 0, 0xffffe51f, 0, 0  ; node ID 4095, depth 1

cage_body_rig_joint_3:
  .word 0x300980, asset_slot_jc_geo_geo, 0, 0, 0, 0, 0x1  ; node ID 3, depth 2

cage_body_rig_joint_2:
  .word 0x200988, asset_slot_jc_geo_geo, 0, 0, 0xffff75e4, 0x1ced, 0x2  ; node ID 2, depth 3

cage_body_rig_joint_5:
  .word 0x500980, asset_slot_jc_geo_geo, 0, 0x3999, 0xffffaf5d, 0x1ccc, 0x3  ; node ID 5, depth 3

cage_body_rig_joint_7:
  .word 0x700990, asset_slot_jc_geo_geo, 0, 0xfffff852, 0xffffaccd, 0xa4, 0x4  ; node ID 7, depth 4

cage_body_rig_joint_9:
  .word 0x900998, asset_slot_jc_geo_geo, 0, 0x28f, 0xffffb5a5, 0, 0x5  ; node ID 9, depth 5

cage_body_rig_joint_6:
  .word 0x600993, asset_slot_jc_geo_geo, 0, 0xffffc667, 0xffffae15, 0x1ccc, 0x6  ; node ID 6, depth 3

cage_body_rig_joint_8:
  .word 0x800991, asset_slot_jc_geo_geo, 0, 0x7ae, 0xffffaccd, 0x147, 0x7  ; node ID 8, depth 4

cage_body_rig_joint_10:
  .word 0xa00999, asset_slot_jc_geo_geo, 0, 0xfffffd71, 0xffffb5c3, 0, 0x8  ; node ID 10, depth 5

cage_body_rig_joint_11:
  .word 0xb00980, asset_slot_jc_geo_geo, 0, 0x15c2, 0x251e, 0xb85, 0x9  ; node ID 11, depth 2

cage_body_rig_joint_13:
  .word 0xd00990, asset_slot_jc_geo_geo, 0, 0xfffffb86, 0xffff8667, 0x51e, 0xa  ; node ID 13, depth 3

cage_body_rig_joint_15:
  .word 0xf00998, asset_slot_jc_geo_geo, 0, 0xeb8, 0xffff7b86, 0x1eb, 0xb  ; node ID 15, depth 4

cage_body_rig_joint_12:
  .word 0xc00993, asset_slot_jc_geo_geo, 0, 0xffffea3e, 0x251e, 0xb85, 0xc  ; node ID 12, depth 2

cage_body_rig_joint_14:
  .word 0xe00991, asset_slot_jc_geo_geo, 0, 0x47a, 0xffff8667, 0x51e, 0xd  ; node ID 14, depth 3

cage_body_rig_joint_16:
  .word 0x1000999, asset_slot_jc_geo_geo, 0, 0xfffff0a3, 0xffff7b86, 0x1eb, 0xe  ; node ID 16, depth 4

; cage: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
cage_cut_rig:
  .word 0x400994, asset_slot_jc_geo_geo, 0, 0, 0xfffedc6b, 0, 0  ; node ID 4, depth 0

cage_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_jc_geo_geo, 0, 0, 0xffffe51f, 0, 0xf  ; node ID 4095, depth 1

cage_cut_rig_joint_3:
  .word 0x300980, asset_slot_jc_geo_geo, 0, 0, 0, 0, 0x10  ; node ID 3, depth 2

cage_cut_rig_joint_2:
  .word 0x200988, asset_slot_jc_geo_geo, 0, 0, 0xffff75e4, 0x1ced, 0x11  ; node ID 2, depth 3

cage_cut_rig_joint_5:
  .word 0x500980, asset_slot_jc_geo_geo, 0, 0x3999, 0xffffaf5d, 0x1ccc, 0x12  ; node ID 5, depth 3

cage_cut_rig_joint_7:
  .word 0x700990, asset_slot_jc_geo_geo, 0, 0xfffff852, 0xffffaccd, 0x147, 0x13  ; node ID 7, depth 4

cage_cut_rig_joint_9:
  .word 0x900998, asset_slot_jc_geo_geo, 0, 0x28f, 0xffffb5c3, 0, 0x14  ; node ID 9, depth 5

cage_cut_rig_joint_6:
  .word 0x600993, asset_slot_jc_geo_geo, 0, 0xffffc667, 0xffffae15, 0x1ccc, 0x15  ; node ID 6, depth 3

cage_cut_rig_joint_8:
  .word 0x800991, asset_slot_jc_geo_geo, 0, 0x7ae, 0xffffaccd, 0x147, 0x16  ; node ID 8, depth 4

cage_cut_rig_joint_10:
  .word 0xa00999, asset_slot_jc_geo_geo, 0, 0xfffffd71, 0xffffb5c3, 0, 0x17  ; node ID 10, depth 5

cage_cut_rig_joint_11:
  .word 0xb00980, asset_slot_jc_geo_geo, 0, 0x15c2, 0x251e, 0xb85, 0x18  ; node ID 11, depth 2

cage_cut_rig_joint_13:
  .word 0xd00990, asset_slot_jc_geo_geo, 0, 0xfffffb86, 0xffff8667, 0x51e, 0x19  ; node ID 13, depth 3

cage_cut_rig_joint_15:
  .word 0xf00998, asset_slot_jc_geo_geo, 0, 0xeb8, 0xffff7b86, 0x1eb, 0x1a  ; node ID 15, depth 4

cage_cut_rig_joint_12:
  .word 0xc00993, asset_slot_jc_geo_geo, 0, 0xffffea3e, 0x251e, 0xb85, 0x1b  ; node ID 12, depth 2

cage_cut_rig_joint_14:
  .word 0xe00991, asset_slot_jc_geo_geo, 0, 0x47a, 0xffff8667, 0x51e, 0x1c  ; node ID 14, depth 3

cage_cut_rig_joint_16:
  .word 0x1000999, asset_slot_jc_geo_geo, 0, 0xfffff148, 0xffff7b86, 0x1eb, 0x1d  ; node ID 16, depth 4

cage_body_alternate_rig:
  .word 0x400994, 0x517330
  .zero 8
  .word 0xfffedc6b
  .zero 8
  .word 0xfff00890, 0x517330
  .zero 8
  .word 0xffffe51f
  .zero 8
  .word 0x300980, 0x517330
  .zero 16
  .word 0x1, 0x200988, 0x517330
  .zero 8
  .word 0xffff75e4, 0x1ced, 0x2, 0x500980, 0x517330
  .zero 4
  .word 0x3999, 0xffffaf5d, 0x1ccc, 0x3, 0x700990, 0x517330
  .zero 4
  .word 0xfffff852, 0xffffaccd, 0x147, 0x4, 0x900998, 0x517330
  .zero 4
  .word 0x28f, 0xffffb5c3
  .zero 4
  .word 0x5, 0x600993, 0x517330
  .zero 4
  .word 0xffffc667, 0xffffae15, 0x1ccc, 0x6, 0x800991, 0x517330
  .zero 4
  .word 0x7ae, 0xffffaccd, 0x147, 0x7, 0xa00999, 0x517330
  .zero 4
  .word 0xfffffd71, 0xffffb5c3
  .zero 4
  .word 0x8, 0xb00980, 0x517330
  .zero 4
  .word 0x15c2, 0x251e, 0xb85, 0x9, 0xd00990, 0x517330
  .zero 4
  .word 0xfffffb86, 0xffff8667, 0x51e, 0xa, 0xf00998, 0x517330
  .zero 4
  .word 0xeb8, 0xffff7b86, 0x1eb, 0xb, 0xc00993, 0x517330
  .zero 4
  .word 0xffffea3e, 0x251e, 0xb85, 0xc, 0xe00991, 0x517330
  .zero 4
  .word 0x47a, 0xffff8667, 0x51e, 0xd, 0x1000999, 0x517330
  .zero 4
  .word 0xfffff148, 0xffff7b86, 0x1eb, 0xe

cage_cut_alternate_rig:
  .word 0x400994, 0x517330
  .zero 8
  .word 0xfffedc6b
  .zero 8
  .word 0xfff00890, 0x517330
  .zero 8
  .word 0xffffe51f
  .zero 4
  .word 0xf, 0x300980, 0x517330
  .zero 16
  .word 0x10, 0x200988, 0x517330
  .zero 8
  .word 0xffff75e4, 0x1ced, 0x11, 0x500980, 0x517330
  .zero 4
  .word 0x3999, 0xffffaf5d, 0x1ccc, 0x12, 0x700990, 0x517330
  .zero 4
  .word 0xfffff852, 0xffffaccd, 0x147, 0x13, 0x900998, 0x517330
  .zero 4
  .word 0x28f, 0xffffb5c3
  .zero 4
  .word 0x14, 0x600993, 0x517330
  .zero 4
  .word 0xffffc667, 0xffffae15, 0x1ccc, 0x15, 0x800991, 0x517330
  .zero 4
  .word 0x7ae, 0xffffaccd, 0x147, 0x16, 0xa00999, 0x517330
  .zero 4
  .word 0xfffffd71, 0xffffb5c3
  .zero 4
  .word 0x17, 0xb00980, 0x517330
  .zero 4
  .word 0x15c2, 0x251e, 0xb85, 0x18, 0xd00990, 0x517330
  .zero 4
  .word 0xfffffb86, 0xffff8667, 0x51e, 0x19, 0xf00998, 0x517330
  .zero 4
  .word 0xeb8, 0xffff7b86, 0x1eb, 0x1a, 0xc00993, 0x517330
  .zero 4
  .word 0xffffea3e, 0x251e, 0xb85, 0x1b, 0xe00991, 0x517330
  .zero 4
  .word 0x47a, 0xffff8667, 0x51e, 0x1c, 0x1000999, 0x517330
  .zero 4
  .word 0xfffff148, 0xffff7b86, 0x1eb, 0x1d

; goro: body rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
goro_body_alternate_rig:
  .word 0x400994, asset_slot_go_geo_geo, 0, 0, 0xfffeb22e, 0, 0  ; node ID 4, depth 0

goro_body_rig_joint_4095:
  .word 0xfff00890, asset_slot_go_geo_geo, 0, 0, 0xffffe117, 0, 0  ; node ID 4095, depth 1

goro_body_rig_joint_3:
  .word 0x300980, asset_slot_go_geo_geo, 0, 0, 0, 0x235, 0x1  ; node ID 3, depth 2

goro_body_rig_joint_2:
  .word 0x200980, asset_slot_go_geo_geo, 0, 0, 0xffff45f1, 0x1ac3, 0x2  ; node ID 2, depth 3

goro_body_rig_joint_0:
  .word 0x898, asset_slot_go_geo_geo, 0, 0, 0, 0, 0x3  ; node ID 0, depth 4

goro_body_rig_joint_5:
  .word 0x500980, asset_slot_go_geo_geo, 0, 0x4472, 0xffff88c5, 0x211e, 0x4  ; node ID 5, depth 3

goro_body_rig_joint_7:
  .word 0x700990, asset_slot_go_geo_geo, 0, 0xfffff72c, 0xffffa052, 0x178, 0x5  ; node ID 7, depth 4

goro_body_rig_joint_9:
  .word 0x900998, asset_slot_go_geo_geo, 0, 0x2f1, 0xffffaaa0, 0, 0x6  ; node ID 9, depth 5

goro_body_rig_joint_6:
  .word 0x600983, asset_slot_go_geo_geo, 0, 0xffffbb8e, 0xffff88c5, 0x211e, 0x7  ; node ID 6, depth 3

goro_body_rig_joint_8:
  .word 0x800991, asset_slot_go_geo_geo, 0, 0x8d4, 0xffffa052, 0x178, 0x8  ; node ID 8, depth 4

goro_body_rig_joint_10:
  .word 0xa00999, asset_slot_go_geo_geo, 0, 0xfffffd0f, 0xffffaaa0, 0, 0x9  ; node ID 10, depth 5

goro_body_rig_joint_17:
  .word 0x1100980, asset_slot_go_geo_geo, 0, 0x37ef, 0xffffb667, 0x149b, 0xa  ; node ID 17, depth 3

goro_body_rig_joint_18:
  .word 0x1200990, asset_slot_go_geo_geo, 0, 0xfffff72c, 0xffffa052, 0x178, 0xb  ; node ID 18, depth 4

goro_body_rig_joint_19:
  .word 0x1300998, asset_slot_go_geo_geo, 0, 0x2f1, 0xffffaaa0, 0, 0xc  ; node ID 19, depth 5

goro_body_rig_joint_20:
  .word 0x1400993, asset_slot_go_geo_geo, 0, 0xffffc811, 0xffffb667, 0x149b, 0xd  ; node ID 20, depth 3

goro_body_rig_joint_21:
  .word 0x1500991, asset_slot_go_geo_geo, 0, 0x8d4, 0xffffa052, 0x178, 0xe  ; node ID 21, depth 4

goro_body_rig_joint_22:
  .word 0x1600999, asset_slot_go_geo_geo, 0, 0xfffffd0f, 0xffffaaa0, 0, 0xf  ; node ID 22, depth 5

goro_body_rig_joint_11:
  .word 0xb00980, asset_slot_go_geo_geo, 0, 0x1b3b, 0x2ab0, 0x991, 0x10  ; node ID 11, depth 2

goro_body_rig_joint_13:
  .word 0xd00990, asset_slot_go_geo_geo, 0, 0xfffffada, 0xffff7429, 0xae4, 0x11  ; node ID 13, depth 3

goro_body_rig_joint_15:
  .word 0xf00998, asset_slot_go_geo_geo, 0, 0x10ed, 0xffff67a6, 0x235, 0x12  ; node ID 15, depth 4

goro_body_rig_joint_12:
  .word 0xc00993, asset_slot_go_geo_geo, 0, 0xffffe4c5, 0x2ab0, 0x991, 0x13  ; node ID 12, depth 2

goro_body_rig_joint_14:
  .word 0xe00991, asset_slot_go_geo_geo, 0, 0x526, 0xffff7429, 0xae4, 0x14  ; node ID 14, depth 3

goro_body_rig_joint_16:
  .word 0x1000999, asset_slot_go_geo_geo, 0, 0xffffef13, 0xffff67a6, 0x235, 0x15  ; node ID 16, depth 4

; goro: cut rig. Nodes: flags/ID, asset slot, matrix, x/y/z Q16, mesh index.
; Tree flags: bit 3 = leaf, bit 4 = last sibling (common/mk4_assets.c:rig_walk).
goro_cut_alternate_rig:
  .word 0x400994, asset_slot_go_geo_geo, 0, 0, 0xfffeb22e, 0, 0  ; node ID 4, depth 0

goro_cut_rig_joint_4095:
  .word 0xfff00890, asset_slot_go_geo_geo, 0, 0, 0xffffe117, 0, 0x16  ; node ID 4095, depth 1

goro_cut_rig_joint_3:
  .word 0x300980, asset_slot_go_geo_geo, 0, 0, 0, 0x235, 0x17  ; node ID 3, depth 2

goro_cut_rig_joint_2:
  .word 0x200980, asset_slot_go_geo_geo, 0, 0, 0xffff45f1, 0x1ac3, 0x18  ; node ID 2, depth 3

goro_cut_rig_joint_0:
  .word 0x898, asset_slot_go_geo_geo, 0, 0, 0, 0, 0x19  ; node ID 0, depth 4

goro_cut_rig_joint_5:
  .word 0x500980, asset_slot_go_geo_geo, 0, 0x4472, 0xffff88c5, 0x211e, 0x1a  ; node ID 5, depth 3

goro_cut_rig_joint_7:
  .word 0x700990, asset_slot_go_geo_geo, 0, 0xfffff72c, 0xffffa052, 0x178, 0x1b  ; node ID 7, depth 4

goro_cut_rig_joint_9:
  .word 0x900998, asset_slot_go_geo_geo, 0, 0x2f1, 0xffffaaa0, 0, 0x1c  ; node ID 9, depth 5

goro_cut_rig_joint_6:
  .word 0x600983, asset_slot_go_geo_geo, 0, 0xffffbb8e, 0xffff88c5, 0x211e, 0x1d  ; node ID 6, depth 3

goro_cut_rig_joint_8:
  .word 0x800991, asset_slot_go_geo_geo, 0, 0x8d4, 0xffffa052, 0x178, 0x1e  ; node ID 8, depth 4

goro_cut_rig_joint_10:
  .word 0xa00999, asset_slot_go_geo_geo, 0, 0xfffffd0f, 0xffffaaa0, 0, 0x1f  ; node ID 10, depth 5

goro_cut_rig_joint_17:
  .word 0x1100980, asset_slot_go_geo_geo, 0, 0x37ef, 0xffffb667, 0x149b, 0x20  ; node ID 17, depth 3

goro_cut_rig_joint_18:
  .word 0x1200990, asset_slot_go_geo_geo, 0, 0xfffff72c, 0xffffa052, 0x178, 0x21  ; node ID 18, depth 4

goro_cut_rig_joint_19:
  .word 0x1300998, asset_slot_go_geo_geo, 0, 0x2f1, 0xffffaaa0, 0, 0x22  ; node ID 19, depth 5

goro_cut_rig_joint_20:
  .word 0x1400993, asset_slot_go_geo_geo, 0, 0xffffc811, 0xffffb667, 0x149b, 0x23  ; node ID 20, depth 3

goro_cut_rig_joint_21:
  .word 0x1500991, asset_slot_go_geo_geo, 0, 0x8d4, 0xffffa052, 0x178, 0x24  ; node ID 21, depth 4

goro_cut_rig_joint_22:
  .word 0x1600999, asset_slot_go_geo_geo, 0, 0xfffffd0f, 0xffffaaa0, 0, 0x25  ; node ID 22, depth 5

goro_cut_rig_joint_11:
  .word 0xb00980, asset_slot_go_geo_geo, 0, 0x1b3b, 0x2ab0, 0x991, 0x26  ; node ID 11, depth 2

goro_cut_rig_joint_13:
  .word 0xd00990, asset_slot_go_geo_geo, 0, 0xfffffada, 0xffff7429, 0xae4, 0x27  ; node ID 13, depth 3

goro_cut_rig_joint_15:
  .word 0xf00998, asset_slot_go_geo_geo, 0, 0x10ed, 0xffff67a6, 0x235, 0x28  ; node ID 15, depth 4

goro_cut_rig_joint_12:
  .word 0xc00993, asset_slot_go_geo_geo, 0, 0xffffe4c5, 0x2ab0, 0x991, 0x29  ; node ID 12, depth 2

goro_cut_rig_joint_14:
  .word 0xe00991, asset_slot_go_geo_geo, 0, 0x526, 0xffff7429, 0xae4, 0x2a  ; node ID 14, depth 3

goro_cut_rig_joint_16:
  .word 0x1000999, asset_slot_go_geo_geo, 0, 0xffffef13, 0xffff67a6, 0x235, 0x2b  ; node ID 16, depth 4
  .word 0x1850000
  .zero 12
  .word 0xc6799000
  .zero 8
  .word 0x348457cd, 0x3be000, 0xfb000000, 0x849801b, 0x7f7, 0xf70007ea
  .word 0xf3fc8009
  .zero 4
  .word 0xf5c00000, 0x1850000
  .zero 12
  .word 0xc6799000
  .zero 8
  .word 0x348457cd, 0x3be000, 0xfb000000, 0x849801b, 0x7f7, 0xf70007ea
  .word 0xf3fc8009
  .zero 4
  .word 0xf5c00000, 0x1850000
  .zero 4
  .word 0x3
  .zero 4
  .word 0xc6799000
  .zero 8
  .word 0x348457cd, 0x3be000, 0xfb000000, 0x849801b, 0x7f7, 0xf70007ea
  .word 0xf3fc8009
  .zero 4
  .word 0xf5c00000, 0x1850000
  .zero 12
  .word 0xc6799000
  .zero 8
  .word 0x348457cd, 0x3be000, 0xfb000000, 0x849801b, 0x7f7, 0xf70007ea
  .word 0xf3fc8009
  .zero 4
  .word 0xf5c00000
  .zero 12
  .word 0x7fe
  .zero 60
  .word 0x7fe
  .zero 60
  .word 0x7fe
  .zero 60
  .word 0x7fe
  .zero 336
  .word 0x39b800
  .zero 8
  .word 0xc8400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x354007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x183000
  .zero 8
  .word 0xd5400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x1bc007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x39b800
  .zero 8
  .word 0xc8400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x354007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x39b800
  .zero 8
  .word 0xc8400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x354007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x183000
  .zero 8
  .word 0xd5400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x1bc007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x39b800
  .zero 8
  .word 0xc8400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x354007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x39b800
  .zero 8
  .word 0xc8400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x354007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de, 0x183000
  .zero 8
  .word 0xd5400000, 0x3dd800
  .zero 4
  .word 0x3b97e0
  .zero 4
  .word 0x1bc007da, 0x3bb800
  .zero 4
  .word 0xd000000
  .zero 4
  .word 0xf2017fed, 0xf3000000
  .zero 4
  .word 0xf2c2c7de
  .zero 12
  .word 0x3b27f8, 0xca4007f8, 0x3cd800
  .zero 4
  .word 0x36021fbf, 0x3c4800
  .zero 4
  .word 0x589e053, 0x7d0, 0xf9c007cc, 0xf17e2800
  .zero 4
  .word 0xf2c00000, 0x2f000, 0x1c00000, 0x43fe4, 0x3b27f8, 0xca4007f8
  .word 0x3cd800
  .zero 4
  .word 0x36021fbf, 0x3c4800
  .zero 4
  .word 0x589e053, 0x7d0, 0xf9c007cc, 0xf17e2800
  .zero 4
  .word 0xf2c00000, 0x2f000, 0x1c00000, 0x43fe4, 0x3b27f8, 0xca4007f8
  .word 0x3cd800
  .zero 4
  .word 0x36021fbf, 0x3c4800
  .zero 4
  .word 0x589e053, 0x7d0, 0xf9c007cc, 0xf17e2800
  .zero 4
  .word 0xf2c00000, 0x2f000, 0x1c00000, 0x43fe4, 0x3b27f8, 0xca4007f8
  .word 0x3cd800
  .zero 4
  .word 0x36021fbf, 0x3c4800
  .zero 4
  .word 0x589e053, 0x7d0, 0xf9c007cc, 0xf17e2800
  .zero 4
  .word 0xf2c00000
  .zero 440
  .word 0x51a624, 0x1, 0x100000, 0x51a664, 0x1, 0x100000
  .word 0x51a6a4, 0x1, 0x100000, 0x51a6e4, 0x1, 0x100000
  .word 0x51a724, 0x1, 0x100000, 0x51a764, 0x1, 0x100000
  .word 0x51a7a4, 0x1, 0x100000, 0x51a7e4, 0x1, 0x100000
  .word 0x51a824, 0x1, 0x120000, 0x51a86c, 0x1, 0x120000
  .word 0x51a8b4, 0x1, 0x120000, 0x51a8fc, 0x1, 0x120000
  .word 0x51a944, 0x1, 0x110000, 0x51a9cc, 0x1, 0x110000
  .word 0x51aa10, 0x1, 0x110000, 0x51aa98, 0x1, 0x110000
  .word 0x51aadc, 0x1, 0x110000, 0x51ab64, 0x1, 0x100000
  .word 0x51aba4, 0x1, 0x100000, 0x51abe4, 0x1, 0x100000
  .word 0x51ac24, 0x1, 0x100000, 0x51ac64, 0x1, 0x100000
  .word 0x51aca4, 0x1, 0x100000, 0x51ace4, 0x1, 0x100000
  .word 0x51ad24, 0x1, 0x100000, 0x51ad64, 0x1, 0x170000
  .word 0x51adc0, 0x1, 0x170000, 0x9875, 0x9873, 0x4c40d0
  .word 0x4c40f0, 0x4c40f0
  .zero 16
  .word 0x3fe00000, 0x776f70, 0x1, 0x6f6d6604, 0x64
  .zero 4
  .word 0x160200, 0x4c4db4, 0x4c8436, 0x4c83cf, 0x4c8436, 0x4c8394
  .word 0x4c8436, 0x4c83cf, 0x4c8436, 0x4c83cd, 0x4c83cd, 0x4c83f7
  .word 0x4c83cd, 0x4c8436, 0x4c8436, 0x4c83cf, 0x4c8436, 0x19930520
  .zero 12
  .word 0x4c4e30, 0x2, 0x1000
  .zero 4
  .word 0x51b000, 0x51b000, 0x51b018, 0x51b018, 0xffffffff, 0xffffffff
  .word 0xf0, 0xf1
  .zero 8192
  .word 0x51b000, 0x1e0, 0x4ceb1c, 0x4ceb0c, 0x4cafe0, 0x4cafe0
  .word 0x4cafe0, 0x4cafe0, 0x4cafe0, 0x4cafe0, 0x8040201
  .zero 4
  .word 0x3a4, 0x82798260, 0x21
  .zero 4
  .word 0xdfa6
  .zero 4
  .word 0xa5a1
  .zero 4
  .word 0xfce09f81
  .zero 4
  .word 0xfc807e40
  .zero 4
  .word 0x3a8, 0xa3daa3c1, 0x20
  .zero 20
  .word 0xfe81
  .zero 4
  .word 0xfe40
  .zero 4
  .word 0x3b5, 0xa3daa3c1, 0x20
  .zero 20
  .word 0xfe81
  .zero 4
  .word 0xfe41
  .zero 4
  .word 0x3b6, 0xa2e4a2cf, 0xa2e5001a, 0x5ba2e8
  .zero 16
  .word 0xfe81
  .zero 4
  .word 0xfea17e40
  .zero 4
  .word 0x551, 0xda5eda51, 0xda5f0020, 0x32da6a
  .zero 16
  .word 0xded8d381, 0xf9e0, 0xfe817e31
  .zero 4
  .word 0xffffffff, 0xa00
  .zero 12
  .word 0x80000000, 0x7fff
  .zero 4
  .word 0xffff8000, 0xb9d7a7dc, 0xb1716685, 0x400d
  .zero 4
  .word 0x400dffff, 0xc4336f7, 0x95f61998, 0x3ffd
  .zero 4
  .word 0x3fe0
  .zero 8
  .word 0x70786503
  .zero 8
  .word 0x140100, 0x4c8091, 0x4c839d, 0x4c83a2, 0x4c81d1
  .zero 16
  .word 0xc0000000, 0xc235ffff, 0xdaa22168, 0x3fffc90f
  .zero 4
  .word 0x3ff00000, 0x8040801, 0x8040808, 0xc040008, 0xc040008, 0x8
  .word 0x1, 0x16, 0x2, 0x2, 0x3, 0x2
  .word 0x4, 0x18, 0x5, 0xd, 0x6, 0x9
  .word 0x7, 0xc, 0x8, 0xc, 0x9, 0xc
  .word 0xa, 0x7, 0xb, 0x8, 0xc, 0x16
  .word 0xd, 0x16, 0xf, 0x2, 0x10, 0xd
  .word 0x11, 0x12, 0x12, 0x2, 0x21, 0xd
  .word 0x35, 0x2, 0x41, 0xd, 0x43, 0x2
  .word 0x50, 0x11, 0x52, 0xd, 0x53, 0xd
  .word 0x57, 0x16, 0x59, 0xb, 0x6c, 0xd
  .word 0x6d, 0x20, 0x70, 0x1c, 0x72, 0x9
  .word 0x6, 0x16, 0x80, 0xa, 0x81, 0xa
  .word 0x82, 0x9, 0x83, 0x16, 0x84, 0xd
  .word 0x91, 0x29, 0x9e, 0xd, 0xa1, 0x2
  .word 0xa4, 0xb, 0xa7, 0xd, 0xb7, 0x11
  .word 0xce, 0x2, 0xd7, 0xb, 0x718, 0xc
  .zero 8
  .word 0x100, 0x7000004, 0xa0000, 0xd00, 0x3f700000, 0x3f880000
  .word 0x5f000000, 0x100, 0x7000004, 0xa0000, 0xd00
  .zero 4
  .word 0x3fee0000
  .zero 4
  .word 0x43f00000
  .zero 4
  .word 0x3bf00000
  .zero 4
  .word 0x3ff00000
  .zero 4
  .word 0x3fe00000
  .zero 4
  .word 0xffff0000, 0x8cf47ffe, 0x8cfa004c, 0x8cff004c, 0x8d05004c, 0x8d0a004c
  .word 0x8d10004c, 0x8d16004c, 0x8d1c004c, 0x8d22004c, 0x8d3e004c, 0x8d43004c
  .word 0x8d59004c, 0x8d5e004c, 0x8d7a004c, 0x8d8a004c, 0x8d9e004c, 0x8dae004c
  .word 0x8dce004c, 0x8dd3004c, 0x8ded004c, 0x8df2004c, 0x8e12004c, 0x8e26004c
  .word 0x8e3e004c, 0x8e52004c, 0x8e72004c, 0x8e77004c, 0x8e91004c, 0x8e96004c
  .word 0x8eb6004c, 0x8eca004c, 0x8ee2004c, 0x8ef6004c, 0x8f16004c, 0x8f1b004c
  .word 0x8f35004c, 0x8f3a004c, 0x8f5a004c, 0x8f6e004c, 0x8f86004c, 0x8f9a004c
  .word 0x8fba004c, 0x8fbf004c, 0x8fd9004c, 0x8fde004c, 0x8ffe004c, 0x9012004c
  .word 0x902a004c, 0x903e004c, 0x905e004c, 0x9063004c, 0x907d004c, 0x9082004c
  .word 0x90a2004c, 0x90b6004c, 0x90ce004c, 0x90e2004c, 0x9102004c, 0x9107004c
  .word 0x9121004c, 0x9126004c, 0x9146004c, 0x915a004c, 0x9172004c, 0x4c
  .word 0xf99f40
  .zero 4
  .word 0xf99f40, 0x101
  .zero 8
  .word 0x1000
  .zero 16
  .word 0x2, 0x1
  .zero 24
  .word 0x2, 0x2
  .zero 556
  .word 0xc0000005, 0xb
  .zero 4
  .word 0xc000001d, 0x4
  .zero 4
  .word 0xc0000096, 0x4
  .zero 4
  .word 0xc000008d, 0x8
  .zero 4
  .word 0xc000008e, 0x8
  .zero 4
  .word 0xc000008f, 0x8
  .zero 4
  .word 0xc0000090, 0x8
  .zero 4
  .word 0xc0000091, 0x8
  .zero 4
  .word 0xc0000092, 0x8
  .zero 4
  .word 0xc0000093, 0x8
  .zero 4
  .word 0x3, 0x7, 0xa, 0x8c, 0x2, 0x4cee64
  .word 0x8, 0x4cee38, 0x9, 0x4cee0c, 0xa, 0x4cede8
  .word 0x10, 0x4cedbc, 0x11, 0x4ced8c, 0x12, 0x4ced68
  .word 0x13, 0x4ced3c, 0x18, 0x4ced04, 0x19, 0x4cecdc
  .word 0x1a, 0x4ceca4, 0x1b, 0x4cec6c, 0x1c, 0x4cec44
  .word 0x78, 0x4cec34, 0x79, 0x4cec24, 0x7a, 0x4cec14
  .word 0xfc, 0x4cec10, 0xff, 0x4cec00, 0x51d862, 0x51d862
  .word 0x200000, 0x200020, 0x200020, 0x200020, 0x200020, 0x280028
  .word 0x280028, 0x200028, 0x200020, 0x200020, 0x200020, 0x200020
  .word 0x200020, 0x200020, 0x200020, 0x200020, 0x480020, 0x100010
  .word 0x100010, 0x100010, 0x100010, 0x100010, 0x100010, 0x100010
  .word 0x840010, 0x840084, 0x840084, 0x840084, 0x840084, 0x100084
  .word 0x100010, 0x100010, 0x100010, 0x810081, 0x810081, 0x810081
  .word 0x10001, 0x10001, 0x10001, 0x10001, 0x10001, 0x10001
  .word 0x10001, 0x10001, 0x10001, 0x10001, 0x100010, 0x100010
  .word 0x100010, 0x820082, 0x820082, 0x820082, 0x20002, 0x20002
  .word 0x20002, 0x20002, 0x20002, 0x20002, 0x20002, 0x20002
  .word 0x20002, 0x20002, 0x100010, 0x100010, 0x20
  .zero 268
  .word 0x1, 0x2e, 0x1
  .zero 4
  .word 0x400, 0xfffffc01, 0x35, 0xb, 0x40, 0x3ff
  .word 0x80, 0xffffff81, 0x18, 0x8, 0x20, 0x7f
  .zero 4
  .word 0x7ff00000
  .zero 4
  .word 0xfff80000, 0xffffffff, 0x7fefffff
  .zero 4
  .word 0x100000
  .zero 4
  .word 0x80000000, 0x14, 0x4cf01c, 0x1d, 0x4cf018, 0x1a
  .word 0x4cf014, 0x1b, 0x4cf00c, 0x1f, 0x4cf004, 0x13
  .word 0x4ceffc, 0x21, 0x4ceff4, 0xe, 0x4cefec, 0xd
  .word 0x4cefe4, 0xf, 0x4cefdc, 0x10, 0x4cefd4, 0x5
  .word 0x4cefcc, 0x1e, 0x4cefc8, 0x12, 0x4cefc4, 0x20
  .word 0x4cefc0, 0xc, 0x4cefb8, 0xb, 0x4cefb0, 0x15
  .word 0x4cefa8, 0x1c, 0x4cefa0, 0x19, 0x4cef98, 0x11
  .word 0x4cef90, 0x18, 0x4cef88, 0x16, 0x4cef80, 0x17
  .word 0x4cef78, 0x22, 0x4cef74, 0x23, 0x4cef70, 0x24
  .word 0x4cef6c, 0x2694
  .zero 8
  .word 0x80000000, 0x4410
  .zero 4
  .word 0x1, 0x80000000, 0x3000
  .zero 12
  .word 0x4002a000
  .zero 8
  .word 0x4005c800
  .zero 8
  .word 0x4008fa00
  .zero 8
  .word 0x400c9c40
  .zero 8
  .word 0x400fc350
  .zero 8
  .word 0x4012f424
  .zero 4
  .word 0x80000000, 0x40169896
  .zero 4
  .word 0x20000000, 0x4019bebc
  .zero 4
  .word 0xc9bf0400, 0x40348e1b, 0xa1000000, 0x1bcecced, 0x404ed3c2, 0xb59ef020
  .word 0xada82b70, 0x40699dc5, 0x25fd5dd0, 0x4f8e1ae5, 0x4083eb19, 0x95d79671
  .word 0x8d050e43, 0x409eaf29, 0x44a0bff9, 0x8f1281ed, 0x40b98281, 0xa6d53cbf
  .word 0x1f49ffcf, 0x40d3c278, 0x8ce0c66f, 0x47c980e9, 0x41a893ba, 0x556b85bc
  .word 0xf78d3927, 0x427ce070, 0xde8eddbc, 0xebfb9df9, 0x4351aa7e, 0xe376e6a1
  .word 0x2f29f2cc, 0x44268184, 0xaa171028, 0xe310aef8, 0x44fac4c5, 0xf3d4a7eb
  .word 0x4ae1ebf7, 0x45cf957a, 0x91c7cc65, 0xa0aea60e, 0x46a3e319, 0xc17650d
  .word 0x75868175, 0x4d48c976, 0xa7e44258, 0x353b3993, 0x53edb2b8, 0x5de5a74d
  .word 0x3b5dc53d, 0x5a929e8b, 0xf0a65dff, 0x54c020a1, 0x61378ca5, 0x5a8bfdd1
  .word 0x5d25d88b, 0x67dbf989, 0xf3f895aa, 0xc8a2bf27, 0x6e80dd5d, 0x979bc94c
  .word 0x52028a20, 0x7525c460
  .zero 4
  .word 0xcccdcccd, 0xcccccccc, 0x3ffbcccc, 0xd70a3d71, 0xa3d70a3, 0x3ff8a3d7
  .word 0xdf3b645a, 0x6e978d4f, 0x3ff58312, 0x652cd3c3, 0x1758e219, 0x3ff1d1b7
  .word 0x84230fd0, 0xac471b47, 0x3feea7c5, 0x69b6a640, 0xbd05af6c, 0x3feb8637
  .word 0x42bc3d33, 0x94d5e57a, 0x3fe7d6bf, 0xcefdfdc2, 0x77118461, 0x3fe4abcc
  .word 0xe15b4c2f, 0x94bec44d, 0x3fc9e695, 0x3b53c492, 0x14cd4475, 0x3faf9abe
  .word 0x94ba67de, 0x1ead4539, 0x3f94cfb1, 0xe2c62324, 0x313bbabc, 0x3f7a8b61
  .word 0xc1595561, 0x7c53b17e, 0x3f5fbb12, 0x8d2feed7, 0x8592be06, 0x3f44fb15
  .word 0xe9a53f24, 0xea27a539, 0x3f2aa87f, 0xe4a1ac7d, 0x467c64bc, 0x3e55ddd0
  .word 0xcc067b63, 0x83775423, 0x3d8191ff, 0x193afa91, 0x4325637a, 0x3cacc031
  .word 0x38d18921, 0xb8974782, 0x3bd7fd00, 0x85888dc, 0xe3e8b11b, 0x3b03a686
  .word 0x424584c6, 0x7599b607, 0x3a2edb37, 0xd21c7133, 0xee32db23, 0x395a9049
  .word 0xc0be87a6, 0x82a5da57, 0x32b5a2a6, 0x11b268e2, 0x449f52a7, 0x2c10b759
  .word 0x2de44925, 0x534f3436, 0x256bceae, 0xa404598f, 0x7dc2dec0, 0x1ec6e8fb
  .word 0x5a88e79e, 0xbf3c9157, 0x18228350, 0x62654b4e, 0xaf8f83fd, 0x117d9406
  .word 0x9fde2de4, 0x4c8d2ce, 0xad8a6dd
  .zero 2188

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_001:
  .zero 26416

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_002:
  .zero 96

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_003:
  .zero 11356

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_004:
  .zero 20

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_005:
  .zero 34212

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_006:
  .zero 52

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_007:
  .zero 32

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_008:
  .zero 96

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_009:
  .zero 56

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_010:
  .zero 24

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_011:
  .zero 16

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_012:
  .zero 52

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_013:
  .zero 28

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_014:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_015:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_016:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_017:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_018:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_019:
  .zero 8192

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_020:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_021:
  .zero 24

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_022:
  .zero 68

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_023:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_024:
  .zero 32

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_025:
  .zero 28

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_026:
  .zero 36

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_027:
  .zero 56

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_028:
  .zero 224

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_029:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_030:
  .zero 56

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_031:
  .zero 72

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_032:
  .zero 32

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_033:
  .zero 8364

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_034:
  .zero 36

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_035:
  .zero 24

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_036:
  .zero 136

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_037:
  .zero 16

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_038:
  .zero 48

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_039:
  .zero 40

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_040:
  .zero 60

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_041:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_042:
  .zero 44

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_043:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_044:
  .zero 112

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_045:
  .zero 56

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_046:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_047:
  .zero 88

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_048:
  .zero 24

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_049:
  .zero 168

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_050:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_051:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_052:
  .zero 268

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_053:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_054:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_055:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_056:
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_057:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_058:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_059:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_060:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_061:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_062:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_063:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_064:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_065:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_066:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_067:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_068:
  .zero 20

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_069:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_070:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_071:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_072:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_073:
  .zero 28

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_074:
  .zero 44

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_075:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_076:
  .zero 12

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_077:
  .zero 24

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_078:
  .zero 60

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_079:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_080:
  .zero 16

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_081:
  .zero 48

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_082:
  .zero 48

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_083:
  .zero 15200

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_084:
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_085:
  .zero 14908

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_086:
  .zero 76

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_087:
  .zero 84

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_088:
  .zero 40

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_089:
  .zero 24
  .word clip(286)
  .zero 4
  .word word(unresolved_fighter_assets_019), word(unresolved_fighter_assets_011), word(unresolved_fighter_assets_080), word(unresolved_fighter_assets_001), word(unresolved_fighter_assets_038)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_090:
  .word word(unresolved_fighter_assets_010)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_091:
  .word word(unresolved_fighter_assets_035)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_092:
  .word word(unresolved_fighter_assets_039)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_093:
  .word word(unresolved_fighter_assets_089)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_094:
  .word word(unresolved_fighter_assets_030)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_095:
  .word word(unresolved_fighter_assets_078)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_096:
  .word word(unresolved_fighter_assets_033)

; Retained referenced block; semantic role is not established.
unresolved_fighter_assets_097:
  .word word(unresolved_fighter_assets_077), word(unresolved_fighter_assets_005), word(unresolved_fighter_assets_003), word(unresolved_fighter_assets_083), word(unresolved_fighter_assets_047)
  .zero 212
  .word word(unresolved_fighter_assets_002), word(unresolved_fighter_assets_027), word(unresolved_fighter_assets_075), word(unresolved_fighter_assets_009), word(unresolved_fighter_assets_088), word(unresolved_fighter_assets_045)
  .word word(unresolved_fighter_assets_049)
  .zero 8
  .word word(unresolved_retained_frontend_002)
  .zero 4
  .word word(unresolved_fighter_assets_025), word(unresolved_dispatch_and_motion_002), word(unresolved_fighter_assets_012), word(unresolved_stance_and_queries_009), word(unresolved_projectile_parameters_019), word(unresolved_fighter_assets_070)
  .word word(unresolved_fighter_assets_071), word(unresolved_fighter_assets_072)
  .zero 8
  .word word(unresolved_scene_parameters_008), word(unresolved_scene_parameters_009), word(unresolved_scene_parameters_010), word(unresolved_scene_parameters_007), word(unresolved_fighter_assets_036), word(unresolved_fighter_assets_032)
  .word word(unresolved_fighter_assets_082), word(unresolved_fighter_assets_007)
  .zero 16
  .word word(unresolved_reactions_and_audio_001), word(unresolved_fighter_assets_017), word(unresolved_fighter_assets_018), word(unresolved_fighter_assets_015), word(unresolved_fighter_assets_016), word(unresolved_fighter_assets_079)
  .word word(unresolved_fighter_assets_006), word(unresolved_arenas_002), word(unresolved_arenas_002), word(unresolved_arenas_003), word(unresolved_arenas_001), word(unresolved_fighter_assets_014)
  .word word(unresolved_fighter_assets_037)
  .zero 96
  .word word(unresolved_fighter_assets_090), word(unresolved_fighter_assets_091), word(unresolved_fighter_assets_092), word(unresolved_fighter_assets_093), word(unresolved_fighter_assets_094), word(unresolved_fighter_assets_095)
  .word word(unresolved_fighter_assets_096), word(unresolved_fighter_assets_097)
  .zero 24
  .word word(unresolved_retained_frontend_039)
  .zero 4
  .word word(unresolved_retained_frontend_052)
  .zero 4
  .word word(unresolved_retained_frontend_062)
  .zero 4
  .word word(unresolved_retained_frontend_078), word(unresolved_retained_frontend_080), word(unresolved_retained_frontend_082), word(unresolved_retained_frontend_084)
  .zero 8
  .word word(unresolved_retained_frontend_091), word(unresolved_retained_frontend_091), word(unresolved_retained_frontend_091)
  .zero 4
  .word word(unresolved_retained_frontend_093), word(unresolved_retained_frontend_093), word(unresolved_retained_frontend_093)
  .zero 4
  .word word(unresolved_arenas_045), word(unresolved_arenas_050), word(unresolved_arenas_046), word(unresolved_arenas_047), word(unresolved_arenas_044), word(unresolved_arenas_050)
  .word word(unresolved_arenas_043), word(unresolved_arenas_049), word(unresolved_arenas_045), word(unresolved_arenas_050), word(unresolved_arenas_044), word(unresolved_arenas_050)
  .word word(unresolved_arenas_046), word(unresolved_arenas_047), word(unresolved_arenas_042), word(unresolved_arenas_048), word(unresolved_arenas_043), word(unresolved_arenas_049)
  .word word(unresolved_arenas_045), word(unresolved_arenas_050), word(unresolved_arenas_044), word(unresolved_arenas_050)

animation_descriptor_mmove_lk_owner_return:
  .word clip(282)
  .zero 4
  .word word(unresolved_throw_parameters_081), word(unresolved_throw_parameters_077), word(unresolved_throw_parameters_078), word(unresolved_throw_parameters_076), word(unresolved_throw_parameters_075), word(unresolved_throw_parameters_074)
  .word word(unresolved_throw_parameters_073), word(unresolved_throw_parameters_071), word(unresolved_throw_parameters_072), word(unresolved_throw_parameters_079), word(unresolved_throw_parameters_080), word(unresolved_throw_parameters_070)
  .word word(unresolved_throw_parameters_069), word(unresolved_throw_parameters_068), word(unresolved_throw_parameters_067), word(unresolved_throw_parameters_066), word(unresolved_throw_parameters_064), word(unresolved_throw_parameters_065)
  .word word(unresolved_throw_parameters_063), word(unresolved_throw_parameters_062), word(unresolved_throw_parameters_061), word(unresolved_throw_parameters_060), word(unresolved_throw_parameters_059), word(unresolved_throw_parameters_058)
  .word word(unresolved_throw_parameters_056), word(unresolved_throw_parameters_054), word(unresolved_throw_parameters_055), word(unresolved_throw_parameters_052), word(unresolved_throw_parameters_053), word(unresolved_throw_parameters_051)
  .word word(unresolved_throw_parameters_050), word(unresolved_throw_parameters_049), word(unresolved_throw_parameters_048), word(unresolved_throw_parameters_047), word(unresolved_throw_parameters_046), word(unresolved_throw_parameters_045)
  .word word(unresolved_throw_parameters_044), word(unresolved_throw_parameters_043), word(unresolved_throw_parameters_057), word(unresolved_throw_parameters_042), word(unresolved_throw_parameters_041), word(unresolved_throw_parameters_040)
  .word word(unresolved_throw_parameters_034), word(unresolved_throw_parameters_035), word(unresolved_throw_parameters_036), word(unresolved_throw_parameters_031), word(unresolved_throw_parameters_032), word(unresolved_throw_parameters_033)
  .word word(unresolved_throw_parameters_026), word(unresolved_throw_parameters_027), word(unresolved_throw_parameters_028), word(unresolved_throw_parameters_029), word(unresolved_throw_parameters_030), word(unresolved_throw_parameters_024)
  .word word(unresolved_throw_parameters_025), word(unresolved_throw_parameters_022), word(unresolved_throw_parameters_037), word(unresolved_throw_parameters_038), word(unresolved_throw_parameters_039), word(unresolved_throw_parameters_018)
  .word word(unresolved_throw_parameters_019), word(unresolved_throw_parameters_020), word(unresolved_throw_parameters_021), word(unresolved_throw_parameters_023), word(unresolved_throw_parameters_015), word(unresolved_throw_parameters_016)
  .word word(unresolved_throw_parameters_017), word(unresolved_throw_parameters_004), word(unresolved_throw_parameters_005), word(unresolved_throw_parameters_006), word(unresolved_throw_parameters_007), word(unresolved_throw_parameters_008)
  .word word(unresolved_throw_parameters_009), word(unresolved_throw_parameters_010), word(unresolved_throw_parameters_011), word(unresolved_throw_parameters_012), word(unresolved_throw_parameters_013), word(unresolved_throw_parameters_014)
  .word word(unresolved_throw_parameters_003), word(unresolved_throw_parameters_002), word(unresolved_throw_parameters_001), word(unresolved_throw_parameters_084), word(unresolved_throw_parameters_083), word(unresolved_throw_parameters_082)
  .word word(unresolved_throw_parameters_085), word(unresolved_throw_parameters_086), clip(258), clip(219), clip(317), clip(307)
  .word word(unresolved_fighter_assets_029), word(unresolved_fighter_assets_004), clip(307), clip(600), word(unresolved_body_assets_003), word(unresolved_body_assets_003)
  .word word(unresolved_body_assets_001), word(unresolved_body_assets_001), word(unresolved_body_assets_002), word(unresolved_body_assets_002), word(liu_kang_dragon_asset_slot), word(liu_kang_dragon_asset_slot)
  .zero 8
  .word word(unresolved_scene_assets_257), word(unresolved_scene_assets_257), word(shinnok_hand_asset_slot), word(shinnok_hand_asset_slot)
  .zero 24
  .word word(unresolved_scene_assets_256), word(unresolved_scene_assets_256)
  .zero 8
  .word word(unresolved_scene_assets_255), word(unresolved_scene_assets_255)
  .zero 8
  .word word(unresolved_body_assets_003), word(unresolved_body_assets_003), word(unresolved_scene_assets_350), word(unresolved_scene_assets_347), word(unresolved_scene_assets_349), word(unresolved_scene_assets_344)
  .word word(unresolved_scene_assets_352), word(unresolved_scene_assets_354), word(unresolved_scene_assets_351), word(unresolved_scene_assets_350), word(unresolved_scene_assets_346), word(unresolved_scene_assets_353)
  .word word(unresolved_scene_assets_348), word(unresolved_scene_assets_345), word(unresolved_scene_assets_342), word(unresolved_scene_assets_343), word(unresolved_scene_assets_355)
  .zero 4
  .word word(unresolved_scene_assets_258), word(unresolved_scene_assets_259), word(unresolved_scene_assets_260), word(unresolved_scene_assets_261), word(unresolved_scene_assets_258), word(unresolved_scene_assets_259)
  .word word(unresolved_scene_assets_260), word(unresolved_scene_assets_261), word(unresolved_scene_assets_258), word(unresolved_scene_assets_259), word(unresolved_scene_assets_260), word(unresolved_scene_assets_261)
  .zero 8
  .word word(unresolved_scene_parameters_001), word(unresolved_scene_parameters_002), word(unresolved_scene_parameters_003), word(unresolved_scene_parameters_004), word(unresolved_scene_parameters_005), word(unresolved_scene_parameters_006)
  .word word(unresolved_scene_assets_311), word(unresolved_scene_assets_312), word(unresolved_scene_assets_313), word(unresolved_scene_assets_314), word(unresolved_scene_assets_315), word(unresolved_scene_assets_316)
  .word word(unresolved_scene_assets_317), word(unresolved_scene_assets_318), word(unresolved_scene_assets_319), word(unresolved_scene_assets_320), word(unresolved_scene_assets_321), word(unresolved_scene_assets_322)
  .word word(unresolved_scene_assets_323), word(unresolved_scene_assets_324), word(unresolved_scene_assets_325), word(unresolved_scene_assets_326), word(unresolved_scene_assets_327), word(unresolved_scene_assets_328)
  .word word(unresolved_scene_assets_329), word(unresolved_scene_assets_330), word(unresolved_scene_assets_331), word(unresolved_scene_assets_332), word(unresolved_scene_assets_333), word(unresolved_scene_assets_334)
  .word word(unresolved_scene_assets_335), word(unresolved_scene_assets_336), word(unresolved_scene_assets_301), word(unresolved_scene_assets_302), word(unresolved_scene_assets_303), word(unresolved_scene_assets_304)
  .word word(unresolved_scene_assets_305), word(unresolved_scene_assets_306), word(unresolved_scene_assets_307), word(unresolved_scene_assets_308), word(unresolved_scene_assets_309), word(unresolved_scene_assets_310)
  .word word(unresolved_scene_assets_339), word(unresolved_scene_assets_340), word(unresolved_scene_assets_341), word(unresolved_scene_assets_337), word(unresolved_scene_assets_338)
  .zero 4
  .word word(unresolved_arenas_004), word(unresolved_arenas_005), word(unresolved_arenas_006)
  .zero 12
