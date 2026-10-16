#!/usr/bin/env python3
"""Check retail script decoding and the optional native disassembly navigator."""
import importlib.util
import json
from pathlib import Path
import re
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from retail_moves import RetailImage
from scorpion_moves import script_program
from disassemble_retail import Disassembly


class ScriptTests(unittest.TestCase):
    def setUp(self):
        self.image = RetailImage('MK4/MK-4.EXE')

    def test_original_normal_vm_fetches(self):
        # Actual original execution includes native continuations into these
        # roots. Every executed fetch must remain a decoded opcode boundary.
        roots = [0x4e62f0, 0x4e6330, 0x4e6360, 0x4e63b0, 0x4e5ca0,
                 0x4e5c70, 0x4e6798, 0x4e62a0]
        rows = {row['va']: row['word'] for root in roots
                for row in script_program(self.image, root)['instructions']}
        fixture = json.loads(Path('tests/fixtures/original_scorpion.json').read_text())
        for scenario in fixture['moves']:
            for fetch in scenario['commands']:
                self.assertEqual(rows[fetch['pc']], fetch['word'], scenario['name'])

    def test_optional_descriptor_operand(self):
        # Opcode 0x47 calls 0x45bf80: ONLY byte 1 == 2 consumes a descriptor.
        address = 0x570000
        for word, operands in [(0x47000005, []), (0x47000205, [(0x4fad7c + 293 * 12) // 4])]:
            values = [word, *operands, 0x3f000000]
            struct.pack_into('<%dI' % len(values), self.image.memory, address - 0x400000, *values)
            rows = script_program(self.image, address)['instructions']
            self.assertEqual(len(rows), 2)
            self.assertEqual(int(rows[-1]['va'], 16), address + 4 * (1 + len(operands)))
            if operands:
                self.assertEqual(rows[0]['clip'], 293)
            else:
                self.assertNotIn('clip', rows[0])

    def test_loop_and_character_selector(self):
        rows = script_program(self.image, 0x4ed588)['instructions']
        loop = next(row for row in rows if row['opcode'] == '0x3b')
        self.assertEqual(loop['branch'], '0x4ed5bc')
        kick = script_program(self.image, 0x4e5ca0)['instructions']
        table = next(row for row in kick if row['opcode'] == '0x3c')
        self.assertEqual(len(table['character_table']), 16)
        self.assertEqual(table['character_table'][0], '0x4e6c90')
        self.assertEqual([row['clip'] for row in kick if 'clip' in row], [66])

    def test_zero_opcode_and_park(self):
        # A retail selector points at an overlapping final table word whose
        # high opcode byte is zero. Treating it as an unknown loses the clip.
        rows = script_program(self.image, 0x4e63d8)['instructions']
        self.assertTrue(any(row['va'] == '0x4e641c' and row['opcode'] == '0x0' for row in rows))
        self.assertEqual([row['clip'] for row in rows if 'clip' in row], [58])
        parked = script_program(self.image, 0x4e5bc8)['instructions']
        self.assertEqual(parked[-1]['opcode'], '0x3e')

    def test_scorpion_blocking_opcodes(self):
        # Static handler evidence: 51 -> query B, 62/26 consume only inline;
        # 5a fetches one word and negates it. 30 pops the continuation stack.
        for root in (0x4e6d08, 0x4e60b8, 0x4e9d00, 0x4e9938, 0x4e97f8):
            program = script_program(self.image, root)
            self.assertFalse(any('unresearched' in row['operation'] for row in program['instructions']), hex(root))
            self.assertIn(program['instructions'][-1]['opcode'], ('0x30', '0x32', '0x3f'))
            self.assertFalse(program['whole_move_verified'])
        low_kick = script_program(self.image, 0x4e6d08)['instructions']
        query = next(row for row in low_kick if row['opcode'] == '0x51')
        self.assertEqual(query['native_target'], '0x48d3f0')
        self.assertEqual(query['contact']['selector'], 6)
        wait = next(row for row in low_kick if row['opcode'] == '0x37')
        self.assertEqual((wait['wait_if_clear'], wait['wait_if_set']), (5, 8))
        spear = script_program(self.image, 0x4e9d00)['instructions']
        self.assertEqual([row['va'] for row in spear[:3]], ['0x4e9d00', '0x4e9d04', '0x4e9d08'])
        self.assertEqual(spear[0]['native_target'], '0x40a470')

    def test_motion_and_continuation_boundaries(self):
        address = 0x570000
        struct.pack_into('<6I', self.image.memory, address - 0x400000,
                         0x5a000000, 0x1999, 0x61000000, 0x30000000, 0xff000000, 0)
        program = script_program(self.image, address)
        self.assertEqual([row['va'] for row in program['instructions']], ['0x570000', '0x570008', '0x57000c'])
        self.assertEqual(program['instructions'][0]['speed_q16'], -0x1999)
        self.assertEqual(program['instructions'][1]['native_target'], '0x490330')
        self.assertEqual(program['unresolved_native_edges'][-1]['kind'], 'dynamic continuation return')

    def test_unknown_is_explicit_terminal(self):
        address = 0x570000
        struct.pack_into('<II', self.image.memory, address - 0x400000, 0xff000000, 0x3f000000)
        program = script_program(self.image, address)
        self.assertEqual(len(program['instructions']), 1)
        self.assertIsNone(program['instructions'][0]['handler'])
        self.assertEqual(program['unresolved_native_edges'][0]['kind'], 'unknown opcode')
        self.assertFalse(program['whole_move_verified'])


@unittest.skipUnless(importlib.util.find_spec('capstone'), 'Capstone is optional; install to check native disassembly')
class NativeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.disassembly = Disassembly('MK4/MK-4.EXE')
        cls.functions = cls.disassembly.closure()

    def test_native_switch_and_callback_closure(self):
        uppercut = self.functions[0x46cde0]
        self.assertEqual([(table['va'], table['count']) for table in uppercut['tables']], [(0x46d1a8, 8)])
        spear = self.functions[0x49a6a0]
        self.assertEqual([(table['va'], table['count']) for table in spear['tables']], [(0x49ae74, 7)])
        self.assertIn(('callback_store', 0x49a6a0), spear['edges'])
        for entry in [0x49b170, 0x49c810, 0x480040, 0x467ce0, 0x467cd0, 0x467e00, 0x467e70,
                      0x43cbb0, 0x43bd70, 0x41f570, 0x48de80]:
            self.assertTrue(self.functions[entry]['instructions'])
        # An indexed JMP with a modified index must not inherit the CMP's bound.
        cs = self.disassembly.cs
        bad = list(cs.disasm(bytes.fromhex('83f807 7710 40 ff2485a8d14600'), 0x570000))
        self.assertIsNone(self.disassembly.switch(bad[-1], bad[:-1]))
        changed_flags = list(cs.disasm(bytes.fromhex('83f807 85db 7710 ff2485a8d14600'), 0x570000))
        self.assertIsNone(self.disassembly.switch(changed_flags[-1], changed_flags[:-1]))

    def test_every_native_byte_and_static_edge(self):
        for entry, function in self.functions.items():
            self.assertFalse(any(site['reason'] == 'instruction budget reached' for site in function['unresolved']))
            for address, insn in function['instructions'].items():
                self.assertEqual(bytes(insn.bytes), self.disassembly.raw_bytes(address, insn.size))
            for _, target in function['edges']:
                if self.disassembly.code(target):
                    self.assertIn(target, self.functions, (hex(entry), hex(target)))
        self.assertIn(0x41f64a, [site['at'] for site in self.functions[0x41f570]['unresolved']])
        self.assertIn('KERNEL32.dll!QueryPerformanceCounter', self.disassembly.imports.values())
        self.assertIn('WINMM.dll!timeGetTime', self.disassembly.imports.values())

    def test_native_script_reference_and_labels(self):
        self.assertIn((0x46dd05, 0x4e6330), self.functions[0x46dd00]['scripts'])
        text = self.disassembly.function_markdown(self.functions[0x46cde0], self.functions,
                                                   {entry: set() for entry in self.functions})
        self.assertIn('L_0046ce35:', text)
        self.assertIn('current_task (packed word index)', text)
        self.assertIn('CMP `0046cdfe`', text)

    def test_bundle_links_and_reproducibility(self):
        # Re-emit from the checked graph without repeating discovery.
        disassembly = self.disassembly
        original = disassembly.closure
        disassembly.closure = lambda: self.functions
        try:
            with tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                manifest = disassembly.write(root, 'MK4/FILESYS.DAT', full_text=False)
                self.assertEqual(len(manifest['functions']), len(self.functions))
                for file in root.rglob('*.md'):
                    for link in re.findall(r'\]\(([^)]+)\)', file.read_text()):
                        self.assertTrue((file.parent / link).is_file(), (str(file), link))
                first = (root / 'manifest.json').read_bytes()
                disassembly.write(root, 'MK4/FILESYS.DAT', full_text=False)
                self.assertEqual((root / 'manifest.json').read_bytes(), first)
                self.assertIn('004ed470  2d000000 3400002d 3f000000 0f000009',
                              (root / 'data-words.txt').read_text())
        finally:
            disassembly.closure = original


if __name__ == '__main__':
    unittest.main(verbosity=2)
