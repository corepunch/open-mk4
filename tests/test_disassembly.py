#!/usr/bin/env python3
"""Check retail script decoding and the optional native disassembly navigator."""
import importlib.util
import json
from pathlib import Path
import re
import struct
import subprocess
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

    def test_scepter_low_down_branch(self):
        program = script_program(self.image, 0x4e94b0)
        rows = {row['va']: row for row in program['instructions']}
        self.assertEqual(rows['0x4e94e0']['branch'], '0x4e94f8')
        self.assertEqual(rows['0x4e94e0']['opcode'], '0x44')
        self.assertEqual(rows['0x4e94e8']['clip'], 194)
        self.assertEqual(rows['0x4e94f8']['clip'], 193)

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

    def test_reaction_and_throw_paths(self):
        for root in (0x4e89a0, 0x4e8ea8, 0x4e9308, 0x4e9568, 0x4e93a8,
                     0x4e8ab8, 0x4e9260, 0x4e9238, 0x4de328, 0x4e93d8):
            rows = script_program(self.image, root)['instructions']
            self.assertFalse(any('unresearched' in r['operation'] for r in rows), hex(root))
            self.assertIn(rows[-1]['opcode'], ('0x32', '0x3f'))
        rows = script_program(self.image, 0x4e93a8)['instructions']
        motion = next(r for r in rows if r['opcode'] == '0x65')
        self.assertEqual(motion['operands'], ['0xffffe667', '0x0', '0x0'])
        self.assertEqual(rows[rows.index(motion) + 1]['va'], '0x4e93c0')

    def test_motion_and_continuation_boundaries(self):
        thaw = script_program(self.image, 0x4e9d38)['instructions']
        self.assertEqual([row['va'] for row in thaw], ['0x4e9d38', '0x4e9d40', '0x4e9d44'])
        self.assertEqual(thaw[0]['operands'], ['0x147'])
        self.assertEqual(thaw[-1]['native_target'], '0x486530')
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


class QueryTests(unittest.TestCase):
    def test_c_view_preserves_low_kick_steps_and_native_order(self):
        from retail_script_c import script_c_text
        text = script_c_text(RetailImage('MK4/MK-4.EXE'), 0x4e6d08)
        code = text.split('```c\n', 1)[1].split('```', 1)[0]
        self.assertIn('void scorpion_low_kick(void) {', code)
        self.assertIn('open_animation(ANIM_SCORPION_LOW_KICK, 0); // 18 stored samples', code)
        self.assertIn('31 decoded instructions; 31 shown', text)
        steps = re.findall(r'show_sample_for_ticks\((\d+), (\d+)\);', code)
        self.assertEqual(steps, [(str(sample), '1') for sample in (0, 2, 4, 6, 8, 10, 12, 14, 16, 17)])
        self.assertEqual(code.count('task_slot_5c_clear();'), 2)
        self.assertLess(code.index('task_slot_5c_set_one();'), code.index('show_sample_for_ticks(14, 1);'))
        self.assertGreater(code.rindex('task_slot_5c_clear();'), code.index('show_sample_for_ticks(17, 1);'))
        ordered = ['contact_query_b(6);', 'set_action(ACTION_1006);', 'release_attack_bookkeeping();',
                   'wait_ticks(native_result_bit0() ? 8 : 5);', 'wait_pose_then_controller_return();', 'return;']
        positions = [code.index(statement) for statement in ordered]
        self.assertEqual(positions, sorted(positions))
        self.assertNotRegex(code, r'0x[0-9a-f]{6}|inline=|native_target=')
        self.assertIn('damage 11%', text)
        self.assertIn('reactions hit/air/block = 17/10/6', text)
        self.assertIn('upstream `0046a9b0`', text)
        self.assertIn('not a compilable C port', text)

    def test_c_view_decodes_values_and_honors_instruction_limit(self):
        from retail_script_c import script_c_text
        image = RetailImage('MK4/MK-4.EXE')
        struct.pack_into('<I', image.memory, 0x4e6d24 - 0x400000, 0x49000005)
        struct.pack_into('<I', image.memory, 0x4e6d90 - 0x400000, 0x37007003)
        text = script_c_text(image, 0x4e6d08)
        self.assertIn('show_sample_for_ticks(5, 1);', text)
        self.assertIn('wait_ticks(native_result_bit0() ? 7 : 3);', text)
        limited = script_c_text(image, 0x4e6d08, limit=5)
        self.assertIn('show_sample(5);', limited)
        self.assertNotIn('show_sample_for_ticks', limited)
        self.assertIn('31 decoded instructions; 5 shown', limited)
        self.assertIn('continuation omitted', limited)

    def test_c_view_stops_at_unknown_operations_and_control_flow(self):
        from retail_script_c import script_c_text
        image = RetailImage('MK4/MK-4.EXE')
        entry = 0x570000
        # A jump reaches valid low-kick instructions. Never print those as
        # sequential C after an unsupported branch in the sorted VM graph.
        struct.pack_into('<II', image.memory, entry - 0x400000, 0x33000000, (entry + 8) // 4)
        struct.pack_into('<II', image.memory, entry + 8 - 0x400000, 0x35000000, 0x46a950)
        struct.pack_into('<I', image.memory, entry + 16 - 0x400000, 0x3f000000)
        text = script_c_text(image, entry)
        self.assertIn('C view stopped: script jump', text)
        self.assertNotIn('task_slot_5c_clear();', text)
        struct.pack_into('<I', image.memory, entry - 0x400000, 0xff000000)
        self.assertIn('unresearched opcode; path stopped', script_c_text(image, entry))
        # The decoder's sorted graph can place a target below the entry. The
        # C view must begin at the requested root, not that earlier block.
        struct.pack_into('<II', image.memory, entry - 0x400000, 0x35000000, 0x46a950)
        struct.pack_into('<I', image.memory, entry + 8 - 0x400000, 0x3f000000)
        struct.pack_into('<II', image.memory, entry + 16 - 0x400000, 0x33000000, entry // 4)
        text = script_c_text(image, entry + 16)
        self.assertIn('C view stopped: script jump', text)
        self.assertNotIn('task_slot_5c_clear();', text)

    def test_c_view_cli_accepts_script_and_move_queries_only(self):
        root = Path(__file__).resolve().parents[1]
        tool = str(root / 'tools/disassemble_retail.py')
        with tempfile.TemporaryDirectory() as directory:
            command = [sys.executable, tool, '--exe', str(root / 'MK4/MK-4.EXE'), '--archive', 'missing.dat', '--c']
            for query in (['--script', '0x4e6d08'], ['--move', 'low_kick']):
                result = subprocess.run([*command, *query], cwd=directory, capture_output=True, text=True, check=True)
                self.assertIn('void scorpion_low_kick(void)', result.stdout)
                self.assertIn('show_sample_for_ticks(17, 1);', result.stdout)
            self.assertEqual(list(Path(directory).iterdir()), [])
            subprocess.run([*command, '--move', 'low_kick', '--output', 'kick.md'], cwd=directory, capture_output=True, check=True)
            self.assertEqual([path.name for path in Path(directory).iterdir()], ['kick.md'])
            self.assertIn('void scorpion_low_kick(void)', Path(directory, 'kick.md').read_text())
            for query in (['--attack', 'B', '6'], ['--native', '0x46a950'], ['--script', '0x4e6d08', '--raw']):
                result = subprocess.run([*command, *query], cwd=directory, capture_output=True, text=True)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('--c requires --script or --move', result.stderr)

    def test_build_specific_names_and_reference_sources(self):
        from retail_query import instruction_text, script_text
        from retail_symbols import REFERENCES, reference_rows, target_text
        image = RetailImage('MK4/MK-4.EXE')
        text = script_text(image, 0x4e6d08)
        self.assertIn('native_target=0x46a950 (task_slot_5c_clear)', text)
        self.assertIn('native_target=0x46a970 (task_slot_5c_set_one)', text)
        self.assertIn('native_target=0x48d3f0 (contact_query_b)', text)
        self.assertIn('wait_pose_then_controller_return', text)
        self.assertIn('upstream `0046a9b0`', text)
        self.assertIn('different EXE revision', text)
        # Upstream addresses must never become our labels by numeric equality
        # or by applying a single revision delta to all functions.
        self.assertEqual(target_text('0x46a9b0'), '0x46a9b0')
        self.assertNotIn('MoveSel_0048d3f0', text)
        self.assertEqual(reference_rows([0x48d3f0]), [])
        branch = dict(va='0x570000', opcode='0x31', operation='branch', branch='0x46a950')
        self.assertEqual(instruction_text(branch), '00570000  branch; branch=0x46a950')
        limited = script_text(image, 0x4e6d08, limit=1)
        self.assertIn('upstream `0046a9b0`', limited)
        self.assertNotIn('upstream `0046a9d0`', limited)

    @unittest.skipUnless(Path('reference/mk4decomp/config/symbols.yaml').is_file(), 'Optional mk4decomp checkout unavailable')
    def test_reference_sources_contain_the_named_containers(self):
        from retail_symbols import REFERENCES
        root = Path(__file__).resolve().parents[1] / 'reference/mk4decomp'
        for _, symbol, source, _ in REFERENCES.values():
            self.assertIn(symbol.split(' +')[0], (root / source).read_text())

    def test_named_helpers_match_local_machine_code(self):
        image = RetailImage('MK4/MK-4.EXE')
        # These exact local entries support the conservative field-operation
        # names independently of upstream naming or its address layout.
        for address, code in (
                (0x46a950, '8b0d20c4530033c0a32cc4530089048d5c000000c3'),
                (0x46a970, '8b0d20c45300b801000000a32cc4530089048d5c000000c3')):
            expected = bytes.fromhex(code)
            offset = address - 0x400000
            self.assertEqual(image.memory[offset:offset + len(expected)], expected)

    def test_script_limit_and_typed_contact(self):
        from retail_query import script_text, contact_text
        image = RetailImage('MK4/MK-4.EXE')
        text = script_text(image, 0x4e6d08, limit=2)
        self.assertIn('Truncated:', text)
        self.assertIn('raise --limit', text)
        text = script_text(image, 0x4e6d08)
        self.assertIn('B[6]; record 0x4ebb28', text)
        self.assertIn('bit0 clear=5 ticks; set=8 ticks', text)
        self.assertIn('damage 11%', contact_text(image, 'B', 6))

    def test_compact_move_pages_and_honest_scope(self):
        from retail_query import move_text, move_list_text, contact_text
        from scorpion_catalog import MOVES
        from scorpion_moves import inventory
        image = RetailImage('MK4/MK-4.EXE')
        spear = move_text(image, 'spear')
        self.assertIn('B,B + low_punch; window=27', spear)
        self.assertIn('predicate `0x45ea60`', spear)
        self.assertIn('victim hit', spear)
        self.assertIn('70 -> `--native 0x485fb0`', spear)
        self.assertIn('projectile task', spear)
        self.assertIn('rows omitted from preview', spear)
        self.assertIn('HP=00000000; LP=004712d0', move_text(image, 'air_hp'))
        self.assertIn('scene entry', move_text(image, 'fatality_1'))
        sword = move_text(image, 'sword_back_lp')
        self.assertIn('Unknown 0x42', sword)
        self.assertIn('Path stopped', sword)
        record = contact_text(image, 'weapon', 2)
        self.assertIn('record 0x4eb9c0', record)
        self.assertIn('damage 20%', record)
        self.assertIn('record 0x4ebeb8', contact_text(image, 'record', 0x4ebeb8))
        for name in MOVES:
            report = move_text(image, name)
            self.assertLess(len(report), 16000, name)
            self.assertNotIn('failed:', report, name)
            self.assertIn('counts cover only the selected roots', report)
            self.assertIn(name, move_list_text())
        self.assertEqual(inventory(image)['coverage']['implementation'],
                         'Not assessed; see docs/character-script-port.md')

    def test_query_cli_writes_only_requested_file(self):
        root = Path(__file__).resolve().parents[1]
        tool = str(root / 'tools/disassemble_retail.py')
        with tempfile.TemporaryDirectory() as directory:
            command = [sys.executable, tool, '--exe', str(root / 'MK4/MK-4.EXE'),
                       '--archive', 'missing.dat', '--script', '004e9d00']
            result = subprocess.run(command, cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('clip=199', result.stdout)
            self.assertEqual(list(Path(directory).iterdir()), [])
            subprocess.run([*command, '--output', 'spear.md'], cwd=directory, capture_output=True, check=True)
            self.assertEqual([p.name for p in Path(directory).iterdir()], ['spear.md'])
            # Supply a real EXE to reach address validation rather than a missing-file error.
            page = subprocess.run([sys.executable, tool, '--exe', str(root / 'MK4/MK-4.EXE'),
                                   '--archive', 'missing.dat', '--move', 'spear'],
                                  cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('# Scorpion: spear', page.stdout)
            unknown = subprocess.run([sys.executable, tool, '--exe', str(root / 'MK4/MK-4.EXE'), '--move', 'nonsense'],
                                     cwd=directory, capture_output=True, text=True)
            self.assertNotEqual(unknown.returncode, 0)
            self.assertIn('--list-moves', unknown.stderr)
            bad = subprocess.run([*command[:-1], '4e9d01'], cwd=directory, capture_output=True, text=True)
            self.assertNotEqual(bad.returncode, 0)
            self.assertIn('aligned image address', bad.stderr)
            help_result = subprocess.run([sys.executable, tool], cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('--script', help_result.stdout)
            self.assertEqual([p.name for p in Path(directory).iterdir()], ['spear.md'])

    @unittest.skipUnless(importlib.util.find_spec('capstone'), 'Capstone required for native queries')
    def test_native_query_is_bounded_and_does_not_expand_closure(self):
        from retail_query import native_text
        disassembly = Disassembly('MK4/MK-4.EXE')
        def forbidden():
            self.fail('A targeted query must not discover the whole binary')
        disassembly.closure = disassembly.discover_entries = forbidden
        text = native_text(disassembly, 0x46d290, limit=3)
        self.assertIn('instruction budget reached', text)
        self.assertEqual(text.count('\n004'), 3)

    @unittest.skipUnless(importlib.util.find_spec('capstone'), 'Capstone required for native queries')
    def test_native_query_names_calls_and_links_upstream_source(self):
        from retail_query import native_text
        disassembly = Disassembly('MK4/MK-4.EXE')
        text = native_text(disassembly, 0x459490, limit=8)
        self.assertIn('jmp 0x4594b0 ; script_dispatch', text)
        self.assertIn('ArgSarStoreJmp', text)
        self.assertIn('upstream `004594f0`', text)


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
