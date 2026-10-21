"""Source registration audits for our shared runtime data boundary."""
import copy
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_coverage import audit, profile_for
from retail_moves import RetailImage


class ControlTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_audit_rejects_dropped_command_registration(self):
        profile = copy.deepcopy(profile_for(14))
        del profile['command_bindings']['0x467e00']
        errors = audit(self.image, 14, profile=profile)['coverage_errors']
        self.assertIn('unregistered retail command target 0x467e00', errors)

    def test_cpu_reachability_through_input_does_not_supply_missing_cpu_rows(self):
        profile = copy.deepcopy(profile_for(14))
        del profile['cpu_bindings']['0x479b20']
        errors = audit(self.image, 14, profile=profile)['coverage_errors']
        self.assertIn('unregistered retail CPU target 0x479b20', errors)


if __name__ == '__main__':
    unittest.main()
