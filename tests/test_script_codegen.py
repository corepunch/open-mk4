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

    def test_blend_bind_requires_source_pose_and_valid_sample_one(self):
        # Raiden's finisher opcode 71 blends the inherited pose, then resumes
        # destination playback at sample one. It must not silently start at zero.
        descriptor = (0x4fad7c + 54 * 12) // 4
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x71000000, descriptor, 0x3f000000])
        source = self.compile([0x0f000000, descriptor, 0x71000000, descriptor, 0x3f000000])
        self.assertIn('mk4_script_blend_bind(fighter, 54,', source)
        # Descriptor decoding and ordinary cursor validation apply to blends.
        struct.pack_into('<I', self.image.memory, descriptor * 4 + 4 - 0x400000, 1)
        with self.assertRaisesRegex(CompileError, 'animation cursor outside stored clip'):
            self.compile([0x0f000000, descriptor, 0x71000000, descriptor, 0x3f000000])

    def test_both_branch_outcomes_and_jumps_into_waits_remain_edges(self):
        # Down branches into the second wait, which must remain a distinct
        # target rather than being folded into its predecessor.
        e = self.entry
        source = self.compile([0x44000000, (e + 12) // 4, 0x34000005, 0x34000002, 0x3f000000])
        self.assertIn('goto L_0057000c;', source)
        self.assertIn('fighter_wait_ticks(5);', source)
        self.assertIn('fighter_wait_ticks(2);', source)
        self.assertIn('fighter->held & BUTTON_BIT(BUTTON_DOWN)', source)
        self.assertNotIn('goto L_00570008;', source)

    def test_predicate_flags_and_binding_roles_are_typed(self):
        e = self.entry
        binding = Binding('test_gate', 'tests/test_script_codegen.py', ('predicate',), result='flags', evidence='synthetic predicate fixture')
        source = self.compile([0x36000000, 0x46de60, (e + 16) // 4, 0x34000001, 0x3f000000], {0x46de60: binding})
        self.assertIn('result_flags = test_gate(fighter);', source)
        self.assertIn('result_flags & 4u', source)
        with self.assertRaisesRegex(CompileError, 'cannot be used as predicate'):
            self.compile([0x36000000, 0x46a950, (e + 16) // 4, 0x34000001, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'does not accept inline selector'):
            self.compile([0x6b000007, 0x3f000000])
        source = self.compile([0x6c123456, 17, 0x3f000000])
        self.assertIn('liu_script_motion(fighter, 17, 1110, 291)', source)
        with self.assertRaisesRegex(ValueError, 'Predicate must return flags'):
            dataclasses.replace(binding, result='void')
        with self.assertRaisesRegex(CompileError, 'does not accept task type'):
            self.compile([0x40000000, 65, 0x49c110, 0x3f000000])

    def test_unknown_helpers_opcodes_and_truncated_paths_fail(self):
        with self.assertRaisesRegex(CompileError, 'unsupported opcode'):
            self.compile([0xff000000])
        with self.assertRaisesRegex(CompileError, 'unbound tail'):
            self.compile([0x32000000, 0x460000])
        with self.assertRaisesRegex(CompileError, 'not initialized'):
            self.compile([0x37008005, 0x3f000000])
        descriptor = (0x4fad7c + 54 * 12) // 4
        with self.assertRaisesRegex(CompileError, 'outside an incoming clip'):
            self.compile([0x0f000000, descriptor, 0x490000ff, 0x3f000000])
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
        # A nonzero packed word whose low-byte count is zero must not be
        # mistaken for a guaranteed yield by cycle validation.
        with self.assertRaisesRegex(CompileError, 'unsupported counted-playback'):
            descriptor = (0x4fad7c + 54 * 12) // 4
            self.compile([0x02000100, descriptor, 0x33000000, e // 4])

    def test_postprocessor_removes_fallthrough_and_structures_simple_loops(self):
        e = self.entry
        source = self.compile([0x34000001, 0x33000000, e // 4])
        self.assertIn('for (;;) {', source)
        self.assertIn('fighter_wait_ticks(1);', source)
        self.assertIn('continue;', source)
        self.assertNotIn('goto L_00570000;', source)
        self.assertNotIn('goto L_00570004;', source)

    def test_postprocessor_cli_updates_inc_files_in_place(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'sample.inc'
            path.write_text('''static inline void sample(void) {
L_00000000: {
  work();
  goto L_00000004;
}
L_00000004: {
  goto L_00000000;
}
}
''')
            subprocess.run([sys.executable, str(ROOT / 'tools/retail_c_postprocess.py'), str(path)],
                           check=True, capture_output=True, text=True)
            source = path.read_text()
        self.assertNotIn('goto L_00000004;', source)
        self.assertIn('for (;;) {', source)
        self.assertIn('continue;', source)

    def test_character_selector_is_specialized_and_optional_prefix_is_not_speed(self):
        program, _ = compile_program(self.image, 0x4e5ca0, 3, bindings={0x46a760: Binding('fighter_ready', 'game/moves.c', ('tail',), flow='terminal', evidence='synthetic selector continuation')})
        self.assertEqual(next(row for row in program['instructions'] if row['opcode'] == '0x3c')['selected_character'], 3)
        source = generate(self.image, [0x4e9ec8], 3, bindings={0x460250: Binding('fighter_ready', 'game/moves.c', ('tail',),
                          flow='terminal', evidence='synthetic continuation binding for playback syntax test')})
        self.assertIn('mk4_script_bind(fighter, 247, 5, 0);', source)
        self.assertIn('mk4_script_finish_clip(fighter, 1);', source)
        self.assertNotIn('fighter, 2)', source)

    def test_generated_only_audit_without_claiming_whole_native_parity(self):
        report = audit(self.image, 3)
        self.assertEqual(report['total'], report['compilable'])
        self.assertEqual(report['total'], report['generated'])
        self.assertEqual(report['legacy'], 0)
        self.assertEqual(report['coverage_errors'], [])
        self.assertIn('not native implementation parity', report['scope'])
        self.assertTrue(all(row['status'] == 'compilable' and not row['diagnostics'] for row in report['scripts']))

    def test_native_script_links_and_explicit_entry_contracts(self):
        # The three native jab tails only enter another VM script; flattening
        # them preserves the repeated branch without growing a fiber's stack.
        source = generate(self.image, [0x4e62f0], 3, 'fighter_ready')
        for target in ('004e6330', '004e6360', '004e63b0'):
            self.assertNotIn('goto L_' + target, source)
            self.assertIn('EXE ' + target, source)
            self.assertNotIn('retail_script_' + target + '(fighter)', source)
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            compile_program(self.image, 0x4e61e0, 3, return_to='fighter_ready')
        context = dict(clip=262, count=10, sample=0, evidence='46bea0 opens clip before VM entry')
        source = generate(self.image, [0x4e61e0], 3, 'fighter_ready', contexts={'0x4e61e0': context})
        self.assertIn('mk4_script_inherit(fighter, 262, 10, 0)', source)
        with self.assertRaisesRegex(ValueError, 'Invalid entry clip'):
            compile_program(self.image, 0x4e61e0, 3, return_to='fighter_ready', context={**context, 'sample': 10})
        source = generate(self.image, [0x4de428], 3, 'caller', contexts={
            '0x4de428': dict(clip=35, count=44, sample=38, evidence='42c0d0 damage continuation')})
        self.assertNotIn('fighter_fiber_finish(caller)', source)
        self.assertIn('return;', source)

    def test_cursor_progress_proves_loops_but_rejects_zero_progress(self):
        e = self.entry
        descriptor = (0x4fad7c + 54 * 12) // 4
        # This loop resets to 1, advances to 4, and therefore always suspends.
        self.compile([0x0f000001, descriptor, 0x05000004, 0x12000001, 0x33000000, (e + 8) // 4])
        with self.assertRaisesRegex(CompileError, 'without a positive wait'):
            self.compile([0x0f000004, descriptor, 0x05000004, 0x33000000, (e + 8) // 4])
        # A native call can invalidate an otherwise proven cursor reset.
        clobber = Binding('sets_cursor', 'tests/test_script_codegen.py', ('call',),
                          cursor_effect='unknown', evidence='synthetic native cursor mutation')
        with self.assertRaisesRegex(CompileError, 'without a positive wait'):
            self.compile([0x0f000000, descriptor, 0x12000000, 0x35000000, 0x460000,
                          0x05000004, 0x33000000, (e + 8) // 4], {0x460000: clobber})
        # Source 459f80 decrements the packed count twice, not once per wait.
        source = self.compile([0x0f000000, descriptor, 0x19000011, 0x3f000000])
        self.assertIn('mk4_script_advance(fighter, 9, 2)', source)

    def test_contact_branch_and_reaction_context_are_structural(self):
        e = self.entry
        source = self.compile([0x51000006, 0x42000000, (e + 16) // 4, 0x34000002, 0x3f000000])
        self.assertIn('result_flags & 1u', source)
        self.assertIn('goto L_00570010', source)
        with self.assertRaisesRegex(CompileError, 'flags is not initialized'):
            self.compile([0x42000000, (e + 8) // 4, 0x3f000000])
        source = self.compile([0x4e000000, 0x3f000000])
        self.assertNotIn('mk4_script_finish_clip', source)
        self.assertIn('owns reaction context', source)
        descriptor = (0x4fad7c + 54 * 12) // 4
        source = self.compile([0x11000003, descriptor, 0x34000001, 0x3f000000])
        self.assertIn('mk4_script_bind(fighter, 54, 13, 3)', source)

    def test_contact_query_d_lowers_inline_selector_to_researched_record(self):
        bindings = bindings_for(10)
        for address in (0x47a6d0, 0x47b010):
            bindings[address] = Binding('reptile_loop', 'game/c_reptile.c', ('tail',), flow='terminal',
                                        evidence='synthetic native-loop boundary for the retail D-contact fixture')
        source = generate(self.image, [0x4e8178], 10, 'fighter_ready', bindings=bindings)
        self.assertIn('static const AttackRecord retail_contact_4ec080', source)
        self.assertIn('battle_contact(fighter, &retail_contact_4ec080)', source)

    def test_advance_twice_to_sample_uses_packed_low_byte_target(self):
        binding = Binding('jarek_projectile', 'game/c_jarek.c', ('spawn',), ('fighter', 'task_type'),
                          flow='independent', task_types=(78,), evidence='synthetic native task boundary')
        source = generate(self.image, [0x4ed7f0], 12, 'fighter_ready', bindings={0x497580: binding})
        self.assertIn('fighter_fiber_play_to(fighter, 17, 2);', source)
        self.assertNotIn('fighter_fiber_play_to(fighter, 529, 2);', source)

    def test_opcode_69_plays_remaining_clip_through_shared_fiber_helper(self):
        binding = Binding('jax_projectile', 'tests/test_script_codegen.py', ('spawn',), ('fighter', 'task_type'),
                          flow='independent', task_types=(80,), evidence='synthetic native task boundary')
        source = generate(self.image, [0x4ed090], 13, 'fighter_ready', bindings={0x496fc0: binding})
        self.assertIn('mk4_script_finish_clip(fighter, 1);', source)
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x69000000, 0x3f000000])

    def test_generated_conditional_code_runs_both_paths_in_C(self):
        # Execute only generated portable C. Wait/finish stubs inspect emitted
        # C control flow; real actor suspension is covered by the native Liu suite.
        e = self.entry
        words = [0x44000000, (e + 20) // 4, 0x34000005, 0x33000000, (e + 24) // 4,
                 0x34000002, 0x3f000000]
        source = self.compile(words)
        harness = r"""
#include <setjmp.h>
#include <stdlib.h>
static jmp_buf done;
static int elapsed;
void fighter_ready(Fighter *fighter) { (void)fighter; }
void fighter_wait_ticks(int ticks) { if (ticks <= 0) { abort(); } elapsed += ticks; }
_Noreturn void fighter_fiber_finish(FighterFn next) { if (next != fighter_ready) { abort(); } longjmp(done, 1); }
int main(void) {
  Fighter fighter = {0};
  for (int down = 0; down < 2; ++down) {
    fighter.held = down ? BUTTON_BIT(BUTTON_DOWN) : 0;
    elapsed = 0;
    if (setjmp(done) == 0) { retail_script_00570000(&fighter); abort(); }
    if (elapsed != (down ? 3 : 6)) { return 1; }
  }
  return 0;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'branches.c'
            binary = Path(directory) / 'branches'
            path.write_text(source + harness)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path), '-o', str(binary)],
                           check=True, capture_output=True, text=True)
            subprocess.run([str(binary)], check=True)

    def test_reviewed_audit_detects_input_cpu_reaction_and_manifest_regressions(self):
        import copy
        from unittest.mock import patch
        from retail_coverage import audit as coverage_audit, profile_for, inventory
        profile = profile_for(3)
        self.assertEqual(coverage_audit(self.image, 3)['coverage_errors'], [])
        broken = copy.deepcopy(profile)
        broken['generated_scripts'].remove('0x4e62f0')
        broken['moves']['bicycle_kick']['symbol'] = 'missing_bicycle_implementation'
        errors = coverage_audit(self.image, 3, profile=broken)['coverage_errors']
        self.assertTrue(any('no generation entry' in error for error in errors))
        self.assertTrue(any('Missing portable C' in error for error in errors))
        broken = copy.deepcopy(profile)
        broken['reactions']['2']['symbol'] = 'react_jab_block'
        self.assertTrue(any('C dispatch' in error for error in coverage_audit(self.image, 3, profile=broken)['coverage_errors']))
        # A command-table reader regression must not silently drop releases.
        tables = inventory(self.image, 3)['inputs']
        with patch('retail_coverage.commands', return_value=[table for table in tables if table['button'] != 'low_kick_release']):
            self.assertIn('missing input event low_kick_release', coverage_audit(self.image, 3)['coverage_errors'])
        struct.pack_into('<I', self.image.memory, 0x4df8e8 + 3 * 4 - 0x400000, 0x4000f)
        errors = coverage_audit(self.image, 3)['coverage_errors']
        self.assertTrue(any('inventory changed' in error for error in errors))
        self.assertTrue(any('input/CPU target' in error for error in errors))

    def test_reviewed_audit_skips_zeroed_cpu_rows(self):
        from unittest.mock import patch
        from retail_coverage import audit as coverage_audit, profile_for, inventory, fingerprint
        observed = inventory(self.image, 3)
        for row in observed['cpu'].values():
            row['target'] = None
        profile = profile_for(3)
        profile['inventory_sha256'] = fingerprint(observed)
        with patch('retail_coverage.inventory', return_value=observed):
            errors = coverage_audit(self.image, 3, profile=profile)['coverage_errors']
        self.assertFalse(any('input/CPU target None' in error for error in errors))

    def test_selected_air_table_word_is_a_valid_overlapping_no_op(self):
        binding = Binding('fighter_ready', 'game/moves.c', ('tail',), flow='terminal', evidence='synthetic native boundary')
        source = generate(self.image, [0x4e6500], 3, bindings={0x46e3d0: binding})
        self.assertIn('EXE 004e6544', source)
        self.assertIn('decoded VM no-op', source)

    def test_cli_fails_atomically_and_check_never_writes(self):
        tool = ROOT / 'tools/compile_retail.py'
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'script.inc'
            base = [sys.executable, str(tool), '--character', 'liu_kang', '--output', str(path), '--return-to', 'fighter_ready']
            subprocess.run([*base, '--script', '4e60b8'], check=True, capture_output=True)
            original = path.read_bytes()
            failed = subprocess.run([*base, '--script', '4e60b8', '--script', '4e61e0'], capture_output=True)
            self.assertEqual(failed.returncode, 2)
            self.assertEqual(path.read_bytes(), original)
            subprocess.run([*base, '--script', '4e60b8', '--check'], check=True, capture_output=True)
            path.write_text('stale\n')
            failed = subprocess.run([*base, '--script', '4e60b8', '--check'], capture_output=True)
            self.assertEqual(failed.returncode, 2)
            self.assertEqual(path.read_text(), 'stale\n')


if __name__ == '__main__':
    unittest.main(verbosity=2)
