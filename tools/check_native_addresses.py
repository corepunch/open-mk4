#!/usr/bin/env python3
"""Report native, script and data address naming in the shared C runtime."""
from pathlib import Path
import re
import sys
from c_source import mask_c

ROOT = Path(__file__).resolve().parents[1]


def address_definitions(source):
    """Locate literal constant definitions, never ordinary assignments."""
    result = []
    for enum in re.finditer(r'\benum(?:\s+\w+)?\s*\{[^}]*\}', source, re.S):
        for match in re.finditer(r'\b(\w+)\s*=\s*(0x[0-9a-fA-F]+)', enum[0]):
            result.append((int(match[2], 16), enum.start() + match.start(2)))
    for match in re.finditer(
            r'#define\s+\w+\s+(0x[0-9a-fA-F]+)|\b(?:static\s+)?const\s+uint32_t\s+\w+\s*=\s*(0x[0-9a-fA-F]+)', source):
        result.append((int(match[1] or match[2], 16), match.start(1) if match[1] else match.start(2)))
    return result


def category(address):
    if 0x401000 <= address < 0x4d0000:
        return 'Native'
    if address == 0x400000 or 0x4d0000 <= address <= 0x580000:
        return 'Script/data'
    return None


def main():
    named = {'Native': set(), 'Script/data': set()}
    literals = []
    vague = []
    paths = [path for directory in ('game', 'common') for path in (ROOT / directory).rglob('*')]
    for path in sorted(paths):
        if path.suffix not in ('.c', '.h', '.inc'):
            continue
        source = mask_c(path.read_text())
        for match in re.finditer(
                r'\bn[0-9a-f]{6}_\w+|\bRETAIL_NATIVE_(?:NATIVE_VM|VM_ENTER)_\w+|'
                r'\bMK4_RETAIL_SCRIPT_(?:NATIVE_VM|LK_SCRIPT)_\w+|'
                r'\bretail_(?:weapon_native|scene)_[0-9a-f]{6}\w*|'
                r'\b[A-Za-z_]\w*_[45][0-9a-fA-F]{5}(?:_\w+)?\b', source):
            vague.append((path.relative_to(ROOT), source.count('\n', 0, match.start()) + 1, match[0]))
        allowed = set()
        for address, offset in address_definitions(source):
            group = category(address)
            if group:
                named[group].add(address)
                allowed.add(offset)
        for match in re.finditer(r'\b0x[0-9a-fA-F]+\b', source):
            address = int(match[0], 16)
            group = category(address)
            if group and match.start() not in allowed:
                literals.append((group, address, path.relative_to(ROOT), source.count('\n', 0, match.start()) + 1))
    completed_total = total_total = 0
    for group, definitions in named.items():
        remaining = [row for row in literals if row[0] == group]
        unnamed = {row[1] for row in remaining}
        total = len(definitions | unnamed)
        completed = len(definitions - unnamed)
        percent = 100 * completed / total if total else 100
        print(f'{group} address naming: {completed}/{total} unique addresses ({percent:.1f}%); {len(remaining)} raw use sites remain.')
        completed_total += completed
        total_total += total
    percent = 100 * completed_total / total_total if total_total else 100
    print(f'Overall: {completed_total}/{total_total} ({percent:.1f}%).')
    for _, address, path, line in literals:
        print(f'{path}:{line}: unnamed retail address {address:#x}')
    print(f'Address-based/dispatcher placeholder labels: {len(vague)}.')
    for path, line, symbol in vague:
        print(f'{path}:{line}: replace placeholder {symbol} with a verified role name')
    return bool(literals or vague)


if __name__ == '__main__':
    sys.exit(main())
