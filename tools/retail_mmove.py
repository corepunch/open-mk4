"""Offline frame-table lowering for statically timed decoded script paths.

Resolve roster branches and counted loops at generation time. Unsupported
runtime decisions and yielding native bindings fail closed; native decisions
may explicitly select another generated phase. No runtime instruction pointer,
interpreter or suspended C stack is emitted.
"""
from retail_codegen import instruction_binding, expression, symbol, CompileError
from retail_script_c import successors
import re


def emit_mmove(program, bindings, return_to, context=None):
    context = context or {}
    rows = {int(row['va'], 16): row for row in program['instructions']}
    entry = int(program['entry'], 16)
    name = symbol(entry)
    events = [[]]
    samples = []
    sample = context.get('sample')
    count = context.get('count')
    loop = selector = None
    pc = entry

    def fail(reason):
        raise CompileError([dict(pc=hex(pc), reason='mmove: ' + reason)])

    def emit(line):
        events[-1].append(line)

    def wait(ticks):
        if sample is None:
            fail('wait without a known sample')
        if ticks < 0 or len(samples) + ticks > 4096:
            fail('unbounded frame expansion')
        for _ in range(ticks):
            samples.append(sample)
            events.append([])

    def advance(target, step=1, paired=False):
        nonlocal sample
        if sample is None or count is None:
            fail('playback without a known clip')
        while sample < target if step > 0 else sample > target:
            wait(1)
            sample = min(target, sample + step) if step > 0 else max(target, sample + step)
            emit('fighter->sample_override = %d;' % sample)
            if paired:
                emit('if (!mk4_mmove_pair_step(fighter)) { return; }')

    if 'clip' in context:
        if context['clip'] == 'current':
            fail('dynamic inherited clip is not supported')
        emit('mk4_script_inherit(fighter, %d, %d, %d);' % (context['clip'], count, sample))

    for _ in range(8192):
        row = rows[pc]
        op = int(row['opcode'], 16)
        inline = row.get('inline', 0)
        arg = inline & 255
        raw = ' '.join([row['word'], *row.get('operands', [])])
        emit('/* EXE %08x: %s */' % (pc, raw))
        following = pc + 4 * (1 + len(row.get('operands', [])))
        key = instruction_binding(row)
        binding = bindings.get(key[0]) if key else None
        if binding and binding.script_target:
            pc = binding.script_target
            continue
        if op in (0x33, 0x3c):
            pc = successors(row)[0]
            continue
        if op == 0x3a:
            loop = arg
        elif op == 0x3b:
            if loop is None or loop <= 0:
                fail('unknown loop counter')
            loop -= 1
            if loop:
                following = int(row['branch'], 16)
        elif key:
            if not binding.mmove:
                fail('native binding is not reviewed for nonblocking frame callbacks: ' + binding.symbol)
            if binding.flow == 'suspend':
                fail('yielding binding needs a frame lowering: ' + binding.symbol)
            if key[1] == 'physics':
                emit('fighter->body.physics = %s;' % binding.symbol)
            else:
                if binding.result != 'void':
                    fail('dynamic native result requires a phase branch')
                call = expression(binding, row)
                if 'selector_u32' in binding.arguments:
                    if selector is None:
                        fail('unknown attack selector')
                    call = call.replace('attack_selector', str(selector))
                emit(call + ';')
            if key[1] == 'tail' or (key[1] == 'handler' and 'unresearched' in row['operation']):
                break
        else:
            if 'clip' in row:
                first = row.get('start_sample', inline if op == 8 else 0)
                if op == 9:
                    first = row['stored_count'] - 1
                if op == 0x0e:
                    fail('paired clip binding not yet lowered')
                sample, count = first, row['stored_count']
                emit('mk4_mmove_bind(fighter, %d, %d, %d);' % (row['clip'], count, sample))
            if op in (0, 0x0f, 0x11, 0x4e):
                pass
            elif op == 0x1f:
                selector = int(row['operands'][0], 16)
            elif op in (1, 3, 7, 8, 9, 0x48, 0x69):
                advance(0 if op in (7, 9) else count - 1, -1 if op in (7, 9) else 1)
                wait(1)
            elif op in (2, 4, 6, 0x0c, 0x19):
                steps = 1 if op == 0x0c else arg if op == 2 else (inline + 1) // 2 if op == 0x19 else inline
                step = -1 if op == 6 else 2 if op == 0x19 else 1
                for _ in range(steps):
                    wait(1)
                    sample = max(0, min(count - 1, sample + step))
                    emit('fighter->sample_override = %d;' % sample)
            elif op in (5, 0x47, 0x4a, 0x4d):
                advance(inline if op == 5 else arg, -1 if op == 0x4a else 2 if op == 0x4d else 1)
            elif op in (0x12, 0x49, 0x1d):
                sample = arg
                emit('fighter->sample_override = %d;' % sample)
            elif op == 0x34:
                wait(inline)
            elif op in (0x20, 0x27, 0x2b):
                emit('battle_sound(SOUND_%s, %d);' % ({0x20: 'A', 0x27: 'C', 0x2b: 'B'}[op], arg))
            elif op in (0x29, 0x2d):
                emit('fighter->action = ACTION_RETAIL_%s;' % ('1006' if op == 0x29 else '1007'))
            elif op == 0x2e:
                emit('fighter->flags |= FF_NO_GUARD;')
            elif op == 0x5b:
                emit('mk4_script_stop(fighter);')
            elif op == 0x39:
                emit('fighter->body.physics = NULL;')
            elif op == 0x5a:
                emit('fighter_set_speed(fighter, %d);' % row['speed_q16'])
            elif op in (0x30, 0x3f):
                if return_to == 'caller':
                    fail('caller continuation needs an explicit phase successor')
                if op == 0x3f:
                    wait(1)
                emit('fighter_finish_mmove(fighter, %s);' % return_to)
                break
            else:
                fail('unsupported frame opcode ' + hex(op))
        pc = following
    else:
        fail('unbounded control-flow expansion')

    # Entry effects run immediately. Each row describes the pose held during a
    # tick; its leave callback performs the decoded work at the tick boundary.
    lines = []
    callbacks = set()
    for tick, statements in enumerate(events[1:], 1):
        # Moving to the next row already selects its sample. Keep assignments
        # only at boundaries that also execute actual native/script effects.
        meaningful = [line for line in statements if not line.startswith('/*') and
                      not re.fullmatch(r'fighter->sample_override = \d+;', line)]
        if not meaningful:
            continue
        callbacks.add(tick)
        lines += ['static void %s_at_%d(Fighter *fighter) {' % (name, tick)]
        lines += ['  ' + statement for statement in statements] or ['  (void)fighter;']
        lines += ['}']
    if samples:
        lines += ['// clang-format off: Keep table entries on one row, including long rows.',
                  'static const mframe_t %s_frames[] = {' % name]
        for tick, pose in enumerate(samples, 1):
            callback = ', .leave = %s_at_%d' % (name, tick) if tick in callbacks else ''
            lines.append('  {.sample = %d, .duration = 1%s},' % (pose, callback))
        lines += ['};', '// clang-format on',
                  'static const mmove_t %s_phase = {.firstframe = 0, .lastframe = %d, .frame = %s_frames};' % (name, len(samples) - 1, name)]
    lines += ['static inline void %s(Fighter *fighter) {' % name]
    lines += ['  ' + statement for statement in events[0]]
    if samples:
        lines += ['  fighter_mmove_phase(fighter, &%s_phase);' % name]
    lines += ['}']
    return '\n'.join(lines)
