"""Static coverage gates for character ports, separate from behavioral parity.

Inventory reads typed EXE tables and selected decoded VM graphs, without native
execution or bulk discovery. A reviewed manifest names existing C boundaries;
legacy VM bodies remain visible and never count as generated coverage.
"""
import hashlib
import json
from pathlib import Path
import re

from character_catalog import CHARACTERS, moves_for
from retail_bindings import bindings_for
from retail_codegen import CompileError, compile_program, instruction_binding, symbol, generate, format_c
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
        raise ValueError('Invalid character generation profile')
    return profile


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
    cpu = {}
    for name, base in CPU_TABLES.items():
        word = image.word(base + slot * 4)
        flags, action = word >> 16 & 255, word & 255
        dispatch = next((table for bit, table in DISPATCH if flags & bit), None)
        target = image.word(dispatch + action * 4) if dispatch else None
        cpu[name] = dict(table='0x%08x' % (base + slot * 4), word=hex(word), target=hex(target) if target else None)
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


def audit(image, character, return_to=None, profile=None):
    observed = inventory(image, character)
    profile = profile_for(character) if profile is None else profile
    errors = []
    bindings = bindings_for(character)
    continuation = return_to or (profile or {}).get('return_to')
    root_names = {}
    for name, move in moves_for(character).items():
        for role, entry in move['scripts']:
            root_names.setdefault(entry, []).append('%s: %s' % (name, role))
    generated = {int(entry, 16) for entry in (profile or {}).get('generated_scripts', [])}
    legacy = (profile or {}).get('legacy_scripts', {})
    contexts = (profile or {}).get('entry_contexts', {})
    for entry in contexts.keys() - {hex(root) for root in generated}:
        errors.append('%s: stale generated entry context' % entry)
    scripts = []
    for entry, names in sorted(root_names.items()):
        try:
            program, _ = compile_program(image, entry, character, bindings, contexts.get(hex(entry), {}).get('return_to', continuation), contexts.get(hex(entry)))
            status, diagnostics = 'compilable', []
        except (CompileError, ValueError) as exc:
            status = 'unsupported'
            diagnostics = exc.diagnostics if isinstance(exc, CompileError) else [dict(pc=hex(entry), reason=str(exc))]
        implementation = 'generated' if entry in generated else 'legacy' if hex(entry) in legacy else 'unreviewed'
        scripts.append(dict(entry=hex(entry), moves=names, status=status, implementation=implementation, diagnostics=diagnostics))
        if implementation == 'generated' and status != 'compilable':
            errors.append('%s: generated script no longer compiles' % hex(entry))
        if implementation == 'legacy':
            check_reference(legacy[hex(entry)], errors, 'legacy script ' + hex(entry))
        if implementation == 'unreviewed':
            errors.append('%s: script has no generation entry or reviewed legacy boundary' % hex(entry))
    for entry in generated - root_names.keys():
        errors.append('%s: generation entry is outside the selected catalog' % hex(entry))
    for entry in set(legacy) - {hex(root) for root in root_names}:
        errors.append('%s: stale legacy script boundary' % entry)
    for entry in {hex(root) for root in generated} & set(legacy):
        errors.append('%s: script cannot be both generated and legacy' % entry)
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
        integration = profile.get('integration_source')
        if generated:
            if not integration or not (ROOT / integration).is_file():
                errors.append('generated scripts need an integration source')
            else:
                source = read_source(ROOT / integration)
                if profile.get('mmove_only') and re.search(r'\bfighter_(?:start_fiber|wait_ticks|fiber_(?:move|sequence|play_to|finish))\s*\(', masked_c(source)):
                    errors.append('mmove-only integration still contains coroutine calls: ' + integration)
                referenced = {entry for entry in generated if re.search(r'\b' + symbol(entry) + r'\b|&' + symbol(entry) + r'_(?:repeat_)?phase\b', masked_c(source))}
                reachable = set(referenced)
                for entry in referenced:
                    try:
                        program, _ = compile_program(image, entry, character, bindings,
                                                     contexts.get(hex(entry), {}).get('return_to', continuation), contexts.get(hex(entry)))
                        reachable.update(int(row['va'], 16) for row in program['instructions'])
                    except (CompileError, ValueError):
                        pass # Per-root compilation errors were reported above.
                for entry in sorted(generated - reachable):
                    errors.append('%s: generated function is not referenced by integration source or a linked generated CFG' % hex(entry))
                include = (ROOT / profile['output']).relative_to((ROOT / integration).parent).as_posix()
                if '#include "%s"' % include not in source:
                    errors.append('integration source does not include ' + include)
    if generated and all(row['status'] == 'compilable' for row in scripts if row['implementation'] == 'generated'):
        output = ROOT / profile['output']
        try:
            expected = format_c(generate(image, generated, character, continuation, contexts=contexts, mmove_entries=profile.get('mmove_scripts', []), mmove_only=profile.get('mmove_only', False)))
            if not output.is_file() or output.read_text() != expected:
                errors.append('generated output is missing, stale or manually edited: ' + str(profile['output']))
        except ValueError as exc:
            errors.append('generation failed: ' + str(exc))
    # Every typed boundary is navigable to a real definition. Prose helper
    # names alone never satisfy this check.
    for address, binding in sorted(bindings.items()):
        check_reference(dict(source=binding.source, symbol=binding.symbol, evidence=binding.evidence), errors, 'binding ' + hex(address))
    return dict(schema=1, character=CHARACTERS[character], exe_sha256=EXE_SHA256,
                scope='Typed EXE inputs/CPU/stances/weapons, catalogued VM graphs and native frontiers; not native implementation parity or exhaustive x86 reachability',
                inventory_sha256=fingerprint(observed), scripts=scripts,
                native_frontier=observed['native_frontier'], inputs=observed['inputs'], cpu=observed['cpu'],
                stances=observed['stances'], weapon=observed['weapon'], reactions=observed['reactions'],
                compilable=sum(row['status'] == 'compilable' for row in scripts), total=len(scripts),
                generated=sum(row['implementation'] == 'generated' for row in scripts),
                legacy=sum(row['implementation'] == 'legacy' for row in scripts), coverage_errors=sorted(set(errors)))


def summary(report):
    lines = ['%s: %d generated, %d legacy VM roots; %d/%d graphs compilable.' %
             (report['character'], report['generated'], report['legacy'], report['compilable'], report['total']),
             '%d input event tables, %d CPU rows, %d stance tables, %d reactions (contacts and VM dispatch).' %
             (len(report['inputs']), len(report['cpu']), len(report['stances']), len(report['reactions'])),
             '%d coverage errors. Legacy bodies do not count as generated.' % len(report['coverage_errors'])]
    lines.extend('- ' + error for error in report['coverage_errors'])
    blockers = {}
    for script in report['scripts']:
        for row in script['diagnostics']:
            blockers.setdefault(row['reason'], []).append(row['pc'])
    if blockers:
        lines.append('Compiler blockers (all call sites are available in JSON audit):')
        lines.extend('- %s (%d sites)' % (reason, len(set(sites))) for reason, sites in sorted(blockers.items())[:8])
        if len(blockers) > 8:
            lines.append('- %d more blocker groups; use the JSON audit for the full list.' % (len(blockers) - 8))
    return '\n'.join(lines) + '\n'
