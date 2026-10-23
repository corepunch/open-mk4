"""Relocatable table/script contracts, preserved scalars and import evidence."""
from pathlib import Path
from tempfile import TemporaryDirectory
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from mk4_link import BASE, HEADER, link
from mk4_asm import OPS
from scorpion_moves import SCRIPT_OPS
from import_mk4_data import export
from retail_moves import RetailImage


class LinkTests(unittest.TestCase):
    def source(self, text):
        temp = TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        path = Path(temp.name) / 'project.asm'
        path.write_text(text)
        return path

    def test_table_and_script_references_follow_insertions(self):
        text = '''.section data
table:
.word word(entry), name, 0x500000
name:
.word 0x004b4d
.section script
entry:
wait 3
jump 0, word(done)
done:
return 0
.export ENTRY entry
.export TABLE table
'''
        first, h1, labels1 = link(self.source(text))
        second, h2, labels2 = link(self.source(text.replace('name:\n', '.zero 20\nname:\n').replace('done:\n', 'wait 7\ndone:\n')))
        for package, symbols in ((first, labels1), (second, labels2)):
            data = package[HEADER.size:]
            self.assertEqual(struct.unpack_from('<III', data), (symbols['entry']//4, symbols['name'], 0x500000))
            self.assertEqual(struct.unpack_from('<I', data, symbols['entry']-BASE+8)[0], symbols['done']//4)
        self.assertEqual(labels2['entry']-labels1['entry'], 20)
        self.assertEqual(labels2['done']-labels1['done'], 24)
        self.assertNotEqual(h1, h2)
        content, h3, _ = link(self.source(text.replace('wait 3', 'wait 9')))
        self.assertEqual(h1, h3)
        self.assertNotEqual(first, content)

    def test_reject_invalid_layout_and_links(self):
        for body in ('jump 0, word(missing)', 'jump 0, word(table)', 'wait 0x1000000',
                     '.org 0x4d0000', '.expect entry 0x4d0000', 'prefixed_to 0x103',
                     '.alias cycle cycle', 'call 0', '.word 0x100000000', 'entry:\nreturn 0'):
            with self.subTest(body=body), self.assertRaises((ValueError, KeyError)):
                link(self.source('.section data\ntable:\n.word 0\n.section script\nentry:\n'+body+'\n'))

    def test_strings_comments_and_continued_operands(self):
        source = self.source(r'''.section data
name:
.string "a;b\"c" ; comment after a quoted semicolon and escaped quote
table:
.word name, \
  word(entry) ; packed reference on the following line
.section script
entry:
jump 0, \
  word(done)
done:
return 0
''')
        package, _, labels = link(source)
        payload = package[HEADER.size:]
        self.assertEqual(payload[:8], b'a;b"c\0\0\0')
        self.assertEqual(struct.unpack_from('<II', payload, labels['table'] - BASE),
                         (labels['name'], labels['entry'] // 4))
        self.assertEqual(struct.unpack_from('<I', payload, labels['entry'] - BASE + 4)[0],
                         labels['done'] // 4)
        for text in ('.string "embedded\\u0000nul"', '.string 42', '.word 0, \\'):
            with self.subTest(text=text), self.assertRaises(ValueError):
                link(self.source('.section data\n' + text + '\n'))

    def test_imported_data_and_wordcode_evidence(self):
        self.assertEqual({op:count for op,count in OPS.values()}, {op:row[1] for op,row in SCRIPT_OPS.items()})
        with TemporaryDirectory() as temp:
            destination = Path(temp) / 'import'
            # Import validates every retained word and every typed relocation;
            # no magnitude-based pointer rewrite can change Q16 values.
            export(RetailImage(ROOT/'MK4/MK-4.EXE'), destination)
            package, header, _ = link(destination/'project.asm')
            self.assertEqual(package[:4], b'MK4D')
        link(ROOT/'scripts/mk4/project.asm')
        self.assertNotIn('.org ', (ROOT/'scripts/mk4/program.asm').read_text())


if __name__=='__main__':
    unittest.main()
