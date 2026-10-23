; Contact/balance records and retained intervening data.
; Edit values in place; references use labels. Record widths/counts remain reader contracts.

  .word 0xffffffff, 0x4e574f44, 0x44414f4c, 0x616c5020, 0x20726579, 0x34702034
  .word 0x6168635f, 0x64252072, 0x5f327020, 0x746c616e, 0x642520, 0x4e574f44
  .word 0x44414f4c, 0x616c5020, 0x20726579, 0x33702033, 0x6168635f, 0x64252072
  .word 0x5f317020, 0x746c616e, 0x642520, 0x4e574f44, 0x44414f4c, 0x616c5020
  .word 0x20726579, 0x32702032, 0x6168635f, 0x64252072, 0x5f327020, 0x746c616e
  .word 0x642520, 0x4e574f44, 0x44414f4c, 0x616c5020, 0x20726579, 0x31702031
  .word 0x6168635f, 0x64252072, 0x5f317020, 0x746c616e, 0x642520
  .zero 4

; Query weapon, selector 42. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query weapon, selector 44. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_42_record:
  .word 0x1a666, 0xffff3db3, 0x12b43, 0x12106, 0x770aa9, 0x28f5  ; contact weapon 44

; Query weapon, selector 43. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_43_record:
  .word 0x204dd, 0xffff7605, 0x15439, 0xb062, 0x770aaa, 0x23d7  ; contact weapon 43

; Query weapon, selector 40. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_40_record:
  .word 0x204dd, 0xffff7605, 0x15439, 0xb062, 0x779898, 0x2e14  ; contact weapon 40

; Query weapon, selector 39. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_39_record:
  .word 0x1e0c4, 0xffff974c, 0x16624, 0x3810, 0x769797, 0  ; contact weapon 39

; Query weapon, selector 38. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_38_record:
  .word 0x1c24d, 0x4f5c, 0x11eb8, 0xc76c, 0x1769696, 0x2666  ; contact weapon 38

; Query weapon, selector 37. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_37_record:
  .word 0x14ccc, 0xffff2b86, 0x10ccc, 0xdeb8, 0x779595, 0x28f5  ; contact weapon 37

; Query weapon, selector 36. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_36_record:
  .word 0x1ee14, 0xffff8000, 0xfd70, 0xdeb8, 0x772c2c, 0x2666  ; contact weapon 36

; Query weapon, selector 35. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_35_record:
  .word 0x1b333, 0xffff75c3, 0x14f5c, 0x6b43, 0x769494, 0x2666  ; contact weapon 35

; Query weapon, selector 34. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_34_record:
  .word 0xf0a3, 0xffffa8f6, 0xcf5c, 0x6666, 0x779393, 0x2e14  ; contact weapon 34

; Query weapon, selector 41. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_41_record:
  .word 0x1828f, 0xffff8f5d, 0x107ae, 0x8f5c, 0x770a92, 0x2e14  ; contact weapon 41

; Query weapon, selector 31. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_31_record:
  .word 0x1e666, 0xffff7ae2, 0x1451e, 0xc7ae, 0x778e8e, 0x2e14  ; contact weapon 31

; Query weapon, selector 32. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_32_record:
  .word 0x1e666, 0xffff7ae2, 0x1451e, 0xc7ae, 0x779191, 0x28f5  ; contact weapon 32

; Query weapon, selector 33. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_33_record:
  .word 0x1828f, 0xffff4d0f, 0x107ae, 0x70a3, 0x770a92, 0x2e14  ; contact weapon 33

; Query weapon, selector 30. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_30_record:
  .word 0x1c24d, 0xa147, 0x11eb8, 0x7ae1, 0x1768d8d, 0x2e14  ; contact weapon 30

; Query weapon, selector 29. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_29_record:
  .word 0x1eb43, 0xffff78d5, 0x18000, 0x10a3d, 0x768c8c, 0x3333  ; contact weapon 29

; Query weapon, selector 28. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_28_record:
  .word 0x1eb43, 0xffff78d5, 0x18000, 0x10a3d, 0x768b8b, 0x2b85  ; contact weapon 28

; Query weapon, selector 23. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_23_record:
  .word 0x1c000, 0xffff47af, 0x1170a, 0x10a3d, 0x770a35, 0x2e14  ; contact weapon 23

; Query weapon, selector 24. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_24_record:
  .word 0x14c8b, 0xffff8084, 0x1076c, 0x4a3d, 0x772f2f, 0x2e14  ; contact weapon 24

; Query weapon, selector 25. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_25_record:
  .word 0x1d1a9, 0xffff4a3e, 0x1170a, 0x12106, 0x775d5d, 0x2147  ; contact weapon 25

; Query weapon, selector 27. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
axe_followup_attack_record:
  .word 0x1d1a9, 0xffff4a3e, 0x1170a, 0x12106, 0x778a8a, 0x2147

; Query weapon, selector 26. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_26_record:
  .word 0x1f581, 0xffff7605, 0x16872, 0x5958, 0x773333, 0x2e14  ; contact weapon 26

; Query weapon, selector 21. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_21_record:
  .word 0x18000, 0xffff2b86, 0x111eb, 0x13ae1, 0x779090, 0x1eb8  ; contact weapon 21

; Query weapon, selector 20. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
flurry_attack_record:
  .word 0x16666, 0xfffed1ec, 0x1147a, 0xe624, 0x700a89, 0xccc

; Query weapon, selector 19. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
sweep_attack_record:
  .word 0x1cccc, 0xffff199a, 0x1451e, 0xb0a3, 0x770a60, 0x2147

; Query weapon, selector 18. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_18_record:
  .word 0x251eb, 0xffff0ccd, 0x1851e, 0x14c8b, 0x773f3f, 0x2666  ; contact weapon 18

; Query weapon, selector 17. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_17_record:
  .word 0x1b333, 0xffff9c30, 0x15eb8, 0x2e14, 0x763e3e, 0x2666  ; contact weapon 17

; Query weapon, selector 16. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_16_record:
  .word 0x1851e, 0xffff4000, 0xfd70, 0xd70a, 0x773d3d, 0x2666  ; contact weapon 16

; Query weapon, selector 15. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_15_record:
  .word 0x168f5, 0xffff8f5d, 0x1147a, 0x2e14, 0x750a3c, 0x11eb  ; contact weapon 15

; Query weapon, selector 12. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_12_record:
  .word 0x19eb8, 0xffff2b86, 0x130a3, 0x13ae1, 0x773a3a, 0x2147  ; contact weapon 12

; Query weapon, selector 13. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_13_record:
  .word 0x19eb8, 0xffff2b86, 0x130a3, 0x13ae1, 0x773b3b, 0x28f5  ; contact weapon 13

; Query weapon, selector 14. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_14_record:
  .word 0x1d47a, 0xffff91ec, 0x1828f, 0x7d70, 0x773b3b, 0x28f5  ; contact weapon 14

; Query weapon, selector 11. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_11_record:
  .word 0x1828f, 0xffff8f5d, 0x107ae, 0x8f5c, 0x773939, 0x23d7  ; contact weapon 11

; Query weapon, selector 10. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_10_record:
  .word 0x1828f, 0xffff8f5d, 0x107ae, 0x8f5c, 0x773737, 0x3333  ; contact weapon 10

; Query weapon, selector 8. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_08_record:
  .word 0x1e666, 0xffff7ae2, 0x1451e, 0xc7ae, 0x773636, 0x2e14  ; contact weapon 8

; Query weapon, selector 9. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_09_record:
  .word 0x1ee14, 0xffff8000, 0xfd70, 0xdeb8, 0x773838, 0x2666  ; contact weapon 9

; Query weapon, selector 7. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_07_record:
  .word 0x1c000, 0xffff9eb9, 0x1170a, 0x8a3d, 0x770a35, 0x2666  ; contact weapon 7

; Query weapon, selector 6. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_06_record:
  .word 0x1851e, 0xffff4000, 0xfd70, 0xd70a, 0x773434, 0x2666  ; contact weapon 6

; Query weapon, selector 4. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_04_record:
  .word 0x19999, 0xffff947b, 0x1147a, 0x51eb, 0x773333, 0x2666  ; contact weapon 4

; Query weapon, selector 5. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_05_record:
  .word 0x170a3, 0xffff9eb9, 0x123d7, 0x570a, 0x772f2f, 0x2666  ; contact weapon 5

; Query weapon, selector 22. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_22_record:
  .word 0x13333, 0x76c, 0x2f333, 0x6147, 0x760a63, 0xccc  ; contact weapon 22

; Query weapon, selector 0. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query weapon, selector 1. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_00_record:
  .word 0x18000, 0xffff75c3, 0xdeb8, 0x9999, 0x770c30, 0x3333  ; contact weapon 1
  .word 0x1570a, 0xffff75c3, 0xca3d, 0x9999, 0x770c30, 0x3333

; Query weapon, selector 2. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_02_record:
  .word 0x1e666, 0xffff7ae2, 0x1451e, 0xc7ae, 0x770c31, 0x3333  ; contact weapon 2

; Query weapon, selector 3. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_weapon_03_record:
  .word 0x1b333, 0xffff7ae2, 0x1451e, 0xc7ae, 0x770c32, 0x3333  ; contact weapon 3

; Query b, selector 11. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
handstand_rear_attack_record:
  .word 0x1e624, 0xffffdc6b, 0x10000, 0x9958, 0x57fa7, 0x170a

; Query b, selector 10. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
handstand_front_attack_record:
  .word 0x1851e, 0xffff42d1, 0x10000, 0x9958, 0x57f7f, 0x1c28

; Query b, selector 9. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
lunge_attack_record:
  .word 0x18000, 0xffff574c, 0xe147, 0x13ced, 0x57f7f, 0x23d7

; Query b, selector 30. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_30_record:
  .word 0x12147, 0xffffe148, 0xeb85, 0x6147, 0x2041717, 0x1999  ; contact b 30

; Query b, selector 31. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_31_record:
  .word 0x1170a, 0xffffee15, 0x1051e, 0x6147, 0x2041717, 0x1999  ; contact b 31

; Query b, selector 32. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_32_record:
  .word 0x12147, 0x51e, 0x9999, 0x4ccc, 0x2041717, 0x1999  ; contact b 32

; Query b, selector 27. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_27_record:
  .word 0x12666, 0xfffff5c3, 0x107ae, 0x851e, 0x2041616, 0x2666  ; contact b 27

; Query b, selector 28. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_28_record:
  .word 0xd999, 0xfffff5c3, 0xca3d, 0x947a, 0x2041616, 0x2666  ; contact b 28

; Query b, selector 29. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_29_record:
  .word 0xd999, 0x7ae, 0xde76, 0x75c2, 0x2041616, 0x2666  ; contact b 29

; Query b, selector 0. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_00_record:
  .word 0x1451e, 0xffff3852, 0x10000, 0xc7ae, 0x60d10, 0x2147  ; contact b 0

; Query b, selector 1. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_01_record:
  .word 0x15e76, 0xffff2667, 0x151a9, 0x111eb, 0x60d10, 0x2147  ; contact b 1

; Query b, selector 2. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_02_record:
  .word 0x116c8, 0xffff218a, 0x1476c, 0x12395, 0x60d10, 0x2147  ; contact b 2

; Query b, selector 5. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_05_record:
  .word 0x17d70, 0xffffbd71, 0xfae1, 0x70a3, 0x60a11, 0x1c28  ; contact b 5

; Query b, selector 6. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_06_record:
  .word 0x170a3, 0xffffca3e, 0x11958, 0x5c28, 0x60a11, 0x1c28  ; contact b 6

; Query b, selector 7. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_07_record:
  .word 0x15c28, 0xffffca3e, 0x104df, 0x5c28, 0x60a11, 0x1c28  ; contact b 7

; Query b, selector 35. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_35_record:
  .word 0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 0x1041212, 0x23d7  ; contact b 35

; Query b, selector 36. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_36_record:
  .word 0x170a3, 0x8518, 0xdeb8, 0x8000, 0x1041313, 0x23d7  ; contact b 36

; Query b, selector 38. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_38_record:
  .word 0xeb85, 0xffffd1ec, 0xab85, 0x11958, 0x30a1b, 0x1999  ; contact b 38

; Query b, selector 33. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_33_record:
  .word 0x10f5c, 0xffff547b, 0xf0a3, 0x6b85, 0x51414, 0x2e14  ; contact b 33

; Query b, selector 34. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_34_record:
  .word 0x10f5c, 0xffff547b, 0xf0a3, 0x6b85, 0x50f0f, 0x2e14  ; contact b 34

; Query d, selector 12. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
flying_attack_record:
  .word 0x14f5c, 0xfffff0a4, 0xf5c2, 0x51eb, 0x1f1f, 0x23d7

; Query b, selector 17. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_17_record:
  .word 0xf851, 0xffff63d8, 0xf5c2, 0xeb85, 0x1e1e, 0x2e14  ; contact b 17

; Query b, selector 40. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_40_record:
  .word 0x107ae, 0xffffb852, 0xe666, 0xa8f5, 0x80a15, 0x2147  ; contact b 40

; Query b, selector 39. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_39_record:
  .word 0x10f5c, 0xffff999a, 0xfae1, 0x75c2, 0x80a15, 0x2147  ; contact b 39

; Query a, selector 4. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_04_record:
  .word 0x107ae, 0xffff63d8, 0xdc28, 0xa8f5, 0x8f0a26, 0x170a  ; contact a 4

; Query a, selector 5. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_05_record:
  .word 0x12b85, 0xffffb0a4, 0x10a3d, 0xf5c2, 0x42727, 0x1eb8  ; contact a 5

; Query d, selector 6. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
bicycle_attack_record:
  .word 0xf851, 0xffffab86, 0xf5c2, 0x70a3, 0x5454, 0x2666

; Query b, selector 24. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_24_record:
  .word 0x10000, 0xfffff5c3, 0x1051e, 0x47ae, 0x50a18, 0x1eb8  ; contact b 24

; Query b, selector 25. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_25_record:
  .word 0xbd70, 0xffff9eb9, 0xae14, 0x75c2, 0x50a19, 0x1eb8  ; contact b 25

; Query b, selector 26. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_26_record:
  .word 0xa8f5, 0xffff28f6, 0xa3d7, 0xb851, 0x71a1a, 0x1999  ; contact b 26

; Query b, selector 23. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_b_23_record:
  .word 0x13d70, 0x87ae, 0x107ae, 0x8000, 0x40a1c, 0xf5c  ; contact b 23

; Query a, selector 3. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_03_record:
  .word 0x10000, 0xfffff0a4, 0xa624, 0x51eb, 0x40a1d, 0xa3d  ; contact a 3

; Query a, selector 8. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
weapon_air_lp_attack_record:
  .word 0x18ccc, 0xffffee15, 0xfae1, 0xd1eb, 0x49b9b, 0x2e14

; Query a, selector 7. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
weapon_air_hp_attack_record:
  .word 0x1b062, 0xffff122e, 0x10a3d, 0xf5c2, 0x49c9c, 0x2e14

; Query a, selector 0. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_00_record:
  .word 0xc7ae, 0xffff4d0f, 0x970a, 0x6e14, 0x10b20, 0xccc  ; contact a 0

; Query a, selector 1. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_01_record:
  .word 0xf333, 0xffffb852, 0xcccc, 0x7333, 0x10b25, 0xa3d  ; contact a 1

; Query a, selector 2. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_02_record:
  .word 0xe3d7, 0xffff0290, 0x947a, 0x10518, 0x62222, 0x28f5  ; contact a 2

; Query a, selector 6. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_06_record:
  .word 0x12106, 0xfffeee57, 0xfa9f, 0x17a9f, 0x62222, 0xa3d  ; contact a 6

; Query a, selector 18. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_18_record:
  .word 0x1ba9f, 0xfffeee57, 0x19439, 0x17a9f, 0xafafaf, 0  ; contact a 18

; Query a, selector 19. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_19_record:
  .word 0x1ba9f, 0xfffeee57, 0x19439, 0x17a9f, 0xb6b6b6, 0  ; contact a 19

; Query a, selector 16. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_16_record:
  .word 0x10000, 0xffff6bc7, 0xdeb8, 0xc9fb, 0x30a2b, 0xa3d  ; contact a 16

; Query a, selector 17. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_17_record:
  .word 0x10000, 0xffff75c3, 0xdeb8, 0x63d7, 0x70a24, 0x147a  ; contact a 17

; Query projectile, selector 20. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
boomerang_attack_record:
  .word 0xc76c, 0xffffbdb3, 0x9be7, 0x8c8b, 0x2009aa9, 0x3333

; Query projectile, selector 19. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_projectile_19_record:
  .word 0x6b43, 0xffffa419, 0xe395, 0xbfb7, 0x2079f9f, 0x451e  ; contact projectile 19

; Query projectile, selector 18. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
weapon_throw_attack_record:
  .word 0xe106, 0xffff6bc7, 0x12106, 0x8c8b, 0x2009a9a, 0x4000

; Query projectile, selector 17. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
acid_attack_record:
  .word 0x1072b, 0xffff7d30, 0xcf1a, 0x6147, 0x2022929, 0x1999

; Query projectile, selector 21. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
star_attack_record:
  .word 0x1072b, 0xffff7d30, 0xcf1a, 0x6147, 0x2022d2d, 0x2147

; Query projectile, selector 9. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
rise_attack_record:
  .word 0x3d70, 0xffff47af, 0x7d70, 0x96c8, 0x2008282, 0x2147

; Query projectile, selector 8. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
drop_attack_record:
  .word 0x4000, 0x1958, 0x7d70, 0x96c8, 0x2008181, 0x2147

; Query projectile, selector 7. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
skull_attack_record:
  .word 0x824d, 0xffff9688, 0xc000, 0x947a, 0x2060a80, 0x1999

; Query projectile, selector 16. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
bolt_attack_record:
  .word 0x3d70, 0xffffe148, 0x7d70, 0x3ae1, 0x2020a65, 0xccc

; Query projectile, selector 6. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
spear_projectile:
  .word 0x3333, 0xffffe148, 0x947a, 0x3ae1, 0x2704646, 0xccc  ; contact projectile 6

; Query projectile, selector 0. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
orb_attack_record:
  .word 0x3d70, 0xffffe148, 0x7d70, 0x3ae1, 0x2020a40, 0x1999

; Query projectile, selector 1. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
fireball_low_attack_record:
  .word 0x3d70, 0xffffe148, 0x7d70, 0x3ae1, 0x2020a41, 0x1999

; Query projectile, selector 2. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
generic_projectile_attack_record:
  .word 0x570a, 0xffffe148, 0xa147, 0x3ae1, 0x2020a42, 0x23d7

; Query projectile, selector 3. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
ice_projectile_attack_record:
  .word 0x570a, 0xffffe148, 0xa147, 0x3ae1, 0x2004343, 0

; Query d, selector 5. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
ice_clone_attack_record:
  .word 0x4ccc, 0xfffff5c3, 0xbae1, 0x9e76, 0x2004343, 0

; Query d, selector 4. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
teleport_attack_record:
  .word 0x128f5, 0xffffc51f, 0xab85, 0x91eb, 0x2005a5a, 0x170a

; Query projectile, selector 4. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
blade_attack_record:
  .word 0x570a, 0xffffe148, 0xa147, 0x3ae1, 0x2020a44, 0x1eb8

; Query d, selector 3. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
leg_attack_record:
  .word 0x168f5, 0xffff8ccd, 0x14f5c, 0x12b85, 0x1085858, 0x2e14

; Query d, selector 30. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
fire_attack_record:
  .word 0x24e56, 0xffff3688, 0x23cac, 0xb7ce, 0xadad, 0x1eb8

; Query d, selector 29. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
flip_kick_attack_record:
  .word 0x1353f, 0xffff54fe, 0x12395, 0x1b2b0, 0x9abab, 0x1eb8

; Query d, selector 28. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
spin_attack_record:
  .word 0x12dd2, 0xf5c, 0x16872, 0x6dd2, 0x39e9e, 0x1eb8

; Query d, selector 25. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
shadow_attack_record:
  .word 0x17fbe, 0xffffae57, 0x13020, 0x2b43, 0xa5a2a2, 0x23d7

; Query d, selector 26. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
power_attack_record:
  .word 0xc7ae, 0xfffe99dc, 0xe872, 0x13020, 0x3a3a3, 0x1eb8

; Query d, selector 27. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
groin_punch_attack_record:
  .word 0xd6c8, 0x28f5, 0xba5e, 0xd3f7, 0xa5a4a4, 0x28f5

; Query d, selector 24. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
flip_attack_record:
  .word 0xd1eb, 0x570a, 0x16872, 0xbfbe, 0x8a0a0, 0x2666

; Query d, selector 23. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
dash_attack_record:
  .word 0xd1eb, 0x570a, 0x16872, 0xbfbe, 0xa59e9e, 0x1eb8

; Query d, selector 22. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
dash_attack_record_648:
  .word 0x16b43, 0xffffb375, 0xcf1a, 0x2b85, 0xa50a9d, 0x28f5

; Query d, selector 20. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_d_20_record:
  .word 0x125e3, 0xffffb646, 0xb84b, 0x8f1a, 0x90a11, 0x170a  ; contact d 20

; Query d, selector 21. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
roll_attack_record:
  .word 0xe872, 0xffffc084, 0xb84b, 0x56c8, 0x90a28, 0x170a

; Query d, selector 19. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
ball_attack_record:
  .word 0x5958, 0xffffb646, 0xccc6, 0xadd2, 0x2a2a, 0x23d7

; Query d, selector 18. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
cartwheel_attack_record:
  .word 0x12624, 0xffff4fe0, 0x11993, 0x15be7, 0x97979, 0x11eb

; Query d, selector 17. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
drill_attack_record:
  .word 0x12624, 0xffff974c, 0x11993, 0x15be7, 0x97a5e, 0x2666

; Query d, selector 16. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
aerial_attack_record:
  .word 0x107ae, 0xffff70e6, 0x107ae, 0xb851, 0x7b7b, 0x2147

; Query d, selector 15. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
dive_attack_record:
  .word 0x12666, 0xfffff5c3, 0x107ae, 0x851e, 0xa0a, 0x147a

; Query d, selector 14. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
teleport_slam_attack_record:
  .word 0x11c28, 0xffff0a7f, 0xfa9f, 0x1cc8b, 0x1858, 0

; Query d, selector 13. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
handstand_spin_attack_record:
  .word 0x111a9, 0xffffc51f, 0x16b43, 0x51a9, 0xa7e, 0x170a

; Query d, selector 11. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
weapon_steal_attack_record:
  .word 0x4a3d7, 0xfffe87af, 0x4a3d7, 0x1e147, 0x6c6c, 0x7ae

; Query d, selector 10. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
stomp_attack_record:
  .word 0x68f5, 0x6e14, 0xf0a3, 0xa3d7, 0x6a6a, 0x3851

; Query d, selector 9. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
triple_attack_record:
  .word 0x970a, 0xfffee8f6, 0xb851, 0xffbe, 0x85d5d, 0x30a3

; Query d, selector 8. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_d_08_record:
  .word 0x470a3, 0xffff147b, 0x2fd70, 0x1f333, 0xa55c5c, 0  ; contact d 8

; Query d, selector 0. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
torpedo_attack_record:
  .word 0x14f5c, 0xfffff0a4, 0xf5c2, 0x51eb, 0x5252, 0x2666

; Query d, selector 7. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
slide_attack_record:
  .word 0xf0a3, 0x947a, 0x111eb, 0x8f5c, 0xa57a7a, 0x147a

; Query d, selector 1. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
aerial_bicycle_attack_record:
  .word 0x13ae1, 0xfffffae2, 0xf333, 0x51eb, 0x5656, 0x2666

; Query d, selector 2. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
square_attack_record:
  .word 0x4a3d, 0xffffd99a, 0x9999, 0xe147, 0x5757, 0x3333

; Query a, selector 9. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 10. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 11. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 12. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 13. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 14. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query a, selector 15. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 5. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 10. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 11. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 12. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 13. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 14. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
; Query projectile, selector 15. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_a_09_record:
  .word 0, 0, 0, 0, 0x11, 0x147a  ; contact projectile 15

; Query d, selector 31. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
contact_d_31_record:
  .word 0xe0c4, 0xffff5021, 0x109fb, 0xbced, 0x7bcbc, 0x1c28  ; contact d 31

; Query d, selector 32. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
punch_attack_record:
  .word 0x13810, 0xffff1efa, 0xfd2f, 0x11be7, 0x7bdbd, 0x2666

; Query d, selector 33. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
kick_attack_record:
  .word 0x1b2b0, 0xffff94bd, 0x16872, 0xa624, 0x7bebe, 0x2666

; Query d, selector 34. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
stomp_attack_record_666:
  .word 0x68f5, 0x44dd, 0xe666, 0xc76c, 0xbfbfbf, 0

; Query d, selector 35. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
uppercut_attack_record:
  .word 0x16b43, 0xffff30a4, 0x109fb, 0x1424d, 0x6c0c0, 0x23d7

; Query projectile, selector 22. Record: reach/top/length/height Q16, hit/air/block/flags bytes, Q16 damage.
; Reader: game/script/vm.c:script_vm_attack_record.
fireball_attack_record:
  .word 0x3d70, 0xffffae15, 0x7d70, 0x7333, 0x2070a40, 0x23d7

attack_records_end:
  .word 0x432a60  ; numeric; printable bytes do not establish text
  .word 0x432700, 0x12, 0x48e360, 0x48ebd0, 0x490110, 0x48fb70
  .word 0x48fee0

aux_reaction:
  .word MK4_NATIVE_VELOCITY_FRICTION, 0x41
  .zero 8
  .word 0x45fc90
  .zero 52
  .word 0x12, 0x48e360, word(unresolved_scene_assets_408), word(unresolved_scene_assets_409), 0x15e0000, 0x230000
  .word word(unresolved_scene_assets_412), word(unresolved_scene_assets_413), 0x15e0000, 0x1b0000, word(unresolved_scene_assets_414), word(unresolved_scene_assets_415)
  .word 0x1590000, 0x200000, word(unresolved_scene_assets_416), word(unresolved_scene_assets_417), 0x15e0000, 0x1b0000
  .word word(unresolved_scene_assets_418), word(unresolved_scene_assets_419), 0x15e0000, 0x1b0000, word(unresolved_scene_assets_420), word(unresolved_scene_assets_421)
  .word 0x15e0000, 0x1b0000, word(unresolved_scene_assets_422), word(unresolved_scene_assets_423), 0x15e0000, 0x1b0000
  .word word(unresolved_scene_assets_436), word(unresolved_scene_assets_437), 0x1560000, 0x1b0000, word(unresolved_scene_assets_424), word(unresolved_scene_assets_425)
  .word 0x15e0000, 0x200000, word(unresolved_scene_assets_426), word(unresolved_scene_assets_427), 0x14f0000, 0x200000
  .word word(unresolved_scene_assets_428), word(unresolved_scene_assets_429), 0x15e0000, 0x1f0000, word(unresolved_scene_assets_430), word(unresolved_scene_assets_431)
  .word 0x15e0000, 0x210000, word(unresolved_scene_assets_432), word(unresolved_scene_assets_433), 0x15e0000, 0x210000
  .word word(unresolved_scene_assets_410), word(unresolved_scene_assets_411), 0x1570000, 0x1b0000, word(unresolved_scene_assets_434), word(unresolved_scene_assets_435)
