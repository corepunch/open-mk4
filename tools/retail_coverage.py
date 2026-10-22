"""Static coverage gates for character ports, separate from behavioral parity.

Inventory reads typed EXE tables and decoded VM graphs without native execution
or bulk discovery. Coverage checks the shared runtime and native registrations
directly; no per-fighter coverage declarations are required.
"""
import hashlib
import json
from pathlib import Path
import re

from character_catalog import CHARACTERS, LK_NATIVE_SELECTION, COMBO_NATIVE_SELECTION, moves_for
from retail_bindings import bindings_for
from retail_contracts import instruction_binding
from retail_helper_view import port_source
from retail_moves import EXE_SHA256
from retail_port import masked_c
from c_reachability import unreachable_references
from retail_query import CONTACT_TABLES
from scorpion_moves import commands, script_program, attack_record, attack_record_at, DISPATCH

ROOT = Path(__file__).resolve().parents[1]
INPUT_EVENTS = {'up', 'high_punch', 'low_punch', 'block', 'high_kick', 'low_kick', 'low_kick_release', 'run'}
STANCE_TABLES = {
    'standing': 0x4e5058, 'crouching': 0x4e5070, 'straight_air': 0x4e6ec8,
    'air_punch_followup': 0x4e6ee0, 'diagonal_air': 0x4e70d8,
    'close_hp_followup': 0x4e7078, 'combo_hp_followup': 0x4e7090,
    'close_hk_followup': 0x4e70a8, 'air_followup': 0x4e70c0,
}
CPU_TABLES = {'near': 0x4df8e8, 'medium': 0x4df928, 'far': 0x4df968, 'projectile': 0x4df9a8}


def cpu_records(image, character):
    slot = 2 if character == 16 else 7 if character == 17 else character
    cpu = {}
    for name, base in CPU_TABLES.items():
        word = image.word(base + slot * 4)
        flags, action = word >> 16 & 255, word & 255
        dispatch = next((table for bit, table in DISPATCH if flags & bit), None)
        target = image.word(dispatch + action * 4) if dispatch else None
        cpu[name] = dict(table='0x%08x' % (base + slot * 4), word=hex(word), target=hex(target) if target else None)
    return cpu


def inventory(image, character):
    moves = moves_for(character)
    native_slot = 2 if character == 16 else 4 if character == 17 else character
    if image.word(0x4ddfb8 + native_slot * 4) != LK_NATIVE_SELECTION[character]:
        raise ValueError('LK native research selection differs from the loaded retail table')
    if image.word(0x4e6c50 + native_slot * 4) != COMBO_NATIVE_SELECTION[character]:
        raise ValueError('Combo native research selection differs from the loaded retail table')
    roots = sorted({root for move in moves.values() for _, root in move['scripts']})
    programs = {root: script_program(image, root, character) for root in roots}
    contacts = {}
    for move in moves.values():
        for table, selector in move['contacts']:
            record = attack_record_at(image, selector) if table == 'record' else attack_record(image, CONTACT_TABLES[table.upper()], selector)
            contacts[record['va']] = record
    native = {}
    dispatched_reactions = set()
    def dependency(address, source):
        native.setdefault('0x%08x' % address, set()).add(source)
    for name, move in moves.items():
        dependency(move['entry'], 'entry:' + name)
        for role, address in move['native']:
            dependency(address, name + ':' + role)
    for root, program in programs.items():
        for row in program['instructions']:
            if 'contact' in row:
                contacts[row['contact']['va']] = row['contact']
            if row.get('handler') and int(row['handler'], 16) in (0x45b1f0, 0x45b230):
                dispatched_reactions.add(row['inline'] & 255)
            key = instruction_binding(row)
            if key:
                dependency(key[0], row['va'] + ':' + key[1])
    inputs = commands(image, character)
    slot = 2 if character == 16 else 7 if character == 17 else character
    cpu = cpu_records(image, character)
    stances = {name: dict(table=hex(base), entries=[hex(image.word(base + index * 4)) for index in range(6)]) for name, base in STANCE_TABLES.items()}
    descriptor = image.word(0x53d5c0 + character * 4) * 4
    weapon = None
    if descriptor:
        values = [image.word(descriptor + index * 4) for index in range(8)]
        button_table = values[3] * 4
        weapon = dict(descriptor=hex(descriptor), words=[hex(word) for word in values],
                      button_table=hex(button_table), buttons=[hex(image.word(button_table + index * 4)) for index in range(6)])
    reaction_ids = sorted(dispatched_reactions | {record[key] for record in contacts.values() for key in ('reaction', 'alternate_reaction', 'blocked_reaction')})
    reactions = {str(value): hex(image.word(0x4ea4e8 + value * 4)) for value in reaction_ids}
    return dict(schema=1, character=CHARACTERS[character], exe_sha256=EXE_SHA256,
                moves={name: dict(entry=hex(move['entry']), scripts=[hex(root) for _, root in move['scripts']],
                                 native=[hex(address) for _, address in move['native']]) for name, move in moves.items()},
                scripts={hex(root): [{'pc': row['va'], 'words': [row['word'], *row.get('operands', [])]} for row in program['instructions']]
                         for root, program in programs.items()},
                native_frontier={address: sorted(sites) for address, sites in sorted(native.items())},
                inputs=inputs, cpu=cpu, stances=stances, weapon=weapon, contacts=contacts, reactions=reactions,
                finish_distances=[image.word(base + slot * 4) for base in (0x4dfe10, 0x4dfe50)])


def fingerprint(observed):
    return hashlib.sha256(json.dumps(observed, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def check_reference(reference, errors, label):
    if not isinstance(reference, dict) or not all(reference.get(key) for key in ('source', 'symbol', 'evidence')):
        errors.append('%s: source, symbol and evidence are required' % label)
        return
    path = (ROOT / reference['source']).resolve()
    if not path.is_relative_to(ROOT) or not path.is_file():
        errors.append('%s: missing workspace source %s' % (label, reference['source']))
        return
    try:
        port_source((reference['source'], reference['symbol']))
    except ValueError as exc:
        errors.append('%s: %s' % (label, exc))


def native_entries():
    source = masked_c((ROOT / 'game/script/entries.inc').read_text())
    table = source.split('static const NativeAction entries[] = {', 1)[1].split('};', 1)[0]
    constants = {}
    for name in ('script/native_ids.h', 'native/actions.h'):
        constants.update({symbol: int(value, 16) for symbol, value in re.findall(
            r'(MK4_NATIVE_\w+) = (0x[0-9a-f]+)', (ROOT / 'game' / name).read_text())})
    result = {}
    for address, symbol in re.findall(r'\{(0x[0-9a-f]+|MK4_NATIVE_\w+),\s*(\w+)\}', table):
        value = int(address, 16) if address.startswith('0x') else constants[address]
        result[value] = symbol
    return result


def audit(image, character):
    observed = inventory(image, character)
    errors = unreachable_references({'character': CHARACTERS[character]})
    bindings = bindings_for(character)
    root_names = {}
    for name, move in moves_for(character).items():
        for role, entry in move['scripts']:
            root_names.setdefault(entry, []).append('%s: %s' % (name, role))
    scripts = []
    for entry, names in sorted(root_names.items()):
        program = script_program(image, entry, character)
        # This audit checks the original catalog/evidence and native registration.
        # Linked source layout is independently validated by mk4_link and its tests.
        supported = {int(value, 16) for value in re.findall(r'case (0x[0-9a-f]+):', (ROOT / 'game/script/vm.c').read_text())}
        if any(int(row['opcode'], 16) not in supported for row in program['instructions']):
            errors.append('%s: runtime opcode is unsupported' % hex(entry))
        for row in program['instructions']:
            boundary = instruction_binding(row)
            if boundary and boundary[0]:
                binding = bindings.get(boundary[0])
                if binding is None:
                    errors.append('%s: runtime native %08x has no registration' % (row['va'], boundary[0]))
                elif boundary[1] not in binding.roles:
                    errors.append('%s: runtime native %08x does not implement role %s' % (row['va'], boundary[0], boundary[1]))
        scripts.append(dict(entry=hex(entry), moves=names, status='interpreted', implementation='runtime', diagnostics=[]))
    event_names = {table['button'] for table in observed['inputs']}
    for event in sorted(INPUT_EVENTS - event_names):
        errors.append('missing input event ' + event)
    # Check actual shared dispatch registrations, not per-fighter declarations.
    entries = native_entries()
    for label, targets in (
        ('command', {row['target'] for table in observed['inputs'] for row in table['records']}),
        ('CPU', {row['target'] for row in observed['cpu'].values()} - {None}),
    ):
        for target in sorted(targets):
            if int(target, 16) not in entries:
                errors.append('unregistered retail %s target %s' % (label, target))
    # Every typed boundary is navigable to a real definition. Prose helper
    # names alone never satisfy this check.
    for address, binding in sorted(bindings.items()):
        check_reference(dict(source=binding.source, symbol=binding.symbol, evidence=binding.evidence), errors, 'binding ' + hex(address))
    return dict(schema=1, character=CHARACTERS[character], exe_sha256=EXE_SHA256,
                scope='Typed EXE inputs/CPU/stances/weapons, catalogued VM graphs and native frontiers; not native implementation parity or exhaustive x86 reachability',
                inventory_sha256=fingerprint(observed), scripts=scripts,
                native_frontier=observed['native_frontier'], inputs=observed['inputs'], cpu=observed['cpu'],
                stances=observed['stances'], weapon=observed['weapon'], reactions=observed['reactions'],
                total=len(scripts), generated=0,
                runtime=sum(row['implementation'] == 'runtime' for row in scripts),
                legacy=0, coverage_errors=sorted(set(errors)))


def summary(report):
    lines = ['%s: %d/%d catalogued roots interpreted.' % (report['character'], report['runtime'], report['total']),
             '%d input event tables, %d CPU rows, %d stance tables, %d reactions; %d coverage errors.' %
             (len(report['inputs']), len(report['cpu']), len(report['stances']), len(report['reactions']), len(report['coverage_errors']))]
    lines.extend('- ' + error for error in report['coverage_errors'])
    return '\n'.join(lines) + '\n'
