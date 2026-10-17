"""Bounded action-code research, sharing the C catalog rather than guessing names.

Portable references are lexical navigation aids, not an exhaustive C data-flow
analysis. Native evidence is explicitly selected and checked against this EXE.
No archive, native execution, Capstone or upstream checkout is required.
"""
import ast
from pathlib import Path
import re

from character_catalog import character_name, moves_for
from scorpion_moves import script_program

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / 'game/retail_actions.inc'
ROW = re.compile(r'RETAIL_ACTION\((ACTION_RETAIL_[0-9A-F]+),\s*(0x[0-9a-f]+),\s*("[^"\n]*"),\s*("[^"\n]*")\)')


def catalog():
    result = {}
    for line, text in enumerate(CATALOG.read_text().splitlines(), 1):
        match = ROW.fullmatch(text.strip())
        if not match:
            if text.lstrip().startswith('RETAIL_ACTION('):
                raise ValueError('Invalid action catalog row %d' % line)
            continue
        name, value, confidence, description = match.groups()
        value = int(value, 16)
        if value in result or name != 'ACTION_RETAIL_%04X' % value:
            raise ValueError('Duplicate or inconsistent action catalog value')
        result[value] = dict(name=name, confidence=ast.literal_eval(confidence),
                             description=ast.literal_eval(description), line=line)
    return result


# value: (role, query entry, exact instruction site, expected bytes, meaning).
# Addresses are for our checked EXE, not imported upstream numeric addresses.
NATIVE = {
    0x1007: (
        ('writer', 0x459760, 0x459766, 'b807100000a32cc4530089048d74000000', 'Opcode 2d writes the current controller action at +0x74.'),
        ('reader', 0x49b170, 0x49b2d1, '3d07100000', 'Spear victim continuation accepts owner action 1007 or 300; otherwise takes reaction/cleanup path 49af80.'),
    ),
    0x1006: (
        ('writer', 0x470e80, 0x470e9e, 'b806100000a32cc4530089049574000000', 'Shared recovery helper called by opcode 29 (handler 45b190).'),
    ),
    0x300: (
        ('writer', 0x49a630, 0x49a636, 'b800030000a32cc4530089048d74000000', 'Scorpion spear entry writes controller action 300.'),
        ('reader', 0x49b170, 0x49b2dd, '3d00030000', 'Alternative to owner action 1007 in the spear victim continuation.'),
    ),
}


def native_evidence(image, value):
    rows = []
    for role, entry, site, expected, note in NATIVE.get(value, ()):
        raw = bytes.fromhex(expected)
        if bytes(image.memory[site - 0x400000:site - 0x400000 + len(raw)]) != raw:
            raise ValueError('Action evidence mismatch at %08x' % site)
        rows.append('- **%s** at `%08x`: %s Query: `--native %08x`.' % (role, site, note, entry))
    return rows


def script_writers(image, value, character):
    # Only reached, decoded opcode boundaries from the selected catalog roots.
    # Never search arbitrary data words for opcode-looking integers.
    found = {}
    roots = {}
    for move, data in moves_for(character).items():
        for role, root in data['scripts']:
            roots.setdefault(root, []).append('%s (%s)' % (move, role))
    for root, names in sorted(roots.items()):
        program = script_program(image, root, character)
        for row in program['instructions']:
            if {0x29: 0x1006, 0x2d: 0x1007}.get(int(row['opcode'], 16)) == value:
                found.setdefault(int(row['va'], 16), set()).update(names)
    return ['- `%08x`: set action `%#x`; %s.' % (site, value, ', '.join(sorted(names)))
            for site, names in sorted(found.items())]


def portable_references(value, entries):
    name = entries.get(value, {}).get('name')
    pattern = re.compile(r'(?:\b\w+->action|\.action|\bvictim_action\([^;\n]*?\))\s*(==|!=|=|<=|>=|<|>)\s*'
                         r'(0x[0-9a-fA-F]+|\d+|ACTION_RETAIL_[0-9A-F]+)\b')
    rows = []
    for path in sorted((ROOT / 'game').glob('*.c')):
        text = path.read_text()
        # Keep line numbers while excluding comments and strings.
        masked = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"',
                        lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text, flags=re.S)
        lines = text.splitlines()
        seen = set()
        for match in pattern.finditer(masked):
            token = match[2]
            if not (token == name or (not token.startswith('ACTION_') and int(token, 0) == value)):
                continue
            line = masked.count('\n', 0, match.start()) + 1
            role = 'writer' if match[1] == '=' else 'reader'
            if (line, role) not in seen:
                seen.add((line, role))
                rows.append('- **%s** [%s:%d](%s:%d): `%s`' %
                            (role, path.relative_to(ROOT), line, path, line, lines[line - 1].strip().replace('`', '')))
    return rows


def action_text(image, value, character=0, limit=80):
    if not 0 <= value <= 0xffffffff:
        raise ValueError('Action code must fit an unsigned 32-bit value')
    entries = catalog()
    entry = entries.get(value)
    rows = ['# Retail action %#x' % value, '',
            'Controller field `+0x74`; action codes are separate from animation IDs, reaction selectors and our broad StateId.', '']
    if entry:
        rows += ['- C name: `%s`.' % entry['name'], '- Meaning confidence: **%s**.' % entry['confidence'],
                 '- Recorded use: ' + entry['description'],
                 '- Catalog: [retail_actions.inc:%d](%s:%d).' % (entry['line'], CATALOG, entry['line'])]
    else:
        rows += ['**Uncatalogued action.** No meaning or C identifier is inferred from the number.']
    remaining = limit
    sections = [('Selected native evidence (checked instruction bytes)', native_evidence(image, value)),
                ('Decoded script writers — ' + character_name(character), script_writers(image, value, character)),
                ('Portable C writers/readers (lexical navigation)', portable_references(value, entries))]
    for title, items in sections:
        rows += ['', '## ' + title, '']
        shown = items[:remaining]
        rows += shown
        if not items:
            rows += ['No recorded matches in this scope.']
        remaining -= len(shown)
        if len(shown) < len(items):
            rows.append('%d additional matches omitted by --limit.' % (len(items) - len(shown)))
    if value == 0x1007:
        rows += ['', '## Upstream clue', '',
                 'The matching assembly calls this container `RoundOverFsmCluster`, but our verified caller is the spear victim continuation. '
                 'That name is not evidence for a round-end meaning.', '',
                 '- [Upstream comparison](%s:155594).' % (ROOT / 'reference/mk4decomp/src/engine/five_block_dispatch_variants.c')]
    rows += ['', 'Coverage: selected native sites, decoded selected-roster catalog roots, and direct portable C field comparisons/assignments. '
             'Script paths retain the existing bounded decoder budget. Indirect assignments, masked classifications, uncatalogued roots and transitive '
             'native behavior are not exhaustively indexed. No matches does not establish absence of behavior.', '']
    return '\n'.join(rows)
