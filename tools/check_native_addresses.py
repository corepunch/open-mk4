#!/usr/bin/env python3
"""Report native-address naming coverage in the shared C runtime."""
from pathlib import Path
import re
import sys
from c_source import mask_c

ROOT = Path(__file__).resolve().parents[1]
CATALOGS = {'retail_native_addresses.h', 'retail_native.h'}


def main():
    names = {}
    literals = []
    for path in sorted((ROOT / 'game').rglob('*')):
        if path.suffix not in ('.c', '.h', '.inc'):
            continue
        source = mask_c(path.read_text())
        definitions = list(re.finditer(r'\b(RETAIL_NATIVE_\w+)\s*=\s*(0x[0-9a-fA-F]+)', source))
        allowed = set()
        if path.name in CATALOGS:
            for match in definitions:
                address = int(match[2], 16)
                if 0x401000 <= address < 0x4d0000:
                    if address in names:
                        raise ValueError('Duplicate native address definition: ' + match[2])
                    names[address] = match[1]
                    allowed.add(match.start(2))
        for match in re.finditer(r'\b0x[0-9a-fA-F]+\b', source):
            address = int(match[0], 16)
            if 0x401000 <= address < 0x4d0000 and match.start() not in allowed:
                literals.append((address, path.relative_to(ROOT), source.count('\n', 0, match.start()) + 1))
    unnamed = {address for address, _, _ in literals}
    total = len(set(names) | unnamed)
    completed = len(set(names) - unnamed)
    percent = 100 * completed / total if total else 100
    print(f'Native address naming: {completed}/{total} unique addresses ({percent:.1f}%); {len(literals)} raw use sites remain.')
    for address, path, line in literals:
        print(f'{path}:{line}: unnamed native address {address:#x}')
    return bool(literals)


if __name__ == '__main__':
    sys.exit(main())
