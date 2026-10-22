#!/usr/bin/env python3
"""Extract Scorpion's retail command records, dispatch targets and attack records.

This is a source inventory, not a claim that every native path is implemented.
Run from the workspace root. The supplied EXE's exact hash is checked.
"""
import argparse
import json
from pathlib import Path

from retail_moves import EXE_SHA256, RetailImage


# Button names confirmed by the retail sc_m_geo.geo move-list texture as well
# as its command records: D,B,HP; B,B,LP; F,F,HK; D,F,LP and the finishers.
COMMAND_TABLES = [
    ('up', 0x53c888, 0x53cb10),
    ('high_punch', 0x53c8d0, 0x53cb58),
    ('low_punch', 0x53c918, 0x53cba0),
    ('block', 0x53c960, 0x53cbe8),
    ('high_kick', 0x53c9a8, 0x53cc30),
    ('low_kick', 0x53c9f0, 0x53cc78),
    ('low_kick_release', 0x53ca38, 0x53ca80),
    ('run', 0x53cac8, 0x53ccc0),
]
DISPATCH = [(1, 0x4e6ef8), (2, 0x4ed828), (4, 0x4e8910),
            (8, 0x4e7008), (32, 0x4dfc90)]
# Researched contiguous handler region, 0x4e3988..0x4e3b60 inclusive.
# The original VM itself does not bounds-check its opcode index.
VM_HANDLER_COUNT = 0x77

# Operand counts checked in the supplied handlers, not inferred from an
# apparent pointer following an opcode. Unlisted opcodes stop extraction.
SCRIPT_OPS = {
    0x00: ('VM no-op / continue', 0),
    0x01: ('clip descriptor playback', 1),
    0x02: ('open clip and advance for count', 1),
    0x03: ('finish forwards', 0),
    0x04: ('advance forwards for count', 0),
    0x05: ('advance forwards to sample', 0),
    0x06: ('advance backwards for count', 0),
    0x07: ('finish backwards', 0),
    0x08: ('clip descriptor playback', 1),
    0x09: ('clip descriptor operation', 1),
    0x0a: ('advance animation cursor by low byte', 0),
    0x13: ('set cursor to final sample', 0),
    0x14: ('submit final animation sample', 0),
    0x0c: ('bind clip, submit inline sample and advance cursor', 1),
    0x0d: ('native playback helper', 0),
    0x0e: ('open victim clip at sample 0', 1),
    0x0f: ('open clip at sample', 1),
    0x10: ('advance self and victim poses for count', 0),
    0x11: ('bind clip and set animation cursor', 1),
    0x12: ('set animation cursor', 0),
    0x17: ('bind clip and play two source poses per visit', 1),
    0x1a: ('bind clip and play interleaved poses with native limit 200', 1),
    0x1b: ('return operand and low-byte count to native caller', 1),
    0x1c: ('play inherited clip with interleaved poses and inline count', 0),
    0x16: ('mark fatality scene active', 0),
    0x19: ('advance twice per visit for count', 0),
    0x1d: ('native playback helper', 0),
    0x1e: ('native helper', 0),
    0x1f: ('set attack selector', 1),
    0x20: ('sound event', 0),
    0x22: ('install task callback 0x457cb0 at +0x4c', 0),
    0x24: ('release attack bookkeeping', 0),
    0x25: ('native helper', 0),
    0x26: ('indexed sound helper with inline argument', 0),
    0x27: ('sound event', 0),
    0x28: ('native helper', 0),
    0x29: ('set action 0x1006', 0),
    0x2a: ('spawn helper with inline argument', 0),
    0x2b: ('sound event', 0),
    0x2c: ('native helper', 0),
    0x2d: ('set action 0x1007', 0),
    0x2e: ('set fighter no-guard flag', 0),
    0x2f: ('branch if airborne', 1),
    0x30: ('return through tagged continuation stack', 0),
    0x32: ('native tail call', 1),
    0x33: ('script jump', 1),
    0x34: ('scheduler wait', 0),
    0x35: ('native call', 1),
    0x36: ('native predicate and conditional script branch', 2),
    0x37: ('wait selected by native result bit 0', 0),
    0x38: ('install group callback at +0x44', 1),
    0x39: ('clear group callback at +0x44', 0),
    0x3a: ('set loop counter', 0),
    0x3b: ('decrement counter and loop to preceding 0x3a', 0),
    0x3c: ('select character script', 16),
    0x3d: ('branch if contact result is blocked', 1),
    0x43: ('native predicate and conditional script branch', 2),
    0x50: ('contact query B and branch on contact', 1),
    0x54: ('wait selected by native result bit 0', 0),
    0x5d: ('adjust position along heading and vertically', 2),
    0x74: ('play native sound token 0x627', 0),
    0x3e: ('park task with repeated 60-tick reschedule', 0),
    0x3f: ('return to fighter controller', 0),
    0x40: ('spawn cooperative task', 2),
    0x41: ('native helper', 0),
    0x42: ('branch if contact result bit 0 set', 1),
    0x44: ('branch if down held', 1),
    0x45: ('conditional script branch', 1),
    0x46: ('advance at half speed for count with optional clip prefix', -1),
    # -1: handler calls 0x45bf80; a descriptor follows ONLY if byte 1 is 2.
    0x47: ('advance to sample with optional clip prefix', -1),
    0x48: ('native playback with optional clip prefix', -1),
    0x49: ('native playback with optional clip prefix', -1),
    0x4a: ('native playback helper', 0),
    0x4b: ('native playback helper', 0),
    0x4c: ('native playback helper', 0),
    0x4d: ('advance twice per visit to sample', 0),
    0x4e: ('save active fighter in reaction context', 0),
    0x4f: ('native helper', 0),
    0x51: ('contact query B with inline selector', 0),
    0x53: ('contact query D with inline selector', 0),
    0x52: ('contact query A with inline selector', 0),
    0x55: ('native predicate and conditional script branch', 2),
    0x56: ('dispatch scene event with inline selector', 0),
    0x58: ('motion operands', 3),
    0x59: ('native motion parameter block', 4),
    0x5a: ('set horizontal speed to negated Q16 operand', 1),
    0x5b: ('native helper', 0),
    0x5c: ('set gravity to Q16 operand', 1),
    0x5e: ('weapon contact query with inline selector', 0),
    0x5f: ('horizontal motion operand', 1),
    0x60: ('native helper', 0),
    0x61: ('clear fighter busy flag', 0),
    0x62: ('indexed native helper with inline argument', 0),
    0x63: ('health subtraction with Q16 operand', 1),
    0x64: ('correct facing toward opponent', 0),
    0x65: ('motion via native 0x490460, vertical velocity and gravity', 3),
    0x6c: ('schedule horizontal motion with packed delay/duration', 1),
    0x6a: ('native helper', 0),
    0x6f: ('branch when current fighter occupies outer arena edge', 1),
    0x70: ('dispatch peer reaction within Q16 distance operand', 1),
    0x72: ('start shared native edge-push task', 0),
    0x6b: ('sound helper', 0),
    0x6e: ('disarm opponent through native helper', 0),
    0x6d: ('native helper with word operand', 1),
    0x68: ('restart animation at half speed with optional clip prefix', -1),
    0x69: ('play remaining animation at half speed', 0),
    0x71: ('blend into clip and resume at sample one', 1),
    0x73: ('play one of two sampled sound tokens from random bit zero', 0),
    0x75: ('play indexed block sound when selected player is blocking', 0),
    0x76: ('play alternate indexed block sound when selected player is blocking', 0),
}


# These slots are direct aliases of the original dispatcher, with no operands.
for _op in (0x0b, 0x15, 0x18, 0x21, 0x23, 0x57, 0x66, 0x67):
    SCRIPT_OPS[_op] = ('VM no-op / continue', 0)


# Verified from exact-entry handlers in the supplied EXE, not execution traces.
# These wrappers consume no following words: they mask inline to its low byte,
# call the target, then resume dispatch (0x61 simply calls 0x490330).
# 0x5a instead calls word-fetch 0x45bff0 once, negates it, then calls 0x490390.
# The transitive effects of 0x26/0x2a/0x62 remain native research dependencies.
INLINE_NATIVE_OPS = {
    0x56: 0x4315b0, 0x2e: 0x490140, 0x6e: 0x483ae0,
    0x26: 0x48a100, 0x2a: 0x457b50, 0x51: 0x48d3f0,
    0x52: 0x48d3d0, 0x5e: 0x48d430, 0x61: 0x490330, 0x62: 0x40a470,
}


def signed_word(value):
    return value if value < 0x80000000 else value - 0x100000000


def script_program(image, entry, character=0):
    """Extract all known VM paths; native behavior remains an explicit edge.

    Reaching a native tail closes this VM path, not the whole move. In
    particular installing a group callback does not execute it in the VM.
    """
    if not 0 <= character < 18:
        raise ValueError('Character ID must be 0..17')
    selection = 2 if character == 16 else 7 if character == 17 else character
    pending, seen, rows, open_edges = [entry], set(), [], []
    while pending:
        pc = pending.pop()
        while pc not in seen:
            if len(seen) >= 512:
                raise ValueError('Scorpion script graph exceeds researched bounds')
            seen.add(pc)
            word = image.word(pc)
            op = word >> 24
            row = dict(va=hex(pc), word=hex(word), opcode=hex(op),
                       handler=hex(image.word(0x4e3988 + op * 4)) if op < VM_HANDLER_COUNT else None)
            rows.append(row)
            if op not in SCRIPT_OPS:
                row['operation'] = 'unresearched opcode; path stopped'
                open_edges.append(dict(source=hex(pc), kind='unknown opcode', target=hex(op)))
                break
            operation, count = SCRIPT_OPS[op]
            if count == -1:
                count = int(((word >> 8) & 255) == 2)
            operands = [image.word(pc + (i + 1) * 4) for i in range(count)]
            row.update(operation=operation, inline=word & 0xffffff,
                       operands=[hex(value) for value in operands])
            if op in (0x01, 0x02, 0x08, 0x09, 0x0c, 0x0e, 0x0f, 0x11, 0x17, 0x1a, 0x46, 0x47, 0x48, 0x49, 0x4e, 0x68, 0x71) and operands:
                clip = image.clip(operands[0])
                row.update(clip=clip, stored_count=image.word(operands[0] * 4 + 4))
                if op in (0x01, 0x0c, 0x0f, 0x11):
                    row['start_sample'] = word & 255
                row['clip_owner'] = 'victim' if op == 0x0e else 'self'
                if op == 0x71:
                    row['start_sample'] = 1
            if op in (0x32, 0x35, 0x36, 0x38, 0x43, 0x55):
                row['native_target'] = hex(operands[0])
                open_edges.append(dict(source=hex(pc), kind=operation, target=hex(operands[0])))
            if op in INLINE_NATIVE_OPS:
                row['native_target'] = hex(INLINE_NATIVE_OPS[op])
                row['argument'] = word & 255 if op != 0x61 else None
                open_edges.append(dict(source=hex(pc), kind='native helper dependency',
                                       target=row['native_target']))
            if op in (0x50, 0x51, 0x52, 0x53, 0x5e):
                table, label = {0x50: (0x53d2b8, 'B'), 0x51: (0x53d2b8, 'B'), 0x52: (0x53d268, 'A'),
                                0x53: (0x53d178, 'D'), 0x5e: (0x53d0c0, 'weapon')}[op]
                row['contact_table'] = label
                row['contact'] = attack_record(image, table, word & 255)
            if op == 0x5a:
                row['speed_q16'] = signed_word((-operands[0]) & 0xffffffff)
                row['native_target'] = '0x490390'
                open_edges.append(dict(source=hex(pc), kind='native motion dependency', target=row['native_target']))
            if op == 0x40:
                row.update(task_type=operands[0], callback=hex(operands[1]))
                open_edges.append(dict(source=hex(pc), kind=operation, target=hex(operands[1])))
            if operation in ('native helper', 'native playback helper', 'native helper with word operand') or op in (0x22, 0x4e, 0x58, 0x59, 0x5f, 0x65):
                open_edges.append(dict(source=hex(pc), kind='native helper semantics',
                                       target=row['handler']))
            if op == 0x30:
                open_edges.append(dict(source=hex(pc), kind='dynamic continuation return', target='0x41f780'))
            if op in (0x37, 0x54):
                row['wait_if_clear'] = word & 0xfff
                row['wait_if_set'] = (word >> 12) & 0xfff
            if op in (0x2f, 0x3d, 0x42, 0x44, 0x45, 0x50, 0x6f):
                pending.append(operands[0] * 4)
                row['branch'] = hex(operands[0] * 4)
            if op in (0x36, 0x43, 0x55):
                pending.append(operands[1] * 4)
                row['branch'] = hex(operands[1] * 4)
            if op == 0x3b:
                # Match the native word scan; it searches instruction AND
                # operand words backwards for a top byte of 0x3a.
                for back in range(pc - 4, max(0x400000, pc - 2048) - 1, -4):
                    if image.word(back) >> 24 == 0x3a:
                        row['branch'] = hex(back + 4)
                        pending.append(back + 4)
                        break
                else:
                    open_edges.append(dict(source=hex(pc), kind='loop opener outside scan bound', target='unknown'))
            if op == 0x3c:
                row['character_table'] = [hex(value * 4) for value in operands]
                row['selected_character'] = selection
                pc = operands[selection] * 4
            elif op == 0x33:
                row['branch'] = hex(operands[0] * 4)
                pc = operands[0] * 4
            elif op in (0x1b, 0x30, 0x32, 0x3e, 0x3f):
                break
            else:
                pc += (count + 1) * 4
    return dict(entry=hex(entry), instructions=sorted(rows, key=lambda row: int(row['va'], 16)),
                unresolved_native_edges=open_edges, whole_move_verified=False)


def commands(image, character=0):
    result = []
    if not 0 <= character < 18:
        raise ValueError('Character ID must be 0..17')
    queries = []
    for button, p1, p2 in COMMAND_TABLES:
        p1 += character * 4
        p2 += character * 4
        root1, root2 = image.word(p1) * 4, image.word(p2) * 4
        queries.append((button, p1, p2, root1, 1 if root1 != root2 else None))
        if root2 != root1:
            queries.append((button, p1, p2, root2, 2))
    for button, p1, p2, root, player in queries:
        records = []
        pc = root
        for _ in range(256):
            if not pc:
                break
            pattern, extra, word = [image.word(pc + k * 4) for k in range(3)]
            if pattern == 0:
                break
            flags, action, gate = (word >> 16) & 255, word & 255, word >> 24
            table = next((base for bit, base in DISPATCH if flags & bit), None)
            tokens = []
            if pattern != 0xffffffff:
                remaining = pattern
                while remaining:
                    tokens.append(remaining & 255)
                    remaining >>= 8
            records.append(dict(va=hex(pc), raw_words=[hex(pattern), hex(extra), hex(word)],
                                tokens=tokens, unconditional_pattern=pattern == 0xffffffff,
                                window=(word >> 8) & 255, flags=hex(flags), action=action,
                                predicate_index=gate,
                                predicate=hex(image.word(0x4e4ec0 + gate * 4)) if gate else None,
                                dispatch_table=hex(table) if table else None,
                                target=hex(image.word(table + action * 4)) if table else None))
            pc += 12
        else:
            raise ValueError('Unterminated command list')
        result.append(dict(button=button, p1_table=hex(p1), p2_table=hex(p2),
                           selected_list=hex(root), records=records))
        if player is not None:
            result[-1]['player'] = player
    return result


def attack_record_at(image, va):
    words = [image.word(va + k * 4) for k in range(6)]
    return dict(va=hex(va), raw_words=[hex(word) for word in words],
                rectangle_q16=[signed_word(word) for word in words[:4]],
                reaction=words[4] & 255, alternate_reaction=(words[4] >> 8) & 255,
                blocked_reaction=(words[4] >> 16) & 255, flags=words[4] >> 24,
                damage_q16=words[5])


def attack_record(image, table, selector):
    va = image.word(table + selector * 4) * 4
    return dict(table=hex(table), selector=selector, **attack_record_at(image, va))


def inventory(image):
    # These roots were followed directly in the supplied EXE. Several native
    # routines use shared animation/motion/weapon/scene machinery beyond a VM
    # script. Listing a root does not close that dependency graph.
    roots = [
        ('jab', 0x46dca0, 0x4e62f0),
        ('high_kick', 0x46a680, 0x4e5ca0),
        ('sweep', 0x46ef90, 0x4e6798),
        ('uppercut', 0x46ccd0, 0x4e62a0),
        ('spear', 0x49a630, 0x4ed448),
        ('breathe_fire', 0x468610, 0x4e5b58),
        ('teleport_punch', 0x47fc90, 0x4e87b0),
        ('draw_weapon', 0x46d1d0, 0x4e62c0),
        ('fatality_1', 0x467ce0, None),
        ('fatality_2', 0x467cd0, None),
        ('spike_fatality', 0x467e70, None),
        ('fan_fatality', 0x467e00, None),
    ]
    return dict(exe_sha256=EXE_SHA256, character=0,
                move_list_asset='geogfx/sc_m_geo.geo',
                command_records=commands(image),
                entry_points=[dict(name=name, native=hex(native), script=hex(script) if script else None,
                                   implementation='not assessed by static decoder')
                              for name, native, script in roots],
                special_programs={name: script_program(image, script) for name, script in [
                    ('spear_entry', 0x4ed448), ('spear_return', 0x4ed4a8),
                    ('breathe_fire', 0x4e5b58), ('teleport_entry', 0x4e87b0),
                    ('teleport_landing', 0x4e8818), ('weapon_draw', 0x4e62c0),
                    ('weapon_draw_return', 0x4e62d8)]},
                kick_contacts=[attack_record(image, 0x53d2b8, i) for i in (1, 2)],
                coverage=dict(scope='Selected Scorpion roots; not exhaustive character coverage',
                              instruction_decode='Per-script unknown stops and native dependencies are explicit',
                              native_semantics='Not established by instruction decoding',
                              implementation='Not assessed; see docs/character-script-port.md'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    report = json.dumps(inventory(RetailImage(args.exe)), indent=2) + '\n'
    if args.output:
        args.output.write_text(report)
    else:
        print(report, end='')


if __name__ == '__main__':
    main()
