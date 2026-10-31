"""Relocatable table/script contracts, preserved scalars and import evidence."""
from pathlib import Path
from tempfile import TemporaryDirectory
import struct
import json
import re
import shutil
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from mk4_link import BASE, HEADER, link
from mk4_asm import OPS
from scorpion_moves import SCRIPT_OPS
from import_mk4_data import export, export_scripts, verify_scripts, SCRIPT_LAYOUT
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

    def test_export_equ_matches_separate_definition_and_export(self):
        source = '''.equ PRIVATE 7
.export_equ NATIVE 0x496f80
.export_equ DERIVED PRIVATE + 3
.section data
table:
.word NATIVE, DERIVED
.section script
entry:
call 0, NATIVE
ready 0
'''
        expanded = re.sub(r'^\.export_equ (\w+) (.+)$', r'.equ \1 \2\n.export \1 \1', source, flags=re.M)
        self.assertEqual(link(self.source(source)), link(self.source(expanded)))
        for declaration in ('.equ NATIVE 1', '.alias NATIVE entry', '.export NATIVE entry', '.export_equ NATIVE 1'):
            with self.subTest(declaration=declaration), self.assertRaisesRegex(ValueError, 'Duplicate'):
                link(self.source(declaration + '\n' + source))
        with self.assertRaisesRegex(ValueError, 'MISSING'):
            link(self.source(source.replace('.export_equ DERIVED PRIVATE + 3', '.export_equ DERIVED MISSING')))

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
        _, _, symbols = link(ROOT/'scripts/mk4/project.asm')
        layout = json.loads(SCRIPT_LAYOUT.read_text())
        names = [name for module in layout['modules'].values() for name in module['entries']]
        self.assertEqual(len(names), len(set(names)))
        self.assertTrue(all(name in symbols for name in names))
        for name, value in layout['natives'].items():
            self.assertEqual(symbols[name], int(value, 16))
        self.assertGreater(verify_scripts(RetailImage(ROOT / 'MK4/MK-4.EXE'), ROOT / 'scripts/mk4/project.asm'), 0)

    def test_fighter_export_preserves_words_names_and_checks_without_writing(self):
        image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        with TemporaryDirectory() as temp:
            destination = Path(temp)
            shutil.copytree(ROOT / 'scripts/mk4', destination / 'scripts/mk4')
            statistics, changed = export_scripts(image, destination)
            self.assertGreater(statistics['instructions'], statistics['entries'])
            self.assertTrue(changed)
            generated = list((destination / 'fighters').glob('*.asm'))
            generated += [destination / 'scripts/mk4/program.asm', destination / 'game/script/opcodes.h']
            for path in generated:
                self.assertEqual(path.read_bytes(), (ROOT / path.relative_to(destination)).read_bytes(), str(path))
                self.assertNotRegex(path.read_text(), r'(?m)^\.export (\w+) \1$')
            self.assertGreater(verify_scripts(image, destination / 'scripts/mk4/project.asm'), 0)
            self.assertEqual(export_scripts(image, destination, check=True)[1], [])
            # A selector's embedded operand entry must continue into its
            # original next stream even when that stream moved to another file.
            shared = destination / 'fighters/shared.asm'
            original_shared = shared.read_text()
            without_bridge = original_shared.replace('  jump 0, word(scorpion_roundhouse_attacker)\n', '', 1)
            self.assertNotEqual(without_bridge, original_shared)
            shared.write_text(without_bridge)
            with self.assertRaisesRegex(ValueError, 'Fall-through changed'):
                verify_scripts(image, destination / 'scripts/mk4/project.asm')
            shared.write_text(original_shared)
            module = destination / 'fighters/sonya.asm'
            original = module.read_text()
            self.assertIn('sonya_fatality_2_second_cycle:', original)
            self.assertIn('.export_equ MK4_NATIVE_SONYA_SCRIPT_SECOND_CYCLE_END ', original)
            self.assertIn('.export MK4_LINK_MK4_ADDRESSES_MK4_SCRIPT_SONYA_FATALITY_2_SECOND_CYCLE ', original)
            modified = original.replace('  wait 0xf', '  wait 0xe', 1)
            self.assertNotEqual(modified, original)
            module.write_text(modified)
            self.assertEqual(export_scripts(image, destination, check=True)[1], [str(module)])
            self.assertEqual(module.read_text(), modified)
            with self.assertRaisesRegex(ValueError, 'Retail word mismatch'):
                verify_scripts(image, destination / 'scripts/mk4/project.asm')
            missing = destination / 'missing'
            self.assertTrue(export_scripts(image, missing, check=True)[1])
            self.assertFalse(missing.exists())


if __name__=='__main__':
    unittest.main()
