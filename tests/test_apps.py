#!/usr/bin/env python3
"""Confirm all three frontends reject the same malformed package payloads."""
from pathlib import Path
import struct
import subprocess
import tempfile

root = Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory() as temp:
    path = Path(temp) / 'FILESYS.DAT'
    cases = [(b'', 'truncated archive index')]
    for label, entries in [
        ('overlapping payload entries', [(1, 0x3000, 5), (2, 0x3001, 4)]),
        ('invalid index ordering or padding', [(2, 0x3000, 1), (1, 0x3001, 1)]),
        ('entry extends outside archive payload', [(1, 0x3000, 500)]),
    ]:
        data = bytearray(0x3008)
        for i, entry in enumerate(entries):
            struct.pack_into('<III', data, i * 12, *entry)
        cases.append((data, label))
    for data, error in cases:
        path.write_bytes(data)
        for app in ['fsys', 'mk4-viewer', 'mk4-game']:
            args = [str(root / app)]
            args += [str(path), '--list'] if app == 'fsys' else ['--archive', str(path), '--self-test']
            result = subprocess.run(args, capture_output=True, text=True, cwd=root)
            assert result.returncode == 1, (app, result)
            assert error in result.stderr, (app, result.stderr)
print('PASS: same truncated, overlapping, unsorted, and out-of-bounds packages rejected by all three apps')
