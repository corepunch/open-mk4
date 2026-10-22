#!/usr/bin/env python3
"""Explicit, read-only EXE disassembly into editable MK4 assembly (never a build step)."""
import argparse
from pathlib import Path
import re

from character_catalog import moves_for
from retail_moves import RetailImage
from scorpion_moves import SCRIPT_OPS, script_program
from mk4_asm import BEGIN, END, NAMES, OPS, assemble

ROOT = Path(__file__).resolve().parents[1]


def collect(image):
    symbols = {}
    for path in ('common/mk4_retail_data_addresses.h', 'game/retail_layout.h', 'game/retail_native_addresses.h', 'game/retail_native.h'):
        for name, value in re.findall(r'\b(\w+)\s*=\s*(0x[0-9a-fA-F]+)', (ROOT / path).read_text()):
            symbols.setdefault(int(value, 16), name)
    roots = {root for c in range(18) for move in moves_for(c).values() for _, root in move['scripts']}
    paths = [path for directory in ('game', 'common') for path in (ROOT / directory).rglob('*') if path.suffix in ('.c', '.h', '.inc')]
    for path in paths:
        for name, value in re.findall(r'\b(\w+)\s*=\s*(0x[0-9a-fA-F]+)', path.read_text()):
            address = int(value, 16)
            if BEGIN < address < END and ('_SCRIPT_' in name or name.endswith('_SCRIPT')):
                roots.add(address)
    rows = {}
    for root in sorted(roots):
        for character in range(16):
            for row in script_program(image, root, character)['instructions']:
                address = int(row['va'], 16)
                if 'operands' not in row or not BEGIN <= address < END:
                    raise ValueError('Undecoded script instruction at ' + row['va'])
                rows[address] = row
    return rows, symbols, roots


def export(image, destination):
    rows, symbols, roots = collect(image)
    # Some legacy entry points deliberately execute a table/operand word as nop.
    # Emit each physical word once and retain those entry labels as aliases.
    words, covered, aliases = {}, set(), {}
    for address, row in sorted(rows.items()):
        if address in covered:
            aliases[address] = row
            continue
        for i, value in enumerate([row['word'], *row['operands']]):
            at = address + i * 4
            if at in covered:
                raise ValueError(f'Overlapping decoded instruction at {at:#x}')
            words[at] = int(value, 16)
            covered.add(at)
    labels = {at: symbols.get(at, f'script_{at:06x}') for at in rows}
    external = {}

    def operand(value, packed=False):
        address = value * 4 if packed else value
        if packed and 0x4fad7c <= address < 0x4fad7c + 612 * 12 and (address - 0x4fad7c) % 12 == 0:
            return f'clip({(address - 0x4fad7c) // 12})'
        if address in labels:
            name = labels[address]
        elif address in symbols:
            name = symbols[address]
            external[name] = address
        else:
            return hex(value)
        return f'word({name})' if packed else name

    body, previous = [], None
    for address, row in sorted(rows.items()):
        if address in aliases:
            continue
        if address != previous:
            body += ['', f'.org {address:#x}']
        body.append(labels[address] + ':')
        op = int(row['opcode'], 16)
        values = [int(w, 16) for w in row['operands']]
        args = [hex(row['inline'])]
        for i, value in enumerate(values):
            packed = ((op in (0x2f,0x33,0x3c,0x3d,0x42,0x44,0x45,0x50,0x6f)) or
                      (op in (0x36,0x43,0x55) and i == 1) or
                      ('clip' in row and i == 0))
            native = (op in (0x32,0x35,0x36,0x38,0x43,0x55) and i == 0) or (op == 0x40 and i == 1)
            args.append(operand(value, packed) if packed or native else hex(value))
        body.append('  ' + NAMES[op] + ' ' + ', '.join(args))
        previous = address + 4 * (1 + len(values))
    header = ['; MK4 PC script source. Assembled by tools/mk4_asm.py; edit this file.',
              '; Imported from the checked EXE; source owns behavior after import.',
              '; Inline fields are 24-bit values; word(label) encodes packed references.',
              '.include "symbols.inc"']
    for address, row in aliases.items():
        header += [f'; Shared operand entry: {NAMES[int(row["opcode"],16)]} {row["inline"]:#x}',
                   f'.equ {labels[address]} {address:#x}']
    header += [f'.expect {labels[address]} {address:#x}' for address in sorted(roots)]
    destination.mkdir(parents=True, exist_ok=True)
    (destination / 'symbols.inc').write_text('; External data/native ABI addresses in the supported MK4 PC image.\n' +
        ''.join(f'.equ {name} {value:#x}\n' for name, value in sorted(external.items())))
    source = destination / 'program.asm'
    source.write_text('\n'.join(header + body) + '\n')
    actual = assemble(source)
    assert actual == words
    print(f'{len(rows)} decoded instructions, {len(words)} words, {len(aliases)} shared operand entries; exact round trip.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    parser.add_argument('--output', type=Path, required=True, help='new destination; existing sources are never overwritten')
    args = parser.parse_args()
    if args.output.exists():
        parser.error('Output already exists; import into a new directory and review the diff')
    export(RetailImage(args.exe), args.output)


if __name__ == '__main__':
    main()
