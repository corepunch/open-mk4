#!/usr/bin/env python3
"""Compiler safety, CFG preservation, deterministic output and real C11 checks."""
import dataclasses
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_moves import RetailImage
from retail_bindings import Binding, bindings_for
from retail_codegen import CompileError, compile_program, generate, symbol
from compile_retail import audit, format_c


class CompilerTests(unittest.TestCase):
    def setUp(self):
        self.image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        self.entry = 0x570000

    def fixture(self, words):
        struct.pack_into('<%dI' % len(words), self.image.memory, self.entry - 0x400000, *words)
        return self.entry

    def compile(self, words, bindings=None):
        return generate(self.image, [self.fixture(words)], 3, return_to='fighter_ready', bindings=bindings)

    def test_real_normal_and_loop_scripts_compile_as_C11(self):
        entries = [0x4e6d08, 0x4e60b8]
        source = format_c(generate(self.image, entries, 3, 'fighter_ready'))
        self.assertEqual(source, format_c(generate(self.image, reversed(entries), 3, 'fighter_ready')))
        for entry in entries:
            self.assertIn(symbol(entry), source)
        self.assertIn('mk4_script_reverse_return(fighter);', source)
        self.assertIn('if (--loop_counter != 0)', source)
        self.assertIn('mk4_script_finish_clip(fighter, -1);', source)
        self.assertIn('result_flags & 1u ? 8 : 5', source)
        self.assertIn('0x9000000 0x13ec01', source)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'compiled.c'
            path.write_text(source + '\nvoid use_generated(Fighter *fighter) {\n' +
                            '\n'.join('  %s(fighter);' % symbol(entry) for entry in entries) + '\n}\n')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_both_branch_outcomes_and_jumps_into_waits_remain_edges(self):
        # Down branches into the wait itself, so folding it with show-sample
        # would skip an externally reachable operation.
        e = self.entry
        source = self.compile([0x44000000, (e + 12) // 4, 0x34000005, 0x34000002, 0x3f000000])
        self.assertIn('goto L_0057000c;', source)
        self.assertIn('fighter_wait_ticks(5);', source)
        self.assertIn('fighter_wait_ticks(2);', source)
        self.assertIn('fighter->held & BUTTON_BIT(BUTTON_DOWN)', source)
        self.assertIn('goto L_00570008;', source)

    def test_predicate_flags_and_binding_roles_are_typed(self):
        e = self.entry
        binding = Binding('test_gate', 'tests/test_script_codegen.py', ('predicate',), result='flags', evidence='synthetic predicate fixture')
        source = self.compile([0x36000000, 0x46de60, (e + 16) // 4, 0x34000001, 0x3f000000], {0x46de60: binding})
        self.assertIn('result_flags = test_gate(fighter);', source)
        self.assertIn('result_flags & 4u', source)
        with self.assertRaisesRegex(CompileError, 'cannot be used as predicate'):
            self.compile([0x36000000, 0x46a950, (e + 16) // 4, 0x34000001, 0x3f000000])
        with self.assertRaisesRegex(ValueError, 'Predicate must return flags'):
            dataclasses.replace(binding, result='void')

    def test_unknown_helpers_opcodes_and_truncated_paths_fail(self):
        with self.assertRaisesRegex(CompileError, 'unsupported opcode'):
            self.compile([0xff000000])
        with self.assertRaisesRegex(CompileError, 'unbound tail'):
            self.compile([0x32000000, 0x46ddd0])
        with self.assertRaisesRegex(CompileError, 'not initialized'):
            self.compile([0x37008005, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'overlaps operand'):
            self.compile([0x44000000, (self.entry + 4) // 4, 0x3f000000])

    def test_non_yielding_cycles_and_loop_entry_state_fail(self):
        e = self.entry
        with self.assertRaisesRegex(CompileError, 'without a positive wait'):
            self.compile([0x33000000, e // 4])
        # One route bypasses the wait in a larger strongly connected graph.
        with self.assertRaisesRegex(CompileError, 'without a positive wait'):
            self.compile([0x44000000, e // 4, 0x34000001, 0x33000000, e // 4])
        with self.assertRaisesRegex(CompileError, 'zero loop count'):
            self.compile([0x3a000000, 0x34000001, 0x3b000000, 0x3f000000])

    def test_character_selector_is_specialized_and_optional_prefix_is_not_speed(self):
        program, _ = compile_program(self.image, 0x4e5ca0, 3, bindings={0x46a760: Binding('fighter_ready', 'game/moves.c', ('tail',), flow='terminal', evidence='synthetic selector continuation')})
        self.assertEqual(next(row for row in program['instructions'] if row['opcode'] == '0x3c')['selected_character'], 3)
        source = generate(self.image, [0x4e9ec8], 3, bindings={0x460250: Binding('fighter_ready', 'game/moves.c', ('tail',),
                          flow='terminal', evidence='synthetic continuation binding for playback syntax test')})
        self.assertIn('mk4_script_bind(fighter, 247, 5, 0);', source)
        self.assertIn('mk4_script_finish_clip(fighter, 1);', source)
        self.assertNotIn('fighter, 2)', source)

    def test_audit_reports_unsupported_without_claiming_whole_move_parity(self):
        report = audit(self.image, 3)
        self.assertGreater(report['total'], report['compilable'])
        self.assertIn('not native implementation parity', report['scope'])
        self.assertTrue(any(row['status'] == 'unsupported' and row['diagnostics'] for row in report['scripts']))

    def test_cli_fails_atomically_and_check_never_writes(self):
        tool = ROOT / 'tools/compile_retail.py'
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'script.inc'
            base = [sys.executable, str(tool), '--character', 'liu_kang', '--output', str(path), '--return-to', 'fighter_ready']
            subprocess.run([*base, '--script', '4e60b8'], check=True, capture_output=True)
            original = path.read_bytes()
            failed = subprocess.run([*base, '--script', '4e60b8', '--script', '4e62f0'], capture_output=True)
            self.assertEqual(failed.returncode, 2)
            self.assertEqual(path.read_bytes(), original)
            subprocess.run([*base, '--script', '4e60b8', '--check'], check=True, capture_output=True)
            path.write_text('stale\n')
            failed = subprocess.run([*base, '--script', '4e60b8', '--check'], capture_output=True)
            self.assertEqual(failed.returncode, 2)
            self.assertEqual(path.read_text(), 'stale\n')


if __name__ == '__main__':
    unittest.main(verbosity=2)
