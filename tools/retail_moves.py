#!/usr/bin/env python3
"""Read animation metadata and the high-kick script graph from the supplied PC build.

Uses only the standard library. Addresses and native continuation edges below
are verified for this EXE hash, not guessed from the reference build's symbols.
This is an evidence extractor, not a general VM decompiler.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct


EXE_SHA256 = '4bb7112eb54093dde5ec3ddcef12166c6cf95d08cd282a70d4da405132730b1d'
DESCRIPTORS = 0x4fad7c
KICK_TABLE = 0x4e5ca4


class RetailImage:
    def __init__(self, path):
        raw = Path(path).read_bytes()
        if hashlib.sha256(raw).hexdigest() != EXE_SHA256:
            raise ValueError('Unsupported retail EXE revision (SHA-256 differs)')
        pe = struct.unpack_from('<I', raw, 0x3c)[0]
        optional_size = struct.unpack_from('<H', raw, pe + 20)[0]
        self.sections = []
        self.memory = bytearray(0x180000)
        for i in range(struct.unpack_from('<H', raw, pe + 6)[0]):
            pos = pe + 24 + optional_size + i * 40
            _, rva, size, offset = struct.unpack_from('<IIII', raw, pos + 8)
            payload = raw[offset:offset + size]
            self.sections.append((rva + 0x400000, payload))
            if rva < len(self.memory):
                self.memory[rva:rva + size] = payload
        self.fixups = {}
        for lo, hi in [(0x4d3e80, 0x4d93f0), (0x4d93f8, 0x4d9d60)]:
            for at in range(lo, hi, 8):
                dst, src = self.word(at), self.word(at + 4)
                self.fixups[dst] = src >> 2
        for dst, value in self.fixups.items():
            struct.pack_into('<I', self.memory, dst - 0x400000, value)

    def word(self, va):
        if va < 0x400000 or va + 4 > 0x400000 + len(self.memory):
            raise ValueError('Address outside image: %#x' % va)
        return struct.unpack_from('<I', self.memory, va - 0x400000)[0]

    def clip(self, packed):
        offset = packed * 4 - DESCRIPTORS
        if offset < 0 or offset % 12 or offset // 12 >= 612:
            raise ValueError('Not an animation descriptor: %#x' % (packed * 4))
        return offset // 12


def clip_inventory(image, archive):
    raw = Path(archive).read_bytes()
    entries = {h: (off, size) for h, off, size in
               struct.iter_unpack('<III', raw[:0x3000]) if h}
    off, size = entries[0x2da2df95]
    ap0 = raw[off:off + size]
    ap1_off, ap1_size = entries[0x2da3df95]
    clips, cursor = {}, 0
    for at in range(512, len(ap0), 4):
        tag, size = struct.unpack_from('<HH', ap0, at)
        if tag == 0xffff:
            break
        clip = tag & 0x7fff
        desc = DESCRIPTORS + clip * 12
        flags = image.word(desc + 8)
        count = image.word(desc + 4)
        if clip in clips or not 0 <= clip < 612 or cursor + size > ap1_size:
            raise ValueError('Invalid animation directory')
        clips[clip] = dict(id=clip, descriptor_va=hex(desc), first=0,
                           frame_count=count, last=count - 1,
                           words_per_frame=flags >> 16, channels=(flags >> 16) - 3,
                           descriptor_flags=hex(flags), compressed=bool(tag & 0x8000),
                           ap1_offset=cursor, archive_offset=ap1_off + cursor,
                           payload_bytes=size)
        cursor += size
    if len(clips) != 612 or cursor != ap1_size:
        raise ValueError('Incomplete animation directory')
    return [clips[i] for i in range(612)]


def kick_graph(image, character):
    """Follow the selected bytecode and its measured native continuations.

    Reject unresearched opcodes/continuations rather than inventing lengths.
    The named parameter blocks are native code data, not VM instructions.
    """
    index = {16: 2, 17: 7}.get(character, character)
    pc = image.word(KICK_TABLE + index * 4) * 4
    result = dict(character=character, action='0x100', entry='0x46a680',
                  selector='0x4e5ca0', selected_script=hex(pc), instructions=[], native=[])
    seen = set()
    while pc not in seen:
        seen.add(pc)
        word = image.word(pc)
        op = word >> 24
        row = dict(va=hex(pc), word=hex(word), opcode=hex(op),
                   handler=hex(image.word(0x4e3988 + op * 4)))
        result['instructions'].append(row)
        pc += 4
        if op == 0x1f:
            row.update(operation='set attack selector', argument=image.word(pc))
            pc += 4
        elif op == 0x20:
            row.update(operation='sound event', argument=word & 255)
        elif op in (0x02, 0x47):
            row.update(operation='open clip and wait', clip=image.clip(image.word(pc)),
                       argument=word & 255)
            pc += 4
        elif op in (0x19, 0x04, 0x06):
            row.update(operation={0x19: 'advance with two pose evaluations per visit',
                                  0x04: 'advance forwards', 0x06: 'advance backwards'}[op],
                       argument=word & 255)
        elif op in (0x03, 0x07):
            row.update(operation='finish forwards' if op == 3 else 'finish backwards')
        elif op == 0x24:
            row.update(operation='release attack bookkeeping')
        elif op == 0x29:
            row.update(operation='set action 0x1006')
        elif op == 0x32:
            target = image.word(pc)
            row.update(operation='native tail call', target=hex(target))
            edges = {
                0x46a760: (0x4e5d30, 0x46a770, 0x4e5c70),
                0x46a790: (0x4e6978, 0x46a7a0, 0x4e6998),
                0x46a6b0: (0x4e5d00, 0x46a6c0, 0x4e5d20),
            }
            if target not in edges:
                raise ValueError('Unresearched native continuation: %#x' % target)
            params, callback, pc = edges[target]
            result['native'].append(dict(entry=hex(target), runner='0x470c60',
                parameter_va=hex(params), parameter_words=[hex(image.word(params + i * 4))
                                                         for i in range(7)],
                continuation=hex(callback), next_script=hex(pc)))
        elif op == 0x3f:
            row.update(operation='return to fighter controller', target='0x46f650')
            break
        else:
            raise ValueError('Unresearched opcode %#x at %#x' % (op, pc - 4))
    else:
        raise ValueError('Unexpected loop in kick script graph')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--archive', default='MK4/FILESYS.DAT')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    image = RetailImage(args.exe)
    report = dict(exe_sha256=EXE_SHA256,
                  scope='All stored clips; high-kick scripts and native recovery edges for Scorpion and Sub-Zero',
                  limitations=['Stored frame count is not move duration or a frame rate.',
                               'Native hit detection, input, sound and interrupted paths need separate verification.',
                               'Other moves and special attacks are not decoded by this graph extractor.'],
                  clips=clip_inventory(image, args.archive),
                  high_kicks=[kick_graph(image, i) for i in (0, 4)])
    text = json.dumps(report, indent=2) + '\n'
    if args.output:
        args.output.write_text(text)
    else:
        print(text, end='')


if __name__ == '__main__':
    main()
