"""C-style presentation of researched straight-line VM operations.

This is pseudocode, not a native C port or a replacement VM. Unsupported
control flow stops the view rather than flattening the decoder's graph.
"""
import re

from retail_symbols import NAMES, reference_rows
from scorpion_moves import script_program


ANIMATION_NAMES = {67: 'ANIM_SCORPION_LOW_KICK'}


def animation_name(clip):
    return ANIMATION_NAMES.get(clip, 'ANIM_CLIP_%d' % clip)


def script_name(entry):
    from scorpion_catalog import MOVES
    for name, move in MOVES.items():
        for index, (role, root) in enumerate(move['scripts']):
            if root == entry:
                suffix = '' if index == 0 else '_' + re.sub(r'[^a-z0-9]+', '_', role.lower()).strip('_')
                return 'scorpion_' + name + suffix
    return 'script_%08x' % entry


def instruction_c(row):
    """Return presentation statements, or None at an unsupported boundary."""
    op = int(row['opcode'], 16)
    if op in (0x32, 0x35) and row['inline'] == 0:
        target = int(row['native_target'], 16)
        name = NAMES.get(target, 'unresolved_native_%08x' % target)
        return [name + '();'] + (['return;'] if op == 0x32 else [])
    if op == 0x0f:
        return ['open_animation(%s, %d); // %d stored samples' %
                (animation_name(row['clip']), row['start_sample'], row['stored_count'])]
    if op == 0x49 and (row['inline'] >> 8) in (0, 2):
        lines = ['bind_animation(%s);' % animation_name(row['clip'])] if 'clip' in row else []
        return lines + ['show_sample(%d);' % (row['inline'] & 255)]
    if op == 0x1f:
        return ['set_attack_selector(%d);' % int(row['operands'][0], 16)]
    if op == 0x20:
        return ['sound_event_a(%d);' % (row['inline'] & 255)]
    if op in (0x51, 0x52):
        return ['contact_query_%s(%d);' % (row['contact_table'].lower(), row['contact']['selector'])]
    if op == 0x29:
        return ['set_action(ACTION_1006);']
    if op == 0x24:
        return ['release_attack_bookkeeping();']
    if op == 0x34:
        return ['wait_ticks(%d);' % row['inline']]
    if op == 0x37:
        return ['wait_ticks(native_result_bit0() ? %d : %d);' % (row['wait_if_set'], row['wait_if_clear'])]
    return None


def program_c(program, limit):
    """Keep source order and combine only adjacent sample/wait operations."""
    instructions = program['instructions']
    # The decoder sorts its graph by address, so a backwards branch can put
    # another block before the requested entry. Begin at the actual root.
    entry_index = next(index for index, row in enumerate(instructions) if row['va'] == program['entry'])
    visible = instructions[entry_index:entry_index + limit]
    lines, rendered, index, stopped = [], [], 0, False
    while index < len(visible):
        row = visible[index]
        statements = instruction_c(row)
        if statements is None:
            lines.append('// C view stopped: %s (opcode %s). Remaining instructions omitted.' %
                         (row['operation'], row['opcode']))
            stopped = True
            break
        consumed = 1
        if int(row['opcode'], 16) == 0x49 and index + 1 < len(visible):
            following = visible[index + 1]
            next_pc = int(row['va'], 16) + 4 * (1 + len(row['operands']))
            if int(following['opcode'], 16) == 0x34 and int(following['va'], 16) == next_pc:
                statements[-1] = 'show_sample_for_ticks(%d, %d);' % (row['inline'] & 255, following['inline'])
                consumed = 2
        lines.extend(statements)
        rendered.extend(visible[index:index + consumed])
        index += consumed
        if int(row['opcode'], 16) == 0x32:
            break
    if not stopped and len(instructions) > limit:
        lines.append('// C view truncated; continuation omitted. Raise --limit to see more.')
    declarations = ['enum { %s = %d };' % (animation_name(clip), clip)
                    for clip in sorted({row['clip'] for row in rendered if 'clip' in row})]
    if any(int(row['opcode'], 16) == 0x29 for row in rendered):
        declarations.append('enum { ACTION_1006 = 0x1006 };')
    code = declarations + ([''] if declarations else [])
    code += ['void %s(void) {' % script_name(int(program['entry'], 16))]
    code += ['  ' + line for line in lines]
    code += ['}']
    return code, rendered, stopped


def script_c_text(image, entry, limit=80):
    program = script_program(image, entry)
    code, rendered, stopped = program_c(program, limit)
    rows = ['# %s — C-style script' % script_name(entry), '',
            'Pseudocode: waits suspend/resume the script. Helper names describe researched operations;',
            'this is not a compilable C port or a complete account of native behavior.', '',
            '%d decoded instructions; %d shown in the C view%s.' %
            (len(program['instructions']), len(rendered), '; stopped at an unsupported operation' if stopped else ''), '',
            '```c', *code, '```']
    if len(program['instructions']) > limit:
        rows += ['', 'Truncated by --limit; raise it (up to 512) for more decoded instructions.']
    contacts = {(row['contact_table'], row['contact']['selector']): row['contact']
                for row in rendered if 'contact' in row}
    if contacts:
        rows += ['', 'Contact records (dimensions rounded for display):', '']
        for (table, selector), record in contacts.items():
            rectangle = ', '.join('%g' % (value / 65536) for value in record['rectangle_q16'])
            rows.append('- %s[%d]: damage %.4g%%; reach/top/length/height = [%s]; '
                        'reactions hit/air/block = %d/%d/%d; flags = %d.' %
                        (table, selector, record['damage_q16'] * 100 / 65536, rectangle,
                         record['reaction'], record['alternate_reaction'], record['blocked_reaction'], record['flags']))
    rows += reference_rows(int(row['native_target'], 16) for row in rendered if 'native_target' in row)
    rows += ['', 'For addresses, operands and every unresolved boundary, use `--script 0x%x` without `--c`.' % entry]
    return '\n'.join(rows) + '\n'


def move_c_text(image, name, limit=80):
    from scorpion_catalog import MOVES
    if name not in MOVES:
        raise ValueError('Unknown Scorpion move %r; use --list-moves' % name)
    roots = MOVES[name]['scripts']
    if not roots:
        raise ValueError('No script roots catalogued for %s; inspect --move %s without --c' % (name, name))
    rows = ['# Scorpion: %s — C-style scripts' % name, '',
            'Selected script roots only; native move entry, input gates and reactions remain separate research.', '']
    remaining = limit
    for _, entry in roots:
        if remaining <= 0:
            rows.append('Additional script roots omitted by --limit.')
            break
        rows.append(script_c_text(image, entry, remaining))
        remaining -= min(remaining, len(script_program(image, entry)['instructions']))
    return '\n'.join(rows) + '\n'
