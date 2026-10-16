#!/usr/bin/env python3
"""Optional x86 oracle for Scorpion normal-move playback and branch decisions.

Executes the supplied VM, task scheduler and native continuations. Pose, audio and contact are
stubbed. Injected contact results test control flow, NOT original collision,
damage, victim reactions, hitstop or scheduler timing. No world physics runs.
"""
import argparse
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from retail_moves import EXE_SHA256, RetailImage

SCRIPTS = {'jab': 0x4e62f0, 'uppercut': 0x4e62a0, 'sweep': 0x4e6798, 'kick': 0x4e5ca0}
CONTACTS = (0x48d3d0, 0x48d3f0, 0x48d410, 0x48d450)
SCENARIOS = [dict(name=name + '_miss', move=name) for name in SCRIPTS]
for move, first in [('uppercut', 7), ('sweep', 18), ('kick', 11), ('jab', 8)]:
    for status, label in [(1, 'block'), (2, 'hit'), (3, 'air_hit')]:
        SCENARIOS.append(dict(name=move + '_' + label, move=move, status=status, contact_step=first))
SCENARIOS += [
    dict(name='jab_repeat_once', move='jab', presses=[8]),
    dict(name='jab_repeat_twice', move='jab', presses=[8, 17]),
    dict(name='jab_repeat_three', move='jab', presses=[8, 17, 27]),
    dict(name='jab_late_repeat', move='jab', presses=[10]),
    dict(name='jab_repeat_facing_away', move='jab', presses=[8], facing_away=True),
    dict(name='jab_repeat_four', move='jab', presses=[8, 17, 27, 37]),
    dict(name='jab_repeat_early', move='jab', presses=[1]),
]
SCENARIOS += [dict(name='uppercut_%s_at_%d' % (label, step), move='uppercut',
                   status=status, contact_step=step)
              for step in (8, 9) for status, label in [(1, 'block'), (2, 'hit')]]


def trace(image, scenario, wait_probe=False):
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP

    machine = Uc(UC_ARCH_X86, UC_MODE_32)
    machine.mem_map(0x400000, 0x1600000)
    for base, payload in image.sections:
        machine.mem_write(base, payload)

    def word(va, value):
        machine.mem_write(va, struct.pack('<I', value & 0xffffffff))

    def get(va):
        return struct.unpack('<I', machine.mem_read(va, 4))[0]

    for dst, value in image.fixups.items():
        word(dst, value)
    # Use a real pool slot: native unlink scans predecessor slots in this
    # pool, so an invented out-of-pool parent breaks spawned task cleanup.
    node, group, other, victim = 0x538728, 0x1001000, 0x1002000, 0x1003000
    for va, value in [(0x53c420, node // 4), (0x53c41c, group // 4),
                      (0x4d07ac, 0x1100000 // 4), (node + 4, 0x1101000 // 4),
                      (node + 0x34, 0), (node + 0x3c, other // 4),
                      (node + 0x38, victim // 4), (other + 0x38, group // 4),
                      (other + 0x3c, node // 4),
                      (group + 0x48, 0x10000), (group + 0x58, 0x10000),
                      (victim + 0x48, 0x10000), (victim + 0x58, 0x10000),
                      (group + 0x18, 0x1004000 // 4), (victim + 0x18, 0x1005000 // 4),
                      (victim + 0x54, 4 * 65536),
                      (group + 0x34, int(scenario.get('facing_away', False))),
                      (0x532518, group // 4), (0x53251c, victim // 4),
                      (0x5323f8, node // 4), (0x5323fc, other // 4),
                      (0x4d00a4, 0xffffffff), (0x4d00a8, 0xffffffff),
                      (0x53019c, 4 * 65536)]:
        word(va, value)
    tick, done, connected = 0, False, False
    poses, commands, contacts, frames = [], [], [], []

    def hook(machine, va, size, unused):
        nonlocal done, connected
        if va == 0x46f650:
            done = True
            machine.emu_stop()
            return
        if va == 0x4594d6:
            commands.append(dict(step=tick, pc=hex(get(node + 0x48) * 4 - 4),
                                 word=hex(get(0x53c42c))))
        if va == 0x407920:
            poses.append(dict(step=tick, clip=image.clip(get(group + 0x24)), frame=get(group + 0x28)))
        if va in CONTACTS:
            status = 0
            if not connected and tick >= scenario.get('contact_step', 0):
                status = scenario.get('status', 0)
                connected = bool(status)
            contacts.append(dict(step=tick, selector=get(0x53c42c), routine=hex(va), result=status))
            word(0x53c44c, (get(0x53c44c) & ~1) | int(bool(status)))
            word(0x53c448, status)
        if va in CONTACTS + (0x407920, 0x489f00, 0x489f30):
            sp = machine.reg_read(UC_X86_REG_ESP)
            machine.reg_write(UC_X86_REG_EIP, get(sp))
            machine.reg_write(UC_X86_REG_ESP, sp + 4)

    machine.hook_add(UC_HOOK_CODE, hook)

    def call(va, arguments=()):
        stack = 0x1800000
        machine.mem_write(stack, struct.pack('<%dI' % (len(arguments) + 1), 0x1700000, *arguments))
        machine.reg_write(UC_X86_REG_ESP, stack)
        machine.emu_start(va, 0x1700000, count=100000)
        if not done:
            assert machine.reg_read(UC_X86_REG_EIP) == 0x1700000, 'Instruction budget exhausted'
            assert machine.reg_read(UC_X86_REG_ESP) == stack + 4, 'Unbalanced CPU stack'

    script = SCRIPTS[scenario['move']]
    if wait_probe:
        script = 0x1006000
        # A test stream, NOT a discovered Scorpion move: open clip 66 for
        # one dispatch, wait 45 scheduler ticks, return to the controller.
        for index, value in enumerate([0x02000001, (0x4fad7c + 66 * 12) // 4,
                                       0x3400002d, 0x3f000000]):
            word(script + index * 4, value)
    call(0x459490, (script,))
    # Mirror the scheduler's post-callback save for the entry dispatch. All
    # subsequent ticks execute the ORIGINAL task pump at 0x41f570, including
    # signed 16-bit countdown at node +0xdc and task-local scratch restore.
    # A direct callback loop silently turns opcode 0x34's 45-tick wait into 1.
    for offset, scratch in [(0x14, 0x53c43c), (0x18, 0x53c440),
                            (0x1c, 0x53c444), (0x20, 0x53c448),
                            (0x10, 0x53c40c), (0x24, 0x53c414),
                            (0x28, 0x53c418), (0x2c, 0x53c41c)]:
        word(node + offset, get(scratch))
    word(node + 0xd8, get(node + 8))
    word(node + 0xdc, get(0x53c40c) & 0xffff)
    word(0x524efc, node)
    for tick in range(256):
        if tick:
            word(0x53c22c, 0)
            if tick in scenario.get('presses', []):
                word(0x53c204, tick + 1)
            call(0x41f570)
        if done:
            break
        assert poses, 'No initial pose'
        frames.append({key: poses[-1][key] for key in ('clip', 'frame')})
    assert done, 'Move never returned to fighter controller'
    assert get(node + 4) == 0x1101000 // 4, 'Continuation stack did not unwind'
    return dict(**scenario, end_step=tick, displayed=frames, pose_calls=poses,
                commands=commands, contact_queries=contacts)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    probe = trace(RetailImage(args.exe), dict(name='scheduler_wait45', move='kick'), wait_probe=True)
    assert probe['end_step'] == 46
    assert [row['step'] for row in probe['commands']] == [0, 1, 46]
    assert all(row == dict(clip=66, frame=0) for row in probe['displayed'])
    result = dict(exe_sha256=EXE_SHA256,
                  limits=__doc__, moves=[trace(RetailImage(args.exe), row) for row in SCENARIOS])
    directory = Path(__file__).parent / 'fixtures'
    fixture = directory / 'original_scorpion.json'
    if args.record:
        fixture.write_text(json.dumps(result, indent=2) + '\n')
        rows = ['/* Generated by tests/verify_scorpion.py --record from original x86. */',
                '// clang-format off: Keep table entries on one row, including long rows.']
        for move in result['moves']:
            state = dict(jab='S_PUNCH', kick='S_KICK', sweep='S_SWEEP', uppercut='S_UPPERCUT')[move['move']]
            clips = ', '.join(str(frame['clip']) for frame in move['displayed'])
            frames = ', '.join(str(frame['frame']) for frame in move['displayed'])
            presses = ', '.join(map(str, move.get('presses', []))) or '0'
            rows.append('{"%s", %s, %d, %d, %d, {%s}, %d, %d, {%s}, {%s}},' %
                        (move['name'], state, move.get('status', 0), move.get('contact_step', 0),
                         len(move.get('presses', [])), presses, move.get('facing_away', False),
                         move['end_step'], clips, frames))
        rows.append('// clang-format on')
        (directory / 'original_scorpion.inc').write_text('\n'.join(rows) + '\n')
    else:
        assert result == json.loads(fixture.read_text()), 'Scorpion playback fixture changed'
    print('PASS: %d original Scorpion VM/native scenarios; balanced continuations' % len(result['moves']))
    print('PASS: original scheduler preserves the 45-tick wait and held pose')
    for move in result['moves']:
        print('  %s: %d steps, contacts %s' %
              (move['name'], move['end_step'], [row['step'] for row in move['contact_queries']]))


if __name__ == '__main__':
    main()
