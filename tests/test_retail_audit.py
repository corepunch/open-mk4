"""Minimal gates for our interpreter registration audit, not authored moves."""
import copy
from dataclasses import replace
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_bindings import bindings_for
from retail_coverage import audit, profile_for
from retail_moves import RetailImage


class RuntimeAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

    def test_translated_script_configuration_is_rejected(self):
        profile = copy.deepcopy(profile_for(0))
        profile['generated_scripts'] = [next(iter(profile['runtime_scripts']))]
        errors = audit(self.image, 0, profile=profile)['coverage_errors']
        self.assertTrue(any('translated/legacy script configuration is forbidden' in error for error in errors))

    def test_missing_or_wrong_native_role_is_rejected(self):
        for missing in (True, False):
            bindings = bindings_for(0)
            if missing:
                del bindings[0x46a8d0]
            else:
                bindings[0x46a8d0] = replace(bindings[0x46a8d0], roles=('call',), flow='immediate', mmove_lowering='')
            with patch('retail_coverage.bindings_for', return_value=bindings):
                errors = audit(self.image, 0)['coverage_errors']
            self.assertTrue(any('runtime native 0046a8d0' in error for error in errors))


if __name__ == '__main__':
    unittest.main()
