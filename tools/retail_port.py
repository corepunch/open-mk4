"""Bounded native source navigation and retail operand provenance.

Existing ports are source excerpts, never automatic decompilation. Only character-module
function references are expanded. Native literals retain instruction provenance;
no operand is called Q16, damage, action or a host pointer just from its number.
"""
from collections import deque
from c_source import read_source, source_files, function_source, canonical_name
import re

from character_catalog import moves_for, character_name, CHARACTERS
from retail_actions import catalog, native_evidence, NATIVE
from retail_helper_view import ROOT, helper_rows, port_source
from retail_helpers import HELPERS
from retail_symbols import native_name, reference_rows
from scorpion_moves import script_program

PORT_NOTES = {
    (4, 'lp_throw'): 'Retail selector 4de798[4] chooses 4de6b8: owner/victim 76/153, damage 0x2666 at sample 25, '
                    'owner return after sample 48. Reaction 102 plays 142/141/143, waits 20 and reverses into getup 328. '
                    'The engine uses its pair axis for camera-relative tumble motion and keeps each roster bind-ground offset.',
    (4, 'lk_throw'): 'Retail 42c180/4de490/4de4b8: owner/victim 41/165; owner-only opening 10, paired counts 29+5+8, '
                    'damage 0x2e14 at 52, final paired 5 through 57, then owner idle hold 60. The saved -0x1999 entry velocity '
                    'is restored at damage with opposite victim momentum. Victim 4e9ee0 reverses 45->41 and cycles to 46 three '
                    'times, then plays clip 200. World alignment and friction use the engine body context.',
}


def masked_c(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda m: re.sub(r'[^\n]', ' ', m[0]), text, flags=re.S)


def port_entry(character, name):
    move = moves_for(character).get(name, {})
    if name in ('draw_weapon', 'weapon_followup'):
        return ('game/retail_weapon.c', 'retail_weapon_draw')
    # Armed button names navigate to shared selectors; retail data chooses
    # the actual weapon routine and inventory at runtime.
    if name.startswith(('weapon_', 'sword_', 'staff_', 'scepter_', 'axe_')):
        for suffix, symbol in (('back_hp', 'back_high_punch'), ('back_lp', 'back_low_punch'),
                               ('hp', 'high_punch'), ('lp', 'low_punch')):
            if name.endswith('_' + suffix):
                return ('game/retail_weapon.c', 'retail_weapon_' + symbol)
    from retail_coverage import native_entries
    symbol = native_entries().get(move.get('entry'))
    if symbol:
        from c_source import public_definitions
        paths = public_definitions().get(symbol, set())
        if len(paths) == 1:
            return (next(iter(paths)), symbol)
    helper = HELPERS.get(move.get('entry'))
    if helper and helper.port:
        path, symbol = helper.port
        return path, canonical_name(path, symbol)
    if move.get('scripts'):
        return ('game/retail_vm.c', 'retail_vm_enter')
    return None


def source_closure(port):
    """Lexical character-module references, including callback arguments; no C call-graph claim."""
    path, root = port
    root = canonical_name(path, root)
    source = read_source(ROOT / path)
    masked = masked_c(source)
    definitions = {m[1] for m in re.finditer(r'^[A-Za-z_][\w* \t]*\b(\w+)\s*\([^;{}]*\)\s*\{', masked, re.M)}
    pending, found = deque([root]), {}
    while pending:
        name = pending.popleft()
        if name in found:
            continue
        line, code = port_source((path, name))
        found[name] = (line, code)
        tokens = set(re.findall(r'\b\w+\b', masked_c(code)))
        pending.extend(sorted((tokens & definitions) - found.keys()))
    return found


def source_rows(port, limit):
    path, entry = port
    found = source_closure(port)
    rows = ['', '## Existing engine C — entry and local helpers', '',
            'Read from the current workspace, not decompiled x86. Character-module function references (including callbacks) are expanded.',
            'These are review/extraction fragments: retain their supporting data/macros and engine APIs; they are not a standalone translation unit.', '']
    remaining = limit
    for name, (line, code) in found.items():
        actual, line, _ = function_source(path, name)
        rows.append('- [%s](%s:%d).' % (name, ROOT / actual, line))
        count = len(code.splitlines())
        if count <= remaining:
            rows += ['', '```c', code, '```', '']
            remaining -= count
        else:
            rows += ['Complete body omitted by --limit; use the source link. No partial function is emitted.', '']
    # Supporting data are navigation links: include exact definitions, not guesses
    # at C initializer types or macro expansion. Do not recursively expand tables.
    used = set(re.findall(r'\b\w+\b', masked_c('\n'.join(code for _, code in found.values()))))
    data = []
    for actual in source_files(ROOT / path):
        masked = masked_c(actual.read_text())
        data.extend((m[1], actual, masked.count('\n', 0, m.start()) + 1) for m in
                    re.finditer(r'^static const [^;={}]+?\b(\w+)\s*(?:\[[^;={}]*\])?\s*=', masked, re.M) if m[1] in used)
    rows += ['### Supporting local data', '']
    rows += ['- [%s](%s:%d).' % (name, actual, line) for name, actual, line in data]
    if not data:
        rows.append('No direct local const definitions referenced.')
    rows += ['', 'Engine calls and dynamically selected weapon/button callbacks remain explicit source boundaries. '
             'Use `--c` for retail control flow and helper contracts; source references are lexical navigation, not exhaustive C data flow.']
    return rows


def compiler_binding_rows(image, move, character):
    from retail_bindings import bindings_for
    from retail_contracts import instruction_binding
    bindings = bindings_for(character)
    used = {}
    for _, root in move['scripts']:
        for row in script_program(image, root, character)['instructions']:
            key = instruction_binding(row)
            if key and key[0] in bindings:
                used[key[0]] = bindings[key[0]]
    if not used:
        return []
    rows = ['', '## Typed runtime boundaries', '',
            'Direct decoded edges only. These links identify engine ownership and lifetime; native semantics remain separate evidence.', '']
    for address, binding in sorted(used.items()):
        if not (ROOT / binding.source).is_file():
            rows.append('- `%08x`: %s, %s; generated integration is not yet installed.' % (address, binding.symbol, binding.flow))
            continue
        line, _ = port_source((binding.source, binding.symbol))
        rows.append('- `%08x`: [%s](%s:%d), %s; %s.' %
                    (address, binding.symbol, ROOT / binding.source, line, binding.flow, binding.evidence))
    return rows


def script_operands(image, move, character, limit):
    rows = ['', '## Script operands — exact words and decoded roles', '']
    remaining, native = limit, set()
    for role, entry in move['scripts']:
        if remaining <= 0:
            rows.append('Additional script roots omitted by --limit; query --script %08x.' % entry)
            continue
        program = script_program(image, entry, character)
        instructions = program['instructions'][:remaining]
        rows += ['', '%s: `--script %08x --raw --character %d`.' % (role, entry, character), '', '```text']
        for row in instructions:
            from retail_query import instruction_text
            rows.append(instruction_text(row, raw=True))
            native.update(int(row[key], 16) for key in ('handler', 'native_target', 'callback') if row.get(key))
        rows += ['```']
        remaining -= len(instructions)
        if len(instructions) < len(program['instructions']):
            rows.append('Additional decoded instructions omitted by --limit.')
    if not move['scripts']:
        rows.append('Native move: no VM script root is catalogued; inspect the native literal ledger below.')
    return rows, native


def native_literals(disassembly, entries, limit):
    """Inspect selected exact-entry graphs, sharing one instruction budget."""
    from capstone.x86_const import X86_OP_IMM
    rows = ['', '## Native literals and functions — supplied EXE', '',
            'One shared budget of %d native instructions. No recursive native closure. Values below are instruction operands, '
            'not inferred C arguments or automatically classified Q16/action values.' % limit, '']
    remaining, references = limit, set()
    seen = set()
    for entry in dict.fromkeys(entries):
        if not disassembly.code(entry):
            rows.append('- `%08x`: outside native code; no function inferred.' % entry)
            continue
        if remaining <= 0:
            rows.append('- Omitted by --limit: `--native %08x` %s.' % (entry, native_name(entry)))
            continue
        function = disassembly.function(entry, limit=remaining)
        references.add(entry)
        rows += ['### `%08x` — %s' % (entry, native_name(entry)), '', '```text']
        for pc, insn in sorted(function['instructions'].items()):
            remaining -= 1
            if pc in seen:
                continue
            seen.add(pc)
            literals = [operand.imm & 0xffffffff for operand in insn.operands if operand.type == X86_OP_IMM]
            if not literals:
                continue
            role = 'control target' if insn.mnemonic.startswith(('j', 'call', 'loop')) else 'literal'
            rows.append('%08x  %-36s %s %s  // %s: %s' %
                        (pc, insn.bytes.hex(' '), insn.mnemonic, insn.op_str, role, ', '.join('%#x / %d' % (v, v if v < 0x80000000 else v - 0x100000000) for v in literals)))
        rows += ['```']
        for kind, target in function['edges']:
            rows.append('- %s: `--native %08x` %s.' % (kind, target, native_name(target)))
            references.add(target)
        for site, script in function['scripts']:
            rows.append('- Script at `%08x`: `--script %08x` (retain the selected --character).' % (site, script))
        for unresolved in function['unresolved']:
            rows.append('- Boundary `%08x`: %s.' % (unresolved['at'], unresolved['reason']))
    rows += reference_rows(references)
    return rows


def port_text(disassembly, name, character=0, limit=512):
    moves = moves_for(character)
    if name not in moves:
        raise ValueError('Unknown %s move %r; use --list-moves' % (character_name(character), name))
    move = moves[name]
    image = disassembly.image
    rows = ['# %s: %s — native integration worksheet' % (character_name(character), name), '',
            'Retail entry `--native %08x`; EXE bytes are authoritative. mk4decomp links use checked assembly correspondence, '
            'not equal numeric addresses or assumed original symbols.' % move['entry']]
    port = port_entry(character, name)
    if (character, name) in PORT_NOTES:
        rows += ['', '**Port notes:** ' + PORT_NOTES[character, name]]
    rows += source_rows(port, limit) if port else ['', 'No portable C entry is indexed for this character/move. No other character body is substituted.']
    operands, native = script_operands(image, move, character, limit)
    rows += compiler_binding_rows(image, move, character)
    rows += operands
    from retail_query import CONTACT_TABLES, attack_text
    from scorpion_moves import attack_record, attack_record_at
    rows += ['', '## Exact contact records', '']
    for table, selector in move['contacts']:
        record = attack_record_at(image, selector) if table == 'record' else attack_record(image, CONTACT_TABLES[table], selector)
        rows.append('- ' + attack_text(record))
        values = [*record['rectangle_q16'], record['reaction'], record['alternate_reaction'], record['blocked_reaction'], record['flags'], record['damage_q16']]
        rows += ['```c', 'static const AttackRecord retail_contact_%s = {%s};' %
                 (record['va'].replace('0x', ''), ', '.join(hex(value) for value in values)), '```']
    if not move['contacts']:
        rows.append('No separate contact record catalogued; decoded script operands and native contracts retain their own contacts.')
    entries = [move['entry'], *(address for _, address in move['native']), *sorted(native)]
    rows += native_literals(disassembly, entries, limit)
    rows += ['', '## Action-code evidence', '']
    selected = {value for value, sites in NATIVE.items() if any(entry in entries for _, entry, _, _, _ in sites)}
    if port:
        text = '\n'.join(code for _, code in source_closure(port).values())
        selected.update(int(value, 16) for value in re.findall(r'\bACTION_RETAIL_([0-9A-F]+)\b', masked_c(text)))
    for value in sorted(selected):
        entry = catalog().get(value)
        rows.append('- `%s = %#x`: %s (%s). Research: `--action %x`.' %
                    (entry['name'] if entry else 'uncatalogued', value, entry['description'] if entry else 'No meaning recorded',
                     entry['confidence'] if entry else 'unresolved', value))
        rows += native_evidence(image, value)
    rows += helper_rows(entries, image, include_related=True, limit=limit)
    rows += ['', 'Budgets are separate: %d C source lines, %d decoded script instructions, and %d native instructions. '
             'Omissions retain exact follow-up queries/source links. Values absent from this bounded worksheet are not established absent from the move.' %
             (limit, limit, limit), '']
    return '\n'.join(rows)
