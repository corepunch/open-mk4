#!/usr/bin/env python3
"""Map upstream matching assembly to this EXE, without executing either game.

Only unique, complete instruction-shape matches are accepted. Registers, small
constants, operand widths, internal branches and repeated address references
must agree. Relocated addresses are bound consistently, never by a global delta.
The upstream binary is optional: matching __asm source supplies the signatures.
"""
import argparse
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re
import sys

from retail_moves import EXE_SHA256, RetailImage

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_REFERENCE = ROOT / 'reference/mk4decomp'
MAP_PATH = Path(__file__).with_name('retail_symbol_map.json')


def load_symbols(root):
    """Read the flat functions list without adding a PyYAML dependency."""
    rows, row = [], None
    for line in (root / 'config/symbols.yaml').read_text().splitlines():
        match = re.fullmatch(r'  - addr: (0x[0-9a-fA-F]+)', line)
        if match:
            row = {'addr': int(match[1], 16)}
            rows.append(row)
        elif row is not None:
            field = re.fullmatch(r'    (name|file|size): (.+)', line)
            if field:
                row[field[1]] = int(field[2], 0) if field[1] == 'size' else field[2].strip('"\'')
    return rows


def assembly_bodies(text):
    """Read matching asm only, never the NON_MATCHING C implementation."""
    pattern = r'__declspec\(naked\)\s+[\w\s*]+?\b(\w+)\s*\([^)]*\)\s*\{\s*__asm\s*\{(.*?)\n\s*\}'
    for match in re.finditer(pattern, text, re.S):
        body = re.sub(r'/\*.*?\*/|//[^\n]*', '', match[2], flags=re.S)
        lines, labels, valid = [], {}, True
        for line in body.splitlines():
            line = line.strip()
            if not line:
                continue
            label = re.fullmatch(r'(\w+):', line)
            if label:
                labels[label[1].lower()] = len(lines)
            elif re.match(r'[a-z][a-z0-9]*\s*(?:\s|$)', line, re.I) and not line.startswith(('_', '#')):
                lines.append(line)
            else:
                valid = False
        if valid and lines:
            yield match[1], lines, labels, text.count('\n', 0, match.start()) + 1


def normalize_instruction(line, labels=None):
    """Return a comparable instruction plus ordered relocation identities."""
    line = line.lower().strip()
    line = re.sub(r'\b(short|near ptr)\s+', '', line)
    line = re.sub(r'\b([0-9][0-9a-f]*)h\b', lambda m: hex(int(m[1], 16)), line)
    mnemonic, _, operands = line.partition(' ')
    mnemonic = {'jz': 'je', 'jnz': 'jne', 'retn': 'ret', 'sal': 'shl'}.get(mnemonic, mnemonic)
    operands = operands.strip()
    if mnemonic.startswith('j') or mnemonic in ('call', 'loop', 'loope', 'loopne'):
        if labels is not None and operands in labels:
            return (mnemonic, '@%d' % labels[operands]), ()
        if re.fullmatch(r'(?:0x[0-9a-f]+|\w+)', operands) and operands not in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'):
            return (mnemonic, 'ADDRESS'), (operands,)
    relocations = []

    def relocate(match):
        token = match[0]
        numeric = int(token, 0) if re.fullmatch(r'0x[0-9a-f]+|\d+', token) else None
        if numeric is not None:
            # Packed pointers are deliberately not guessed from their magnitude.
            if 0x400000 <= numeric < 0x1000000:
                relocations.append(token)
                return 'ADDRESS'
            return str(numeric & 0xffffffff)
        if token in ('offset',):
            return ''
        if token.startswith('g_') or token.startswith('str_') or token[:1].isupper():
            relocations.append(token)
            return 'ADDRESS'
        return token

    # Function symbols may not use the g_ prefix. OFFSET marks them explicitly.
    operands = re.sub(r'\boffset\s+(\w+)', lambda m: 'g_offset_' + m[1], operands)
    operands = re.sub(r'0x[0-9a-f]+|\b\d+\b|\b[a-z_]\w*\b', relocate, operands)
    operands = re.sub(r'\s+', '', operands)
    # Capstone canonicalizes constant/register order inside memory expressions.
    operands = re.sub(r'\[([^\]]+)\]', lambda m: '[' + '+'.join(sorted(m[1].split('+'))) + ']', operands)
    return (mnemonic, operands), tuple(relocations)


def source_signature(lines, labels):
    return [normalize_instruction(line, labels) for line in lines]


def bind_addresses(source, target):
    """Require one-to-one relocation bindings within the matched function."""
    forward, reverse = {}, {}
    for left, right in zip(source, target):
        if len(left) != len(right):
            return False
        for a, b in zip(left, right):
            if forward.setdefault(a, b) != b or reverse.setdefault(b, a) != a:
                return False
    return True


def match_body(signature, instructions, candidates, normalized):
    shape = [item[0] for item in signature]
    source_refs = [item[1] for item in signature]
    rough = [(op, 'ADDRESS' if operand.startswith('@') else operand) for op, operand in shape]
    matches = []
    for start in candidates:
        if normalized[start:start + len(rough)] != rough:
            continue
        part = instructions[start:start + len(shape)]
        if len(part) != len(shape):
            continue
        labels = {hex(ins.address): i for i, ins in enumerate(part)}
        actual = [normalize_instruction(ins.mnemonic + ' ' + ins.op_str, labels) for ins in part]
        if [item[0] for item in actual] == shape and bind_addresses(source_refs, [item[1] for item in actual]):
            matches.append((part[0].address, part[-1].address + part[-1].size))
    return matches


def generate(exe, reference):
    import capstone
    image = RetailImage(exe)
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    cs.skipdata = True
    # The supplied image's executable .text ends before 0x4d0000.
    instructions = [ins for base, data in image.sections if base == 0x401000 for ins in cs.disasm(data, base)]
    by_prefix = defaultdict(list)
    normalized = [normalize_instruction(ins.mnemonic + ' ' + ins.op_str)[0] for ins in instructions]
    width = 6
    for i in range(len(instructions) - width + 1):
        by_prefix[tuple(normalized[i:i + width])].append(i)
    print('Indexed %d retail instructions' % len(instructions), file=sys.stderr, flush=True)
    symbols = load_symbols(reference)
    bodies, files = {}, {}
    for symbol in symbols:
        source = symbol.get('file')
        if not source:
            continue
        if source not in files:
            path = reference / source
            files[source] = path.read_text(errors='replace') if path.is_file() else ''
            for name, lines, labels, line in assembly_bodies(files[source]):
                bodies.setdefault(name, []).append((source, lines, labels, line))
    print('Read %d matching-assembly names' % len(bodies), file=sys.stderr, flush=True)
    mapped, ambiguous, unavailable = [], [], []
    for index, symbol in enumerate(symbols):
        if index % 500 == 0:
            print('Matched %d/%d symbol candidates' % (index, len(symbols)), file=sys.stderr, flush=True)
        options = bodies.get(symbol['name'], [])
        found = {}
        for source, lines, labels, line in options:
            if len(lines) < 12:
                continue
            signature = source_signature(lines, labels)
            prefix = tuple(normalize_instruction(line)[0] for line in lines[:width])
            for local, end in match_body(signature, instructions, by_prefix.get(prefix, []), normalized):
                found[local] = dict(local=hex(local), upstream=hex(symbol['addr']), name=symbol['name'],
                                    source=symbol['file'], assembly_source=source, assembly_line=line, instructions=len(lines), end=hex(end),
                                    assembly_sha256=hashlib.sha256(files[source].encode()).hexdigest(),
                                    local_sha256=hashlib.sha256(image.memory[local - 0x400000:end - 0x400000]).hexdigest(),
                                    evidence='unique complete matching-assembly shape; consistent relocations and internal branches')
        if len(found) == 1:
            mapped.append(next(iter(found.values())))
        elif found:
            ambiguous.append(dict(name=symbol['name'], upstream=hex(symbol['addr']), candidates=[hex(a) for a in sorted(found)]))
        else:
            unavailable.append(symbol['name'])
    # Different upstream functions matching one local body are ambiguous too.
    counts = defaultdict(list)
    for row in mapped:
        counts[row['local']].append(row)
    accepted = [row for row in mapped if len(counts[row['local']]) == 1]
    for row in mapped:
        if len(counts[row['local']]) > 1:
            ambiguous.append(dict(name=row['name'], upstream=row['upstream'], candidates=[row['local']]))
    return dict(exe_sha256=EXE_SHA256, symbols_sha256=hashlib.sha256((reference / 'config/symbols.yaml').read_bytes()).hexdigest(),
                method='matching assembly; no numeric-address or uniform-delta fallback',
                mapped=sorted(accepted, key=lambda r: int(r['local'], 16)), ambiguous=ambiguous,
                unmatched=unavailable)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    parser.add_argument('--reference', type=Path, default=DEFAULT_REFERENCE)
    parser.add_argument('--output', type=Path, default=MAP_PATH)
    args = parser.parse_args()
    report = generate(args.exe, args.reference)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print('%d mapped; %d ambiguous; %d unmatched. Wrote %s' %
          (len(report['mapped']), len(report['ambiguous']), len(report['unmatched']), args.output))


if __name__ == '__main__':
    main()
