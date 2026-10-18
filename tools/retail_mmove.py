"""Offline frame-table lowering for statically timed decoded script paths.

Resolve roster branches and counted loops at generation time. Runtime
predicates select named phases specialized for their known clip/cursor state.
Unsupported yielding native bindings fail closed. No runtime instruction pointer,
interpreter or suspended C stack is emitted.
"""
from retail_codegen import instruction_binding, expression, symbol, CompileError
from retail_script_c import successors
import re


def frame_table(name, samples, callbacks=None, endfunc='fighter_stop_mmove'):
    """Emit fixed-step callback rows, with pose mapping separate from actions."""
    callbacks = callbacks or {}
    linear = samples == list(range(samples[0], samples[0] + len(samples)))
    first = samples[0] if linear else 0
    lines = []
    if not linear:
        lines += ['static const int %s_samples[] = {%s};' % (name, ', '.join(map(str, samples)))]
    if callbacks:
        lines += ['// clang-format off: Keep table entries on one row, including long rows.',
                  'static const mframe_t %s_frames[%d] = {' % (name, len(samples))]
        for frame, callback in sorted(callbacks.items()):
            lines.append('  [%d] = {.thinkfunc = %s},' % (frame, callback))
        lines += ['};', '// clang-format on']
    else:
        lines += ['static const mframe_t %s_frames[%d] = {{0}};' % (name, len(samples))]
    mapping = ', .samples = %s_samples' % name if not linear else ''
    lines += ['static const mmove_t %s_phase = {.firstframe = %d, .lastframe = %d, .frame = %s_frames, .endfunc = %s%s};' %
              (name, first, first + len(samples) - 1, name, endfunc, mapping)]
    return lines


def emit_mmove(program, bindings, return_to, context=None):
    context = context or {}
    root = symbol(int(program['entry'], 16))
    pending = [(int(program['entry'], 16), context.get('sample'), context.get('count'), context.get('paired', False), None, None)]
    names = {pending[0]: root}
    def enqueue(state):
        if state not in names:
            if len(names) >= 512:
                raise CompileError([dict(pc=hex(state[0]), reason='mmove: excessive phase expansion')])
            names[state] = root + '_branch_' + str(len(names))
            pending.append(state)
        return names[state]
    bodies = []
    for state in pending:
        bodies.append(emit_phase(program, bindings, return_to, context if names[state] == root else {},
                                 names[state], state, enqueue))
    declarations = ['static inline void %s(Fighter *fighter);' % name for name in names.values()]
    return '\n'.join(declarations + bodies)


def emit_phase(program, bindings, return_to, context, name, state, enqueue):
    context = context or {}
    rows = {int(row['va'], 16): row for row in program['instructions']}
    events = [[]]
    samples = []
    extra = []
    pc, sample, count, victim_bound, loop, selector = state

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

    def advance(target, step=1, paired=False, period=1):
        nonlocal sample
        if paired and not victim_bound:
            fail('paired playback without a known victim clip')
        if sample is None or count is None:
            fail('playback without a known clip')
        while sample < target if step > 0 else sample > target:
            wait(period)
            sample = min(target, sample + step) if step > 0 else max(target, sample + step)
            emit('fighter->sample_override = %d;' % sample)
            if paired:
                emit('if (!mk4_mmove_pair_step(fighter)) { return; }')

    if 'clip' in context:
        if context['clip'] == 'current':
            emit('mk4_mmove_inherit_current(fighter, %d, %d);' % (count, sample))
        else:
            emit('mk4_script_inherit(fighter, %d, %d, %d);' % (context['clip'], count, sample))

    def branch(condition, target, following):
        yes = enqueue((target, sample, count, victim_bound, loop, selector))
        no = enqueue((following, sample, count, victim_bound, loop, selector))
        emit('if (%s) { %s(fighter); } else { %s(fighter); }' % (condition, yes, no))

    visited = set()
    for _ in range(8192):
        cursor = (pc, sample, count, victim_bound, loop, selector)
        if cursor in visited:
            if not samples:
                fail('cycle without a timed frame')
            emit(enqueue(cursor) + '(fighter);')
            break
        visited.add(cursor)
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
        elif key and binding.mmove_lowering == 'reverse_return':
            advance(0, -1)
            wait(1)
            emit('fighter_finish_mmove(fighter, fighter_ready);')
            break
        elif key and binding.mmove_lowering == 'controller_return':
            emit('mk4_mmove_thrown_finish(fighter);')
            break
        elif key and binding.mmove_lowering == 'current':
            advance(count - 1)
            wait(1)
        elif key and binding.mmove_lowering == 'pair_to':
            advance(arg, paired=True)
        elif key:
            if not binding.mmove:
                fail('native binding is not reviewed for nonblocking frame callbacks: ' + binding.symbol)
            if binding.flow == 'suspend':
                fail('yielding binding needs a frame lowering: ' + binding.symbol)
            if key[1] == 'physics':
                emit('fighter->body.physics = %s;' % binding.symbol)
            else:
                call = expression(binding, row)
                if 'selector_u32' in binding.arguments:
                    if selector is None:
                        fail('unknown attack selector')
                    call = call.replace('attack_selector', str(selector))
                emit(('fighter->mmove_result = ' if binding.result == 'flags' else '') + call + ';')
                if key[1] == 'predicate':
                    branch('fighter->mmove_result & %du' % (4 if op == 0x36 else 1), int(row['branch'], 16), following)
                    break
            if key[1] == 'tail' or (key[1] == 'handler' and 'unresearched' in row['operation']):
                break
        else:
            if 'clip' in row and op != 0x71:
                first = row.get('start_sample', inline if op == 8 else 0)
                if op == 9:
                    first = row['stored_count'] - 1
                if op == 0x0e:
                    victim_bound = True
                    emit('mk4_mmove_bind((Fighter *)fighter_opponent(fighter), %d, %d, %d);' % (row['clip'], row['stored_count'], first))
                else:
                    sample, count = first, row['stored_count']
                    emit('mk4_mmove_bind(fighter, %d, %d, %d);' % (row['clip'], count, sample))
            if op in (0, 0x0e, 0x0f, 0x11, 0x4e):
                pass
            elif op == 0x1f:
                selector = int(row['operands'][0], 16)
            elif op in (1, 3, 7, 8, 9, 0x48):
                advance(0 if op in (7, 9) else count - 1, -1 if op in (7, 9) else 1)
                wait(1)
            elif op == 0x71:
                sample, count = 0, row['stored_count']
                emit('mk4_mmove_blend_bind(fighter, %d, %d);' % (row['clip'], count))
                step, first = 0xa3d, 0x4000
                frames = (0x10000 - first) // step + 1
                for frame in range(1, frames + 1):
                    wait(1)
                    emit('fighter->blend_weight = %d;' % (first + min(frame, frames - 1) * step))
                emit('fighter->blend_duration = 0;')
                emit('fighter->blend_weight = -1;')
                sample = 1
                emit('fighter->sample_override = 1;')
            elif op == 0x46:
                first = sample
                advance(min(count - 1, sample + arg) if arg else count - 1, period=2)
                if arg == 0 or first + arg >= count:
                    wait(1)
            elif op == 0x69:
                advance(count - 1, period=2)
                wait(1)
            elif op in (2, 4, 6, 0x0c, 0x19, 0x10):
                if op == 0x10 and not victim_bound:
                    fail('paired playback without a known victim clip')
                steps = 1 if op == 0x0c else arg if op == 2 else (inline + 1) // 2 if op == 0x19 else inline
                step = -1 if op == 6 else 2 if op == 0x19 else 1
                for _ in range(steps):
                    wait(1)
                    sample = max(0, min(count - 1, sample + step))
                    emit('fighter->sample_override = %d;' % sample)
                    if op == 0x10:
                        emit('if (!mk4_mmove_pair_step(fighter)) { return; }')
            elif op in (5, 0x47, 0x4a, 0x4d):
                advance(inline if op == 5 else arg, -1 if op == 0x4a else 2 if op == 0x4d else 1)
            elif op == 0x0d:
                if sample is None or count is None or sample + 1 >= count:
                    fail('next pose exceeds bound clip')
                sample += 1
                emit('fighter->sample_override = %d;' % sample)
            elif op in (0x12, 0x49, 0x1d):
                sample = arg
                emit('fighter->sample_override = %d;' % sample)
            elif op in (0x2f, 0x44, 0x45, 0x42):
                condition = {0x2f: 'fighter_airborne(fighter)', 0x44: 'fighter->held & BUTTON_BIT(BUTTON_DOWN)',
                             0x45: 'fighter_fast_start(fighter)', 0x42: 'fighter->mmove_result & 1u'}[op]
                branch(condition, int(row['branch'], 16), following)
                break
            elif op == 0x37:
                # The branch successors carry identical clip state but different
                # bounded holds. Each side owns its rows, with no runtime timer VM.
                a, b = row['wait_if_set'], row['wait_if_clear']
                suffix = enqueue((following, sample, count, victim_bound, loop, selector))
                if a == b:
                    wait(a)
                    emit(suffix + '(fighter);')
                else:
                    holds = []
                    for label, ticks in (('set', a), ('clear', b)):
                        if ticks == 0:
                            holds.append(suffix + '(fighter);')
                            continue
                        hold = name + '_hold_' + label
                        extra.extend(frame_table(hold, [sample] * ticks, endfunc=suffix))
                        holds.append('fighter_mmove_phase(fighter, &%s_phase);' % hold)
                    emit('if (fighter->mmove_result & 1u) { %s } else { %s }' % tuple(holds))
                break
            elif op in (0x51, 0x52, 0x53, 0x5e):
                emit('fighter->mmove_result = battle_contact(fighter, &retail_contact_%s) != CONTACT_MISS ? 1u : 0u;' % row['contact']['va'][2:])
            elif op == 0x61:
                emit('fighter->flags &= ~(unsigned)FF_BUSY;')
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
            elif op == 0x58:
                from retail_codegen import signed_word
                words = [signed_word(int(word, 16)) for word in row['operands']]
                emit('fighter_set_speed(fighter, %d);' % words[0])
                emit('fighter->body.vel[AXIS_Y] = %d;' % words[1])
                emit('fighter->body.gravity = %d;' % words[2])
            elif op == 0x3e:
                if sample is None:
                    fail('park without a known pose')
                hold = name + '_park'
                extra.extend(frame_table(hold, [sample], {0: 'mk4_mmove_hold'}))
                emit('fighter_mmove_phase(fighter, &%s_phase);' % hold)
                break
            elif op == 0x60:
                suffix = enqueue((following, sample, count, victim_bound, loop, selector))
                hold = name + '_landing'
                extra.extend(frame_table(hold, [sample], {0: 'mk4_mmove_wait_grounded'}, endfunc=suffix))
                emit('if (fighter_airborne(fighter)) { fighter_mmove_phase(fighter, &%s_phase); } else { %s(fighter); }' % (hold, suffix))
                break
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

    # Entry setup runs immediately. Decoded boundary effects become optional
    # think callbacks on fixed simulation frames, as in monster move tables.
    lines = extra
    callbacks = {}
    for tick, statements in enumerate(events[1:], 1):
        # Moving to the next row already selects its sample. Keep assignments
        # only at boundaries that also execute actual native/script effects.
        meaningful = [line for line in statements if not line.startswith('/*') and
                      not re.fullmatch(r'fighter->sample_override = \d+;', line)]
        if not meaningful:
            continue
        callbacks[tick - 1] = '%s_at_%d' % (name, tick)
        lines += ['static void %s_at_%d(Fighter *fighter) {' % (name, tick)]
        lines += ['  ' + statement for statement in statements] or ['  (void)fighter;']
        lines += ['}']
    if samples:
        lines += frame_table(name, samples, callbacks)
    lines += ['static inline void %s(Fighter *fighter) {' % name,
              '  require(!fighter_fiber_active(fighter), "Frame program cannot execute in a fiber");']
    lines += ['  ' + statement for statement in events[0]]
    if samples:
        lines += ['  fighter_mmove_phase(fighter, &%s_phase);' % name]
    lines += ['}']
    return '\n'.join(lines)
