#!/usr/bin/env python3
"""Static symbol correspondence checks; never execute retail code."""
import hashlib
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from map_retail_symbols import assembly_bodies, bind_addresses, load_symbols, normalize_instruction, source_signature
from retail_symbols import MAP, MAPPED, mapped_symbol, native_name, reference_rows
from retail_moves import RetailImage


class SymbolTests(unittest.TestCase):
    def test_normalization_preserves_semantics_and_relocation_identity(self):
        self.assertEqual(normalize_instruction('mov eax, dword ptr [g_baseSel]')[0],
                         normalize_instruction('mov eax, dword ptr [0x53c420]')[0])
        self.assertNotEqual(normalize_instruction('mov eax, byte ptr [g_baseSel]')[0],
                            normalize_instruction('mov eax, dword ptr [g_baseSel]')[0])
        self.assertNotEqual(normalize_instruction('and eax, 4')[0], normalize_instruction('and eax, 1')[0])
        self.assertEqual(normalize_instruction('jne L_again', {'l_again': 3})[0], ('jne', '@3'))
        self.assertTrue(bind_addresses([('a',), ('a',)], [('0x53c420',), ('0x53c420',)]))
        self.assertFalse(bind_addresses([('a',), ('a',)], [('0x53c420',), ('0x53c41c',)]))
        self.assertFalse(bind_addresses([('a',), ('b',)], [('0x53c420',), ('0x53c420',)]))

    def test_all_mapped_bodies_match_supplied_executable(self):
        image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        self.assertGreater(len(MAPPED), 300)
        for row in MAPPED:
            lo, hi = int(row['local'], 16), int(row['end'], 16)
            self.assertEqual(hashlib.sha256(image.memory[lo - 0x400000:hi - 0x400000]).hexdigest(), row['local_sha256'], row['name'])
        # Do not arbitrarily name repeated wrappers or choose a nearest delta.
        self.assertTrue(any(row['name'] == 'func_GuardedDispatch4' for row in MAP['ambiguous']))
        self.assertNotIn('func_GuardedDispatch4', [row['name'] for row in MAPPED])

    def test_sub_zero_interior_entries_use_verified_container(self):
        for address in (0x47f4d0, 0x47f6d0, 0x49bc60, 0x49bcb0):
            row = mapped_symbol(address)
            self.assertIsNotNone(row)
            self.assertIn(row['name'], '\n'.join(reference_rows([address])))
        self.assertEqual(mapped_symbol(0x49bc60)['local'], '0x49bc00')
        self.assertIn('at_0049bc60', native_name(0x49bc60))
        self.assertNotEqual(native_name(0x49bc60), 'Vec2SumMul10ChainCompute')

    @unittest.skipUnless((ROOT / 'reference/mk4decomp/config/symbols.yaml').is_file(), 'Optional reference checkout unavailable')
    def test_mapping_provenance_and_representative_instruction_shapes(self):
        import capstone
        root = ROOT / 'reference/mk4decomp'
        symbols = {row['name']: row for row in load_symbols(root)}
        self.assertEqual(hashlib.sha256((root / 'config/symbols.yaml').read_bytes()).hexdigest(), MAP['symbols_sha256'])
        image = RetailImage(ROOT / 'MK4/MK-4.EXE')
        cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        for address in (0x47f480, 0x47f6d0, 0x49bc00):
            row = mapped_symbol(address)
            self.assertEqual(symbols[row['name']]['addr'], int(row['upstream'], 16))
            text = (root / row['assembly_source']).read_text()
            self.assertEqual(hashlib.sha256(text.encode()).hexdigest(), row['assembly_sha256'])
            body = next(item for item in assembly_bodies(text) if item[0] == row['name'])
            lo, hi = int(row['local'], 16), int(row['end'], 16)
            instructions = list(cs.disasm(bytes(image.memory[lo - 0x400000:hi - 0x400000]), lo))
            labels = {hex(ins.address): i for i, ins in enumerate(instructions)}
            expected = source_signature(body[1], body[2])
            actual = [normalize_instruction(ins.mnemonic + ' ' + ins.op_str, labels) for ins in instructions]
            self.assertEqual([item[0] for item in actual], [item[0] for item in expected])
            self.assertTrue(bind_addresses([item[1] for item in expected], [item[1] for item in actual]))


if __name__ == '__main__':
    unittest.main(verbosity=2)
