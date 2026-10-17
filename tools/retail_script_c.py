"""C-like control flow for every reachable decoded script instruction.

Waits are resumable operations, not blocking C calls. Native calls remain
explicit boundaries; this view does not invent their signatures or compile a VM.
"""
import re

from retail_symbols import native_name, reference_rows
from scorpion_moves import SCRIPT_OPS, script_program, signed_word
from retail_helpers import HELPERS
from character_catalog import moves_for, character_name, CHARACTERS

ANIMATION_NAMES = {67: 'ANIM_SCORPION_LOW_KICK'}
TERMINALS = {0x30, 0x32, 0x3e, 0x3f}
CONDITIONALS = {0x2f, 0x36, 0x44, 0x45, 0x55, 0x3b}


def animation_name(clip):
    return ANIMATION_NAMES.get(clip, 'ANIM_CLIP_%d' % clip)


def script_name(entry, character=0):
    for name, move in moves_for(character).items():
        for index, (role, root) in enumerate(move['scripts']):
            if root == entry:
                suffix = '' if index == 0 else '_' + re.sub(r'[^a-z0-9]+', '_', role.lower()).strip('_')
                return CHARACTERS[character] + '_' + name + suffix
    return 'script_%08x' % entry


def next_pc(row):
    return int(row['va'], 16) + 4 * (1 + len(row.get('operands', [])))


def successors(row):
    """Explicit VM edges; a native tail is terminal only for this script."""
    op = int(row['opcode'], 16)
    if op not in SCRIPT_OPS or op in TERMINALS:
        return []
    if op == 0x33:
        return [int(row['branch'], 16)]
    if op == 0x3c:
        return [int(row['character_table'][row['selected_character']], 16)]
    if op in CONDITIONALS:
        return ([int(row['branch'], 16)] if 'branch' in row else []) + [next_pc(row)]
    return [next_pc(row)]


def instruction_c(row):
    """Render operations; opaque native semantics never hide later VM code."""
    op, inline = int(row['opcode'], 16), row.get('inline', 0)
    arg = inline & 255
    words = [int(value, 16) for value in row.get('operands', [])]
    target = native_name(int(row['native_target'], 16)) if 'native_target' in row else None
    if op not in SCRIPT_OPS:
        return ['unresolved_opcode(%s, %s);' % (row['va'], row['word']), 'return;']
    helper = HELPERS.get(int(row['handler'], 16)) if row.get('handler') else None
    if helper is not None and helper.call:
        arguments = dict(arg=arg, inline=hex(inline), clip=animation_name(row['clip']) if 'clip' in row else 'UNBOUND_CLIP')
        arguments.update({'w%d' % index: signed_word(value) for index, value in enumerate(words)})
        return [helper.call.format(**arguments)]
    if op == 0:
        return ['/* VM no-op */']
    if op in (0x32, 0x35):
        # These handlers do not forward inline as a C argument.
        return (["native_inline_word(%#x);" % inline] if inline else []) + [target + '();'] + (['return;'] if op == 0x32 else [])
    if op == 0x0f:
        return ['open_animation(%s, %d); // %d stored samples' %
                (animation_name(row['clip']), row['start_sample'], row['stored_count'])]
    if op == 0x0e:
        return ['open_victim_animation(%s, 0);' % animation_name(row['clip'])]
    if op in (0x01, 0x02, 0x08, 0x0c, 0x11):
        name = {1: 'play_clip_descriptor', 2: 'open_clip_and_advance', 8: 'play_clip_descriptor_08',
                0x0c: 'bind_submit_and_advance', 0x11: 'bind_clip_and_set_cursor'}[op]
        return ['%s(%s, %#x);' % (name, animation_name(row['clip']), inline)]
    if op in (0x49, 0x47) and (inline >> 8) in (0, 2):
        lines = ['bind_animation(%s);' % animation_name(row['clip'])] if 'clip' in row else []
        return lines + ['%s(%d);' % ('show_sample' if op == 0x49 else 'advance_to_sample', arg)]
    if op in (0x03, 0x07):
        return ['finish_%s();' % ('forwards' if op == 0x03 else 'backwards')]
    if op in (0x04, 0x06):
        return ['advance_%s(%d);' % ('forwards' if op == 0x04 else 'backwards', inline)]
    if op == 0x05:
        return ['advance_to_sample(%d);' % inline]
    if op == 0x10:
        return ['advance_self_and_victim(%d);' % inline]
    if op == 0x12:
        return ['set_animation_cursor(%d);' % inline]
    if op in (0x19, 0x4d):
        return ['%s(%d);' % ('advance_twice_for_count' if op == 0x19 else 'advance_twice_to_sample', inline)]
    if op in (0x20, 0x27, 0x2b):
        return ['sound_event_%s(%d);' % ({0x20: 'a', 0x27: 'c', 0x2b: 'b'}[op], arg)]
    if op == 0x1f:
        return ['set_attack_selector(%d);' % words[0]]
    if op in (0x51, 0x52, 0x5e):
        return ['contact_query_%s(%d);' % (row['contact_table'].lower(), row['contact']['selector'])]
    if op in (0x29, 0x2d):
        return ['set_action(%s);' % ('ACTION_RETAIL_1006' if op == 0x29 else 'ACTION_RETAIL_1007')]
    if op == 0x24:
        return ['release_attack_bookkeeping();']
    if op == 0x34:
        return ['wait_ticks(%d);' % inline]
    if op == 0x37:
        return ['wait_ticks(native_result_bit0() ? %d : %d);' % (row['wait_if_set'], row['wait_if_clear'])]
    if op == 0x3a:
        return ['loop_counter = %d; // task +0x78, shared by this script' % arg]
    if op in (0x33, 0x3c):
        comment = ' // roster script slot %d' % row['selected_character'] if op == 0x3c else ''
        return ['goto L_%08x;%s' % (successors(row)[0], comment)]
    if op in CONDITIONALS:
        prefix = []
        if op in (0x36, 0x55):
            prefix = (['native_argument(%d);' % arg] if op == 0x55 else []) + [target + '();']
        condition = {0x2f: 'fighter_is_airborne()', 0x44: 'down_is_held()', 0x45: 'fast_startup_allowed()',
                     0x36: '(native_result_flags() & 4) != 0', 0x55: '(native_result_flags() & 1) != 0',
                     0x3b: '--loop_counter != 0'}[op]
        branch = ['goto L_%08x;' % int(row['branch'], 16)] if 'branch' in row else ['unresolved_loop_target();', 'return;']
        return prefix + ['if (%s) {' % condition] + ['  ' + line for line in branch] + ['}']
    if op == 0x40:
        return ['spawn_task(%d, %s); // independent continuation; inline byte ignored' % (row['task_type'], native_name(int(row['callback'], 16)))]
    if op == 0x38:
        return ['set_group_callback(%s); // install; do not invoke here' % target]
    if op == 0x39:
        return ['clear_group_callback();']
    if op == 0x22:
        return ['set_task_callback(%s);' % native_name(0x457cb0)]
    if op == 0x30:
        return ['resume_tagged_continuation();', 'return;']
    if op == 0x3e:
        return ['for (;;) {', '  wait_ticks(60);', '}']
    if op == 0x3f:
        return ['fighter_controller_return();', 'return;']
    if op == 0x5a:
        return ['set_horizontal_speed_q16(%d); // negated operand' % row['speed_q16']]
    if op == 0x5c:
        return ['set_gravity_q16(%d);' % signed_word(words[0])]
    if op == 0x6c:
        return ['schedule_horizontal_motion(%d, %d, %d);' % (inline & 0xfff, (inline >> 12) & 0xfff, signed_word(words[0]))]
    if op == 0x63:
        return ['health_subtract_q16(%d);' % signed_word(words[0])]
    if op in (0x2e, 0x26, 0x2a, 0x61, 0x62, 0x6e):
        return ['%s(%s);' % (target, str(arg) if op != 0x61 else '')]
    # All fixed operands remain visible, including optional clip prefixes.
    # The handler address identifies the exact semantics still to adapt.
    arguments = ', '.join([hex(inline), *map(hex, words)])
    return ['vm_helper_%08x(%s); // %s' % (int(row['handler'], 16), arguments, row['operation'])]


def program_c(program, limit, character=0):
    instructions = program['instructions']
    entry = int(program['entry'], 16)
    ordered = sorted(instructions, key=lambda row: int(row['va'], 16))
    # Keep the requested root visible even when it is above a backwards target.
    if len(ordered) > limit:
        root = next(row for row in ordered if int(row['va'], 16) == entry)
        ordered = [root, *(row for row in ordered if row is not root)][:limit]
        ordered.sort(key=lambda row: int(row['va'], 16))
    visible = {int(row['va'], 16): row for row in ordered}
    leaders = set()
    for row in ordered:
        op = int(row['opcode'], 16)
        if op in CONDITIONALS or op in (0x33, 0x3c):
            leaders.update(successors(row))
    if ordered and int(ordered[0]['va'], 16) != entry:
        leaders.add(entry)
    code = ['enum { %s = %d };' % (animation_name(clip), clip)
            for clip in sorted({row['clip'] for row in ordered if 'clip' in row})]
    if any(int(row['opcode'], 16) in (0x29, 0x2d) for row in ordered):
        code.append('// ACTION_RETAIL_* comes from game/retail_actions.inc; research with --action CODE.')
    if code:
        code.append('')
    code.append('void %s(void) {' % script_name(entry, character))
    if any(int(row['opcode'], 16) in (0x3a, 0x3b) for row in ordered):
        code.append('  uint32_t loop_counter = task_loop_counter(); // retain across waits; one counter, not nested C scopes')
    if ordered and int(ordered[0]['va'], 16) != entry:
        code.append('  goto L_%08x;' % entry)
    rendered, index, boundaries = [], 0, set()
    while index < len(ordered):
        row = ordered[index]
        pc, op = int(row['va'], 16), int(row['opcode'], 16)
        if pc in leaders:
            code.append('L_%08x:' % pc)
        statements, consumed = instruction_c(row), 1
        # Do not combine across an incoming branch or a discontinuity.
        if op == 0x49 and index + 1 < len(ordered):
            following = ordered[index + 1]
            if int(following['opcode'], 16) == 0x34 and int(following['va'], 16) == next_pc(row) and next_pc(row) not in leaders:
                statements[-1] = 'show_sample_for_ticks(%d, %d);' % (row['inline'] & 255, following['inline'])
                consumed = 2
        code.extend('  ' + line for line in statements)
        rendered.extend(ordered[index:index + consumed])
        last = ordered[index + consumed - 1]
        edges = successors(last)
        next_visible = int(ordered[index + consumed]['va'], 16) if index + consumed < len(ordered) else None
        # Conditional and sequential fallthrough must never enter an unrelated
        # block just because the decoder sorted it next in address order.
        if edges and op not in (0x33, 0x3c):
            fallthrough = next_pc(last)
            if fallthrough != next_visible:
                code.append('  goto L_%08x;' % fallthrough)
                leaders.add(fallthrough)
                boundaries.add(fallthrough)
        boundaries.update(target for target in edges if target not in visible)
        index += consumed
    for target in sorted(boundaries - visible.keys()):
        code.extend(['L_%08x:' % target, '  omitted_script_continuation(%#x);' % target, '  return;'])
    code.append('}')
    if len(rendered) < len(instructions):
        code.append('// C view truncated by --limit; continuation omitted, with explicit exits above.')
    return code, rendered, any(int(row['opcode'], 16) not in SCRIPT_OPS for row in rendered)


def script_dependencies(instructions):
    dependencies = {int(row[key], 16) for row in instructions for key in ('native_target', 'callback') if row.get(key)}
    for row in instructions:
        if row.get('handler'):
            handler = int(row['handler'], 16)
            if handler in HELPERS or any('vm_helper_' in line for line in instruction_c(row)):
                dependencies.add(handler)
    return dependencies


def script_c_text(image, entry, limit=512, character=0, details=True):
    program = script_program(image, entry, character)
    code, rendered, unknown = program_c(program, limit, character)
    rows = ['# %s — C-style script' % script_name(entry, character), '',
            'Pseudocode: waits suspend/resume; native calls may also suspend. Translate them into engine continuations.',
            'This is not a compilable C port. All decoded paths for the selected roster are shown unless limited;',
            'unresolved native semantics are explicit helper calls, not omitted instructions.', '',
            '%d decoded instructions; %d shown in the C view%s.' %
            (len(program['instructions']), len(rendered), '; unresolved opcode paths terminate explicitly' if unknown else ''), '',
            '```c', *code, '```']
    if details:
        contacts = {(row['contact_table'], row['contact']['selector']): row['contact'] for row in rendered if 'contact' in row}
        if contacts:
            rows += ['', 'Contact records (dimensions rounded for display):', '']
            for (table, selector), record in contacts.items():
                rectangle = ', '.join('%g' % (value / 65536) for value in record['rectangle_q16'])
                rows.append('- %s[%d]: damage %.4g%%; reach/top/length/height = [%s]; reactions hit/air/block = %d/%d/%d; flags = %d.' %
                            (table, selector, record['damage_q16'] * 100 / 65536, rectangle,
                             record['reaction'], record['alternate_reaction'], record['blocked_reaction'], record['flags']))
        dependencies = script_dependencies(rendered)
        from retail_helper_view import helper_rows
        rows += helper_rows(dependencies, image, limit=limit)
        rows += reference_rows(dependencies)
        rows += ['', 'For source addresses and fixed operands, use `--script 0x%x` without `--c`.' % entry]
        if character:
            rows.append('Keep `--character %s` on follow-up script queries.' % CHARACTERS[character])
    return '\n'.join(rows) + '\n'


def move_c_text(image, name, limit=512, character=0):
    from retail_query import command_gate_rows, CONTACT_TABLES, attack_text
    from retail_helper_view import helper_rows, port_source, ROOT
    from scorpion_moves import attack_record, attack_record_at
    moves = moves_for(character)
    if name not in moves:
        raise ValueError('Unknown %s move %r; use --list-moves' % (character_name(character), name))
    move = moves[name]
    rows = ['# %s: %s — C-style scripts' % (character_name(character), name), '',
            'All decoded paths for this roster; native contracts and engine adaptations are separate from script decompilation.',
            '', 'Entry: `--native %08x --c`' % move['entry'], '', '## Input and gates', '']
    gates, predicates = command_gate_rows(image, move['entry'], character)
    rows += gates
    dependencies = {move['entry'], *predicates, *(address for _, address in move['native'])}
    contacts = {}
    for table, selector in move['contacts']:
        record = attack_record_at(image, selector) if table == 'record' else attack_record(image, CONTACT_TABLES[table], selector)
        contacts[record['va']] = record
    remaining = limit
    for _, entry in move['scripts']:
        if remaining <= 0:
            rows.append('Additional script roots omitted by --limit.')
            break
        program = script_program(image, entry, character)
        _, rendered, _ = program_c(program, remaining, character)
        rows.append(script_c_text(image, entry, remaining, character, details=False))
        dependencies.update(script_dependencies(rendered))
        for row in rendered:
            if 'contact' in row:
                contacts[row['contact']['va']] = row['contact']
        remaining -= len(rendered)
    if not move['scripts']:
        rows += ['', '## Native move — existing portable C adaptations', '',
                 'This move is native, not an empty VM script. Bodies below are read from our C implementation;',
                 'they are reuse examples, not automatically decompiled x86. Contracts identify native timing and branches.']
        selected = [move['entry'], *(address for _, address in move['native'])]
        ports = set()
        for address in selected:
            helper = HELPERS.get(address)
            if helper is None or not helper.port or helper.port in ports:
                continue
            ports.add(helper.port)
            line, code = port_source(helper.port)
            rows += ['', '[%s:%d](%s:%d)' % (helper.port[0], line, ROOT / helper.port[0], line)]
            if len(code.splitlines()) <= remaining:
                rows += ['', '```c', code, '```']
                remaining -= len(code.splitlines())
            else:
                rows += ['', 'C definition omitted by --limit; follow the source link for its complete body.']
        if not ports:
            rows += ['', 'No portable C adapter is recorded. Follow the explicit native boundaries below.']
    rows += ['', '## Contacts and reactions', '']
    for record in contacts.values():
        rows += ['- ' + attack_text(record)]
        indices = sorted({record[key] for key in ('reaction', 'alternate_reaction', 'blocked_reaction')})
        for reaction in indices:
            target = image.word(0x4ea4e8 + reaction * 4)
            dependencies.add(target)
            rows.append('  Reaction %d -> `%08x` %s.' % (reaction, target, native_name(target)))
    if not contacts:
        rows.append('No contact records catalogued or decoded inline; helper contracts describe native effects.')
    if move['buttons']:
        rows += ['', '## Follow-up buttons', '', 'HP, LP, BLOCK, HK, LK, RUN; zero disables a button. Native handlers retain their own gates.']
        for role, table in move['buttons']:
            targets = [image.word(table + index * 4) for index in range(6)]
            rows += ['', '- %s at `%08x`: %s.' % (role, table, ', '.join('%08x %s' % (target, native_name(target)) if target else '0' for target in targets))]
            dependencies.update(target for target in targets if target)
    rows += helper_rows(dependencies, image, include_related=True, limit=limit)
    rows += reference_rows(dependencies)
    return '\n'.join(rows) + '\n'
