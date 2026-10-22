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

# Build a changed source without reading the EXE; the C VM must execute it.
from pathlib import Path
import struct
import sys
from tempfile import TemporaryDirectory
sys.path.insert(0, 'tools')
from mk4_asm import assemble, encode
with TemporaryDirectory() as directory:
    source = Path(directory) / 'fixture.asm'
    package = Path(directory) / 'fixture.scripts'
    source.write_text('.org 0x4d0000\nwait 3\nreturn 0\n')
    data = encode(assemble(source))
    package.write_bytes(data)
    subprocess.run(['build/test-retail-vm', 'program', str(package)], check=True, timeout=10)
    for broken in (data[:-1], b'BAD!' + data[4:], data[:12] + data[12:20] * 2):
        package.write_bytes(broken)
        result = subprocess.run(['build/test-retail-vm', 'program', str(package)], capture_output=True, timeout=10)
        assert result.returncode != 0, 'malformed assembled package was accepted'
print('PASS: project script assembly reaches the C VM; malformed packages are rejected')
