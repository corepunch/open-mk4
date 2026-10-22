"""Malformed runtime streams must stop with useful diagnostics, never hang."""
import subprocess

for mode, message in {
    'opcode': 'unsupported opcode',
    'native': 'unreviewed native binding',
    'sample': 'animation sample outside clip',
    'budget': 'instruction budget exhausted',
    'range': 'script cursor outside reviewed range',
}.items():
    result = subprocess.run(['build/test-retail-vm', mode], capture_output=True,
                            text=True, timeout=10)
    assert result.returncode != 0, mode
    assert 'Retail VM at ' in result.stderr and message in result.stderr, (mode, result.stderr)

result = subprocess.run(['build/test-retail-vm', 'callback'], capture_output=True, text=True, timeout=10)
assert result.returncode != 0 and 'Invalid native address continuation' in result.stderr, result.stderr

print('PASS: malformed VM streams and script/native completion type errors rejected')

# The shared loader validates package length, layout identity and script membership.
from pathlib import Path
from tempfile import TemporaryDirectory
import struct
with TemporaryDirectory() as directory:
    package = Path(directory) / 'fixture.data'
    data = Path('build/mk4.data').read_bytes()
    bad_layout = bytearray(data)
    bad_layout[20] ^= 1
    bad_mask = bytearray(data)
    size = struct.unpack_from('<I', data, 8)[0]
    bad_mask[52 + size] |= 1
    for broken in (data[:-1], b'BAD!' + data[4:], data + b'x', bad_layout, bad_mask):
        package.write_bytes(broken)
        result = subprocess.run(['build/test-retail-vm', 'load', str(package)], capture_output=True, timeout=10)
        assert result.returncode != 0, 'malformed linked package was accepted'
print('PASS: malformed packages, layout mismatch and invalid script membership rejected')
