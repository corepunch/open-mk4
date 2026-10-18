"""Retail-derived command generation and integration regressions."""
import copy
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_controls import generate_controls
from retail_coverage import audit, profile_for
from retail_moves import RetailImage
from c_reachability import DefinitionGraph, unreachable_references


class ControlTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_generated_stage_command_uses_retail_pattern_and_gate(self):
        text = generate_controls(self.image, 14, profile_for(14))
        self.assertIn('BUTTON_HIGH_KICK, {TOKEN_DOWN, TOKEN_FORWARD, TOKEN_FORWARD}, 3, 40, 0x0, mk4_can_fan_finish', text)
        self.assertIn('BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD}, 3, 40, 0x0, mk4_can_spike_finish', text)
        self.assertNotIn('0x467e00', text)

    def test_existing_noob_shadow_commands_survive_table_generation(self):
        text = generate_controls(self.image, 17, profile_for(17))
        low = text.index('NULL, noob_saibot_command_498b70')
        high = text.index('noob_saibot_shadow_up_enabled, noob_saibot_command_498e70')
        self.assertLess(low, high)
        self.assertIn('BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 26, 0x10', text)

    def test_unreviewed_predicate_is_rejected(self):
        profile = copy.deepcopy(profile_for(14))
        del profile['command_predicates']['0x45e5a0']
        with self.assertRaisesRegex(ValueError, 'Unreviewed command predicate'):
            generate_controls(self.image, 14, profile)

    def test_audit_rejects_dropped_command_registration(self):
        profile = copy.deepcopy(profile_for(14))
        del profile['command_bindings']['0x467e00']
        errors = audit(self.image, 14, profile=profile)['coverage_errors']
        self.assertIn('unregistered retail command target 0x467e00', errors)
        self.assertTrue(any('runtime command table differs' in error for error in errors))

    def test_generated_table_must_be_connected_to_the_character(self):
        graph = DefinitionGraph({'game/cage/character.c': '''
static const Commands retail_commands = {0};
const CharacterDef mk4_cage = {0};
'''})
        with patch('c_reachability.workspace_graph', return_value=graph):
            errors = unreachable_references(profile_for(14))
        self.assertTrue(any('runtime command table: unreachable implementation' in error for error in errors))


if __name__ == '__main__':
    unittest.main()
