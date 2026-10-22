"""MK4 assembler contracts: references, optional operands and executable round trip."""
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from mk4_asm import assemble, encode, OPS
from import_mk4_scripts import export
from retail_moves import RetailImage
from scorpion_moves import SCRIPT_OPS


class AssemblyTests(unittest.TestCase):
    def source(self, text):
        directory = TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        path = Path(directory.name) / 'test.asm'
        path.write_text(text)
        return path

    def test_labels_packed_references_and_optional_clip(self):
        words = assemble(self.source('''.org 0x4d0000
start:
  prefixed_to 0x203, clip(14)
  jump 0, word(done)
  wait 7
done:
  return 0
'''))
        self.assertEqual(words[0x4d0000], 0x47000203)
        self.assertEqual(words[0x4d0004], (0x4fad7c + 14 * 12) // 4)
        self.assertEqual(words[0x4d000c], 0x4d0014 // 4)
        changed = assemble(self.source('.org 0x4d0000\nwait 9\nreturn 0\n'))
        self.assertEqual(changed[0x4d0000], 0x34000009)
        self.assertTrue(encode(changed).startswith(b'MK4S'))

    def test_invalid_programs_fail_instead_of_truncating_or_overwriting(self):
        for body in ('wait 0x1000000', 'jump 0, word(missing)', 'prefixed_to 0x103',
                     '.expect end 0x4d0000\nwait 1\nend:\nreturn 0', 'call 0', 'wait 1\n.org 0x4d0000\nreturn 0', 'damage 0, 0x100000000'):
            with self.subTest(body=body), self.assertRaises(ValueError):
                assemble(self.source('.org 0x4d0000\n' + body + '\n'))
        with self.assertRaises(ValueError):
            assemble(self.source('.org 0x4ef000\nreturn 0\n'))

    def test_import_round_trip_and_decoder_operand_contract(self):
        self.assertEqual({op: count for op, count in OPS.values()}, {op: row[1] for op, row in SCRIPT_OPS.items()})
        image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        with TemporaryDirectory() as directory:
            destination = Path(directory) / 'imported'
            export(image, destination)
            words = assemble(destination / 'program.asm')
            self.assertGreater(len(words), 4000)
            for address, value in words.items():
                self.assertEqual(value, image.word(address), hex(address))


if __name__ == '__main__':
    unittest.main()
