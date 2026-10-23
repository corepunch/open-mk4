"""Script starts without a runtime or outside the script section must stop with diagnostics; malformed packages are rejected."""
import subprocess

for mode, message in {
    'closed': 'Lua runtime is not open',
    'range': 'outside the linked script section',
    'callback': 'Invalid native address continuation',
}.items():
    result = subprocess.run(['build/test-script-runtime', mode], capture_output=True, text=True, timeout=10)
    assert result.returncode != 0, mode
    assert message in result.stderr, (mode, result.stderr)
print('PASS: closed runtime, out-of-section entries and native/script completion type errors rejected')

# The shared loader validates package length, version and layout identity.
from pathlib import Path
from tempfile import TemporaryDirectory
with TemporaryDirectory() as directory:
    package = Path(directory) / 'fixture.data'
    data = Path('build/mk4.data').read_bytes()
    bad_layout = bytearray(data)
    bad_layout[20] ^= 1
    old_version = bytearray(data)
    old_version[4] = 1
    for broken in (data[:-1], b'BAD!' + data[4:], data + b'x', bad_layout, old_version):
        package.write_bytes(broken)
        result = subprocess.run(['build/test-script-runtime', 'load', str(package)], capture_output=True, timeout=10)
        assert result.returncode != 0, 'malformed linked package was accepted'
print('PASS: malformed packages, layout mismatch and old package versions rejected')
