#!/usr/bin/env python3
"""Assemble project-owned MK4 wordcode. No executable or disassembler is needed."""
import argparse
import ast
from pathlib import Path
import re
import struct

BEGIN, END = 0x4d0000, 0x4ef000
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


def expression(text, symbols):
    def visit(node):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.Name) and node.id in symbols:
            return symbols[node.id]
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
            return -visit(node.operand)
        if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub)):
            a, b = visit(node.left), visit(node.right)
            return a + b if isinstance(node.op, ast.Add) else a - b
        if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and len(node.args) == 1 and not node.keywords:
            value = visit(node.args[0])
            if node.func.id == 'word' and value % 4 == 0 and 0x400000 <= value < 0x580000:
                return value // 4
            if node.func.id == 'clip' and 0 <= value < 612:
                return (0x4fad7c + value * 12) // 4
        raise ValueError('Invalid expression or unknown symbol: ' + text)
    return visit(ast.parse(text, mode='eval').body)


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


def assemble(path):
    symbols, pending, expected, pc = {}, [], [], None
    for location, line in lines(Path(path)):
        try:
            if line.startswith('.expect '):
                name, value = line[8:].split(None, 1)
                expected.append((location, name, value))
            elif line.startswith('.equ '):
                name, value = line[5:].split(None, 1)
                if name in symbols:
                    raise ValueError('Duplicate symbol ' + name)
                symbols[name] = expression(value, symbols)
            elif line.startswith('.org '):
                pc = expression(line[5:], symbols)
                if pc % 4 or not BEGIN <= pc < END:
                    raise ValueError('Script origin outside MK4 wordcode range')
            elif line.endswith(':'):
                name = line[:-1]
                if pc is None or not re.fullmatch(r'[A-Za-z_]\w*', name) or name in symbols:
                    raise ValueError('Invalid or duplicate label ' + name)
                symbols[name] = pc
            else:
                name, _, arguments = line.partition(' ')
                if name not in OPS or pc is None:
                    raise ValueError('Unknown instruction or missing origin: ' + name)
                args = [a.strip() for a in arguments.split(',')]
                op, count = OPS[name]
                inline = expression(args[0], symbols)
                if not 0 <= inline <= 0xffffff:
                    raise ValueError('Inline field exceeds 24 bits')
                if count == -1:
                    prefix = inline >> 8 & 255
                    if prefix not in (0, 2):
                        raise ValueError('Invalid optional clip prefix')
                    count = int(prefix == 2)
                if len(args) != count + 1:
                    raise ValueError(f'{name} expects inline field and {count} operands')
                if pc + 4 * len(args) > END:
                    raise ValueError('Instruction exceeds script range')
                pending.append((location, pc, op << 24 | inline, args[1:]))
                pc += 4 * len(args)
        except (ValueError, SyntaxError) as exc:
            raise ValueError(f'{location}: {exc}') from exc
    for location, name, value in expected:
        if name not in symbols or symbols[name] != expression(value, symbols):
            raise ValueError(f'{location}: externally referenced entry {name} moved')
    words = {}
    for location, pc, opcode, operands in pending:
        try:
            values = [opcode] + [expression(value, symbols) for value in operands]
            for i, value in enumerate(values):
                address = pc + i * 4
                if address in words:
                    raise ValueError(f'Overlapping script at {address:#x}')
                if not -(1 << 31) <= value <= 0xffffffff:
                    raise ValueError('Operand exceeds 32 bits')
                words[address] = value & 0xffffffff
        except (ValueError, SyntaxError) as exc:
            raise ValueError(f'{location}: {exc}') from exc
    if not words:
        raise ValueError('Empty script package')
    return words


def encode(words):
    return struct.pack('<4sII', b'MK4S', 1, len(words)) + b''.join(struct.pack('<II', *row) for row in sorted(words.items()))


def read_package(path):
    data = Path(path).read_bytes()
    if len(data) < 12:
        raise ValueError('Truncated MK4 script package')
    magic, version, count = struct.unpack_from('<4sII', data)
    if magic != b'MK4S' or version != 1 or not count or len(data) != 12 + count * 8:
        raise ValueError('Invalid MK4 script package')
    words, previous = {}, 0
    for address, word in struct.iter_unpack('<II', data[12:]):
        if address % 4 or not BEGIN <= address < END or address <= previous:
            raise ValueError('Invalid or duplicate script address')
        words[address] = word
        previous = address
    return words


def apply_package(image, path):
    for address, value in read_package(path).items():
        struct.pack_into('<I', image.memory, address - 0x400000, value)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    try:
        data = encode(assemble(args.source))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        temporary = args.output.with_suffix(args.output.suffix + '.tmp')
        temporary.write_bytes(data)
        temporary.replace(args.output)
    except (ValueError, OSError) as exc:
        parser.exit(1, str(exc) + '\n')


if __name__ == '__main__':
    main()
