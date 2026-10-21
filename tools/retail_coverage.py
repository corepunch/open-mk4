"""Static coverage gates for character ports, separate from behavioral parity.

Inventory reads typed EXE tables and selected decoded VM graphs, without native
execution or bulk discovery. A reviewed manifest names existing C boundaries;
translated or legacy VM bodies are rejected; original words run in the shared runtime.
"""
import hashlib
import json
from pathlib import Path
import re

from character_catalog import CHARACTERS, moves_for
from retail_bindings import bindings_for
from retail_contracts import instruction_binding
from retail_helper_view import port_source
from retail_moves import EXE_SHA256
from retail_port import masked_c
from c_source import read_source, private_aliases
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


def profile_for(character):
    path = ROOT / 'game' / CHARACTERS[character] / 'port.json'
    if not path.is_file():
        return None
    profile = json.loads(path.read_text())
    if profile.get('schema') != 1 or profile.get('character') != CHARACTERS[character]:
        raise ValueError('Invalid runtime coverage profile')
    return profile


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
    descriptor = image.word(0x53d5c0 + slot * 4) * 4
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


def check_reference(reference, errors, label, table=False):
    if not isinstance(reference, dict) or not all(reference.get(key) for key in ('source', 'symbol', 'evidence')):
        errors.append('%s: source, symbol and evidence are required' % label)
        return
    path = (ROOT / reference['source']).resolve()
    if not path.is_relative_to(ROOT) or not path.is_file():
        errors.append('%s: missing workspace source %s' % (label, reference['source']))
        return
    if table:
        pattern = r'\b' + re.escape(private_aliases(path).get(reference['symbol'], reference['symbol'])) + r'\s*(?:\[[^;={}]*\])?\s*='
        if re.search(pattern, masked_c(read_source(path))) is None:
            errors.append('%s: missing C table %s' % (label, reference['symbol']))
    else:
        try:
            port_source((reference['source'], reference['symbol']))
        except ValueError as exc:
            errors.append('%s: %s' % (label, exc))


def audit(image, character, profile=None):
    observed = inventory(image, character)
    profile = profile_for(character) if profile is None else profile
    errors = []
    bindings = bindings_for(character)
    root_names = {}
    for name, move in moves_for(character).items():
        for role, entry in move['scripts']:
            root_names.setdefault(entry, []).append('%s: %s' % (name, role))
    runtime = (profile or {}).get('runtime_scripts', {})
    for key in ('generated_scripts', 'legacy_scripts', 'entry_contexts'):
        if (profile or {}).get(key):
            errors.append('%s: translated/legacy script configuration is forbidden; use the shared interpreter' % key)
    scripts = []
    for entry, names in sorted(root_names.items()):
        if hex(entry) in runtime:
            reference = runtime[hex(entry)]
            check_reference(reference, errors, 'runtime script ' + hex(entry))
            program = script_program(image, entry, character)
            limits = (ROOT / 'game/retail_vm.h').read_text()
            begin = int(re.search(r'#define RETAIL_SCRIPT_BEGIN (0x[0-9a-f]+)', limits)[1], 16)
            end = int(re.search(r'#define RETAIL_SCRIPT_END (0x[0-9a-f]+)', limits)[1], 16)
            if any(int(row['va'], 16) < begin or int(row['va'], 16) + 4 * (1 + len(row.get('operands', []))) > end
                   for row in program['instructions']):
                errors.append('%s: runtime data range does not contain the decoded graph' % hex(entry))
            supported = {int(value, 16) for value in re.findall(r'case (0x[0-9a-f]+):', (ROOT / 'game/retail_vm.c').read_text())}
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
            continue
        scripts.append(dict(entry=hex(entry), moves=names, status='unreviewed', implementation='missing', diagnostics=[]))
        errors.append('%s: script has no reviewed interpreter boundary' % hex(entry))
    for entry in set(runtime) - {hex(root) for root in root_names}:
        errors.append('%s: stale runtime script boundary' % entry)
    event_names = {table['button'] for table in observed['inputs']}
    for event in sorted(INPUT_EVENTS - event_names):
        errors.append('missing input event ' + event)
    if not profile:
        errors.append('no reviewed coverage profile for ' + CHARACTERS[character])
    else:
        if profile.get('inventory_sha256') != fingerprint(observed):
            errors.append('EXE/root inventory changed: review --inventory and update its reviewed fingerprint')
        errors.extend(unreachable_references(profile))
        references = profile.get('moves', {})
        for name in observed['moves']:
            check_reference(references.get(name), errors, 'move ' + name)
        for name in references.keys() - observed['moves'].keys():
            errors.append('stale move boundary ' + name)
        if profile.get('command_bindings'):
            bound = profile.get('command_bindings', {})
            required = {row['target'] for table in observed['inputs'] for row in table['records']}
            for target in sorted(required - bound.keys()):
                errors.append('unregistered retail command target ' + str(target))
            for target in bound.keys() - required:
                errors.append('stale runtime command binding ' + target)
            for target, reference in bound.items():
                check_reference(reference, errors, 'runtime command ' + target)
            cpu_bound = profile.get('cpu_bindings', {})
            cpu_required = {row['target'] for row in observed['cpu'].values()} - {None}
            for target in sorted(cpu_required - cpu_bound.keys()):
                errors.append('unregistered retail CPU target ' + str(target))
            for target in cpu_bound.keys() - cpu_required:
                errors.append('stale runtime CPU binding ' + target)
            for target, reference in cpu_bound.items():
                check_reference(reference, errors, 'runtime CPU ' + target)
            for target, reference in profile.get('cpu_finish_bindings', {}).items():
                check_reference(reference, errors, 'runtime CPU finish ' + target)
        entry_targets = {move['entry'] for move in observed['moves'].values()}
        extras = profile.get('entry_adapters', {})
        targets = {row['target'] for table in observed['inputs'] for row in table['records']} | {row['target'] for row in observed['cpu'].values()}
        targets.discard(None)  # Zeroed CPU rows have no retail action to implement.
        for target in sorted(targets - entry_targets, key=str):
            check_reference(extras.get(target), errors, 'input/CPU target ' + str(target))
        for name in observed['stances']:
            check_reference(profile.get('stances', {}).get(name), errors, 'stance ' + name, table=True)
        if observed['weapon']:
            check_reference(profile.get('weapon'), errors, 'weapon', table=True)
        # A registered reaction must match the actual C dispatch, rather than
        # just naming an arbitrary existing function. Paired/cinematic native
        # adapters explicitly declare their different dispatch ownership.
        reaction_source = (ROOT / 'game/reactions.c').read_text()
        table = re.search(r'static const FighterFn reactions\[\d+\]\s*=\s*\{(.*?)\};', masked_c(reaction_source), re.S)
        actual = dict(re.findall(r'\[(\d+)\]\s*=\s*(\w+)', table[1])) if table else {}
        for value, native in observed['reactions'].items():
            reference = profile.get('reactions', {}).get(value)
            check_reference(reference, errors, 'reaction ' + value + '/' + native)
            if reference and reference.get('source') == 'game/reactions.c' and actual.get(value) != reference.get('symbol'):
                errors.append('reaction %s: C dispatch %s differs from reviewed %s' % (value, actual.get(value), reference.get('symbol')))
            elif reference and reference.get('source') != 'game/reactions.c' and not reference.get('adaptation'):
                errors.append('reaction %s: non-table dispatch needs an explicit adaptation' % value)
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
