; Editable MK4 wordcode. Insert/remove instructions freely; references follow labels.
.section script
MK4_SCRIPT_SUB_ZERO_FATALITY_2_VICTIM:
  prefixed_to 0x214, word(data_4fb724)
script_4d2bf8:
  sound_special 0x0
script_4d2bfc:
  forward_to 0x5a
script_4d2c00:
  sound_special 0x0
script_4d2c04:
  forward 0x0
script_4d2c08:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_VICTIM_END
MK4_SCRIPT_SUB_ZERO_FATALITY_2_BREATH:
  sound_indexed 0x4
script_4d2c14:
  play_clip 0xa, word(data_4fc75c)
script_4d2c1c:
  spawn 0x0, 0x0, MK4_NATIVE_BREATH_SCENE_DECORATION
script_4d2c28:
  sound_c 0x3f
script_4d2c2c:
  native_release 0x38
script_4d2c30:
  loop 0x3
script_4d2c34:
  cursor_set 0xa
script_4d2c38:
  forward_to 0x1d
script_4d2c3c:
  repeat 0x0
script_4d2c40:
  native_release 0x39
script_4d2c44:
  sound_c 0x40
script_4d2c48:
  forward_to 0x53
script_4d2c4c:
  scene_active 0x0
script_4d2c50:
  spawn 0x0, 0x0, MK4_NATIVE_BREATH_SHATTER_DECORATION
script_4d2c5c:
  loop 0x5
script_4d2c60:
  cursor_set 0x53
script_4d2c64:
  forward_to 0x5d
script_4d2c68:
  repeat 0x0
script_4d2c6c:
  forward 0x0
script_4d2c70:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_APPROACH
MK4_SCRIPT_SUB_ZERO_BREATH_APPROACH:
  native_release 0x3a
script_4d2c7c:
  wait 0x14
script_4d2c80:
  speed_negative 0x0, 0xffffd99a
script_4d2c88:
  sound_c 0xf
script_4d2c8c:
  open_clip 0x0, word(data_4fb1f0)
script_4d2c94:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SLIDE
  .zero 4

MK4_SCRIPT_BREATH_RESUME:
  open_clip 0x0, word(data_4fb994)
script_4d2ca8:
  cursor_set 0x19
script_4d2cac:
  forward 0x0
script_4d2cb0:
  sound_c 0x1c
script_4d2cb4:
  play_clip 0x5, word(data_4fbb38)
script_4d2cbc:
  sound_c 0x3b
script_4d2cc0:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_BREATH_SHATTER
MK4_SCRIPT_SUB_ZERO_BREATH_SHATTER:
  loop 0x8
script_4d2ccc:
  forward_to 0x8
script_4d2cd0:
  playback_pair 0x6
script_4d2cd4:
  repeat 0x0
script_4d2cd8:
  forward 0x0
script_4d2cdc:
  wait 0x3c
script_4d2ce0:
  tail 0x0, MK4_NATIVE_VICTORY_CONTINUE
  .zero 4

MK4_SCRIPT_REIKO_SECOND_FATALITY:
  sound_indexed 0x3e
script_4d2d14:
  native_release 0x40
script_4d2d18:
  wait 0x0
script_4d2d1c:
  scene_active 0x0
script_4d2d20:
  play_clip 0x1d, word(data_4fc768)
script_4d2d28:
  spawn 0x0, 0x0, MK4_NATIVE_REIKO_SECOND_STARS
script_4d2d34:
  loop 0x9
script_4d2d38:
  cursor_set 0x1d
script_4d2d3c:
  forward_to 0x26
script_4d2d40:
  repeat 0x0
script_4d2d44:
  cursor_set 0x1d
script_4d2d48:
  forward_to 0x21
script_4d2d4c:
  wait 0x14
script_4d2d50:
  forward 0x0
script_4d2d54:
  tail 0x0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
  .zero 4

MK4_SCRIPT_VICTIM_STAGGER:
  blend 0x0, word(data_4fc774)
script_4d2d68:
  open_clip 0x0, word(data_4fc774)
script_4d2d70:
  loop 0x4
script_4d2d74:
  cursor_set 0x0
script_4d2d78:
  forward_to 0x14
script_4d2d7c:
  repeat 0x0
script_4d2d80:
  native_release 0x41
script_4d2d84:
  forward_to 0x87
script_4d2d88:
  tail 0x0, MK4_NATIVE_REIKO_SECOND_VICTIM_FALL
MK4_SCRIPT_REIKO_SECOND_VICTIM_FALL:
  forward_to 0x91
script_4d2d94:
  sound_special 0x0
script_4d2d98:
  forward 0x0
script_4d2d9c:
  sound_special 0x0
script_4d2da0:
  native_release 0x42
script_4d2da4:
  tail 0x0, MK4_NATIVE_REIKO_SECOND_VICTIM_END
  .zero 4

MK4_SCRIPT_JAREK_FATALITY_2:
  native_release 0x3b
script_4d2db4:
  play_clip 0x1, word(data_4fc888)
script_4d2dbc:
  scene_active 0x0
script_4d2dc0:
  half_count 0x6
script_4d2dc4:
  call 0x0, MK4_NATIVE_JAREK_SECOND_GLOW
script_4d2dcc:
  half_count 0x13
script_4d2dd0:
  sound_b 0x1
script_4d2dd4:
  tail 0x0, MK4_NATIVE_JAREK_SECOND_HANDOFF
  .zero 4

MK4_SCRIPT_JAREK_SECOND_HANDOFF:
  half_count 0xc
script_4d2de4:
  sound_c 0xb
script_4d2de8:
  half_count 0x14
script_4d2dec:
  sound_c 0xb
script_4d2df0:
  half_count 0x14
script_4d2df4:
  sound_c 0xb
script_4d2df8:
  half_count 0x14
script_4d2dfc:
  sound_c 0xb
script_4d2e00:
  half_count 0x14
script_4d2e04:
  sound_c 0xb
script_4d2e08:
  tail 0x0, MK4_NATIVE_VICTORY_CONTINUE
MK4_SCRIPT_VICTIM_START:
  sound_c 0xb
script_4d2e14:
  play_clip 0x10, word(data_4fb604)
script_4d2e1c:
  backward_count 0xf
script_4d2e20:
  forward 0x0
script_4d2e24:
  sound_b 0xc
script_4d2e28:
  spawn 0x0, 0x0, MK4_NATIVE_JAREK_SECOND_HEAD_FALL
script_4d2e34:
  prefixed_to 0x214, word(data_4fb724)
script_4d2e3c:
  sound_special 0x0
script_4d2e40:
  forward_to 0x5a
script_4d2e44:
  forward 0x0
script_4d2e48:
  tail 0x0, MK4_NATIVE_JAREK_SECOND_VICTIM_HOLD
  .zero 4

MK4_SCRIPT_BEAST_START:
  native_release 0x3c
script_4d2e5c:
  play_clip 0x18, word(data_4fae0c)
script_4d2e64:
  sound_c 0x4b
script_4d2e68:
  sound_c 0x4a
script_4d2e6c:
  spawn 0x0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_TRANSFORM_EFFECT
script_4d2e78:
  forward 0x0
script_4d2e7c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_BEGIN
  .zero 4

MK4_SCRIPT_SCORPION_BEAST_BEGIN:
  scene_active 0x0
script_4d2e8c:
  clip 0x0, word(data_4fc894)
script_4d2e94:
  native_release 0x3d
script_4d2e98:
  clip 0x0, word(data_4fc8a0)
script_4d2ea0:
  sound_roster 0x1
script_4d2ea4:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_ATTACK
  .zero 4

MK4_SCRIPT_SCORPION_FATALITY_2_CREATURE_ATTACK:
  clip 0x0, word(data_4fc8ac)
script_4d2eb8:
  loop 0x5
script_4d2ebc:
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VOICE
script_4d2ec4:
  backward_count 0x8
script_4d2ec8:
  forward_count 0x8
script_4d2ecc:
  repeat 0x0
script_4d2ed0:
  spawn_effect 0x6
script_4d2ed4:
  sound_context 0x12
script_4d2ed8:
  sound_roster 0x8
script_4d2edc:
  clip 0x0, word(data_4fc8b8)
script_4d2ee4:
  native_release 0x3e
script_4d2ee8:
  bind_cursor 0x0, word(data_4fc894)
script_4d2ef0:
  tail 0x0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
MK4_SCRIPT_SCORPION_FATALITY_2_VICTIM:
  play_clip 0x36, word(data_4fc8dc)
script_4d2f00:
  sound_b 0x1
script_4d2f04:
  sound_context 0x7
script_4d2f08:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_VICTIM_PIECES
MK4_SCRIPT_SCORPION_BEAST_VICTIM_PIECES:
  forward 0x0
script_4d2f14:
  loop 0x5
script_4d2f18:
  backward_count 0x8
script_4d2f1c:
  forward_count 0x8
script_4d2f20:
  repeat 0x0
script_4d2f24:
  spawn 0x0, 0x0, MK4_NATIVE_SCORPION_SCRIPT_BEAST_GORE
script_4d2f30:
  clip 0x0, word(data_4fc8e8)
script_4d2f38:
  tail 0x0, MK4_NATIVE_TASK_PARK_60_TICK_LOOP
  .zero 4

MK4_SCRIPT_JAREK_LK_THROW_STARTUP:
  native_release 0x20
script_4ddffc:
  sound_indexed 0x7
script_4de000:
  open_peer_clip 0x0, word(data_4fb52c)
script_4de008:
  open_clip 0x0, word(data_4faee4)
script_4de010:
  playback_single 0x16
script_4de014:
  spawn_effect 0x0
script_4de018:
  sound_a 0x24
script_4de01c:
  sound_roster 0x1
script_4de020:
  damage 0x0, 0x1999
script_4de028:
  playback_single 0x3d
script_4de02c:
  native_step 0x99
script_4de030:
  tail 0x0, MK4_NATIVE_JAREK_LK_THROW_OWNER_RETURN
MK4_SCRIPT_TANYA_LK_THROW_STARTUP:
  native_release 0x18
script_4de03c:
  sound_indexed 0x1d
script_4de040:
  open_peer_clip 0x0, word(data_4fb514)
script_4de048:
  open_clip 0x0, word(data_4faf74)
script_4de050:
  playback_single 0x1e
script_4de054:
  tail 0x0, MK4_NATIVE_LK_BRANCH_CONTINUE
  .zero 4

MK4_SCRIPT_LK_BRANCH_CONTINUE:
  playback_single 0x2f
script_4de064:
  spawn_effect 0x0
script_4de068:
  sound_a 0x24
script_4de06c:
  sound_roster 0x1
script_4de070:
  damage 0x0, 0x1999
script_4de078:
  playback_single 0x7f
script_4de07c:
  native_step 0x87
script_4de080:
  tail 0x0, MK4_NATIVE_LK_RECOIL
MK4_SCRIPT_SHINNOK_LK_THROW_STARTUP:
  sound_indexed 0x37
script_4de08c:
  native_release 0x15
script_4de090:
  open_peer_clip 0x0, word(data_4fb454)
script_4de098:
  open_clip 0x0, word(data_4faef0)
script_4de0a0:
  playback_single 0x3b
script_4de0a4:
  disarm_peer 0x0
script_4de0a8:
  spawn_effect 0x0
script_4de0ac:
  sound_a 0x24
script_4de0b0:
  sound_roster 0x1
script_4de0b4:
  damage 0x0, 0x1999
script_4de0bc:
  playback_single 0x7f
script_4de0c0:
  spawn_effect 0x0
script_4de0c4:
  sound_a 0x24
script_4de0c8:
  sound_roster 0x1
script_4de0cc:
  damage 0x0, 0x23d7
script_4de0d4:
  playback_single 0x9d
script_4de0d8:
  native_step 0x86
script_4de0dc:
  tail 0x0, MK4_NATIVE_LK_JUMP_BACK
  .zero 4

MK4_SCRIPT_REPTILE_LK_THROW_STARTUP:
  native_release 0x12
script_4de0ec:
  sound_indexed 0x1b
script_4de0f0:
  open_peer_clip 0x0, word(data_4fb460)
script_4de0f8:
  open_clip 0x0, word(data_4faefc)
script_4de100:
  playback_single 0x55
script_4de104:
  disarm_peer 0x0
script_4de108:
  spawn_effect 0x0
script_4de10c:
  sound_a 0x24
script_4de110:
  sound_roster 0x1
script_4de114:
  damage 0x0, 0x35c2
script_4de11c:
  playback_single 0x69
script_4de120:
  native_release 0x13
script_4de124:
  playback_single 0x7b
script_4de128:
  native_step 0x85
script_4de12c:
  tail 0x0, MK4_NATIVE_LK_PAUSE_LONG
  .zero 4

MK4_SCRIPT_REIKO_LK_THROW_STARTUP:
  open_peer_clip 0x0, word(data_4fb46c)
script_4de140:
  open_clip 0x0, word(data_4faf5c)
script_4de148:
  playback_single 0x26
script_4de14c:
  disarm_peer 0x0
script_4de150:
  spawn_effect 0x0
script_4de154:
  sound_a 0x24
script_4de158:
  sound_roster 0x1
script_4de15c:
  damage 0x0, 0x35c2
script_4de164:
  tail 0x0, MK4_NATIVE_LK_THROW_ROCK_REPEAT_ONE
  .zero 4

MK4_SCRIPT_LK_THROW_ROCK_REPEAT_ONE:
  playback_single 0x2b
script_4de174:
  playback_pair_reverse 0x2a
script_4de178:
  tail 0x0, MK4_NATIVE_LK_THROW_ROCK_REPEAT_TWO
MK4_SCRIPT_LK_THROW_ROCK_REPEAT_TWO:
  playback_single 0x2b
script_4de184:
  playback_pair_reverse 0x2a
script_4de188:
  tail 0x0, MK4_NATIVE_LK_THROW_ROCK_RELEASE
MK4_SCRIPT_LK_THROW_ROCK_RELEASE:
  playback_single 0x2b
script_4de194:
  playback_pair_reverse 0x2a
script_4de198:
  playback_single 0x2b
script_4de19c:
  playback_pair_reverse 0x2a
script_4de1a0:
  playback_single 0x3b
script_4de1a4:
  native_step 0x84
script_4de1a8:
  forward 0x0
script_4de1ac:
  ready 0x0
MK4_SCRIPT_QUAN_CHI_LK_THROW_STARTUP:
  native_release 0x10
script_4de1b4:
  open_peer_clip 0x0, word(data_4fb4fc)
script_4de1bc:
  open_clip 0x0, word(data_4faf44)
script_4de1c4:
  playback_single 0x28
script_4de1c8:
  disarm_peer 0x0
script_4de1cc:
  spawn_effect 0x0
script_4de1d0:
  sound_a 0x24
script_4de1d4:
  sound_roster 0x8
script_4de1d8:
  damage 0x0, 0x35c2
script_4de1e0:
  playback_single 0x63
script_4de1e4:
  native_release 0x11
script_4de1e8:
  native_step 0x83
script_4de1ec:
  native_word 0x0, 0x8000
script_4de1f4:
  motion 0x0, 0xfffff334, 0xffffe148, 0x28f
script_4de204:
  tail 0x0, MK4_NATIVE_LK_VICTIM_RETURN
  .zero 4

MK4_SCRIPT_RAIDEN_LK_THROW_STARTUP:
  sound_indexed 0x19
script_4de21c:
  native_release 0xe
script_4de220:
  open_peer_clip 0x0, word(data_4fb478)
script_4de228:
  open_clip 0x0, word(data_4faf08)
script_4de230:
  playback_single 0x29
script_4de234:
  disarm_peer 0x0
script_4de238:
  spawn_effect 0x0
script_4de23c:
  sound_c 0x3a
script_4de240:
  sound_roster 0x1
script_4de244:
  damage 0x0, 0x35c2
script_4de24c:
  native_step 0x6f
script_4de250:
  forward 0x0
script_4de254:
  ready 0x0
MK4_SCRIPT_KAI_LK_THROW_STARTUP:
  native_release 0xd
script_4de25c:
  open_peer_clip 0x0, word(data_4fb4f0)
script_4de264:
  open_clip 0x0, word(data_4faf14)
script_4de26c:
  playback_single 0x48
script_4de270:
  disarm_peer 0x0
script_4de274:
  spawn_effect 0x0
script_4de278:
  sound_a 0x24
script_4de27c:
  sound_roster 0x8
script_4de280:
  damage 0x0, 0x3333
script_4de288:
  native_step 0x6e
script_4de28c:
  native_word 0x0, 0x1999
script_4de294:
  forward 0x0
script_4de298:
  tail 0x0, MK4_NATIVE_LK_PAUSE_SHORT
MK4_SCRIPT_FUJIN_LK_THROW_STARTUP:
  native_release 0x7
script_4de2a4:
  open_peer_clip 0x0, word(data_4fb4c0)
script_4de2ac:
  open_clip 0x0, word(data_4faf2c)
script_4de2b4:
  playback_single 0x31
script_4de2b8:
  disarm_peer 0x0
script_4de2bc:
  spawn_effect 0x0
script_4de2c0:
  sound_a 0x24
script_4de2c4:
  sound_roster 0x8
script_4de2c8:
  damage 0x0, 0x2e14
script_4de2d0:
  playback_single 0x2e
script_4de2d4:
  native_release 0x8
script_4de2d8:
  playback_single 0x56
script_4de2dc:
  clear_handler 0x0
script_4de2e0:
  native_step 0x5b
script_4de2e4:
  forward_to 0x59
script_4de2e8:
  wait 0xc
script_4de2ec:
  forward 0x0
script_4de2f0:
  ready 0x0
  .zero 4

MK4_SCRIPT_SCORPION_LK_THROW_STARTUP:
  native_release 0x4
script_4de2fc:
  open_peer_clip 0x0, word(data_4fb4d8)
script_4de304:
  open_clip 0x0, word(data_4faf38)
script_4de30c:
  playback_single 0x12
script_4de310:
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_PAIRED_CONTINUATION
MK4_SCRIPT_CAGE_LK_THROW_DAMAGE:
  playback_single 0x2a
script_4de31c:
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_RELEASE
  .zero 4

MK4_SCRIPT_CAGE_LK_THROW_RELEASE:
  damage 0x0, 0x2e14
script_4de330:
  disarm_peer 0x0
script_4de334:
  sound_a 0x24
script_4de338:
  sound_roster 0x8
script_4de33c:
  spawn_effect 0x0
script_4de340:
  playback_single 0x41
script_4de344:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4de34c:
  native_step 0x4d
script_4de350:
  tail 0x0, MK4_NATIVE_MMOVE_LK_OWNER_READY
  .zero 4

MK4_SCRIPT_SONYA_LK_THROW_STARTUP:
  sound_indexed 0x13
script_4de364:
  open_peer_clip 0x0, word(data_4fb520)
script_4de36c:
  open_clip 0x0, word(data_4faf80)
script_4de374:
  playback_single 0x6
script_4de378:
  playback_single 0x18
script_4de37c:
  native_release 0x3
script_4de380:
  playback_single 0x2c
script_4de384:
  damage 0x0, 0x2e14
script_4de38c:
  spawn_effect 0x0
script_4de390:
  disarm_peer 0x0
script_4de394:
  sound_a 0x24
script_4de398:
  sound_roster 0x8
script_4de39c:
  playback_single 0x4b
script_4de3a0:
  native_step 0x4e
script_4de3a4:
  tail 0x0, MK4_NATIVE_MMOVE_LK_BLEND_RETURN
  .zero 4

MK4_SCRIPT_LIU_KANG_LK_THROW_STARTUP:
  open_peer_clip 0x0, word(data_4fb3b8)
script_4de3b8:
  open_clip 0x0, word(data_4faf20)
script_4de3c0:
  play_pair 0xa
script_4de3c4:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_CONTINUE
  .zero 4

MK4_SCRIPT_CAGE_LK_THROW_STARTUP:
  native_release 0x27
script_4de3d4:
  sound_indexed 0x38
script_4de3d8:
  open_peer_clip 0x0, word(data_4fc690)
script_4de3e0:
  open_clip 0x0, word(data_4fc684)
script_4de3e8:
  playback_single 0x17
script_4de3ec:
  damage 0x0, 0x1999
script_4de3f4:
  native_step 0xae
script_4de3f8:
  forward_count 0x3
script_4de3fc:
  wait 0x10
script_4de400:
  forward 0x0
script_4de404:
  tail 0x0, MK4_NATIVE_CAGE_LK_THROW_OWNER_RETURN
  .zero 4

MK4_SCRIPT_LIU_KANG_LK_CONTINUE:
  native_release 0x1
script_4de414:
  play_pair 0x14
script_4de418:
  forward_to 0x26
script_4de41c:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_DAMAGE
  .zero 4

MK4_SCRIPT_LIU_KANG_LK_THROW_DAMAGE_RELEASE:
  disarm_peer 0x0
script_4de42c:
  sound_a 0x24
script_4de430:
  sound_roster 0x8
script_4de434:
  play_pair 0x2
script_4de438:
  native_return 0x50
script_4de43c:
  forward_to 0x2b
script_4de440:
  playback_pair 0x27
script_4de444:
  forward_to 0x2b
script_4de448:
  playback_pair 0x27
script_4de44c:
  forward_to 0x2b
script_4de450:
  playback_pair 0x27
script_4de454:
  forward_to 0x2b
script_4de458:
  playback_pair 0x27
script_4de45c:
  playback_pair 0x0
script_4de460:
  ready 0x0
  .zero 4

MK4_SCRIPT_LIU_KANG_LK_THROW_RETREAT_ENTRY:
  speed_negative 0x0, 0x11eb
script_4de470:
  tail 0x0, MK4_NATIVE_LIU_KANG_LK_THROW_RETREAT
MK4_SCRIPT_LIU_KANG_LK_THROW_RETREAT:
  open_clip 0x0, word(data_4fb604)
script_4de480:
  double_to 0x1c
script_4de484:
  landing 0x0
script_4de488:
  native_sound 0x0
script_4de48c:
  ready 0x0
MK4_SCRIPT_SUB_ZERO_LK_THROW_STARTUP:
  open_peer_clip 0x0, word(data_4fb538)
script_4de498:
  open_clip 0x0, word(data_4faf68)
script_4de4a0:
  forward_count 0xa
script_4de4a4:
  native_release 0x2
script_4de4a8:
  sound_a 0x3
script_4de4ac:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_REBASE
  .zero 4

MK4_SCRIPT_SUB_ZERO_LK_REBASE:
  play_pair 0x1d
script_4de4bc:
  disarm_peer 0x0
script_4de4c0:
  sound_a 0x24
script_4de4c4:
  play_pair 0x5
script_4de4c8:
  sound_roster 0x8
script_4de4cc:
  play_pair 0x8
script_4de4d0:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4de4d8:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_DAMAGE
MK4_SCRIPT_SUB_ZERO_LK_DAMAGE:
  play_pair 0x5
script_4de4e4:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_OWNER_RETURN
  .zero 4

MK4_SCRIPT_JAX_LP_THROW_FIRST_SLAM:
  open_peer_clip 0x0, word(data_4fb448)
script_4de4f8:
  open_clip 0x0, word(data_4fb160)
script_4de500:
  playback_single 0x37
script_4de504:
  spawn_effect 0x0
script_4de508:
  call 0x0, MK4_NATIVE_THROW_GRUNT
script_4de510:
  tail 0x0, MK4_NATIVE_JAX_LP_SLAM_FIRST
SLAM_FIRST_SCRIPT:
  playback_single 0x52
script_4de51c:
  damage 0x0, 0xccc
script_4de524:
  spawn_effect 0x0
script_4de528:
  call 0x0, MK4_NATIVE_THROW_GRUNT
script_4de530:
  tail 0x0, MK4_NATIVE_JAX_LP_SLAM_SECOND
SLAM_SECOND_SCRIPT:
  playback_single 0x74
script_4de53c:
  spawn_effect 0x0
script_4de540:
  damage 0x0, 0xccc
script_4de548:
  call 0x0, MK4_NATIVE_THROW_GRUNT
script_4de550:
  tail 0x0, MK4_NATIVE_JAX_LP_SLAM_THIRD
SLAM_SECOND_RELEASE_SCRIPT:
  native_return 0x7d
script_4de55c:
  jump 0x0, word(script_4de56c)
  .zero 4

SLAM_FIRST_RELEASE_SCRIPT:
  native_return 0x64
script_4de56c:
  native_word 0x0, 0xf5c
script_4de574:
  damage 0x0, 0xccc
script_4de57c:
  tail 0x0, MK4_NATIVE_JAX_LP_THROW_RETURN
  .zero 4

script_4de608:
  tail 0x0, MK4_NATIVE_LP_SELECT_SHORT
MK4_DATA_LP_SELECT_SHORT:
  native_release 0xb
script_4de614:
  open_peer_clip 0x0, word(data_4fb4e4)
script_4de61c:
  open_clip 0x0, word(data_4fb148)
script_4de624:
  playback_single 0x2e
script_4de628:
  native_return 0x6d
script_4de62c:
  damage 0x0, 0x2e14
script_4de634:
  forward 0x0
script_4de638:
  tail 0x0, MK4_NATIVE_LP_THROW_OWNER_READY
MK4_DATA_LP_SELECT_LONG:
  native_release 0xc
script_4de644:
  open_peer_clip 0x0, word(data_4fb4b4)
script_4de64c:
  open_clip 0x0, word(data_4fb13c)
script_4de654:
  playback_single 0x39
script_4de658:
  native_return 0x6b
script_4de65c:
  tail 0x0, MK4_NATIVE_LP_END
  .zero 4

MK4_SCRIPT_LP_END:
  damage 0x0, 0x2e14
script_4de670:
  forward 0x0
script_4de674:
  ready 0x0
script_4de678:
  native_release 0x24
script_4de67c:
  open_peer_clip 0x0, word(data_4fb490)
script_4de684:
  open_clip 0x0, word(data_4fb130)
script_4de68c:
  playback_single 0x2d
script_4de690:
  native_return 0x68
script_4de694:
  native_word 0x0, 0xccc
script_4de69c:
  damage 0x0, 0x2666
script_4de6a4:
  forward 0x0
script_4de6a8:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4

MK4_DATA_LP_SELECT:
  native_release 0x16
script_4de6bc:
  open_peer_clip 0x0, word(data_4fb4a8)
script_4de6c4:
  open_clip 0x0, word(data_4fb10c)
script_4de6cc:
  playback_single 0x19
script_4de6d0:
  native_return 0x66
script_4de6d4:
  native_word 0x0, 0x1eb8
script_4de6dc:
  jump 0x0, word(script_4de898)
MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE:
  native_release 0x16
script_4de6e8:
  open_peer_clip 0x0, word(data_4fb49c)
script_4de6f0:
  open_clip 0x0, word(data_4fb100)
script_4de6f8:
  playback_single 0x2a
script_4de6fc:
  native_return 0x64
script_4de700:
  native_word 0x0, 0x1eb8
script_4de708:
  damage 0x0, 0x2666
script_4de710:
  forward 0x0
script_4de714:
  tail 0x0, MK4_NATIVE_LP_THROW_VICTIM_ADJUST_RETURN
  .zero 4

MK4_SCRIPT_GORO_LK_THROW_STARTUP:
  call 0x0, MK4_NATIVE_THROW_PAIR_START
script_4de728:
  native_release 0x52
script_4de72c:
  open_peer_clip 0x0, word(data_4fca14)
script_4de734:
  open_clip 0x0, word(data_4fca08)
script_4de73c:
  playback_single 0x53
script_4de740:
  spawn_effect 0x0
script_4de744:
  sound_a 0x24
script_4de748:
  sound_roster 0x8
script_4de74c:
  damage 0x0, 0x3333
script_4de754:
  playback_single 0x8c
script_4de758:
  tail 0x0, MK4_NATIVE_LP_ALIGN_AND_RELEASE
script_4de760:
  call 0x0, MK4_NATIVE_THROW_PAIR_START
script_4de768:
  native_release 0x51
script_4de76c:
  open_peer_clip 0x0, word(data_4fc9e4)
script_4de774:
  open_clip 0x0, word(data_4fc9cc)
script_4de77c:
  playback_single 0x2f
script_4de780:
  sound_context 0x1
script_4de784:
  spawn_effect 0x1
script_4de788:
  effect 0x15
script_4de78c:
  effect 0x15
script_4de790:
  tail 0x0, MK4_NATIVE_GORO_LP_THROW_FIVE_PHASE_SEQUENCE
MK4_SCRIPT_LP_THROW_SCRIPT:
  sound_b 0x7
script_4de79c:
  select_roster 0x0, word(MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE), word(script_4de858), word(script_4de860), word(script_4de678), word(MK4_DATA_LP_SELECT), word(script_4de608), word(MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE), word(script_4de858), word(script_4de860), word(script_4de678), word(MK4_DATA_LP_SELECT), word(script_4de608), word(script_4de858), word(script_4de7dc), word(MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE), word(script_4de760)
script_4de7e0:
  tail 0x0, MK4_NATIVE_JAX_LP_THROW_THROW_CONTINUATION
MK4_SCRIPT_GORO_LP_THROW_REPEATED_DAMAGE:
  open_peer_clip 0x0, word(data_4fc9f0)
script_4de7f0:
  open_clip 0x0, word(data_4fc9d8)
script_4de7f8:
  playback_single 0xb
script_4de7fc:
  sound_context 0x1
script_4de800:
  spawn_effect 0x1
script_4de804:
  effect 0x15
script_4de808:
  effect 0x15
script_4de80c:
  effect 0x15
script_4de810:
  damage 0x0, 0x7ae
script_4de818:
  playback_single 0x17
script_4de81c:
  sound_context 0x1
script_4de820:
  spawn_effect 0x1
script_4de824:
  effect 0x15
script_4de828:
  effect 0x15
script_4de82c:
  damage 0x0, 0x7ae
script_4de834:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_LP_PHASE_REPEAT
  .zero 4

MK4_SCRIPT_LP_SHARED_RELEASE:
  jump 0x0, word(MK4_SCRIPT_GORO_LP_THROW_FINAL_DAMAGE)
  .zero 4

script_4de858:
  tail 0x0, MK4_NATIVE_LP_SELECT_LONG
script_4de860:
  native_release 0x16
script_4de864:
  open_peer_clip 0x0, word(data_4fb4cc)
script_4de86c:
  open_clip 0x0, word(data_4fb124)
script_4de874:
  playback_single 0x15
script_4de878:
  native_return 0x67
script_4de87c:
  damage 0x0, 0x2666
script_4de884:
  forward_count 0x2
script_4de888:
  wait 0xa
script_4de88c:
  forward 0x0
script_4de890:
  ready 0x0
  .zero 4

script_4de898:
  damage 0x0, 0x2666
script_4de8a0:
  forward 0x0
script_4de8a4:
  tail 0x0, MK4_NATIVE_SUB_ZERO_LP_THROW_OWNER_RETURN
  .zero 4

MK4_SCRIPT_GORO_SPECIAL_PUNCH:
  prefixed_to 0x20e, word(data_4fc990)
script_4dfb68:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_PUNCH_TAIL
MK4_SCRIPT_GORO_SPECIAL_KICK:
  prefixed_to 0x208, word(data_4fc984)
script_4dfb78:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_KICK_TAIL
  .zero 4

MK4_SCRIPT_GORO_LEAP_STOMP:
  open_clip 0x0, word(data_4fc9a8)
script_4dfbc0:
  double_to 0x17
script_4dfbc4:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_LEAP_CONTINUE
  .zero 4

MK4_SCRIPT_GORO_LEAP_STOMP_STOMP_HIT:
  handler 0x0, MK4_NATIVE_GORO_STOMP_HOLD_SEPARATION
script_4dfbd8:
  prefixed_sample 0x2e
script_4dfbdc:
  native_context 0x0
script_4dfbe0:
  forward 0x0
script_4dfbe4:
  prefixed_to 0x219, word(data_4fc9b4)
script_4dfbec:
  sound_special 0x0
script_4dfbf0:
  forward 0x0
script_4dfbf4:
  prefixed_to 0x19
script_4dfbf8:
  sound_special 0x0
script_4dfbfc:
  forward 0x0
script_4dfc00:
  prefixed_to 0x8
script_4dfc04:
  motion 0x0, 0xffffe148, 0xffffe148, 0x28f
script_4dfc14:
  forward_to 0x19
script_4dfc18:
  native_context 0x0
script_4dfc1c:
  call 0x0, MK4_NATIVE_DISTURB_RESTING_WEAPONS
script_4dfc24:
  sound_special 0x0
script_4dfc28:
  landing 0x0
script_4dfc2c:
  forward 0x0
script_4dfc30:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_STOMP_RETURN
MK4_SCRIPT_GORO_LEAP_STOMP_MISS_LANDING:
  sound_special 0x0
script_4dfc3c:
  forward 0x0
script_4dfc40:
  prefixed_finish 0x200, word(data_4fc9c0)
script_4dfc48:
  ready 0x0
  .zero 4

MK4_SCRIPT_QUAKE_JUMP:
  prefixed_to 0x215, word(data_4fc9b4)
script_4dfc58:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_QUAKE_LAND
MK4_SCRIPT_GORO_QUAKE_LAND:
  sound_special 0x0
script_4dfc64:
  forward 0x0
script_4dfc68:
  ready 0x0
  .zero 4

MK4_SCRIPT_FUJIN_SECOND_FATALITY:
  wait 0x3c
script_4dfed4:
  sound_indexed 0x3c
script_4dfed8:
  native_release 0x37
script_4dfedc:
  prefixed_finish 0x200, word(data_4fc720)
script_4dfee4:
  bind_cursor 0x0, word(data_4fc72c)
script_4dfeec:
  wait 0x8
script_4dfef0:
  scene_active 0x0
script_4dfef4:
  spawn 0x0, 0x26, MK4_NATIVE_FUJIN_SECOND_BLOOD
script_4dff00:
  native_return 0xb5
script_4dff04:
  park 0x0
MK4_SCRIPT_REPTILE_FATALITY_2:
  native_release 0x35
script_4dff0c:
  spawn 0x0, 0x26, MK4_NATIVE_SCENE_HOVER
script_4dff18:
  bind_cursor 0x0, word(data_4fc498)
script_4dff20:
  wait 0x96
script_4dff24:
  scene_active 0x0
script_4dff28:
  native_return 0xb4
script_4dff2c:
  spawn 0x0, 0x0, MK4_NATIVE_REPTILE_SECOND_ACID
script_4dff38:
  sound_indexed 0x3d
script_4dff3c:
  prefixed_finish 0x200, word(data_4fc6fc)
script_4dff44:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff4c:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff54:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff5c:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff64:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff6c:
  prefixed_finish 0x200, word(data_4fc708)
script_4dff74:
  clip_operation 0x0, word(data_4fc6fc)
script_4dff7c:
  bind_cursor 0x0, word(data_4fc498)
script_4dff84:
  park 0x0
MK4_SCRIPT_KAI_FATALITY_2:
  bind_cursor 0x0, word(data_4fc498)
script_4dff90:
  scene_event 0x2d
script_4dff94:
  wait 0x28
script_4dff98:
  spawn 0x0, 0x26, MK4_NATIVE_SCENE_HOVER
script_4dffa4:
  native_release 0x2e
script_4dffa8:
  wait 0x3c
script_4dffac:
  sound_indexed 0x3a
script_4dffb0:
  scene_active 0x0
script_4dffb4:
  wait 0x1e
script_4dffb8:
  call 0x0, MK4_NATIVE_KAI_SECOND_CHILD_WALK
script_4dffc0:
  prefixed_finish 0x200, word(data_4fc444)
script_4dffc8:
  sound_c 0x33
script_4dffcc:
  spawn 0x0, 0x26, MK4_NATIVE_KAI_SECOND_SKULL
script_4dffd8:
  bind_cursor 0x0, word(data_4fc450)
script_4dffe0:
  park 0x0
  .zero 4

MK4_SCRIPT_LIU_KANG_START_FATALITY:
  scene_event 0x2a
script_4dfffc:
  wait 0xf
script_4e0000:
  scene_event 0x2b
script_4e0004:
  wait 0x14
script_4e0008:
  scene_event 0x2c
script_4e000c:
  wait 0x3c
script_4e0010:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_SETUP
  .zero 4

MK4_SCRIPT_LIU_KANG_FIREBALL_FATALITY_PAIRED_OPENING:
  call 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_FATAL_ALIGN
script_4e0028:
  sound_b 0xa
script_4e002c:
  open_peer_clip 0x0, word(data_4fc6c0)
script_4e0034:
  open_clip 0x0, word(data_4fc6a8)
script_4e003c:
  play_pair 0x25
script_4e0040:
  scene_active 0x0
script_4e0044:
  native_return 0xb0
script_4e0048:
  forward 0x0
script_4e004c:
  wait 0x12
script_4e0050:
  sound_a 0x21
script_4e0054:
  sound_indexed 0x39
script_4e0058:
  prefixed_finish 0x200, word(data_4fc6b4)
script_4e0060:
  spawn 0x0, 0x26, MK4_NATIVE_LIU_FINISHER_PROJECTILE
script_4e006c:
  playback_pair 0x0
script_4e0070:
  park 0x0
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_2:
  scene_event 0x30
script_4e008c:
  wait 0xf
script_4e0090:
  scene_event 0x31
script_4e0094:
  wait 0x14
script_4e0098:
  scene_event 0x32
script_4e009c:
  wait 0x14
script_4e00a0:
  native_release 0x33
script_4e00a4:
  wait 0x14
script_4e00a8:
  native_return 0xb3
script_4e00ac:
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_VANISH
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_2_FIRST_HAND:
  prefixed_to 0x278, word(data_4fc6d8)
script_4e00c0:
  wait 0x78
script_4e00c4:
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
script_4e00cc:
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_7_PIECES
script_4e00d4:
  forward 0x0
script_4e00d8:
  park 0x0
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_2_SECOND_HAND:
  prefixed_to 0x278, word(data_4fc6d8)
script_4e00e8:
  spawn_effect 0x6
script_4e00ec:
  sound_c 0x69
script_4e00f0:
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_CLAP
MK4_SCRIPT_SHINNOK_SECOND_CLAP:
  wait 0x78
script_4e00fc:
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_19_PIECES
script_4e0104:
  call 0x0, MK4_NATIVE_SHINNOK_SECOND_GORE_8_PIECES
script_4e010c:
  forward 0x0
script_4e0110:
  park 0x0
  .zero 4

MK4_SCRIPT_JAX_FAN_FATALITY_PERFORMER:
  prefixed_to 0x245, word(MK4_ANIMATION_DESCRIPTOR_CLIP_415)
script_4e0120:
  tail 0x0, MK4_NATIVE_FAN_TAIL
MK4_SCRIPT_JAREK_FAN_FATALITY_MALE_VICTIM_OWNER_CYCLE:
  forward_to 0x8
script_4e012c:
  cursor_set 0x0
script_4e0130:
  tail 0x0, MK4_NATIVE_FAN_MALE_VICTIM_OWNER_CYCLE
MK4_SCRIPT_JAX_FAN_FATALITY_OWNER_CYCLE:
  forward_to 0x4d
script_4e013c:
  cursor_set 0x45
script_4e0140:
  tail 0x0, MK4_NATIVE_FAN_FEMALE_VICTIM_OWNER_CYCLE
MK4_SCRIPT_JAX_FAN_FATALITY_OWNER_RELEASE:
  cursor_set 0x4e
script_4e014c:
  forward_to 0x6b
script_4e0150:
  tail 0x0, MK4_NATIVE_FAN_VICTORY
MK4_SCRIPT_FAN_VICTORY:
  forward 0x0
script_4e015c:
  prefixed_finish 0x200, word(data_4fc0cc)
script_4e0164:
  cursor_set 0x12
script_4e0168:
  forward 0x0
script_4e016c:
  jump 0x0, word(script_4e0164)
  .zero 4

MK4_SCRIPT_JAX_FAN_FATALITY_VICTIM:
  prefixed_to 0x245, word(data_4fc0fc)
script_4e0190:
  forward_to 0x4d
script_4e0194:
  cursor_set 0x45
script_4e0198:
  jump 0x0, word(script_4e0190)
MK4_SCRIPT_JAX_FAN_FATALITY_VICTIM_RELEASE:
  cursor_set 0x4e
script_4e01a4:
  forward_to 0x6b
script_4e01a8:
  tail 0x0, MK4_NATIVE_FAN_VICTIM_END
  .zero 4

MK4_SCRIPT_SONYA_FIRST_FATALITY:
  prefixed_to 0x22a, word(data_4fbfac)
script_4e01c8:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_FIRST_RELEASE
MK4_SCRIPT_SONYA_FATALITY_1_OWNER_LOOP:
  forward 0x0
script_4e01d4:
  cursor_set 0x54
script_4e01d8:
  jump 0x0, word(MK4_SCRIPT_SONYA_FATALITY_1_OWNER_LOOP)
  .zero 4

MK4_SCRIPT_RAIDEN_FIRST_FATALITY:
  prefixed_finish 0x200, word(data_4fbfdc)
script_4e0218:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_RELEASE
MK4_SCRIPT_RAIDEN_FATALITY_1_OWNER_RELEASE:
  playback_pair 0x14
script_4e0224:
  forward 0x0
script_4e0228:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_FIRST_END
  .zero 4

MK4_SCRIPT_BURN_START:
  prefixed_to 0x22b, word(data_4fbfc4)
script_4e0280:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_BEGIN
MK4_SCRIPT_SCORPION_BURN_BEGIN:
  forward_to 0x69
script_4e028c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_START
  .zero 4

MK4_SCRIPT_SCORPION_FATALITY_1_OWNER_FLAME:
  forward_to 0x7b
script_4e029c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT1
  .zero 4

MK4_SCRIPT_SCORPION_BURN_REPEAT1:
  cursor_set 0x69
script_4e02ac:
  forward_to 0x7b
script_4e02b0:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_REPEAT2
MK4_SCRIPT_SCORPION_BURN_REPEAT2:
  cursor_set 0x69
script_4e02bc:
  forward_to 0x7b
script_4e02c0:
  forward 0x0
script_4e02c4:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTORY
  .zero 4

MK4_SCRIPT_SCORPION_BURN_VICTORY:
  prefixed_finish 0x200, word(data_4fb2e0)
script_4e02d8:
  prefixed_finish 0x200, word(data_4fb2ec)
script_4e02e0:
  prefixed_finish 0x200, word(data_4fb2f8)
script_4e02e8:
  bind_cursor 0x200, word(data_4fb304)
script_4e02f0:
  park 0x0
  .zero 4

MK4_SCRIPT_SCORPION_FATALITY_1_VICTIM:
  prefixed_to 0x20c, word(data_4fbfd0)
script_4e0308:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_CUE
MK4_SCRIPT_SCORPION_BURN_VICTIM_CUE:
  forward_to 0xe
script_4e0314:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_REPEAT
  .zero 4

MK4_SCRIPT_SCORPION_BURN_VICTIM_REPEAT:
  forward 0x0
script_4e0324:
  cursor_set 0x3f
script_4e0328:
  forward 0x0
script_4e032c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_BLEND
  .zero 4

MK4_SCRIPT_SCORPION_BURN_VICTIM_BLEND:
  prefixed_to 0x213, word(data_4fb724)
script_4e0340:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_IMPACT
MK4_SCRIPT_SCORPION_BURN_VICTIM_IMPACT:
  forward_to 0x59
script_4e034c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_SETTLE
  .zero 4

MK4_SCRIPT_SCORPION_BURN_VICTIM_SETTLE:
  forward 0x0
script_4e035c:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_BURN_VICTIM_END
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_SETUP:
  wait 0x32
script_4e036c:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_BEGIN
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_FIRST_PULL:
  prefixed_to 0x211, word(data_4fbf88)
script_4e0380:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_PULL
MK4_SCRIPT_SUB_ZERO_FATALITY_1_PULL_FORWARD:
  forward_to 0x2a
script_4e038c:
  cursor_set 0x29
script_4e0390:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES1
MK4_SCRIPT_SUB_ZERO_FATALITY_1_PULL_REVERSE:
  playback_pair 0x1f
script_4e039c:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES2
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_SECOND_PULL:
  forward_to 0x2a
script_4e03ac:
  cursor_set 0x29
script_4e03b0:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES3
MK4_SCRIPT_SUB_ZERO_FATALITY_1_SECOND_REVERSE:
  playback_pair 0x1f
script_4e03bc:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_CUES4
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_HEAD_RELEASE:
  forward_to 0x2c
script_4e03cc:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_HEAD
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_OWNER_LOOP:
  forward_to 0x3c
script_4e03dc:
  forward 0x0
script_4e03e0:
  cursor_set 0x60
script_4e03e4:
  jump 0x0, word(script_4e03dc)
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_VICTIM:
  prefixed_to 0x20f, word(data_4fbfa0)
script_4e03f8:
  cursor_set 0xe
script_4e03fc:
  playback_pair 0x4
script_4e0400:
  forward_to 0xf
script_4e0404:
  cursor_set 0xe
script_4e0408:
  playback_pair 0x4
script_4e040c:
  forward_to 0x11
script_4e0410:
  forward_to 0x30
script_4e0414:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_BLENDS
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_VICTIM_FINISH:
  forward_to 0x3f
script_4e0424:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_COLLAPSE
  .zero 4

MK4_SCRIPT_SUB_ZERO_FATALITY_1_VICTIM_CLEANUP:
  forward 0x0
script_4e0434:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_FIRST_VICTIM_END
  .zero 4

MK4_SCRIPT_JAREK_FATALITY_1_FEMALE_VICTIM_OWNER:
  prefixed_to 0x22b, word(data_4fc060)
script_4e1228:
  jump 0x0, word(script_4e1238)
MK4_SCRIPT_JAREK_FATALITY_1_MALE_VICTIM_OWNER:
  prefixed_to 0x22b, word(data_4fc054)
script_4e1238:
  tail 0x0, MK4_NATIVE_JAREK_FIRST_HEART
  .zero 4

MK4_SCRIPT_JAREK_FIRST_HEART:
  forward 0x0
script_4e124c:
  bind_cursor 0x200, word(data_4fc078)
script_4e1254:
  park 0x0
  .zero 4

MK4_SCRIPT_VICTIM_REACH:
  prefixed_to 0x22b, word(data_4fc06c)
script_4e1268:
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_RIP
MK4_SCRIPT_JAREK_FIRST_VICTIM_RIP:
  forward_to 0x93
script_4e1274:
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_IMPACT
  .zero 4

MK4_SCRIPT_JAREK_FIRST_VICTIM_IMPACT:
  forward_to 0xc2
script_4e1284:
  tail 0x0, MK4_NATIVE_JAREK_FIRST_VICTIM_END
  .zero 4

MK4_SCRIPT_JAREK_FIRST_VICTIM_END:
  forward 0x0
script_4e1294:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_CAGE_FATALITY_1_SCENE_SETUP:
  forward 0x0
script_4e12a4:
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_1_SIGNAL
  .zero 4

MK4_SCRIPT_CAGE_FATALITY_1_SCENE_LOOP:
  prefixed_finish 0x200, word(data_4fc240)
script_4e12b8:
  jump 0x0, word(MK4_SCRIPT_CAGE_FATALITY_1_SCENE_LOOP)
MK4_SCRIPT_CAGE_TORSO_RIP_MALE_VICTIM:
  prefixed_to 0x23a, word(data_4fc0b4)
script_4e12c8:
  jump 0x0, word(script_4e12d8)
MK4_SCRIPT_CAGE_TORSO_RIP_FEMALE_VICTIM:
  prefixed_to 0x23a, word(data_4fc0c0)
script_4e12d8:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_59
MK4_SCRIPT_CAGE_FIRST_VICTIM_59:
  forward_to 0x82
script_4e12e4:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_131
  .zero 4

MK4_SCRIPT_CAGE_FIRST_VICTIM_131:
  forward_to 0x93
script_4e12f4:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_148
  .zero 4

MK4_SCRIPT_CAGE_FIRST_VICTIM_148:
  forward_to 0xa5
script_4e1304:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_166
  .zero 4

MK4_SCRIPT_CAGE_FIRST_VICTIM_166:
  forward_to 0xb8
script_4e1314:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_185
  .zero 4

MK4_SCRIPT_CAGE_FIRST_VICTIM_185:
  forward_to 0xcd
script_4e1324:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_VICTIM_206
  .zero 4

MK4_SCRIPT_CAGE_FIRST_VICTIM_206:
  forward_to 0xe0
script_4e1334:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_SEVER
  .zero 4

MK4_SCRIPT_CAGE_TORSO_RIP_LOWER_FINISH:
  forward 0x0
script_4e1344:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_CAGE_FIRST_SEVER:
  prefixed_to 0x23a, word(data_4fc09c)
script_4e1358:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_HALF
MK4_SCRIPT_CAGE_FIRST_TORSO_HALF:
  forward_to 0x7a
script_4e1364:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_123
  .zero 4

MK4_SCRIPT_CAGE_FIRST_TORSO_123:
  forward_to 0x8a
script_4e1374:
  tail 0x0, MK4_NATIVE_CAGE_FIRST_TORSO_FINISH
  .zero 4

MK4_SCRIPT_CAGE_FIRST_TORSO_FINISH:
  forward 0x0
script_4e1384:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_JAX_FATALITY_1_OWNER_OPENING:
  prefixed_finish 0x200, word(data_4fbf7c)
script_4e1398:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_1_VICTORY
MK4_SCRIPT_JAX_FATALITY_1_OWNER_LOOP:
  prefixed_finish 0x200, word(data_4fc240)
script_4e13a8:
  jump 0x0, word(MK4_SCRIPT_JAX_FATALITY_1_OWNER_LOOP)
MK4_SCRIPT_ARM_TEAR_VICTIM_CLIP_204:
  prefixed_to 0x231, word(data_4fb70c)
script_4e13b8:
  jump 0x0, word(script_4e13c8)
MK4_SCRIPT_ARM_TEAR_VICTIM_CLIP_400:
  prefixed_to 0x231, word(data_4fc03c)
script_4e13c8:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_1_GRAB
MK4_SCRIPT_JAX_FATALITY_1_GRAB:
  forward_to 0x4e
script_4e13d4:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_1_FIRST_ARM
  .zero 4

MK4_SCRIPT_JAX_FATALITY_1_FIRST_ARM:
  forward_to 0x80
script_4e13e4:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_1_SECOND_ARM
  .zero 4

MK4_SCRIPT_JAX_FATALITY_1_SECOND_ARM:
  forward 0x0
script_4e13f4:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_KAI_FATALITY_1_OWNER_OPENING:
  prefixed_to 0x22d, word(data_4fc21c)
script_4e1438:
  tail 0x0, MK4_NATIVE_KAI_FIRST_OWNER_GRAB
MK4_SCRIPT_KAI_FIRST_OWNER_GRAB:
  forward_to 0x7b
script_4e1444:
  tail 0x0, MK4_NATIVE_KAI_FIRST_OWNER_SPIN
  .zero 4

MK4_SCRIPT_KAI_FIRST_OWNER_SPIN:
  prefixed_finish 0x200, word(data_4fc258)
script_4e1458:
  prefixed_finish 0x200, word(data_4fc258)
script_4e1460:
  tail 0x0, MK4_NATIVE_KAI_FIRST_OWNER_TEAR
MK4_SCRIPT_KAI_FATALITY_1_OWNER_VICTORY:
  forward 0x0
script_4e146c:
  prefixed_finish 0x200, word(data_4fc240)
script_4e1474:
  jump 0x0, word(script_4e146c)
  .zero 4

MK4_SCRIPT_KAI_FATALITY_1_VICTIM_OPENING:
  prefixed_to 0x246, word(data_4fc228)
script_4e1488:
  tail 0x0, MK4_NATIVE_KAI_FIRST_VICTIM_71
MK4_SCRIPT_KAI_FIRST_VICTIM_71:
  forward_to 0x7b
script_4e1494:
  prefixed_finish 0x200, word(data_4fc24c)
script_4e149c:
  tail 0x0, MK4_NATIVE_KAI_FIRST_VICTIM_TURN
  .zero 4

MK4_SCRIPT_KAI_FIRST_VICTIM_TURN:
  prefixed_finish 0x200, word(data_4fc24c)
script_4e14b0:
  tail 0x0, MK4_NATIVE_KAI_FIRST_VICTIM_SEVER
MK4_SCRIPT_KAI_FATALITY_1_VICTIM_FINISH:
  forward 0x0
script_4e14bc:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_KAI_FATALITY_1_TORSO_OPENING:
  prefixed_to 0x2ca, word(data_4fc234)
script_4e14d0:
  tail 0x0, MK4_NATIVE_KAI_FIRST_TORSO_203
MK4_SCRIPT_KAI_FIRST_TORSO_203:
  forward_to 0xf2
script_4e14dc:
  tail 0x0, MK4_NATIVE_KAI_FIRST_TORSO_IMPACT
  .zero 4

MK4_SCRIPT_KAI_FIRST_TORSO_IMPACT:
  forward 0x0
script_4e14ec:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_QUAN_CHI_FATALITY_1_OWNER_OPENING:
  prefixed_to 0x228, word(data_4fc12c)
script_4e1500:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_CUE_40
MK4_SCRIPT_QUAN_CHI_FATALITY_CUE_40:
  forward_to 0x75
script_4e150c:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_LEG
  .zero 4

MK4_SCRIPT_QUAN_CHI_FATALITY_LEG:
  forward_to 0xb3
script_4e151c:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_HIT_179
  .zero 4

MK4_SCRIPT_QUAN_CHI_FATALITY_HIT_179:
  forward_to 0xf1
script_4e152c:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_HIT_241
  .zero 4

MK4_SCRIPT_QUAN_CHI_FATALITY_HIT_241:
  forward 0x0
script_4e153c:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_1_OWNER_BEAT
  .zero 4

MK4_SCRIPT_QUAN_CHI_FATALITY_1_OWNER_BEAT:
  prefixed_to 0x212, word(data_4fc138)
script_4e1550:
  tail 0x0, MK4_NATIVE_QUAN_CHI_FATALITY_BEAT
MK4_SCRIPT_QUAN_CHI_FATALITY_BEAT:
  forward 0x0
script_4e155c:
  jump 0x0, word(MK4_SCRIPT_QUAN_CHI_FATALITY_1_OWNER_BEAT)
  .zero 4

MK4_SCRIPT_VICTIM_STREAM:
  prefixed_finish 0x200, word(data_4fc15c)
script_4e1570:
  prefixed_finish 0x200, word(data_4fc108)
script_4e1578:
  jump 0x0, word(script_4e1570)
MK4_SCRIPT_LEG_ENTRY:
  prefixed_finish 0x200, word(data_4fc144)
script_4e1588:
  prefixed_finish 0x200, word(data_4fc150)
script_4e1590:
  jump 0x0, word(script_4e1588)
MK4_SCRIPT_OWNER_KISS_START:
  prefixed_to 0x234, word(data_4fc000)
script_4e15a0:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_53
MK4_SCRIPT_TANYA_FIRST_OWNER_53:
  forward_to 0x68
script_4e15ac:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_DRIFT
  .zero 4

MK4_SCRIPT_TANYA_FIRST_OWNER_DRIFT:
  forward 0x0
script_4e15bc:
  prefixed_finish 0x200, word(data_4fc00c)
script_4e15c4:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_REPLAYS
  .zero 4

MK4_SCRIPT_TANYA_FIRST_OWNER_LOOP:
  prefixed_finish 0x0
script_4e15d4:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_OWNER_LOOP
  .zero 4

MK4_SCRIPT_VICTIM_SCENE:
  prefixed_finish 0x200, word(data_4fc018)
script_4e15e8:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_SWELL
MK4_SCRIPT_TANYA_FATALITY_1_VICTIM_PULSE:
  playback_pair 0xbc
script_4e15f4:
  forward 0x0
script_4e15f8:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_REPEAT
MK4_SCRIPT_TANYA_FATALITY_1_VICTIM_SHAKE:
  prefixed_to 0x209, word(data_4fc0a8)
script_4e1608:
  forward_to 0xb
script_4e160c:
  playback_pair 0x9
script_4e1610:
  forward_to 0xb
script_4e1614:
  playback_pair 0x9
script_4e1618:
  forward_to 0xb
script_4e161c:
  playback_pair 0x9
script_4e1620:
  forward_to 0xb
script_4e1624:
  playback_pair 0x9
script_4e1628:
  forward_to 0x15
script_4e162c:
  forward_to 0x17
script_4e1630:
  playback_pair 0x15
script_4e1634:
  forward_to 0x17
script_4e1638:
  playback_pair 0x15
script_4e163c:
  forward_to 0x17
script_4e1640:
  playback_pair 0x15
script_4e1644:
  forward_to 0x17
script_4e1648:
  playback_pair 0x15
script_4e164c:
  forward_to 0x26
script_4e1650:
  forward_to 0x28
script_4e1654:
  playback_pair 0x26
script_4e1658:
  forward_to 0x28
script_4e165c:
  playback_pair 0x26
script_4e1660:
  forward_to 0x28
script_4e1664:
  playback_pair 0x26
script_4e1668:
  forward_to 0x28
script_4e166c:
  playback_pair 0x26
script_4e1670:
  forward_to 0x37
script_4e1674:
  forward_to 0x39
script_4e1678:
  playback_pair 0x37
script_4e167c:
  forward_to 0x39
script_4e1680:
  playback_pair 0x37
script_4e1684:
  forward_to 0x39
script_4e1688:
  playback_pair 0x37
script_4e168c:
  forward_to 0x39
script_4e1690:
  playback_pair 0x37
script_4e1694:
  forward_to 0x40
script_4e1698:
  forward_to 0x42
script_4e169c:
  playback_pair 0x40
script_4e16a0:
  forward_to 0x42
script_4e16a4:
  playback_pair 0x40
script_4e16a8:
  forward_to 0x42
script_4e16ac:
  playback_pair 0x40
script_4e16b0:
  forward_to 0x48
script_4e16b4:
  forward_to 0x4a
script_4e16b8:
  playback_pair 0x48
script_4e16bc:
  forward_to 0x4a
script_4e16c0:
  playback_pair 0x48
script_4e16c4:
  tail 0x0, MK4_NATIVE_TANYA_FIRST_VICTIM_BURST
  .zero 4

MK4_SCRIPT_TANYA_FIRST_VICTIM_BURST:
  forward 0x0
script_4e16d4:
  playback_pair 0x48
script_4e16d8:
  jump 0x0, word(MK4_SCRIPT_TANYA_FIRST_VICTIM_BURST)
  .zero 4

MK4_SCRIPT_FUJIN_FIRST_FATALITY:
  half_restart 0x200, word(data_4fc024)
script_4e1f60:
  tail 0x0, MK4_NATIVE_FUJIN_FIRST_LOOP_START
WIND_FINISH_CYCLE_SCRIPT:
  prefixed_finish 0x200, word(data_4fc030)
script_4e1f70:
  tail 0x0, MK4_NATIVE_FUJIN_FIRST_CYCLE
  .zero 4

WIND_FINISH_EXIT_SCRIPT:
  prefixed_finish 0x200, word(data_4fb754)
script_4e1f88:
  prefixed_finish 0x200, word(data_4fb76c)
script_4e1f90:
  tail 0x0, MK4_NATIVE_FUJIN_FIRST_SHOOT
MK4_SCRIPT_FUJIN_FATALITY_1_CROSSBOW_SHOT:
  prefixed_finish 0x200, word(data_4fb778)
script_4e1fa0:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
DRAGON_VICTIM_SCRIPT:
  prefixed_finish 0x200, word(DRAGON_VICTIM_ANIMATION_DESCRIPTOR)
script_4e1fb0:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_OWNER_TEAR:
  half_count 0x220, word(data_4fc168)
script_4e2cf8:
  forward_to 0x2b
script_4e2cfc:
  playback_pair 0x29
script_4e2d00:
  forward_to 0x2b
script_4e2d04:
  playback_pair 0x29
script_4e2d08:
  forward 0x0
script_4e2d0c:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_VICTORY
  .zero 4

MK4_SCRIPT_OWNER_VICTORY_LOOP:
  prefixed_finish 0x200, word(data_4fc240)
script_4e2d20:
  jump 0x0, word(MK4_SCRIPT_OWNER_VICTORY_LOOP)
MK4_SCRIPT_VICTIM_PLACE:
  prefixed_to 0x225, word(data_4fc18c)
script_4e2d30:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_TEAR
MK4_SCRIPT_REIKO_FIRST_TEAR:
  cursor_set 0x26
script_4e2d3c:
  forward_to 0x75
script_4e2d40:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_VICTIM_118
MK4_SCRIPT_REIKO_FIRST_VICTIM_118:
  forward_to 0xa5
script_4e2d4c:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_VICTIM_165
  .zero 4

MK4_SCRIPT_REIKO_FIRST_VICTIM_165:
  forward 0x0
script_4e2d5c:
  tail 0x0, MK4_NATIVE_REIKO_LEG_TEAR_VICTIM_FINISH
  .zero 4

MK4_SCRIPT_TORSO_START:
  prefixed_to 0x21d, word(data_4fc198)
script_4e2d70:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_TORSO_29
MK4_SCRIPT_REIKO_FIRST_TORSO_29:
  forward 0x0
script_4e2d7c:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_TORSO_END
  .zero 4

MK4_SCRIPT_HEAD_START:
  prefixed_to 0x229, word(data_4fc1a4)
script_4e2d90:
  cursor_set 0x15
script_4e2d94:
  forward_to 0x29
script_4e2d98:
  cursor_set 0x15
script_4e2d9c:
  forward_to 0x29
script_4e2da0:
  cursor_set 0x15
script_4e2da4:
  forward_to 0x29
script_4e2da8:
  cursor_set 0x15
script_4e2dac:
  forward_to 0x29
script_4e2db0:
  cursor_set 0x15
script_4e2db4:
  forward_to 0x3a
script_4e2db8:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_HEAD_59
MK4_SCRIPT_REIKO_FIRST_HEAD_59:
  forward_to 0x52
script_4e2dc4:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_HEAD_83
  .zero 4

MK4_SCRIPT_REIKO_FIRST_HEAD_83:
  forward 0x0
script_4e2dd4:
  tail 0x0, MK4_NATIVE_REIKO_LEG_TEAR_HEAD_FINISH
  .zero 4

MK4_SCRIPT_LEFT_ARM_START:
  prefixed_to 0x219, word(data_4fc180)
script_4e2de8:
  cursor_set 0x4
script_4e2dec:
  forward_to 0x37
script_4e2df0:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_LEFT_ARM_56
MK4_SCRIPT_REIKO_FIRST_LEFT_ARM_56:
  forward_to 0x42
script_4e2dfc:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_LEFT_ARM_LAND
  .zero 4

MK4_SCRIPT_REIKO_FIRST_LEFT_ARM_LAND:
  forward 0x0
script_4e2e0c:
  tail 0x0, MK4_NATIVE_REIKO_LEG_TEAR_LEFT_ARM_FINISH
  .zero 4

MK4_SCRIPT_RIGHT_ARM_START:
  prefixed_to 0x21d, word(data_4fc174)
script_4e2e20:
  cursor_set 0x4
script_4e2e24:
  forward_to 0x1d
script_4e2e28:
  cursor_set 0x4
script_4e2e2c:
  forward_to 0x1d
script_4e2e30:
  cursor_set 0x4
script_4e2e34:
  forward_to 0x2b
script_4e2e38:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_RIGHT_ARM_44
MK4_SCRIPT_REIKO_FIRST_RIGHT_ARM_44:
  forward_to 0x44
script_4e2e44:
  tail 0x0, MK4_NATIVE_REIKO_FIRST_RIGHT_ARM_LAND
  .zero 4

MK4_SCRIPT_REIKO_FIRST_RIGHT_ARM_LAND:
  forward 0x0
script_4e2e54:
  tail 0x0, MK4_NATIVE_REIKO_LEG_TEAR_RIGHT_ARM_FINISH
  .zero 4

MK4_SCRIPT_OWNER_ENTRY:
  prefixed_to 0x20a, word(data_4fc1bc)
script_4e2e68:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_CYCLE
MK4_SCRIPT_REPTILE_FIRST_OWNER_CYCLE:
  forward_to 0x8a
script_4e2e74:
  cursor_set 0x63
script_4e2e78:
  forward_to 0x8a
script_4e2e7c:
  cursor_set 0x63
script_4e2e80:
  forward_to 0x8a
script_4e2e84:
  cursor_set 0x63
script_4e2e88:
  forward_to 0xa8
script_4e2e8c:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_BITE
  .zero 4

MK4_SCRIPT_REPTILE_FIRST_OWNER_BITE:
  forward 0x0
script_4e2e9c:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_OWNER_LOOP
  .zero 4

MK4_SCRIPT_REPTILE_FIRST_OWNER_LOOP:
  jump 0x0, word(MK4_SCRIPT_REPTILE_FIRST_OWNER_BITE)
MK4_SCRIPT_VICTIM_ENTRY:
  prefixed_to 0x255, word(data_4fc1b0)
script_4e2eb8:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_BITE
MK4_SCRIPT_REPTILE_FIRST_VICTIM_BITE:
  forward_to 0x7c
script_4e2ec4:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_SHAKE
  .zero 4

MK4_SCRIPT_REPTILE_FIRST_VICTIM_SHAKE:
  cursor_set 0x55
script_4e2ed4:
  forward_to 0x7c
script_4e2ed8:
  cursor_set 0x55
script_4e2edc:
  forward_to 0x7c
script_4e2ee0:
  cursor_set 0x55
script_4e2ee4:
  tail 0x0, MK4_NATIVE_REPTILE_FIRST_VICTIM_COLLAPSE
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_1_OWNER_VANISH:
  prefixed_to 0x220, word(data_4fae0c)
script_4e2ef8:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VANISHED
MK4_SCRIPT_SHINNOK_FIRST_VANISHED:
  forward 0x0
script_4e2f04:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_HIDDEN
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_RETURN:
  prefixed_to 0x212, word(data_4fc1ec)
script_4e2f18:
  tail 0x0, MK4_NATIVE_SHINNOK_FATALITY_REAPPEAR
MK4_SCRIPT_SHINNOK_FATALITY_REAPPEAR:
  forward 0x0
script_4e2f24:
  tail 0x0, MK4_NATIVE_SHINNOK_FATALITY_VICTORY
  .zero 4

MK4_SCRIPT_VICTORY_LOOP:
  prefixed_finish 0x200, word(data_4fc240)
script_4e2f38:
  jump 0x0, word(MK4_SCRIPT_VICTORY_LOOP)
MK4_SCRIPT_HAND_ENTRY:
  prefixed_to 0x2a4, word(data_4fc1d4)
script_4e2f48:
  cursor_set 0x9f
script_4e2f4c:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_HAND_SQUEEZE
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_1_HAND_SQUEEZE:
  forward_to 0xa4
script_4e2f5c:
  cursor_set 0x9f
script_4e2f60:
  tail 0x0, MK4_NATIVE_SHINNOK_HAND_SQUEEZE_CYCLE
MK4_SCRIPT_SHINNOK_FATALITY_1_HAND_RELEASE:
  forward 0x0
script_4e2f6c:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_VICTIM_LIFT:
  prefixed_to 0x240, word(data_4fc1e0)
script_4e2f80:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_HELD
MK4_SCRIPT_SHINNOK_FIRST_VICTIM_HELD:
  cursor_set 0x3b
script_4e2f8c:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_SQUEEZE
  .zero 4

MK4_SCRIPT_SHINNOK_FATALITY_1_VICTIM_SQUEEZE:
  forward_to 0x40
script_4e2f9c:
  cursor_set 0x3b
script_4e2fa0:
  tail 0x0, MK4_NATIVE_SHINNOK_VICTIM_SQUEEZE_CYCLE
MK4_SCRIPT_SHINNOK_FATALITY_1_VICTIM_CRUSH:
  forward_to 0x8a
script_4e2fac:
  tail 0x0, MK4_NATIVE_SHINNOK_FIRST_VICTIM_CRUSH
  .zero 4

MK4_SCRIPT_SHINNOK_FIRST_VICTIM_CRUSH:
  forward 0x0
script_4e2fbc:
  tail 0x0, MK4_NATIVE_TASK_RETIRE
  .zero 4

MK4_SCRIPT_SHINNOK_SECOND_VANISH:
  prefixed_to 0x220, word(data_4fae0c)
script_4e2fd0:
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_VANISHED
MK4_SCRIPT_SHINNOK_SECOND_VANISHED:
  forward 0x0
script_4e2fdc:
  tail 0x0, MK4_NATIVE_SHINNOK_SECOND_HANDS
  .zero 4

MK4_SCRIPT_RAIDEN_SECOND_FATALITY:
  native_release 0x4b
script_4e56cc:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_BLEND
  .zero 4

MK4_SCRIPT_RAIDEN_FATALITY_2_FIRST_PLAYBACK:
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_PARTICLES
script_4e56e0:
  playback_pair 0x5e
script_4e56e4:
  forward 0x0
script_4e56e8:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_EFFECT
MK4_SCRIPT_RAIDEN_FATALITY_2_REPEATED_PLAYBACK:
  native_release 0x4c
script_4e56f4:
  playback_pair 0x5e
script_4e56f8:
  forward 0x0
script_4e56fc:
  playback_pair 0x5e
script_4e5700:
  forward 0x0
script_4e5704:
  playback_pair 0x50
script_4e5708:
  scene_active 0x0
script_4e570c:
  blend 0x0, word(data_4fc834)
script_4e5714:
  native_return 0xbb
script_4e5718:
  clip 0x0, word(data_4fc834)
script_4e5720:
  tail 0x0, MK4_NATIVE_RAIDEN_FATALITY_2_HELD_POSE
MK4_SCRIPT_RAIDEN_FATALITY_2_HELD_POSE:
  bind_cursor 0x0, word(data_4fc840)
script_4e5730:
  wait 0xd3
script_4e5734:
  native_release 0x4d
script_4e5738:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_RELEASE
MK4_SCRIPT_RAIDEN_FATALITY_2_ENDING:
  spawn 0x0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
script_4e574c:
  clip 0x0, word(data_4fc84c)
script_4e5754:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_END
  .zero 4

MK4_SCRIPT_RAIDEN_FATALITY_2_OWNER_HOLD:
  clip 0x0, word(data_4fc858)
script_4e5768:
  park 0x0
  .zero 4

MK4_SCRIPT_RAIDEN_FATALITY_2_VICTIM:
  clip 0x0, word(data_4fc864)
script_4e5778:
  bind_cursor 0x0, word(data_4fc870)
script_4e5780:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_SECOND_VICTIM_END
MK4_SCRIPT_RAIDEN_FATALITY_2_VICTIM_HOLD:
  wait 0xd3
script_4e578c:
  clip 0x0, word(data_4fc87c)
script_4e5794:
  park 0x0
MK4_SCRIPT_TANYA_FATALITY_2:
  native_release 0x4e
script_4e579c:
  wait 0x1e
script_4e57a0:
  wait 0x1e
script_4e57a4:
  scene_active 0x0
script_4e57a8:
  sound_indexed 0x12
script_4e57ac:
  native_return 0xba
script_4e57b0:
  clip 0x0, word(data_4fc7ec)
script_4e57b8:
  tail 0x0, MK4_NATIVE_TANYA_SECOND_HEIGHT
MK4_SCRIPT_TANYA_SECOND_HEIGHT:
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
script_4e57cc:
  clip 0x0, word(data_4fc7f8)
script_4e57d4:
  scene_event 0x45
script_4e57d8:
  wait 0xa
script_4e57dc:
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
script_4e57e8:
  clip 0x0, word(data_4fc7f8)
script_4e57f0:
  scene_event 0x46
script_4e57f4:
  wait 0xa
script_4e57f8:
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_OWNER_CUE
script_4e5804:
  clip 0x0, word(data_4fc7f8)
script_4e580c:
  clip 0x0, word(data_4fc804)
script_4e5814:
  tail 0x0, MK4_NATIVE_TANYA_SECOND_FINISH
  .zero 4

MK4_SCRIPT_TANYA_FATALITY_2_VICTIM_SCENE:
  clip 0x0, word(data_4fc810)
script_4e5828:
  clip 0x0, word(data_4fc81c)
script_4e5830:
  wait 0xa
script_4e5834:
  clip 0x0, word(data_4fc81c)
script_4e583c:
  wait 0xa
script_4e5840:
  clip 0x0, word(data_4fc81c)
script_4e5848:
  spawn 0x0, 0x26b, MK4_NATIVE_TANYA_SECOND_VICTIM_CUE
script_4e5854:
  tail 0x0, MK4_NATIVE_TANYA_FATALITY_2_VICTIM_COLLAPSE
  .zero 4

MK4_SCRIPT_TANYA_FATALITY_2_VICTIM_COLLAPSE:
  native_release 0x47
script_4e5864:
  clip 0x0, word(data_4fc828)
script_4e586c:
  park 0x0
MK4_SCRIPT_CAGE_FATALITY_2_SCENE_ENTRY:
  native_release 0x44
script_4e5874:
  wait 0x1e
script_4e5878:
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_FATALITY_2_CONTINUE
MK4_SCRIPT_CAGE_FATALITY_2_UPPERCUT:
  wait 0x5
script_4e5884:
  scene_active 0x0
script_4e5888:
  sound_c 0x2
script_4e588c:
  wait 0x6
script_4e5890:
  spawn 0x0, 0x26b, MK4_NATIVE_CAGE_SECOND_CHARGE
script_4e589c:
  wait 0x6
script_4e58a0:
  play_clip 0x1, word(data_4fbb38)
script_4e58a8:
  half_count 0x4
script_4e58ac:
  spawn_effect 0x6
script_4e58b0:
  sound_context 0x2
script_4e58b4:
  spawn 0x0, 0x26b, MK4_NATIVE_CAGE_SECOND_FLASH
script_4e58c0:
  half_count 0x1
script_4e58c4:
  sound_c 0x5d
script_4e58c8:
  sound_b 0x1
script_4e58cc:
  native_return 0xb9
script_4e58d0:
  tail 0x0, MK4_NATIVE_CAGE_SECOND_REPEATED_UPPERCUTS
MK4_SCRIPT_CAGE_SECOND_REPEATED_UPPERCUTS:
  prefixed_to 0x23c, word(data_4fb2b0)
script_4e58e0:
  native_release 0x50
script_4e58e4:
  forward 0x0
script_4e58e8:
  cursor_set 0x2a
script_4e58ec:
  forward 0x0
script_4e58f0:
  jump 0x0, word(script_4e6ac4)
  .zero 4

MK4_SCRIPT_CAGE_SECOND_VICTIM_HIT:
  clip 0x0, word(data_4fb394)
script_4e5908:
  blend 0x0, word(data_4fb724)
script_4e5910:
  play_clip 0x14, word(data_4fb724)
script_4e5918:
  sound_special 0x0
script_4e591c:
  forward_to 0x5a
script_4e5920:
  sound_special 0x0
script_4e5924:
  tail 0x0, MK4_NATIVE_CAGE_SECOND_VICTIM_FINISH
  .zero 4

MK4_SCRIPT_SONYA_SECOND_FATALITY:
  native_release 0x4f
script_4e5934:
  wait 0x28
script_4e5938:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_OPEN
MK4_SCRIPT_SONYA_FATALITY_2_OPENING:
  prefixed_finish 0x200, word(data_4fc7a4)
script_4e5948:
  native_return 0xb8
script_4e594c:
  scene_active 0x0
script_4e5950:
  clip 0x0, word(data_4fc7b0)
script_4e5958:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_REPEAT
MK4_SCRIPT_SONYA_FATALITY_2_CYCLE:
  spawn 0x0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
script_4e596c:
  play_clip 0x18, word(data_4fc7bc)
script_4e5974:
  wait 0x6
script_4e5978:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_BRANCH
MK4_SCRIPT_SONYA_FATALITY_2_FIRST_CYCLE:
  playback_tail 0x0
script_4e5984:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_PAUSE
  .zero 4

MK4_SCRIPT_SONYA_FATALITY_2_SECOND_CYCLE:
  playback_tail 0x0
script_4e5994:
  wait 0xf
script_4e5998:
  clip 0x0, word(data_4fc7bc)
script_4e59a0:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END
MK4_SCRIPT_SONYA_FATALITY_2_OWNER_ENDING:
  forward 0x0
script_4e59ac:
  prefixed_finish 0x200, word(data_4fb2d4)
script_4e59b4:
  playback_pair 0x45
script_4e59b8:
  forward_to 0x63
script_4e59bc:
  jump 0x0, word(script_4e59b4)
  .zero 4

MK4_SCRIPT_SONYA_FATALITY_2_VICTIM:
  clip 0x0, word(data_4fc7c8)
script_4e59d0:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SECOND_VICTIM_END
  .zero 4

MK4_SCRIPT_SONYA_FATALITY_2_VICTIM_END:
  forward 0x0
script_4e5a24:
  park 0x0
  .zero 4

MK4_SCRIPT_JAX_FATALITY_2:
  native_release 0x43
script_4e5a84:
  bind_cursor 0x0, word(data_4fbacc)
script_4e5a8c:
  wait 0x14
script_4e5a90:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_2_APPROACH
MK4_SCRIPT_JAX_FATALITY_2_APPROACH:
  spawn 0x0, 0x26b, MK4_NATIVE_JAX_FATALITY_2_SPARKS
script_4e5aa4:
  native_release 0x43
script_4e5aa8:
  sound_c 0x46
script_4e5aac:
  wait 0xf
script_4e5ab0:
  scene_active 0x0
script_4e5ab4:
  sound_c 0x46
script_4e5ab8:
  wait 0xf
script_4e5abc:
  sound_c 0x2
script_4e5ac0:
  prefixed_finish 0x200, word(data_4fc780)
script_4e5ac8:
  spawn 0x0, 0x26b, MK4_NATIVE_JAX_FATALITY_2_FLASH
script_4e5ad4:
  spawn_effect 0x6
script_4e5ad8:
  sound_c 0x69
script_4e5adc:
  sound_c 0x2c
script_4e5ae0:
  native_return 0xb7
script_4e5ae4:
  wait 0xf
script_4e5ae8:
  clip 0x0, word(data_4fc78c)
script_4e5af0:
  native_release 0x50
script_4e5af4:
  prefixed_finish 0x200, word(data_4fb2d4)
script_4e5afc:
  playback_pair 0x45
script_4e5b00:
  forward_to 0x63
script_4e5b04:
  jump 0x0, word(script_4e5afc)
  .zero 4

MK4_SCRIPT_JAX_FATALITY_2_HEAD:
  sound_b 0x8
script_4e5b14:
  clip 0x0, word(data_4fc798)
script_4e5b1c:
  spawn 0x0, 0x26b, MK4_NATIVE_SCENE_KEEPALIVE_TASK
script_4e5b28:
  clip 0x0, word(data_4fb724)
script_4e5b30:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_2_FINISH
MK4_SCRIPT_CAGE_FATALITY_2_VICTIM_START:
  forward 0x0
script_4e5b3c:
  tail 0x0, MK4_NATIVE_JAX_FATALITY_2_VICTIM_PAUSE
  .zero 4

FIRE_SCRIPT_STARTUP:
  open_clip 0x0, word(data_4fc27c)
script_4e5b60:
  cursor_set 0x1
script_4e5b64:
  forward_to 0x4
script_4e5b68:
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_DECOR
script_4e5b70:
  forward_to 0x8
script_4e5b74:
  call 0x0, MK4_NATIVE_SCORPION_SCRIPT_FIRE_FLAME
script_4e5b7c:
  forward_to 0xb
script_4e5b80:
  tail 0x0, MK4_NATIVE_FIRE_CONTACT
  .zero 4

FIRE_SCRIPT_RECOVERY:
  forward_to 0x1e
script_4e5b9c:
  tail 0x0, MK4_NATIVE_FIRE_RECOVERY
  .zero 4

MK4_SCRIPT_GROIN_PUNCH_GROIN_RECOVER:
  backward 0x0
script_4e5bac:
  ready 0x0
MK4_SCRIPT_REPTILE_INVISIBILITY:
  play_clip 0x18, word(data_4fae0c)
script_4e5bb8:
  sound_c 0x4b
script_4e5bbc:
  sound_c 0x4a
script_4e5bc0:
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_INVISIBILITY
  .zero 4

MK4_SCRIPT_KAI_SECOND_VICTIM_FALL:
  prefixed_to 0x214, word(data_4fb724)
script_4e5be8:
  sound_special 0x0
script_4e5bec:
  forward_to 0x5a
script_4e5bf0:
  sound_special 0x0
script_4e5bf4:
  forward 0x0
script_4e5bf8:
  park 0x0
  .zero 4

MK4_SCRIPT_FUJIN_WIND_SLAM:
  native_return 0x61
script_4e5c04:
  prefixed_to 0x206, word(data_4fb220)
script_4e5c0c:
  forward 0x0
script_4e5c10:
  damage 0x0, 0x147a
script_4e5c18:
  ready 0x0
  .zero 4

MK4_SCRIPT_FUJIN_WIND_RECOVERY:
  forward 0x0
script_4e5c24:
  playback_pair 0x22
script_4e5c28:
  forward_to 0x28
script_4e5c2c:
  playback_pair 0x22
script_4e5c30:
  forward_to 0x28
script_4e5c34:
  playback_pair 0x1e
script_4e5c38:
  ready 0x0
script_4e5c3c:
  open_clip 0x0, word(data_4fb214)
script_4e5c44:
  double_to 0x10
script_4e5c48:
  jump 0x0, word(script_4e5c64)
MK4_SCRIPT_FUJIN_WIND:
  sound_c 0x3f
script_4e5c54:
  fast_start 0x0, word(script_4e5c3c)
script_4e5c5c:
  prefixed_to 0x210, word(data_4fb214)
script_4e5c64:
  query_d 0x8
script_4e5c68:
  tail 0x0, MK4_NATIVE_FUJIN_WIND_RECOVERY
MK4_SCRIPT_JAX_HIGH_KICK_RECOVERY:
  backward_count 0x2
script_4e5c74:
  release_attack 0x0
script_4e5c78:
  backward 0x0
script_4e5c7c:
  ready 0x0
script_4e5c80:
  attack_selector 0x0, 0x2
script_4e5c88:
  sound_a 0x1
script_4e5c8c:
  prefixed_to 0x20a, word(data_4fb0d0)
script_4e5c94:
  tail 0x0, MK4_NATIVE_KICK_CONTACT_NARROW
  .zero 4

MK4_DATA_HIGH_KICK:
  select_roster 0x0, word(script_4e6c90), word(script_4e5c80), word(script_4e5ce0), word(script_4e6c90), word(script_4e5c80), word(script_4e5ce0), word(script_4e6c90), word(script_4e6c90), word(script_4e5c80), word(script_4e6c90), word(script_4e6c90), word(script_4e5c80), word(script_4e5ce0), word(script_4e6c90), word(script_4e5c80), word(script_4e6c90)
script_4e5ce4:
  attack_selector 0x0, 0x0
script_4e5cec:
  sound_a 0x1
script_4e5cf0:
  prefixed_to 0x20a, word(data_4fb088)
script_4e5cf8:
  tail 0x0, MK4_NATIVE_KICK_CONTACT_DEFAULT
  .zero 4

MK4_SCRIPT_FUJIN_HIGH_KICK_RECOVERY:
  backward_count 0x2
script_4e5d24:
  release_attack 0x0
script_4e5d28:
  tail 0x0, MK4_NATIVE_HIGH_KICK_REVERSE_RETURN
  .zero 4

MK4_DATA_LOW_KICK:
  select_roster 0x0, word(script_4e6d08), word(script_4e6da0), word(script_4e6cb0), word(script_4e6d08), word(script_4e6da0), word(script_4e6cb0), word(script_4e6d08), word(script_4e6d08), word(script_4e6da0), word(script_4e6cb0), word(script_4e6d08), word(script_4e6da0), word(script_4e6cb0), word(script_4e6d08), word(script_4e6da0), word(script_4e6d08)
  .zero 4

MK4_DATA_AIR_THROW_LIFT:
  sound_indexed 0x36
script_4e5ddc:
  open_peer_clip 0x0, word(data_4fb43c)
script_4e5de4:
  open_clip 0x0, word(data_4fb16c)
script_4e5dec:
  playback_single 0x14
script_4e5df0:
  tail 0x0, MK4_NATIVE_AIR_LIFT_FALL
MK4_SCRIPT_AIR_LIFT_FALL:
  playback_single 0x20
script_4e5dfc:
  sound_a 0x24
script_4e5e00:
  sound_roster 0x1
script_4e5e04:
  spawn_effect 0x6
script_4e5e08:
  playback_single 0x2b
script_4e5e0c:
  disarm_peer 0x0
script_4e5e10:
  tail 0x0, MK4_NATIVE_AIR_LIFT_RELEASE
MK4_SCRIPT_AIR_LIFT_RELEASE:
  playback_single 0x30
script_4e5e1c:
  forward 0x0
script_4e5e20:
  ready 0x0
  .zero 4

MK4_DATA_AIR_THROW:
  native_release 0x17
script_4e5e2c:
  sound_indexed 0xd
script_4e5e30:
  open_peer_clip 0x0, word(data_4fb6f4)
script_4e5e38:
  open_clip 0x0, word(data_4fb154)
script_4e5e40:
  playback_single 0x1c
script_4e5e44:
  tail 0x0, MK4_NATIVE_AIR_THROW_RELEASE
  .zero 4

MK4_SCRIPT_AIR_THROW_RELEASE:
  forward_count 0x1
script_4e5e54:
  motion 0x0, 0x7ae, 0xffffe667, 0x28f
script_4e5e64:
  wait 0xa
script_4e5e68:
  forward 0x0
script_4e5e6c:
  native_context 0x0
script_4e5e70:
  tail 0x0, MK4_NATIVE_AIR_THROW_LAND
MK4_DATA_AIR_THROW_SLAM:
  landing 0x0
script_4e5e7c:
  native_release 0x17
script_4e5e80:
  sound_indexed 0x15
script_4e5e84:
  open_peer_clip 0x0, word(data_4fb484)
script_4e5e8c:
  open_clip 0x0, word(data_4fb0f4)
script_4e5e94:
  playback_single 0x12
script_4e5e98:
  tail 0x0, MK4_NATIVE_AIR_SLAM_FALL
MK4_SCRIPT_AIR_SLAM_FALL:
  playback_single 0x1b
script_4e5ea4:
  tail 0x0, MK4_NATIVE_NOOB_SAIBOT_AIR_THROW_LANDING_LINK
  .zero 4

MK4_SCRIPT_QUAN_CHI_AIR_THROW_LANDING:
  native_context 0x0
script_4e5eb4:
  tail 0x0, MK4_NATIVE_AIR_SLAM_END
  .zero 4

MK4_DATA_AIR_THROW_FLIP:
  native_release 0x17
script_4e5ed4:
  sound_indexed 0x10
script_4e5ed8:
  open_peer_clip 0x0, word(data_4fb508)
script_4e5ee0:
  open_clip 0x0, word(data_4fb118)
script_4e5ee8:
  play_pair 0x18
script_4e5eec:
  tail 0x0, MK4_NATIVE_AIR_FLIP_RELEASE
  .zero 4

MK4_SCRIPT_FOLLOW_CROUCH_HK:
  speed 0x0, 0x1999
script_4e5fd8:
  attack_selector 0x0, 0x1a
script_4e5fe0:
  sound_a 0x1
script_4e5fe4:
  play_clip 0x7, word(data_4fafec)
script_4e5fec:
  query_b 0x1a
script_4e5ff0:
  action_recovery 0x0
script_4e5ff4:
  landing 0x0
script_4e5ff8:
  forward 0x0
script_4e5ffc:
  loop 0x5
script_4e6000:
  playback_tail 0xa
script_4e6004:
  wait 0x2
script_4e6008:
  playback_tail 0x9
script_4e600c:
  wait 0x2
script_4e6010:
  repeat 0x0
script_4e6014:
  clip_operation 0x0, word(data_4fafec)
script_4e601c:
  return 0x0
MK4_SCRIPT_CROUCH_HIGH_KICK:
  select_roster 0x0, word(script_4e60b8), word(script_4e5fd8), word(script_4e6060), word(script_4e60b8), word(script_4e5fd8), word(script_4e6060), word(script_4e60b8), word(script_4e60b8), word(script_4e5fd8), word(script_4e60b8), word(script_4e5fd8), word(script_4e5fd8), word(script_4e60b8), word(script_4e60b8), word(script_4e5fd8), word(script_4e5fd8)
script_4e6064:
  attack_selector 0x0, 0x18
script_4e606c:
  sound_a 0x1
script_4e6070:
  play_clip 0x9, word(data_4faff8)
script_4e6078:
  query_b 0x18
script_4e607c:
  action_recovery 0x0
script_4e6080:
  forward 0x0
script_4e6084:
  loop 0x2
script_4e6088:
  playback_tail 0xb
script_4e608c:
  wait 0x2
script_4e6090:
  playback_tail 0xa
script_4e6094:
  wait 0x2
script_4e6098:
  playback_tail 0xb
script_4e609c:
  wait 0x2
script_4e60a0:
  playback_tail 0xc
script_4e60a4:
  wait 0x2
script_4e60a8:
  repeat 0x0
script_4e60ac:
  clip_operation 0x0, word(data_4faff8)
script_4e60b4:
  return 0x0
script_4e60b8:
  attack_selector 0x0, 0x19
script_4e60c0:
  sound_a 0x1
script_4e60c4:
  play_clip 0x9, word(data_4fb004)
script_4e60cc:
  query_b 0x19
script_4e60d0:
  action_recovery 0x0
script_4e60d4:
  forward 0x0
data_4e60d8:
  loop 0x2
script_4e60dc:
  playback_tail 0xb
script_4e60e0:
  wait 0x2
script_4e60e4:
  playback_tail 0xa
script_4e60e8:
  wait 0x2
script_4e60ec:
  playback_tail 0xb
script_4e60f0:
  wait 0x2
script_4e60f4:
  playback_tail 0xc
script_4e60f8:
  wait 0x2
script_4e60fc:
  repeat 0x0
script_4e6100:
  clip_operation 0x0, word(data_4fb004)
script_4e6108:
  return 0x0
  .zero 4

MK4_SCRIPT_CROUCH_LOW_KICK:
  attack_selector 0x0, 0x17
script_4e6160:
  sound_c 0x1b
script_4e6164:
  open_clip 0x0, word(data_4fbc40)
script_4e616c:
  cursor_set 0x1
script_4e6170:
  forward_to 0x9
script_4e6174:
  tail 0x0, MK4_NATIVE_CROUCH_LOW_KICK_TAIL
  .zero 4

MK4_SCRIPT_CROUCH_LOW_KICK_RECOVER:
  cursor_set 0x9
script_4e6184:
  playback_pair 0x1
script_4e6188:
  return 0x0
  .zero 4

MK4_SCRIPT_KNEE_SCRIPT:
  open_clip 0x0, word(data_4fbc34)
script_4e6198:
  playback_tail 0x5
script_4e619c:
  wait 0x1
script_4e61a0:
  playback_tail 0x6
script_4e61a4:
  wait 0x1
script_4e61a8:
  playback_tail 0x7
script_4e61ac:
  wait 0x1
script_4e61b0:
  playback_tail 0x8
script_4e61b4:
  wait 0x1
script_4e61b8:
  playback_tail 0x9
script_4e61bc:
  action_recovery 0x0
script_4e61c0:
  tail 0x0, MK4_NATIVE_CLOSE_HIGH_KICK_TAIL
  .zero 4

MK4_SCRIPT_KNEE_MISS_FINISHED:
  wait 0x4
script_4e61d4:
  backward 0x0
script_4e61d8:
  ready 0x0
  .zero 4

MK4_SCRIPT_CLOSE_HIGH_PUNCH:
  attack_selector 0x0, 0x110
script_4e61e8:
  forward_to 0x7
script_4e61ec:
  tail 0x0, MK4_NATIVE_CLOSE_HIGH_PUNCH_TAIL
  .zero 4

MK4_SCRIPT_AIR_STRAIGHT_KICK_STARTUP:
  select_roster 0x0, word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6df0), word(script_4e6238), word(script_4e6238)
script_4e623c:
  attack_selector 0x0, 0x28
script_4e6244:
  sound_a 0x1f
script_4e6248:
  clip 0x0, word(data_4fbcd0)
script_4e6250:
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_TAIL
  .zero 4

MK4_SCRIPT_CAGE_AIR_KICK_NARROW_MISS_REVERSAL:
  clip_operation 0x0, word(data_4fbcd0)
script_4e6278:
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_LAND
MK4_SCRIPT_AIR_PUNCH_CONTACT:
  allow_commands 0x0
script_4e6284:
  wait 0xc
script_4e6288:
  gravity 0x0, 0x28f
script_4e6290:
  native_context 0x0
script_4e6294:
  tail 0x0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4

MK4_SCRIPT_UPPERCUT:
  attack_selector 0x0, 0x102
script_4e62a8:
  prefixed_to 0x205, word(data_4fbb38)
script_4e62b0:
  sound_c 0x1c
script_4e62b4:
  tail 0x0, MK4_NATIVE_UPPERCUT_DEFAULT
  .zero 4

MK4_SCRIPT_CAGE_DRAW_WEAPON_DRAW:
  sound_variant 0x0
script_4e62c4:
  play_clip 0x16, word(data_4fb8ec)
script_4e62cc:
  tail 0x0, MK4_NATIVE_WEAPON_DRAW_EQUIP
  .zero 4

MK4_SCRIPT_SHINNOK_DRAW_WEAPON_EQUIP:
  forward_count 0xd
script_4e62dc:
  sound_c 0x12
script_4e62e0:
  forward 0x0
script_4e62e4:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4

MK4_SCRIPT_JAB:
  call 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE
script_4e62f8:
  sound_a 0xa
script_4e62fc:
  play_clip 0x8, word(data_4fbb44)
script_4e6304:
  call 0x0, MK4_NATIVE_JAB_CONTACT
script_4e630c:
  wait 0x1
script_4e6310:
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(script_4e63a0)
script_4e631c:
  sound_a 0xa
script_4e6320:
  open_clip 0x0, word(data_4fbb50)
script_4e6328:
  tail 0x0, MK4_NATIVE_CAGE_JAB_SECOND_PUNCH
MK4_SCRIPT_CAGE_JAB_SECOND_PUNCH:
  cursor_set 0x0
script_4e6334:
  forward_count 0xa
script_4e6338:
  call 0x0, MK4_NATIVE_JAB_CONTACT
script_4e6340:
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(script_4e63c8)
script_4e634c:
  sound_a 0xa
script_4e6350:
  forward 0x0
script_4e6354:
  tail 0x0, MK4_NATIVE_CAGE_JAB_REPEAT
  .zero 4

MK4_SCRIPT_CAGE_JAB_REPEAT:
  wait 0x1
script_4e6364:
  call 0x0, MK4_NATIVE_JAB_CONTACT
script_4e636c:
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(script_4e63a0)
script_4e6378:
  sound_a 0xa
script_4e637c:
  play_clip 0xa, word(data_4fbb50)
script_4e6384:
  call 0x0, MK4_NATIVE_JAB_CONTACT
script_4e638c:
  predicate_flags 0x0, MK4_NATIVE_JAB_REPEAT_PREDICATE, word(script_4e63c8)
script_4e6398:
  jump 0x0, word(script_4e634c)
script_4e63a0:
  tail 0x0, MK4_NATIVE_CAGE_JAB_RECOVERY
  .zero 4

MK4_SCRIPT_CAGE_JAB_RECOVERY:
  action_recovery 0x0
script_4e63b4:
  open_clip 0x9, word(data_4fbb44)
script_4e63bc:
  double_to 0x11
script_4e63c0:
  native_sound 0x0
script_4e63c4:
  ready 0x0
script_4e63c8:
  action_recovery 0x0
script_4e63cc:
  forward_count 0x3
script_4e63d0:
  ready 0x0
  .zero 4

MK4_SCRIPT_AIR_DIAGONAL_LOW_KICK_STARTUP:
  sound_a 0x19
script_4e63dc:
  select_roster 0x0, word(script_4e641c), word(script_4e6e18), word(script_4e6e30), word(script_4e641c), word(script_4e6e18), word(script_4e6e30), word(script_4e641c), word(script_4e6e18), word(script_4e6e30), word(script_4e641c), word(script_4e6e18), word(script_4e6e30), word(script_4e641c), word(script_4e6e18), word(script_4e6e30), word(script_4e6e30)
script_4e6420:
  clip 0x0, word(data_4fb034)
script_4e6428:
  attack_selector 0x0, 0x1b
script_4e6430:
  tail 0x0, MK4_NATIVE_AIR_LK_TAIL
  .zero 4

MK4_SCRIPT_AIR_LOW_KICK_CLIP_58_REVERSE:
  clip_operation 0x0, word(data_4fb034)
script_4e6460:
  native_context 0x0
script_4e6464:
  tail 0x0, MK4_NATIVE_AIR_LOW_KICK_LAND
  .zero 4

MK4_SCRIPT_AIR_LOW_KICK_CLIP_59_REVERSE:
  clip_operation 0x0, word(data_4fb040)
script_4e6498:
  native_context 0x0
script_4e649c:
  tail 0x0, 0x46e0d0
  .zero 4

MK4_SCRIPT_AIR_LOW_KICK_CLIP_60_REVERSE:
  clip_operation 0x0, word(data_4fb04c)
script_4e64d0:
  native_context 0x0
script_4e64d4:
  tail 0x0, 0x46e1b0
  .zero 4

MK4_SCRIPT_AIR_LOW_KICK_CONTACT_HOLD:
  landing 0x0
script_4e64e4:
  wait 0x8
script_4e64e8:
  allow_commands 0x0
script_4e64ec:
  gravity 0x0, 0x51e
script_4e64f4:
  native_context 0x0
script_4e64f8:
  tail 0x0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
MK4_SCRIPT_AIR_DIAGONAL_HIGH_KICK_STARTUP:
  sound_a 0x19
script_4e6504:
  select_roster 0x0, word(script_4e6544), word(script_4e6e48), word(script_4e6e58), word(script_4e6544), word(script_4e6e48), word(script_4e6e58), word(script_4e6544), word(script_4e6e48), word(script_4e6e58), word(script_4e6544), word(script_4e6e48), word(script_4e6e58), word(script_4e6544), word(script_4e6e48), word(script_4e6e58), word(script_4e6e48)
script_4e6548:
  attack_selector 0x0, 0x1e
script_4e6550:
  tail 0x0, MK4_NATIVE_AIR_DIAGONAL_TAIL
  .zero 4

MK4_SCRIPT_AIR_DIAGONAL_KICK_CLIP_55_REVERSE:
  clip_operation 0x0, word(data_4fb010)
script_4e6590:
  native_context 0x0
script_4e6594:
  tail 0x0, 0x46e450
  .zero 4

MK4_SCRIPT_AIR_DIAGONAL_KICK_CLIP_56_REVERSE:
  clip_operation 0x0, word(data_4fb01c)
script_4e65d8:
  native_context 0x0
script_4e65dc:
  tail 0x0, 0x46e540
  .zero 4

MK4_SCRIPT_AIR_DIAGONAL_KICK_CLIP_57_REVERSE:
  clip_operation 0x0, word(data_4fb028)
script_4e6620:
  native_context 0x0
script_4e6624:
  tail 0x0, 0x46e630
  .zero 4

MK4_SCRIPT_AIR_DIAGONAL_KICK_CONTACT_HOLD:
  landing 0x0
script_4e6634:
  wait 0x4
script_4e6638:
  allow_commands 0x0
script_4e663c:
  wait 0x4
script_4e6640:
  gravity 0x0, 0x51e
script_4e6648:
  native_context 0x0
script_4e664c:
  tail 0x0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4

MK4_SCRIPT_AIR_HIGH_PUNCH_STARTUP:
  attack_selector 0x0, 0x104
script_4e6660:
  sound_a 0x19
script_4e6664:
  tail 0x0, MK4_NATIVE_AIR_HP_TAIL
  .zero 4

MK4_SCRIPT_AIR_HIGH_PUNCH_MISS_REVERSE:
  clip_operation 0x0, word(data_4fb208)
script_4e66a8:
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_LAND
  .zero 4

MK4_SCRIPT_AIR_HIGH_CONTACT_END:
  gravity 0x0, 0x28f
script_4e66c0:
  native_context 0x0
script_4e66c4:
  tail 0x0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4

MK4_SCRIPT_JAX_DRAW_WEAPON_AIR_HP:
  attack_selector 0x0, 0x104
script_4e66d8:
  tail 0x0, MK4_NATIVE_WEAPON_AIR_HIGH_PUNCH_OPEN
  .zero 4

MK4_SCRIPT_JAX_DRAW_WEAPON_AIR_LP:
  attack_selector 0x0, 0x108
script_4e66f8:
  tail 0x0, MK4_NATIVE_WEAPON_AIR_LOW_PUNCH_OPEN
  .zero 4

MK4_SCRIPT_AIR_LOW_PUNCH_STARTUP:
  attack_selector 0x0, 0x105
script_4e6718:
  sound_a 0x19
script_4e671c:
  tail 0x0, MK4_NATIVE_AIR_LP_TAIL
  .zero 4

MK4_SCRIPT_AIR_LOW_PUNCH_MISS_REVERSE:
  clip_operation 0x0, word(data_4fb1fc)
script_4e6760:
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_LAND
  .zero 4

MK4_SCRIPT_AIR_LOW_PUNCH_CONTACT_HOLD:
  allow_commands 0x0
script_4e6774:
  landing 0x0
script_4e6778:
  wait 0xc
script_4e677c:
  gravity 0x0, 0x28f
script_4e6784:
  native_context 0x0
script_4e6788:
  tail 0x0, MK4_NATIVE_AIR_ATTACK_LAND_OR_FINISH
  .zero 4

MK4_SCRIPT_SWEEP:
  select_roster 0x0, word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e67d8), word(script_4e6e68), word(script_4e6e68)
script_4e67dc:
  attack_selector 0x0, 0x24
script_4e67e4:
  open_clip 0x0, word(data_4fb064)
script_4e67ec:
  double_to 0xa
script_4e67f0:
  sound_a 0xaf
script_4e67f4:
  forward_to 0x16
script_4e67f8:
  tail 0x0, MK4_NATIVE_SWEEP_ADVANCING_TAIL
MK4_SCRIPT_ROUNDHOUSE:
  select_roster 0x0, word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6e90), word(script_4e6840), word(script_4e6840)
script_4e6844:
  attack_selector 0x0, 0x21
script_4e684c:
  open_clip 0x0, word(data_4fbc28)
script_4e6854:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_START
  .zero 4

MK4_SCRIPT_CAGE_ROUNDHOUSE_FAST_START:
  double_to 0xc
script_4e6864:
  sound_c 0x1c
script_4e6868:
  double_to 0x15
script_4e686c:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4

MK4_SCRIPT_CAGE_ROUNDHOUSE_NORMAL_START:
  double_to 0xc
script_4e687c:
  sound_c 0x1c
script_4e6880:
  forward_to 0x15
script_4e6884:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_TAIL
  .zero 4

MK4_SCRIPT_JAX_ROUNDHOUSE_FAST_START:
  double_to 0xc
script_4e6894:
  sound_c 0x1c
script_4e6898:
  double_to 0x14
script_4e689c:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_WIDE_TAIL
  .zero 4

MK4_SCRIPT_JAX_ROUNDHOUSE_NORMAL_START:
  double_to 0xc
script_4e68ac:
  sound_c 0x1c
script_4e68b0:
  forward_to 0x14
script_4e68b4:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_WIDE_TAIL
  .zero 4

MK4_SCRIPT_SPIKE_ENTER:
  wait 0x1c
script_4e6954:
  prefixed_to 0x205, word(data_4fbb38)
script_4e695c:
  sound_c 0x1c
script_4e6960:
  query_a 0x12
script_4e6964:
  tail 0x0, MK4_NATIVE_STAGE_OWNER
  .zero 4

MK4_SCRIPT_KAI_HIGH_KICK_RECOVERY:
  action_recovery 0x0
script_4e699c:
  forward_count 0x2
script_4e69a0:
  release_attack 0x0
script_4e69a4:
  forward 0x0
script_4e69a8:
  ready 0x0
  .zero 4

MK4_SCRIPT_JAX_COMBO_HP_ATTACKER:
  open_clip 0x2, word(data_4fb9b8)
script_4e69b8:
  forward_to 0xb
script_4e69bc:
  tail 0x0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  .zero 4

MK4_SCRIPT_SUB_ZERO_COMBO_HP_ATTACKER:
  open_clip 0x0, word(data_4fc66c)
script_4e69d0:
  prefixed_sample 0x0
script_4e69d4:
  wait 0x1
script_4e69d8:
  prefixed_sample 0x1
script_4e69dc:
  wait 0x1
script_4e69e0:
  prefixed_sample 0x2
script_4e69e4:
  wait 0x1
script_4e69e8:
  prefixed_sample 0x3
script_4e69ec:
  wait 0x1
script_4e69f0:
  prefixed_sample 0x4
script_4e69f4:
  wait 0x1
script_4e69f8:
  prefixed_sample 0x6
script_4e69fc:
  wait 0x1
script_4e6a00:
  prefixed_sample 0x8
script_4e6a04:
  wait 0x1
script_4e6a08:
  prefixed_sample 0x9
script_4e6a0c:
  wait 0x1
script_4e6a10:
  prefixed_sample 0xa
script_4e6a14:
  wait 0x1
script_4e6a18:
  prefixed_sample 0xb
script_4e6a1c:
  wait 0x1
script_4e6a20:
  tail 0x0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
MK4_SCRIPT_KITANA_COMBO_HIGH_PUNCH_ATTACKER:
  open_clip 0x2, word(data_4fc678)
script_4e6a30:
  forward_to 0xb
script_4e6a34:
  tail 0x0, MK4_NATIVE_COMBO_HIGH_PUNCH_TAIL
  .zero 4

MK4_SCRIPT_RAIDEN_VICTORY:
  select_roster 0x0, word(script_4e6ad4), word(script_4e6ba4), word(script_4e6b04), word(script_4e6b6c), word(script_4e6ad4), word(script_4e6be0), word(script_4e6ab4), word(script_4e6b1c), word(script_4e6b34), word(script_4e6b88), word(script_4e6bc0), word(script_4e6b50), word(script_4e6b34), word(script_4e6b04), word(script_4e6ab4), word(script_4e6a84)
script_4e6a84:
  native_release 0x53
script_4e6a88:
  prefixed_to 0x24a, word(data_4fc954)
script_4e6a90:
  sound_random 0x0
script_4e6a94:
  playback_pair 0x48
script_4e6a98:
  forward_to 0x4a
script_4e6a9c:
  playback_pair 0x48
script_4e6aa0:
  forward_to 0x4a
script_4e6aa4:
  playback_pair 0x48
script_4e6aa8:
  forward_to 0x4a
script_4e6aac:
  forward 0x0
script_4e6ab0:
  park 0x0
script_4e6ab4:
  prefixed_to 0x23c, word(data_4fb2b0)
script_4e6abc:
  native_release 0xf
script_4e6ac0:
  forward 0x0
script_4e6ac4:
  cursor_set 0x2a
script_4e6ac8:
  forward 0x0
script_4e6acc:
  jump 0x0, word(script_4e6ac4)
script_4e6ad4:
  prefixed_finish 0x200, word(data_4fb2e0)
script_4e6adc:
  open_clip 0x0, word(data_4fb2ec)
script_4e6ae4:
  prefixed_finish 0x0
script_4e6ae8:
  native_release 0x6
script_4e6aec:
  prefixed_finish 0x0
script_4e6af0:
  prefixed_finish 0x200, word(data_4fb2f8)
script_4e6af8:
  bind_cursor 0x0, word(data_4fb304)
script_4e6b00:
  park 0x0
script_4e6b04:
  prefixed_finish 0x200, word(data_4fb2d4)
script_4e6b0c:
  playback_pair 0x45
script_4e6b10:
  forward_to 0x63
script_4e6b14:
  jump 0x0, word(script_4e6b0c)
script_4e6b1c:
  prefixed_to 0x244, word(data_4fb2c8)
script_4e6b24:
  forward 0x0
script_4e6b28:
  cursor_set 0x45
script_4e6b2c:
  jump 0x0, word(script_4e6b24)
script_4e6b34:
  native_release 0x23
script_4e6b38:
  prefixed_finish 0x200, word(data_4fb358)
script_4e6b40:
  cursor_set 0x55
script_4e6b44:
  forward 0x0
script_4e6b48:
  jump 0x0, word(script_4e6b40)
script_4e6b50:
  native_release 0x1f
script_4e6b54:
  prefixed_finish 0x200, word(data_4fb334)
script_4e6b5c:
  cursor_set 0x60
script_4e6b60:
  forward 0x0
script_4e6b64:
  jump 0x0, word(script_4e6b5c)
script_4e6b6c:
  native_release 0x1e
script_4e6b70:
  prefixed_finish 0x200, word(data_4fb310)
script_4e6b78:
  cursor_set 0x90
script_4e6b7c:
  forward 0x0
script_4e6b80:
  jump 0x0, word(script_4e6b78)
script_4e6b88:
  native_release 0x1d
script_4e6b8c:
  prefixed_finish 0x200, word(data_4fb34c)
script_4e6b94:
  cursor_set 0x58
script_4e6b98:
  forward 0x0
script_4e6b9c:
  jump 0x0, word(script_4e6b94)
script_4e6ba4:
  native_release 0x22
script_4e6ba8:
  play_clip 0x14, word(data_4fb340)
script_4e6bb0:
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_VICTORY_EFFECT
script_4e6bb8:
  forward 0x0
script_4e6bbc:
  park 0x0
script_4e6bc0:
  native_release 0x1b
script_4e6bc4:
  prefixed_to 0x25a, word(data_4fb328)
script_4e6bcc:
  native_release 0x1c
script_4e6bd0:
  forward 0x0
script_4e6bd4:
  cursor_set 0xa0
script_4e6bd8:
  jump 0x0, word(script_4e6bd0)
script_4e6be0:
  native_release 0x19
script_4e6be4:
  prefixed_to 0x255, word(data_4fb31c)
script_4e6bec:
  spawn 0x0, 0x26, MK4_NATIVE_SCENE_HOVER
script_4e6bf8:
  native_release 0x1a
script_4e6bfc:
  forward 0x0
script_4e6c00:
  cursor_set 0x5d
script_4e6c04:
  forward 0x0
script_4e6c08:
  jump 0x0, word(script_4e6c00)
  .zero 4

script_4e6c90:
  attack_selector 0x0, 0x1
script_4e6c98:
  sound_a 0x1
script_4e6c9c:
  play_clip 0x3, word(data_4fb094)
script_4e6ca4:
  double_count 0x11
script_4e6ca8:
  tail 0x0, MK4_NATIVE_KICK_CONTACT_WIDE
script_4e6cb0:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6cb8:
  attack_selector 0x0, 0x5
script_4e6cc0:
  sound_a 0x1
script_4e6cc4:
  open_clip 0x0, word(data_4fbbec)
script_4e6ccc:
  cursor_set 0x4
script_4e6cd0:
  double_to 0x10
script_4e6cd4:
  call 0x0, MK4_NATIVE_AVOID_SET
script_4e6cdc:
  double_to 0x18
script_4e6ce0:
  query_b 0x5
script_4e6ce4:
  wait_branch 0x4002
script_4e6ce8:
  action_recovery 0x0
script_4e6cec:
  backward_count 0x2
script_4e6cf0:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6cf8:
  release_attack 0x0
script_4e6cfc:
  tail 0x0, MK4_NATIVE_LOW_KICK_REVERSE_RETURN
  .zero 4

script_4e6d08:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6d10:
  attack_selector 0x0, 0x6
script_4e6d18:
  sound_a 0x1
script_4e6d1c:
  open_clip 0x0, word(data_4fb0a0)
data_4e6d24:
  prefixed_sample 0x0
script_4e6d28:
  wait 0x1
script_4e6d2c:
  prefixed_sample 0x2
script_4e6d30:
  wait 0x1
script_4e6d34:
  prefixed_sample 0x4
script_4e6d38:
  wait 0x1
script_4e6d3c:
  prefixed_sample 0x6
script_4e6d40:
  wait 0x1
script_4e6d44:
  prefixed_sample 0x8
script_4e6d48:
  wait 0x1
script_4e6d4c:
  prefixed_sample 0xa
script_4e6d50:
  wait 0x1
script_4e6d54:
  prefixed_sample 0xc
script_4e6d58:
  wait 0x1
script_4e6d5c:
  call 0x0, MK4_NATIVE_AVOID_SET
script_4e6d64:
  prefixed_sample 0xe
script_4e6d68:
  wait 0x1
script_4e6d6c:
  prefixed_sample 0x10
script_4e6d70:
  wait 0x1
script_4e6d74:
  prefixed_sample 0x11
script_4e6d78:
  wait 0x1
script_4e6d7c:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6d84:
  query_b 0x6
script_4e6d88:
  action_recovery 0x0
script_4e6d8c:
  release_attack 0x0
script_4e6d90:
  wait_branch 0x8005
script_4e6d94:
  tail 0x0, MK4_NATIVE_LOW_KICK_NARROW_REVERSE_RETURN
  .zero 4

script_4e6da0:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6da8:
  attack_selector 0x0, 0x7
script_4e6db0:
  sound_a 0x1
script_4e6db4:
  open_clip 0x0, word(data_4fb0c4)
script_4e6dbc:
  prefixed_to 0x6
script_4e6dc0:
  call 0x0, MK4_NATIVE_AVOID_SET
script_4e6dc8:
  forward_to 0xb
script_4e6dcc:
  call 0x0, MK4_NATIVE_AVOID_CLEAR
script_4e6dd4:
  query_b 0x7
script_4e6dd8:
  action_recovery 0x0
script_4e6ddc:
  wait_branch 0x4002
script_4e6de0:
  forward_count 0x2
script_4e6de4:
  release_attack 0x0
script_4e6de8:
  forward 0x0
script_4e6dec:
  ready 0x0
script_4e6df0:
  attack_selector 0x0, 0x27
script_4e6df8:
  open_clip 0x0, word(data_4fb058)
script_4e6e00:
  cursor_set 0x4
script_4e6e04:
  forward_to 0x7
script_4e6e08:
  sound_a 0x1f
script_4e6e0c:
  forward_to 0xa
script_4e6e10:
  tail 0x0, MK4_NATIVE_AIR_STRAIGHT_WIDE_TAIL
script_4e6e18:
  clip 0x0, word(data_4fb040)
script_4e6e20:
  attack_selector 0x0, 0x1c
script_4e6e28:
  tail 0x0, MK4_NATIVE_AIR_LK_ALT_TAIL
script_4e6e30:
  clip 0x0, word(data_4fb04c)
script_4e6e38:
  attack_selector 0x0, 0x1d
script_4e6e40:
  tail 0x0, MK4_NATIVE_AIR_LK_WIDE_TAIL
script_4e6e48:
  attack_selector 0x0, 0x1f
script_4e6e50:
  tail 0x0, MK4_NATIVE_AIR_DIAGONAL_ALT_TAIL
script_4e6e58:
  attack_selector 0x0, 0x20
script_4e6e60:
  tail 0x0, MK4_NATIVE_AIR_DIAGONAL_WIDE_TAIL
script_4e6e68:
  attack_selector 0x0, 0x23
script_4e6e70:
  open_clip 0x0, word(data_4fb070)
script_4e6e78:
  double_to 0xa
script_4e6e7c:
  sound_a 0xaf
script_4e6e80:
  forward_to 0x16
script_4e6e84:
  tail 0x0, MK4_NATIVE_SWEEP_STANDARD_TAIL
  .zero 4

script_4e6e90:
  attack_selector 0x0, 0x22
script_4e6e98:
  open_clip 0x0, word(data_4fb07c)
script_4e6ea0:
  tail 0x0, MK4_NATIVE_ROUNDHOUSE_WIDE_START
  .zero 4

MK4_SCRIPT_CAGE_CPU_KICK:
  speed 0x0, 0x2666
script_4e80b0:
  sound_c 0x51
script_4e80b4:
  open_clip 0x0, word(data_4fbbec)
script_4e80bc:
  double_to 0x16
script_4e80c0:
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_CPU_KICK_TAIL
  .zero 4

MK4_SCRIPT_CAGE_POWERED_PUNCH:
  sound_c 0x51
script_4e80d4:
  open_clip 0x0, word(data_4fb178)
script_4e80dc:
  double_to 0xe
script_4e80e0:
  speed 0x0, 0x2b85
script_4e80e8:
  double_to 0x18
script_4e80ec:
  tail 0x0, MK4_NATIVE_CAGE_SCRIPT_POWERED_PUNCH_TAIL
  .zero 4

MK4_SCRIPT_QUAN_CHI_FLIP_KICK:
  sound_c 0x56
script_4e8104:
  open_clip 0x0, word(data_4fb268)
script_4e810c:
  prefixed_to 0xa
script_4e8110:
  speed 0x0, 0x547a
script_4e8118:
  forward_to 0xf
script_4e811c:
  tail 0x0, MK4_NATIVE_QUAN_CHI_SCRIPT_FLIP_KICK_TAIL
  .zero 4

MK4_SCRIPT_REPTILE_DASH:
  prefixed_finish 0x200, word(data_4fbba4)
script_4e8130:
  bind_cursor 0x0, word(data_4fbbb0)
script_4e8138:
  speed 0x0, 0x2e14
script_4e8140:
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_DASH_CONTACT
  .zero 4

MK4_SCRIPT_JAX_DASH_PUNCH:
  sound_indexed 0x25
script_4e8154:
  sound_c 0x4e
script_4e8158:
  speed 0x0, 0x2666
script_4e8160:
  open_clip 0x0, word(data_4fbb80)
script_4e8168:
  double_to 0x12
script_4e816c:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_CONTACT
  .zero 4

MK4_SCRIPT_REPTILE_ROLL:
  sound_indexed 0x23
script_4e817c:
  sound_c 0x46
script_4e8180:
  speed 0x0, 0x1eb8
script_4e8188:
  prefixed_to 0x20d, word(data_4fb274)
script_4e8190:
  query_d 0x15
script_4e8194:
  forward_to 0x11
script_4e8198:
  landing 0x0
script_4e819c:
  tail 0x0, MK4_NATIVE_REPTILE_SCRIPT_ROLL_TAIL
  .zero 4

MK4_SCRIPT_JAREK_QUAKE:
  sound_indexed 0x21
script_4e81ac:
  prefixed_to 0x214, word(data_4fb280)
script_4e81b4:
  sound_c 0x44
script_4e81b8:
  spawn_effect 0x3
script_4e81bc:
  tail 0x0, MK4_NATIVE_JAREK_QUAKE_TAIL
  .zero 4

MK4_SCRIPT_JAREK_QUAKE_TAIL:
  forward 0x0
script_4e81cc:
  ready 0x0
MK4_SCRIPT_SONYA_CARTWHEEL:
  sound_indexed 0x20
script_4e81d4:
  sound_c 0x6
script_4e81d8:
  speed 0x0, 0x1999
script_4e81e0:
  prefixed_to 0x20e, word(data_4fb25c)
script_4e81e8:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_CARTWHEEL_TAIL
MK4_SCRIPT_TANYA_DRILL_KICK:
  sound_indexed 0x1f
script_4e81f4:
  speed 0x0, 0x23d7
script_4e81fc:
  prefixed_to 0x20c, word(data_4fb250)
script_4e8204:
  tail 0x0, MK4_NATIVE_TANYA_DRILL_KICK_CONTACT
  .zero 4

MK4_SCRIPT_KAI_AERIAL_PUNCH:
  sound_c 0x3c
script_4e8214:
  motion 0x0, 0x2e14, 0xffffe667, 0x41
script_4e8224:
  prefixed_to 0x209, word(data_4fbb74)
script_4e822c:
  tail 0x0, MK4_NATIVE_KAI_AERIAL_PUNCH_CONTACT
  .zero 4

MK4_SCRIPT_FUJIN_DIVE:
  sound_c 0x39
script_4e825c:
  motion 0x0, 0x1eb8, 0xf5c, 0x41
script_4e826c:
  prefixed_finish 0x200, word(data_4fb034)
script_4e8274:
  tail 0x0, MK4_NATIVE_FUJIN_DIVE_CONTACT
  .zero 4

MK4_SCRIPT_DIVE_RECOIL:
  landing 0x0
script_4e829c:
  wait 0x8
script_4e82a0:
  allow_commands 0x0
script_4e82a4:
  gravity 0x0, 0x51e
script_4e82ac:
  native_context 0x0
script_4e82b0:
  tail 0x0, MK4_NATIVE_FUJIN_DIVE_LAND
MK4_SCRIPT_TELEPORT:
  sound_c 0x36
script_4e82bc:
  prefixed_to 0x203, word(data_4fb184)
script_4e82c4:
  motion 0x0, 0x0, 0xffffcccd, 0x0
script_4e82d4:
  forward 0x0
script_4e82d8:
  tail 0x0, MK4_NATIVE_TELEPORT_VANISH
MK4_SCRIPT_TELEPORT_SLAM:
  sound_indexed 0x18
script_4e82e4:
  native_release 0x17
script_4e82e8:
  landing 0x0
script_4e82ec:
  open_peer_clip 0x0, word(data_4fb484)
script_4e82f4:
  open_clip 0x0, word(data_4fb0f4)
script_4e82fc:
  playback_single 0x12
script_4e8300:
  tail 0x0, MK4_NATIVE_TELEPORT_SLAM_FALL
  .zero 4

MK4_SCRIPT_TELEPORT_SLAM_FALL:
  playback_single 0x1b
script_4e8314:
  tail 0x0, MK4_NATIVE_TELEPORT_SLAM_DROP
  .zero 4

MK4_SCRIPT_KAI_HANDSTAND_ENTER:
  sound_indexed 0x17
script_4e8324:
  prefixed_finish 0x200, word(data_4fad88)
script_4e832c:
  tail 0x0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT
  .zero 4

MK4_SCRIPT_HANDSTAND_SPIN:
  prefixed_finish 0x200, word(data_4fad94)
script_4e8340:
  bind_cursor 0x0, word(data_4fada0)
script_4e8348:
  tail 0x0, MK4_NATIVE_KAI_HANDSTAND_SPIN_BEGIN
  .zero 4

MK4_SCRIPT_HANDSTAND_FRONT:
  sound_a 0x1
script_4e835c:
  prefixed_to 0x209, word(MK4_DATA_ANIMATION_TABLE)
script_4e8364:
  tail 0x0, MK4_NATIVE_KAI_SCRIPT_CONTACT_B10_CONTINUATION
  .zero 4

MK4_SCRIPT_KAI_CONTACT_B10_CONTINUATION:
  wait 0x8
script_4e8394:
  playback_pair 0x0
script_4e8398:
  tail 0x0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT
MK4_SCRIPT_HANDSTAND_REAR:
  sound_a 0x1
script_4e83a4:
  prefixed_to 0x209, word(data_4fadc4)
script_4e83ac:
  tail 0x0, MK4_NATIVE_KAI_HANDSTAND_REAR_CONTACT
  .zero 4

MK4_SCRIPT_KAI_HANDSTAND_REAR_CONTACT:
  playback_pair 0x0
script_4e83dc:
  tail 0x0, MK4_NATIVE_KAI_SCRIPT_STAGE_EVENT
  .zero 4

MK4_SCRIPT_HANDSTAND_EXIT:
  sound_indexed 0x17
script_4e83ec:
  clip_operation 0x0, word(data_4fad88)
script_4e83f4:
  ready 0x0
  .zero 4

MK4_SCRIPT_RAIDEN_TORPEDO_FAST_STARTUP:
  open_clip 0x0, word(data_4fb1e4)
script_4e8448:
  playback_tail 0xe
script_4e844c:
  jump 0x0, word(script_4e8464)
  .zero 4

MK4_SCRIPT_RAIDEN_TORPEDO_ORDINARY_STARTUP:
  open_clip 0x0, word(data_4fb1e4)
script_4e8460:
  double_to 0xe
script_4e8464:
  call 0x0, MK4_NATIVE_RESTORE_MOTION
script_4e846c:
  handler 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MOTION
script_4e8474:
  sound_a 0x2b
script_4e8478:
  sound_c 0xb
script_4e847c:
  loop 0x1d
script_4e8480:
  predicate_argument 0x0, MK4_NATIVE_CONTACT_QUERY_D, word(script_4e8908)
script_4e848c:
  call 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_STEP
script_4e8494:
  wait 0x1
script_4e8498:
  repeat 0x0
script_4e849c:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_MISS
  .zero 4

MK4_SCRIPT_POWER_REWIND:
  clip_operation 0x0, word(MK4_ANIMATION_DESCRIPTOR_CLIP_301)
script_4e84c0:
  native_context 0x0
script_4e84c4:
  clear_handler 0x0
script_4e84c8:
  tail 0x0, MK4_NATIVE_RECOIL_LAND
  .zero 4

MK4_SCRIPT_SUB_ZERO_SLIDE:
  speed_negative 0x0, 0xffffd99a
script_4e84e0:
  sound_c 0xf
script_4e84e4:
  open_clip 0x0, word(data_4fb1f0)
script_4e84ec:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SLIDE_SCRIPT
  .zero 4

MK4_SCRIPT_SUB_ZERO_SLIDE_HIT:
  loop 0x4
script_4e84fc:
  playback_tail 0xa
script_4e8500:
  wait 0x1
script_4e8504:
  playback_tail 0x9
script_4e8508:
  wait 0x1
script_4e850c:
  playback_tail 0x8
script_4e8510:
  wait 0x1
script_4e8514:
  playback_tail 0x9
script_4e8518:
  wait 0x1
script_4e851c:
  repeat 0x0
script_4e8520:
  landing 0x0
script_4e8524:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
  .zero 4

MK4_SCRIPT_SUB_ZERO_SLIDE_BLOCK:
  no_guard 0x0
script_4e8534:
  landing 0x0
script_4e8538:
  loop 0x6
script_4e853c:
  playback_tail 0xa
script_4e8540:
  wait 0x1
script_4e8544:
  playback_tail 0x9
script_4e8548:
  wait 0x1
script_4e854c:
  playback_tail 0x8
script_4e8550:
  wait 0x1
script_4e8554:
  playback_tail 0x9
script_4e8558:
  wait 0x1
script_4e855c:
  repeat 0x0
script_4e8560:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_SLIDE_RETURN
  .zero 4

MK4_SCRIPT_SONYA_LEG_GRAB_PAIRED_GRAB:
  native_release 0x25
script_4e857c:
  sound_roster 0x1
script_4e8580:
  open_peer_clip 0x0, word(data_4fb6e8)
script_4e8588:
  playback_single 0x16
script_4e858c:
  playback_pair_reverse 0x13
script_4e8590:
  playback_single 0x16
script_4e8594:
  playback_pair_reverse 0x13
script_4e8598:
  playback_single 0x22
script_4e859c:
  native_step 0x4f
script_4e85a0:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_LEG_GRAB_TAIL
MK4_SCRIPT_SONYA_LEG_GRAB_MISS_OR_BLOCK:
  forward_to 0x16
script_4e85ac:
  playback_pair 0x13
script_4e85b0:
  forward_to 0x16
script_4e85b4:
  playback_pair 0x13
script_4e85b8:
  forward 0x0
script_4e85bc:
  ready 0x0
MK4_SCRIPT_SONYA_SQUARE_WAVE:
  speed 0x0, 0x1999
script_4e85c8:
  prefixed_finish 0x200, word(data_4fb0dc)
script_4e85d0:
  sound_indexed 0xe
script_4e85d4:
  bind_cursor 0x0, word(data_4fb0e8)
script_4e85dc:
  motion_block 0x0, 0x1999, 0x0, 0xffffe667, 0xffffff7d
script_4e85f0:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_SQUARE_WAVE_TAIL
  .zero 4

MK4_SCRIPT_LIU_KANG_BICYCLE_KICK:
  motion 0x0, 0x2147, 0xfffff5c3, 0x0
script_4e8620:
  call 0x0, MK4_NATIVE_SAVE_MOTION
script_4e8628:
  play_clip 0x6, word(data_4fb0ac)
script_4e8630:
  handler 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_BICYCLE_MOTION
script_4e8638:
  sound_indexed 0x1
script_4e863c:
  call 0x0, MK4_NATIVE_RESTORE_MOTION
script_4e8644:
  gravity 0x0, 0xc4
script_4e864c:
  tail 0x0, MK4_NATIVE_LIU_KANG_BICYCLE_SCRIPT
  .zero 4

MK4_SCRIPT_BICYCLE_LAND:
  clear_handler 0x0
script_4e867c:
  clip_alt 0x6, word(data_4fb0ac)
script_4e8684:
  ready 0x0
MK4_SCRIPT_BICYCLE_CONTACT_END:
  motion 0x0, 0x0, 0xffffeb86, 0xe5
script_4e8698:
  native_context 0x0
script_4e869c:
  clear_handler 0x0
script_4e86a0:
  tail 0x0, MK4_NATIVE_LIU_KANG_BICYCLE_LAND
MK4_SCRIPT_LIU_KANG_FLYING_KICK:
  sound_indexed 0x0
script_4e86ac:
  open_clip 0x0, word(data_4fb1cc)
script_4e86b4:
  motion 0x0, 0x23d7, 0xffffe8f6, 0x106
script_4e86c4:
  tail 0x0, MK4_NATIVE_LIU_KANG_FLYING_KICK_SCRIPT
  .zero 4

MK4_SCRIPT_LIU_KANG_FLYING_KICK_MISS:
  clip_operation 0x0, word(data_4fb1cc)
script_4e86d8:
  bind_submit 0x0, word(data_4fb1cc)
script_4e86e0:
  ready 0x0
  .zero 4

MK4_SCRIPT_LIU_KANG_FLYING_KICK_HIT:
  landing 0x0
script_4e86ec:
  speed_negative 0x0, 0xfffffae2
script_4e86f4:
  clip 0x0, word(data_4fb1d8)
script_4e86fc:
  wait 0x14
script_4e8700:
  gravity 0x0, 0x51e
script_4e8708:
  clip_operation 0x0, word(data_4fb1cc)
script_4e8710:
  native_context 0x0
script_4e8714:
  ready 0x0
MK4_SCRIPT_LIU_KANG_FLYING_KICK_BLOCK:
  landing 0x0
script_4e871c:
  wait 0xc
script_4e8720:
  landing 0x0
script_4e8724:
  wait 0xc
script_4e8728:
  gravity 0x0, 0x28f
script_4e8730:
  native_context 0x0
script_4e8734:
  tail 0x0, MK4_NATIVE_LIU_KANG_FLYING_KICK_LAND
  .zero 4

MK4_SCRIPT_FUJIN_TRIPLE_KICK:
  fast_start 0x0, word(script_4e8764)
script_4e8748:
  prefixed_to 0x206, word(data_4fb22c)
script_4e8750:
  speed 0x0, 0x2b85
script_4e8758:
  forward_to 0xd
script_4e875c:
  tail 0x0, MK4_NATIVE_FUJIN_TRIPLE_CONTACT
script_4e8764:
  tail 0x0, MK4_NATIVE_FUJIN_TRIPLE_FAST
  .zero 4

MK4_SCRIPT_FUJIN_TRIPLE_FAST:
  open_clip 0x0, word(data_4fb22c)
script_4e8778:
  double_to 0x6
script_4e877c:
  speed 0x0, 0x2b85
script_4e8784:
  double_to 0xd
script_4e8788:
  tail 0x0, MK4_NATIVE_FUJIN_TRIPLE_CONTACT
  .zero 4

MK4_SCRIPT_SCORPION_TELEPORT:
  motion 0x0, 0xffffd99a, 0xffffe8f6, 0x1ca
script_4e87c0:
  open_clip 0x0, word(data_4fbc1c)
script_4e87c8:
  sound_c 0x23
script_4e87cc:
  handler 0x0, MK4_NATIVE_TELEPORT_GROUP_CALLBACK
script_4e87d4:
  tail 0x0, MK4_NATIVE_SCORPION_TELEPORT_SCRIPT
  .zero 4

MK4_SCRIPT_TELEPORT_BLOCK_RETURN:
  clip_operation 0x0, word(MK4_ANIMATION_DESCRIPTOR_CLIP_301)
script_4e8820:
  native_context 0x0
script_4e8824:
  clear_handler 0x0
script_4e8828:
  tail 0x0, MK4_NATIVE_LAND
MK4_SCRIPT_FUJIN_WIND_GRAB:
  sound_indexed 0x1e
script_4e8834:
  sound_c 0x3d
script_4e8838:
  prefixed_finish 0x200, word(data_4fadf4)
script_4e8840:
  bind_cursor 0x0, word(data_4fae00)
script_4e8848:
  tail 0x0, MK4_NATIVE_FUJIN_GRAB_CONTROL_START
SWORD_SPIN_SCRIPT:
  prefixed_finish 0x200, word(data_4fb7f0)
script_4e8858:
  bind_cursor 0x0, word(data_4fb7fc)
script_4e8860:
  tail 0x0, MK4_NATIVE_SCORPION_SPIN_SCRIPT
  .zero 4

MK4_SCRIPT_KAI_LUNGE_KICK:
  sound_indexed 0x3b
script_4e8874:
  open_clip 0x0, word(data_4fc750)
script_4e887c:
  fast_start 0x0, word(script_4e88e0)
script_4e8884:
  double_to 0x1a
script_4e8888:
  sound_c 0x1f
script_4e888c:
  speed 0x0, 0x2666
script_4e8894:
  forward_to 0x1d
script_4e8898:
  tail 0x0, MK4_NATIVE_KAI_LUNGE_KICK_CONTACT
  .zero 4

MK4_SCRIPT_KAI_ARMED_HANDSTAND_ENTER:
  sound_indexed 0x17
script_4e88cc:
  open_clip 0x0, word(data_4fad88)
script_4e88d4:
  native_sound 0x0
script_4e88d8:
  tail 0x0, MK4_NATIVE_HANDSTAND_FRONT
script_4e88e0:
  cursor_set 0xe
script_4e88e4:
  jump 0x0, word(script_4e8884)
  .zero 4

script_4e8908:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_CONTACT
  .zero 4

MK4_SCRIPT_GUARD_AIRBORNE_FALL:
  sound_context 0x5
script_4e89a4:
  sound_b 0x3
script_4e89a8:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e89b8:
  task_callback 0x0
script_4e89bc:
  clip 0x0, word(data_4fb610)
script_4e89c4:
  native_context 0x0
script_4e89c8:
  forward 0x0
script_4e89cc:
  wait 0x2
script_4e89d0:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_JAX_DASH_PUNCH_AIRBORNE_VICTIM_168:
  effect 0x9
script_4e89dc:
  sound_b 0x1
script_4e89e0:
  spawn_effect 0x0
script_4e89e4:
  sound_context 0x0
script_4e89e8:
  effect 0x4
script_4e89ec:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e89fc:
  task_callback 0x0
script_4e8a00:
  clip 0x0, word(data_4fb658)
script_4e8a08:
  native_context 0x0
script_4e8a0c:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4

MK4_SCRIPT_REACT_KICK_AIR:
  effect 0x4
script_4e8a1c:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e8a2c:
  task_callback 0x0
script_4e8a30:
  clip 0x0, word(data_4fb658)
script_4e8a38:
  native_context 0x0
script_4e8a3c:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4

MK4_SCRIPT_QUAN_CHI_FLIP_REACTION:
  sound_b 0x3
script_4e8a4c:
  sound_context 0xb
script_4e8a50:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e8a60:
  task_callback 0x0
script_4e8a64:
  clip 0x0, word(data_4fb658)
script_4e8a6c:
  native_context 0x0
script_4e8a70:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
MK4_SCRIPT_REACTION_FALL_RETURN:
  motion 0x0, 0xfffff0a4, 0xffffd47b, 0x28f
script_4e8a88:
  task_callback 0x0
script_4e8a8c:
  open_clip 0x0, word(data_4fb3a0)
script_4e8a94:
  double_to 0xd
script_4e8a98:
  forward_to 0x29
script_4e8a9c:
  nop 0x13ece8
script_4e8aa0:
  native_context 0x0
script_4e8aa4:
  forward_count 0x4
script_4e8aa8:
  prefixed_finish 0x200, word(MK4_ANIMATION_DESCRIPTOR_CLIP_185)
script_4e8ab0:
  tail 0x0, MK4_NATIVE_VM_RETURN
MK4_SCRIPT_IMPACT_REACTION:
  effect 0xc
script_4e8abc:
  sound_context 0xb
script_4e8ac0:
  sound_b 0x3
script_4e8ac4:
  peer_motion 0x0, 0xfffff0a4, 0xffffd47b, 0x28f
script_4e8ad4:
  task_callback 0x0
script_4e8ad8:
  open_clip 0x0, word(data_4fb3a0)
script_4e8ae0:
  double_to 0xd
script_4e8ae4:
  forward_to 0x29
script_4e8ae8:
  nop 0x13ece8
script_4e8aec:
  native_context 0x0
script_4e8af0:
  forward_count 0x4
script_4e8af4:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_REACT_JAB_AIR:
  effect 0x6
script_4e8b04:
  sound_context 0xf
script_4e8b08:
  motion 0x0, 0xfffff334, 0xffffdc29, 0x28f
script_4e8b18:
  task_callback 0x0
script_4e8b1c:
  play_clip 0x1e, word(data_4fb610)
script_4e8b24:
  native_context 0x0
script_4e8b28:
  forward 0x0
script_4e8b2c:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_SONYA_WEAPON_HP_VICTIM:
  sound_c 0x8
script_4e8b3c:
  sound_b 0x1
script_4e8b40:
  effect 0x5
script_4e8b44:
  clip 0x0, word(data_4fb3dc)
script_4e8b4c:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e8b5c:
  task_callback 0x0
script_4e8b60:
  clip 0x0, word(data_4fb610)
script_4e8b68:
  native_context 0x0
script_4e8b6c:
  forward 0x0
script_4e8b70:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_KAI_DRAW_WEAPON_HP_VICTIM:
  sound_b 0x1
script_4e8b7c:
  effect 0x5
script_4e8b80:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e8b90:
  task_callback 0x0
script_4e8b94:
  clip 0x0, word(data_4fb610)
script_4e8b9c:
  native_context 0x0
script_4e8ba0:
  forward 0x0
script_4e8ba4:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_TANYA_WEAPON_REACTION_REACTION_170_VICTIM:
  effect 0x10
script_4e8bb4:
  sound_context 0x1
script_4e8bb8:
  jump 0x0, word(script_4e8bc8)
MK4_SCRIPT_KAI_DRAW_WEAPON_BACK_VICTIM:
  sound_context 0x6
script_4e8bc4:
  effect 0x1
script_4e8bc8:
  spawn_effect 0x0
script_4e8bcc:
  sound_b 0x1
script_4e8bd0:
  reaction_context 0x0
script_4e8bd4:
  motion_block 0x0, 0xfffff5c3, 0xccc, 0x0, 0x0
script_4e8be8:
  handler 0x0, MK4_NATIVE_FACE_PEER_ORIENT_GROUP
script_4e8bf0:
  open_clip 0x0, word(data_4fb604)
script_4e8bf8:
  native_sound 0x0
script_4e8bfc:
  landing 0x0
script_4e8c00:
  ready 0x0
  .zero 4

MK4_SCRIPT_JAREK_WEAPON_REACTION_KNIFE_VICTIM:
  effect 0x1
script_4e8c0c:
  spawn_effect 0x0
script_4e8c10:
  sound_context 0x6
script_4e8c14:
  sound_b 0x1
script_4e8c18:
  speed_negative 0x0, 0xccc
script_4e8c20:
  prefixed_finish 0x200, word(data_4fb394)
script_4e8c28:
  ready 0x0
  .zero 4

MK4_SCRIPT_WEAPON_CLIP_189_LAUNCH_BACK_GETUP:
  effect 0x2
script_4e8c34:
  sound_b 0x1
script_4e8c38:
  sound_context 0x6
script_4e8c3c:
  motion 0x0, 0xfffff99a, 0xffffc000, 0x28f
script_4e8c4c:
  task_callback 0x0
script_4e8c50:
  effect 0x0
script_4e8c54:
  open_clip 0x0, word(data_4fb658)
script_4e8c5c:
  forward_to 0x9
script_4e8c60:
  half_finish 0x0
script_4e8c64:
  native_context 0x0
script_4e8c68:
  forward 0x0
script_4e8c6c:
  tail 0x0, MK4_NATIVE_GETUP_BACK
  .zero 4

MK4_SCRIPT_REACTION_CLIP_311_SHORT_SLIDE:
  effect 0x10
script_4e8c7c:
  sound_context 0x6
script_4e8c80:
  speed_negative 0x0, 0xccc
script_4e8c88:
  clip 0x0, word(data_4fbc10)
script_4e8c90:
  ready 0x0
  .zero 4

MK4_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_137:
  effect 0x8
script_4e8c9c:
  effect 0x9
script_4e8ca0:
  sound_context 0x6
script_4e8ca4:
  speed_negative 0x0, 0xccc
script_4e8cac:
  open_clip 0x0, word(data_4fbc10)
script_4e8cb4:
  tail 0x0, MK4_NATIVE_REPTILE_WEAPON_STAGGER_TAIL
  .zero 4

MK4_SCRIPT_REPTILE_WEAPON_REACTION_REACTION_144:
  effect 0x0
script_4e8cc4:
  sound_b 0x1
script_4e8cc8:
  sound_context 0x6
script_4e8ccc:
  motion 0x0, 0xfffff70b, 0xffffc000, 0x28f
script_4e8cdc:
  task_callback 0x0
script_4e8ce0:
  effect 0x0
script_4e8ce4:
  open_clip 0x0, word(data_4fb658)
script_4e8cec:
  forward_to 0x9
script_4e8cf0:
  half_finish 0x0
script_4e8cf4:
  native_context 0x0
script_4e8cf8:
  forward_count 0x10
script_4e8cfc:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_SHINNOK_WEAPON_REACTION_LP_VICTIM:
  effect 0x0
script_4e8d0c:
  sound_context 0x6
script_4e8d10:
  sound_b 0x1
script_4e8d14:
  speed_negative 0x0, 0xa3d
script_4e8d1c:
  open_clip 0x0, word(data_4fb3ac)
script_4e8d24:
  double_to 0xe
script_4e8d28:
  landing 0x0
script_4e8d2c:
  forward 0x0
script_4e8d30:
  tail 0x0, MK4_NATIVE_GETUP_BACK
MK4_SCRIPT_CAGE_WEAPON_REACTION_LP_VICTIM:
  effect 0x0
script_4e8d3c:
  sound_context 0x6
script_4e8d40:
  sound_b 0x1
script_4e8d44:
  motion 0x0, 0xfffff852, 0xffffc7af, 0x28f
script_4e8d54:
  task_callback 0x0
script_4e8d58:
  clip 0x0, word(data_4fb658)
script_4e8d60:
  native_context 0x0
script_4e8d64:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4

MK4_SCRIPT_WEAPON_CLIP_189_LAUNCH_ROLL_GETUP:
  effect 0x0
script_4e8d74:
  sound_context 0x6
script_4e8d78:
  sound_b 0x1
script_4e8d7c:
  motion 0x0, 0xfffffc29, 0xffffc7af, 0x28f
script_4e8d8c:
  task_callback 0x0
script_4e8d90:
  clip 0x0, word(data_4fb658)
script_4e8d98:
  native_context 0x0
script_4e8d9c:
  tail 0x0, MK4_NATIVE_JAX_DASH_PUNCH_ROLL_GETUP
  .zero 4

MK4_SCRIPT_REACTION_CLIP_326_SHORT_SLIDE:
  effect 0x12
script_4e8dac:
  sound_context 0x6
script_4e8db0:
  speed_negative 0x0, 0xccc
script_4e8db8:
  clip 0x0, word(data_4fbcc4)
script_4e8dc0:
  call 0x0, 0x433c00
script_4e8dc8:
  ready 0x0
  .zero 4

MK4_SCRIPT_GORO_SPECIAL_PUNCH_189_VICTIM:
  effect 0x10
script_4e8dd4:
  spawn_effect 0x0
script_4e8dd8:
  sound_b 0x1
script_4e8ddc:
  sound_context 0x2
script_4e8de0:
  reaction_context 0x0
script_4e8de4:
  task_callback 0x0
script_4e8de8:
  motion_block 0x0, 0x0, 0x1999, 0xffffdeb9, 0x20c
script_4e8dfc:
  play_clip 0x22, word(data_4fbc70)
script_4e8e04:
  native_context 0x0
script_4e8e08:
  forward_count 0x8
script_4e8e0c:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_RAIDEN_STAFF_HP_WEAPON_VICTIM:
  spawn_effect 0x0
script_4e8e1c:
  sound_context 0x2
script_4e8e20:
  effect 0x0
script_4e8e24:
  sound_b 0x1
script_4e8e28:
  speed_negative 0x0, 0xa3d
script_4e8e30:
  open_clip 0x0, word(data_4fb3ac)
script_4e8e38:
  double_to 0xe
script_4e8e3c:
  landing 0x0
script_4e8e40:
  forward_count 0xa
script_4e8e44:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_GORO_SPECIAL_UPPERCUT_192_VICTIM:
  effect 0x3
script_4e8e54:
  effect 0x8
script_4e8e58:
  effect 0x8
script_4e8e5c:
  sound_b 0x1
script_4e8e60:
  sound_context 0x0
script_4e8e64:
  motion 0x0, 0x0, 0xffffc7af, 0x20c
script_4e8e74:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_UPPERCUT_VICTIM_SLIDE
  .zero 4

MK4_SCRIPT_GORO_UPPERCUT_VICTIM_SLIDE:
  sound_a 0x0
script_4e8e84:
  effect 0x0
script_4e8e88:
  play_clip 0x33, word(data_4fb5f8)
script_4e8e90:
  native_context 0x0
script_4e8e94:
  sound_a 0x17
script_4e8e98:
  forward 0x0
script_4e8e9c:
  tail 0x0, MK4_NATIVE_GORO_VICTIM_GETUP
  .zero 4

MK4_SCRIPT_LAUNCH_CLIP_131_BACK_GETUP:
  sound_context 0x6
script_4e8eac:
  sound_b 0x1
script_4e8eb0:
  motion 0x0, 0xfffff334, 0xffffdc29, 0x20c
script_4e8ec0:
  task_callback 0x0
script_4e8ec4:
  play_clip 0x28, word(data_4fb3a0)
script_4e8ecc:
  native_context 0x0
script_4e8ed0:
  forward 0x0
script_4e8ed4:
  tail 0x0, MK4_NATIVE_GETUP_BACK
  .zero 4

MK4_SCRIPT_REACTION_CLIP_180_SLIDE_HOLD:
  effect 0x8
script_4e8ee4:
  effect 0x2
script_4e8ee8:
  sound_context 0x6
script_4e8eec:
  sound_b 0x1
script_4e8ef0:
  speed_negative 0x0, 0x3333
script_4e8ef8:
  bind_submit 0x0, word(data_4fb5ec)
script_4e8f00:
  wait 0x6
script_4e8f04:
  clip 0x0, word(data_4fb5ec)
script_4e8f0c:
  ready 0x0
MK4_SCRIPT_LAUNCH_CLIP_131_NATIVE_FINISH_BACK_GETUP:
  effect 0x8
script_4e8f14:
  effect 0x2
script_4e8f18:
  sound_context 0x6
script_4e8f1c:
  sound_b 0x1
script_4e8f20:
  motion 0x0, 0xfffff5c3, 0xffffdc29, 0x24d
script_4e8f30:
  task_callback 0x0
script_4e8f34:
  play_clip 0x28, word(data_4fb3a0)
script_4e8f3c:
  native_context 0x0
script_4e8f40:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_SONYA_WEAPON_LP_VICTIM:
  effect 0x0
script_4e8f54:
  sound_b 0x1
script_4e8f58:
  speed_negative 0x0, 0x11eb
script_4e8f60:
  play_clip 0xe, word(data_4fb3ac)
script_4e8f68:
  native_context 0x0
script_4e8f6c:
  sound_a 0x4
script_4e8f70:
  landing 0x0
script_4e8f74:
  forward_count 0xe
script_4e8f78:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
MK4_SCRIPT_WEAPON_TUMBLE_BACK_GETUP:
  effect 0x8
script_4e8f84:
  effect 0x2
script_4e8f88:
  spawn_effect 0x6
script_4e8f8c:
  sound_b 0x1
script_4e8f90:
  task_callback 0x0
script_4e8f94:
  motion_block 0x0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0x1ca
script_4e8fa8:
  task_callback 0x0
script_4e8fac:
  handler 0x0, MK4_NATIVE_LANDING_MOTION
script_4e8fb4:
  clip 0x0, word(data_4fb424)
script_4e8fbc:
  bind_cursor 0x0, word(data_4fb418)
script_4e8fc4:
  native_context 0x0
script_4e8fc8:
  clear_handler 0x0
script_4e8fcc:
  open_clip 0x0, word(data_4fb430)
script_4e8fd4:
  cursor_set 0xc
script_4e8fd8:
  forward 0x0
script_4e8fdc:
  tail 0x0, MK4_NATIVE_GETUP_BACK
  .zero 4

MK4_SCRIPT_GROUNDED_TUMBLE_HOLD_RETURN:
  effect 0x0
script_4e8fec:
  reaction_context 0x0
script_4e8ff0:
  spawn_effect 0x0
script_4e8ff4:
  sound_context 0x6
script_4e8ff8:
  sound_b 0x1
script_4e8ffc:
  task_callback 0x0
script_4e9000:
  motion_block 0x0, 0xfffff5c3, 0xa3d, 0xffffeb86, 0xde
script_4e9014:
  handler 0x0, MK4_NATIVE_LANDING_MOTION
script_4e901c:
  clip 0x0, word(data_4fb424)
script_4e9024:
  clip 0x0, word(data_4fb418)
script_4e902c:
  play_clip 0xc, word(data_4fb430)
script_4e9034:
  clear_handler 0x0
script_4e9038:
  forward 0x0
script_4e903c:
  wait 0x14
script_4e9040:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
MK4_SCRIPT_TUMBLE_NATIVE_FINISH_BACK_GETUP:
  reaction_context 0x0
script_4e904c:
  sound_c 0x8
script_4e9050:
  sound_b 0x1
script_4e9054:
  task_callback 0x0
script_4e9058:
  motion_block 0x0, 0xfffff5c3, 0xfffff5c3, 0xffffeb86, 0xde
script_4e906c:
  effect 0x2
script_4e9070:
  effect 0x8
script_4e9074:
  effect 0x8
script_4e9078:
  handler 0x0, MK4_NATIVE_LANDING_MOTION
script_4e9080:
  clip 0x0, word(data_4fb424)
script_4e9088:
  clip 0x0, word(data_4fb418)
script_4e9090:
  play_clip 0xc, word(data_4fb430)
script_4e9098:
  clear_handler 0x0
script_4e909c:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_REACTION_CLIP_132_NATIVE_FINISH:
  effect 0x0
script_4e90b4:
  sound_context 0x6
script_4e90b8:
  sound_b 0x1
script_4e90bc:
  speed_negative 0x0, 0x11eb
script_4e90c4:
  play_clip 0xe, word(data_4fb3ac)
script_4e90cc:
  sound_a 0x4
script_4e90d0:
  landing 0x0
script_4e90d4:
  tail 0x0, MK4_NATIVE_REACTION_GETUP_BACK
  .zero 4

MK4_SCRIPT_STAGGER_CLIP_311_HOLD:
  speed_negative 0x0, 0x2666
script_4e90e8:
  sound_context 0x11
script_4e90ec:
  sound_b 0x3
script_4e90f0:
  bind_submit 0x4, word(data_4fbc10)
script_4e90f8:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9100:
  wait 0xd
script_4e9104:
  forward 0x0
script_4e9108:
  ready 0x0
  .zero 4

MK4_SCRIPT_SONYA_CROUCH_HK_VICTIM:
  speed_negative 0x0, 0x3333
script_4e9118:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9120:
  clip 0x0, word(data_4fb3c4)
script_4e9128:
  ready 0x0
  .zero 4

MK4_SCRIPT_STAGGER_CLIP_134_FRICTION:
  speed_negative 0x0, 0x1eb8
script_4e9138:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9140:
  clip 0x0, word(data_4fb3c4)
script_4e9148:
  ready 0x0
  .zero 4

MK4_SCRIPT_STAGGER_CLIP_134_FRICTION_IMPACT:
  sound_b 0x9
script_4e9154:
  speed_negative 0x0, 0x1eb8
script_4e915c:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9164:
  clip 0x0, word(data_4fb3c4)
script_4e916c:
  ready 0x0
MK4_SCRIPT_CROUCH_STAGGER_CLIP_254:
  speed_negative 0x0, 0x1eb8
script_4e9178:
  sound_b 0x9
script_4e917c:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9184:
  prefixed_finish 0x200, word(data_4fb964)
script_4e918c:
  wait 0x4
script_4e9190:
  playback_pair 0x0
script_4e9194:
  tail 0x0, MK4_NATIVE_CROUCH_RETURN
  .zero 4

MK4_SCRIPT_REAR_HIT_LAUNCH_CLIP_129:
  sound_a 0x16
script_4e91a4:
  motion 0x0, 0xffffd99a, 0xffffcccd, 0x418
script_4e91b4:
  tail 0x0, MK4_NATIVE_REACTION_CLIP_129_STARTUP
  .zero 4

MK4_SCRIPT_REACTION_CLIP_129_STARTUP:
  play_clip 0x22, word(data_4fb388)
script_4e91c8:
  tail 0x0, MK4_NATIVE_REACTION_CLIP_129_WAIT_LANDING
MK4_SCRIPT_REACTION_CLIP_129_WAIT_LANDING:
  native_context 0x0
script_4e91d4:
  sound_a 0x17
script_4e91d8:
  landing 0x0
script_4e91dc:
  tail 0x0, MK4_NATIVE_REACTION_CLIP_129_GETUP
script_4e91e4:
  nop 0x0
script_4e91e8:
  forward 0x0
script_4e91ec:
  wait 0x10
script_4e91f0:
  call 0x0, MK4_NATIVE_FACE_PEER_ORIENT_GROUP
script_4e91f8:
  tail 0x0, MK4_NATIVE_GETUP_BACK
  .zero 4

MK4_SCRIPT_KICK_HIT_STANDING_STAGGER:
  effect 0x4
script_4e920c:
  sound_b 0x3
script_4e9210:
  sound_context 0x1
script_4e9214:
  speed_negative 0x0, 0x2666
script_4e921c:
  bind_submit 0x4, word(data_4fbc10)
script_4e9224:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e922c:
  wait 0xd
script_4e9230:
  forward 0x0
script_4e9234:
  ready 0x0
MK4_SCRIPT_KICK_HIT_CROUCHING_STAGGER:
  no_guard 0x0
script_4e923c:
  effect 0x4
script_4e9240:
  spawn_effect 0x1
script_4e9244:
  sound_b 0x3
script_4e9248:
  bind_submit 0x4, word(data_4fbc10)
script_4e9250:
  wait 0xd
script_4e9254:
  forward 0x0
script_4e9258:
  ready 0x0
  .zero 4

MK4_SCRIPT_STAGGER_CLIP_134_SHAKE:
  no_guard 0x0
script_4e9264:
  speed_negative 0x0, 0x51e
script_4e926c:
  spawn_effect 0x1
script_4e9270:
  sound_context 0x3
script_4e9274:
  sound_b 0x2
script_4e9278:
  open_clip 0x0, word(data_4fb3c4)
script_4e9280:
  forward_count 0x6
script_4e9284:
  sound_indexed 0xa
script_4e9288:
  playback_native 0x0
script_4e928c:
  wait 0x2
script_4e9290:
  playback_native 0x0
script_4e9294:
  wait 0x2
script_4e9298:
  playback_native 0x0
script_4e929c:
  wait 0x2
script_4e92a0:
  playback_native 0x0
script_4e92a4:
  wait 0x2
script_4e92a8:
  forward 0x0
script_4e92ac:
  ready 0x0
  .zero 4

MK4_SCRIPT_KAI_HANDSTAND_REACTION_STANDING_STAGGER:
  effect 0xa
script_4e930c:
  sound_b 0x2
script_4e9310:
  sound_context 0x3
script_4e9314:
  speed_negative 0x0, 0x2666
script_4e931c:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9324:
  open_clip 0x0, word(data_4fb3c4)
script_4e932c:
  forward_count 0x6
script_4e9330:
  playback_native 0x0
script_4e9334:
  wait 0x2
script_4e9338:
  playback_native 0x0
script_4e933c:
  wait 0x2
script_4e9340:
  playback_native 0x0
script_4e9344:
  wait 0x2
script_4e9348:
  playback_native 0x0
script_4e934c:
  wait 0x2
script_4e9350:
  forward 0x0
script_4e9354:
  ready 0x0
  .zero 4

MK4_SCRIPT_CROUCH_REACTION_SCRIPT:
  sound_a 0x14
script_4e9364:
  speed_negative 0x0, 0x2b85
script_4e936c:
  prefixed_to 0x204, word(data_4fb964)
script_4e9374:
  playback_pair 0x0
script_4e9378:
  landing 0x0
script_4e937c:
  tail 0x0, MK4_NATIVE_CROUCH_RETURN
  .zero 4

MK4_SCRIPT_KNOCKDOWN_STAGGER_CLIP_311:
  effect 0x4
script_4e938c:
  no_guard 0x0
script_4e9390:
  spawn_effect 0x1
script_4e9394:
  bind_submit 0x4, word(data_4fbc10)
script_4e939c:
  wait 0xd
script_4e93a0:
  forward 0x0
script_4e93a4:
  ready 0x0
MK4_SCRIPT_FORWARD_FALL_GROUNDED:
  effect 0xb
script_4e93ac:
  sound_b 0x3
script_4e93b0:
  peer_motion 0x0, 0xffffe667, 0x0, 0x0
script_4e93c0:
  clip 0x0, word(data_4fb634)
script_4e93c8:
  sound_special 0x0
script_4e93cc:
  landing 0x0
script_4e93d0:
  tail 0x0, MK4_NATIVE_GETUP_FRONT
MK4_SCRIPT_WEAPON_CLIP_132_DIRECTIONAL_FALL:
  sound_b 0x1
script_4e93dc:
  effect 0x3
script_4e93e0:
  effect 0x8
script_4e93e4:
  effect 0x8
script_4e93e8:
  airborne 0x0, word(script_4e9400)
script_4e93f0:
  peer_motion 0x0, 0xffffe667, 0x0, 0x0
script_4e9400:
  task_callback 0x0
script_4e9404:
  peer_motion 0x0, 0xffffe667, 0x666, 0x147
script_4e9414:
  play_clip 0xd, word(data_4fb3ac)
script_4e941c:
  native_context 0x0
script_4e9420:
  tail 0x0, 0x482f90
  .zero 4

MK4_SCRIPT_FORWARD_FALL_AIRBORNE:
  effect 0xa
script_4e9434:
  motion 0x0, 0xffffe667, 0x0, 0x28f
script_4e9444:
  play_clip 0xe, word(data_4fb634)
script_4e944c:
  native_context 0x0
script_4e9450:
  forward 0x0
script_4e9454:
  tail 0x0, MK4_NATIVE_GETUP_FRONT
  .zero 4

MK4_SCRIPT_SHINNOK_WEAPON_REACTION_SWEEP_VICTIM:
  sound_b 0x0
script_4e9464:
  reaction_context 0x0
script_4e9468:
  motion_block 0x0, 0xfffff0a4, 0xfffff334, 0x0, 0x0
script_4e947c:
  clip 0x0, word(data_4fb6a0)
script_4e9484:
  sound_special 0x0
script_4e9488:
  landing 0x0
script_4e948c:
  tail 0x0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_GETUP
  .zero 4

MK4_SCRIPT_SHINNOK_WEAPON_SWEEP_GETUP:
  down 0x0, word(script_4ea4d0)
script_4e94a0:
  open_clip 0x0, word(data_4fb6b8)
script_4e94a8:
  tail 0x0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH
MK4_SCRIPT_SWEEP_VICTIM_RECOVERY:
  sound_b 0x0
script_4e94b4:
  reaction_context 0x0
script_4e94b8:
  motion_block 0x0, 0xfffff0a4, 0xccc, 0x0, 0x0
script_4e94cc:
  play_clip 0x18, word(data_4fb67c)
script_4e94d4:
  sound_special 0x0
script_4e94d8:
  landing 0x0
script_4e94dc:
  forward 0x0
script_4e94e0:
  down 0x0, word(script_4e94f8)
script_4e94e8:
  open_clip 0x0, word(data_4fb694)
script_4e94f0:
  tail 0x0, MK4_NATIVE_SHINNOK_WEAPON_SWEEP_FINISH
script_4e94f8:
  clip 0x0, word(data_4fb688)
script_4e9500:
  tail 0x0, MK4_NATIVE_CROUCH_RETURN
MK4_SCRIPT_TUMBLE_CAMERA_SIDE_POSITIVE:
  reaction_context 0x0
script_4e950c:
  sound_context 0x1
script_4e9510:
  spawn_effect 0x0
script_4e9514:
  sound_b 0x3
script_4e9518:
  effect 0x0
script_4e951c:
  task_callback 0x0
script_4e9520:
  motion_block 0x0, 0xfffff334, 0xccc, 0xffffeb86, 0xde
script_4e9534:
  handler 0x0, MK4_NATIVE_LANDING_MOTION
script_4e953c:
  clip 0x0, word(data_4fb424)
script_4e9544:
  clip 0x0, word(data_4fb418)
script_4e954c:
  play_clip 0xc, word(data_4fb430)
script_4e9554:
  clear_handler 0x0
script_4e9558:
  forward 0x0
script_4e955c:
  wait 0x8
script_4e9560:
  tail 0x0, MK4_NATIVE_GETUP_BACK
MK4_SCRIPT_TUMBLE_CAMERA_SIDE_NEGATIVE:
  reaction_context 0x0
script_4e956c:
  sound_context 0x1
script_4e9570:
  spawn_effect 0x0
script_4e9574:
  sound_b 0x3
script_4e9578:
  effect 0x0
script_4e957c:
  task_callback 0x0
script_4e9580:
  motion_block 0x0, 0xfffff334, 0xfffff334, 0xffffeb86, 0xde
script_4e9594:
  handler 0x0, MK4_NATIVE_LANDING_MOTION
script_4e959c:
  clip 0x0, word(data_4fb424)
script_4e95a4:
  clip 0x0, word(data_4fb418)
script_4e95ac:
  play_clip 0xc, word(data_4fb430)
script_4e95b4:
  clear_handler 0x0
script_4e95b8:
  forward 0x0
script_4e95bc:
  wait 0x8
script_4e95c0:
  tail 0x0, MK4_NATIVE_GETUP_BACK
MK4_SCRIPT_GORO_QUAKE_VICTIM:
  sound_b 0x1
script_4e95cc:
  speed_negative 0x0, 0x1999
script_4e95d4:
  open_clip 0x0, word(data_4fb604)
script_4e95dc:
  double_to 0x26
script_4e95e0:
  landing 0x0
script_4e95e4:
  double_to 0x32
script_4e95e8:
  ready 0x0
  .zero 4

MK4_SCRIPT_REPTILE_DASH_VICTIM:
  sound_b 0x0
script_4e95f4:
  motion 0x0, 0xa3d, 0xffffcf5d, 0x28f
script_4e9604:
  task_callback 0x0
script_4e9608:
  play_clip 0x27, word(data_4fb5e0)
script_4e9610:
  native_context 0x0
script_4e9614:
  forward 0x0
script_4e9618:
  tail 0x0, MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP
MK4_SCRIPT_REPTILE_REACTION_40:
  effect 0x11
script_4e9624:
  spawn_effect 0x0
script_4e9628:
  sound_context 0x1
script_4e962c:
  motion 0x0, 0xffffeb86, 0xffffe667, 0x24d
script_4e963c:
  task_callback 0x0
script_4e9640:
  clip 0x0, word(data_4fb610)
script_4e9648:
  native_context 0x0
script_4e964c:
  forward 0x0
script_4e9650:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_JAREK_BALL_VICTIM:
  sound_context 0x0
script_4e965c:
  spawn_effect 0x6
script_4e9660:
  motion 0x0, 0xffffe667, 0xffffe667, 0x24d
script_4e9670:
  task_callback 0x0
script_4e9674:
  clip 0x0, word(data_4fb610)
script_4e967c:
  native_context 0x0
script_4e9680:
  forward 0x0
script_4e9684:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_LAUNCH_CLIP_131_SHAKE_IMPACT:
  sound_context 0x2
script_4e9694:
  spawn_effect 0x6
script_4e9698:
  sound_b 0x3
script_4e969c:
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
script_4e96ac:
  task_callback 0x0
script_4e96b0:
  play_clip 0x28, word(data_4fb3a0)
script_4e96b8:
  native_context 0x0
script_4e96bc:
  forward 0x0
script_4e96c0:
  ready 0x0
  .zero 4

MK4_SCRIPT_FALL_REACTION_EFFECTS:
  sound_context 0x6
script_4e96cc:
  spawn_effect 0x0
script_4e96d0:
  effect 0x8
script_4e96d4:
  tail 0x0, MK4_NATIVE_REACTION_FALL
  .zero 4

MK4_SCRIPT_REACTION_FALL:
  effect 0xc
script_4e96e4:
  sound_context 0x0
script_4e96e8:
  sound_b 0x3
script_4e96ec:
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
script_4e96fc:
  task_callback 0x0
script_4e9700:
  play_clip 0x28, word(data_4fb3a0)
script_4e9708:
  native_context 0x0
script_4e970c:
  tail 0x0, MK4_NATIVE_REACTION_FINISH
  .zero 4

MK4_SCRIPT_CAGE_SHADOW_VICTIM:
  effect 0x11
script_4e971c:
  sound_b 0x3
script_4e9720:
  spawn_effect 0x0
script_4e9724:
  sound_context 0x0
script_4e9728:
  motion 0x0, 0xffffe667, 0xffffdc29, 0x28f
script_4e9738:
  task_callback 0x0
script_4e973c:
  clip 0x0, word(data_4fb610)
script_4e9744:
  native_context 0x0
script_4e9748:
  forward 0x0
script_4e974c:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_UPPERCUT_VICTIM_RECOVERY:
  sound_b 0x1
script_4e975c:
  spawn_effect 0x0
script_4e9760:
  sound_context 0x0
script_4e9764:
  effect 0x0
script_4e9768:
  effect 0x8
script_4e976c:
  motion 0x0, 0xfffff852, 0xffffcccd, 0x20c
script_4e977c:
  play_clip 0x35, word(data_4fb37c)
script_4e9784:
  native_context 0x0
script_4e9788:
  sound_a 0x17
script_4e978c:
  sound_special 0x0
script_4e9790:
  tail 0x0, MK4_NATIVE_UPPERCUT_RECOVER
MK4_SCRIPT_JAB_HIT_SHORT_RECOVERY:
  effect 0x6
script_4e979c:
  open_clip 0x0, word(data_4fbc10)
script_4e97a4:
  cursor_set 0x4
script_4e97a8:
  forward_to 0xc
script_4e97ac:
  prefixed_sample 0xe
script_4e97b0:
  wait 0x1
script_4e97b4:
  prefixed_sample 0x10
script_4e97b8:
  wait 0x1
script_4e97bc:
  prefixed_sample 0x12
script_4e97c0:
  wait 0x1
script_4e97c4:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4

MK4_SCRIPT_JAB_HIT_DAMAGE_STAGGER:
  speed_negative 0x0, 0x1999
script_4e97d8:
  open_clip 0x0, word(data_4fb604)
script_4e97e0:
  double_to 0x26
script_4e97e4:
  landing 0x0
script_4e97e8:
  double_to 0x32
script_4e97ec:
  ready 0x0
  .zero 4

MK4_SCRIPT_STAGGER_ENTRY:
  sound_b 0x1
script_4e97fc:
  speed_negative 0x0, 0x3333
script_4e9804:
  open_clip 0x0, word(data_4fb604)
script_4e980c:
  tail 0x0, MK4_NATIVE_STAGGER_TAIL
  .zero 4

MK4_SCRIPT_QUAN_CHI_WEAPON_STEAL_GROUNDED_VICTIM:
  speed_negative 0x0, 0xccc
script_4e9820:
  prefixed_to 0x219, word(data_4fb604)
script_4e9828:
  landing 0x0
script_4e982c:
  forward 0x0
script_4e9830:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
MK4_SCRIPT_QUAN_CHI_WEAPON_STEAL_AIRBORNE_VICTIM:
  motion 0x0, 0xffffee15, 0xffffdc29, 0x28f
script_4e9848:
  task_callback 0x0
script_4e984c:
  clip 0x0, word(data_4fb610)
script_4e9854:
  native_context 0x0
script_4e9858:
  forward 0x0
script_4e985c:
  wait 0x2
script_4e9860:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_QUAN_CHI_STOMP_REACTION:
  sound_context 0xd
script_4e986c:
  sound_b 0x1
script_4e9870:
  prefixed_finish 0x200, word(data_4fb5d4)
script_4e9878:
  tail 0x0, MK4_NATIVE_QUAN_CHI_SCRIPT_STOMP_VICTIM
MK4_SCRIPT_SLAM_VICTIM:
  speed_negative 0x0, 0xffffeb86
script_4e9888:
  sound_special 0x0
script_4e988c:
  prefixed_finish 0x200, word(data_4fb5b0)
script_4e9894:
  wait 0x8
script_4e9898:
  ready 0x0
  .zero 4

MK4_SCRIPT_AIR_VICTIM_FLIP:
  prefixed_sample 0x200, word(data_4fb6c4)
script_4e98a8:
  motion 0x0, 0x0, 0x1999, 0x28f
script_4e98b8:
  native_context 0x0
script_4e98bc:
  call 0x0, MK4_NATIVE_TOUCHABLE
script_4e98c4:
  call 0x0, MK4_NATIVE_DISARM_SELF
script_4e98cc:
  sound_special 0x0
script_4e98d0:
  prefixed_to 0x20c, word(data_4fb6c4)
script_4e98d8:
  speed_negative 0x0, 0xccc
script_4e98e0:
  tail 0x0, MK4_NATIVE_AIR_FLIP_GETUP
MK4_SCRIPT_AIR_VICTIM_LIFT:
  motion 0x0, 0xffffe667, 0xffffd99a, 0x28f
script_4e98f8:
  task_callback 0x0
script_4e98fc:
  prefixed_to 0x223, word(data_4fb40c)
script_4e9904:
  native_context 0x0
script_4e9908:
  tail 0x0, MK4_NATIVE_AIR_LIFT_GETUP
MK4_SCRIPT_AIR_VICTIM_DEFAULT:
  motion 0x0, 0x0, 0x3333, 0x28f
script_4e9920:
  native_context 0x0
script_4e9924:
  sound_special 0x0
script_4e9928:
  forward 0x0
script_4e992c:
  wait 0x4
script_4e9930:
  tail 0x0, MK4_NATIVE_MMOVE_AIR_GETUP
MK4_SCRIPT_SCORPION_TELEPORT_PUNCH_GROUNDED_VICTIM:
  effect 0x9
script_4e993c:
  effect 0x8
script_4e9940:
  spawn_effect 0x0
script_4e9944:
  sound_context 0x1
script_4e9948:
  sound_b 0x3
script_4e994c:
  speed_negative 0x0, 0x2666
script_4e9954:
  bind_submit 0x4, word(data_4fbc10)
script_4e995c:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9964:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_TELEPORT_VICTIM_TAIL
  .zero 4

MK4_SCRIPT_SCORPION_TELEPORT_VICTIM_TAIL:
  wait 0xd
script_4e9974:
  forward 0x0
script_4e9978:
  ready 0x0
  .zero 4

MK4_SCRIPT_TELEPORT_PUNCH_AIRBORNE_VICTIM:
  effect 0x9
script_4e9984:
  effect 0x8
script_4e9988:
  sound_context 0x1
script_4e998c:
  sound_b 0x3
script_4e9990:
  motion 0x0, 0xfffff852, 0xffffdc29, 0x28f
script_4e99a0:
  task_callback 0x0
script_4e99a4:
  clip 0x0, word(data_4fb610)
script_4e99ac:
  native_context 0x0
script_4e99b0:
  forward 0x0
script_4e99b4:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_VICTIM_DISARM_FALL:
  forward_count 0x8
script_4e99c4:
  speed_negative 0x0, 0x147a
script_4e99cc:
  forward_count 0x17
script_4e99d0:
  landing 0x0
script_4e99d4:
  forward 0x0
script_4e99d8:
  tail 0x0, MK4_NATIVE_THROW_VICTIM_STOP_AND_RETURN
MK4_SCRIPT_VICTIM_FRICTION_IMPACT:
  prefixed_finish 0x200, word(MK4_ANIMATION_DESCRIPTOR_CLIP_168)
script_4e99e8:
  ready 0x0
  .zero 4

MK4_SCRIPT_VICTIM_SAVED_MOTION:
  spawn_effect 0x0
script_4e99f4:
  prefixed_to 0x20b, word(MK4_ANIMATION_DESCRIPTOR_CLIP_177)
script_4e99fc:
  call 0x0, MK4_NATIVE_RESTORE_MOTION
script_4e9a04:
  forward_to 0x22
script_4e9a08:
  sound_special 0x0
script_4e9a0c:
  landing 0x0
script_4e9a10:
  forward 0x0
script_4e9a14:
  wait 0x6
script_4e9a18:
  tail 0x0, MK4_NATIVE_JAX_GROUND_POUND_CONTROLLER_RETURN
MK4_SCRIPT_VICTIM_GROUND_TURN:
  speed_negative 0x0, 0x2666
script_4e9a28:
  spawn_effect 0x6
script_4e9a2c:
  sound_a 0x3
script_4e9a30:
  prefixed_finish 0x200, word(data_4fb574)
script_4e9a38:
  call 0x0, MK4_NATIVE_IMPACT_VOICE
script_4e9a40:
  prefixed_finish 0x200, word(data_4fb3f4)
script_4e9a48:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9a50:
  prefixed_finish 0x200, word(data_4fb400)
script_4e9a58:
  wait 0xa
script_4e9a5c:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_SONYA_LEG_GRAB_VICTIM_RECOVERY:
  motion 0x0, 0x1c28, 0xffffe667, 0x24d
script_4e9a78:
  sound_special 0x0
script_4e9a7c:
  task_callback 0x0
script_4e9a80:
  forward 0x0
script_4e9a84:
  tail 0x0, MK4_NATIVE_SONYA_SCRIPT_LEG_VICTIM_END
  .zero 4

MK4_SCRIPT_LIU_KANG_LK_THROW_VICTIM:
  sound_a 0x25
script_4e9a94:
  forward_to 0x23
script_4e9a98:
  loop 0x3
script_4e9a9c:
  playback_pair 0x1f
script_4e9aa0:
  forward_to 0x23
script_4e9aa4:
  repeat 0x0
script_4e9aa8:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_LK_THROWN_TAIL
  .zero 4

MK4_SCRIPT_RAIDEN_TORPEDO_GROUND_RELEASE:
  speed_negative 0x0, 0x4ccc
script_4e9ad8:
  clip 0x0, word(data_4fb604)
script_4e9ae0:
  ready 0x0
  .zero 4

MK4_SCRIPT_RAIDEN_TORPEDO_AIR_RELEASE:
  motion 0x0, 0xfffff334, 0xfffff0a4, 0x312
script_4e9af8:
  task_callback 0x0
script_4e9afc:
  clip 0x0, word(data_4fb610)
script_4e9b04:
  native_context 0x0
script_4e9b08:
  forward 0x0
script_4e9b0c:
  wait 0x2
script_4e9b10:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_LP_VICTIM_SLIDE_RELEASE:
  prefixed_to 0x205, word(data_4fb580)
script_4e9b20:
  speed_negative 0x0, 0x1999
script_4e9b28:
  forward_to 0x18
script_4e9b2c:
  landing 0x0
script_4e9b30:
  forward 0x0
script_4e9b34:
  tail 0x0, 0x485a70
  .zero 4

MK4_SCRIPT_LP_VICTIM_SLIDING_IMPACT_RELEASE:
  speed_negative 0x0, 0x2666
script_4e9b48:
  prefixed_to 0x20c, word(data_4fb58c)
script_4e9b50:
  sound_special 0x0
script_4e9b54:
  forward_count 0x6
script_4e9b58:
  landing 0x0
script_4e9b5c:
  forward 0x0
script_4e9b60:
  tail 0x0, MK4_NATIVE_LP_SLIDING_IMPACT_FLIP_GETUP
MK4_SCRIPT_LP_VICTIM_IMPACT_RELEASE:
  prefixed_to 0x211, word(data_4fb5a4)
script_4e9b70:
  sound_special 0x0
script_4e9b74:
  forward 0x0
script_4e9b78:
  landing 0x0
script_4e9b7c:
  tail 0x0, MK4_NATIVE_GETUP_ROLL_FLIP
  .zero 4

MK4_SCRIPT_LP_VICTIM_TWO_SPEED_SLIDE_RELEASE:
  prefixed_to 0x204, word(data_4fb5bc)
script_4e9b90:
  speed_negative 0x0, 0x3333
script_4e9b98:
  forward_count 0x8
script_4e9b9c:
  speed_negative 0x0, 0x1999
script_4e9ba4:
  forward_count 0x8
script_4e9ba8:
  landing 0x0
script_4e9bac:
  forward 0x0
script_4e9bb0:
  tail 0x0, MK4_NATIVE_LP_TWO_SPEED_SLIDE_DOUBLE_FLIP_ROLL_GETUP
  .zero 4

MK4_SCRIPT_HIT_CLIP_324_RECOVERY:
  effect 0x9
script_4e9c2c:
  sound_a 0x14
script_4e9c30:
  clip 0x0, word(data_4fbcac)
script_4e9c38:
  ready 0x0
  .zero 4

MK4_SCRIPT_HIT_CLIP_325_RECOVERY:
  effect 0x9
script_4e9c44:
  sound_a 0x14
script_4e9c48:
  clip 0x0, word(data_4fbcb8)
script_4e9c50:
  ready 0x0
  .zero 4

MK4_SCRIPT_LAUNCH_CLIP_131_NO_IMPACT:
  motion 0x0, 0xffffe667, 0xffffdc29, 0x24d
script_4e9c68:
  task_callback 0x0
script_4e9c6c:
  play_clip 0x28, word(data_4fb3a0)
script_4e9c74:
  native_context 0x0
script_4e9c78:
  forward 0x0
script_4e9c7c:
  ready 0x0
MK4_SCRIPT_GROUNDED_HIT_CLIP_132_BACK_GETUP:
  sound_b 0x1
script_4e9c84:
  speed_negative 0x0, 0xa3d
script_4e9c8c:
  open_clip 0x0, word(data_4fb3ac)
script_4e9c94:
  double_to 0xe
script_4e9c98:
  landing 0x0
script_4e9c9c:
  forward 0x0
script_4e9ca0:
  tail 0x0, MK4_NATIVE_GETUP_BACK
MK4_SCRIPT_LAUNCH_CLIP_179_FLIP_GETUP:
  sound_b 0x3
script_4e9cac:
  motion 0x0, 0x51e, 0xffffb334, 0x353
script_4e9cbc:
  task_callback 0x0
script_4e9cc0:
  play_clip 0x27, word(data_4fb5e0)
script_4e9cc8:
  native_context 0x0
script_4e9ccc:
  forward 0x0
script_4e9cd0:
  tail 0x0, MK4_NATIVE_SUB_ZERO_LP_THROW_VICTIM_GETUP
MK4_SCRIPT_QUAN_CHI_SKULL_REACTION:
  sound_c 0x2f
script_4e9cdc:
  sound_b 0x3
script_4e9ce0:
  speed_negative 0x0, 0x11eb
script_4e9ce8:
  open_clip 0x0, word(data_4fb604)
script_4e9cf0:
  native_sound 0x0
script_4e9cf4:
  landing 0x0
script_4e9cf8:
  ready 0x0
  .zero 4

MK4_SCRIPT_SCORPION_SPEAR_VICTIM_HIT:
  effect 0x5
script_4e9d04:
  sound_b 0x3
script_4e9d08:
  sound_context 0xa
script_4e9d0c:
  play_clip 0x5, word(data_4fb6d0)
script_4e9d14:
  tail 0x0, MK4_NATIVE_SHAKE_STUN
  .zero 4

MK4_SCRIPT_GUARD_GROUNDED_STAGGER:
  speed_negative 0x0, 0xccc
script_4e9d28:
  prefixed_finish 0x200, word(data_4fb604)
script_4e9d30:
  ready 0x0
  .zero 4

AIRBORNE_THAW_SCRIPT:
  gravity 0x0, 0x147
script_4e9d40:
  native_context 0x0
script_4e9d44:
  tail 0x0, MK4_NATIVE_THAW_LANDING
  .zero 4

MK4_SCRIPT_RAIDEN_LIGHTNING_VICTIM:
  sound_a 0x2d
script_4e9d54:
  speed_negative 0x0, 0x5999
script_4e9d5c:
  play_clip 0x1b, word(data_4fb370)
script_4e9d64:
  clear_handler 0x0
script_4e9d68:
  speed_negative 0x0, 0xffffeb86
script_4e9d70:
  forward_count 0x5
script_4e9d74:
  landing 0x0
script_4e9d78:
  forward 0x0
script_4e9d7c:
  ready 0x0
  .zero 4

MK4_SCRIPT_REACTION_CLIP_127_FRICTION:
  sound_a 0x26
script_4e9d8c:
  speed_negative 0x0, 0x5999
script_4e9d94:
  play_clip 0x1b, word(data_4fb370)
script_4e9d9c:
  clear_handler 0x0
script_4e9da0:
  speed_negative 0x0, 0xffffeb86
script_4e9da8:
  forward_count 0x5
script_4e9dac:
  landing 0x0
script_4e9db0:
  forward 0x0
script_4e9db4:
  ready 0x0
MK4_SCRIPT_TANYA_WEAPON_REACTION_REACTION_169_VICTIM:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9dc0:
  effect 0x4
script_4e9dc4:
  spawn_effect 0x0
script_4e9dc8:
  sound_context 0x1
script_4e9dcc:
  sound_b 0x1
script_4e9dd0:
  speed_negative 0x0, 0x4000
script_4e9dd8:
  clip 0x0, word(data_4fb394)
script_4e9de0:
  ready 0x0
  .zero 4

MK4_SCRIPT_SONYA_RING_VICTIM:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9df0:
  sound_b 0x1
script_4e9df4:
  speed_negative 0x0, 0x4000
script_4e9dfc:
  clip 0x0, word(data_4fb394)
script_4e9e04:
  ready 0x0
  .zero 4

MK4_SCRIPT_FUJIN_BOLT_VICTIM:
  speed_negative 0x0, 0x2666
script_4e9e18:
  bind_submit 0x4, word(data_4fbc10)
script_4e9e20:
  handler 0x0, MK4_NATIVE_VELOCITY_FRICTION
script_4e9e28:
  wait 0x4
script_4e9e2c:
  forward 0x0
script_4e9e30:
  ready 0x0
  .zero 4

MK4_SCRIPT_JAX_DASH_PUNCH_GROUNDED_VICTIM_168:
  effect 0x9
script_4e9e3c:
  sound_b 0x1
script_4e9e40:
  speed_negative 0x0, 0x6666
script_4e9e48:
  clip 0x0, word(data_4fb394)
script_4e9e50:
  ready 0x0
  .zero 4

MK4_SCRIPT_BLOCK_UNARMED_POSE_CYCLE:
  bind_submit 0x3, word(data_4fbc88)
script_4e9e70:
  wait 0x1
script_4e9e74:
  bind_submit 0x5, word(data_4fbc88)
script_4e9e7c:
  wait 0x1
script_4e9e80:
  bind_submit 0x7, word(data_4fbc88)
script_4e9e88:
  wait 0x1
script_4e9e8c:
  bind_submit 0x9, word(data_4fbc88)
script_4e9e94:
  wait 0x1
script_4e9e98:
  bind_submit 0x7, word(data_4fbc88)
script_4e9ea0:
  wait 0x1
script_4e9ea4:
  bind_submit 0x5, word(data_4fbc88)
script_4e9eac:
  wait 0x1
script_4e9eb0:
  bind_submit 0x3, word(data_4fbc88)
script_4e9eb8:
  wait 0x1
script_4e9ebc:
  tail 0x0, MK4_NATIVE_BLOCK_CYCLE
  .zero 4

MK4_SCRIPT_BLOCK_ARMED_RECOVERY:
  prefixed_finish 0x200, word(data_4fb910)
script_4e9ed0:
  playback_pair 0x0
script_4e9ed4:
  landing 0x0
script_4e9ed8:
  tail 0x0, MK4_NATIVE_STANDING_BLOCK_HOLD
MK4_SCRIPT_LK_THROWN_RELEASE:
  cursor_set 0x2d
script_4e9ee4:
  playback_pair 0x29
script_4e9ee8:
  forward_to 0x2e
script_4e9eec:
  playback_pair 0x29
script_4e9ef0:
  forward_to 0x2e
script_4e9ef4:
  playback_pair 0x29
script_4e9ef8:
  forward_to 0x2e
script_4e9efc:
  tail 0x0, MK4_NATIVE_SUB_ZERO_SCRIPT_LK_VICTIM_RISE
  .zero 4

MK4_SCRIPT_SNARE_REACTION:
  open_clip 0x6, word(data_4fb6d0)
script_4e9f10:
  forward_to 0xb
script_4e9f14:
  playback_pair 0x7
script_4e9f18:
  jump 0x0, word(script_4e9f10)
MK4_SCRIPT_SPEAR_ARRIVAL:
  bind_cursor 0x0, word(data_4fae18)
script_4e9f28:
  tail 0x0, MK4_NATIVE_SCORPION_SPEAR_STUN_SCRIPT
  .zero 4

MK4_SCRIPT_PAIR_RELEASE:
  motion 0x0, 0xfffffae2, 0x0, 0x28f
script_4e9f58:
  task_callback 0x0
script_4e9f5c:
  prefixed_finish 0x200, word(data_4fb610)
script_4e9f64:
  native_context 0x0
script_4e9f68:
  forward 0x0
script_4e9f6c:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_GROIN_PUNCH_GROIN_VICTIM:
  speed_negative 0x0, 0x28f5
script_4e9f80:
  spawn_effect 0x0
script_4e9f84:
  sound_context 0x12
script_4e9f88:
  sound_b 0x3
script_4e9f8c:
  prefixed_to 0x20e, word(data_4fb364)
script_4e9f94:
  playback_pair 0xa
script_4e9f98:
  forward_to 0xe
script_4e9f9c:
  playback_pair 0xa
script_4e9fa0:
  forward_to 0xe
script_4e9fa4:
  playback_pair 0xa
script_4e9fa8:
  forward_to 0xe
script_4e9fac:
  ready 0x0
MK4_SCRIPT_ORBIT_RELEASE:
  motion 0x0, 0xffffe667, 0xffffd99a, 0x353
script_4e9fc0:
  task_callback 0x0
script_4e9fc4:
  prefixed_finish 0x200, word(data_4fb610)
script_4e9fcc:
  native_context 0x0
script_4e9fd0:
  forward 0x0
script_4e9fd4:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_VORTEX_LANDED:
  sound_special 0x0
script_4e9fe4:
  jump 0x0, word(script_4ea4d0)
  .zero 4

MK4_SCRIPT_SLAM_LANDED:
  sound_special 0x0
script_4e9ff4:
  wait 0x4
script_4e9ff8:
  motion 0x0, 0x11eb, 0xffffcf5d, 0x28f
script_4ea008:
  task_callback 0x0
script_4ea00c:
  play_clip 0x27, word(data_4fb5e0)
script_4ea014:
  native_context 0x0
script_4ea018:
  forward 0x0
script_4ea01c:
  tail 0x0, MK4_NATIVE_GETUP_BACK
  .zero 4

MK4_SCRIPT_JAX_DASH_VICTIM_HIT:
  effect 0x9
script_4ea02c:
  sound_context 0x1
script_4ea030:
  prefixed_finish 0x200, word(data_4fbc10)
script_4ea038:
  ready 0x0
  .zero 4

MK4_SCRIPT_CAGE_POWER_VICTIM:
  sound_context 0x3
script_4ea044:
  play_clip 0x1b, word(data_4fb370)
script_4ea04c:
  ready 0x0
MK4_SCRIPT_FUJIN_TRIPLE_VICTIM:
  effect 0xa
script_4ea054:
  sound_context 0x3
script_4ea058:
  bind_submit 0x4, word(data_4fbc10)
script_4ea060:
  wait 0xd
script_4ea064:
  ready 0x0
MK4_SCRIPT_RAIDEN_STAFF_HP_STAFF_VICTIM:
  sound_b 0x1
script_4ea06c:
  sound_context 0x2
script_4ea070:
  prefixed_to 0x205, word(data_4fb718)
script_4ea078:
  tail 0x0, MK4_NATIVE_RAIDEN_SCRIPT_STAFF_VICTIM_TAIL
  .zero 4

MK4_SCRIPT_SONYA_CARTWHEEL_VICTIM:
  sound_context 0x1
script_4ea0bc:
  sound_b 0x3
script_4ea0c0:
  motion 0x0, 0xffffe667, 0xffffd47b, 0x28f
script_4ea0d0:
  task_callback 0x0
script_4ea0d4:
  open_clip 0x0, word(data_4fb3a0)
script_4ea0dc:
  double_to 0xd
script_4ea0e0:
  forward_to 0x29
script_4ea0e4:
  nop 0x13ece8
script_4ea0e8:
  native_context 0x0
script_4ea0ec:
  forward_count 0x4
script_4ea0f0:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
MK4_SCRIPT_BICYCLE_CAPTURE:
  clip 0x0, word(data_4fb3dc)
script_4ea100:
  tail 0x0, MK4_NATIVE_BICYCLE_CYCLE
MK4_SCRIPT_RAIDEN_TORPEDO_CONTACT:
  effect 0x13
script_4ea10c:
  handler 0x0, MK4_NATIVE_RAIDEN_SCRIPT_TORPEDO_VICTIM_MOTION
script_4ea114:
  prefixed_finish 0x200, word(data_4fb3d0)
script_4ea11c:
  playback_pair 0x4
script_4ea120:
  forward_to 0x9
script_4ea124:
  jump 0x0, word(script_4ea11c)
  .zero 4

MK4_SCRIPT_SONYA_BICYCLE_VICTIM:
  effect 0x8
script_4ea134:
  effect 0x9
script_4ea138:
  sound_context 0x2
script_4ea13c:
  spawn_effect 0x0
script_4ea140:
  task_callback 0x0
script_4ea144:
  motion 0x0, 0xffffe667, 0x666, 0x147
script_4ea154:
  clip 0x0, word(data_4fb634)
script_4ea15c:
  native_context 0x0
script_4ea160:
  sound_special 0x0
script_4ea164:
  landing 0x0
script_4ea168:
  tail 0x0, MK4_NATIVE_GETUP_FRONT
MK4_SCRIPT_KAI_HANDSTAND_REACTION_KNOCKDOWN:
  effect 0x4
script_4ea174:
  sound_b 0x3
script_4ea178:
  sound_context 0x1
script_4ea17c:
  spawn_effect 0x0
script_4ea180:
  motion 0x0, 0xffffe667, 0xffffd47b, 0x28f
script_4ea190:
  task_callback 0x0
script_4ea194:
  open_clip 0x0, word(data_4fb3a0)
script_4ea19c:
  double_to 0xd
script_4ea1a0:
  forward_to 0x29
script_4ea1a4:
  nop 0x13ece8
script_4ea1a8:
  native_context 0x0
script_4ea1ac:
  forward_count 0x4
script_4ea1b0:
  tail 0x0, MK4_NATIVE_MMOVE_SIDE_GETUP
  .zero 4

MK4_SCRIPT_KAI_HANDSTAND_REACTION_REAR_STAGGER:
  effect 0xa
script_4ea1c4:
  sound_b 0x2
script_4ea1c8:
  sound_context 0x3
script_4ea1cc:
  speed_negative 0x0, 0x3333
script_4ea1d4:
  open_clip 0x0, word(data_4fb370)
script_4ea1dc:
  forward_count 0x14
script_4ea1e0:
  tail 0x0, MK4_NATIVE_KAI_HANDSTAND_VICTIM_TAIL
MK4_SCRIPT_MMOVE_SIDE_GETUP:
  open_clip 0x0, word(data_4fb3a0)
script_4ea1f0:
  cursor_set 0x2d
script_4ea1f4:
  forward 0x0
script_4ea1f8:
  park 0x0
  .zero 4

MK4_SCRIPT_KAI_SECOND_VICTIM:
  effect 0x0
script_4ea244:
  effect 0x8
script_4ea248:
  sound_c 0x69
script_4ea24c:
  spawn_effect 0x6
script_4ea250:
  sound_b 0x8
script_4ea254:
  sound_c 0x34
script_4ea258:
  prefixed_to 0x20a, word(data_4fb394)
script_4ea260:
  half_finish 0x0
script_4ea264:
  native_release 0x2f
script_4ea268:
  tail 0x0, MK4_NATIVE_KAI_SECOND_VICTIM_FALL
MK4_SCRIPT_SHINNOK_SECOND_VICTIM:
  prefixed_finish 0x200, word(data_4fc6e4)
script_4ea278:
  bind_cursor 0x0, word(data_4fc6f0)
script_4ea280:
  wait 0x32
script_4ea284:
  sound_b 0x8
script_4ea288:
  park 0x0
  .zero 4

MK4_SCRIPT_REPTILE_FATALITY_2_VICTIM_MELT:
  bind_cursor 0x0, word(data_4fc714)
script_4ea298:
  speed_negative 0x0, 0x28f
script_4ea2a0:
  tail 0x0, MK4_NATIVE_REPTILE_SECOND_MELT
MK4_SCRIPT_FUJIN_FATALITY_2_VICTIM_FALL:
  prefixed_to 0x228, word(data_4fc744)
script_4ea2b0:
  sound_special 0x0
script_4ea2b4:
  forward 0x0
script_4ea2b8:
  wait 0xa
script_4ea2bc:
  face 0x0
script_4ea2c0:
  scene_event 0x3f
script_4ea2c4:
  tail 0x0, MK4_NATIVE_SCENE_BODY_FINISH
  .zero 4

MK4_SCRIPT_FUJIN_SECOND_VICTIM:
  speed_negative 0x0, 0x28f
script_4ea2e0:
  bind_cursor 0x0, word(data_4fc738)
script_4ea2e8:
  wait 0x5a
script_4ea2ec:
  sound_b 0x8
script_4ea2f0:
  tail 0x0, MK4_NATIVE_FUJIN_SECOND_BURST
MK4_SCRIPT_REACTION_PARK_TASK:
  park 0x0
  .zero 4

MK4_SCRIPT_STAGE_VICTIM:
  spawn_effect 0x0
script_4ea304:
  sound_context 0x0
script_4ea308:
  motion_block 0x0, 0xfffffd71, 0x28f5, 0xffffcccd, 0x0
script_4ea31c:
  open_clip 0x0, word(data_4fb37c)
script_4ea324:
  double_to 0x1f
script_4ea328:
  tail 0x0, MK4_NATIVE_STAGE_IMPALE
MK4_SCRIPT_STAGE_IMPALED:
  landing 0x0
script_4ea334:
  effect 0x8
script_4ea338:
  effect 0x8
script_4ea33c:
  effect 0x14
script_4ea340:
  sound_context 0x7
script_4ea344:
  spawn_effect 0x6
script_4ea348:
  sound_b 0xc
script_4ea34c:
  wait 0x80
script_4ea350:
  native_release 0x29
script_4ea354:
  wait 0x32
script_4ea358:
  tail 0x0, MK4_NATIVE_STAGE_COMPLETE
  .zero 4

MK4_SCRIPT_VICTIM_DISARM_RAISE:
  speed_negative 0x0, 0x1c28
script_4ea3a0:
  sound_b 0x1
script_4ea3a4:
  spawn_effect 0x0
script_4ea3a8:
  sound_context 0x0
script_4ea3ac:
  open_clip 0x0, word(data_4fc69c)
script_4ea3b4:
  half_count 0x7
script_4ea3b8:
  tail 0x0, MK4_NATIVE_THROW_VICTIM_CONTINUE
MK4_SCRIPT_THROW_VICTIM_CONTINUE:
  forward_to 0xe
script_4ea3c4:
  landing 0x0
script_4ea3c8:
  tail 0x0, MK4_NATIVE_THROW_VICTIM_IMPACT
MK4_SCRIPT_THROW_VICTIM_IMPACT:
  sound_special 0x0
script_4ea3d4:
  native_sound 0x0
script_4ea3d8:
  ready 0x0
  .zero 4

MK4_SCRIPT_GORO_KICK_VICTIM:
  sound_context 0x0
script_4ea3e4:
  spawn_effect 0x6
script_4ea3e8:
  motion 0x0, 0xffffe148, 0xffffd47b, 0x24d
script_4ea3f8:
  jump 0x0, word(script_4e9670)
MK4_SCRIPT_GORO_STOMP_VICTIM:
  sound_context 0xd
script_4ea404:
  sound_b 0x1
script_4ea408:
  prefixed_finish 0x200, word(data_4fb5d4)
script_4ea410:
  tail 0x0, MK4_NATIVE_GORO_SCRIPT_STOMP_VICTIM_HOLD
  .zero 4

script_4ea4d0:
  clip 0x0, word(data_4fb6ac)
script_4ea4d8:
  tail 0x0, MK4_NATIVE_CROUCH_RETURN
  .zero 4

MK4_SCRIPT_JAREK_WEAPON_BACK_HP_UPPER:
  open_clip 0x0, word(data_4fb814)
script_4ec4f8:
  double_to 0x10
script_4ec4fc:
  sound_c 0x1f
script_4ec500:
  double_to 0x1a
script_4ec504:
  forward_to 0x1d
script_4ec508:
  tail 0x0, MK4_NATIVE_CAGE_WEAPON_UPPER_CONTACT
  .zero 4

MK4_SCRIPT_KAI_DRAW_WEAPON_BACK_STRIKE:
  open_clip 0x0, word(data_4fb814)
script_4ec5b0:
  double_to 0x10
script_4ec5b4:
  sound_c 0x1f
script_4ec5b8:
  double_to 0x1a
script_4ec5bc:
  forward_to 0x1d
script_4ec5c0:
  tail 0x0, MK4_NATIVE_KAI_WEAPON_BACK_STRIKE_CONTACT
  .zero 4

MK4_SCRIPT_REPTILE_WEAPON_BACK_HP_FLURRY_STARTUP:
  open_clip 0x0, word(data_4fb91c)
script_4ec648:
  forward_to 0x8
script_4ec64c:
  tail 0x0, MK4_NATIVE_REPTILE_WEAPON_FLURRY
  .zero 4

MK4_SCRIPT_LIU_KANG_SWORD_HP_REPEAT_PROGRAM:
  attack_selector 0x0, 0x30f
script_4ec688:
  schedule_motion 0x6006, 0x11eb
script_4ec690:
  open_clip 0x0, word(data_4fb868)
script_4ec698:
  double_to 0x9
script_4ec69c:
  sound_c 0x1d
script_4ec6a0:
  double_to 0x12
script_4ec6a4:
  prefixed_sample 0x200, word(data_4fb79c)
script_4ec6ac:
  weapon_query 0xf
script_4ec6b0:
  predicate_flags 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(script_4ec6f4)
script_4ec6bc:
  schedule_motion 0x6006, 0x11eb
script_4ec6c4:
  sound_c 0x1f
script_4ec6c8:
  forward_to 0xa
script_4ec6cc:
  weapon_query 0xf
script_4ec6d0:
  predicate_flags 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_GATE, word(script_4ecba8)
script_4ec6dc:
  schedule_motion 0x6006, 0x11eb
script_4ec6e4:
  sound_c 0x1d
script_4ec6e8:
  forward_to 0x15
script_4ec6ec:
  jump 0x0, word(script_4ec6a4)
script_4ec6f4:
  action_recovery 0x0
script_4ec6f8:
  forward_count 0x5
script_4ec6fc:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_SWORD_BLEND
  .zero 4

MK4_SCRIPT_SONYA_WEAPON_BACK_HP_ATTACKER:
  open_clip 0x0, word(data_4fb82c)
script_4ec7a8:
  forward_to 0x6
script_4ec7ac:
  sound_c 0x1e
script_4ec7b0:
  forward_to 0xe
script_4ec7b4:
  tail 0x0, MK4_NATIVE_LIU_KANG_SWORD_BACK_HIGH_PUNCH_CONTACT
  .zero 4

MK4_SCRIPT_SWORD_BACK_HIGH_PUNCH_HIT_RECOVERY:
  forward_to 0x18
script_4ec7c4:
  playback_pair 0xf
script_4ec7c8:
  forward_to 0x18
script_4ec7cc:
  playback_pair 0xf
script_4ec7d0:
  forward 0x0
script_4ec7d4:
  ready 0x0
MK4_SCRIPT_SWORD_BACK_HIGH_PUNCH_BLOCK_RECOVERY:
  forward 0x0
script_4ec7dc:
  ready 0x0
  .zero 4

MK4_SCRIPT_SWORD_ATTACK:
  open_clip 0x0, word(data_4fb820)
script_4ec7f8:
  double_to 0x10
script_4ec7fc:
  sound_c 0x1f
script_4ec800:
  double_to 0x1a
script_4ec804:
  forward_to 0x1d
script_4ec808:
  tail 0x0, MK4_NATIVE_CAGE_WEAPON_BACK_LP_CONTACT
  .zero 4

SWORD_BACK_LP_SCRIPT:
  play_clip 0x9, word(data_4fb898)
script_4ec8b8:
  sound_c 0x22
script_4ec8bc:
  speed 0x0, 0x1999
script_4ec8c4:
  forward_count 0x8
script_4ec8c8:
  weapon_query 0x2
script_4ec8cc:
  contact 0x0, word(script_4ec974)
script_4ec8d4:
  forward_count 0x1
script_4ec8d8:
  weapon_query 0x2
script_4ec8dc:
  contact 0x0, word(script_4ec974)
script_4ec8e4:
  forward_count 0x1
script_4ec8e8:
  weapon_query 0x2
script_4ec8ec:
  contact 0x0, word(script_4ec974)
script_4ec8f4:
  landing 0x0
script_4ec8f8:
  forward_count 0x2
script_4ec8fc:
  wait 0x4
script_4ec900:
  action_recovery 0x0
script_4ec904:
  landing 0x0
script_4ec908:
  forward 0x0
script_4ec90c:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4

SWORD_BACK_HP_SCRIPT:
  play_clip 0x8, word(data_4fb8a4)
script_4ec920:
  sound_c 0x22
script_4ec924:
  forward_count 0x8
script_4ec928:
  speed 0x0, 0x1999
script_4ec930:
  forward_to 0x19
script_4ec934:
  weapon_query 0x3
script_4ec938:
  contact 0x0, word(script_4ec974)
script_4ec940:
  forward_count 0x1
script_4ec944:
  weapon_query 0x3
script_4ec948:
  contact 0x0, word(script_4ec974)
script_4ec950:
  forward_count 0x1
script_4ec954:
  weapon_query 0x3
script_4ec958:
  contact 0x0, word(script_4ec974)
script_4ec960:
  action_recovery 0x0
script_4ec964:
  landing 0x0
script_4ec968:
  forward_count 0x2
script_4ec96c:
  sound_c 0x13
script_4ec970:
  wait 0x4
script_4ec974:
  action_recovery 0x0
script_4ec978:
  landing 0x0
script_4ec97c:
  forward 0x0
script_4ec980:
  tail 0x0, MK4_NATIVE_RETURN_TO_PLAYER_OR_CPU_CONTROL
  .zero 4

script_4ecba8:
  action_recovery 0x0
script_4ecbac:
  forward 0x0
script_4ecbb0:
  ready 0x0
  .zero 4

MK4_SCRIPT_REIKO_STAR:
  sound_c 0x64
script_4ed03c:
  prefixed_to 0x20d, word(data_4fc1f8)
script_4ed044:
  spawn 0x0, 0x52, MK4_NATIVE_REIKO_STAR_SPAWN
script_4ed050:
  forward_to 0x12
script_4ed054:
  playback_pair 0x10
script_4ed058:
  forward_to 0x12
script_4ed05c:
  playback_pair 0x10
script_4ed060:
  forward_to 0x12
script_4ed064:
  playback_pair 0x10
script_4ed068:
  forward_to 0x12
script_4ed06c:
  playback_pair 0x10
script_4ed070:
  forward_to 0x12
script_4ed074:
  playback_pair 0x10
script_4ed078:
  forward 0x0
script_4ed07c:
  ready 0x0
  .zero 4

MK4_SCRIPT_JAX_MISSILE:
  prefixed_to 0x20a, word(data_4faed8)
script_4ed098:
  sound_c 0x5d
script_4ed09c:
  spawn 0x0, 0x50, MK4_NATIVE_JAX_MISSILE_SPAWN
script_4ed0a8:
  forward_to 0x19
script_4ed0ac:
  half_finish 0x0
script_4ed0b0:
  ready 0x0
  .zero 4

MK4_SCRIPT_CAST_ORB:
  sound_c 0x5f
script_4ed0dc:
  open_clip 0x0, word(data_4faecc)
script_4ed0e4:
  double_to 0x8
script_4ed0e8:
  forward_to 0x14
script_4ed0ec:
  spawn 0x0, 0x4f, MK4_NATIVE_CAGE_ORB_SPAWN
script_4ed0f8:
  forward 0x0
script_4ed0fc:
  action_followup 0x0
script_4ed100:
  wait 0x16
script_4ed104:
  ready 0x0
  .zero 4

MK4_SCRIPT_JAREK_BLADE:
  sound_c 0x67
script_4ed11c:
  fast_start 0x0, word(script_4ed7f0)
script_4ed124:
  prefixed_to 0x211, word(data_4faec0)
script_4ed12c:
  spawn 0x0, 0x4e, MK4_NATIVE_JAREK_BLADE_SPAWN
script_4ed138:
  forward_to 0x11
script_4ed13c:
  playback_pair 0xf
script_4ed140:
  forward_to 0x11
script_4ed144:
  playback_pair 0xf
script_4ed148:
  forward_to 0x11
script_4ed14c:
  playback_pair 0xf
script_4ed150:
  forward_to 0x11
script_4ed154:
  playback_pair 0xf
script_4ed158:
  forward_to 0x11
script_4ed15c:
  playback_pair 0xf
script_4ed160:
  forward_to 0x11
script_4ed164:
  playback_pair 0xf
script_4ed168:
  forward 0x0
script_4ed16c:
  ready 0x0
  .zero 4

MK4_SCRIPT_BOOMERANG_START:
  sound_c 0x21
script_4ed194:
  play_clip 0x7, word(data_4fb838)
script_4ed19c:
  tail 0x0, MK4_NATIVE_TANYA_BOOMERANG_RELEASE
  .zero 4

MK4_SCRIPT_REPTILE_ACID_SPIT:
  play_clip 0xb, word(data_4faea8)
script_4ed1f0:
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
script_4ed1f8:
  forward_count 0x5
script_4ed1fc:
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_WIDE
script_4ed204:
  forward_count 0x5
script_4ed208:
  spawn 0x0, 0x4c, MK4_NATIVE_REPTILE_ACID_SPAWN
script_4ed214:
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_ACID_DROOL
script_4ed21c:
  forward_count 0x8
script_4ed220:
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_OPEN
script_4ed228:
  action_followup 0x0
script_4ed22c:
  forward_to 0x1e
script_4ed230:
  playback_pair 0x1b
script_4ed234:
  forward_to 0x1e
script_4ed238:
  playback_pair 0x1b
script_4ed23c:
  forward_to 0x1e
script_4ed240:
  playback_pair 0x1b
script_4ed244:
  call 0x0, MK4_NATIVE_REPTILE_SCRIPT_HEAD_CLOSE
script_4ed24c:
  forward 0x0
script_4ed250:
  ready 0x0
  .zero 4

MK4_SCRIPT_TANYA_FIREBALL_CASTER:
  prefixed_finish 0x200, word(data_4faeb4)
script_4ed270:
  sound_a 0x21
script_4ed274:
  block_sound 0x0
script_4ed278:
  spawn 0x0, 0x4b, MK4_NATIVE_FIREBALL_SPAWN
script_4ed284:
  playback_pair 0xb
script_4ed288:
  forward_to 0xd
script_4ed28c:
  action_followup 0x0
script_4ed290:
  playback_pair 0xb
script_4ed294:
  forward_to 0xd
script_4ed298:
  playback_pair 0xb
script_4ed29c:
  forward_to 0xd
script_4ed2a0:
  playback_pair 0xb
script_4ed2a4:
  forward_to 0xd
script_4ed2a8:
  playback_pair 0x0
script_4ed2ac:
  ready 0x0
  .zero 4

MK4_SCRIPT_AIR_FIREBALL:
  sound_a 0x21
script_4ed2c4:
  block_sound_alt 0x0
script_4ed2c8:
  prefixed_to 0x209, word(data_4fae9c)
script_4ed2d0:
  spawn 0x0, 0x4b, MK4_NATIVE_AIR_FIREBALL_SPAWN
script_4ed2dc:
  backward_count 0x5
script_4ed2e0:
  tail 0x0, MK4_NATIVE_FIREBALL_FALL
  .zero 4

MK4_DATA_KAI_GROUND_SKULL:
  prefixed_finish 0x200, word(data_4fae84)
script_4ed300:
  sound_c 0x33
script_4ed304:
  motion 0x0, 0x0, 0xffffcccd, 0x28f
script_4ed314:
  tail 0x0, MK4_NATIVE_KAI_GROUND_SKULL_SPAWN
  .zero 4

MK4_DATA_KAI_RISING_SKULL:
  prefixed_to 0x207, word(data_4fae78)
script_4ed338:
  sound_c 0x33
script_4ed33c:
  tail 0x0, MK4_NATIVE_KAI_RISING_SKULL_SPAWN
  .zero 4

MK4_SCRIPT_KAI_RISING_SKULL_SPAWN:
  playback_pair 0x4
script_4ed34c:
  forward_to 0x6
script_4ed350:
  playback_pair 0x4
script_4ed354:
  forward_to 0x6
script_4ed358:
  playback_pair 0x4
script_4ed35c:
  forward_to 0x6
script_4ed360:
  playback_pair 0x4
script_4ed364:
  forward_to 0x6
script_4ed368:
  playback_pair 0x4
script_4ed36c:
  forward_to 0x6
script_4ed370:
  playback_pair 0x4
script_4ed374:
  forward_to 0x6
script_4ed378:
  playback_pair 0x4
script_4ed37c:
  forward_to 0x6
script_4ed380:
  playback_pair 0x4
script_4ed384:
  forward_to 0x6
script_4ed388:
  tail 0x0, MK4_NATIVE_KAI_RISING_SKULL_FINISH
  .zero 4

MK4_SCRIPT_QUAN_CHI_SKULL:
  open_clip 0x0, word(data_4fae90)
script_4ed3a8:
  fast_start 0x0, word(script_4ed3e4)
script_4ed3b0:
  prefixed_to 0x11
script_4ed3b4:
  tail 0x0, MK4_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE
  .zero 4

MK4_SCRIPT_QUAN_CHI_SKULL_RELEASE:
  sound_c 0x38
script_4ed3c4:
  forward_to 0x17
script_4ed3c8:
  playback_pair 0x13
script_4ed3cc:
  forward_to 0x17
script_4ed3d0:
  playback_pair 0x13
script_4ed3d4:
  forward_to 0x17
script_4ed3d8:
  playback_pair 0x13
script_4ed3dc:
  forward 0x0
script_4ed3e0:
  ready 0x0
script_4ed3e4:
  cursor_set 0x5
script_4ed3e8:
  double_to 0x11
script_4ed3ec:
  tail 0x0, MK4_NATIVE_QUAN_CHI_SCRIPT_SKULL_RELEASE
  .zero 4

MK4_SCRIPT_FUJIN_CROSSBOW_LP_SHOT_RECOVERY:
  forward_to 0x8
script_4ed40c:
  playback_pair 0x4
script_4ed410:
  forward_to 0x8
script_4ed414:
  playback_pair 0x4
script_4ed418:
  forward_to 0x8
script_4ed41c:
  playback_pair 0x4
script_4ed420:
  tail 0x0, MK4_NATIVE_FUJIN_BOW_FINISH
  .zero 4

MK4_SCRIPT_SCORPION_SPEAR:
  sound_c 0x5
script_4ed44c:
  fast_start 0x0, word(script_4ed47c)
script_4ed454:
  open_clip 0x0, word(data_4fae24)
script_4ed45c:
  double_to 0x14
script_4ed460:
  spawn 0x0, 0x43, MK4_NATIVE_SPEAR_TASK
script_4ed46c:
  double_to 0x1e
script_4ed470:
  action_followup 0x0
script_4ed474:
  wait 0x2d
script_4ed478:
  ready 0x0
script_4ed47c:
  open_clip 0x9, word(data_4fae24)
script_4ed484:
  double_count 0x10
script_4ed488:
  spawn 0x0, 0x43, MK4_NATIVE_SPEAR_TASK
script_4ed494:
  double_to 0x1e
script_4ed498:
  action_followup 0x0
script_4ed49c:
  wait 0x2d
script_4ed4a0:
  ready 0x0
  .zero 4

MK4_SCRIPT_SCORPION_SPEAR_RETURN:
  forward_count 0x13
script_4ed4ac:
  native_return 0x47
script_4ed4b0:
  sound_indexed 0x5
script_4ed4b4:
  forward 0x0
script_4ed4b8:
  tail 0x0, MK4_NATIVE_SCORPION_SCRIPT_SPEAR_RETURN
MK4_SCRIPT_SONYA_RING:
  sound_indexed 0x11
script_4ed4c4:
  open_clip 0x0, word(data_4fae30)
script_4ed4cc:
  fast_start 0x0, word(script_4ed520)
script_4ed4d4:
  double_to 0xa
script_4ed4d8:
  forward_to 0xf
script_4ed4dc:
  spawn 0x0, 0x44, MK4_NATIVE_SONYA_RING_OBJECT
script_4ed4e8:
  spawn 0x0, 0x26, MK4_NATIVE_SONYA_SCRIPT_RING_FLASH
script_4ed4f4:
  sound_c 0x4
script_4ed4f8:
  forward_to 0x13
script_4ed4fc:
  action_followup 0x0
script_4ed500:
  playback_pair 0xf
script_4ed504:
  forward_to 0x13
script_4ed508:
  playback_pair 0xf
script_4ed50c:
  forward_to 0x13
script_4ed510:
  playback_pair 0xf
script_4ed514:
  forward_to 0x13
script_4ed518:
  playback_pair 0x0
script_4ed51c:
  ready 0x0
script_4ed520:
  double_to 0xf
script_4ed524:
  jump 0x0, word(script_4ed4dc)
  .zero 4

MK4_SCRIPT_SUB_ZERO_ICE:
  sound_indexed 0x4
script_4ed58c:
  open_clip 0x0, word(data_4fae3c)
script_4ed594:
  fast_start 0x0, word(script_4ed818)
script_4ed59c:
  forward_to 0xf
script_4ed5a0:
  sound_c 0x0
script_4ed5a4:
  spawn 0x0, 0x42, MK4_NATIVE_ICE_PROJECTILE_TASK
script_4ed5b0:
  wait 0x1
script_4ed5b4:
  action_followup 0x0
script_4ed5b8:
  loop 0x2
script_4ed5bc:
  forward_to 0x17
script_4ed5c0:
  playback_pair 0x12
script_4ed5c4:
  repeat 0x0
script_4ed5c8:
  forward 0x0
script_4ed5cc:
  ready 0x0
  .zero 4

MK4_SCRIPT_RAIDEN_LIGHTNING:
  sound_indexed 0x14
script_4ed5f4:
  sound_c 0x2b
script_4ed5f8:
  fast_start 0x0, word(script_4ed614)
script_4ed600:
  play_clip 0x4, word(data_4fae48)
script_4ed608:
  forward 0x0
script_4ed60c:
  jump 0x0, word(script_4ed624)
script_4ed614:
  open_clip 0x0, word(data_4fae48)
script_4ed61c:
  double_to 0x4
script_4ed620:
  double_count 0xa
script_4ed624:
  spawn 0x0, 0x41, MK4_NATIVE_RAIDEN_LIGHTNING_OBJECT
script_4ed630:
  wait 0x1
script_4ed634:
  action_followup 0x0
script_4ed638:
  loop 0x3
script_4ed63c:
  clip 0x4, word(data_4fae48)
script_4ed644:
  repeat 0x0
script_4ed648:
  clip_alt 0x4, word(data_4fae48)
script_4ed650:
  ready 0x0
  .zero 4

MK4_DATA_LIU_KANG_LOW_FIREBALL_LOW:
  open_clip 0x0, word(data_4fae60)
script_4ed680:
  double_to 0x14
script_4ed684:
  sound_a 0x21
script_4ed688:
  sound_indexed 0x2
script_4ed68c:
  spawn 0x0, 0x40, MK4_NATIVE_LIU_LOW_FIREBALL_OBJECT
script_4ed698:
  forward_to 0x17
script_4ed69c:
  action_followup 0x0
script_4ed6a0:
  playback_pair 0x12
script_4ed6a4:
  forward_to 0x17
script_4ed6a8:
  playback_pair 0x12
script_4ed6ac:
  forward_to 0x17
script_4ed6b0:
  playback_pair 0x12
script_4ed6b4:
  forward_to 0x17
script_4ed6b8:
  playback_pair 0x0
script_4ed6bc:
  ready 0x0
MK4_DATA_LIU_KANG_HIGH_FIREBALL_HIGH:
  open_clip 0x0, word(data_4fae54)
script_4ed6c8:
  double_to 0x14
script_4ed6cc:
  sound_a 0x21
script_4ed6d0:
  sound_indexed 0x2
script_4ed6d4:
  spawn 0x0, 0x40, MK4_NATIVE_LIU_HIGH_FIREBALL_OBJECT
script_4ed6e0:
  forward_to 0x1b
script_4ed6e4:
  action_followup 0x0
script_4ed6e8:
  forward_to 0x12
script_4ed6ec:
  forward_to 0x1b
script_4ed6f0:
  playback_pair 0x0
script_4ed6f4:
  ready 0x0
MK4_SCRIPT_LIU_KANG_AIR_FIREBALL_AIR:
  clip 0x0, word(data_4fae6c)
script_4ed700:
  sound_a 0x21
script_4ed704:
  sound_indexed 0x3
script_4ed708:
  spawn 0x0, 0x40, MK4_NATIVE_LIU_AIR_FIREBALL_OBJECT
script_4ed714:
  wait 0x10
script_4ed718:
  clip_operation 0x0, word(data_4fae6c)
script_4ed720:
  tail 0x0, MK4_NATIVE_LIU_KANG_LIU_SCRIPT_AIR_RETURN
  .zero 4

MK4_SCRIPT_FUJIN_CROSSBOW_FOLLOWUP_SHORT_SHOT:
  forward_to 0x8
script_4ed78c:
  return 0x0
MK4_SCRIPT_GORO_FIREBALL:
  open_clip 0x0, word(data_4fc93c)
script_4ed798:
  forward_to 0xe
script_4ed79c:
  sound_a 0x21
script_4ed7a0:
  spawn 0x0, 0x53, MK4_NATIVE_GORO_FIREBALL_SPAWN
script_4ed7ac:
  playback_pair 0xc
script_4ed7b0:
  forward_to 0xe
script_4ed7b4:
  playback_pair 0xc
script_4ed7b8:
  forward_to 0xe
script_4ed7bc:
  playback_pair 0xc
script_4ed7c0:
  forward_to 0xe
script_4ed7c4:
  playback_pair 0xc
script_4ed7c8:
  forward 0x0
script_4ed7cc:
  ready 0x0
  .zero 4

script_4ed7f0:
  open_clip 0x0, word(data_4faec0)
script_4ed7f8:
  double_to 0x211
script_4ed7fc:
  jump 0x0, word(script_4ed12c)
  .zero 4

script_4ed818:
  double_to 0xf
script_4ed81c:
  jump 0x0, word(script_4ed5a0)
