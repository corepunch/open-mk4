; CPU action choices and finisher dispatch/distances, with retained neighboring scene data.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.


; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
cpu_action_tables:
  .word 0x10028  ; row 0

cpu_action_tables_row_01:
  .word 0x4000a  ; row 1

cpu_action_tables_row_02:
  .word 0x40007  ; row 2

cpu_action_tables_row_03:
  .word 0x40004  ; row 3

cpu_action_tables_row_04:
  .word 0x40001  ; row 4

cpu_action_tables_row_05:
  .word 0x40009  ; row 5

cpu_action_tables_row_06:
  .word 0  ; row 6

cpu_action_tables_row_07:
  .word 0x4000e  ; row 7

cpu_action_tables_row_08:
  .word 0x4001a  ; row 8

cpu_action_tables_row_09:
  .word 0x40012  ; row 9

cpu_action_tables_row_10:
  .word 0x40018  ; row 10

cpu_action_tables_row_11:
  .word 0x20008  ; row 11

cpu_action_tables_row_12:
  .word 0x40015  ; row 12

cpu_action_tables_row_13:
  .word 0x40017  ; row 13

cpu_action_tables_row_14:
  .word 0x4001b  ; row 14

cpu_action_tables_row_15:
  .word 0  ; row 15

cpu_action_tables_row_16:
  .word 0x40000  ; row 16

cpu_action_tables_row_17:
  .word 0x4000c  ; row 17

cpu_action_tables_row_18:
  .word 0x40005  ; row 18

cpu_action_tables_row_19:
  .word 0x20002  ; row 19

cpu_action_tables_row_20:
  .word 0x40001  ; row 20

cpu_action_tables_row_21:
  .word 0x1002d  ; row 21

cpu_action_tables_row_22:
  .word 0  ; row 22

cpu_action_tables_row_23:
  .word 0x4000e  ; row 23

cpu_action_tables_row_24:
  .word 0x4000b  ; row 24

cpu_action_tables_row_25:
  .word 0x40012  ; row 25

cpu_action_tables_row_26:
  .word 0x40018  ; row 26

cpu_action_tables_row_27:
  .word 0x20009  ; row 27

cpu_action_tables_row_28:
  .word 0x40015  ; row 28

cpu_action_tables_row_29:
  .word 0x1002c  ; row 29

cpu_action_tables_row_30:
  .word 0x4001b  ; row 30

cpu_action_tables_row_31:
  .word 0  ; row 31

cpu_action_tables_row_32:
  .word 0x40000  ; row 32

cpu_action_tables_row_33:
  .word 0x4000a  ; row 33

cpu_action_tables_row_34:
  .word 0x40013  ; row 34

cpu_action_tables_row_35:
  .word 0x40003  ; row 35

cpu_action_tables_row_36:
  .word 0x40002  ; row 36

cpu_action_tables_row_37:
  .word 0x40009  ; row 37

cpu_action_tables_row_38:
  .word 0  ; row 38

cpu_action_tables_row_39:
  .word 0x4001f  ; row 39

cpu_action_tables_row_40:
  .word 0x4000b  ; row 40

cpu_action_tables_row_41:
  .word 0x40012  ; row 41

cpu_action_tables_row_42:
  .word 0x40016  ; row 42

cpu_action_tables_row_43:
  .word 0  ; row 43

cpu_action_tables_row_44:
  .word 0x40015  ; row 44

cpu_action_tables_row_45:
  .word 0x40017  ; row 45

cpu_action_tables_row_46:
  .word 0x4001b  ; row 46

cpu_action_tables_row_47:
  .word 0x200005  ; row 47

cpu_action_tables_row_48:
  .word 0x20006  ; row 48

cpu_action_tables_row_49:
  .word 0x20003  ; row 49

cpu_action_tables_row_50:
  .word 0x20005  ; row 50

cpu_action_tables_row_51:
  .word 0x20002  ; row 51

cpu_action_tables_row_52:
  .word 0x20004  ; row 52

cpu_action_tables_row_53:
  .word 0  ; row 53

cpu_action_tables_row_54:
  .word 0  ; row 54

cpu_action_tables_row_55:
  .word 0x20018  ; row 55

cpu_action_tables_row_56:
  .word 0x20007  ; row 56

cpu_action_tables_row_57:
  .word 0x2000a  ; row 57

cpu_action_tables_row_58:
  .word 0x2000c  ; row 58

cpu_action_tables_row_59:
  .word 0x20008  ; row 59

cpu_action_tables_row_60:
  .word 0x2000e  ; row 60

cpu_action_tables_row_61:
  .word 0x20015  ; row 61

cpu_action_tables_row_62:
  .word 0x20013  ; row 62

cpu_action_tables_row_63:
  .word 0x2001a  ; row 63
  .word 0x439f40, 0x40, 0x439dd0
  .zero 4
  .word 0x6, 0x436d20, 0x43a3d0, 0x43a2c0, 0x43a200, 0x43a140
  .word 0x43a0b0, 0x43aa50, 0x43b0c0, 0x43bc20, 0x43bd00, 0x43bc40
  .word 0x43bd50, 0x43bd20, 0x43a930, MK4_NATIVE_GORO_SPECIAL_UPPERCUT, 0x4, MK4_NATIVE_GORO_LEAP_STOMP
  .word 0x43b2a0, 0x43a940, 0x437f50
  .zero 4
  .word 0x6, 0x43b2a0, MK4_NATIVE_GORO_LEAP_STOMP, 0x43abc0, 0x43b310, 0x43ae50
  .word MK4_NATIVE_GORO_QUAKE_STOMP
  .zero 4
  .word 0x5, 0x43a9f0, MK4_NATIVE_GORO_SPECIAL_PUNCH, 0x43ac70, 0x439090, 0x43ad90
  .word 0x6, 0x43ae70, MK4_NATIVE_GORO_SPECIAL_KICK, 0x43abc0, 0x43ac70, 0x43b310
  .word 0x43ad90
  .zero 4
  .word 0x9, 0x43ae70, 0x43abc0, 0x43abc0, 0x43b2a0, MK4_NATIVE_GORO_LEAP_STOMP
  .word 0x43b310, 0x43ae50, 0x43ad90, MK4_NATIVE_GORO_QUAKE_STOMP, 0x74000000, 0x4700022d
  .word clip(596), 0x5000039, 0x27000016, 0x5000045, 0x27000016, 0x5000051
  .word 0x27000016, 0x500005d, 0x27000016, 0x500005e, 0x1200002d, 0x5000039
  .word 0x27000016, 0x5000045, 0x27000016, 0x5000051, 0x27000016, 0x500005d
  .word 0x27000016, 0x500005e, 0x3000000, 0x12000016, 0x7000000, 0x32000000
  .word 0x43aa50
  .zero 4
  .word 0x5f000000, 0x1999, 0x4700020e, clip(597), 0x5b000000, 0x32000000
  .word 0x43ae90
  .zero 36
  .word 0x4700020a, clip(591), 0x58000000, 0x2b85, 0xffffeb86, 0x189
  .word 0x5000011, 0x60000000, 0x35000000, MK4_NATIVE_DISTURB_RESTING_WEAPONS, 0x28000000, 0x3000000
  .word 0x32000000, 0x43b2c0
  .zero 184
  .word 0x3, 0x43b310, MK4_NATIVE_GORO_LEAP_STOMP, 0x433ba0, 0x3, 0x43b310
  .word 0x436ad0, 0x438c70

action_table_32:
  .word 0x437a30  ; numeric; printable bytes do not establish text
  .word 0x4370a0, 0x438f10, 0x437b00, MK4_NATIVE_GORO_SPECIAL_KICK, MK4_NATIVE_GORO_LEAP_STOMP, 0x437a30
  .word 0x437a30, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x4366a0, 0x436620
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433da0, 0x433ba0, 0x433ba0
  .zero 8
  .word 0x435e70, 0x434af0, 0x435400, 0x436730, 0x435300, 0x435310
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .zero 4
  .word 0x433ba0, 0x4341e0, 0x4341f0, 0x437910, 0x433ba0, 0x433ba0
  .word 0x433eb0, 0x4342d0, 0x434400, 0x4344a0, 0x433ba0, 0x433ba0
  .word 0x433ba0, 0x433ba0, 0x434290, 0x4341d0, 0x4340e0, 0x433f40
  .word 0x4340b0, 0x433ba0, 0x433ba0, 0x434480, 0x433f00, 0x433f30
  .word 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0, 0x433ba0
  .zero 4
  .word 0xcccc, 0x439050, 0x10000, 0x438f10, 0x10000, 0x438f00
  .word 0x10000, 0x438ef0, 0xcccc, 0x438ec0, 0x43a540, 0x43a520
  .word 0x43a560, 0x43a580, 0x43a5a0
  .zero 4

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
first_finish_distance_table:
  .word 0x20000  ; row 0

first_finish_distance_table_row_01:
  .word 0xcccc  ; row 1

first_finish_distance_table_row_02:
  .word 0x30000  ; row 2

first_finish_distance_table_row_03:
  .word 0x20000  ; row 3

first_finish_distance_table_row_04:
  .word 0xcccc  ; row 4

first_finish_distance_table_row_05:
  .word 0x20000  ; row 5

first_finish_distance_table_row_06:
  .word 0x10000  ; row 6

first_finish_distance_table_row_07:
  .word 0xcccc  ; row 7

first_finish_distance_table_row_08:
  .word 0xcccc  ; row 8

first_finish_distance_table_row_09:
  .word 0xe666  ; row 9

first_finish_distance_table_row_10:
  .word 0xcccc  ; row 10

first_finish_distance_table_row_11:
  .word 0xcccc  ; row 11

first_finish_distance_table_row_12:
  .word 0xcccc  ; row 12

first_finish_distance_table_row_13:
  .word 0xcccc  ; row 13

first_finish_distance_table_row_14:
  .word 0xcccc  ; row 14

first_finish_distance_table_row_15:
  .word 0  ; row 15

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
second_finish_distance_table:
  .word 0x10000  ; row 0

second_finish_distance_table_row_01:
  .word 0xcccc  ; row 1

second_finish_distance_table_row_02:
  .word 0x18000  ; row 2

second_finish_distance_table_row_03:
  .word 0x10000  ; row 3

second_finish_distance_table_row_04:
  .word 0x28000  ; row 4

second_finish_distance_table_row_05:
  .word 0x20000  ; row 5

second_finish_distance_table_row_06:
  .word 0x1cccc  ; row 6

second_finish_distance_table_row_07:
  .word 0x26666  ; row 7

second_finish_distance_table_row_08:
  .word 0x18000  ; row 8

second_finish_distance_table_row_09:
  .word 0xe666  ; row 9

second_finish_distance_table_row_10:
  .word 0x23333  ; row 10

second_finish_distance_table_row_11:
  .word 0x1cccc  ; row 11

second_finish_distance_table_row_12:
  .word 0x2cccc  ; row 12

second_finish_distance_table_row_13:
  .word 0x10000  ; row 13

second_finish_distance_table_row_14:
  .word 0xcccc  ; row 14

second_finish_distance_table_row_15:
  .word 0  ; row 15

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
second_finish_table:
  .word MK4_NATIVE_SCORPION_BEAST_FATALITY  ; row 0

second_finish_table_row_01:
  .word MK4_NATIVE_RAIDEN_SECOND_FATALITY  ; row 1

second_finish_table_row_02:
  .word MK4_NATIVE_SONYA_SECOND_FATALITY  ; row 2

second_finish_table_row_03:
  .word MK4_NATIVE_LIU_KANG_SECOND_FATALITY  ; row 3

second_finish_table_row_04:
  .word MK4_NATIVE_SUB_ZERO_SECOND_FATALITY  ; row 4

second_finish_table_row_05:
  .word MK4_NATIVE_FUJIN_SECOND_FATALITY  ; row 5

second_finish_table_row_06:
  .word MK4_NATIVE_SHINNOK_FATALITY_2  ; row 6

second_finish_table_row_07:
  .word MK4_NATIVE_REIKO_SECOND_FATALITY  ; row 7

second_finish_table_row_08:
  .word MK4_NATIVE_QUAN_CHI_SECOND_FATALITY  ; row 8

second_finish_table_row_09:
  .word MK4_NATIVE_TANYA_FATALITY_2  ; row 9

second_finish_table_row_10:
  .word MK4_NATIVE_REPTILE_FATALITY_2  ; row 10

second_finish_table_row_11:
  .word MK4_NATIVE_KAI_FATALITY_2  ; row 11

second_finish_table_row_12:
  .word MK4_NATIVE_JAREK_FATALITY_2  ; row 12

second_finish_table_row_13:
  .word MK4_NATIVE_JAX_FATALITY_2  ; row 13

second_finish_table_row_14:
  .word MK4_NATIVE_CAGE_FATALITY_2  ; row 14

second_finish_table_row_15:
  .word 0  ; row 15
  .zero 280
  .word word(unresolved_body_assets_009), 0x3333, 0xffffcccd, word(unresolved_scene_assets_471)
  .zero 32
  .word MK4_NATIVE_KAI_SECOND_CHILD_WALK
  .zero 92
  .word word(unresolved_scene_assets_263), 0x1999, 0xffff8ccd, word(unresolved_scene_assets_194)
  .zero 240

; Retained referenced block; semantic role is not established.
unresolved_cpu_finishers_001:
  .word 0x500004d, 0x12000045, 0x33000000, word(unresolved_cpu_finishers_001)
  .zero 40
  .word 0x48000200, clip(413), 0x32000000, 0x43dc90
  .zero 32
  .word 0x11000200, clip(13), 0x32000000, 0x43fb30, 0x4700025a, clip(206)
  .word 0x32000000, 0x440160, 0x3000000, 0x32000000, 0x440190
  .zero 36
  .word 0x3000000, 0x4a000014, 0x3000000, 0x4a000014, 0x3000000, 0x4a000014
  .word 0x48000200, clip(401)

; Retained referenced block; semantic role is not established.
unresolved_cpu_finishers_002:
  .word 0x48000200, clip(443), 0x33000000, word(unresolved_cpu_finishers_002), 0x48000200, clip(393)

; Retained referenced block; semantic role is not established.
unresolved_cpu_finishers_003:
  .word 0x48000200, clip(394), 0x33000000, word(unresolved_cpu_finishers_003)
  .zero 124
  .word 0x32000000, 0x443cb0
  .zero 324
  .word 0x48000200, clip(386)

; Retained referenced block; semantic role is not established.
unresolved_cpu_finishers_004:
  .word 0x12000034, 0x3000000, 0x33000000, word(unresolved_cpu_finishers_004), 0x1, 0x4468c0
  .word 0x2, 0x4468c0, 0x12, 0x4468e0, 0x11000200, clip(13)
  .word 0x32000000, 0x446840, 0x1, 0x4468c0, 0x2, 0x4468c0
  .word 0x12, 0x4468e0

; Indexed table; reader in game/data/tables.c. Script slots 16/17 alias 2/7 where the reader specifies it.
first_finish_table:
  .word MK4_NATIVE_SCORPION_BURN_FATALITY  ; row 0

first_finish_table_row_01:
  .word MK4_NATIVE_RAIDEN_FIRST_FATALITY  ; row 1

first_finish_table_row_02:
  .word MK4_NATIVE_SONYA_FIRST_FATALITY  ; row 2

first_finish_table_row_03:
  .word MK4_NATIVE_LIU_KANG_FIRST_FATALITY  ; row 3

first_finish_table_row_04:
  .word MK4_NATIVE_SUB_ZERO_FIRST_FATALITY  ; row 4

first_finish_table_row_05:
  .word MK4_NATIVE_FUJIN_FIRST_FATALITY  ; row 5

first_finish_table_row_06:
  .word MK4_NATIVE_SHINNOK_FATALITY_1  ; row 6

first_finish_table_row_07:
  .word MK4_NATIVE_REIKO_FIRST_FATALITY  ; row 7

first_finish_table_row_08:
  .word MK4_NATIVE_QUAN_CHI_FIRST_FATALITY  ; row 8

first_finish_table_row_09:
  .word MK4_NATIVE_TANYA_FATALITY_1  ; row 9

first_finish_table_row_10:
  .word MK4_NATIVE_REPTILE_FATALITY_1  ; row 10

first_finish_table_row_11:
  .word MK4_NATIVE_KAI_FATALITY_1  ; row 11

first_finish_table_row_12:
  .word MK4_NATIVE_JAREK_FATALITY_1  ; row 12

first_finish_table_row_13:
  .word MK4_NATIVE_JAX_FATALITY_1  ; row 13

first_finish_table_row_14:
  .word MK4_NATIVE_CAGE_FATALITY_1  ; row 14

first_finish_table_row_15:
  .word 0  ; row 15
  .word 0x9, 0x147a, 0x7, 0x1999, 0x3, 0x4ccc
  .word 0xb, 0xffffc000, 0x9, 0x147a, 0xf, 0x1999
  .word 0x10, 0x1999, 0xb, 0xffffc000, 0x6, 0xffffe667
  .word 0xc, 0xffffc000, 0xd, 0x3333, 0xe, 0x3333
  .word 0x3, 0x4ccc, 0xa, 0x147a, 0x7, 0x1999
  .word 0x8, 0x1999, 0xf, 0x1999, 0xc, 0xffffc000
  .word 0xa, 0x147a, 0xd, 0x3333, 0x5, 0xffffe667
  .word 0x8, 0x1999, 0x10, 0x1999, 0x6, 0xffffe667
  .word 0xe, 0x3333, 0x5, 0xffffe667, 0x2, 0x1999
  .zero 8
  .word 0x18000, 0xc0000, 0xfff40000, 0xfffd8000, 0xa0000, 0xa0000
  .word 0xfffdcccd, 0xfff80000, 0x80000, 0xfffe199a, 0xfffa0000, 0xfffa0000
  .word 0xfffe6667, 0x40000, 0xfffc0000, 0xfffeb334, 0x20000, 0x20000
  .word 0xffff0000
  .zero 8
  .word 0xffff4ccd, 0x30000, 0xfff60000
  .zero 4
  .word 0xfffe6667, 0xfffb0000, 0xfffb0000, 0xfffe6667, 0xfffd0000, 0xfff90000
  .word 0xfffe6667, 0xfffd0000, 0xfff90000, 0xfffe6667, 0xfffd0000, 0xfff90000
  .word 0xfffe6667, 0xfffd0000, 0xfff90000, 0xfffe6667, 0xfffd0000, 0xfff90000
  .word 0xfffe6667, 0x250f234, 0x250f250, 0x250f26c, 0x250f288, 0x250f2a4
  .word 0x250f2c0, 0x250f2dc, 0x250f2f8, 0x250f314, 0x250f330, 0x250f34c
  .word 0x250f368, 0x250f384, 0x250f3a0, 0x250f3bc
  .zero 4
  .word 0x3, 0x4ccc, 0x2, 0x1999, 0x5, 0xffffe667
  .word 0x6, 0xffffe667, 0x7, 0x1999, 0x8, 0x1999
  .word 0x9, 0x147a, 0xa, 0x147a
  .zero 8
  .word 0x150f11c, 0x150f138, 0x150f154, 0x150f170, 0x150f18c, 0x150f18c
  .word 0x150f18c, 0xa50f18c
  .zero 8
  .word 0x250f1a8, 0x250f1c4, 0x250f1e0, 0x250f1fc, 0x250f218
  .zero 4
  .word 0x250f1e0, 0x250f1fc, 0x250f218, 0x250f1a8, 0x250f1fc, 0x250f1c4
  .zero 8
  .word 0x150f004, 0x150f020, 0x150f03c, 0x150f058, 0x150f074, 0x150f074
  .word 0x150f074, 0xa50f074
  .zero 8
  .word 0x250f090, 0x250f0ac, 0x250f0c8, 0x250f0e4, 0x250f100
  .zero 4
  .word 0x250f0c8, 0x250f0e4, 0x250f100, 0x250f090, 0x250f0e4, 0x250f0ac
  .zero 8
  .word 0x2, 0x7, 0x8, 0xd, 0xe, 0x4
  .word 0x5, 0x6, 0xf, 0x10, 0x3, 0x4
  .zero 8
  .word 0x150db74, 0x150db90, 0x150dbac, 0x150dbc8, 0x250dbe4, 0x250dc00
  .word 0x250d89c, 0x150d8b8, 0x150d8d4, 0x150d8f0, 0x150d928, 0x150d960
  .word 0x8150d998, 0x405dd0, 0x9, 0x48000, 0x7, 0x1e
  .word 0xf
  .zero 8
  .word 0x49999
  .zero 4
  .word 0x10, 0x4ccc, 0xe, 0x3c, 0xf, 0x10000
  .word 0x9999, 0x30000, 0x7ae, 0x7, 0x48000, 0x5
  .word 0x1e, 0xf
  .zero 8
  .word 0x49999
  .zero 4
  .word 0xa, 0x19999, 0x8, 0x3c, 0xf
  .zero 8
  .word 0x1b333
  .zero 4
  .word 0x5, 0x48000, 0x6, 0x1770, 0xf
  .zero 8
  .word 0x49999
  .zero 4
  .word 0xe, 0x4ccc, 0xc, 0x5a, 0xf, 0x10000
  .word 0x9999, 0x30000, 0xffffe667, 0xf, 0x60000, 0xd
  .word 0x1e, 0xf, 0x14ccc, 0x9999, 0x30000, 0x7ae
  .word 0x8, 0x19999, 0x6, 0x1e, 0xf
  .zero 8
  .word 0x1b333
  .zero 4
  .word 0xd, 0x60000, 0xb, 0x1e, 0xf, 0x14ccc
  .word 0x9999, 0x30000, 0xffffe667, 0x6, 0x19999, 0x5
  .word 0x1770, 0xf
  .zero 8
  .word 0x1b333
  .zero 4
  .word 0xb, 0x60000, 0x6, 0x1770, 0xf, 0x14ccc
  .word 0x9999, 0x30000, 0xffffe667, 0xc, 0x4ccc, 0x5
  .word 0x1770, 0xf, 0x10000, 0x9999, 0x30000, 0xffffe667
  .word 0x3, 0x31999, 0x4, 0x1770, 0xf, 0x50000
  .zero 48
  .word 0x2, 0x31999, 0x3, 0x1770, 0xf, 0x50000
  .zero 8
  .word 0xfffff852
  .zero 36
  .word 0x40000, 0x20000, 0xfffb4ccd, 0xfffc0000, 0x20000, 0xfffb4ccd
  .word 0xfffc0000, 0x20000, 0xfffb4ccd, 0xfffc0000, 0x20000, 0xfffb4ccd
  .word 0xfffc0000, 0x20000, 0xfffb4ccd, 0xfffc0000, 0x20000, 0xfffb4ccd
  .word 0xfffc0000, 0x20000, 0xfffb4ccd, 0xfffc0000, 0x20000, 0xfffb4ccd
  .word 0xfffc0000
  .zero 4
  .word 0x40000, 0xfffb0000, 0xfff80000, 0xfffc0000, 0xfffb0000, 0xfff80000
  .word 0xfffc0000, 0xfffb0000, 0xfff80000, 0xfffc0000, 0xfffb0000, 0xfff80000
  .word 0xfffc0000, 0xfffb0000, 0xfff80000, 0xfffc0000, 0xfffb0000, 0xfff80000
  .word 0xfffc0000, 0xfffb0000, 0xfff80000, 0xfffc0000, 0xfffb0000, 0xfff80000
  .word 0xfffc0000
  .zero 4
  .word 0x40000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0xffffe419, 0xfff70000
  .zero 4
  .word 0x40000
  .zero 4
  .word 0xfffc6148, 0xfff80000
  .zero 4
  .word 0xfffadeb9, 0xfff92148
  .zero 4
  .word 0xfff9b852, 0xfffa9eb9
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a
  .zero 4
  .word 0xfff90000, 0xfffd999a, 0x8150f6e8, 0x444460, 0x8150f704, 0x444460
  .word 0x8150f720, 0x444460, 0x8150f73c, 0x444460, 0x8150f758, 0x444460
  .word 0x8150f774, 0x444460, 0x8150f6e8, 0x444460, 0x8150f704, 0x444460
  .word 0x8150f720, 0x444460, 0x8150f73c, 0x444460, 0x8150f758, 0x444460
  .word 0x8150f774, 0x444460, 0x8150f6e8, 0x444460, 0x8150f704, 0x444460
  .word 0x8150f720, 0x444460, 0x8150f73c, 0x444460, 0x8150f758, 0x444460
  .word 0x8150f774, 0x444460, 0x8150f790, 0x444460, 0x8150f7ac, 0x444460
  .word 0x8150f7c8, 0x444460, 0x8150f7e4, 0x444460, 0x8050f7e4, 0x405dd0
  .word 0x450f5b4, 0x450f5d0, 0x450f5ec, 0x450f608, 0x150f624, 0x150f640
  .word 0x150f65c, 0x150f678, 0x150f624, 0x150f640, 0x150f65c, 0x150f678
  .word 0x150f624, 0x150f640, 0x150f65c, 0x150f678, 0x150f624, 0x150f640
  .word 0x150f65c, 0x150f678, 0x150f624, 0x150f640, 0x150f65c, 0x150f678
  .word 0x150f624, 0x150f640, 0x150f65c, 0x150f678, 0x150f624, 0x150f640
  .word 0x150f65c, 0x150f678, 0x250f694, 0x250f6b0, 0x250f6cc, 0x8050f6cc
  .word 0x444d30
  .zero 4
  .word 0x2350f800, 0x1950f800, 0xf50f800, 0x150f800, 0x150f81c, 0x150f838
  .word 0x150f854, 0x150f870, 0x150f88c, 0x150f8a8, 0x150f8c4, 0x8050f8c4
  .word 0x405dd0
  .zero 4
  .word 0x1950f800, 0xf50f800, 0x150f800, 0x150f81c, 0x150f838, 0x150f854
  .word 0x150f870, 0x150f88c, 0x150f8a8, 0x150f8c4, 0x8050f8c4, 0x405dd0
  .zero 8
  .word 0xf50f800, 0x150f800, 0x150f81c, 0x150f838, 0x150f854, 0x150f870
  .word 0x150f88c, 0x150f8a8, 0x150f8c4, 0x8050f8c4, 0x405dd0
  .zero 4
  .word 0x150f800, 0x150f81c, 0x150f838, 0x150f854, 0x150f870, 0x150f88c
  .word 0x150f8a8, 0x150f8c4, 0x8050f8c4, 0x405dd0
  .zero 8
  .word 0x150f854, 0x150f870, 0x150f88c, 0x150f8a8, 0x150f8c4, 0x8050f8c4
  .word 0x405dd0
  .zero 4
  .word 0x70000
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
  .word 0xfff80000, 0xfffe0000, 0x50000, 0xfff90000, 0xfffe199a, 0x50000
  .word 0xfff90000, 0xfffe2667, 0x50000, 0xfff90000, 0xfffe3334, 0x50000
  .word 0xfff90000, 0xfffe4000, 0x50000, 0xfff90000, 0xfffe4ccd, 0x50000
  .word 0xfff90000, 0xfffe4ccd, 0x50000, 0xfff90000, 0xfffe4ccd
