"""Deterministic, fail-closed lowering of decoded retail scripts to C11 fibers.

No native code execution, runtime opcode dispatch, or text-to-text translation.
Every reachable instruction is lowered from the existing decoder's graph;
unsupported instructions/edges fail before any output is written.
"""
import re

from retail_moves import EXE_SHA256
from scorpion_moves import script_program, signed_word
from retail_script_c import successors
from retail_bindings import bindings_for


class CompileError(ValueError):
    def __init__(self, diagnostics):
        self.diagnostics = sorted(diagnostics, key=lambda row: (row['pc'], row['reason']))
        super().__init__('\n'.join('%s: %s' % (row['pc'], row['reason']) for row in self.diagnostics))


# Fully understood operations. All remaining decoded operations require an
# explicit handler binding; a readable pseudocode name is not a lowering.
CORE_OPS = {0, 1, 2, 3, 4, 5, 6, 7, 9, 0x0f, 0x12, 0x19, 0x1d, 0x1f,
            0x20, 0x27, 0x2b, 0x29, 0x2d, 0x2e, 0x2f, 0x30, 0x32, 0x33,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
            0x3e, 0x3f, 0x40, 0x44, 0x45, 0x47, 0x48, 0x49, 0x4a, 0x4d,
            0x51, 0x52, 0x55, 0x58, 0x5a, 0x5b, 0x5c, 0x5e, 0x61}


def symbol(entry):
    return 'retail_script_%08x' % entry


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
              'task_type': str(row.get('task_type', 0)), 'selector_u32': 'attack_selector'}
    values.update({'word%d_i32' % i: str(word) for i, word in enumerate(words)})
    try:
        return '%s(%s)' % (binding.symbol, ', '.join(values[arg] for arg in binding.arguments))
    except KeyError as error:
        raise ValueError('binding argument %s is absent from instruction' % error.args[0]) from None


def reads_defines(row, bindings):
    op = int(row['opcode'], 16)
    reads = {'flags'} if op == 0x37 else {'loop'} if op == 0x3b else set()
    defines = {'flags'} if op in (0x36, 0x55, 0x51, 0x52, 0x5e) else {'loop'} if op == 0x3a else set()
    if 'clip' in row:
        defines.add('clip')
    elif op in (3, 4, 5, 6, 7, 0x12, 0x19, 0x1d, 0x47, 0x48, 0x49, 0x4a, 0x4d):
        reads.add('clip')
    if op == 0x1f:
        defines.add('selector')
    key = instruction_binding(row)
    binding = bindings.get(key[0]) if key else None
    if binding:
        if binding.result == 'flags':
            defines.add('flags')
        if 'selector_u32' in binding.arguments:
            reads.add('selector')
    return reads, defines


def validate(program, bindings, return_to):
    rows = {int(row['va'], 16): row for row in program['instructions']}
    entry = int(program['entry'], 16)
    errors = []
    def error(pc, reason):
        errors.append(dict(pc='0x%08x' % pc, reason=reason))
    occupied = {}
    for pc, row in rows.items():
        op = int(row['opcode'], 16)
        if 'unresearched' in row['operation']:
            error(pc, 'unsupported opcode %#x' % op)
        for word in range(pc, pc + 4 * (1 + len(row.get('operands', []))), 4):
            if word in occupied and occupied[word] != pc:
                error(pc, 'instruction overlaps operand of 0x%08x' % occupied[word])
            occupied[word] = pc
        for target in successors(row):
            if target not in rows:
                error(pc, 'missing script edge 0x%08x' % target)
        if op == 0x3b and 'branch' not in row:
            error(pc, 'unresolved loop target')
        if op == 0x3a and row['inline'] & 255 == 0:
            error(pc, 'zero loop count would underflow')
        if op in (0x30, 0x3f) and return_to is None:
            error(pc, 'controller/tagged return needs an explicit engine continuation')
        if op in (0x47, 0x48, 0x49) and row['inline'] >> 8 not in (0, 2):
            error(pc, 'unsupported playback prefix (only absent/descriptor prefixes are typed)')
        if 'clip' in row:
            first = row.get('start_sample', 0)
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
                try:
                    expression(binding, row)
                except ValueError as exc:
                    error(pc, str(exc))
    # Definite initialization over all incoming paths. No invented scratch
    # values for scripts entered midway through a native continuation.
    incoming = {pc: {'flags', 'loop', 'clip', 'selector'} for pc in rows}
    incoming[entry] = set()
    predecessors = {pc: [] for pc in rows}
    for pc, row in rows.items():
        for target in successors(row):
            if target in rows:
                predecessors[target].append(pc)
    changed = True
    while changed:
        changed = False
        for pc in sorted(rows):
            candidates = [incoming[p] | reads_defines(rows[p], bindings)[1] for p in predecessors[pc]]
            if pc == entry:
                candidates.append(set())
            value = set.intersection(*candidates) if candidates else set()
            if value != incoming[pc]:
                incoming[pc] = value
                changed = True
    for pc, row in rows.items():
        for name in reads_defines(row, bindings)[0] - incoming[pc]:
            error(pc, '%s is not initialized on every incoming path' % name)
    # Delete guaranteed-yield instructions, then reject cycles in the
    # remaining graph. A cycle that can bypass a wait is unsafe too.
    def yields(row):
        op = int(row['opcode'], 16)
        return op in (1, 3, 7, 9, 0x48, 0x3e) or (op == 0x34 and row['inline'] > 0) or (
            op == 0x37 and min(row['wait_if_set'], row['wait_if_clear']) > 0)
    active, done = set(), set()
    def visit(pc):
        if pc in active:
            error(pc, 'control-flow cycle can run without a positive wait')
            return
        if pc in done or pc not in rows or yields(rows[pc]):
            return
        active.add(pc)
        for target in successors(rows[pc]):
            visit(target)
        active.remove(pc)
        done.add(pc)
    for pc in sorted(rows):
        visit(pc)
    if errors:
        raise CompileError(errors)


def lower(row, bindings, return_to):
    op, inline = int(row['opcode'], 16), row['inline']
    arg = inline & 255
    words = [signed_word(int(word, 16)) for word in row.get('operands', [])]
    key = instruction_binding(row)
    if key:
        address, role = key
        binding = bindings[address]
        call = expression(binding, row)
        if role == 'physics':
            return ['fighter->body.physics = %s;' % binding.symbol]
        if role == 'predicate':
            bit = 4 if op == 0x36 else 1
            return ['result_flags = %s;' % call, 'if (result_flags & %du) {' % bit,
                    '  goto L_%08x;' % int(row['branch'], 16), '}']
        return [('result_flags = ' if binding.result == 'flags' else '') + call + ';'] + (['return;'] if role == 'tail' else [])
    lines = []
    if 'clip' in row:
        first = row.get('start_sample', 0)
        if op == 9:
            first = row['stored_count'] - 1
        lines.append('mk4_script_bind(fighter, %d, %d, %d);' % (row['clip'], row['stored_count'], first))
    if op == 0:
        lines.append('(void)fighter; /* decoded VM no-op */')
    elif op in (0x0f, 0x12, 0x49, 0x1d):
        if op != 0x0f:
            lines.append('fighter->sample_override = %d;' % arg)
    elif op in (1, 3, 0x48):
        lines.append('mk4_script_finish_clip(fighter, 1);')
    elif op in (7, 9):
        lines.append('mk4_script_finish_clip(fighter, -1);')
    elif op in (2, 4, 6, 0x19):
        lines.append('mk4_script_advance(fighter, %d, %d);' % (arg if op == 2 else inline, -1 if op == 6 else 2 if op == 0x19 else 1))
    elif op in (5, 0x47, 0x4a, 0x4d):
        lines.append('fighter_fiber_play_to(fighter, %d, %d);' % (arg if op in (0x47, 0x4a) else inline, -1 if op == 0x4a else 2 if op == 0x4d else 1))
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
        lines.append('fighter_fiber_finish(%s);' % return_to)
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
    elif op in (0x51, 0x52, 0x5e):
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
    else:
        raise AssertionError('Missing lowering for %#x' % op)
    return lines


def compile_program(image, entry, character=0, bindings=None, return_to=None):
    if return_to is not None and not re.fullmatch(r'[A-Za-z_]\w*', return_to):
        raise ValueError('Invalid continuation symbol')
    bindings = bindings_for(character) if bindings is None else bindings
    program = script_program(image, entry, character)
    validate(program, bindings, return_to)
    return program, bindings


def emit_program(program, bindings, return_to=None):
    rows = program['instructions']
    entry = int(program['entry'], 16)
    ops = {int(row['opcode'], 16) for row in rows}
    lines = ['static void %s(Fighter *fighter) {' % symbol(entry)]
    if ops & {0x36, 0x55, 0x51, 0x52, 0x5e, 0x37} or any(binding.result == 'flags' for binding in bindings.values()):
        lines.append('  unsigned result_flags = 0;')
        lines.append('  (void)result_flags;')
    if ops & {0x3a, 0x3b}:
        lines.append('  uint32_t loop_counter = 0;')
        lines.append('  (void)loop_counter;')
    if 0x1f in ops:
        lines += ['  int attack_selector = 0;', '  (void)attack_selector;']
    lines.append('  goto L_%08x;' % entry)
    for row in rows:
        pc = int(row['va'], 16)
        raw = ' '.join([row['word'], *row.get('operands', [])])
        lines += ['L_%08x: {' % pc, '  /* EXE %08x: %s */' % (pc, raw)]
        for statement in lower(row, bindings, return_to):
            lines.extend('  ' + part for part in statement.splitlines())
        op = int(row['opcode'], 16)
        if op not in (0x30, 0x32, 0x33, 0x3c, 0x3e, 0x3f):
            # Explicit fallthrough protects against address ordering and gaps.
            following = pc + 4 * (1 + len(row.get('operands', [])))
            lines.append('  goto L_%08x;' % following)
        lines.append('}')
    lines.append('}')
    return '\n'.join(lines)


def generate(image, entries, character=0, return_to=None, bindings=None):
    programs = [compile_program(image, entry, character, bindings, return_to) for entry in sorted(set(entries))]
    lines = ['/* Generated by tools/compile_retail.py. Do not edit. */',
             '/* EXE SHA-256: %s; roster %d. */' % (EXE_SHA256, character), '#include "script.h"', '']
    contacts = {row['contact']['va']: row['contact'] for program, _ in programs for row in program['instructions'] if 'contact' in row}
    for address, record in sorted(contacts.items()):
        values = [*record['rectangle_q16'], record['reaction'], record['alternate_reaction'], record['blocked_reaction'], record['flags'], record['damage_q16']]
        lines.append('static const AttackRecord retail_contact_%s = {%s};' % (address[2:], ', '.join(map(str, values))))
    for program, binding in programs:
        lines += ['', emit_program(program, binding, return_to)]
    return '\n'.join(lines) + '\n'
