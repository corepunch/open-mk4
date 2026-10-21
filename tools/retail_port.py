"""Bounded source extraction and operand ledger for translating a move to C.

Existing ports are source excerpts, never automatic decompilation. Only character-module
function references are expanded. Native literals retain instruction provenance;
no operand is called Q16, damage, action or a host pointer just from its number.
"""
from collections import deque
from c_source import read_source, source_files, function_source
import re
import json

from character_catalog import moves_for, character_name
from retail_actions import catalog, native_evidence, NATIVE
from retail_helper_view import ROOT, helper_rows, port_source
from retail_symbols import native_name, reference_rows
from scorpion_moves import script_program

# This is a portable implementation index, separate from the retail root map.
COMMON = {'air_throw': ('game/mk4_common.c', 'mk4_air_throw_frames')}
ENTRIES = {
    'high_punch': '{c}_hp', 'lp_throw': 'enter_throw', 'lk_throw': 'enter_lk_throw',
    'jab': '{c}_jab', 'high_kick': '{c}_kick', 'low_kick': '{c}_lk',
    'sweep': '{c}_sweep', 'uppercut': '{c}_uppercut', 'roundhouse': '{c}_roundhouse',
    'close_hp': '{c}_hp', 'combo_hp': '{c}_combo_hp', 'close_hk': '{c}_hk',
    'crouch_lp': '{c}_crouch_lp', 'crouch_lk': '{c}_crouch_lk', 'crouch_hk': '{c}_crouch_hk',
    'air_hp': 'air_hp', 'air_lp': 'air_lp', 'air_kick': 'air_kick',
    'air_diagonal_hk': 'air_hk_diagonal', 'air_diagonal_lk': 'air_lk_diagonal',
    'draw_weapon': '{c}_weapon', 'fan_fatality': '{c}_fan_fatality', 'spike_fatality': '{c}_spike_fatality',
}
SPECIFIC = {
    0: {'spear': 'scorpion_spear', 'breathe_fire': 'scorpion_fire', 'teleport_punch': 'scorpion_teleport',
        'sword_back_lp': 'sword_back_lp', 'sword_back_hp': 'sword_back_hp', 'sword_spin': 'sword_spin',
        'fatality_1': 'scorpion_burn_fatality', 'fatality_2': 'scorpion_beast_fatality',
        'sword_throw': ('game/weapons.c', 'weapon_throw')},
    1: {'lightning': 'raiden_lightning', 'torpedo': 'raiden_torpedo', 'teleport': 'raiden_teleport',
        'staff_hp': 'weapon_hp', 'staff_lp': 'weapon_lp', 'staff_back_hp': 'weapon_back_hp', 'staff_back_lp': 'weapon_back_lp',
        'fatality_1': 'first_fatality', 'fatality_2': 'second_fatality', 'sidestep': ('game/mk4_common.c', 'mk4_sidestep'),
        'victory': 'enter_victory', 'weapon_throw': ('game/weapons.c', 'weapon_throw')},
    2: {'ring': 'sonya_ring', 'leg_grab': 'sonya_leg_grab', 'bicycle': 'sonya_bicycle', 'square_wave': 'sonya_square_wave', 'cartwheel': 'sonya_cartwheel',
        'weapon_hp': 'weapon_hp', 'weapon_lp': 'weapon_lp', 'weapon_back_hp': 'weapon_back_hp', 'weapon_back_lp': 'weapon_back_lp',
        'fatality_1': 'first_fatality', 'fatality_2': 'second_fatality', 'sidestep': ('game/mk4_common.c', 'mk4_sidestep'),
        'air_throw': 'sonya_air_throw', 'victory': 'enter_victory', 'weapon_throw': ('game/weapons.c', 'weapon_throw')},
    3: {
        'jab': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e62f0'),
        'high_kick': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6c90'),
        'low_kick': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6d08'),
        'uppercut': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e62a0'),
        'sweep': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e67d8'),
        'roundhouse': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6e90'),
        'close_hp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e61e0'),
        'combo_hp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e69b0'),
        'close_hk': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6190'),
        'crouch_lk': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6158'),
        'crouch_hk': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e60b8'),
        'air_hp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6658'),
        'air_lp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6710'),
        'air_kick': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6df0'),
        'air_diagonal_hk': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6500'),
        'air_diagonal_lk': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e63d8'),
        'lp_throw': ('game/liu_kang/generated/scripts.inc', 'retail_script_004de798'),
        'lk_throw': ('game/liu_kang/generated/scripts.inc', 'retail_script_004de3b0'),
        'bicycle_kick': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e8610'),
        'flying_kick': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e86a8'),
        'high_fireball': ('game/liu_kang/generated/scripts.inc', 'retail_script_004ed6c0'),
        'low_fireball': ('game/liu_kang/generated/scripts.inc', 'retail_script_004ed678'),
        'air_fireball': ('game/liu_kang/generated/scripts.inc', 'retail_script_004ed6f8'),
        'sword_hp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004ec680'),
        'sword_back_lp': ('game/liu_kang/generated/scripts.inc', 'retail_script_004ec7f0'),
        'draw_weapon': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e62c0'),
        'fatality_2': ('game/liu_kang/generated/scripts.inc', 'retail_script_004dfff8'),
        'fan_fatality': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e0118'),
        'spike_fatality': ('game/liu_kang/generated/scripts.inc', 'retail_script_004e6950'),
        'fatality_1': 'liu_kang_first_fatality',
        'sidestep': ('game/mk4_common.c', 'mk4_sidestep'),
        'sword_lp': 'sword_lp',
        'sword_back_hp': 'sword_back_hp',
        'weapon_throw': ('game/weapons.c', 'weapon_throw'),
        'fireball_reaction': ('game/reactions.c', 'react_liu_record'),
    },
    4: {'ice': 'sub_zero_ice', 'clone': 'sub_zero_clone', 'slide': 'sub_zero_slide', 'sidestep': ('game/mk4_common.c', 'mk4_sidestep'),
        'scepter_hp': 'scepter_hp', 'scepter_lp': 'scepter_lp', 'scepter_back_hp': 'scepter_back_hp', 'scepter_back_lp': 'scepter_back_lp',
        'fatality_1': 'sub_zero_first_fatality', 'fatality_2': 'sub_zero_second_fatality',
        'weapon_throw': ('game/weapons.c', 'weapon_throw')},
}

# Generated VM entries are first-class extraction sources. Native-only entries
# still point at their engine integration above.
for _character, _slug in ((0, 'scorpion'), (1, 'raiden'), (2, 'sonya'), (4, 'sub_zero')):
    for _name, _move in moves_for(_character).items():
        if _move['scripts'] and (_character not in (1, 2) or _name not in SPECIFIC[_character]):
            _root = _move['scripts'][0][1]
            _profile = json.loads((ROOT / 'game' / _slug / 'port.json').read_text())
            if hex(_root) in _profile.get('runtime_scripts', {}):
                SPECIFIC[_character][_name] = ('game/%s/character.c' % _slug, '%s_entry_retail_script_%08x' % (_slug, _root))
            else:
                SPECIFIC[_character][_name] = ('game/%s/generated/scripts.inc' % _slug, 'retail_script_%08x' % _root)


# This scene has one native coordinator for all fighters, with direct retail words.
for _character in SPECIFIC:
    SPECIFIC[_character]['fan_fatality'] = ('game/retail_scene.c', 'retail_fan_finish')
    SPECIFIC[_character]['high_punch'] = ('game/native_combat.c', 'retail_high_punch')
    SPECIFIC[_character]['jab'] = ('game/retail_combat.c', 'retail_jab')
    SPECIFIC[_character]['roundhouse'] = ('game/retail_combat.c', 'retail_roundhouse')
    SPECIFIC[_character]['uppercut'] = ('game/retail_combat.c', 'retail_uppercut')
    SPECIFIC[_character]['sweep'] = ('game/retail_combat.c', 'retail_sweep')
    SPECIFIC[_character]['high_kick'] = ('game/retail_combat.c', 'retail_high_kick')
    SPECIFIC[_character]['low_kick'] = ('game/retail_combat.c', 'retail_low_kick')


# All catalogued script roots now enter the shared interpreter. Keep extraction
# navigation on their native entry adapters while those adapters are retired.
for _character, _entries in SPECIFIC.items():
    _slug = {0: 'scorpion', 1: 'raiden', 2: 'sonya', 3: 'liu_kang', 4: 'sub_zero'}[_character]
    for _name, _entry in list(_entries.items()):
        if isinstance(_entry, tuple) and _entry[0].endswith('/generated/scripts.inc'):
            _entries[_name] = ('game/%s/character.c' % _slug, '%s_entry_%s' % (_slug, _entry[1]))


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
    if character not in SPECIFIC:
        return None
    if name in COMMON and character != 2:
        return COMMON[name]
    c = {0: 'scorpion', 1: 'raiden', 2: 'sonya', 3: 'liu_kang', 4: 'sub_zero'}[character]
    entry = SPECIFIC[character].get(name, ENTRIES.get(name))
    if isinstance(entry, tuple):
        return entry
    return ('game/%s/character.c' % c, entry.format(c=c)) if entry else None


def source_closure(port):
    """Lexical character-module references, including callback arguments; no C call-graph claim."""
    path, root = port
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
    from retail_codegen import instruction_binding
    bindings = bindings_for(character)
    used = {}
    for _, root in move['scripts']:
        for row in script_program(image, root, character)['instructions']:
            key = instruction_binding(row)
            if key and key[0] in bindings:
                used[key[0]] = bindings[key[0]]
    if not used:
        return []
    rows = ['', '## Typed compiler boundaries', '',
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
    rows = ['# %s: %s — C port worksheet' % (character_name(character), name), '',
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
