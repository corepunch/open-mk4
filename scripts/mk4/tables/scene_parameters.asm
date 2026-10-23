; Finisher camera and body-part parameters; unresolved neighboring words are retained.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


spine_pull_camera_intro:
  .word 0x30000, 0x20000, 0xfffd0000, 0xfffe0000, 0x20000, 0xfffd0000
  .word 0xfffe0000, 0x20000, 0xfffd0000, 0xfffe0000, 0x20000, 0xfffd0000
  .word 0xfffe0000, 0x20000, 0xfffd0000, 0xfffe0000, 0x20000, 0xfffd0000
  .word 0xfffe0000, 0x20000, 0xfffd0000, 0xfffe0000

spine_pull_camera_orbit:
  .word 0x150000, 0xfffb0000, 0xfff90000, 0xfffd8000, 0xfffa0000, 0xfffa0000
  .word 0xfffd3334, 0xfffa0000, 0x60000, 0xfffc199a, 0x60000, 0x60000
  .word 0xfffcb334, 0x60000, 0xfffa0000, 0xfffd199a, 0xfff90000, 0xfff90000
  .word 0xfffd6667, 0xfff80000, 0x80000, 0xfffd999a, 0x90000, 0x90000
  .word 0xfffdcccd, 0xa0000, 0xfff60000, 0xfffe3334, 0xfff60000, 0xfff60000
  .word 0xfffe6667, 0xfff60000, 0xa0000, 0xfffe6667, 0xa0000, 0xa0000
  .word 0xfffe6667, 0xa0000, 0xfff60000, 0xfffe3334, 0xfff60000, 0xfff60000
  .word 0xfffe0000, 0xfff60000, 0xa0000, 0xfffdcccd, 0xa0000, 0xa0000
  .word 0xfffdb334, 0xa0000, 0xfff60000, 0xfffd999a, 0xfff60000, 0xfff60000
  .word 0xfffd999a, 0xfff60000, 0xa0000, 0xfffd999a, 0xa0000, 0xa0000
  .word 0xfffd999a, 0xa0000, 0xfff60000, 0xfffd999a, 0xfff60000, 0xfff60000
  .word 0xfffd999a, 0xfff60000, 0xa0000, 0xfffd999a, 0xa0000, 0xa0000
  .word 0xfffd999a, 0xa0000, 0xfff60000, 0xfffd999a, 0x2, 0x18ce0000
  .word 0x80
  .zero 4
  .word 0x2, 0x18ce0000, 0x60
  .zero 4
  .word 0x2, 0x18ce0000, 0x40
  .zero 4
  .word 0x2, 0x18ce0000, 0x20
  .zero 4
  .word 0x2, 0x18ce0000, 0x10
  .zero 4
  .word 0x2, 0x18ce0000, 0x8
  .zero 4
  .word 0x2, 0x18ce0000, 0x4
  .zero 4
  .word 0x8150f4b8, 0x446980, 0x8150f4d4, 0x446980, 0x8150f4f0, 0x446980
  .word 0x8150f50c, 0x446980, 0x8150f528, 0x446980, 0x8150f544, 0x446980
  .word 0x8150f560, 0x446980, 0x8150f57c, 0x446980, 0x8150f598, 0x446980
  .word 0x8150f3d8, 0x446980, 0x8150f3f4, 0x446980, 0x8150f598, 0x446980
  .word 0x8150f3d8, 0x446980, 0x8150f410, 0x446ad0, 0x8150f3f4, 0x446980
  .word 0x8150f410, 0x446ad0, 0x8150f42c, 0x446af0, 0x8150f448, 0x446b10
  .word 0x8150f464, 0x446b30, 0x8150f480, 0x446b50, 0x8150f49c, 0x446b70
  .word 0x8050f49c, 0x405dd0, 0x8150f57c, 0x446980, 0x8150f598, 0x446980
  .word 0x8150f3d8, 0x446980, 0x8150f3f4, 0x446980, 0x8150f598, 0x446980
  .word 0x8150f3d8, 0x446980, 0x8150f410, 0x446ad0, 0x8150f3f4, 0x446980
  .word 0x8150f410, 0x446ad0, 0x8150f42c, 0x446af0, 0x8150f448, 0x446b10
  .word 0x8150f464, 0x446b30, 0x8150f480, 0x446b50, 0x8150f49c, 0x446b70
  .word 0x8050f49c, 0x405dd0
  .zero 56
  .word 0x32000000, MK4_NATIVE_TASK_RETIRE
  .zero 416
  .word 0x47000291, clip(446), 0x32000000, 0x449ca0, 0x50000ab, 0x32000000
  .word 0x449cf0
  .zero 4
  .word 0x47000278, clip(447), 0x32000000, 0x44a220
  .zero 688
  .word 0x8508338, 0xe508354, 0x84508370, 0x447120, 0x650838c, 0x3508370
  .word 0x2508354
  .zero 4
  .word 0x30000, 0xfffe0000, 0xfffd0000, 0xfffe0000, 0xfffe0000, 0xfffd0000
  .word 0xfffe0000, 0xfffe0000, 0xfffd0000, 0xfffe0000, 0xfffe0000, 0xfffd0000
  .word 0xfffe0000, 0xfffe0000, 0xfffd0000, 0xfffe0000, 0xfffe0000, 0xfffd0000
  .word 0xfffe0000, 0xfffe0000, 0xfffd0000, 0xfffe0000, 0x30000, 0x10000
  .word 0xfff90000, 0xfffdb334, 0x10000, 0xfff90000, 0xfffdb334, 0x10000
  .word 0xfff90000, 0xfffdb334, 0x10000, 0xfff90000, 0xfffdb334, 0x10000
  .word 0xfff90000, 0xfffdb334, 0x10000, 0xfff90000, 0xfffdb334, 0x10000
  .word 0xfff90000, 0xfffdb334, 0x50000, 0x60000
  .zero 4
  .word 0xfffe0000
  .zero 4
  .word 0x60000, 0xfffe0000, 0xfffa0000
  .zero 4
  .word 0xfffe0000
  .zero 4
  .word 0xfffb0000, 0xfffe0000
  .zero 4
  .word 0xfffc4ccd, 0xfffe0000, 0xb333, 0xfffd8000, 0xfffe0000, 0xb333
  .word 0xfffd8000, 0xfffe0000, 0xb333, 0xfffd8000, 0xfffe0000, 0xb333
  .word 0xfffd8000, 0xfffe0000, 0xb333, 0xfffd8000, 0xfffe0000
  .zero 4
  .word 0x30000, 0xffff3334, 0xfffcb334, 0xfffe0000, 0xffff3334, 0xfffcb334
  .word 0xfffe0000, 0xffff3334, 0xfffcb334, 0xfffe0000, 0xffff3334, 0xfffcb334
  .word 0xfffe0000, 0xffff3334, 0xfffcb334, 0xfffe0000, 0xffff3334, 0xfffcb334
  .word 0xfffe0000, 0xffff3334, 0xfffcb334, 0xfffe0000, 0x50000, 0xfff40000
  .word 0x80000, 0xfffb0000, 0xc0000, 0xa0000, 0xfffb0000, 0x80000
  .word 0xfff40000, 0xfffe0000, 0x20000, 0xfffa0000, 0xffff0000, 0x20000
  .word 0xfffa0000, 0xfffd8000, 0x20000, 0xfffa0000, 0xfffd8000, 0x20000
  .word 0xfffa0000, 0xfffd8000, 0x20000, 0xfffa0000, 0xfffd8000
  .zero 4
  .word 0x30000, 0xfffd3334, 0xfffd3334, 0xfffcc51f, 0xfffd3334, 0xfffd3334
  .word 0xfffcc51f, 0xfffd3334, 0xfffd3334, 0xfffcc51f, 0xfffd3334, 0xfffd3334
  .word 0xfffcc51f, 0xfffd3334, 0xfffd3334, 0xfffcc51f, 0xfffd3334, 0xfffd3334
  .word 0xfffcc51f, 0xfffd3334, 0xfffd3334, 0xfffcc51f, 0xfffd3334, 0xfffd3334
  .word 0xfffcc51f
  .zero 4
  .word 0x40000
  .zero 4
  .word 0xfff00000, 0xfff90000, 0x34ccc, 0xfffa3334, 0xfffd8000, 0x34ccc
  .word 0xfffa3334, 0xfffd8000, 0x34ccc, 0xfffa3334, 0xfffd8000, 0x34ccc
  .word 0xfffa3334, 0xfffd8000, 0x34ccc, 0xfffa3334, 0xfffd8000, 0x34ccc
  .word 0xfffa3334, 0xfffd8000, 0xc0000, 0x20000, 0xfff80000, 0xfffd0000
  .word 0x40000, 0xfffc0000, 0xfffb0000, 0x60000, 0x40000, 0xffff0000
  .word 0xfffa8000, 0x40000, 0xffff0000, 0xfffa8000, 0xfffc0000, 0xffff0000
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a
  .word 0x2f333, 0xfffa0000, 0xfffd999a, 0x2f333, 0xfffa0000, 0xfffd999a

leg_tear_camera_side:
  .word 0x30000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000
  .zero 4
  .word 0xfffa0000, 0xfffe0000

leg_tear_camera_orbit:
  .word 0x40000, 0x88000, 0xfff80000, 0xfffe4ccd, 0x60000, 0x60000
  .word 0xfffe8000, 0x40000, 0x48000, 0xfffeb334, 0xfff80000, 0x80000
  .word 0xfffeb334, 0xfff80000, 0xfff80000, 0xfffeb334, 0x80000, 0xfff80000
  .word 0xfffeb334, 0x80000, 0x80000, 0xfffeb334, 0xfff80000, 0x80000
  .word 0xfffeb334, 0xfff80000, 0xfff80000, 0xfffeb334, 0x80000, 0xfff80000
  .word 0xfffeb334, 0x80000, 0x80000, 0xfffeb334, 0x2, 0x7
  .word 0x8, 0xd, 0xe, 0x4, 0x5, 0x6
  .word 0xf, 0x10, 0x3, 0x4
  .zero 8
  .word 0x150db74, 0x150db90, 0x150dbac, 0x150dbc8, 0x250dbe4, 0x250dc00
  .word 0x250d89c, 0x150d8b8, 0x150d8d4, 0x150d8f0, 0x150d928, 0x150d960
  .word 0x8150d998, 0x405dd0, 0x9, 0x147a, 0x10, 0x1999
  .word 0x5, 0xffffe667, 0xd, 0x3333, 0x3, 0x4ccc
  .word 0xffffffff
  .zero 4
  .word 0xb, 0x4000, 0xa, 0x147a, 0xf, 0x1999
  .word 0x6, 0xffffe667, 0x7, 0x1999, 0xffffffff
  .zero 4
  .word 0x8, 0x1999, 0xc, 0x4000, 0xe, 0x3333
  .word 0xffffffff
  .zero 4
  .word 0x9, 0x147a, 0x10, 0x1999, 0x5, 0xffffe667
  .word 0xd, 0x3333, 0x3, 0x4ccc, 0xffffffff
  .zero 4
  .word 0xb, 0x4000, 0xa, 0x147a, 0xf, 0x1999
  .word 0x6, 0xffffe667, 0x7, 0x1999, 0xffffffff
  .zero 4
  .word 0x8, 0x1999, 0xc, 0x4000, 0xe, 0x3333
  .zero 8

head_hand_part_parameters:
  .word 0x2, 0x1999, 0x9, 0x147a, 0xa, 0x147a
  .zero 8
  .word 0x30000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000
  .zero 4
  .word 0xfff8cccd, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000
  .zero 4
  .word 0xfff9999a, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000
  .zero 4
  .word 0xfffa6667, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000
  .zero 4
  .word 0xfffb3334, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000
  .zero 4
  .word 0xfffc0000, 0xfffe0000, 0x30000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 4
  .word 0xfff80000, 0xfffe0000
  .zero 32
  .word 0x44f9b0
  .zero 60
  .word 0x75, 0x7e, 0x94, 0x7f, 0xa7, 0x77
  .word 0x7b, 0x7c, 0x7d, 0x96, 0x91, 0xc1
  .zero 8
  .word 0x93, 0x92, 0xc1
  .zero 4
  .word 0x50000
  .zero 4
  .word 0xfff50000, 0xfffd0000, 0xfffc0000, 0xfff70000, 0xfffd3334, 0xfff90000
  .word 0xfff90000, 0xfffe3334, 0xfffa0000, 0xffff0000, 0xfffe8000, 0xfffc0000
  .word 0xffff0000, 0xfffecccd, 0xfffc6667, 0xffff0000, 0xffff3334, 0xfffc6667
  .word 0xffff0000, 0xffff4ccd, 0xfffc6667, 0xffff0000, 0xffff599a
  .zero 4
  .word 0x10000, 0xfff90000, 0x8c000, 0xfffe0000, 0xfff90000, 0x8c000
  .word 0xfffe0000, 0xfff90000, 0x8c000, 0xfffe0000, 0xfff90000, 0x8c000
  .word 0xfffe0000
  .zero 4
  .word 0x10000
  .zero 4
  .word 0xfff50000, 0xfffd0000
  .zero 4
  .word 0xfff50000, 0xfffd0000
  .zero 4
  .word 0xfff50000, 0xfffd0000
  .zero 4
  .word 0xfff50000, 0xfffd0000
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_001:
  .word 0x150ad1c, 0x150ad38, 0x150ad54, 0x150ad70, 0x150ad8c, 0x150ada8
  .word 0x150adc4, 0x150ade0, 0x150adfc, 0x150ae18, 0x150ae34, 0x150ae50
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_002:
  .word 0x250ad1c, 0x150ad38, 0x150ad54, 0x150ad70, 0x150ad8c, 0x150ada8
  .word 0x150adc4, 0x150ade0, 0x150adfc, 0x150ae18, 0x150ae34, 0x150ae50
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_003:
  .word 0x150ad1c, 0x150ad38, 0x250ad54, 0x150ad70, 0x150ad8c, 0x150ada8
  .word 0x150adc4, 0x150ade0, 0x250adfc, 0x150ae18, 0x150ae34, 0x150ae50
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_004:
  .word 0x150ad1c, 0x250ad38, 0x150ad54, 0x150ad70, 0x150ad8c, 0x250ada8
  .word 0x150adc4, 0x150ade0, 0x150adfc, 0x250ae18, 0x150ae34, 0x150ae50
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_005:
  .word 0x150ad1c, 0x250ad38, 0x150ad54, 0x150ad70, 0x250ad8c, 0x150ada8
  .word 0x250adc4, 0x150ade0, 0x150adfc, 0x250ae18, 0x150ae34, 0x150ae50
  .zero 8

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_006:
  .word 0x250ad1c, 0x150ad38, 0x250ad54, 0x150ad70, 0x150ad8c, 0x250ada8
  .word 0x150adc4, 0x250ade0, 0x150adfc, 0x150ae18, 0x250ae34, 0x150ae50
  .zero 8
  .word 0x39, 0xfffff334, 0x1eb8, 0x51e, 0x28f5, 0xfffff0a4
  .word 0xffffdeb9, 0xffffd70b, 0xf5c, 0xffffd99a, 0x7ae, 0x3851
  .word 0xffffb334, 0xfffff852, 0xffffc7af, 0xffffb5c3, 0x3ae1, 0x23d7
  .word 0xffff8a3e, 0xffffc51f, 0x2147, 0xffff8a3e
  .zero 4
  .word 0xffffbae2, 0xffff8a3e, 0x51eb, 0xfffff852, 0xffff6148, 0xffffdeb9
  .word 0x4a3d, 0xffff5eb9, 0xffffcf5d, 0xffffbd71, 0xffff6148, 0xa3d
  .word 0xffffa3d8, 0xffff35c3, 0x5c28, 0xa3d, 0xffff35c3, 0xfffff5c3
  .word 0x5c28, 0xffff3ae2, 0xffffa3d8, 0xfffff5c3, 0xffff3852, 0xffffd70b
  .word 0x6147, 0xffff0f5d, 0xffff9eb9, 0xffffd70b, 0xffff0ccd, 0x28f5
  .word 0xffff9eb9, 0xffff0f5d, 0x6147, 0x28f5, 0xffff0f5d, 0xffffa3d8
  .word 0xffffb5c3, 0xfffee3d8, 0x28f5, 0xffff91ec, 0xfffee3d8, 0x75c2
  .word 0x51e, 0xfffee3d8, 0x1eb8, 0x7333, 0xfffee3d8, 0xffff9c29
  .word 0x428f, 0xfffee667, 0x7ae, 0xffff7d71, 0xfffebd71, 0x7d70
  .word 0xffffdeb9, 0xfffebd71, 0x47ae, 0x6e14, 0xfffebd71, 0xffffae15
  .word 0x6666, 0xfffebd71, 0xffff851f, 0xffffd1ec, 0xfffebae2, 0xffff851f
  .word 0x47ae, 0xfffe947b, 0xffff851f, 0xffffb852, 0xfffe91ec
  .zero 4
  .word 0xffff70a4, 0xfffe970b, 0x7ae1, 0xffffb852, 0xfffe947b, 0x7ae1
  .word 0x47ae, 0xfffe91ec
  .zero 4
  .word 0x8f5c, 0xfffe91ec, 0xffff7d71, 0x547a, 0xfffe68f6, 0xffff6e15
  .word 0xffffcf5d, 0xfffe68f6, 0xffffca3e, 0xffff6e15, 0xfffe68f6, 0x51eb
  .word 0xffff7ae2, 0xfffe6e15, 0x9999, 0xffffee15, 0xfffe6b86, 0x6e14
  .word 0x6b85, 0xfffe68f6, 0xfffff0a4, 0x9999, 0xfffe6b86, 0xffff7334
  .word 0xffffa667, 0xfffe451f, 0xffffee15, 0xffff599a, 0xfffe4000, 0x75c2
  .word 0xffff8a3e, 0xfffe4290, 0xa666, 0x11eb, 0xfffe451f, 0x5999
  .word 0x8ccc, 0xfffe4290, 0xffffc7af, 0x9eb8, 0xfffe4000, 0xffff6148
  .word 0x3851, 0xfffe4290, 0x851e, 0x7ae1, 0xfffe199a, 0x7ae
  .word 0xb333, 0xfffe199a, 0xffff851f, 0x851e, 0xfffe199a, 0xffff4ccd
  .word 0x7ae, 0xfffe199a, 0xffff7ae2, 0xffff851f, 0xfffe199a, 0xfffff852
  .word 0xffff4ccd, 0xfffe199a, 0x7ae1, 0xffff7ae2, 0xfffe170b, 0xb333
  .word 0xfffff852, 0xfffe199a, 0xffffa667
  .zero 8
  .word 0x5999
  .zero 12
  .word 0xffffa667
  .zero 8
  .word 0x5999
  .zero 12
  .word 0xffffc000
  .zero 8
  .word 0x4000, 0x450e90, 0x450fe0, 0x450fe0
  .zero 4
  .word 0x450e70, 0x450fc0, 0x450fc0
  .zero 4
  .word 0x1, 0x4000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0xffff8000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1, 0xffff8000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xffff0000, 0x1, 0x1, 0xffff4000, 0xffffb334
  .word 0xfffe0000
  .zero 4
  .word 0x1, 0x8000, 0xffffb334, 0xfffe0000
  .zero 8
  .word 0xfffe8000, 0xffffb334
  .zero 8
  .word 0x1, 0xc000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xffff4000, 0xffffb334, 0xfffd0000, 0x1, 0x1
  .word 0x4000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1, 0x10000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0xffff4000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x1, 0x8000, 0xffffb334
  .zero 4
  .word 0x1, 0x1, 0x10000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0xc000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xfffd8000, 0x1, 0x1, 0xc000, 0xffffb334
  .word 0xffff8000
  .zero 4
  .word 0x1, 0x4000, 0xffffb334, 0xfffe0000
  .zero 4
  .word 0x1, 0xffffc000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0xffffc000, 0xffffb334, 0xfffd0000, 0x1
  .zero 4
  .word 0xfffe8000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0xffff0000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xfffec000, 0xffffb334
  .zero 8
  .word 0x1, 0x4000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x2, 0xfffe6667, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xffff0000, 0xffffb334, 0xffff8000, 0x1, 0x1
  .word 0xffff0000, 0xffffb334, 0xfffe0000
  .zero 4
  .word 0x1, 0x8000, 0xffffb334, 0xffff8000
  .zero 8
  .word 0xfffe8000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0x8000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x1, 0xffff8000, 0xffffb334, 0xfffe0000, 0x1, 0x1
  .word 0xc000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x1, 0xfffec000, 0xffffb334, 0xfffe0000
  .zero 4
  .word 0x2, 0xfffe8000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1, 0x4000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xffffc000, 0xffffb334, 0xfffe0000, 0x1, 0x1
  .word 0xffff0000, 0xffffb334, 0xfffd8000
  .zero 4
  .word 0x1, 0xc000, 0xffffb334, 0xfffe0000
  .zero 4
  .word 0x1, 0xffff8000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xffff0000, 0xffffb334
  .zero 4
  .word 0x1, 0x1, 0xfffec000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0x8000, 0xffffb334, 0xfffd0000
  .zero 4
  .word 0x1, 0xffffc000, 0xffffb334
  .zero 8
  .word 0x1, 0xffff4000, 0xffffb334, 0xffff0000, 0x1, 0x1
  .word 0xc000, 0xffffb334
  .zero 8
  .word 0x1, 0xffff4000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334
  .zero 8
  .word 0x1, 0xc000, 0xffffb334, 0xfffd0000, 0x1, 0x1
  .word 0xffffc000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1, 0xffff4000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0x1, 0x10000, 0xffffb334
  .zero 4
  .word 0x1, 0x1, 0xffff0000, 0xffffb334, 0xfffe8000
  .zero 4
  .word 0x1, 0xffffc000, 0xffffb334, 0xffff0000
  .zero 4
  .word 0x1, 0xffff4000, 0xffffb334
  .zero 8
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xfffe0000, 0x1, 0x1, 0x8000, 0xffffb334
  .word 0xffff0000
  .zero 4
  .word 0x1
  .zero 4
  .word 0xffffb334, 0xfffd0000
  .zero 4
  .word 0x1, 0x4000, 0xffffb334
  .zero 8
  .word 0x1, 0x8000, 0xffffb334, 0xfffe8000, 0x1, 0x1
  .word 0xffff8000, 0xffffb334, 0xfffd0000
  .zero 4
  .word 0x1, 0x4000, 0xffffb334, 0xfffd0000
  .zero 4
  .word 0x1, 0xffff8000, 0xffffb334
  .zero 8
  .word 0x1, 0xffffc000, 0xffffb334, 0xfffd8000, 0x1, 0x1
  .word 0xffff8000, 0xffffb334, 0xffff8000
  .zero 4
  .word 0xffffffff
  .zero 24
  .word 0xffff0000
  .zero 8
  .word 0x2, 0x3, 0x5, 0x3, 0x6, 0x3
  .word 0xb, 0x3, 0xc, 0x3, 0x9, 0x3
  .word 0xa, 0x3, 0xf, 0x3, 0x10, 0x3
  .word 0xd, 0x3, 0xe, 0x3, 0x7, 0x3
  .word 0x5, 0x3, 0xffffffff
  .zero 4
  .word 0x2, 0x3, 0x2, 0x3, 0x2, 0x3
  .word 0xffffffff
  .zero 4
  .word 0x10000, 0x20000, 0xfff70000, 0xfffd8000, 0x20000, 0xfff70000
  .word 0xfffd8000, 0x20000, 0xfff70000, 0xfffd8000, 0x20000, 0xfff70000
  .word 0xfffd8000, 0x20000, 0xfff70000, 0xfffd8000, 0x20000, 0xfff70000
  .word 0xfffd8000, 0x20000, 0xfff70000, 0xfffd8000, 0xf0000, 0x80000
  .word 0xfffa0000, 0xfffd3334
  .zero 4
  .word 0xfff70000, 0xfffd3334, 0xffff0000, 0xfff70000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x10000
  .word 0xfff60000, 0xfffc999a, 0x10000, 0xfff60000, 0xfffc999a, 0x350ab5c
  .word 0x350ab78, 0x350ab94, 0x350abb0, 0x350abcc, 0x350abe8, 0x250ac04
  .word 0x350ac20, 0x250ac3c, 0x350ac58, 0x250ac74, 0x350ac90, 0x350acac
  .word 0x350acc8, 0x350ace4, 0x350ad00, 0x8050ad00, 0x405dd0, 0x3510a28
  .word 0x3510a44, 0x3510a60, 0x3510a7c, 0x3510a98, 0x3510ab4, 0x2510ad0
  .word 0x3510aec, 0x2510b08, 0x3510b24, 0x2510b40, 0x3510b5c, 0x3510b78
  .word 0x3510b94, 0x3510bb0, 0x3510bcc, 0x80510bcc, 0x405dd0
  .zero 760
  .word 0x20000, 0xfffb4000, 0xfffc7bdb, 0xfffee000, 0xfffb4000, 0xfffc7bdb
  .word 0xfffee000, 0xfffb4000, 0xfffc7bdb, 0xfffee000, 0xfffb4000, 0xfffc7bdb
  .word 0xfffee000, 0xfffb4000, 0xfffc7bdb, 0xfffee000, 0xfffb4000, 0xfffc7bdb
  .word 0xfffee000
  .zero 4
  .word 0x10000
  .zero 4
  .word 0xfff90000, 0xfffd5efa
  .zero 4
  .word 0xfff90000, 0xfffd5efa
  .zero 4
  .word 0xfff90000, 0xfffd5efa
  .zero 4
  .word 0xfff90000, 0xfffd5efa
  .zero 4
  .word 0xfff90000, 0xfffd5efa, 0x10000, 0x30000, 0xffff0000, 0xfffd8000
  .word 0x30000, 0xffff0000, 0xfffd8000, 0x30000, 0xffff0000, 0xfffd8000
  .word 0x30000, 0xffff0000, 0xfffd8000
  .zero 4
  .word 0x6f333, 0x30000, 0x80000, 0xfffd8000, 0xfff90000, 0x50000
  .word 0xfffd8000, 0xfffb0000, 0xfffb0000, 0xfffd8000, 0x20000, 0xfffc4000
  .word 0xfffdc000, 0x30000, 0xfffd0000, 0xfffde667, 0x4b333, 0xfffe0000
  .word 0xfffe4ccd, 0x60000, 0xfffe451f, 0xfffe4ccd, 0x60000, 0xfffe451f
  .word 0xfffe4ccd, 0x60000, 0xfffe451f, 0xfffe4ccd, 0x60000, 0xfffe451f
  .word 0xfffe4ccd
  .zero 4
  .word 0x10000, 0x4844d, 0x12000, 0xfffda667, 0x4844d, 0x12000
  .word 0xfffda667, 0x4844d, 0x12000, 0xfffda667, 0x4844d, 0x12000
  .word 0xfffda667, 0x4844d, 0x12000, 0xfffda667, 0x150db74, 0x150db90
  .word 0x150dbac, 0x150dbc8, 0x250dbe4, 0x250dc00, 0x250d89c, 0x150d8b8
  .word 0x150d8d4, 0x150d8f0, 0x150d928, 0x150d960, 0x8150d998, 0x405dd0
  .word 0x1506594, 0x15065b0, 0x15065cc, 0x15065e8, 0x1506604, 0x1506620
  .word 0x150663c, 0x1506658, 0x1506674, 0x7f506690, 0x7f506690, 0x7f506690
  .word 0x6f506690, 0x2506674, 0x2506658, 0x150663c, 0x2506620, 0x1506604
  .word 0x25065e8, 0x15065cc, 0x25065b0, 0x1506594, 0x80506594, 0x405dd0
  .word 0x29, 0xa, 0x2a, 0x14, 0x1b, 0xa
  .word 0x33, 0xa, 0x1c, 0x14, 0x2b, 0x3
  .word 0x2a, 0x4, 0x2b, 0x3, 0x29, 0x4
  .word 0x2a, 0x3, 0x29, 0x4, 0x2b, 0x4
  .word 0x29, 0x4, 0x29, 0x4, 0x2a, 0x3
  .word 0x29, 0x4, 0x2b, 0x3, 0x2a, 0x4
  .word 0x2b, 0x3, 0x2a, 0x4, 0x1b, 0x6
  .word 0x33, 0xd
  .zero 8
  .word 0xcccc, 0x4ccc, 0xfff50000, 0xfffb0000, 0x4ccc, 0xfff30000
  .word 0xfffb0000, 0x4ccc, 0xfff20000, 0xfffb0000, 0x4ccc, 0xfff18000
  .word 0xfffb0000
  .zero 4
  .word 0x70000, 0x10000, 0xfff30000, 0xfffd0000, 0x20000, 0xfff40000
  .word 0xfffd0000, 0x28000, 0xfff44000, 0xfffd0000, 0x2c000, 0xfff4c000
  .word 0xfffd0000, 0x2e000, 0xfff58000, 0xfffd0000, 0x2f000, 0xfff60000
  .word 0xfffd0000, 0x2f800, 0xfff60000, 0xfffd0000, 0x2fc00, 0xfff60000
  .word 0xfffd0000, 0x2fe00, 0xfff60000, 0xfffd0000, 0x2ff00, 0xfff60000
  .word 0xfffd0000
  .zero 4
  .word 0x50000, 0xc0000, 0xfff40000, 0xfffe0000, 0xc0000, 0xc0000
  .word 0xfffe0ccd, 0xfff60000, 0xa0000, 0xfffe199a, 0xfff70000, 0xfff70000
  .word 0xfffe3334, 0xffff0000, 0xfff80000, 0xfffe4ccd, 0xffff0000, 0xfffa0000
  .word 0xfffe4ccd, 0xffff0000, 0xfffa0000, 0xfffe4ccd, 0xffff0000, 0xfffa0000
  .word 0xfffe4ccd, 0xffff0000, 0xfffa0000, 0xfffe4ccd, 0x2, 0x9
  .word 0x10
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_007:
  .word 0xfe, 0xff, 0x20, 0x5d, 0x61, 0x7e
  .zero 8
  .word word(unresolved_body_assets_029), word(unresolved_body_assets_028), word(unresolved_body_assets_093), word(unresolved_body_assets_067), word(unresolved_body_assets_068), word(unresolved_body_assets_069)
  .word word(unresolved_body_assets_070), word(unresolved_body_assets_071), word(unresolved_body_assets_072), word(unresolved_body_assets_073), word(unresolved_body_assets_074), word(unresolved_body_assets_075)
  .word word(unresolved_body_assets_076), word(unresolved_body_assets_077), word(unresolved_body_assets_078), word(unresolved_body_assets_079), word(unresolved_body_assets_080), word(unresolved_body_assets_081)
  .word word(unresolved_body_assets_031), word(unresolved_body_assets_032), word(unresolved_body_assets_033), word(unresolved_body_assets_034), word(unresolved_body_assets_035), word(unresolved_body_assets_036)
  .word word(unresolved_body_assets_037), word(unresolved_body_assets_038), word(unresolved_body_assets_039), word(unresolved_body_assets_040), word(unresolved_body_assets_082), word(unresolved_body_assets_083)
  .word word(unresolved_body_assets_084), word(unresolved_body_assets_085), word(unresolved_body_assets_086), word(unresolved_body_assets_087), word(unresolved_body_assets_088), word(unresolved_body_assets_041)
  .word word(unresolved_body_assets_042), word(unresolved_body_assets_043), word(unresolved_body_assets_044), word(unresolved_body_assets_045), word(unresolved_body_assets_046), word(unresolved_body_assets_047)
  .word word(unresolved_body_assets_048), word(unresolved_body_assets_049), word(unresolved_body_assets_050), word(unresolved_body_assets_051), word(unresolved_body_assets_052), word(unresolved_body_assets_053)
  .word word(unresolved_body_assets_054), word(unresolved_body_assets_055), word(unresolved_body_assets_056), word(unresolved_body_assets_057), word(unresolved_body_assets_058), word(unresolved_body_assets_059)
  .word word(unresolved_body_assets_060), word(unresolved_body_assets_061), word(unresolved_body_assets_062), word(unresolved_body_assets_063), word(unresolved_body_assets_064), word(unresolved_body_assets_065)
  .word word(unresolved_body_assets_066), word(unresolved_body_assets_089), word(unresolved_body_assets_090), word(unresolved_body_assets_091), word(unresolved_body_assets_041), word(unresolved_body_assets_042)
  .word word(unresolved_body_assets_043), word(unresolved_body_assets_044), word(unresolved_body_assets_045), word(unresolved_body_assets_046), word(unresolved_body_assets_047), word(unresolved_body_assets_048)
  .word word(unresolved_body_assets_049), word(unresolved_body_assets_050), word(unresolved_body_assets_051), word(unresolved_body_assets_052), word(unresolved_body_assets_053), word(unresolved_body_assets_054)
  .word word(unresolved_body_assets_055), word(unresolved_body_assets_056), word(unresolved_body_assets_057), word(unresolved_body_assets_058), word(unresolved_body_assets_059), word(unresolved_body_assets_060)
  .word word(unresolved_body_assets_061), word(unresolved_body_assets_062), word(unresolved_body_assets_063), word(unresolved_body_assets_064), word(unresolved_body_assets_065), word(unresolved_body_assets_066)
  .word word(unresolved_body_assets_095), word(unresolved_body_assets_094), word(unresolved_body_assets_096), word(unresolved_body_assets_092)

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_008:
  .word 0x30  ; numeric; printable bytes do not establish text
  .word 0x39
  .zero 4
  .word 0x106, word(unresolved_scene_assets_378), word(unresolved_scene_assets_379), word(unresolved_scene_assets_380), word(unresolved_scene_assets_381), word(unresolved_scene_assets_382)
  .word word(unresolved_scene_assets_383), word(unresolved_scene_assets_384), word(unresolved_scene_assets_385), word(unresolved_scene_assets_386), word(unresolved_scene_assets_387)

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_009:
  .word 0x30  ; numeric; printable bytes do not establish text
  .word 0x39
  .zero 4
  .word 0x83, word(unresolved_body_assets_097), word(unresolved_body_assets_098), word(unresolved_body_assets_099), word(unresolved_body_assets_100), word(unresolved_body_assets_101)
  .word word(unresolved_body_assets_102), word(unresolved_body_assets_103), word(unresolved_body_assets_104), word(unresolved_body_assets_105), word(unresolved_body_assets_106)

; Retained referenced block; semantic role is not established.
unresolved_scene_parameters_010:
  .word 0x30  ; numeric; printable bytes do not establish text
  .word 0x39
  .zero 4
  .word 0x41, word(unresolved_body_assets_107), word(unresolved_body_assets_108), word(unresolved_body_assets_109), word(unresolved_body_assets_110), word(unresolved_body_assets_111)
  .word word(unresolved_body_assets_112), word(unresolved_body_assets_113), word(unresolved_body_assets_114), word(unresolved_body_assets_115), word(unresolved_body_assets_116)

camera_shake_rows:
  .word 0x3, 0x4, 0x2666, 0x2, 0x2, 0x1999
  .word 0x2, 0x2, 0x1999, 0x4, 0x3, 0x2666
  .word 0xa, 0x3, 0x2e14, 0x2, 0x3, 0x2e14
  .word 0x3, 0x3, 0x3333, 0x2, 0x3, 0xf5c
  .word 0x6, 0x3, 0x2e14, 0x2, 0x2, 0x7ae
  .word 0x1, MK4_NATIVE_TASK_RETIRE, 0x2, MK4_NATIVE_TASK_RETIRE, 0x4, 0x458770
  .word 0x8, 0x4587e0, 0x100, MK4_NATIVE_TASK_RETIRE, 0x200, MK4_NATIVE_TASK_RETIRE
  .word 0x400, 0x458790, 0x800, 0x458800, 0x8c
  .zero 20
  .word 0xb333
  .zero 4
  .word 0xfff80000, 0x273
  .zero 8
  .word 0xfff80000, 0x272, 0xffff4ccd
  .zero 4
  .word 0xfff80000, 0x271, 0x41, 0x42, 0x43, 0x44
  .word 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a
  .word 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50
  .word 0x51, 0x52, 0x53, 0x54, 0x55, 0x56
  .word 0x57, 0x58, 0x59, 0x5a, 0x30, 0x31
  .word 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
  .word 0x38, 0x39, 0x7b, 0x7c, 0x7d, 0xffffffff
  .word 0xe75, 0xe7a, 0xe7f, 0xe84, 0xe89, 0xe8e
  .word 0xe93, 0xe98, 0xe9d, 0xea2, 0xea7, 0xeac
  .word 0xeb1, 0xeb6, 0xebb, 0xec0, 0xec5, 0xeca
  .word 0xecf, 0xed4, 0xed9, 0xede, 0xee3, 0xee8
  .word 0xeed, 0xef2, 0xf3c, 0xf37, 0xf32, 0xf2d
  .word 0xf28, 0xf23, 0xf1e, 0xf19, 0xf14, 0xf0f
  .word 0xefb, 0xef6, 0xf00
  .zero 4
  .word 0x5, 0x14, 0xa, 0x5, 0x14, 0x2
  .word 0x3, 0x8, 0x2, 0x3, 0x8, 0xa
  .word 0x13, 0x8, 0x13, 0x2, 0x14, 0xc
  .word 0xa, 0x14, 0xc
  .zero 4
  .word 0x12, 0x12, 0x9, 0x4, 0x16, 0xd
  .word 0x8, 0x6, 0x12, 0x4, 0x17, 0x5
  .zero 4
  .word 0x6, 0x6
  .zero 4
  .word 0x18, 0xa, 0xa, 0xa, 0xfffffff5, 0xfffffff5
  .word 0xfffffff5, 0x3, 0x8, 0x17, 0x5, 0x14
  .word 0x17, 0x12, 0x14, 0x17, 0x12, 0x14
  .word 0xa, 0xffffffff, 0x4, 0x9, 0x1, 0x13
  .word 0x11
  .zero 4
  .word 0xc
  .zero 4
  .word 0x1
  .zero 4
  .word 0x4594b0, 0x4599e0, 0x459e40, 0x45a6e0, 0x45a330, 0x45a580
  .word 0x459b70, 0x459c20, 0x459cd0, 0x459ac0, 0x45a490, 0x4594b0
  .word 0x45a510, 0x45a560, 0x45a800, 0x45a850, 0x45a8b0, 0x45a9c0
  .word 0x45aa50, 0x45aa80, 0x45aab0, 0x4594b0, 0x45baf0, 0x45a3e0
  .word 0x4594b0, 0x459f30, 0x45a070, 0x45a0c0, 0x45a0f0, 0x45a4d0
  .word MK4_NATIVE_DISPATCH_OPPONENT_REACTION, 0x459570, 0x459530, 0x4594b0, 0x45b1d0, 0x4594b0
  .word 0x459800, 0x4596e0, 0x4596a0, 0x459660, 0x45b1b0, 0x45b190
  .word 0x4595a0, 0x459620, 0x4595e0, 0x459760, 0x459780, 0x4597a0
  .word 0x459820
  .zero 4
  .word 0x459890, 0x45b3c0, 0x459830, 0x459960, 0x45b570, 0x4598b0
  .word 0x45b140, 0x45b170, 0x45b3f0, 0x45b420, 0x45b4a0, 0x45b500
  .word 0x45b530, MK4_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN, 0x459990, MK4_NATIVE_SCRIPT_REACTION_DISPATCH, 0x45b5c0, 0x45b5f0
  .word 0x45b620, 0x45b650, 0x45b800, 0x45b8b0, 0x45b960, 0x45ba00
  .word 0x45ba40, 0x45bb10, 0x45bbc0, 0x45a630, 0x45ac70, 0x45abf0
  .word 0x45ac80, 0x45ab30, 0x45ab70, 0x45aaf0, 0x45acf0, 0x45ad90
  .word MK4_NATIVE_CONTEXT, 0x4594b0, 0x45aef0, 0x45af60, 0x45afb0, 0x45b010
  .word 0x45b110, 0x45b0d0, 0x45abb0, 0x45afe0, 0x45b2f0, 0x45b3a0
  .word 0x45b310, 0x45b350, MK4_NATIVE_FACE_PEER_AND_RESUME_SCRIPT, 0x45ae80, 0x4594b0, 0x4594b0
  .word 0x45b750, 0x45b6d0, 0x45a780, 0x459720, 0x45b050, 0x45ae50
  .word 0x45b030, 0x4597d0, 0x45b270, 0x45bc70, 0x45c050, 0x45c070
  .word 0x45c100, 0x45c150, 0x45c1c0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45c9b0, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ca10, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ca30, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ca50, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ca70, 0x45ca90, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cb40, 0x45caf0, 0x45cb60
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cbe0, 0x45cb90, 0x45cc00
  .word 0x45cc20, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cc80, 0x45cc40, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cd00, 0x45ccb0, 0x45cd20
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cd60, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cda0, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ce40
  .word 0x45cdf0, MK4_NATIVE_TASK_RETIRE, 0x45ce60, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x458590
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45ce80, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cee0, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cf00, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cf20, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45cf90, 0x45cf40, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d010, 0x45cfc0, 0x45d030
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d3b0, 0x45d360, 0x45d3d0
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d440, 0x45d400, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d4c0, 0x45d470, 0x45d4e0
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d560, 0x45d510, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42f690, MK4_NATIVE_TASK_RETIRE, 0x462a60, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word 0x4585b0
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d7d0, 0x45d7e0, 0x45d5f0, MK4_NATIVE_TASK_RETIRE
  .word 0x45d800, 0x42ed00, MK4_NATIVE_TASK_RETIRE, 0x462880, MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE
  .zero 4
  .word MK4_NATIVE_TASK_RETIRE, MK4_NATIVE_TASK_RETIRE, 0x45d850, 0x45d860, 0x45d5c0, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x42ed40, MK4_NATIVE_TASK_RETIRE, 0x4628d0, 0x45d850, MK4_NATIVE_TASK_RETIRE
  .word MK4_NATIVE_TASK_RETIRE, 0x430420, 0x60
  .zero 4
  .word 0x70, 0x7, 0x6000
  .zero 4
  .word 0x7000, 0x70, 0x50, 0x3, 0x70, 0x7
  .word 0x5000, 0x30, 0x7000, 0x70, 0x30
  .zero 4
  .word 0x71, 0x7, 0x3000
  .zero 4
  .word 0x7100, 0x70, 0x60, 0x3, 0x71, 0x7
  .word 0x6000, 0x30, 0x7100, 0x70, 0x20, 0x4
  .word 0x71, 0x7, 0x2000, 0x40, 0x7100, 0x70
  .word 0x60, 0x2, 0x71, 0x7, 0x6000, 0x20
  .word 0x7100, 0x70, 0x30, 0x4, 0x71, 0x7
  .word 0x3000, 0x40, 0x7100, 0x70, 0x20, 0x1
  .word 0x70, 0x7, 0x2000, 0x10, 0x7000, 0x70
  .word 0x20, 0x2, 0x71, 0x7, 0x2000, 0x20
  .word 0x7100, 0x70, 0x45, 0x46, 0x20, 0x1
  .word 0x73, 0x7, 0x2000, 0x10, 0x7300, 0x70
  .word 0x22, 0x1, 0x73, 0x7, 0x2200, 0x10
  .word 0x7300, 0x70, 0x45, 0x46, 0x60
  .zero 4
  .word 0x71, 0x7, 0x6000
  .zero 4
  .word 0x7100, 0x70, 0x20, 0x3, 0x71, 0x7
  .word 0x2000, 0x30, 0x7100, 0x70, 0x10, 0x45ca60
  .word 0x20, 0x45cb80, 0x40, 0x45cc30, 0x1000, 0x45cf30
  .word 0x2000, 0x45d050, 0x4000, 0x45d3f0, 0x1, 0x45c9a0
  .word 0x2, 0x45ca00, 0x4, 0x45ca20, 0x8, 0x45ca40
  .word 0x100, 0x45ce70, 0x200, 0x45ced0, 0x400, 0x45cef0
  .word 0x800, 0x45cf10
  .zero 8
  .word 0x1, 0x45cae0, 0x2, 0x45cca0, 0x4, 0x45cde0
  .word 0x10, 0x45cfb0, 0x20, 0x45d460, 0x40, 0x45d500
  .zero 8
  .word 0x2, 0x45cd40, 0x20, 0x45cd50
  .zero 8
  .word 0x4, 0x45d580, 0x20, 0x45d810, 0x1000, 0x45d060
  .word 0x800, 0x45d120, 0x1, 0x45d1e0, 0x2, 0x45d2a0
  .zero 8

; Command list used by: p1_hp_jax, p2_hp_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_jax_commands:
  .word 0xdd02, 0, 0x3011829  ; command 0

p1_hp_jax_commands_command_1:
  .word 0xaadddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_jax, p2_lp_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_jax_commands:
  .word 0xdd02, 0, 0x22021815  ; command 0

p1_lp_jax_commands_command_1:
  .word 0xaa02, 0, 0x1f041817  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_jax, p2_lk_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_jax_commands:
  .word 0x2dddd, 0, 0x1202c  ; command 0

p1_lk_jax_commands_command_1:
  .word 0xaadddd, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_jax, p2_block_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_jax_commands:
  .word 0x2ddddaa, 0, 0x1b413243  ; command 0

p1_block_jax_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x6110023  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_release_jax, p2_lk_release_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_release_jax_commands:
  .word 0xdd02dddd, 0, 0x2e014640  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_jarek, p2_hp_jarek.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_jarek_commands:
  .word 0xdd02dd, 0, 0x4201d  ; command 0

p1_hp_jarek_commands_command_1:
  .word 0xdddd, 0, 0x3011829  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_jarek, p2_lp_jarek.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_jarek_commands:
  .word 0xaa02, 0, 0x2180e  ; command 0

p1_lp_jarek_commands_command_1:
  .word 0xddddaa, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_jarek, p2_block_jarek.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_jarek_commands:
  .word 0xdddd0101, 0, 0x25413243  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hk_jarek, p2_hk_jarek.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_jarek_commands:
  .word 0xaa02aa, 0, 0x42014  ; command 0

p1_hk_jarek_commands_command_1:
  .word 0xdd02dd, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_jarek, p2_lk_jarek.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_jarek_commands:
  .word 0xddaa, 0, 0x41015  ; command 0

p1_lk_jarek_commands_command_1:
  .word 0xddddaadd, 0, 0x1b012840  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_kitana, p1_hk_noob_saibot, p2_hk_kitana, p2_hk_noob_saibot.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_kitana_commands:
  .word 0xdd02dd, 0, 0x33012842  ; command 0

p1_hk_kitana_commands_command_1:
  .word 0xaaaa02, 0, 0x1a012841  ; command 1

p1_hk_kitana_commands_command_2:
  .word 0xdddd, 0, 0x3011a29  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_lp_kitana, p1_lp_noob_saibot, p2_lp_kitana, p2_lp_noob_saibot.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_kitana_commands:
  .word 0xdd02, 0, 0x21a0a  ; command 0

p1_lp_kitana_commands_command_1:
  .word 0xdd02, 0, 0x12121a0b  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hp_kitana, p2_hp_kitana.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_kitana_commands:
  .word 0xaadd, 0, 0x41805  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_tanya, p2_hp_tanya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_tanya_commands:
  .word 0xdd02, 0, 0x21a0a  ; command 0

p1_hp_tanya_commands_command_1:
  .word 0x2ddaa, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_tanya, p2_lp_tanya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_tanya_commands:
  .word 0xaa02, 0, 0x12121a0b  ; command 0

p1_lp_tanya_commands_command_1:
  .word 0xdddddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_tanya, p2_block_tanya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_tanya_commands:
  .word 0x2010202, 0, 0x2b414640  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hk_tanya, p2_hk_tanya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_tanya_commands:
  .word 0xdddd, 0, 0x3011a29  ; command 0

p1_hk_tanya_commands_command_1:
  .word 0xdd02dd02, 0, 0x1b013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_tanya, p2_lk_tanya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_tanya_commands:
  .word 0xdddd, 0, 0x21041c1e  ; command 0

p1_lk_tanya_commands_command_1:
  .word 0xaa02dd, 0, 0x41c12  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hp_reptile, p2_hp_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_reptile_commands:
  .word 0xdd02, 0, 0x18021a0c  ; command 0

p1_hp_reptile_commands_command_1:
  .word 0x2020201, 0, 0x25013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_reptile, p2_lp_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_reptile_commands:
  .word 0xddaa, 0, 0x41216  ; command 0

p1_lp_reptile_commands_command_1:
  .word 0xdddd02, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_reptile, p2_hk_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_reptile_commands:
  .word 0xffffffff, 0xffffffff, 0x1701001f  ; command 0

p1_hk_reptile_commands_command_1:
  .word 0xdd0202, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_reptile, p2_lk_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_reptile_commands:
  .word 0xaaaa, 0, 0x3011829  ; command 0

p1_lk_reptile_commands_command_1:
  .word 0xddaa, 0, 0x1e041818  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_reptile, p2_block_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_reptile_commands:
  .word 0xffffffff, 0xffffffff, 0x1701001f  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_up_reptile, p2_up_reptile.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_up_reptile_commands:
  .word 0xffffffff, 0xffffffff, 0x2c010040  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_shinnok, p2_hp_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_shinnok_commands:
  .word 0xdddd02, 0, 0x32012414  ; command 0

p1_hp_shinnok_commands_command_1:
  .word 0xdd02dd, 0, 0x32012415  ; command 1

p1_hp_shinnok_commands_command_2:
  .word 0x202, 0, 0x32011a25  ; command 2

p1_hp_shinnok_commands_command_3:
  .word 0xaadd02, 0, 0x33012842  ; command 3
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_shinnok, p2_lp_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_shinnok_commands:
  .word 0xaa02, 0, 0x32011017  ; command 0

p1_lp_shinnok_commands_command_1:
  .word 0xaadd, 0, 0x32011013  ; command 1

p1_lp_shinnok_commands_command_2:
  .word 0xddaa, 0, 0x3011829  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_block_shinnok, p2_block_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_shinnok_commands:
  .word 0xaaaaaa, 0, 0x32412424  ; command 0

p1_block_shinnok_commands_command_1:
  .word 0x2ddaa, 0, 0x3241241b  ; command 1

p1_block_shinnok_commands_command_2:
  .word 0xddaaaa, 0, 0x3241241c  ; command 2

p1_block_shinnok_commands_command_3:
  .word 0x2010102, 0, 0x1b413243  ; command 3
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_shinnok, p2_hk_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_shinnok_commands:
  .word 0xddaaaa, 0, 0x32012416  ; command 0

p1_hk_shinnok_commands_command_1:
  .word 0xaadddd, 0, 0x32012418  ; command 1

p1_hk_shinnok_commands_command_2:
  .word 0xdd02dd, 0, 0x3201241e  ; command 2

p1_hk_shinnok_commands_command_3:
  .word 0xdd0202, 0, 0x1a012841  ; command 3
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_shinnok, p2_lk_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_shinnok_commands:
  .word 0xaaaaaa, 0, 0x32012419  ; command 0

p1_lk_shinnok_commands_command_1:
  .word 0xddaadd, 0, 0x3201241a  ; command 1

p1_lk_shinnok_commands_command_2:
  .word 0xdddddd, 0, 0x3201241d  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_run_shinnok, p2_run_shinnok.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_shinnok_commands:
  .word 0x2ddaa02, 0, 0x1b415040  ; command 0

p1_run_shinnok_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 1

p1_run_shinnok_commands_command_2:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_hp_fujin, p2_hp_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_fujin_commands:
  .word 0xdd02dd, 0, 0x901282d  ; command 0

p1_hp_fujin_commands_command_1:
  .word 0xaaddaa, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_fujin, p2_lp_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_fujin_commands:
  .word 0xaaaa, 0, 0x3011829  ; command 0

p1_lp_fujin_commands_command_1:
  .word 0x2dd, 0, 0x14041808  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_fujin, p2_block_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_fujin_commands:
  .word 0x1dddd02, 0, 0x25413243  ; command 0

p1_block_fujin_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x27410040  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_fujin, p2_hk_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_fujin_commands:
  .word 0xdd02, 0, 0x41209  ; command 0

p1_hk_fujin_commands_command_1:
  .word 0x20202, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_fujin, p2_lk_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_fujin_commands:
  .word 0x201, 0, 0xe142410  ; command 0

p1_lk_fujin_commands_command_1:
  .word 0x2ddaa, 0, 0xa01282b  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_run_fujin, p2_run_fujin.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_fujin_commands:
  .word 0xffffffff, 0xffffffff, 0x27410040  ; command 0

p1_run_fujin_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 1

p1_run_fujin_commands_command_2:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_hp_quan_chi, p2_hp_quan_chi.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_quan_chi_commands:
  .word 0xaadd, 0, 0x11012  ; command 0

p1_hp_quan_chi_commands_command_1:
  .word 0x2dddd, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_quan_chi, p2_lp_quan_chi.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_quan_chi_commands:
  .word 0xdddd, 0, 0x21807  ; command 0

p1_lp_quan_chi_commands_command_1:
  .word 0x2020101, 0, 0x25013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_quan_chi, p2_lk_quan_chi.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_quan_chi_commands:
  .word 0x2dd, 0, 0x41a0b  ; command 0

p1_lk_quan_chi_commands_command_1:
  .word 0xaadddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_quan_chi, p2_hk_quan_chi.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_quan_chi_commands:
  .word 0xdddd, 0, 0x41a1a  ; command 0

p1_hk_quan_chi_commands_command_1:
  .word 0xaa02, 0, 0x3011829  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_release_quan_chi, p2_lk_release_quan_chi.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_release_quan_chi_commands:
  .word 0xdd02dd, 0, 0x2a012040  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_kai, p2_hp_kai.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_kai_commands:
  .word 0xaaaa, 0, 0x13021c08  ; command 0

p1_hp_kai_commands_command_1:
  .word 0xdd02, 0, 0x41a11  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_kai, p2_lp_kai.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_kai_commands:
  .word 0xdddd, 0, 0xf121a09  ; command 0

p1_lp_kai_commands_command_1:
  .word 0xaa02, 0, 0x3011829  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_kai, p2_block_kai.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_kai_commands:
  .word 0x2dddd, 0, 0x1a412841  ; command 0

p1_block_kai_commands_command_1:
  .word 0x2010101, 0, 0x25413243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_kai, p2_hk_kai.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_kai_commands:
  .word 0xaa01dd01, 0, 0x2d413c40  ; command 0

p1_hk_kai_commands_command_1:
  .word 0x2ddaa, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_kai, p2_lk_kai.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_kai_commands:
  .word 0xffffffff, 0xffffffff, 0xd04000d  ; command 0

p1_lk_kai_commands_command_1:
  .word 0xdd02, 0, 0x41a21  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hp_liu_kang, p2_hp_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_liu_kang_commands:
  .word 0xdddd, 0, 0x121801  ; command 0

p1_hp_liu_kang_commands_command_1:
  .word 0x10202dd, 0, 0x1b013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_liu_kang, p2_lp_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_liu_kang_commands:
  .word 0xdddd, 0, 0x21802  ; command 0

p1_lp_liu_kang_commands_command_1:
  .word 0xaadddd, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_liu_kang, p2_block_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_liu_kang_commands:
  .word 0x2dddddd, 0, 0x26413c40  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hk_liu_kang, p2_hk_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_liu_kang_commands:
  .word 0xdddd, 0, 0x41803  ; command 0

p1_hk_liu_kang_commands_command_1:
  .word 0xaadddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_liu_kang, p2_lk_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_liu_kang_commands:
  .word 0xddaa, 0, 0x3011829  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_lk_release_liu_kang, p2_lk_release_liu_kang.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_release_liu_kang_commands:
  .word 0xffffffff, 0xffffffff, 0x11040004  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_scorpion, p2_hp_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_scorpion_commands:
  .word 0xaa02, 0, 0x7141800  ; command 0

p1_hp_scorpion_commands_command_1:
  .word 0x102ddaa, 0, 0x1b013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_scorpion, p2_lp_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_scorpion_commands:
  .word 0xaaaa, 0, 0x15021b06  ; command 0

p1_lp_scorpion_commands_command_1:
  .word 0xdd02, 0, 0x11428  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_scorpion, p2_block_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_scorpion_commands:
  .word 0xaaddddaa, 0, 0x25413240  ; command 0

p1_block_scorpion_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x6110022  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_scorpion, p2_lk_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_scorpion_commands:
  .word 0x202dd, 0, 0x1a012841  ; command 0

p1_lk_scorpion_commands_command_1:
  .word 0xddddaa, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_run_scorpion, p2_run_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_scorpion_commands:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 0

p1_run_scorpion_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_scorpion, p2_hk_scorpion.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_scorpion_commands:
  .word 0xdddd, 0, 0x3011829  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_raiden, p2_hp_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_raiden_commands:
  .word 0xaadd, 0, 0x3011a29  ; command 0

p1_hp_raiden_commands_command_1:
  .word 0x1010102, 0, 0x1b013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_raiden, p2_lp_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_raiden_commands:
  .word 0xaa02, 0, 0x21003  ; command 0

p1_lp_raiden_commands_command_1:
  .word 0x2dddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_raiden, p2_block_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_raiden_commands:
  .word 0xaadd02, 0, 0x1a412841  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hk_raiden, p2_hk_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_raiden_commands:
  .word 0x101aadd, 0, 0x1b413240  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_lk_raiden, p2_lk_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_raiden_commands:
  .word 0xdddd, 0, 0x141a0a  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_run_raiden, p2_run_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_raiden_commands:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 0

p1_run_raiden_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_up_raiden, p2_up_raiden.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_up_raiden_commands:
  .word 0x2, 0, 0x1604200c  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_block_quan_chi, p1_block_noob_saibot, p2_block_quan_chi, p2_block_noob_saibot.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_quan_chi_commands:
  .word 0xffffffff, 0xffffffff, 0x6110020  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hp_sub_zero, p2_hp_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_sub_zero_commands:
  .word 0x2ddaadd, 0, 0x1c413c40  ; command 0

p1_hp_sub_zero_commands_command_1:
  .word 0xaa02aaaa, 0, 0x25013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_sub_zero, p2_lp_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_sub_zero_commands:
  .word 0xffffffff, 0xffffffff, 0x1040002  ; command 0

p1_lp_sub_zero_commands_command_1:
  .word 0xdd02, 0, 0xc021a04  ; command 1

p1_lp_sub_zero_commands_command_2:
  .word 0xaa02, 0, 0x8141201  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_block_sub_zero, p2_block_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_sub_zero_commands:
  .word 0xffffffff, 0xffffffff, 0x1040002  ; command 0

p1_block_sub_zero_commands_command_1:
  .word 0x2ddaadd, 0, 0x1c413c40  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_sub_zero, p2_hk_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_sub_zero_commands:
  .word 0x1010102, 0, 0x1a013041  ; command 0

p1_hk_sub_zero_commands_command_1:
  .word 0xdd02, 0, 0x3011829  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_sub_zero, p2_lk_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_sub_zero_commands:
  .word 0xffffffff, 0xffffffff, 0x1040002  ; command 0

p1_lk_sub_zero_commands_command_1:
  .word 0x20202, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_run_sub_zero, p2_run_sub_zero.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_sub_zero_commands:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 0

p1_run_sub_zero_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 1

p1_run_sub_zero_commands_command_2:
  .word 0x2ddaadd, 0, 0x1c413c40  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_hp_goro, p2_hp_goro.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_goro_commands:
  .word 0xaadd, 0, 0x23000  ; command 0

p1_hp_goro_commands_command_1:
  .word 0xdddd, 0, 0x13031  ; command 1

p1_hp_goro_commands_command_2:
  .word 0x202, 0, 0x13033  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_hk_goro, p2_hk_goro.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_goro_commands:
  .word 0xaadddd, 0, 0x10030  ; command 0

p1_hk_goro_commands_command_1:
  .word 0xaaaa, 0, 0x13032  ; command 1

p1_hk_goro_commands_command_2:
  .word 0x202ddaa, 0, 0x13034  ; command 2
  .word 0  ; end of command list
  .word 0xaa02
  .zero 4
  .word 0x7141800, 0xddaa
  .zero 4
  .word 0x41216
  .zero 8

; Command list used by: p1_run_goro, p1_run_kitana, p1_run_noob_saibot, p2_run_goro, p2_run_kitana, p2_run_noob_saibot.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_goro_commands:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 0

p1_run_goro_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hp_sonya, p2_hp_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_sonya_commands:
  .word 0xaadd, 0, 0x41805  ; command 0

p1_hp_sonya_commands_command_1:
  .word 0xdd02dd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_sonya, p2_lp_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_sonya_commands:
  .word 0xdd02, 0, 0x21005  ; command 0

p1_lp_sonya_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x5040007  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_sonya, p2_hk_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_sonya_commands:
  .word 0x2aaaa, 0, 0x42806  ; command 0

p1_hk_sonya_commands_command_1:
  .word 0xaaaa02, 0, 0x1a012841  ; command 1

p1_hk_sonya_commands_command_2:
  .word 0x1020201, 0, 0x25013243  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_lk_sonya, p2_lk_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_sonya_commands:
  .word 0xdd02aa, 0, 0x19041d13  ; command 0

p1_lk_sonya_commands_command_1:
  .word 0xdddd, 0, 0x3011829  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_sonya, p2_block_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_sonya_commands:
  .word 0xffffffff, 0xffffffff, 0x4110021  ; command 0

p1_block_sonya_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x5040007  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_run_sonya, p2_run_sonya.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_sonya_commands:
  .word 0x1020202, 0, 0x2f413240  ; command 0

p1_run_sonya_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 1

p1_run_sonya_commands_command_2:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_hp_cage, p2_hp_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_cage_commands:
  .word 0xdd02, 0, 0x21a14  ; command 0

p1_hp_cage_commands_command_1:
  .word 0xaa02aa, 0, 0x4241c  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lp_cage, p2_lp_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_cage_commands:
  .word 0xffffffff, 0xffffffff, 0x2001002a  ; command 0

p1_lp_cage_commands_command_1:
  .word 0xaa02, 0, 0x21a13  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_cage, p2_block_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_cage_commands:
  .word 0x2dd0202, 0, 0x1b413243  ; command 0

p1_block_cage_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2001002a  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hk_cage, p2_hk_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_cage_commands:
  .word 0x202aadd, 0, 0x1b413c40  ; command 0

p1_hk_cage_commands_command_1:
  .word 0xdddd02, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_cage, p2_lk_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_cage_commands:
  .word 0xddaa, 0, 0x4181b  ; command 0

p1_lk_cage_commands_command_1:
  .word 0xdd02dd, 0, 0x3011829  ; command 1

p1_lk_cage_commands_command_2:
  .word 0xddddaa, 0, 0x33012842  ; command 2
  .word 0  ; end of command list

; Command list used by: p1_run_cage, p2_run_cage.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_cage_commands:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 0

p1_run_cage_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_hp_reiko, p2_hp_reiko.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hp_reiko_commands:
  .word 0xaa02, 0, 0x3011029  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_up_reiko, p1_up_noob_saibot, p2_up_reiko, p2_up_noob_saibot.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_up_reiko_commands:
  .word 0x2, 0, 0x14200e  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_lp_reiko, p2_lp_reiko.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lp_reiko_commands:
  .word 0xdd02, 0, 0x31021a18  ; command 0

p1_lp_reiko_commands_command_1:
  .word 0xaa0202, 0, 0x1a012841  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_block_reiko, p2_block_reiko.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_block_reiko_commands:
  .word 0xdd02dd, 0, 0x29411c40  ; command 0
  .word 0  ; end of command list

; Command list used by: p1_hk_reiko, p2_hk_reiko.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_hk_reiko_commands:
  .word 0xdd02aa, 0, 0x2304181f  ; command 0

p1_hk_reiko_commands_command_1:
  .word 0x202aaaa, 0, 0x25013243  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_lk_reiko, p2_lk_reiko.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_lk_reiko_commands:
  .word 0xddaa, 0, 0x24041820  ; command 0

p1_lk_reiko_commands_command_1:
  .word 0x2dddd, 0, 0x33012842  ; command 1
  .word 0  ; end of command list
  .zero 4

; Command list used by: p1_run_liu_kang, p1_run_reiko, p1_run_quan_chi, p1_run_tanya, p1_run_reptile, p1_run_kai, p1_run_jarek, p1_run_jax, p2_run_liu_kang, p2_run_reiko, p2_run_quan_chi, p2_run_tanya, p2_run_reptile, p2_run_kai, p2_run_jarek, p2_run_jax.
; Rows: input pattern, extra input, packed predicate/flags/window/action. Zero pattern terminates.
; Reader: game/data/tables.c:mk4_command_next.
p1_run_liu_kang_commands:
  .word 0xffffffff, 0xffffffff, 0x1d01000f  ; command 0

p1_run_liu_kang_commands_command_1:
  .word 0xffffffff, 0xffffffff, 0x2010003  ; command 1
  .word 0  ; end of command list
  .zero 4
