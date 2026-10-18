"""Integration checks: fixed archive fixtures, live polling, real payload identity."""
import csv
import io
import os
from pathlib import Path
import signal
import struct
import subprocess
import sys
import tempfile
import time
import unittest

BINARY = Path(sys.argv.pop(1) if len(sys.argv) > 1 else './fsys').resolve()
ROOT = Path(__file__).resolve().parents[1]


def archive(entries):
    """Entries are (hash, payload), already sorted by hash."""
    table = bytearray(0x3000)
    data = bytearray()
    for i, (key, payload) in enumerate(entries):
        struct.pack_into('<III', table, i * 12, key, 0x3000 + len(data), len(payload))
        data.extend(payload)
    return bytes(table + data)


def wargods_archive(entries):
    table = bytearray(struct.pack('<I', len(entries) + 1))
    offset = 4 + (len(entries) + 1) * 17
    payloads = bytearray()
    for name, payload in entries:
        table.extend(struct.pack('<I13s', offset, name.encode()))
        payloads.extend(payload)
        offset += len(payload)
    table.extend(struct.pack('<I13s', offset, b''))
    return bytes(table + payloads)


def rows(text):
    return list(csv.DictReader(io.StringIO(text), delimiter='\t'))


def eventually(check, timeout=5):
    until = time.monotonic() + timeout
    while time.monotonic() < until:
        if check():
            return
        time.sleep(0.025)
    raise AssertionError('Timed out waiting for watcher')


class FsysTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='mk4-fsys-')
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.source = self.base / 'FILESYS.DAT'
        self.output = self.base / 'out'
        self.payloads = [(1, b'hello'), (2, b'ESF\x06\x03\0\0\0abc'),
                         (3, b'0.1v' + bytes(range(16)))]
        self.source.write_bytes(archive(self.payloads))

    def run_tool(self, *args, expected=0):
        result = subprocess.run([str(BINARY), str(self.source), *map(str, args)],
                                capture_output=True, text=True)
        self.assertEqual(result.returncode, expected, result.stderr)
        return result

    def test_extract_bytes_and_repeat_preserves_mtime(self):
        self.run_tool('-o', self.output)
        manifest = rows((self.output / 'manifest.tsv').read_text())
        self.assertEqual([r['type'] for r in manifest], ['bin', 'esf', 'geo'])
        before = {}
        for row, (_, payload) in zip(manifest, self.payloads):
            target = self.output / row['path']
            self.assertEqual(target.read_bytes(), payload)
            before[target] = target.stat().st_mtime_ns
        result = self.run_tool('-o', self.output)
        self.assertIn('0 files updated', result.stdout)
        self.assertTrue(all(p.stat().st_mtime_ns == t for p, t in before.items()))

    def test_listing_is_read_only(self):
        result = self.run_tool('--list')
        self.assertEqual(len(rows(result.stdout)), 3)
        self.assertFalse(self.output.exists())

    def test_reject_malformed_before_writing(self):
        good = archive(self.payloads)
        cases = [b'', good[:100], bytes(0x3000)]
        for label, offset, value in [('bounds', 8, 0xffffffff),
                                     ('table reference', 4, 1),
                                     ('duplicate hash', 12, 1),
                                     ('zero hash', 0, 0),
                                     ('overlap', 16, 0x3000)]:
            broken = bytearray(good)
            struct.pack_into('<I', broken, offset, value)
            cases.append(bytes(broken))
        hole = bytearray(good)
        hole[12:24] = bytes(12)
        cases.append(bytes(hole))
        for data in cases:
            with self.subTest(size=len(data), prefix=data[:24]):
                self.source.write_bytes(data)
                self.run_tool('-o', self.output, expected=1)
                self.assertFalse(self.output.exists())

    def test_full_index_and_zero_length_entry(self):
        self.source.write_bytes(archive([(i + 1, b'') for i in range(1024)]))
        result = self.run_tool('--list')
        self.assertEqual(len(rows(result.stdout)), 1024)

    def test_known_filename_hash_from_executable(self):
        # Full-path hash observed in the supplied archive, not calculated here.
        self.source.write_bytes(archive([(0x56C2C9B5, b'0.1v' + bytes(8))]))
        exe = self.base / 'fake.exe'
        exe.write_bytes(b'\0sc_geo.geo\0')
        result = self.run_tool('--exe', exe, '--list')
        row = rows(result.stdout)[0]
        self.assertEqual(row['path'], 'geogfx/56c2c9b5_sc_geo.geo')
        self.assertEqual(row['name_status'], 'candidate')
        self.assertEqual(row['original_path_candidate'], r'c:\source\mk4\win\geogfx\sc_geo.geo')

    def test_output_symlink_directory_is_rejected(self):
        self.output.mkdir()
        elsewhere = self.base / 'elsewhere'
        elsewhere.mkdir()
        (self.output / 'other').symlink_to(elsewhere, target_is_directory=True)
        self.run_tool('-o', self.output, expected=1)
        self.assertEqual(list(elsewhere.iterdir()), [])

    def test_wargods_names_payloads_and_validation(self):
        entries = [('0', b'directory'), ('TEST~1.TGA', b'image'),
                   ('ABCDE.W', b'one'), ('EBCDA.W', b'two'), ('EMPTY.W', b'')]
        good = wargods_archive(entries)
        self.source.write_bytes(good)
        self.run_tool('-o', self.output)
        manifest = rows((self.output / 'manifest.tsv').read_text())
        self.assertEqual(len(manifest), len(entries))
        for row, (name, payload) in zip(manifest, entries):
            self.assertEqual(row['name_status'], 'stored')
            self.assertEqual(row['path'], 'files/' + name)
            self.assertEqual((self.output / row['path']).read_bytes(), payload)
        self.assertIn('0 files updated', self.run_tool('-o', self.output).stdout)
        cases = [good[:20], good[:-1]]
        for offset, value in [(21, 1), (21, len(good) + 1), (4 + len(entries) * 17, len(good) - 1)]:
            bad = bytearray(good)
            struct.pack_into('<I', bad, offset, value)
            cases.append(bad)
        for name in [b'../BAD', b'BAD/FILE', b'BAD\\FILE', b'A' * 13, b'']:
            bad = bytearray(good)
            bad[25:38] = name.ljust(13, b'\0')
            cases.append(bad)
        bad = bytearray(good)
        bad[25:38] = b'0'.ljust(13, b'\0')
        cases.append(bad)
        for data in cases:
            self.source.write_bytes(data)
            self.run_tool('--list', expected=1)

    @unittest.skipUnless((ROOT / 'Wargods/WARGODS.DAT').exists(), 'local War Gods archive absent')
    def test_real_wargods_payload_identity(self):
        self.source.write_bytes((ROOT / 'Wargods/WARGODS.DAT').read_bytes())
        self.run_tool('-o', self.output)
        data = self.source.read_bytes()
        manifest = rows((self.output / 'manifest.tsv').read_text())
        self.assertEqual(len(manifest), 558)
        for i, row in enumerate(manifest):
            offset = struct.unpack_from('<I', data, 4 + i * 17)[0]
            end = struct.unpack_from('<I', data, 21 + i * 17)[0]
            name = data[8 + i * 17:21 + i * 17].split(b'\0')[0].decode()
            self.assertEqual(row['original_path_candidate'], name)
            self.assertEqual((self.output / row['path']).read_bytes(), data[offset:end])

    def test_bad_cli(self):
        for args in [('--watch', 'nan'), ('--watch', '0'), ('--watch', '-1'),
                     ('--watch', '0.001'), ('--watch', 'inf'), ('--wat',),
                     ('--list', '-o', str(self.output)), ('--exe',)]:
            self.run_tool(*args, expected=2)

    def test_poll_changes_and_recovery(self):
        log = self.base / 'watch.log'
        with log.open('w+') as stream:
            proc = subprocess.Popen([str(BINARY), str(self.source), '-o',
                                     str(self.output), '--watch', '0.05'],
                                    stdout=stream, stderr=stream)
            def stop():
                if proc.poll() is None:
                    proc.send_signal(signal.SIGINT)
                    try:
                        proc.wait(timeout=5)
                    except subprocess.TimeoutExpired:
                        proc.kill()
                        proc.wait()
            self.addCleanup(stop)
            target = self.output / 'other/00000001.bin'
            eventually(lambda: target.exists() and target.read_bytes() == b'hello')
            original_time = target.stat().st_mtime_ns
            time.sleep(0.15)
            self.assertEqual(target.stat().st_mtime_ns, original_time)
            self.source.write_bytes(b'incomplete')
            eventually(lambda: 'truncated archive' in log.read_text())
            self.assertEqual(target.read_bytes(), b'hello')
            # Atomic replacement and same-size payload edit with restored mtime.
            replacement = self.base / 'replacement'
            replacement.write_bytes(archive([(1, b'world'), *self.payloads[1:]]))
            replacement.replace(self.source)
            eventually(lambda: target.read_bytes() == b'world')
            stamp = self.source.stat()
            with self.source.open('r+b') as f:
                f.seek(0x3000)
                f.write(b'again')
            os.utime(self.source, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
            eventually(lambda: target.read_bytes() == b'again')
            # A temporarily missing archive should recover too.
            self.source.unlink()
            eventually(lambda: 'No such file' in log.read_text())
            self.source.write_bytes(archive([(1, b'final'), *self.payloads[1:]]))
            eventually(lambda: target.read_bytes() == b'final')
            stop()
            self.assertEqual(proc.returncode, 0, log.read_text())

    @unittest.skipUnless((ROOT / 'MK4/FILESYS.DAT').exists(), 'local game archive absent')
    def test_real_archive_all_payloads_identical(self):
        source = ROOT / 'MK4/FILESYS.DAT'
        exe = ROOT / 'MK4/MK-4.EXE'
        command = [str(BINARY), str(source), '-o', str(self.output)]
        if exe.exists():
            command.extend(['--exe', str(exe)])
        result = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        data = source.read_bytes()
        manifest = rows((self.output / 'manifest.tsv').read_text())
        self.assertEqual(len(manifest), 799)
        if exe.exists():
            self.assertEqual(sum(r['name_status'] == 'candidate' for r in manifest), 797)
        for row in manifest:
            index = int(row['index'])
            key, offset, size = struct.unpack_from('<III', data, index * 12)
            self.assertEqual(row['hash'], f'{key:08x}')
            self.assertEqual(int(row['offset']), offset)
            self.assertEqual(int(row['size']), size)
            self.assertEqual((self.output / row['path']).read_bytes(), data[offset:offset + size])


if __name__ == '__main__':
    unittest.main(verbosity=2)
