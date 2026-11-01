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
from mk4_native import declaration, c_registry
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

    def test_native_relocations_and_binding_identity(self):
        source = '''.native native_action, call=perform_action
.section data
table:
.word native_action
.section script
entry:
call 0, native_action
ready 0
'''
        for text in (source, '.native native_other\n' + source):
            package, _, symbols = link(self.source(text))
            payload = package[HEADER.size:]
            self.assertEqual(struct.unpack_from('<I', payload)[0], symbols['native_action'])
            self.assertEqual(struct.unpack_from('<I', payload, symbols['entry'] - BASE + 4)[0], symbols['native_action'])
        first, _, _ = link(self.source(source))
        changed, _, _ = link(self.source(source.replace('call=perform_action', 'call=another_action')))
        self.assertNotEqual(first[20:52], changed[20:52])
        self.assertEqual(first[HEADER.size:], changed[HEADER.size:])
        for operand in ('0x44bcd0', 'missing', 'entry'):
            with self.subTest(operand=operand), self.assertRaisesRegex(ValueError, 'declared symbol|missing'):
                link(self.source(source.replace('call 0, native_action', 'call 0, ' + operand)))

    def test_native_imports_check_signatures_and_script_completions(self):
        for source in ('.native native_action, spawn=spawn_action',
                       '.native native_action, tail_script=entry',
                       '.native native_action, call=first, call=second',
                       '.native native_action, call=first, call_global=second'):
            with self.subTest(source=source), self.assertRaises(ValueError):
                declaration(source)
        with self.assertRaisesRegex(ValueError, 'Incompatible native signatures'):
            c_registry({'native_a': {'call': 'function'}, 'native_b': {'gate': 'function'}}, {})
        with self.assertRaisesRegex(ValueError, 'non-script symbol'):
            c_registry({'native_a': {'tail_script': 'data', 'tail_finish': 'inherit'}}, {'dat_data': BASE})

    def test_native_continuation_links_and_parameters(self):
        source = '''.native native_script, script=entry, alternate_script=alternate, prepare=prepare, finish=finish
.native native_record, continuation_arg=consume_record, parameter=dat_record
.section data
record:
.word 7
.section script
entry:
ready 0
alternate:
ready 0
'''
        link(self.source(source))
        for first, second, error in (('alternate_script=alternate', 'alternate_script=record', 'non-script'),
                                     ('parameter=dat_record', 'parameter=dat_missing', 'parameter'),
                                     ('parameter=dat_record', 'parameter=0x100000000', '32 bits'),
                                     ('script=entry', 'script=entry, continuation=finish', 'Conflicting'),
                                     ('continuation_arg=consume_record, parameter=dat_record', 'parameter=7', 'requires')):
            with self.subTest(second=second), self.assertRaisesRegex(ValueError, error):
                link(self.source(source.replace(first, second)))

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

    def test_script_labels_have_a_c_namespace_without_export_aliases(self):
        source = '''.native native_example, call=example
.equ LOOKALIKE 0x1000004
.section data
table:
.word 0
.section script
entry:
call 0, native_example
ready 0
.alias embedded entry + 4
.alias data_alias table
guard:
.zero 4
'''
        _, header, symbols = link(self.source(source))
        for name in ('entry', 'embedded'):
            self.assertIn(f'#define fn_{name} 0x{symbols[name]:x}u', header)
        for name in ('table', 'data_alias', 'guard', 'LOOKALIKE'):
            self.assertNotIn('#define fn_' + name + ' ', header)
        self.assertNotIn('#define entry ', header)
        self.assertNotIn('MK4_LINK_', header)
        moved = source.replace('entry:\n', '.zero 12\nentry:\n')
        _, moved_header, moved_symbols = link(self.source(moved))
        self.assertNotEqual(header, moved_header)
        self.assertIn(f'#define fn_entry 0x{moved_symbols["entry"]:x}u', moved_header)
        with self.assertRaisesRegex(ValueError, 'Duplicate script export'):
            link(self.source(source + '.export fn_entry entry\n'))

    def test_data_labels_and_fields_use_their_declared_section(self):
        source = '''.section data
table:
.word 0
text:
.string "x"
data_end:
.section script
entry:
ready 0
.alias data_field table + 4
.alias field_chain data_field
.alias byte_field text + 1
.alias end_field data_end
.alias code_alias entry
.equ LOOKALIKE table
'''
        _, header, symbols = link(self.source(source))
        for name in ('table', 'text', 'data_end', 'data_field', 'field_chain', 'byte_field', 'end_field'):
            self.assertIn(f'#define dat_{name} 0x{symbols[name]:x}u', header)
            self.assertNotIn('#define fn_' + name + ' ', header)
        for name in ('entry', 'code_alias'):
            self.assertIn(f'#define fn_{name} 0x{symbols[name]:x}u', header)
            self.assertNotIn('#define dat_' + name + ' ', header)
        self.assertNotIn('#define dat_LOOKALIKE ', header)
        with self.assertRaisesRegex(ValueError, 'Duplicate data export'):
            link(self.source(source + '.export dat_table table\n'))

    def test_cpu_policy_records_match_retail_after_native_relocation(self):
        image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        layout = json.loads((ROOT / 'tools/mk4_import_layout.json').read_text())
        locations = {name: int(address, 16) for address, name in layout['names'].items()}
        natives = {int(address, 16): name for name, address in json.loads(SCRIPT_LAYOUT.read_text())['natives'].items()}
        package, _, symbols = link(ROOT / 'scripts/mk4/project.asm')

        def linked_word(name, offset):
            return struct.unpack_from('<I', package, HEADER.size + symbols[name] - BASE + offset)[0]

        for name in ('cpu_choices_close', 'cpu_choices_near', 'cpu_choices_medium', 'cpu_choices_far'):
            original = locations[name]
            count = image.word(original)
            self.assertEqual(linked_word(name, 0), count)
            for i in range(count):
                offset = (i + 1) * 4
                self.assertEqual(linked_word(name, offset), symbols[natives[image.word(original + offset)]])
        for name, end, stride in (('cpu_near_attacks', 'cpu_near_attacks_end', 8),
                                  ('cpu_reaction_weights', 'cpu_reaction_weights_end', 4)):
            for offset in range(0, locations[end] - locations[name], stride):
                self.assertEqual(linked_word(name, offset), image.word(locations[name] + offset))
                if stride == 8:
                    target = image.word(locations[name] + offset + 4)
                    self.assertEqual(linked_word(name, offset + 4), symbols[natives[target]])

    def test_imported_data_and_wordcode_evidence(self):
        self.assertEqual({op:count for op,count in OPS.values()}, {op:row[1] for op,row in SCRIPT_OPS.items()})
        with TemporaryDirectory() as temp:
            destination = Path(temp) / 'import'
            # Import validates every retained word and every typed relocation;
            # no magnitude-based pointer rewrite can change Q16 values.
            export(RetailImage(ROOT/'MK4/MK-4.EXE'), destination)
            package, header, _ = link(destination/'project.asm')
            self.assertEqual(package[:4], b'MK4D')
            self.assertFalse((destination / 'exports.inc').exists())
            self.assertNotIn('MK4_LINK_', header)
        _, header, symbols = link(ROOT/'scripts/mk4/project.asm')
        layout = json.loads(SCRIPT_LAYOUT.read_text())
        names = [name for module in layout['modules'].values() for name in module['entries']]
        self.assertEqual(len(names), len(set(names)))
        self.assertTrue(all(name in symbols for name in names))
        for name in names:
            self.assertIn(f'#define fn_{name} 0x{symbols[name]:x}u', header)
        for name, value in layout['natives'].items():
            self.assertTrue(0 < symbols[name] <= len(layout["natives"]))
            self.assertNotEqual(symbols[name], int(value, 16))
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
                self.assertNotRegex(path.read_text(), r'(?m)^\.export ')
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
            self.assertIn('native_sonya_script_second_cycle_end', original)
            self.assertNotIn('.export_equ', original)
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
