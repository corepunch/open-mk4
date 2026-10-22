"""Minimal gates for our interpreter registration audit, not authored moves."""
from dataclasses import replace
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from retail_bindings import bindings_for
from retail_coverage import audit
from retail_moves import RetailImage
import check_native_addresses


class AddressNamingTests(unittest.TestCase):
    def test_shared_asset_sources_are_checked_with_game_sources(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'game').mkdir()
            (root / 'common').mkdir()
            (root / 'common/addresses.h').write_text('enum { ROSTER = 0x4eb2e8 };\n')
            source = root / 'common/mk4_assets.c'
            for expression, rejected in (('0x4eb2e8', True), ('ROSTER', False)):
                source.write_text('/* evidence: 0x4eb2e8 */\nvoid load(void) { read(' + expression + '); }\n')
                output = StringIO()
                with patch.object(check_native_addresses, 'ROOT', root), redirect_stdout(output):
                    self.assertEqual(check_native_addresses.main(), rejected)
                if rejected:
                    self.assertIn('common/mk4_assets.c:2: unnamed retail address 0x4eb2e8', output.getvalue())


class RuntimeAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = RetailImage(ROOT / 'MK4/MK-4.EXE')

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
