"""Deterministic, fail-closed lowering of decoded retail scripts to C11 fibers.

No native code execution, runtime opcode dispatch, or text-to-text translation.
Every reachable instruction is lowered from the existing decoder's graph;
unsupported instructions/edges fail before any output is written.
"""
import re
from functools import lru_cache
import os
from pathlib import Path
import shutil
import subprocess

from retail_moves import EXE_SHA256
from character_catalog import CHARACTERS
from scorpion_moves import script_program, signed_word
from retail_script_c import successors
from retail_bindings import bindings_for
from retail_c_postprocess import postprocess


class CompileError(ValueError):
    def __init__(self, diagnostics):
        self.diagnostics = sorted(diagnostics, key=lambda row: (row['pc'], row['reason']))
        super().__init__('\n'.join('%s: %s' % (row['pc'], row['reason']) for row in self.diagnostics))


# Fully understood operations. All remaining decoded operations require an
# explicit handler binding; a readable pseudocode name is not a lowering.
CORE_OPS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x0c, 0x0d, 0x0e, 0x10, 0x60, 0x0f, 0x11, 0x12, 0x19, 0x1d, 0x1f,
            0x20, 0x27, 0x2b, 0x29, 0x2d, 0x2e, 0x2f, 0x30, 0x32, 0x33,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
            0x3e, 0x3f, 0x40, 0x42, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4d,
            0x4e, 0x51, 0x52, 0x53, 0x55, 0x58, 0x5a, 0x5b, 0x5c, 0x5e, 0x61, 0x68, 0x69, 0x71}


def symbol(entry):
    return 'retail_script_%08x' % entry


def edges(row, bindings):
    key = instruction_binding(row)
    binding = bindings.get(key[0]) if key else None
    return [binding.script_target] if binding and binding.script_target else successors(row)


def instruction_binding(row):
    op = int(row['opcode'], 16)
    role = {0x32: 'tail', 0x35: 'call', 0x36: 'predicate', 0x55: 'predicate',
            0x38: 'physics', 0x40: 'spawn'}.get(op)
    if role:
        return int(row.get('callback', row.get('native_target', '0')), 16), role
    if op not in CORE_OPS:
        return int(row['handler'], 16) if row.get('handler') else 0, 'handler'
    return None


def expression(binding, row):
    words = [signed_word(int(word, 16)) for word in row.get('operands', [])]
    values = {'fighter': 'fighter', 'arg_u8': str(row.get('inline', 0) & 255),
              'task_type': str(row.get('task_type', 0)), 'inline_lo_u12': str(row.get('inline', 0) & 4095), 'inline_hi_u12': str(row.get('inline', 0) >> 12 & 4095), 'selector_u32': 'attack_selector'}
    values.update({'word%d_i32' % i: str(word) for i, word in enumerate(words)})
    try:
        return '%s(%s)' % (binding.symbol, ', '.join(values[arg] for arg in binding.arguments))
    except KeyError as error:
        raise ValueError('binding argument %s is absent from instruction' % error.args[0]) from None


def reads_defines(row, bindings):
    op = int(row['opcode'], 16)
    reads = {'flags'} if op in (0x37, 0x42) else {'loop'} if op == 0x3b else set()
    if op == 0x69 or (op == 0x68 and 'clip' not in row):
        reads.add('clip')
    defines = {'flags'} if op in (0x36, 0x55, 0x51, 0x52, 0x53, 0x5e) else {'loop'} if op == 0x3a else set()
    if 'clip' in row and row.get('clip_owner') != 'victim':
        defines.add('clip')
    elif op in (3, 4, 5, 6, 7, 0x0d, 0x12, 0x19, 0x1d, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4d, 8, 0x10):
        reads.add('clip')
    if op == 0x71:
        reads.add('clip')
    if op == 0x1f:
        defines.add('selector')
    key = instruction_binding(row)
    binding = bindings.get(key[0]) if key else None
    if binding:
        if binding.result == 'flags':
            defines.add('flags')
        if binding.cursor_effect == 'finish':
            reads.add('clip')
        if 'selector_u32' in binding.arguments:
            reads.add('selector')
    return reads, defines


def validate(program, bindings, return_to, context=None):
    context = context or {}
    allowed = {"clip", "count", "sample", "evidence", "return_to", "paired", "export_frames", "successor"}
    if "export_frames" in context and not isinstance(context["export_frames"], bool):
        raise ValueError("Frame export selection must be boolean")
    if "successor" in context and (context.get("return_to") != "caller" or not re.fullmatch(r'retail_script_[0-9a-f]{8}', str(context["successor"]))):
        raise ValueError("A frame successor continues a caller-returning root into a generated phase")
    if context.keys() & {'count', 'sample'} and 'clip' not in context:
        raise ValueError('Entry cursor requires a clip contract')
    if context.keys() - allowed or (context and not context.get("evidence")):
        raise ValueError("Entry context requires evidence and known fields")
    if "paired" in context and not isinstance(context["paired"], bool):
        raise ValueError("Paired entry context must be boolean")
    initial = {"clip"} if "clip" in context else set()
    inherited_clip = context.get("clip") == "current"
    if initial and ((not inherited_clip and (not isinstance(context.get("clip"), int) or context['clip'] < 0)) or
                    not isinstance(context.get("count"), int) or context["count"] <= 0 or
                    not isinstance(context.get('sample'), int) or not 0 <= context["sample"] < context["count"]):
        raise ValueError("Invalid entry clip context")
    rows = {int(row['va'], 16): row for row in program['instructions']}
    entry = int(program['entry'], 16)
    errors = []
    def error(pc, reason):
        errors.append(dict(pc='0x%08x' % pc, reason=reason))
    occupied = {}
    for pc, row in rows.items():
        op = int(row['opcode'], 16)
        if 'unresearched' in row['operation']:
            key = instruction_binding(row)
            binding = bindings.get(key[0]) if key and key[1] == 'handler' else None
            if binding is None:
                error(pc, 'unsupported opcode %#x' % op)
        for word in range(pc, pc + 4 * (1 + len(row.get('operands', []))), 4):
            if word in occupied and occupied[word] != pc:
                owner = rows[occupied[word]]
                # Retail air selectors deliberately execute their final zero
                # table word as a no-op. It is the selected target itself,
                # not an arbitrary branch into an operand.
                selected_zero = (int(owner['opcode'], 16) == 0x3c and word == pc and int(row['opcode'], 16) == 0 and
                                 int(owner['character_table'][owner['selected_character']], 16) == pc)
                if not selected_zero:
                    error(pc, 'instruction overlaps operand of 0x%08x' % occupied[word])
            occupied[word] = pc
        for target in edges(row, bindings):
            if target not in rows:
                error(pc, 'missing script edge 0x%08x' % target)
        if op == 0x3b and 'branch' not in row:
            error(pc, 'unresolved loop target')
        if op == 0x3a and row['inline'] & 255 == 0:
            error(pc, 'zero loop count would underflow')
        if op in (2, 8, 0x19) and row['inline'] > 255:
            error(pc, 'unsupported counted-playback inline bits')
        if op in (0x30, 0x3f) and return_to is None:
            error(pc, 'controller/tagged return needs an explicit engine continuation')
        if op in (0x46, 0x47, 0x48, 0x49, 0x68) and row['inline'] >> 8 not in (0, 2):
            error(pc, 'unsupported playback prefix (only absent/descriptor prefixes are typed)')
        if 'clip' in row:
            first = row.get('start_sample', row['inline'] if op == 8 else 0)
            if row['stored_count'] <= 0 or first >= row['stored_count']:
                error(pc, 'animation cursor outside stored clip')
        key = instruction_binding(row)
        if key:
            address, role = key
            binding = bindings.get(address)
            if binding is None:
                error(pc, 'unbound %s 0x%08x' % (role, address))
            elif role not in binding.roles:
                error(pc, 'binding 0x%08x cannot be used as %s' % (address, role))
            else:
                if binding.inline_values and row['inline'] & 255 not in binding.inline_values:
                    error(pc, 'binding does not accept inline selector %d' % (row['inline'] & 255))
                if role == 'spawn' and row['task_type'] not in binding.task_types:
                    error(pc, 'spawn binding does not accept task type %d' % row['task_type'])
                try:
                    expression(binding, row)
                except ValueError as exc:
                    error(pc, str(exc))
    # Definite initialization over all incoming paths. No invented scratch
    # values for scripts entered midway through a native continuation.
    incoming = {pc: {'flags', 'loop', 'clip', 'selector'} for pc in rows}
    incoming[entry] = initial.copy()
    predecessors = {pc: [] for pc in rows}
    for pc, row in rows.items():
        for target in edges(row, bindings):
            if target in rows:
                predecessors[target].append(pc)
    changed = True
    while changed:
        changed = False
        for pc in sorted(rows):
            candidates = [incoming[p] | reads_defines(rows[p], bindings)[1] for p in predecessors[pc]]
            if pc == entry:
                candidates.append(initial.copy())
            value = set.intersection(*candidates) if candidates else set()
            if value != incoming[pc]:
                incoming[pc] = value
                changed = True
    for pc, row in rows.items():
        for name in reads_defines(row, bindings)[0] - incoming[pc]:
            error(pc, '%s is not initialized on every incoming path' % name)
    # All possible incoming clip lengths matter at merges. A cursor valid
    # on one branch must not silently index past another branch's clip.
    clip_in = {pc: set() for pc in rows}
    changed = True
    while changed:
        changed = False
        for pc in sorted(rows):
            values = {context.get('count')} if pc == entry else set()
            for predecessor in predecessors[pc]:
                row = rows[predecessor]
                values |= {row['stored_count']} if 'clip' in row and row.get('clip_owner') != 'victim' else clip_in[predecessor]
            if values != clip_in[pc]:
                clip_in[pc] = values
                changed = True
    for pc, row in rows.items():
        op = int(row['opcode'], 16)
        if op in (5, 0x12, 0x1d, 0x47, 0x49, 0x4a, 0x4d):
            lengths = {row['stored_count']} if 'clip' in row else clip_in[pc] - {None}
            target = row['inline'] & 255 if op in (0x12, 0x1d, 0x47, 0x49, 0x4a, 0x4d) else row['inline']
            # 429990 permits the end cursor equal to stored_count without
            # submitting a pose there. Only forward playback may reach it;
            # direct pose selection and reverse targets remain strict.
            endpoint = op in (5, 0x47, 0x4d)
            if any(target > count or (target == count and not endpoint) for count in lengths):
                error(pc, 'target sample %d is outside an incoming clip' % target)
    # Delete guaranteed-yield instructions, then reject cycles in the
    # remaining graph. A cycle that can bypass a wait is unsafe too.
    def yields(row):
        op = int(row['opcode'], 16)
        return (op in (2, 4, 6, 0x19, 0x10) and row['inline'] > 0) or op in (1, 3, 7, 8, 9, 0x46, 0x48, 0x3e, 0x68, 0x69, 0x71) or (op == 0x34 and row['inline'] > 0) or (
            op == 0x37 and min(row['wait_if_set'], row['wait_if_clear']) > 0)
    cursors = {pc: set() for pc in rows}
    def cursor_after(row, values):
        op, inline = int(row['opcode'], 16), row['inline']
        if 'clip' in row and row.get('clip_owner') != 'victim':
            first = row.get('start_sample', inline if op == 8 else 0)
            if op == 9:
                first = row['stored_count'] - 1
            values = {(row['stored_count'], first)}
        if op in (0x12, 0x1d, 0x49):
            return {(n, inline & 255) for n, _ in values}
        if op == 0x0d:
            return {(n, min(n, c + 1)) for n, c in values}
        if op in (5, 0x47, 0x4a, 0x4d):
            target = inline & 255 if op in (0x47, 0x4a, 0x4d) else inline
            return {(n, max(c, target) if op != 0x4a else min(c, target)) for n, c in values}
        key = instruction_binding(row)
        binding = bindings.get(key[0]) if key else None
        if binding and binding.cursor_effect == 'unknown':
            return {(n, cursor) for n, _ in values for cursor in range(n)}
        if op in (1, 3, 7, 8, 9, 0x48, 0x68, 0x69) or (binding and binding.cursor_effect == 'finish'):
            return {(n, 0 if op in (7, 9) else n - 1) for n, _ in values}
        if op == 0x46:
            return {(n, min(n - 1, c + (inline & 255)) if inline & 255 else n - 1) for n, c in values}
        if op in (2, 4, 6, 0x19):
            delta = -(inline & 255) if op == 6 else 2 * ((inline + 1) // 2) if op == 0x19 else inline & 255
            return {(n, max(0, min(n - 1, c + delta))) for n, c in values}
        return values
    changed = True
    while changed:
        changed = False
        for pc in sorted(rows):
            values = set()
            if pc == entry and 'count' in context:
                values.add((context['count'], context.get('sample', 0)))
            for predecessor in predecessors[pc]:
                values |= cursor_after(rows[predecessor], cursors[predecessor])
            if values != cursors[pc]:
                cursors[pc] = values
                changed = True
    guaranteed = set()
    for pc, row in rows.items():
        op = int(row['opcode'], 16)
        if op == 0x0d and any(c + 1 >= n for n, c in cursors[pc]):
            error(pc, 'next pose exceeds bound clip')
        if op in (5, 0x47, 0x4a, 0x4d):
            values = cursors[pc]
            if 'clip' in row:
                values = {(row['stored_count'], row.get('start_sample', 0))}
            target = row['inline'] & 255 if op in (0x47, 0x4a, 0x4d) else row['inline']
            if values and all(c > target if op == 0x4a else c < target for _, c in values):
                guaranteed.add(pc)
    active, done = set(), set()
    def visit(pc):
        if pc in active:
            error(pc, 'control-flow cycle can run without a positive wait')
            return
        if pc in done or pc not in rows or pc in guaranteed or yields(rows[pc]):
            return
        active.add(pc)
        for target in edges(rows[pc], bindings):
            visit(target)
        active.remove(pc)
        done.add(pc)
    for pc in sorted(rows):
        visit(pc)
    if errors:
        raise CompileError(errors)


def lower(row, bindings, return_to):
    op, inline = int(row['opcode'], 16), row.get('inline', 0)
    arg = inline & 255
    words = [signed_word(int(word, 16)) for word in row.get('operands', [])]
    key = instruction_binding(row)
    if key:
        address, role = key
        binding = bindings[address]
        if binding.script_target:
            return ['goto L_%08x;' % binding.script_target]
        call = expression(binding, row)
        if role == 'physics':
            return ['fighter->body.physics = %s;' % binding.symbol]
        if role == 'predicate':
            bit = 4 if op == 0x36 else 1
            return ['result_flags = %s;' % call, 'if (result_flags & %du) {' % bit,
                    '  goto L_%08x;' % int(row['branch'], 16), '}']
        terminal_handler = role == 'handler' and 'unresearched' in row['operation']
        return [('result_flags = ' if binding.result == 'flags' else '') + call + ';'] + (['return;'] if role == 'tail' or terminal_handler else [])
    lines = []
    if 'clip' in row and op != 0x71:
        first = row.get('start_sample', inline if op == 8 else 0)
        if op == 9:
            first = row['stored_count'] - 1
        lines.append('%s(fighter, %d, %d, %d);' % ('mk4_script_bind_victim' if op == 0x0e else 'mk4_script_bind', row['clip'], row['stored_count'], first))
    if op == 0:
        lines.append('(void)fighter; /* decoded VM no-op */')
    elif op in (0x0f, 0x12, 0x49, 0x1d):
        if op != 0x0f:
            lines.append('fighter->sample_override = %d;' % arg)
    elif op == 0x46:
        lines.append('mk4_script_advance_slow(fighter, %d);' % arg)
    elif op in (0x68, 0x69):
        if op == 0x68:
            lines.append('fighter->sample_override = 0;')
        lines.append('mk4_script_finish_clip(fighter, 0);')
    elif op in (1, 3, 8, 0x48):
        lines.append('mk4_script_finish_clip(fighter, 1);')
    elif op in (7, 9):
        lines.append('mk4_script_finish_clip(fighter, -1);')
    elif op == 0x0d:
        lines.append('fighter->sample_override = fighter_sample(fighter) + 1;')
    elif op in (2, 4, 6, 0x19):
        lines.append('mk4_script_advance(fighter, %d, %d);' % ((inline + 1) // 2 if op == 0x19 else arg if op == 2 else inline, -1 if op == 6 else 2 if op == 0x19 else 1))
    elif op in (0x0e, 0x11):
        pass
    elif op == 0x4e:
        lines.append('(void)fighter; /* explicit Fighter argument owns reaction context */')
    elif op == 0x42:
        lines += ['if (result_flags & 1u) {', '  goto L_%08x;' % int(row['branch'], 16), '}']
    elif op == 0x10:
        lines.append('mk4_script_pair_advance(fighter, %d);' % arg)
    elif op == 0x0c:
        lines.append('mk4_script_advance(fighter, 1, 1);')
    elif op in (5, 0x47, 0x4a, 0x4d):
        lines.append('fighter_fiber_play_to(fighter, %d, %d);' % (arg if op in (0x47, 0x4a, 0x4d) else inline, -1 if op == 0x4a else 2 if op == 0x4d else 1))
    elif op == 0x1f:
        lines.append('attack_selector = %d;' % words[0])
    elif op in (0x20, 0x27, 0x2b):
        lines.append('battle_sound(SOUND_%s, %d);' % ({0x20: 'A', 0x27: 'C', 0x2b: 'B'}[op], arg))
    elif op in (0x29, 0x2d):
        lines.append('fighter->action = ACTION_RETAIL_%s;' % ('1006' if op == 0x29 else '1007'))
    elif op == 0x2e:
        lines.append('fighter->flags |= FF_NO_GUARD;')
    elif op == 0x61:
        lines.append('fighter->flags &= ~(unsigned)FF_BUSY;')
    elif op in (0x30, 0x3f):
        if op == 0x3f:
            lines.append('fighter_wait_ticks(1);')
        lines.append('return;' if return_to == 'caller' else 'fighter_fiber_finish(%s);' % return_to)
    elif op in (0x33, 0x3c):
        lines.append('goto L_%08x;' % successors(row)[0])
    elif op == 0x34:
        if inline:
            lines.append('fighter_wait_ticks(%d);' % inline)
        else:
            lines.append('(void)fighter; /* zero-duration wait */')
    elif op == 0x37:
        lines += ['int wait_ticks = result_flags & 1u ? %d : %d;' % (row['wait_if_set'], row['wait_if_clear']),
                  'if (wait_ticks > 0) {', '  fighter_wait_ticks(wait_ticks);', '}']
    elif op == 0x39:
        lines.append('fighter->body.physics = NULL;')
    elif op == 0x3a:
        lines.append('loop_counter = %du;' % arg)
    elif op == 0x3b:
        lines += ['if (--loop_counter != 0) {', '  goto L_%08x;' % int(row['branch'], 16), '}']
    elif op == 0x3e:
        lines += ['for (;;) {', '  fighter_wait_ticks(60);', '}']
    elif op in (0x2f, 0x44, 0x45):
        condition = {0x2f: 'fighter_airborne(fighter)', 0x44: 'fighter->held & BUTTON_BIT(BUTTON_DOWN)', 0x45: 'fighter_fast_start(fighter)'}[op]
        lines += ['if (%s) {' % condition, '  goto L_%08x;' % int(row['branch'], 16), '}']
    elif op in (0x51, 0x52, 0x53, 0x5e):
        lines.append('result_flags = battle_contact(fighter, &retail_contact_%s) != CONTACT_MISS ? 1u : 0u;' % row['contact']['va'][2:])
    elif op == 0x58:
        lines += ['fighter_set_speed(fighter, %d);' % words[0], 'fighter->body.vel[AXIS_Y] = %d;' % words[1],
                  'fighter->body.gravity = %d;' % words[2]]
    elif op == 0x5a:
        lines.append('fighter_set_speed(fighter, %d);' % row['speed_q16'])
    elif op == 0x5c:
        lines.append('fighter->body.gravity = %d;' % words[0])
    elif op == 0x5b:
        lines.append('mk4_script_stop(fighter);')
    elif op == 0x60:
        lines.append('while (fighter_airborne(fighter)) {\n  fighter_wait_ticks(1);\n}')
    elif op == 0x71:
        lines.append('mk4_script_blend_bind(fighter, %d, %d);' % (row['clip'], row['stored_count']))
    else:
        raise AssertionError('Missing lowering for %#x' % op)
    return lines


@lru_cache(maxsize=1)
def shared_fiber_only_continuations():
    # Completion callbacks run after the owning fiber has been retired, and
    # frame completions have no coroutine. Follow shared helper calls so a
    # wrapper cannot accidentally re-enter a yielding/finish boundary.
    from c_reachability import definitions
    source = (Path(__file__).resolve().parents[1] / 'game/script.c').read_text()
    functions = {name: body for name, _, body in definitions(source)}
    forbidden = {'fighter_fiber_finish', 'fighter_wait_ticks', 'fighter_fiber_move',
                 'fighter_fiber_play_to', 'fighter_fiber_sequence', 'fighter_fiber_await_frames',
                 'native_wait', 'native_delay', 'native_calls', 'native_play_to', 'native_until'}
    unsafe = set()
    changed = True
    while changed:
        changed = False
        for name, body in functions.items():
            calls = set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', body)) - {name}
            if name not in unsafe and calls & (forbidden | unsafe):
                unsafe.add(name)
                changed = True
    return unsafe


def compile_program(image, entry, character=0, bindings=None, return_to=None, context=None):
    if return_to is not None and not re.fullmatch(r'[A-Za-z_]\w*', return_to):
        raise ValueError('Invalid continuation symbol')
    if return_to in shared_fiber_only_continuations():
        raise CompileError([{'pc': hex(entry), 'reason': 'completion callback requires a running fiber: ' + return_to}])
    bindings = bindings_for(character) if bindings is None else bindings
    program = script_program(image, entry, character)
    rows = {row['va']: row for row in program['instructions']}
    pending = list(rows.values())
    while pending:
        row = pending.pop()
        key = instruction_binding(row)
        binding = bindings.get(key[0]) if key else None
        if binding and binding.script_target and hex(binding.script_target) not in rows:
            for linked in script_program(image, binding.script_target, character)['instructions']:
                if linked['va'] not in rows:
                    rows[linked['va']] = linked
                    pending.append(linked)
    program['instructions'] = sorted(rows.values(), key=lambda row: int(row['va'], 16))
    validate(program, bindings, return_to, context)
    return program, bindings


def emit_program(program, bindings, return_to=None, context=None):
    context = context or {}
    rows = program['instructions']
    entry = int(program['entry'], 16)
    ops = {int(row['opcode'], 16) for row in rows}
    lines = ['static inline void %s(Fighter *fighter) {' % symbol(entry)]
    if ops & {0x36, 0x55, 0x51, 0x52, 0x53, 0x5e, 0x37, 0x42} or any(binding.result == 'flags' for binding in bindings.values()):
        lines.append('  unsigned result_flags = 0;')
        lines.append('  (void)result_flags;')
    if ops & {0x3a, 0x3b}:
        lines.append('  uint32_t loop_counter = 0;')
        lines.append('  (void)loop_counter;')
    if 0x1f in ops:
        lines += ['  int attack_selector = 0;', '  (void)attack_selector;']
    if 'clip' in context:
        if context['clip'] == 'current':
            lines.append('  mk4_script_inherit_current(fighter, %d, %d);' % (context['count'], context['sample']))
        else:
            lines.append('  mk4_script_inherit(fighter, %d, %d, %d);' % (context['clip'], context['count'], context.get('sample', 0)))
    lines.append('  goto L_%08x;' % entry)
    for row in rows:
        pc = int(row['va'], 16)
        raw = ' '.join([row['word'], *row.get('operands', [])])
        lines += ['L_%08x: {' % pc, '  /* EXE %08x: %s */' % (pc, raw)]
        for statement in lower(row, bindings, return_to):
            lines.extend('  ' + part for part in statement.splitlines())
        op = int(row['opcode'], 16)
        key = instruction_binding(row)
        binding = bindings.get(key[0]) if key else None
        terminal_handler = key is not None and key[1] == 'handler' and binding is not None and 'unresearched' in row['operation']
        if op not in (0x30, 0x32, 0x33, 0x3c, 0x3e, 0x3f) and not terminal_handler:
            # Explicit fallthrough protects against address ordering and gaps.
            following = pc + 4 * (1 + len(row.get('operands', [])))
            lines.append('  goto L_%08x;' % following)
        lines.append('}')
    lines.append('}')
    return '\n'.join(lines)


def generate(image, entries, character=0, return_to=None, bindings=None, contexts=None, mmove_entries=(), mmove_only=False):
    from retail_mmove import emit_mmove
    entries = set(entries)
    mmove_entries = {int(entry, 16) if isinstance(entry, str) else entry for entry in mmove_entries}
    if mmove_entries - set(entries):
        raise ValueError("mmove roots must belong to generated_scripts")
    if mmove_only and mmove_entries != entries:
        raise ValueError("mmove-only profile requires every generated root to use phases")
    contexts = contexts or {}
    programs = [compile_program(image, entry, character, bindings, contexts.get(hex(entry), {}).get('return_to', return_to), contexts.get(hex(entry)))
                for entry in sorted(set(entries))]
    lines = ['/* Generated by tools/compile_retail.py. Do not edit. */',
             '/* EXE SHA-256: %s; roster %d. */' % (EXE_SHA256, character), '#include "script.h"', '']
    local = {}
    for program, table in programs:
        for row in program['instructions']:
            key = instruction_binding(row)
            binding = table.get(key[0]) if key else None
            if binding and binding.source.startswith('game/' + CHARACTERS[character] + '/') and not binding.script_target:
                local[binding.symbol] = binding
    for name, binding in sorted(local.items()):
        types = ['Fighter *' if arg == 'fighter' else 'int' for arg in binding.arguments]
        lines.append('%s %s(%s);' % ('unsigned' if binding.result == 'flags' else 'void', name, ', '.join(types) or 'void'))
    contacts = {row['contact']['va']: row['contact'] for program, _ in programs for row in program['instructions'] if 'contact' in row}
    for address, record in sorted(contacts.items()):
        values = [*record['rectangle_q16'], record['reaction'], record['alternate_reaction'], record['blocked_reaction'], record['flags'], record['damage_q16']]
        lines.append('static const AttackRecord retail_contact_%s = {%s};' % (address[2:], ', '.join(map(str, values))))
    for program, binding in programs:
        emitter = emit_mmove if int(program['entry'], 16) in mmove_entries else emit_program
        context = contexts.get(program['entry'], {})
        if context.get('export_frames') and emitter != emit_mmove:
            exported = emit_mmove(program, binding, context.get('return_to', return_to), context)
            root = symbol(int(program['entry'], 16))
            lines += ['', '/* Reusable generated phase for native frame composition. */', exported.replace(root, root + '_repeat')]

        lines += ['', emitter(program, binding, contexts.get(program['entry'], {}).get('return_to', return_to), contexts.get(program['entry']))]
    return postprocess('\n'.join(lines) + '\n')


def format_c(source):
    """Canonical repository formatting; generation/check use the same path."""
    root = Path(__file__).resolve().parents[1]
    formatter = os.environ.get('CLANG_FORMAT') or shutil.which('clang-format')
    if formatter is None:
        result = subprocess.run(['xcrun', '--find', 'clang-format'], capture_output=True, text=True)
        formatter = result.stdout.strip() if result.returncode == 0 else None
    if not formatter:
        raise ValueError('clang-format is required for canonical generated C')
    command = [formatter, '--style=file', '--assume-filename=' + str(root / 'game/generated.inc')]
    # clang-format can move a trailing comment out of an empty labeled block
    # on its first pass and then reflow that block on its second pass. Generated
    # output must match the repository's in-place formatter, so use its stable
    # result rather than a one-pass intermediate.
    formatted = subprocess.run(command, input=source, text=True, capture_output=True, check=True).stdout
    return subprocess.run(command, input=formatted, text=True, capture_output=True, check=True).stdout
