"""Source registration audits for our shared runtime data boundary."""
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_coverage import audit, native_entries
from retail_moves import RetailImage


class ControlTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_audit_rejects_dropped_command_registration(self):
        entries = native_entries()
        del entries[0x467e00]
        with patch('retail_coverage.native_entries', return_value=entries):
            errors = audit(self.image, 14)['coverage_errors']
        self.assertIn('unregistered retail command target 0x467e00', errors)

    def test_cpu_reachability_through_input_does_not_supply_missing_cpu_rows(self):
        entries = native_entries()
        del entries[0x479b20]
        with patch('retail_coverage.native_entries', return_value=entries):
            errors = audit(self.image, 14)['coverage_errors']
        self.assertIn('unregistered retail CPU target 0x479b20', errors)


if __name__ == '__main__':
    unittest.main()
