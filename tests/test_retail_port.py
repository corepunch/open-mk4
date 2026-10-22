#!/usr/bin/env python3
"""Check bounded C extraction and exact operand provenance without retail execution."""
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from character_catalog import moves_for
from c_source import function_source, read_source
from disassemble_retail import Disassembly
from retail_actions import NATIVE, native_evidence
from retail_moves import RetailImage
from retail_port import port_entry, source_closure, source_rows, native_literals, port_text
from retail_helper_view import port_source
from scorpion_moves import script_program


class PortTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.disassembly = Disassembly(ROOT / 'MK4/MK-4.EXE')

    def test_all_character_move_entries_and_top_level_extraction(self):
        for character in (0, 1, 2, 3, 4):
            for name in moves_for(character):
                port = port_entry(character, name)
                self.assertIsNotNone(port, (character, name))
                found = source_closure(port)
                self.assertIn(port[1], found)
                for function, (_, code) in found.items():
                    self.assertNotIn(function, ('if', 'for', 'while', 'switch', 'battle_spray'))
                    self.assertRegex(code, r'^[A-Za-z_]')
                    self.assertTrue(code.endswith('}'))
                    self.assertIn(function + '(', code)
        ice = source_closure(port_entry(4, 'ice'))
        self.assertIn('sub_zero_ice', ice)
        self.assertIn('retail_vm_enter(fighter, 0x4ed588, fighter_ready)', ice['sub_zero_ice'][1])
        self.assertIn('sub_zero_ice_projectile', source_closure(('game/sub_zero/character.c', 'script_spawn_ice')))  # A callback argument, not an inline call.
        with self.assertRaisesRegex(ValueError, 'Missing portable C definition'):
            port_source(('game/sub_zero/character.c', 'if'))
        with self.assertRaisesRegex(ValueError, 'Missing portable C definition'):
            port_source(('game/sub_zero/character.c', 'battle_spray'))

    def test_ice_and_native_clone_have_reusable_phases_and_exact_values(self):
        ice = port_text(self.disassembly, 'ice', 4)
        for value in ('retail_vm_enter(fighter, 0x4ed588, fighter_ready)',
                      'script_spawn_ice', 'raw_fixed=', '0x570a', '0049bc74', 'mk4decomp',
                      'Existing engine C', 'not a standalone translation unit', 'literal', 'control target'):
            self.assertIn(value, ice)
        clone = port_text(self.disassembly, 'clone', 4)
        for value in ('clone_start', 'clone_object', '0x6147', 'ACTION_RETAIL_0406', 'Native move: no VM script root'):
            self.assertIn(value, clone)
        spear = port_text(self.disassembly, 'spear')
        self.assertIn('retail_vm_enter(fighter, 0x4ed448, scorpion_script_spear_done)', spear)
        self.assertIn('scheduler wait; ticks=45', spear)
        self.assertIn('script_spear_return', spear)
        self.assertIn('Typed runtime boundaries', spear)
        other = port_text(self.disassembly, 'jab', 5, 12)
        self.assertIn('No portable C entry is indexed', other)
        self.assertNotIn('static void scorpion_jab(', other)

    def test_shared_instruction_budget_and_atomic_c_bodies(self):
        class Counting(Disassembly):
            def __init__(self):
                super().__init__(ROOT / 'MK4/MK-4.EXE')
                self.visits = []
            def function(self, entry, limit=8192):
                result = super().function(entry, limit)
                self.visits.append((limit, len(result['instructions'])))
                return result
        disassembly = Counting()
        text = '\n'.join(native_literals(disassembly, [0x49bc60, 0x49bcb0, 0x486230], 9))
        self.assertLessEqual(sum(count for _, count in disassembly.visits), 9)
        self.assertIn('Omitted by --limit', text)
        self.assertIn('--native 0049bcb0', text)
        source = '\n'.join(source_rows(port_entry(4, 'clone'), 1))
        self.assertNotIn('```c', source)
        self.assertIn('No partial function', source)
        self.assertIn('native/motion.c:', source)
        with self.assertRaisesRegex(ValueError, 'Unknown'):
            port_text(disassembly, 'not_a_move', 4)

    def test_throw_roster_selection_and_complete_retail_continuations(self):
        image = self.disassembly.image
        scorpion = script_program(image, moves_for(0)['lp_throw']['scripts'][0][1], 0)
        sub_zero = script_program(image, moves_for(4)['lp_throw']['scripts'][0][1], 4)
        self.assertIn(75, {r['clip'] for r in scorpion['instructions'] if 'clip' in r})
        self.assertIn(76, {r['clip'] for r in sub_zero['instructions'] if 'clip' in r})
        self.assertEqual(image.word(0x4de7a0 + 4 * 4) * 4, 0x4de6b8)  # Packed script pointer.
        self.assertEqual(image.word(0x4ddfb8 + 4 * 4), 0x42c180)  # Native byte address, not VM code.
        self.assertEqual(moves_for(4)['lk_throw']['scripts'][0][1], 0x4de490)
        for name, fragments in (
                ('lp_throw', ('76/153', '0x2666', '142/141/143', 'getup 328', 'retail_vm_enter(fighter, 0x4de798')),
                ('lk_throw', ('41/165', '0x2e14', '29+5+8', 'owner idle hold 60', 'retail_lk_target'))):
            text = port_text(self.disassembly, name, 4, 64)
            self.assertIn('Port notes', text)
            for fragment in fragments:
                self.assertIn(fragment, text)
            self.assertNotIn('retains existing gameplay', text)
        self.assertNotIn('Known adaptation', port_text(self.disassembly, 'lp_throw', 0, 64))
        self.assertEqual([entry for _, entry in moves_for(4)['lk_throw']['scripts']], [0x4de490, 0x4de4b8, 0x4de4e0, 0x4e9ee0])
        counts = script_program(image, 0x4de4b8, 4)['instructions']
        self.assertEqual([r['inline'] for r in counts if int(r['opcode'], 16) == 0x10], [29, 5, 8])
        final = script_program(image, 0x4de4e0, 4)['instructions']
        self.assertEqual(final[0]['inline'], 5)
        victim = script_program(image, 0x4e9bd0, 4)['instructions']
        self.assertEqual([(r['clip'], r['stored_count']) for r in victim if 'clip' in r], [(142, 17), (141, 17), (143, 29)])
        self.assertEqual([r['inline'] for r in victim if int(r['opcode'], 16) == 0x34], [20])
        # Numeric evidence is data/assembly, not copied back from the C implementation.
        self.assertEqual(image.word(0x42c226), 0x2e14)
        self.assertEqual(image.word(0x42c370), 60)
        self.assertEqual(image.clip(image.word(0x53cf90)), 200)
        self.assertEqual(image.clip(image.word(0x4ea214)), 328)

    def test_action_evidence_is_real_literal_to_controller_field(self):
        from capstone import Cs, CS_ARCH_X86, CS_MODE_32
        from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
        decoder = Cs(CS_ARCH_X86, CS_MODE_32)
        decoder.detail = True
        image = self.disassembly.image
        for value, sites in NATIVE.items():
            native_evidence(image, value)
            for role, _, site, raw, _ in sites:
                if role != 'writer':
                    continue
                registers, writes = {}, []
                for insn in decoder.disasm(bytes.fromhex(raw), site):
                    ops = insn.operands
                    if insn.mnemonic != 'mov':
                        self.assertEqual(insn.mnemonic, 'push')
                        continue
                    destination, source = ops
                    literal = source.imm if source.type == X86_OP_IMM else registers.get(source.reg) if source.type == X86_OP_REG else None
                    if destination.type == X86_OP_REG:
                        registers[destination.reg] = literal
                    elif destination.type == X86_OP_MEM and destination.mem.disp == 0x74:
                        writes.append(literal)
                self.assertEqual(writes, [value], (value, hex(site), raw))
        changed = RetailImage(ROOT / 'MK4/MK-4.EXE')
        struct.pack_into('<I', changed.memory, 0x49bc75 - 0x400000, 0x305)
        with self.assertRaisesRegex(ValueError, 'evidence mismatch'):
            native_evidence(changed, 0x304)

    def test_cli_is_read_only_and_rejects_invalid_combinations(self):
        tool = str(ROOT / 'tools/disassemble_retail.py')
        base = [sys.executable, tool, '--exe', str(ROOT / 'MK4/MK-4.EXE'), '--archive', 'missing']
        with tempfile.TemporaryDirectory() as directory:
            result = subprocess.run([*base, '--character', 'sub_zero', '--move', 'ice', '--port'], cwd=directory, text=True, capture_output=True, check=True)
            self.assertIn('retail_vm_enter(fighter, 0x4ed588, fighter_ready)', result.stdout)
            self.assertEqual(list(Path(directory).iterdir()), [])
            for args in (['--port'], ['--port', '--script', '4ed588'], ['--port', '--move', 'ice', '--c'],
                         ['--port', '--move', 'ice', '--raw'], ['--move', 'ice', '--port', '--limit', '0']):
                result = subprocess.run([*base, *args], cwd=directory, text=True, capture_output=True)
                self.assertNotEqual(result.returncode, 0, args)

    def test_private_move_navigation_keeps_generated_code_separate(self):
        integration = 'game/scorpion/character.c'
        actual, line, code = function_source(integration, 'scorpion_teleport')
        self.assertEqual(actual, 'game/native/motion.c')
        self.assertTrue((ROOT / actual).read_text().splitlines()[line - 1].startswith('void scorpion_teleport('))
        native = read_source(integration)
        self.assertIn(code, native)
        self.assertNotIn('static inline void retail_script_', native)
        self.assertIn(actual, '\n'.join(source_rows((integration, 'scorpion_teleport'), 512)))


if __name__ == '__main__':
    unittest.main(verbosity=2)
