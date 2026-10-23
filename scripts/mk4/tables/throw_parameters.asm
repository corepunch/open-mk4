; Throw dispatch and native continuation parameter records.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


lk_throw_table:
  .word MK4_NATIVE_SCORPION_LK_THROW_ENTRY, MK4_NATIVE_RAIDEN_LK_THROW_ENTRY, MK4_NATIVE_SONYA_LK_THROW_ENTRY, MK4_NATIVE_LIU_KANG_LK_THROW_ENTRY, MK4_NATIVE_SUB_ZERO_LK_THROW_ENTRY, MK4_NATIVE_FUJIN_LK_THROW_ENTRY
  .word MK4_NATIVE_SHINNOK_LK_THROW_ENTRY, MK4_NATIVE_REIKO_LK_THROW_ENTRY, MK4_NATIVE_QUAN_CHI_LK_THROW_ENTRY, MK4_NATIVE_TANYA_LK_THROW_ENTRY, MK4_NATIVE_REPTILE_LK_THROW_ENTRY, MK4_NATIVE_KAI_LK_THROW_ENTRY
  .word MK4_NATIVE_JAREK_LK_THROW_ENTRY, MK4_NATIVE_JAREK_LK_THROW_ENTRY, MK4_NATIVE_CAGE_LK_THROW_ENTRY, MK4_NATIVE_GORO_LK_THROW_ENTRY
  .zero 536

animation_descriptor_lk_victim_return:
  .word clip(39), MK4_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN
  .zero 320

mmove_lk_owner_return:
  .word 0x3333  ; numeric; printable bytes do not establish text
  .zero 556

slam_continue_chords:
  .word 0x60  ; numeric; printable bytes do not establish text
  .word 0x4, 0x71, 0x7, 0x6000, 0x40, 0x7100
  .word 0x70, 0x10, 0x3, 0x71, 0x7, 0x1000
  .word 0x30, 0x7100, 0x70, 0x30, 0x2, 0x71
  .word 0x7, 0x3000, 0x20, 0x7100, 0x70, 0x50
  .word 0x3, 0x71, 0x7, 0x5000, 0x30, 0x7100
  .word 0x70
  .zero 92

throw_turn:
  .word MK4_NATIVE_FLIP_FACING
  .zero 72
  .word MK4_NATIVE_FACE_PEER_ORIENT_GROUP
  .zero 404
  .word 0x42cb40, 0x42cb50, 0x42cb60, 0x42cb70
  .zero 88
  .word 0x450d0bc, 0x450d0d8, 0x450d0f4, 0x450d110, 0x450d12c, 0x450d148
  .word 0x550d164, 0x550d180, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c, 0x7f50d19c
  .word 0x8050d19c, 0x405dd0, 0x3333, 0x1999, 0xffffe667, 0xffffcccd
  .word 0x3333, 0x1999, 0xffffe667, 0xffffcccd, 0x1999, 0xccc
  .word 0xfffff334, 0xffffe667, 0x1999, 0xccc, 0xfffff334, 0xffffe667
  .zero 4
  .word 0xfffe8000, 0xfff8f334
  .zero 8
  .word 0xfff50000, 0xffff0000
  .zero 4
  .word 0x4cccc
  .zero 16
  .word 0xfffd0000, 0xfff40000
  .zero 8
  .word 0xffe20000, 0xffff0000
  .zero 4
  .word 0xfff60000, 0xfffd8000, 0xfff90000
  .zero 4
  .word 0x63333
  .zero 12
  .word 0x1b, 0xfffffd71, 0x15
  .zero 8
  .word 0xfff60000, 0xfff60000
  .zero 4
  .word 0x1c, 0xfffffdb3, 0x16, 0xfffe8000
  .zero 4
  .word 0xfffe8000, 0x70000
  .zero 8
  .word 0xfffe8000, 0xffca3334
  .zero 4
  .word 0x12, 0x4313a0, 0x12, 0x431700, 0x12, 0x431c60
  .zero 4
  .word 0xfffd3334, 0xfff6a667
  .zero 4
  .word 0x62666, 0x6428f
  .zero 8
  .word 0x1999, 0xfff735c3, 0xfffeb334
  .zero 4
  .word 0x4e147, 0x6428f
  .zero 8
  .word 0x12, 0x431670, 0x430bc0, 0x430990, 0x430bc0, 0x430570
  .word 0x430740, 0x430570, 0x430bc0, 0x430570, 0x430570, 0x430bc0
  .word 0x430bc0
  .zero 4
  .word 0x50000
  .zero 4
  .word 0xfffe8000, 0xfff76000
  .zero 4
  .word 0xfffdb334, 0xfff9c000
  .zero 4
  .word 0xfffb2148, 0xfffe0000
  .zero 4
  .word 0xfff90000, 0xfffe8000
  .zero 4
  .word 0xfff90000, 0xfffe8000
  .zero 4
  .word 0xfff90000, 0xfffe8000
  .zero 4
  .word 0xfff90000, 0xfffe8000
  .zero 4
  .word 0xfff90000, 0xfffe8000
  .zero 4
  .word 0x8c
  .zero 20
  .word 0x13, 0x98000, 0x147, 0x51e, 0xb85, 0x147a
  .word 0x2000, 0x2e14, 0x3eb8, 0x51eb, 0x67ae, 0x8000
  .word 0x9851, 0xae14, 0xc147, 0xd1eb, 0xe000, 0xeb85
  .word 0xf47a, 0xfae1, 0xfeb8
  .zero 4
  .word 0x13, 0x98000, 0xfeb8, 0xfae1, 0xf47a, 0xeb85
  .word 0xe000, 0xd1eb, 0xc147, 0xae14, 0x9851, 0x8000
  .word 0x67ae, 0x51eb, 0x3eb8, 0x2e14, 0x2000, 0x147a
  .word 0xb85, 0x51e, 0x147
  .zero 4
  .word 0x9, 0x48000, 0x51e, 0x147a, 0x2e14, 0x51eb
  .word 0x8000, 0xae14, 0xd1eb, 0xeb85, 0xfae1
  .zero 4
  .word 0x9, 0x48000, 0xfae1, 0xeb85, 0xd1eb, 0xae14
  .word 0x8000, 0x51eb, 0x2e14, 0x147a, 0x51e
  .zero 4
  .word 0x4, 0x20000, 0x147a, 0x47ae, 0xb851, 0xeb85
  .word 0x4, 0x20000, 0xeb85, 0xb851, 0x47ae, 0x147a
  .word 0x1, 0x10000, 0x10000
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_001:
  .word 0x305a1, 0x6c7ae, 0x5f9db, 0x637ce, 0xfffd3334, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_002:
  .word 0x2e6e9, 0x832f1, 0x5f062, 0x62937, 0xfffbe667, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_003:
  .word 0x30fdf, 0x70000, 0x62189
  .zero 4
  .word 0xfffdb334, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_004:
  .word 0x2e6e9, 0x50f5c, 0x20c4, 0x63b64, 0xffff3375, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_005:
  .word 0x2deb8, 0x70000, 0x62b85, 0x6170a, 0xfffe199a, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_006:
  .word 0x191eb, 0x44000
  .zero 4
  .word 0xfffe6e15, 0xfffe0000, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_007:
  .word 0x50fdf, 0xffff0000, 0x4dced, 0x149ba, 0xfffb3334, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_008:
  .word 0xfffe3ae2, 0x3bd70, 0x5eb85, 0x13d70, 0xfffd599a, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_009:
  .word 0x49f3b, 0x19db, 0x12937, 0x195c2, 0x10000, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_010:
  .word 0x3c831, 0x70000, 0x5c76c, 0xb26e, 0xfffa8000, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_011:
  .word 0xfffcab86, 0x723d7, 0x51e, 0x630a3, 0xfffeb5c3, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_012:
  .word 0x338d4, 0x70000
  .zero 4
  .word 0x1999, 0xfffe8000, 0xc00

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_013:
  .word 0x4245a, 0x4947a, 0x44dd, 0xfdb2, 0xffffcd0f, 0xc00

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_014:
  .word 0x3d26e, 0x70000, 0x608f5, 0x8937, 0xfffd66a8, 0xc00

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_015:
  .word 0x457a0, 0x3949b, 0x63dc5, 0x15f7c, 0xfffe999b, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_016:
  .word 0x19f3b, 0x3b851, 0x63db2, 0x4c28f, 0xfffe999b, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_017:
  .word 0x19f3b, 0xffff8fe0, 0x4d74b, 0x4c28f, 0xfffb6667, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_018:
  .word 0x3e707, 0x3d20e, 0x1d0, 0xf894, 0xfffee668, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_019:
  .word 0x305a1, 0x9b810, 0x61c6a, 0x61c28, 0xfffcb334, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_020:
  .word 0x34d4f, 0x628f5, 0x61ba5, 0x2831, 0xfffdb5ca, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_021:
  .word 0x26c08, 0x99439, 0x5ea3d, 0x591a9, 0xfffbcf5d, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_022:
  .word 0x3be12, 0x40f7c
  .zero 4
  .word 0xfdaf, 0xffff0002, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_023:
  .word 0xffdbf2ff, 0xfffe94de, 0x4bdf3, 0x4ed0e, 0xfffdcccd, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_024:
  .word 0x194fd, 0x5eb85, 0x3f7c, 0x4b581, 0xffff8007, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_025:
  .word 0x20fdf, 0x347ae, 0x5a24d, 0x5228f, 0xfffc4cc7, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_026:
  .word 0x3245a, 0x11b2b0, 0x6147a
  .zero 4
  .word 0xfffb0000, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_027:
  .word 0x3245a, 0xccc49, 0x6147a
  .zero 4
  .word 0xfffc3334, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_028:
  .word 0x3245a, 0x29999, 0x62b85
  .zero 4
  .word 0xfffe4ccd, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_029:
  .word 0x3245a, 0xc6106, 0x5ff3b
  .zero 4
  .word 0xfffa3334, 0x700

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_030:
  .word 0x2d26e, 0x70000, 0x6347a, 0x5fb22, 0xfffe4d0f, 0x700

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_031:
  .word 0x18083, 0x1f0a3
  .zero 4
  .word 0x48f5c, 0xfffe8000, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_032:
  .word 0x1b3b6, 0x735c2, 0x32f1, 0x5170a, 0xffff0042, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_033:
  .word 0xab374, 0x1ae14, 0x4f8d4, 0x131eb, 0xfff90042, 0x700

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_034:
  .word 0x31a1c, 0x62e14
  .zero 8
  .word 0xfffe8000, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_035:
  .word 0x3245a, 0x97a9f
  .zero 8
  .word 0xfffe3375, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_036:
  .word 0x3245a, 0x140000, 0x62666
  .zero 4
  .word 0xfffa4d0f, 0x500

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_037:
  .word 0x3f144, 0x70000, 0x60532, 0xd079, 0xfffc332b, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_038:
  .word 0x3f126, 0x4051e, 0x63ced, 0xea7e, 0xfffee6a8, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_039:
  .word 0x2bdf3, 0xa9e76, 0x5d020, 0x623d7, 0xfff9e6a8, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_040:
  .word 0x338d4, 0x428f5, 0xa51e, 0x2d0, 0xfffde667, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_041:
  .word 0x2fb64, 0x3d70a, 0xa560
  .zero 4
  .word 0xcccc, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_042:
  .word 0x32e97, 0x6b851
  .zero 4
  .word 0x472b, 0xfffe8000, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_043:
  .word 0x31a1c, 0x89958
  .zero 4
  .word 0x62189, 0xfffe8000, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_044:
  .word 0x34d4f, 0x86b43, 0x24d, 0x64666, 0xfffee6a8, 0x18

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_045:
  .word 0x28ac0, 0x3d1eb, 0x9fb, 0x5a831, 0xffff333a, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_046:
  .word 0x2c7ae, 0x547ae, 0x2395, 0x5efdf, 0xffff4d0f, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_047:
  .word 0x3245a, 0x70000, 0x20ff
  .zero 4
  .word 0xffff3334, 0x18

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_048:
  .word 0x305a1, 0x4f5c2, 0x61ef9, 0x64312, 0xfffe3334, 0x30

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_049:
  .word 0x38083, 0x40f5c, 0x3ef9, 0x5581, 0xffffe6a8, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_050:
  .word 0x2c831, 0x375c2, 0x5e395, 0x5e20c, 0xfffd999a, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_051:
  .word 0x3b3b6, 0x3f0a3, 0x2395, 0xa872, 0xffff8007, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_052:
  .word 0x3245a, 0x4e666
  .zero 8
  .word 0xfffee66d, 0x300

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_053:
  .word 0x3245a, 0x375c2, 0x60ed9
  .zero 4
  .word 0xfffe0000, 0x4b0

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_054:
  .word 0x3245a, 0x4e666
  .zero 8
  .word 0xfffee66d, 0x4b0

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_055:
  .word 0x3245a, 0x55c28, 0x53b6, 0x816, 0xffff8007, 0x4b0

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_056:
  .word 0x3578d, 0x48a3d
  .zero 4
  .word 0x872b, 0xfffe8000, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_057:
  .word 0x3578d, 0x63851
  .zero 4
  .word 0x7db2, 0xfffedebf, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_058:
  .word 0x32e97, 0x70000, 0x1e76
  .zero 4
  .word 0xffff19dc, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_059:
  .word 0x37645, 0x70000
  .zero 4
  .word 0x2b43, 0xfffeb375, 0x10

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_060:
  .word 0x2e6e9, 0x48000, 0x5947a
  .zero 4
  .word 0xfffb199a, 0x65

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_061:
  .word 0x3dcac, 0x30000
  .zero 4
  .word 0x108f5, 0xfffe6667, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_062:
  .word 0x1bdf3, 0x8b0a3, 0x6420c, 0x509ba, 0xfffecccd, 0x30

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_063:
  .word 0x1d26e, 0x670a3, 0x6428f, 0x5051e, 0xfffecd0f, 0x50

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_064:
  .word 0x38a3d, 0x51999, 0x5fd70, 0x7851, 0xfffd199a, 0x30

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_065:
  .word 0x43851, 0x51999, 0x6028f, 0x123d7, 0xfffd4ccd, 0x18

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_066:
  .word 0x3b333, 0x47ae1, 0x5f333, 0x8e56, 0xfffd199a, 0x400

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_067:
  .word 0x29eb8, 0x5f0a3, 0x628f5, 0x5f5c2, 0xfffde667, 0x40

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_068:
  .word 0x38000, 0x575c2, 0x2353, 0x65e3, 0xffff99dc, 0x68

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_069:
  .word 0x23851, 0x68a3d, 0x16c8, 0x57333, 0xffff8042, 0x18

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_070:
  .word 0x2b333, 0x923d7
  .zero 4
  .word 0x60f5c, 0xfffe8000, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_071:
  .word 0x3051e, 0x4e666
  .zero 8
  .word 0xfffee8f6, 0x46

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_072:
  .word 0x46147, 0x3b851, 0x60a3d, 0x16147, 0xfffdb334, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_073:
  .word 0x2051e, 0x3428f, 0x2b85, 0x53333, 0xffff0290, 0x200

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_074:
  .word 0x31999, 0x91e76, 0x6245a, 0x64353, 0xfffd3334, 0x30

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_075:
  .word 0x30ccc, 0x5c7ae, 0x34bc, 0x978, 0xffffcd0f, 0x1e

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_076:
  .word 0x2f126, 0x704dd, 0x5e0c4
  .zero 4
  .word 0xfffb8000, 0x26

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_077:
  .word 0x387ae, 0x34ccc
  .zero 4
  .word 0x87ae, 0xfffecccd, 0x1e

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_078:
  .word 0x3051e, 0x4f5c2
  .zero 8
  .word 0xfffee667, 0x20

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_079:
  .word 0x387ae, 0x48000
  .zero 4
  .word 0x87ae, 0xfffecccd, 0x10e

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_080:
  .word 0x3c000, 0x58000, 0xffffc000, 0xcccc, 0xfffd8000, 0x10e

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_081:
  .word 0xfffb0000, 0x2d0000, 0x80000, 0x1e

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_082:
  .word 0x2dcac, 0x4e147, 0x2353, 0x5e666, 0xffff0042, 0x600

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_083:
  .word 0x2dcac, 0x428f5, 0x5e8f5, 0x5e8b4, 0xfffc0000, 0x60

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_084:
  .word 0x31a1c, 0x44ccc, 0x5eb02, 0x272b, 0xfffcb334, 0x60

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_085:
  .word 0x23e, 0x23f, 0x240, 0x241, 0x90000, 0xc
  .word 0x4, 0x8000, 0xfff18000
  .zero 4

; Retained referenced block; semantic role is not established.
unresolved_throw_parameters_086:
  .word 0x242, 0x243, 0x244, 0x245, 0xfff70000, 0xc
  .word 0x4, 0xffff8000, 0xe8000, 0x1, 0x147, 0xfe700000
  .word 0x7, 0x438e70, 0x438e80, 0x437d10, 0x437c60, 0x437dc0
  .word 0x437ea0, 0x436d20, 0x8, 0x436d20, 0x438e60, 0x438e70
  .word 0x438e80, 0x437d10, 0x437c60, 0x437dc0, 0x437ea0
  .zero 4
  .word 0x5, 0x436d20, 0x438e50, 0x438000, 0x437820, 0x437770
  .word 0x5, 0x437690, 0x437b00, 0x437bb0, 0x437910, 0x437ea0
  .word 0x4, 0x437b00, 0x437910, 0x437ea0, 0x437bb0
  .zero 4
  .word 0x3, 0x436c70, 0x436c70, 0x438bf0, 0x8, 0x437b00
  .word 0x437910, 0x437ea0, 0x437bb0, 0x436d20, 0x436b10, 0x436bc0
  .word 0x436b10
  .zero 4
  .word 0x9, 0x436b10, 0x436bc0, 0x436b10, 0x438e70, 0x438e80
  .word 0x437d10, 0x437c60, 0x437ea0, 0x436d20, 0x40000, 0x4000c
  .word 0x40007, 0x20001, 0x40002, 0x1002b, 0x200002, 0x200002
  .word 0x4000b, 0x4001e, 0x40018, 0x20009, 0x40014, 0x1002c
  .word 0x4001b
  .zero 4
  .word 0x4395e0, 0x20, 0x434000
  .zero 4
  .word 0x48efc0, 0x20, 0x4352b0
  .zero 4
  .word 0x435330, 0x435370, 0x435360, 0x435380, 0x435390, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x435420, 0x435be0, 0x435a60, 0x435e30
  .word 0x435420, 0x435420, 0x435420, 0x435980, 0x435990, 0x435420
  .word 0x435420, 0x4357a0, 0x435420, 0x435420, 0x4355d0, 0x435420
  .word 0x435420, 0x435420, 0x435420, 0x435420, 0x435420, 0x435420
  .word 0x4355c0, 0x435420, 0x435420, 0x435420, 0x436580, 0x436210
  .word 0x435ec0, 0x435ec0, 0x435ec0, 0x436210, 0x436250, 0x436240
  .word 0x435ec0, 0x435ec0, 0x436320, 0x436330, 0x435f80, 0x435ec0
  .word 0x4360e0, 0x4362e0, 0x435ec0, 0x436180, 0x4361b0, 0x435f90
  .word 0x4362a0, 0x435f70, 0x435ec0, 0x435ec0, 0x435ec0
  .zero 4
  .word 0x2, 0x438ef0, 0x438f10
  .zero 4
  .word 0x4365f0, 0x1e, 0x4335e0
  .zero 4
  .word 0x4, 0x437220, 0x437220, 0x437350, 0x437a30
  .zero 4
  .word 0x2, 0x437350, 0x4370a0
  .zero 4
  .word 0x5, 0x437380, 0x4378d0, 0x437430, 0x4370a0, 0x4375a0
  .word 0x5, 0x4374d0, 0x4370a0, 0x4378d0, 0x4374f0, 0x4375a0
  .word 0x4367b0, 0x433ba0, 0x433ba0, 0x436780, 0x4367c0, 0x436840
  .word 0x436810, 0x436750, 0x436760, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x439c00, 0xf0, 0x4335e0
  .zero 4
  .word 0x439530, 0x32, 0x4335e0
  .zero 4
  .word 0x4395e0, 0x20, 0x438e80
  .zero 4
  .word 0x4395e0, 0x20, 0x4373c0
  .zero 4
  .word 0x437400, 0x20, 0x438e80
  .zero 4
  .word 0x200001, 0x4000c, 0x40005, 0x200001, 0x40001, 0x200001
  .word 0x200001, 0x4000e, 0x4000b, 0x200001, 0x40016, 0x200001
  .word 0x200001, 0x200001, 0x200001, 0x200001, 0x40000, 0x4000c
  .word 0x40005, 0x200000, 0x40002, 0x200000, 0x200000, 0x4000e
  .word 0x4000b, 0x200000, 0x200000, 0x200000, 0x200000, 0x200000
  .word 0x200000, 0x200000, 0x20006, 0x4000a, 0x40007, 0x40004
  .word 0x40002, 0x40009, 0x200002, 0x4001f, 0x4001a, 0x40012
  .word 0x40018, 0x4000d, 0x40014, 0x40017, 0x1002a, 0x200004
  .word 0x10028, 0x4000a, 0x40007, 0x40004, 0x40002, 0x40009
  .zero 4
  .word 0x4000e, 0x4001a, 0x40012, 0x40018, 0x20008, 0x40015
  .word 0x40017
  .zero 8
