#!/usr/bin/env python3
"""Optional independent x86 oracle. Requires Unicorn; never needed by the viewer.

Executes only the supplied June-1998 EXE's pure animation/matrix routines in an
isolated emulator. Compares all decoded words and the saved matrix fixtures.
"""
import argparse
import json
import math
from pathlib import Path
import struct
import subprocess
import tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', default='MK4/FILESYS.DAT')
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--viewer', default='./mk4-viewer')
    args = parser.parse_args()
    archive = Path(args.archive).read_bytes()
    entries = {h: archive[o:o+n] for h, o, n in
               struct.iter_unpack('<III', archive[:0x3000]) if h}
    ap0, ap1 = entries[0x2da2df95], entries[0x2da3df95]
    raw = Path(args.exe).read_bytes()
    pe = struct.unpack_from('<I', raw, 60)[0]
    opt, sections = struct.unpack_from('<H', raw, pe+20)[0], struct.unpack_from('<H', raw, pe+6)[0]
    machine = Uc(UC_ARCH_X86, UC_MODE_32)
    machine.mem_map(0x400000, 0x1600000)
    for i in range(sections):
        p = pe+24+opt+i*40
        _, va, size, off = struct.unpack_from('<IIII', raw, p+8)
        machine.mem_write(va+0x400000, raw[off:off+size])

    def word(va, value):
        machine.mem_write(va, struct.pack('<I', value))

    def get(va):
        return struct.unpack('<I', machine.mem_read(va, 4))[0]

    def call(va, arguments=()):
        machine.mem_write(0x1800000, struct.pack('<%dI' % (len(arguments)+1), 0x1700000, *arguments))
        machine.reg_write(UC_X86_REG_ESP, 0x1800000)
        machine.emu_start(va, 0x1700000, count=1000000)
        assert machine.reg_read(UC_X86_REG_ESP) == 0x1800004, 'Routine did not return'

    machine.mem_write(0x1000000, ap1)
    machine.mem_write(0x51e518, ap0[:512])
    clips, offset = {}, 0
    for p in range(512, len(ap0), 4):
        tag, size = struct.unpack_from('<HH', ap0, p)
        if tag == 0xffff:
            break
        aid = tag & 0x7fff
        rec = 0x4fad7c+aid*12
        flags, frames = get(rec+8), get(rec+4)
        clips[aid] = (tag, offset, frames, flags >> 16)
        word(rec, 0x1000000+offset)
        word(rec+8, (flags & ~0x8000) | (tag & 0x8000))
        offset += size
    assert offset == len(ap1) and len(clips) == 612
    count = 0
    with tempfile.TemporaryDirectory(prefix='mk4-oracle-') as temp:
        dump = Path(temp)/'frames.txt'
        subprocess.run([args.viewer, '--archive', args.archive, '--exe', args.exe,
                        '--self-test', '--dump-frames', str(dump)], check=True, stdout=subprocess.DEVNULL)
        for line in dump.read_text().splitlines():
            fields = line.split()
            aid, frame = map(int, fields[:2])
            actual = tuple(int(w, 16) for w in fields[2:])
            tag, offset, frames, words = clips[aid]
            if tag & 0x8000:
                for va, value in [(0x51dea4, 0x1000000+offset), (0x51dea0, frame),
                                  (0x51dedc, 0x1500000), (0x51dee0, words-3), (0x51dee4, frames)]:
                    word(va, value)
                call(0x4013a0)
                pointer = 0x1500000
            else:
                call(0x401000, (0x4fad7c+aid*12 >> 2, frame, 0))
                pointer = machine.reg_read(UC_X86_REG_EAX)*4
            expected = struct.unpack('<%dI' % words, machine.mem_read(pointer, words*4))
            assert actual == expected, 'Decoded words differ at clip %d frame %d' % (aid, frame)
            count += 1
    assert count == 36723
    machine.mem_write(0x7aa4a0, b''.join(struct.pack('<i', int(math.cos(i*math.tau/4096)*(1 << 28)+.5))
                                       for i in range(4096)))
    fixtures = json.loads((Path(__file__).parent/'fixtures/original_matrices.json').read_text())
    for f in fixtures:
        machine.mem_write(0x1500000, struct.pack('<3h', *[-a for a in f['angles']]))
        call({1: 0x4b2980, 2: 0x4b2c00}[f['kind']], (0x1500000, 0x1501000))
        assert list(struct.unpack('<9h', machine.mem_read(0x1501000, 18))) == f['q12']
    print('PASS: %d frames match original x86 routines; %d matrix fixtures verified' % (count, len(fixtures)))


if __name__ == '__main__':
    main()
