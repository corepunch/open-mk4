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
