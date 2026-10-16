"""Small, read-only views of decoded scripts and native entry graphs.

No recursive native closure, archive loading, emulation, or dump generation.
"""
from scorpion_moves import attack_record, attack_record_at, script_program, signed_word
from retail_symbols import NAMES, reference_rows, target_text

CONTACT_TABLES = {'A': 0x53d268, 'B': 0x53d2b8, 'WEAPON': 0x53d0c0}


def address(value):
    """Addresses are hex, including bare forms such as 004e6d08."""
    result = int(value, 16)
    if result % 4 or not 0x400000 <= result < 0x580000:
        raise ValueError('Expected an aligned image address in hexadecimal')
    return result


def q16(value):
    return '%g (%#x Q16)' % (value / 65536, value)


def attack_text(record):
    rectangle = ', '.join(q16(value) for value in record['rectangle_q16'])
    return ('record %s: rect [reach, top, length, height]=[%s]; damage %.4g%% (%#x); '
            'reactions hit/air/block=%d/%d/%d; flags=%#x' %
            (record['va'], rectangle, record['damage_q16'] * 100 / 65536,
             record['damage_q16'], record['reaction'], record['alternate_reaction'],
             record['blocked_reaction'], record['flags']))


def instruction_text(row, raw=False):
    """Render only established fields; unknown helper semantics stay explicit."""
    op = int(row['opcode'], 16)
    text = row['operation']
    details = []
    if 'clip' in row:
        details.append('%s clip=%d stored_samples=%d' % (row['clip_owner'], row['clip'], row['stored_count']))
    if 'start_sample' in row:
        details.append('sample=%d' % row['start_sample'])
    if 'contact' in row:
        details.append('%s[%d]; %s' % (row['contact_table'], row['contact']['selector'], attack_text(row['contact'])))
    elif 'speed_q16' in row:
        details.append('speed=' + q16(row['speed_q16']))
    elif op == 0x63:
        value = signed_word(int(row['operands'][0], 16))
        details.append('damage=%s; %.4g%%' % (q16(value), value * 100 / 65536))
    elif op == 0x37:
        details.append('bit0 clear=%d ticks; set=%d ticks' % (row['wait_if_clear'], row['wait_if_set']))
    elif op in (0x02, 0x04, 0x06, 0x10, 0x19):
        details.append('count=%d' % row['inline'])
    elif op in (0x05, 0x12):
        details.append('sample=%d' % row['inline'])
    elif op == 0x40:
        details.append('task_type=%d' % row['task_type'])
    elif op == 0x34:
        details.append('ticks=%d' % row['inline'])
    elif op == 0x1f:
        details.append('selector=%s' % row['operands'][0])
    elif row.get('argument') is not None:
        details.append('argument=%d' % row['argument'])
    elif 'inline' in row:
        details.append('inline=%#x' % row['inline'])
    for key in ('native_target', 'callback', 'branch'):
        if key in row:
            details.append('%s=%s' % (key, target_text(row[key]) if key != 'branch' else row[key]))
    if 'selected_character' in row:
        details.append('character=%d -> %s' % (row['selected_character'], row['character_table'][row['selected_character']]))
    # Keep untyped operands visible; do not invent field names or semantics.
    if row.get('operands') and not any(key in row for key in ('clip', 'contact', 'speed_q16', 'native_target', 'callback', 'branch', 'character_table')) and op not in (0x1f, 0x63):
        details.append('words=' + ','.join(row['operands']))
    if raw:
        details.append('raw_fixed=' + ' '.join([row['word'], *row.get('operands', [])]))
    return '%08x  %s%s' % (int(row['va'], 16), text, '; ' + '; '.join(details) if details else '')


def script_text(image, entry, limit=80, raw=False):
    program = script_program(image, entry)
    instructions = program['instructions']
    unknown = sum('unresearched' in row['operation'] for row in instructions)
    rows = ['# Script %08x (Scorpion selection)' % entry, '',
            '%d decoded rows; %d unknown opcode stops. Native semantics remain separate.' % (len(instructions), unknown), '', '```text']
    rows.extend(instruction_text(row, raw) for row in instructions[:limit])
    rows.append('```')
    if len(instructions) > limit:
        rows.append('Truncated: %d rows omitted; raise --limit (up to 512).' % (len(instructions) - limit))
    edges = program['unresolved_native_edges']
    rows += ['', 'Dependencies / unresolved boundaries (not no-ops):']
    rows.extend('- %s: %s -> %s' %
                (edge['source'], edge['kind'], target_text(edge['target']) if edge['target'].startswith('0x') else edge['target'])
                for edge in edges[:limit])
    if len(edges) > limit:
        rows.append('- %d additional boundaries omitted by --limit.' % (len(edges) - limit))
    if not edges:
        rows.append('- None encountered in this script; this is not whole-move coverage.')
    rows += reference_rows(int(row[key], 16) for row in instructions[:limit]
                           for key in ('native_target', 'callback') if row.get(key))
    rows += ['', 'Inspect a dependency with --native ADDRESS; inspect a script branch with --script ADDRESS.']
    return '\n'.join(rows) + '\n'


def native_text(disassembly, entry, limit=80, raw=False):
    if not disassembly.code(entry):
        raise ValueError('Native entry is outside executable sections')
    function = disassembly.function(entry, limit=limit)
    rows = ['# Native %08x — %s' % (entry, disassembly.label(entry)), '',
            'Exact-entry graph, at most %d instructions; dependencies are not recursively expanded.' % limit, '', '```asm']
    for pc, insn in sorted(function['instructions'].items()):
        target = disassembly.direct(insn) if insn.mnemonic in ('call', 'jmp') else None
        comment = ' ; ' + disassembly.label(target) if target is not None else ''
        rows.append('%08x  %s%s %s%s' % (pc, insn.bytes.hex(' ') + '  ' if raw else '', insn.mnemonic, insn.op_str, comment))
    rows += ['```', '', 'Dependencies (follow with --native ADDRESS):']
    for kind, target in function['edges']:
        rows.append('- %s: %08x %s' % (kind, target, disassembly.label(target)))
    rows += ['', 'Direct script entries (follow with --script ADDRESS):']
    rows.extend('- %08x -> %08x' % (site, script) for site, script in function['scripts'])
    rows += ['', 'Unresolved / limited:']
    rows.extend('- %08x: %s%s' % (site['at'], site['reason'], ' ' + site['expression'] if 'expression' in site else '') for site in function['unresolved'])
    if not function['unresolved']:
        rows.append('- None encountered locally; native behavior is not thereby understood.')
    rows += reference_rows([entry, *(target for _, target in function['edges'])])
    return '\n'.join(rows) + '\n'


def contact_text(image, table, selector):
    if table.lower() == 'record':
        return '# Attack record %08x\n\n%s\n' % (selector, attack_text(attack_record_at(image, address(hex(selector)))))
    table = table.upper()
    if table not in CONTACT_TABLES or not 0 <= selector <= 255:
        raise ValueError('Attack query requires A/B/weapon and selector 0..255, or record and a hex address')
    return '# Attack %s[%d]\n\n%s\n' % (table, selector, attack_text(attack_record(image, CONTACT_TABLES[table], selector)))


def move_list_text():
    from scorpion_catalog import MOVES
    rows = ['# Scorpion move queries', '',
            'Curated navigation roots, not exhaustive coverage or implementation status.', '',
            '| Move (--move NAME) | Native entry | Script roots |', '| --- | --- | --- |']
    rows.extend('| %s | %08x | %d |' % (name, move['entry'], len(move['scripts'])) for name, move in MOVES.items())
    return '\n'.join(rows) + '\n'


def move_text(image, name, limit=80, raw=False):
    from scorpion_catalog import MOVES
    from scorpion_moves import commands, attack_record_at
    if name not in MOVES:
        raise ValueError('Unknown Scorpion move %r; use --list-moves' % name)
    move = MOVES[name]
    rows = ['# Scorpion: ' + name, '',
            'Selected research roots only. Decoded instructions do not establish native semantics,',
            'whole-move coverage, timing totals, or C implementation status.', '',
            'Entry: `--native 0x%x`' % move['entry'], '', '## Input and gates', '']
    matched = False
    tokens = {0xaa: 'B', 0xdd: 'F', 2: 'D', 1: 'U'}
    for command_list in commands(image):
        for command in command_list['records']:
            if command['target'] != hex(move['entry']):
                continue
            matched = True
            pattern = ','.join(tokens.get(value, '%02x' % value) for value in command['tokens']) or 'unconditional'
            rows.append('- `%s`: %s + %s; window=%d ticks; flags=%s; predicate `%s` (inspect with --native).' %
                        (command['va'], pattern, command_list['button'], command['window'], command['flags'], command['predicate'] or 'none'))
    if not matched:
        rows.append('- No direct special-command record for this entry; inspect its stance/native caller for selection and gates.')
    rows += ['', '## Selected scripts', '']
    records, native, vm_helpers, stops = {}, {}, set(), []
    for table, selector in move['contacts']:
        record = attack_record_at(image, selector) if table == 'record' else attack_record(image, CONTACT_TABLES[table], selector)
        records[record['va']] = record
    for role, target in move['native']:
        native.setdefault(target, set()).add(role)
    total, shown, failures = 0, 0, []
    for role, entry in move['scripts']:
        try:
            program = script_program(image, entry)
        except ValueError as error:
            failures.append((entry, str(error)))
            rows.append('- %s: `--script 0x%x` failed: %s' % (role, entry, error))
            continue
        instructions = program['instructions']
        total += len(instructions)
        clips = sorted({row['clip'] for row in instructions if 'clip' in row})
        rows += ['### %s — `--script 0x%x`' % (role, entry), '',
                 '%d rows; clips %s.' % (len(instructions), ', '.join(map(str, clips)) or 'selected natively / none decoded'), '', '```text']
        preview = min(6, max(0, limit - shown))
        rows.extend(instruction_text(row, raw) for row in instructions[:preview])
        shown += min(preview, len(instructions))
        rows.append('```')
        if len(instructions) > preview:
            rows.append('%d rows omitted from preview; use the script query above.' % (len(instructions) - preview))
        rows.append('')
        for row in instructions:
            if 'contact' in row:
                records[row['contact']['va']] = row['contact']
            if 'unresearched' in row['operation']:
                stops.append((row['va'], row['opcode'], row['handler']))
        for edge in program['unresolved_native_edges']:
            if edge['kind'] == 'unknown opcode':
                continue
            if edge['kind'] == 'native helper semantics':
                vm_helpers.add(edge['target'])
            elif edge['target'].startswith('0x'):
                native.setdefault(int(edge['target'], 16), set()).add('%s at %s' % (edge['kind'], edge['source']))
            else:
                failures.append((int(edge['source'], 16), edge['kind']))
    if not move['scripts']:
        rows.append('No script roots catalogued here. Start at the native entry and selected helpers below.')
    rows += ['', '## Contacts and victim entry points', '']
    for record in records.values():
        rows.append('- ' + attack_text(record))
        reactions = sorted({record[key] for key in ('reaction', 'alternate_reaction', 'blocked_reaction')})
        rows.append('  Reaction table `4ea4e8`: ' + '; '.join('%d -> `--native 0x%x`' % (value, image.word(0x4ea4e8 + value * 4)) for value in reactions))
    if not records:
        rows.append('No contact records catalogued or decoded inline; inspect native queries/damage calls.')
    rows += ['', '## Follow-up tables', '']
    for role, table in move['buttons']:
        targets = [(button, image.word(table + index * 4)) for index, button in enumerate(('HP', 'LP', 'BLK', 'HK', 'LK', 'RUN'))]
        rows.append('- %s at `%08x`: %s. Zeros are disabled; native handlers still contain gates.' %
                    (role, table, '; '.join('%s=%08x' % (button, target) for button, target in targets)))
    if not move['buttons']:
        rows.append('No follow-up table catalogued here; absence is not proof that cancels are unavailable.')
    rows += ['', '## Native work to inspect', '']
    targets = list(native.items())
    for target, roles in targets[:12]:
        name = ' (%s)' % NAMES[target] if target in NAMES else ''
        rows.append('- `--native 0x%x`%s: %s' % (target, name, '; '.join(sorted(roles))))
    if len(targets) > 12:
        rows.append('- %d further native dependencies omitted; individual script queries list them.' % (len(targets) - 12))
    rows += ['', '## Decode boundaries', '',
             '%d selected roots; %d decoded rows; %d unknown opcode stops; %d decode/edge failures.' %
             (len(move['scripts']), total, len(stops), len(failures))]
    for site, opcode, handler in stops:
        rows.append('- Unknown %s at %s; handler %s. Path stopped.' % (opcode, site, handler or 'outside researched table'))
    for site, error in failures:
        rows.append('- %08x: %s' % (site, error))
    if vm_helpers:
        rows.append('- VM helper semantics need inspection: ' + ', '.join(sorted(vm_helpers)) + ' (use --native).')
    rows += ['- Native tails, predicates, reactions and computed paths remain separate research; counts cover only the selected roots.',
             '- C port status: docs/character-script-port.md (not inferred by this decoder).']
    rows += reference_rows(native)
    return '\n'.join(rows) + '\n'
