#!/usr/bin/env python3
"""MK4 opcode schema and source include reader used by the linker."""
from pathlib import Path

# The inline field is preserved in full; operand counts include optional clip prefixes.
NAMES = '''nop clip play_clip forward forward_count forward_to backward_count backward
clip_alt clip_operation cursor_advance nop_0b bind_submit playback_native open_peer_clip open_clip
play_pair bind_cursor cursor_set cursor_last submit_last nop_15 scene_active play_double
nop_18 double_count play_interleaved return_word inherited_interleaved playback_tail native_step attack_selector
sound_a nop_21 task_callback nop_23 release_attack sound_indexed sound_context sound_c
sound_special action_recovery spawn_effect sound_b sound_variant action_followup no_guard airborne
return invalid tail jump wait call predicate_flags wait_branch handler
clear_handler loop repeat select_roster blocked park ready spawn
native_return contact predicate_bit down fast_start half_count prefixed_to prefixed_finish
prefixed_sample playback_pair playback_single playback_pair_reverse double_to reaction_context native_release
query_branch query_b query_a query_d wait_result predicate_argument scene_event nop_57
motion motion_block speed_negative landing gravity position weapon_query speed
native_context allow_commands effect damage face peer_motion nop_66 nop_67
half_restart half_finish native_sound sound_roster schedule_motion native_word disarm_peer edge
reaction_distance blend edge_push sound_random sound_token block_sound block_sound_alt'''.split()
COUNTS = {1:1,2:1,8:1,9:1,12:1,14:1,15:1,17:1,23:1,26:1,27:1,31:1,
          47:1,50:1,51:1,53:1,54:2,56:1,60:16,61:1,64:2,66:1,67:2,68:1,69:1,
          70:-1,71:-1,72:-1,73:-1,80:1,85:2,88:3,89:4,90:1,92:1,93:2,
          95:1,99:1,101:3,104:-1,108:1,109:1,111:1,112:1,113:1}
OPS = {name: (op, COUNTS.get(op, 0)) for op, name in enumerate(NAMES) if op != 0x31}


def lines(path, parents=()):
    path = path.resolve()
    if path in parents:
        raise ValueError('Recursive include: ' + str(path))
    for number, raw in enumerate(path.read_text().splitlines(), 1):
        line = raw.split(';', 1)[0].strip()
        if line.startswith('.include '):
            name = line[len('.include '):].strip('"')
            yield from lines(path.parent / name, (*parents, path))
        elif line:
            yield f'{path}:{number}', line
