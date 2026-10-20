#!/usr/bin/env python3
"""Action research preserves evidence, scope, confidence and the shared C catalog."""
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_actions import action_text, catalog, native_evidence, portable_references, script_writers, NATIVE
from retail_moves import RetailImage


class ActionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_shared_catalog_and_uncertainty(self):
        entries = catalog()
        self.assertGreaterEqual(len(entries), 53)
        self.assertEqual(entries[0x1007]['name'], 'ACTION_RETAIL_1007')
        self.assertEqual(entries[0x1007]['confidence'], 'inferred')
        self.assertEqual(entries[0x1003]['confidence'], 'verified')
        self.assertEqual(entries[0x100]['confidence'], 'verified')
        self.assertEqual(entries[0x1005]['confidence'], 'unresolved')
        self.assertIn('#include "retail_actions.inc"', (ROOT / 'game/game.h').read_text())

    def test_native_evidence_matches_and_is_not_silently_trusted(self):
        for value in NATIVE:
            self.assertTrue(native_evidence(self.image, value))
        changed = RetailImage(ROOT / 'MK4/MK-4.EXE')
        struct.pack_into('<I', changed.memory, 0x459767 - 0x400000, 0x1006)
        with self.assertRaisesRegex(ValueError, 'evidence mismatch'):
            native_evidence(changed, 0x1007)

    def test_script_roster_and_opcode_boundaries(self):
        scorpion = '\n'.join(script_writers(self.image, 0x1007, 0))
        ice = '\n'.join(script_writers(self.image, 0x1007, 4))
        self.assertIn('004ed470', scorpion)
        self.assertIn('004ed498', scorpion)
        self.assertNotIn('004ed5b4', scorpion)
        self.assertIn('004ed5b4', ice)
        self.assertNotIn('004ed470', ice)
        changed = RetailImage(ROOT / 'MK4/MK-4.EXE')
        struct.pack_into('<I', changed.memory, 0x4ed5b4 - 0x400000, 0x29000000)
        self.assertNotIn('004ed5b4', '\n'.join(script_writers(changed, 0x1007, 4)))
        self.assertIn('004ed5b4', '\n'.join(script_writers(changed, 0x1006, 4)))

    def test_portable_navigation_unknown_and_limit(self):
        entries = catalog()
        refs = '\n'.join(portable_references(0x1007, entries))
        self.assertIn('game/retail_vm.c:', refs)
        self.assertIn('game/sub_zero/generated/scripts.inc:', refs)
        self.assertIn('ACTION_RETAIL_1007', refs)
        self.assertIn('**reader**', '\n'.join(portable_references(0x1003, entries)))
        unknown = action_text(self.image, 0xdeadbeef)
        self.assertIn('Uncatalogued action', unknown)
        self.assertNotIn('ACTION_RETAIL_DEADBEEF', unknown)
        limited = action_text(self.image, 0x1007, limit=1)
        self.assertEqual(limited.count('- **writer**') + limited.count('- **reader**'), 1)
        self.assertIn('omitted by --limit', limited)
        self.assertIn('not exhaustively indexed', limited)
        self.assertIn('RoundOverFsmCluster', limited)

    def test_cli_read_only_output_and_bad_arguments(self):
        tool = str(ROOT / 'tools/disassemble_retail.py')
        base = [sys.executable, tool, '--exe', str(ROOT / 'MK4/MK-4.EXE'), '--archive', 'missing']
        with tempfile.TemporaryDirectory() as directory:
            result = subprocess.run([*base, '--character', 'sub_zero', '--action', '1007'], cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('004ed5b4', result.stdout)
            self.assertEqual(list(Path(directory).iterdir()), [])
            output = Path(directory) / 'action.md'
            subprocess.run([*base, '--action', '0x1007', '--output', str(output)], cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('ACTION_RETAIL_1007', output.read_text())
            for args in (['--action', '-1'], ['--action', '100000000'], ['--action', 'bad-code'],
                         ['--action', '1007', '--native', '459760'], ['--action', '1007', '--c'], ['--action', '1007', '--raw']):
                bad = subprocess.run([*base, *args], cwd=directory, capture_output=True, text=True)
                self.assertNotEqual(bad.returncode, 0, args)


if __name__ == '__main__':
    unittest.main(verbosity=2)
