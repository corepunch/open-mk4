"""Small, read-only views of decoded scripts and native entry graphs.

No recursive native closure, archive loading, emulation, or dump generation.
"""
from scorpion_moves import attack_record, script_program, signed_word

CONTACT_TABLES = {'A': 0x53d268, 'B': 0x53d2b8}


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
        details.append('%s[%d]; %s' % ('B' if op == 0x51 else 'A', row['contact']['selector'], attack_text(row['contact'])))
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
            details.append('%s=%s' % (key, row[key]))
    if 'selected_character' in row:
        details.append('character=%d -> %s' % (row['selected_character'], row['character_table'][row['selected_character']]))
    # Keep untyped operands visible; do not invent field names or semantics.
    if row.get('operands') and not any(key in row for key in ('clip', 'contact', 'speed_q16', 'native_target', 'branch', 'character_table')) and op not in (0x1f, 0x63):
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
    rows.extend('- %s: %s -> %s' % (edge['source'], edge['kind'], edge['target']) for edge in edges[:limit])
    if len(edges) > limit:
        rows.append('- %d additional boundaries omitted by --limit.' % (len(edges) - limit))
    if not edges:
        rows.append('- None encountered in this script; this is not whole-move coverage.')
    rows += ['', 'Inspect a dependency with --native ADDRESS; inspect a script branch with --script ADDRESS.']
    return '\n'.join(rows) + '\n'


def native_text(disassembly, entry, limit=80, raw=False):
    if not disassembly.code(entry):
        raise ValueError('Native entry is outside executable sections')
    function = disassembly.function(entry, limit=limit)
    rows = ['# Native %08x — %s' % (entry, disassembly.label(entry)), '',
            'Exact-entry graph, at most %d instructions; dependencies are not recursively expanded.' % limit, '', '```asm']
    for pc, insn in sorted(function['instructions'].items()):
        rows.append('%08x  %s%s %s' % (pc, insn.bytes.hex(' ') + '  ' if raw else '', insn.mnemonic, insn.op_str))
    rows += ['```', '', 'Dependencies (follow with --native ADDRESS):']
    for kind, target in function['edges']:
        rows.append('- %s: %08x %s' % (kind, target, disassembly.label(target)))
    rows += ['', 'Direct script entries (follow with --script ADDRESS):']
    rows.extend('- %08x -> %08x' % (site, script) for site, script in function['scripts'])
    rows += ['', 'Unresolved / limited:']
    rows.extend('- %08x: %s%s' % (site['at'], site['reason'], ' ' + site['expression'] if 'expression' in site else '') for site in function['unresolved'])
    if not function['unresolved']:
        rows.append('- None encountered locally; native behavior is not thereby understood.')
    return '\n'.join(rows) + '\n'


def contact_text(image, table, selector):
    table = table.upper()
    if table not in CONTACT_TABLES or not 0 <= selector <= 255:
        raise ValueError('Attack query requires table A or B and a selector in 0..255')
    return '# Attack %s[%d]\n\n%s\n' % (table, selector, attack_text(attack_record(image, CONTACT_TABLES[table], selector)))
