#!/usr/bin/env python3
"""Verify actionable Sub-Zero helper/data views without running retail code."""
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_moves import RetailImage
from c_source import canonical_name
from retail_helpers import HELPERS
from retail_helper_view import native_c_text, port_source, table_values
from retail_script_c import instruction_c, move_c_text, script_c_text
from character_catalog import moves_for
from scorpion_moves import script_program


class HelperTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_ice_has_flow_gates_contacts_and_reusable_contracts(self):
        text = move_c_text(self.image, 'ice', character=4)
        for value in ('16 decoded instructions; 16 shown', 'indexed_sound_event(4);', 'rewind_to_sample(18);',
                      'spawn_task(66, ice_projectile_task);', 'window=26', '0x45f1f0',
                      '67/67/0', 'fighter_fast_start', 'ice_projectile', 'C[3]', '75, 1, 0x40d720'):
            self.assertIn(value, text)
        self.assertNotIn('vm_helper_', text)
        # A previously hidden opcode is an indexed sound table, not bank B.
        self.assertEqual(table_values(self.image, 'indexed_sounds')[4]['token'], 0x852)
        self.assertIn('NOT bank B', text)
        self.assertIn('SOUND_INDEXED', text)
        self.assertIn('[`sound_event`]', text)
        sounds = table_values(self.image, 'indexed_sounds')
        self.assertEqual(len(sounds), 66)
        self.assertEqual(sounds[13]['token'], 2100)
        self.assertEqual(sounds[65]['token'], 0)

    def test_air_throw_exposes_shared_runner_and_victim_release(self):
        text = move_c_text(self.image, 'air_throw', character=4)
        for value in ('indexed_sound_event(13);', 'advance_pair_to_sample(28);',
                      'set_motion_q16(1966, -6553, 655);', 'wait_ticks(10);',
                      'set_motion_q16(0, 13107, 655);', 'scene_impact();', 'wait_ticks(4);',
                      '0x2147'):
            self.assertIn(value, text)
        self.assertEqual(HELPERS[0x46aa80].port, ('game/retail_air_throw.c', 'retail_air_throw'))
        self.assertEqual(HELPERS[0x46aae0].port, ('game/retail_air_throw.c', 'retail_air_throw_release'))
        self.assertEqual(table_values(self.image, 'impact_voices')[4]['token'], 925)
        self.assertEqual([self.image.word(0x4e4b60 + i * 4) for i in range(7)],
                         [0xffffffff, 0xffffffff, 0x01040002, 0x02ddaadd, 0, 0x1c413c40, 0])

    def test_native_only_clone_and_scepter_emit_actual_c_adaptations(self):
        clone = move_c_text(self.image, 'clone', character=4)
        for value in ('void sub_zero_clone(', 'clone_spin(', 'clone_object(',
                      'snapshot', '74', '53', '180', 'clip 301', 'shared_g.geo'):
            self.assertIn(value.lower(), clone.lower())
        self.assertIn('not automatically decompiled x86', clone)
        self.assertIn('game/native/motion.c:', clone)
        scepter = move_c_text(self.image, 'scepter_back_lp', character=4)
        self.assertIn('static void weapon_native(', scepter)
        self.assertIn('weapon_start_retail_strike(', scepter)
        self.assertIn('game/retail_weapon.c:', scepter)
        self.assertIn('reaction 151', scepter)
        self.assertIn('zero direct damage', scepter)

    def test_recovered_presentation_data_and_headers(self):
        image = self.image
        self.assertEqual([r['mesh'] for r in table_values(image, 'ice_meshes')], list(range(75, 83)))
        self.assertTrue(all(r['ticks'] == 1 and r['callback'] == 0x40d720 for r in table_values(image, 'ice_meshes')))
        self.assertEqual(table_values(image, 'ice_trail'), [{'mesh': 339, 'ticks': 10}])
        self.assertEqual(table_values(image, 'camera_shakes')[5], {'pulses': 2, 'half_cycle_ticks': 3, 'dy': 0x2e14})
        self.assertEqual(table_values(image, 'camera_first'), [{'x': 0x20000, 'y': -0x20000, 'z': -0x30000}] * 7)
        self.assertEqual(table_values(image, 'camera_orbit')[0], {'x': -0x50000, 'y': -0x28000, 'z': -0x70000})
        self.assertEqual(table_values(image, 'weapon_bounces'), [
            {'spin_z': 0x20c4, 'launch_y': -0x2666, 'rest_angle': 0x4b5c2},
            {'spin_z': 0x23d7, 'launch_y': -0x1999, 'rest_angle': 0x191eb},
            {'spin_z': 0, 'launch_y': -0x147a, 'rest_angle': 0x191eb}])
        self.assertEqual([r['mesh'] for r in table_values(image, 'blood_splash')], list(range(4, 13)))
        self.assertEqual([r['mesh'] for r in table_values(image, 'spine_rig')], list(range(36, 41)))
        self.assertEqual(table_values(image, 'weapon_descriptor')[0]['type'], 0x6b)
        # Inputs are decoded, not just copied from prose or the current port.
        changed = RetailImage(ROOT / 'MK4/MK-4.EXE')
        struct.pack_into('<I', changed.memory, 0x4e3674 - 0x400000, 7)
        self.assertEqual(table_values(changed, 'camera_shakes')[5]['pulses'], 7)

    def test_all_catalogued_script_wrappers_are_actionable_and_ports_exist(self):
        for move in moves_for(4).values():
            for _, root in move['scripts']:
                for row in script_program(self.image, root, 4)['instructions']:
                    self.assertFalse(any('vm_helper_' in line for line in instruction_c(row)), row)
        for helper in HELPERS.values():
            if helper.port:
                line, code = port_source(helper.port)
                self.assertGreater(line, 0)
                self.assertIn(canonical_name(*helper.port) + '(', code)
                self.assertTrue(code.rstrip().endswith('}'))
        native = native_c_text(self.image, 0x457b50)
        self.assertIn('two pulses, three ticks', native)
        self.assertIn('void battle_camera_shake(', native)
        self.assertNotIn('blood burst', HELPERS[0x457b50].effects)
        self.assertIn('NOT emit a blood burst', HELPERS[0x4456c0].effects)

    def test_native_c_cli_unknown_boundary_and_read_only_queries(self):
        tool = str(ROOT / 'tools/disassemble_retail.py')
        with tempfile.TemporaryDirectory() as directory:
            base = [sys.executable, tool, '--exe', str(ROOT / 'MK4/MK-4.EXE'), '--archive', 'missing', '--c']
            for query in (['--native', '49bc60'], ['--character', 'sub_zero', '--move', 'clone']):
                result = subprocess.run([*base, *query], cwd=directory, capture_output=True, text=True, check=True)
                self.assertIn('C adaptation', result.stdout)
            result = subprocess.run([*base, '--native', '401234'], cwd=directory, capture_output=True, text=True, check=True)
            self.assertIn('Remaining native boundaries', result.stdout)
            self.assertIn('No portable C body is recorded', result.stdout)
            bad = subprocess.run([*base, '--native', '570000'], cwd=directory, capture_output=True, text=True)
            self.assertNotEqual(bad.returncode, 0)
            self.assertIn('outside executable sections', bad.stderr)
            self.assertEqual(list(Path(directory).iterdir()), [])


if __name__ == '__main__':
    unittest.main(verbosity=2)
