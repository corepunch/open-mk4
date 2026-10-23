#!/usr/bin/env python3
"""Link editable MK4 data and script labels; no executable is read during a build."""
import argparse
import ast
import hashlib
import json
from pathlib import Path
import re
import struct
from mk4_asm import OPS, lines

BASE = 0x1000000
MAX_SIZE = 16 * 1024 * 1024
HEADER = struct.Struct('<4sIIII32s')


def evaluate(text, symbols):
    def visit(node):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.Name):
            return symbols[node.id]
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
            return -visit(node.operand)
        if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub)):
            a, b = visit(node.left), visit(node.right)
            return a + b if isinstance(node.op, ast.Add) else a - b
        if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and len(node.args) == 1 and not node.keywords:
            n = visit(node.args[0])
            if node.func.id == 'word' and n % 4 == 0 and BASE <= n < BASE + MAX_SIZE:
                return n // 4
            if node.func.id == 'clip' and 0 <= n < 612:
                return (symbols['MK4_DATA_ANIMATION_TABLE'] + 12 * n) // 4
        raise ValueError('Invalid expression: ' + text)
    return visit(ast.parse(text, mode='eval').body)


def link(path):
    symbols, aliases, exports, rows = {}, {}, {}, []
    pc, section, script_begin = BASE, None, None
    for location, line in lines(Path(path)):
        try:
            if line.startswith('.section '):
                kind = line.split()[1]
                if kind not in ('data', 'script') or (section is not None and (section != 'data' or kind != 'script')):
                    raise ValueError('Expected data section followed by script section')
                section = kind
                if kind == 'script':
                    script_begin = pc
            elif line.startswith(('.equ ', '.alias ', '.export ')):
                directive, name, value = line.split(None, 2)
                if directive == '.export':
                    if name in exports:
                        raise ValueError('Duplicate export ' + name)
                    exports[name] = value
                else:
                    if name in symbols or name in aliases:
                        raise ValueError('Duplicate symbol ' + name)
                    if directive == '.equ':
                        symbols[name] = evaluate(value, symbols)
                    else:
                        aliases[name] = value
            elif line.endswith(':'):
                name = line[:-1]
                if section is None or not re.fullmatch(r'[A-Za-z_]\w*', name) or name in symbols or name in aliases:
                    raise ValueError('Invalid/duplicate label ' + name)
                symbols[name] = pc
            else:
                if section is None:
                    raise ValueError('Missing section')
                name, _, arguments = line.partition(' ')
                code = False
                if name == '.zero':
                    size = evaluate(arguments, symbols)
                    if size < 0 or size % 4:
                        raise ValueError('Zero storage must be word aligned')
                    args = None
                elif name == '.word':
                    args = [a.strip() for a in arguments.split(',')]
                    size = len(args) * 4
                elif name == '.string' and section == 'data':
                    value = json.loads(arguments)
                    if not isinstance(value, str) or '\0' in value:
                        raise ValueError('Expected a string without embedded NUL')
                    encoded = value.encode('ascii') + b'\0'
                    encoded += b'\0' * (-len(encoded) % 4)
                    args = [str(word[0]) for word in struct.iter_unpack('<I', encoded)]
                    size = len(encoded)
                elif name in OPS and section == 'script':
                    args = [a.strip() for a in arguments.split(',')]
                    op, count = OPS[name]
                    inline = evaluate(args[0], symbols)
                    if not 0 <= inline <= 0xffffff:
                        raise ValueError('Inline field exceeds 24 bits')
                    if count == -1:
                        if (inline >> 8 & 255) not in (0, 2):
                            raise ValueError('Invalid optional clip prefix')
                        count = int((inline >> 8 & 255) == 2)
                    if len(args) != count + 1:
                        raise ValueError('Incorrect operand count for ' + name)
                    args[0] = str(op << 24 | inline)
                    size, code = 4 * len(args), True
                else:
                    raise ValueError('Unknown directive/instruction: ' + name)
                if pc + size > BASE + MAX_SIZE:
                    raise ValueError('Linked data exceeds capacity')
                rows.append((location, pc, size, args, code))
                pc += size
        except (ValueError, SyntaxError, KeyError) as exc:
            raise ValueError(f'{location}: {exc}') from exc
    while aliases:
        resolved = []
        for name, value in aliases.items():
            try:
                symbols[name] = evaluate(value, symbols)
                resolved.append(name)
            except KeyError:
                pass
        if not resolved:
            raise ValueError('Unknown/cyclic alias: ' + ', '.join(aliases))
        for name in resolved:
            del aliases[name]
    if script_begin is None or script_begin == pc:
        raise ValueError('Missing script section')
    payload = bytearray(pc - BASE)
    code_words = set()
    for location, address, size, args, code in rows:
        if args is None:
            continue
        try:
            for i, expr in enumerate(args):
                value = evaluate(expr, symbols)
                if not -(1 << 31) <= value <= 0xffffffff:
                    raise ValueError('Word exceeds 32 bits')
                off = address - BASE + i * 4
                struct.pack_into('<I', payload, off, value & 0xffffffff)
                if code:
                    code_words.add(off)
        except (ValueError, KeyError, SyntaxError) as exc:
            raise ValueError(f'{location}: {exc}') from exc
    # Research imports still carry instruction words: check their direct branches,
    # including roster selectors, name linked instructions. The project itself links
    # one placeholder word per Lua script and no instructions.
    for location, address, size, args, code in rows:
        if not code:
            continue
        op = int(args[0]) >> 24
        indexes = (range(1, 17) if op == 0x3c else
                   (1,) if op in (0x2f, 0x33, 0x3d, 0x42, 0x44, 0x45, 0x50) else
                   (2,) if op in (0x36, 0x43, 0x55) else ())
        for index in indexes:
            target = evaluate(args[index], symbols) * 4
            offset = target - BASE
            if not (script_begin <= target < pc and target % 4 == 0 and offset in code_words):
                raise ValueError(f'{location}: branch does not name a linked script word')
    constants = {name: evaluate(value, symbols) for name, value in exports.items()}
    constants.update(MK4_DATA_IMAGE_BASE=BASE, MK4_DATA_IMAGE_END=pc,
                     SCRIPT_ADDRESS_BEGIN=script_begin, SCRIPT_ADDRESS_END=pc,
                     MK4_SCRIPT_BEGIN=script_begin, MK4_SCRIPT_END=pc)
    # The host must be rebuilt if any exported offset changes. Content-only edits
    # retain this identity and can be selected with --data at runtime.
    identity = hashlib.sha256(json.dumps(constants, sort_keys=True).encode()).digest()
    package = HEADER.pack(b'MK4D', 2, len(payload), script_begin - BASE, pc - BASE, identity) + payload
    header = ['/* Generated by mk4_link.py. Source: scripts/mk4/project.asm. */',
              '#ifndef MK4_LINKED_H', '#define MK4_LINKED_H',
              '#define MK4_LAYOUT_ID "' + ''.join('\\x%02x' % b for b in identity) + '"']
    header += [f'#define {name} 0x{value:x}u' for name, value in sorted(constants.items())]
    header += ['#endif', '']
    return package, '\n'.join(header), symbols


def write_changed(path, data):
    path = Path(path)
    if path.exists() and path.read_bytes() == data:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + '.tmp')
    tmp.write_bytes(data)
    tmp.replace(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('package', type=Path)
    parser.add_argument('header', type=Path)
    args = parser.parse_args()
    try:
        package, header, _ = link(args.source)
        write_changed(args.package, package)
        write_changed(args.header, header.encode())
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, str(exc) + '\n')


if __name__ == '__main__':
    main()
