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

    def test_forward_end_cursor_holds_last_pose_but_pose_selection_stays_strict(self):
        descriptor = (0x4fad7c + 98 * 12) // 4
        # Fujin4e5c20 advances to cursor40 on a40-sample clip, then
        # rewinds34. Native429990 never submits pose40 at the end cursor.
        entry = self.fixture([0x0f000000, descriptor, 0x05000028, 0x4a000022, 0x3f000000])
        frames = generate(self.image, [entry], 5, 'fighter_ready')
        self.assertNotIn('fighter->sample_override = 40;', frames)
        samples = frames.split('_samples[] = {', 1)[1].split('}', 1)[0]
        poses = [int(value) for value in samples.split(',')]
        self.assertEqual(max(poses), 39)
        self.assertEqual(poses[-1], 34)
        self.assertEqual(poses.count(39), 3)
        actual = generate(self.image, [0x4e5c20], 5, 'fighter_ready', contexts={
            '0x4e5c20': {'clip': 98, 'count': 40, 'sample': 16, 'evidence': 'Native46a650 retains the wind caster clip.'}})
        self.assertNotIn('fighter->sample_override = 40;', actual)
        for opcode, target in [(5, 41), (0x49, 40), (0x12, 40), (0x4a, 40)]:
            entry = self.fixture([0x0f000000, descriptor, opcode << 24 | target, 0x3f000000])
            with self.assertRaisesRegex(CompileError, 'outside an incoming clip'):
                generate(self.image, [entry], 5, 'fighter_ready')

    def test_gravity_operand_survives_nonblocking_landing_lowering(self):
        descriptor = (0x4fad7c + 58 * 12) // 4
        # Freeze, hold, clear busy, restore gravity and wait for the floor.
        entry = self.fixture([0x0f000000, descriptor, 0x5b000000, 0x34000008,
                              0x61000000, 0x5c000000, 0x51e, 0x60000000, 0x3f000000])
        frames = generate(self.image, [entry], 5, 'fighter_ready')
        self.assertIn('fighter->body.gravity = 1310;', frames)
        self.assertIn('mk4_mmove_wait_grounded', frames)
        self.assertIn('fighter->flags &= ~(unsigned)FF_BUSY;', frames)
        entry = self.fixture([0x0f000000, descriptor, 0x5c000000, 0xfffffae2, 0x3f000000])
        frames = generate(self.image, [entry], 5, 'fighter_ready')
        self.assertIn('fighter->body.gravity = -1310;', frames)

    def test_controller_return_lowering_finishes_the_frame_program(self):
        entry = self.fixture([0x32000000, 0x470420])
        binding = Binding('mk4_mmove_thrown_finish', 'game/script.c', ('tail',), flow='terminal',
                          mmove_lowering='controller_return', evidence='470420 resets motion and returns the controller')
        frames = generate(self.image, [entry], 3, 'fighter_ready', bindings={0x470420: binding})
        self.assertIn('mk4_mmove_thrown_finish(fighter);', frames)

    def test_real_normal_and_loop_scripts_compile_as_C11(self):
        entries = [0x4e6d08, 0x4e60b8]
        source = format_c(generate(self.image, entries, 3, 'fighter_ready'))
        self.assertEqual(source, format_c(generate(self.image, reversed(entries), 3, 'fighter_ready')))
        for entry in entries:
            self.assertIn(symbol(entry), source)
        self.assertIn('fighter_finish_mmove(fighter, fighter_ready);', source)  # reverse return
        self.assertNotIn('loop_counter', source)  # counted loops unroll into rows
        self.assertIn('fighter->mmove_result = battle_contact(', source)
        self.assertIn('_hold_set_phase', source)  # contact-selected 8 or 5 visit holds
        self.assertIn('_hold_clear_phase', source)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'compiled.c'
            path.write_text(source + '\nvoid use_generated(Fighter *fighter) {\n' +
                            '\n'.join('  %s(fighter);' % symbol(entry) for entry in entries) + '\n}\n')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_immediate_pose_step_requires_bounds_and_does_not_add_a_wait(self):
        # 45a560 ->4285b0 submits the pose and increments its cursor without
        # yielding. Kai reaction4e9308 supplies its own two-visit holds.
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x0f000000, descriptor, 0x0d000000, 0x34000002, 0x3f000000])
        frames = generate(self.image, [entry], 11, 'fighter_ready')
        self.assertIn('fighter->sample_override = 1;', frames)
        self.assertIn('.lastframe = 2', frames)  # two explicit waits plus controller return
        entry = self.fixture([0x0d000000, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            generate(self.image, [entry], 11, 'fighter_ready')
        entry = self.fixture([0x1100000c, descriptor, 0x0d000000, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'next pose exceeds bound clip'):
            generate(self.image, [entry], 11, 'fighter_ready')
        actual = generate(self.image, [0x4e9308], 11, 'mk4_script_thrown_ready')
        self.assertIn('mk4_mmove_bind(fighter, 134, 18, 0)', actual)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'next_pose.c'
            path.write_text(actual + '\nvoid use_next_pose(Fighter *fighter) { retail_script_004e9308(fighter); }\n')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_mmove_sweep_is_generated_without_yielding_calls(self):
        source = format_c(generate(self.image, [0x4e6798], 4, 'fighter_ready'))
        self.assertIn('static const mframe_t retail_script_004e6798_frames', source)
        self.assertIn('battle_sound(SOUND_A, 175)', source)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'frames.c'
            path.write_text(source + '\nvoid script_sweep_tail(Fighter *fighter) { fighter_ready(fighter); }\n'
                            'void use_frames(Fighter *fighter) { retail_script_004e6798(fighter); }\n')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)
        bindings = bindings_for(4)
        bindings[0x46f080] = dataclasses.replace(bindings[0x46f080], mmove=False)
        with self.assertRaisesRegex(CompileError, 'not reviewed for nonblocking'):
            generate(self.image, [0x4e6798], 4, 'fighter_ready', bindings)
        bindings[0x46f080] = dataclasses.replace(bindings[0x46f080], mmove=True, flow='suspend')
        with self.assertRaisesRegex(CompileError, 'yielding binding'):
            generate(self.image, [0x4e6798], 4, 'fighter_ready', bindings)

    def test_mmove_counted_recoil_loops_and_phase_branches(self):
        source = generate(self.image, [0x4e84f8, 0x4e8530], 4, 'fighter_ready',
                          contexts={hex(e): dict(clip=95, count=11, sample=9, evidence='native slide contact boundary')
                                    for e in (0x4e84f8, 0x4e8530)})
        self.assertNotIn('loop_counter', source)
        self.assertIn('.lastframe = 15', source)  # four four-pose hit cycles
        self.assertIn('.lastframe = 23', source)  # six four-pose blocked cycles
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x0f000000, descriptor, 0x44000000, (self.entry + 20) // 4, 0x34000001, 0x3f000000])
        source = generate(self.image, [entry], 4, 'fighter_ready')
        self.assertIn('fighter->held & BUTTON_BIT(BUTTON_DOWN)', source)
        self.assertIn('_branch_1', source)

    def test_mmove_repeating_scene_and_complete_profile(self):
        from retail_coverage import profile_for
        profile = profile_for(4)
        entries = [int(root, 16) for root in profile['generated_scripts']]
        source = generate(self.image, entries, 4, profile['return_to'], contexts=profile['entry_contexts'])
        self.assertIn('mk4_mmove_hold', source)
        self.assertIn('retail_script_004e03d8_branch_', source)
        self.assertNotIn('goto L_', source)
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x0f000000, descriptor, 0x34000001, 0x33000000, (self.entry + 8) // 4])
        source = generate(self.image, [entry], 4, 'fighter_ready')
        self.assertNotIn('.duration', source)
        self.assertNotIn('.leave', source)
        self.assertIn('.thinkfunc', source)
        self.assertIn('_branch_1', source)

    def test_mmove_pair_requires_a_victim_binding(self):
        descriptor = (0x4fad7c + 76 * 12) // 4
        entry = self.fixture([0x0f000000, descriptor, 0x4b000019, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'known victim clip'):
            generate(self.image, [entry], 4, 'fighter_ready')
        source = generate(self.image, [0x4de798, 0x4e9bd0], 4, 'fighter_ready',
                          contexts={'0x4e9bd0': dict(clip=153, count=26, sample=25, evidence='native reaction 102 release')})
        self.assertIn('mk4_mmove_pair_step', source)
        self.assertNotIn('mk4_script_pair_to(fighter', source)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'pair.c'
            path.write_text(source + '\nvoid script_reaction(Fighter *f, int r) { (void)f; (void)r; }\n'
                            'void script_lp_owner_return(Fighter *f) { fighter_ready(f); }\n'
                            'void script_ready(Fighter *f) { fighter_ready(f); }\n'
                            'void use_frames(Fighter *f) { retail_script_004de798(f); retail_script_004e9bd0(f); }\n')
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_blend_bind_requires_source_pose_and_valid_sample_one(self):
        # Raiden's finisher opcode 71 blends the inherited pose, then resumes
        # destination playback at sample one. It must not silently start at zero.
        descriptor = (0x4fad7c + 54 * 12) // 4
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x71000000, descriptor, 0x3f000000])
        source = self.compile([0x0f000000, descriptor, 0x71000000, descriptor, 0x3f000000])
        self.assertIn('mk4_mmove_blend_bind(fighter, 54,', source)
        # Descriptor decoding and ordinary cursor validation apply to blends.
        struct.pack_into('<I', self.image.memory, descriptor * 4 + 4 - 0x400000, 1)
        with self.assertRaisesRegex(CompileError, 'animation cursor outside stored clip'):
            self.compile([0x0f000000, descriptor, 0x71000000, descriptor, 0x3f000000])

    def test_both_branch_outcomes_and_jumps_into_waits_remain_edges(self):
        # Down branches into the second wait, which must remain a distinct
        # target rather than being folded into its predecessor.
        e = self.entry
        descriptor = (0x4fad7c + 54 * 12) // 4
        source = self.compile([0x0f000000, descriptor, 0x44000000, (e + 20) // 4, 0x34000005, 0x34000002, 0x3f000000])
        self.assertIn('fighter->held & BUTTON_BIT(BUTTON_DOWN)', source)
        self.assertIn('.lastframe = 2', source)  # Down: the second wait and return
        self.assertIn('.lastframe = 7', source)  # Otherwise both waits and return

    def test_predicate_flags_and_binding_roles_are_typed(self):
        e = self.entry
        descriptor = (0x4fad7c + 54 * 12) // 4
        binding = Binding('test_gate', 'tests/test_script_codegen.py', ('predicate',), result='flags', mmove=True, evidence='synthetic predicate fixture')
        source = self.compile([0x0f000000, descriptor, 0x36000000, 0x46de60, (e + 24) // 4, 0x34000001, 0x3f000000], {0x46de60: binding})
        self.assertIn('fighter->mmove_result = test_gate(fighter);', source)
        self.assertIn('fighter->mmove_result & 4u', source)
        with self.assertRaisesRegex(CompileError, 'cannot be used as predicate'):
            self.compile([0x36000000, 0x46a950, (e + 16) // 4, 0x34000001, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'does not accept inline selector'):
            self.compile([0x6b000007, 0x3f000000])
        source = self.compile([0x0f000000, descriptor, 0x6c123456, 17, 0x3f000000])
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

    def test_character_selector_is_specialized_and_optional_prefix_is_not_speed(self):
        program, _ = compile_program(self.image, 0x4e5ca0, 3, bindings={0x46a760: Binding('fighter_ready', 'game/moves.c', ('tail',), flow='terminal', mmove=True, evidence='synthetic selector continuation')})
        self.assertEqual(next(row for row in program['instructions'] if row['opcode'] == '0x3c')['selected_character'], 3)
        source = generate(self.image, [0x4e9ec8], 3, bindings={0x460250: Binding('fighter_ready', 'game/moves.c', ('tail',),
                          flow='terminal', mmove=True, evidence='synthetic continuation binding for playback syntax test')})
        self.assertIn('mk4_mmove_bind(fighter, 247, 5, 0);', source)
        self.assertIn('{0, 1, 2, 3, 4, 4, 3, 2, 1}', source)
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
        # them preserves the repeated branch as phases, without a C call chain.
        source = generate(self.image, [0x4e62f0], 3, 'fighter_ready')
        self.assertIn('retail_script_004e62f0_branch_', source)
        for target in ('004e6330', '004e6360', '004e63b0'):
            self.assertNotIn('retail_script_' + target + '(fighter)', source)
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            compile_program(self.image, 0x4e61e0, 3, return_to='fighter_ready')
        context = dict(clip=262, count=10, sample=0, evidence='46bea0 opens clip before VM entry')
        source = generate(self.image, [0x4e61e0], 3, 'fighter_ready', contexts={'0x4e61e0': context})
        self.assertIn('mk4_script_inherit(fighter, 262, 10, 0)', source)
        with self.assertRaisesRegex(ValueError, 'Invalid entry clip'):
            compile_program(self.image, 0x4e61e0, 3, return_to='fighter_ready', context={**context, 'sample': 10})
        context = dict(clip=35, count=44, sample=38, paired=True, return_to='caller', evidence='42c0d0 damage continuation')
        with self.assertRaisesRegex(CompileError, 'explicit phase successor'):
            generate(self.image, [0x4de428], 3, contexts={'0x4de428': context})
        source = generate(self.image, [0x4de428], 3, contexts={'0x4de428': dict(context, successor='retail_script_004de468')})
        self.assertIn('retail_script_004de468(fighter);', source)

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
        self.assertIn('{0, 2, 4, 6, 8, 10, 12, 12, 12, 12}', source)  # nine double-step visits, clamped

    def test_contact_branch_and_reaction_context_are_structural(self):
        e = self.entry
        descriptor = (0x4fad7c + 54 * 12) // 4
        source = self.compile([0x0f000000, descriptor, 0x51000006, 0x42000000, (e + 24) // 4, 0x34000002, 0x3f000000])
        self.assertIn('fighter->mmove_result = battle_contact(', source)
        self.assertIn('fighter->mmove_result & 1u', source)
        self.assertIn('_branch_', source)
        with self.assertRaisesRegex(CompileError, 'flags is not initialized'):
            self.compile([0x42000000, (e + 8) // 4, 0x3f000000])
        source = self.compile([0x0f000000, descriptor, 0x4e000000, 0x3f000000])
        self.assertIn('fighter_finish_mmove(fighter, fighter_ready);', source)
        source = self.compile([0x11000003, descriptor, 0x34000001, 0x3f000000])
        self.assertIn('mk4_mmove_bind(fighter, 54, 13, 3)', source)

    def test_contact_query_d_lowers_inline_selector_to_researched_record(self):
        bindings = bindings_for(10)
        for address in (0x47a6d0, 0x47b010):
            bindings[address] = Binding('reptile_loop', 'game/reptile/character.c', ('tail',), flow='terminal', mmove=True,
                                        evidence='synthetic native-loop boundary for the retail D-contact fixture')
        source = generate(self.image, [0x4e8178], 10, 'fighter_ready', bindings=bindings)
        self.assertIn('static const AttackRecord retail_contact_4ec080', source)
        self.assertIn('battle_contact(fighter, &retail_contact_4ec080)', source)

    def test_advance_twice_to_sample_uses_packed_low_byte_target(self):
        binding = Binding('jarek_projectile', 'game/jarek/character.c', ('spawn',), ('fighter', 'task_type'),
                          flow='independent', task_types=(78,), mmove=True, evidence='synthetic native task boundary')
        source = generate(self.image, [0x4ed7f0], 12, 'fighter_ready', bindings={0x497580: binding})
        self.assertIn('{0, 2, 4, 6, 8, 10, 12, 14, 16, 17,', source)
        self.assertNotIn('529', source)

    def test_opcode_69_preserves_native_half_speed(self):
        binding = Binding('jax_projectile', 'tests/test_script_codegen.py', ('spawn',), ('fighter', 'task_type'),
                          flow='independent', task_types=(80,), mmove=True, evidence='synthetic native task boundary')
        source = generate(self.image, [0x4ed090], 13, 'fighter_ready', bindings={0x496fc0: binding})
        self.assertIn('25, 25, 26, 26, 27, 27', source)  # two visits per remaining pose
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x69000000, 0x3f000000])

        # Native45b6d0/428ea0 use two visits per remaining pose. Start
        # at10 of clip54's13 samples: the tail takes five visits, not three.
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x1100000a, descriptor, 0x69000000, 0x3f000000])
        slow = generate(self.image, [entry], 3, 'fighter_ready')
        self.assertIn('.lastframe = 5', slow)
        self.assertIn('10, 10, 11, 11, 12', slow)
        self.assertNotIn('mk4_script_finish_clip', slow)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'slow_tail.c'
            path.write_text(slow + '\nvoid use_slow_tail(Fighter *fighter) { retail_script_%08x(fighter); }\n' % entry)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_restarted_half_speed_preserves_prefix_and_resets_inherited_cursor(self):
        #45b750 parses45bf80's optional descriptor;428e80 resets to0
        #before428ea0 uses the same two-visit playback as opcode69.
        descriptor = (0x4fad7c + 54 * 12) // 4
        for words in ([0x68000200, descriptor, 0x3f000000],
                      [0x1100000a, descriptor, 0x68000000, 0x3f000000]):
            entry = self.fixture(words)
            frames = generate(self.image, [entry], 3, 'fighter_ready')
            self.assertIn('.lastframe = 25', frames)
            self.assertIn('0, 0, 1, 1, 2, 2, 3, 3', frames)
            self.assertNotIn('mk4_script_finish_clip', frames)
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x68000000, 0x3f000000])
        with self.assertRaisesRegex(CompileError, 'unsupported playback prefix'):
            self.compile([0x68000300, 0x3f000000])
        # Actual Fujin first-fatality opening: no handwritten VM body.
        binding = Binding('first_native_tail', 'tests/test_script_codegen.py', ('tail',),
                          flow='terminal', mmove=True, evidence='bounded native boundary44f5e0')
        real = generate(self.image, [0x4e1f58], 5, 'fighter_ready',
                        bindings={0x44f5e0: binding})
        self.assertIn('first_native_tail(fighter);', real)

    def test_counted_half_speed_supports_prefix_and_bounded_clip_end(self):
        # 45b800 optionally binds via45bf80 and runs429120 twice per pose.
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x46000204, descriptor, 0x3f000000])
        slow = generate(self.image, [entry], 3, 'fighter_ready')
        self.assertIn('.lastframe = 8', slow)
        self.assertIn('0, 0, 1, 1, 2, 2, 3, 3', slow)
        self.assertIn('0, 0, 1, 1, 2, 2, 3, 3, 4', slow)
        self.assertNotIn('mk4_script_advance_slow', slow)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'slow_count.c'
            path.write_text(slow + '\nvoid use_slow_count(Fighter *fighter) { retail_script_%08x(fighter); }\n' % entry)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)
        for count in (0, 4):
            entry = self.fixture([0x1100000a, descriptor, 0x46000000 | count, 0x3f000000])
            end = generate(self.image, [entry], 3, 'fighter_ready')
            self.assertIn('.lastframe = 5', end)
            self.assertIn('10, 10, 11, 11, 12', end)
        with self.assertRaisesRegex(CompileError, 'clip is not initialized'):
            self.compile([0x46000004, 0x3f000000])

    def test_generated_blend_keeps_source_pose_without_yielding(self):
        descriptor = (0x4fad7c + 54 * 12) // 4
        entry = self.fixture([0x1100000a, descriptor, 0x71000000, descriptor, 0x3f000000])
        source = generate(self.image, [entry], 3, 'fighter_ready')
        self.assertIn('mk4_mmove_blend_bind(fighter, 54, 13);', source)
        self.assertIn('fighter->blend_weight = 19005;', source)
        self.assertIn('fighter->blend_duration = 0;', source)
        self.assertNotIn('mk4_script_blend_bind(', source)
        self.assertNotIn('native_wait(', source)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'blend_frames.c'
            path.write_text(source + '\nvoid use_blend_frames(Fighter *fighter) { retail_script_%08x(fighter); }\n' % entry)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

    def test_frame_entry_can_preserve_a_reviewed_runtime_clip_choice(self):
        entry = self.fixture([0x03000000, 0x3f000000])
        context = {'clip': 'current', 'count': 13, 'sample': 10,
                   'evidence': 'Native caller selects one of equal-length clips at the same resume pose.'}
        source = generate(self.image, [entry], 3, 'fighter_ready', contexts={hex(entry): context})
        self.assertIn('mk4_mmove_inherit_current(fighter, 13, 10);', source)
        self.assertNotIn('mk4_script_inherit_current(', source)
        self.assertNotIn('mk4_mmove_bind(fighter, 54', source)
        with self.assertRaises(ValueError):
            generate(self.image, [entry], 3, 'fighter_ready', contexts={hex(entry): dict(context, sample=13)})
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'inherited_frames.c'
            path.write_text(source + '\nvoid use_inherited_frames(Fighter *fighter) { retail_script_%08x(fighter); }\n' % entry)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-fsyntax-only',
                            '-I' + str(ROOT / 'game'), '-I' + str(ROOT / 'common'), str(path)], check=True, capture_output=True, text=True)

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

    def test_cpu_fatalities_require_reviewed_minimums_and_use_retail_targets(self):
        import copy
        from retail_controls import generate_controls
        from retail_coverage import profile_for, inventory
        profile = profile_for(5)
        source = generate_controls(self.image, 5, profile)
        self.assertIn('[CPU_FINISH_1] = fujin_first_fatality', source)
        self.assertIn('[CPU_FINISH_2] = fujin_second_fatality', source)
        expected = ', '.join('0x%x' % value for value in inventory(self.image, 5)['finish_distances'])
        self.assertIn('.finish_distance = {' + expected + '}', source)
        partial = copy.deepcopy(profile)
        partial['command_bindings'].pop('0x467cd0')
        partial['cpu_finish_min'].pop('CPU_FINISH_2')
        one = generate_controls(self.image, 5, partial)
        self.assertNotIn('[CPU_FINISH_2]', one)
        self.assertIn('.finish_distance = {0x%x, 0x0}' % inventory(self.image, 5)['finish_distances'][0], one)
        broken = copy.deepcopy(profile)
        broken.pop('cpu_finish_min')
        with self.assertRaisesRegex(ValueError, 'require reviewed'):
            generate_controls(self.image, 5, broken)
        broken = copy.deepcopy(profile)
        broken['cpu_finish_min']['CPU_FINISH_1'] = self.image.word(0x4dfe10 + 5 * 4)
        with self.assertRaisesRegex(ValueError, 'below its retail distance'):
            generate_controls(self.image, 5, broken)

    def test_all_reviewed_generated_roots_remain_in_the_final_roster_catalog(self):
        from character_catalog import CHARACTERS, moves_for
        from retail_coverage import profile_for, inventory, fingerprint
        for character, name in enumerate(CHARACTERS):
            with self.subTest(character=name):
                profile = profile_for(character)
                selected = {root for move in moves_for(character).values() for _, root in move['scripts']}
                generated = {int(root, 16) for root in profile.get('generated_scripts', {})}
                self.assertFalse(generated - selected, 'Generated entries must survive the final fighter-specific catalog overrides')
                self.assertEqual(profile['inventory_sha256'], fingerprint(inventory(self.image, character)))

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
        binding = Binding('fighter_ready', 'game/moves.c', ('tail',), flow='terminal', mmove=True, evidence='synthetic native boundary')
        source = generate(self.image, [0x4e6500], 3, bindings={0x46e3d0: binding})
        self.assertIn('EXE 004e6544', source)

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
