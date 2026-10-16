#!/usr/bin/env python3
"""Inspect a retail PE for compiler/language evidence without executing it.

The optional entry disassembly needs Capstone. Absence of symbols, RTTI or
exception markers does not prove a source language. CRT diagnostics likewise
do not prove that the application uses the corresponding language feature.
"""
import argparse
import hashlib
import re
import struct
from pathlib import Path


def inspect(path, disassemble=False):
    raw = path.read_bytes()

    def unpack(fmt, offset):
        return struct.unpack_from(fmt, raw, offset)

    if raw[:2] != b'MZ':
        raise ValueError('Expected an MZ/PE executable')
    pe, = unpack('<I', 0x3c)
    if raw[pe:pe + 4] != b'PE\0\0':
        raise ValueError('Expected a PE signature')
    machine, section_count, _, symbol_table, symbol_count, optional_size, _ = unpack('<HHIIIHH', pe + 4)
    optional = pe + 24
    if machine != 0x14c or unpack('<H', optional)[0] != 0x10b:
        raise ValueError('Expected 32-bit x86 PE')
    image_base, = unpack('<I', optional + 28)
    entry, = unpack('<I', optional + 16)
    sections = []
    for i in range(section_count):
        pos = optional + optional_size + i * 40
        name = raw[pos:pos + 8].split(b'\0')[0].decode('ascii', 'replace')
        virtual_size, rva, size, offset = unpack('<IIII', pos + 8)
        if offset + size > len(raw):
            raise ValueError('Truncated section')
        sections.append((name, rva, virtual_size, offset, size))

    def file_offset(rva):
        for _, va, _, offset, size in sections:
            if va <= rva < va + size:
                return offset + rva - va
        raise ValueError('RVA outside initialized sections: %x' % rva)

    def string(offset):
        return raw[offset:raw.index(0, offset)].decode('ascii', 'replace')

    directories, = unpack('<I', optional + 92)

    def directory(index):
        return unpack('<II', optional + 96 + index * 8) if index < directories else (0, 0)

    imports = []
    rva, _ = directory(1)
    if rva:
        pos = file_offset(rva)
        while any(raw[pos:pos + 20]):
            original, _, _, name, thunk = unpack('<IIIII', pos)
            symbols = []
            at = file_offset(original or thunk)
            while True:
                value, = unpack('<I', at)
                if not value:
                    break
                symbols.append('#%d' % (value & 0xffff) if value & 0x80000000 else string(file_offset(value) + 2))
                at += 4
            imports.append((string(file_offset(name)), symbols))
            pos += 20

    strings = [(match.start(), match.group().decode('ascii'))
               for match in re.finditer(rb'[ -~]{5,}', raw)]
    strings.extend((match.start(), match.group().decode('utf-16le'))
                   for match in re.finditer(rb'(?:[ -~]\x00){5,}', raw))
    # A standalone basename or full source path, not incidental machine-code text.
    source_pattern = re.compile(r'(?:[A-Za-z]:[\\/])?(?:[\w.-]+[\\/])*[\w.-]+\.(?:c|cpp|cxx|cc)(?=$|[\s:\)])', re.I)
    sources = [(at, text) for at, text in strings if source_pattern.search(text)]
    roots = [(at, text) for at, text in strings if re.search(r'[A-Za-z]:\\source\\', text, re.I)]
    crt = [(at, text) for at, text in strings if 'Microsoft Visual' in text or re.search(r'R60\d\d|pure virtual function', text)]
    rtti = [(at, text) for at, text in strings if re.search(r'\.\?A[UV].*@@', text)]
    print('File:', path)
    print('SHA-256:', hashlib.sha256(raw).hexdigest())
    print('PE linker version: %d.%02d' % (raw[optional + 2], raw[optional + 3]))
    print('Entry VA: 0x%08x' % (image_base + entry))
    print('COFF symbol table: offset=0x%x, symbols=%d' % (symbol_table, symbol_count))
    print('PE debug directory: RVA=0x%x, size=%d' % directory(6))
    print('PE export directory: RVA=0x%x, size=%d' % directory(0))
    for label, values in [('Source filename candidates', sources), ('Source-root asset paths', roots), ('CRT diagnostics', crt), ('C++ RTTI name candidates', rtti)]:
        print('%s: %d' % (label, len(values)))
        for at, text in values:
            print('  file+0x%08x: %s' % (at, text))
    print('Raw C++ exception marker 0xe06d7363 occurrences:', raw.count(struct.pack('<I', 0xe06d7363)))
    print('Raw 0x19930520 marker occurrences (also used by CRT NLG):', raw.count(struct.pack('<I', 0x19930520)))
    print('Imported DLLs:', ', '.join(dll for dll, _ in imports))
    print('Inference: MSVC-era CRT/linker evidence; no definitive C versus C++ source-language proof.')
    if disassemble:
        from capstone import Cs, CS_ARCH_X86, CS_MODE_32
        offset = file_offset(entry)
        print('Entry disassembly (x86; no code executed):')
        for instruction in Cs(CS_ARCH_X86, CS_MODE_32).disasm(raw[offset:offset + 192], image_base + entry):
            print('  0x%08x: %-8s %s' % (instruction.address, instruction.mnemonic, instruction.op_str))
            if instruction.mnemonic == 'ret':
                break


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', nargs='?', type=Path, default=Path('MK4/MK-4.EXE'))
    parser.add_argument('--disassemble-entry', action='store_true')
    args = parser.parse_args()
    inspect(args.exe, args.disassemble_entry)


if __name__ == '__main__':
    main()
