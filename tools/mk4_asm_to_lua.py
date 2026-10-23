#!/usr/bin/env python3
"""Translate the MK4 wordcode script modules into Lua actor scripts.

Each script label becomes one Lua function; jumps and fall-through into a
labelled stream become tail calls; counted loops become `for`; roster
selectors become tables keyed by roster name; natives keep their stable IDs.
Mid-instruction entry points (aliases such as `high_kick + 64`, which the
wordcode executes as no-ops before the following stream) become functions
that own the stream after the no-op words.

This produced the editable Lua sources of the migration; it remains a research
tool for assembled original words (for example an import of the executable).
`--check` compares an existing Lua tree with a translation instead of writing.
"""
import argparse
from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from mk4_asm import OPS, lines  # noqa: E402
from mk4_link import BASE, evaluate, link  # noqa: E402

ROSTER = ['scorpion', 'raiden', 'sonya', 'liu_kang', 'sub_zero', 'fujin', 'shinnok', 'reiko',
          'quan_chi', 'tanya', 'reptile', 'kai', 'jarek', 'jax', 'cage', 'goro']
NOPS = {0x00, 0x0b, 0x15, 0x18, 0x21, 0x23, 0x4e, 0x57, 0x66, 0x67}

# opcode: (lua name, inline field use, operand kinds, form)
#   inline: z zero/ignored, a low byte, w 24 bits, s two 12-bit fields, p optional clip prefix
#   operands: clip, word (branch target), native, num, addr (packed data address)
#   form: stmt, branch, jump, select, loop, repeat, finish
# clang-format off
SPEC = {
    0x01: ('clip', 'z', ['clip'], 'stmt'),
    0x02: ('play_clip', 'a', ['clip'], 'stmt'),
    0x03: ('forward', 'z', [], 'stmt'),
    0x04: ('forward_count', 'a', [], 'stmt'),
    0x05: ('forward_to', 'w', [], 'stmt'),
    0x06: ('backward_count', 'w', [], 'stmt'),
    0x07: ('backward', 'z', [], 'stmt'),
    0x08: ('clip_alt', 'a', ['clip'], 'stmt'),
    0x09: ('clip_operation', 'z', ['clip'], 'stmt'),
    0x0a: ('cursor_advance', 'a', [], 'stmt'),
    0x0c: ('bind_submit', 'a', ['clip'], 'stmt'),
    0x0d: ('playback_native', 'z', [], 'stmt'),
    0x0e: ('open_peer_clip', 'z', ['clip'], 'stmt'),
    0x0f: ('open_clip', 'a', ['clip'], 'stmt'),
    0x10: ('play_pair', 'a', [], 'stmt'),
    0x11: ('bind_cursor', 'a', ['clip'], 'stmt'),
    0x12: ('cursor_set', 'a', [], 'stmt'),
    0x13: ('cursor_last', 'z', [], 'stmt'),
    0x14: ('submit_last', 'z', [], 'stmt'),
    0x16: ('scene_active', 'z', [], 'stmt'),
    0x17: ('play_double', 'z', ['clip'], 'stmt'),
    0x19: ('double_count', 'a', [], 'stmt'),
    0x1a: ('play_interleaved', 'z', ['clip'], 'stmt'),
    0x1b: ('return_word', 'a', ['addr'], 'finish'),
    0x1c: ('inherited_interleaved', 'a', [], 'stmt'),
    0x1d: ('playback_tail', 'a', [], 'stmt'),
    0x1e: ('native_step', 'a', [], 'stmt'),
    0x1f: ('attack_selector', 'z', ['num'], 'stmt'),
    0x20: ('sound_a', 'a', [], 'stmt'),
    0x22: ('task_callback', 'z', [], 'stmt'),
    0x24: ('release_attack', 'z', [], 'stmt'),
    0x25: ('sound_indexed', 'a', [], 'stmt'),
    0x26: ('sound_context', 'a', [], 'stmt'),
    0x27: ('sound_c', 'a', [], 'stmt'),
    0x28: ('sound_special', 'z', [], 'stmt'),
    0x29: ('action_recovery', 'z', [], 'stmt'),
    0x2a: ('spawn_effect', 'a', [], 'stmt'),
    0x2b: ('sound_b', 'a', [], 'stmt'),
    0x2c: ('sound_variant', 'z', [], 'stmt'),
    0x2d: ('action_followup', 'z', [], 'stmt'),
    0x2e: ('no_guard', 'z', [], 'stmt'),
    0x2f: ('airborne', 'z', ['word'], 'branch'),
    0x30: ('finish', 'z', [], 'finish'),
    0x32: ('tail', 'z', ['native'], 'stmt'),
    0x33: ('jump', 'z', ['word'], 'jump'),
    0x34: ('wait', 'w', [], 'stmt'),
    0x35: ('call', 'z', ['native'], 'stmt'),
    0x36: ('predicate_flags', 'z', ['native', 'word'], 'branch'),
    0x37: ('wait_branch', 's', [], 'stmt'),
    0x38: ('handler', 'z', ['native'], 'stmt'),
    0x39: ('clear_handler', 'z', [], 'stmt'),
    0x3a: ('loop', 'a', [], 'loop'),
    0x3b: ('repeat', 'z', [], 'repeat'),
    0x3c: ('select_roster', 'z', ['word'] * 16, 'select'),
    0x3d: ('blocked', 'z', ['word'], 'branch'),
    0x3e: ('park', 'z', [], 'stmt'),
    0x3f: ('ready', 'z', [], 'finish'),
    0x40: ('spawn', 'z', ['num', 'native'], 'stmt'),
    0x41: ('native_return', 'a', [], 'stmt'),
    0x42: ('contact', 'z', ['word'], 'branch'),
    0x43: ('predicate_bit', 'z', ['native', 'word'], 'branch'),
    0x44: ('down', 'z', ['word'], 'branch'),
    0x45: ('fast_start', 'z', ['word'], 'branch'),
    0x46: ('half_count', 'p', [], 'stmt'),
    0x47: ('prefixed_to', 'p', [], 'stmt'),
    0x48: ('prefixed_finish', 'p', [], 'stmt'),
    0x49: ('prefixed_sample', 'p', [], 'stmt'),
    0x4a: ('playback_pair', 'a', [], 'stmt'),
    0x4b: ('playback_single', 'a', [], 'stmt'),
    0x4c: ('playback_pair_reverse', 'a', [], 'stmt'),
    0x4d: ('double_to', 'a', [], 'stmt'),
    0x4f: ('native_release', 'a', [], 'stmt'),
    0x50: ('query_branch', 'a', ['word'], 'branch'),
    0x51: ('query_b', 'a', [], 'stmt'),
    0x52: ('query_a', 'a', [], 'stmt'),
    0x53: ('query_d', 'a', [], 'stmt'),
    0x54: ('wait_result', 's', [], 'stmt'),
    0x55: ('predicate_argument', 'a', ['native', 'word'], 'branch'),
    0x56: ('scene_event', 'a', [], 'stmt'),
    0x58: ('motion', 'z', ['num', 'num', 'num'], 'stmt'),
    0x59: ('motion_block', 'z', ['num', 'num', 'num', 'num'], 'stmt'),
    0x5a: ('speed_negative', 'z', ['num'], 'stmt'),
    0x5b: ('landing', 'z', [], 'stmt'),
    0x5c: ('gravity', 'z', ['num'], 'stmt'),
    0x5d: ('position', 'z', ['num', 'num'], 'stmt'),
    0x5e: ('weapon_query', 'a', [], 'stmt'),
    0x5f: ('speed', 'z', ['num'], 'stmt'),
    0x60: ('native_context', 'z', [], 'stmt'),
    0x61: ('allow_commands', 'z', [], 'stmt'),
    0x62: ('effect', 'a', [], 'stmt'),
    0x63: ('damage', 'z', ['num'], 'stmt'),
    0x64: ('face', 'a', [], 'stmt'),
    0x65: ('peer_motion', 'z', ['num', 'num', 'num'], 'stmt'),
    0x68: ('half_restart', 'p', [], 'stmt'),
    0x69: ('half_finish', 'z', [], 'stmt'),
    0x6a: ('native_sound', 'z', [], 'stmt'),
    0x6b: ('sound_roster', 'a', [], 'stmt'),
    0x6c: ('schedule_motion', 's', ['num'], 'stmt'),
    0x6d: ('native_word', 'z', ['num'], 'stmt'),
    0x6e: ('disarm_peer', 'z', [], 'stmt'),
    0x6f: ('edge', 'z', ['word'], 'branch'),
    0x70: ('reaction_distance', 'a', ['num'], 'stmt'),
    0x71: ('blend', 'z', ['clip'], 'stmt'),
    0x72: ('edge_push', 'z', [], 'stmt'),
    0x73: ('sound_random', 'z', [], 'stmt'),
    0x74: ('sound_token', 'z', [], 'stmt'),
    0x75: ('block_sound', 'z', [], 'stmt'),
    0x76: ('block_sound_alt', 'z', [], 'stmt'),
}
# clang-format on
MNEMONIC_TO_OP = {name: op for name, (op, _) in OPS.items()}


class Instruction:
    def __init__(self, address, module, line, opcode, inline, operands, raw_args):
        self.address, self.module, self.line = address, module, line
        self.opcode, self.inline, self.operands, self.raw_args = opcode, inline, operands, raw_args
        self.size = 4 * (1 + len(operands))


def parse(project):
    """Rows of the script section with addresses, in link order, plus symbols."""
    _, header, symbols = link(project)
    script_begin = int(re.search(r'#define SCRIPT_ADDRESS_BEGIN 0x([0-9a-f]+)u', header).group(1), 16)
    pc, section = BASE, None
    instructions, guards, labels_at = [], set(), {}
    for location, line in lines(Path(project)):
        path, _, number = location.rpartition(':')
        if line.startswith('.section '):
            section = line.split()[1]
            continue
        if line.startswith(('.equ ', '.alias ', '.export ')):
            continue
        if line.endswith(':'):
            if section == 'script':
                labels_at.setdefault(pc, []).append(line[:-1])
            continue
        name, _, arguments = line.partition(' ')
        if name == '.zero':
            size = evaluate(arguments, symbols)
            if section == 'script':
                guards.add(pc)
            pc += size
            continue
        if name == '.word':
            pc += 4 * len(arguments.split(','))
            continue
        if name == '.string':
            import json
            import struct
            encoded = json.loads(arguments).encode('ascii') + b'\0'
            pc += len(encoded) + (-len(encoded) % 4)
            continue
        args = [a.strip() for a in arguments.split(',')]
        op, count = OPS[name]
        inline = evaluate(args[0], symbols)
        if count == -1:
            count = int((inline >> 8 & 255) == 2)
        operands = [evaluate(a, symbols) for a in args[1:]]
        if section == 'script':
            instructions.append(Instruction(pc, Path(path).stem, int(number), op, inline, operands, args[1:]))
        pc += 4 * (1 + len(operands))
    return instructions, guards, labels_at, symbols, script_begin, pc


def collect_comments(module_paths):
    """Comment lines directly above each label, by (module, label)."""
    comments = {}
    for path in module_paths:
        if not path.exists():
            continue
        pending = []
        for raw in path.read_text().splitlines():
            stripped = raw.strip()
            if stripped.startswith(';'):
                pending.append(stripped[1:].strip())
            elif re.fullmatch(r'[A-Za-z_]\w*:', stripped):
                comments[(path.stem, stripped[:-1])] = pending
                pending = []
            elif stripped:
                pending = []
            else:
                pending = []
    return comments


def referenced_names(project_dir):
    """Names used as packed script references or exported as non-fixture entries."""
    text = '\n'.join(p.read_text() for p in project_dir.rglob('*.asm')) + '\n'.join(p.read_text() for p in project_dir.rglob('*.inc'))
    names = set(re.findall(r'word\(([A-Za-z_]\w*)\)', text))
    for export, target in re.findall(r'^\.export\s+(\S+)\s+(\S+)', text, re.M):
        if not export.startswith('MK4_FIXTURE_REF_'):
            names.add(target)
    return names


def lua_int(value, hint=''):
    if hint == 'hex' or value > 4095:
        return f'0x{value:x}'
    return str(value)


def render_operand(kind, value, raw, symbols, labels_by_address, clip_table):
    if kind == 'clip':
        if (value * 4 - clip_table) % 12 == 0 and clip_table <= value * 4:
            return str((value * 4 - clip_table) // 12)
        raise ValueError('operand is not an animation descriptor: ' + raw)
    if kind == 'native':
        if raw.startswith('MK4_NATIVE_'):
            return 'native.' + raw[len('MK4_NATIVE_'):]
        return lua_int(value, 'hex')
    if kind == 'word':
        names = labels_by_address.get(value * 4)
        if not names:
            raise ValueError('branch target is not a script entry: ' + raw)
        return 'S.' + names[0]
    if kind == 'addr':
        match = re.fullmatch(r'word\(([A-Za-z_]\w*)\)', raw)
        if match:
            return 'labels.' + match.group(1)
        return lua_int(value, 'hex')
    # num: keep the source spelling when it is a plain literal
    if re.fullmatch(r'-?0x[0-9a-fA-F]+|-?\d+', raw):
        return raw.lower()
    return lua_int(value & 0xffffffff, 'hex')


class Module:
    def __init__(self, name):
        self.name = name
        self.out = []
        self.entries = []


def translate(project, output_dir, check=False):
    project = Path(project)
    instructions, guards, labels_at, symbols, script_begin, script_end = parse(project)
    by_address = {i.address: i for i in instructions}
    clip_table = symbols['MK4_DATA_ANIMATION_TABLE']
    module_order = []
    for i in instructions:
        if i.module not in module_order:
            module_order.append(i.module)
    comments = collect_comments([project.parent / 'code' / (m + '.asm') for m in module_order])
    referenced = referenced_names(project.parent)

    # Entry points: every label, plus referenced aliases inside instructions.
    entries = {}  # address -> [names]; address is where the owned stream starts
    for address, names in labels_at.items():
        entries.setdefault(address, []).extend(names)
    alias_targets = {}  # alias name -> function it tail-calls (continuation entry)
    for name, value in symbols.items():
        if name.startswith('MK4_') or not script_begin <= value < script_end or value in labels_at or name not in referenced:
            continue
        # Find the enclosing instruction and check the remaining operand words are no-ops.
        owner = max((i for i in instructions if i.address <= value), key=lambda i: i.address)
        if value >= owner.address + owner.size or (value - owner.address) % 4:
            raise ValueError(f'alias {name} is not inside an instruction')
        first = (value - owner.address) // 4 - 1
        for word in owner.operands[first:]:
            if (word >> 24) not in NOPS:
                raise ValueError(f'alias {name}: operand word {word:08x} is not a no-op; cannot enter mid-instruction')
        continuation = owner.address + owner.size
        if continuation in labels_at:
            alias_targets[name] = labels_at[continuation][0]
        else:
            entries.setdefault(continuation, []).append(name)
    boundaries = set(entries)
    # Packed references name labels or aliases by their own address.
    targets = {address: list(names) for address, names in labels_at.items()}
    for address, names in entries.items():
        for name in names:
            if name not in labels_at.get(address, []):
                targets.setdefault(symbols[name], []).append(name)
    for name in alias_targets:
        targets.setdefault(symbols[name], []).append(name)

    modules = {name: Module(name) for name in module_order}
    index = {i.address: n for n, i in enumerate(instructions)}
    statistics = {'functions': 0, 'instructions': 0, 'inline_ignored': 0}

    def emit_function(module, names, start_address):
        statistics['functions'] += 1
        out = module.out
        primary = names[0]
        head = comments.get((module.name, primary), [])
        out.append('')
        for text in head:
            out.append('-- ' + text)
        for alias in names[1:]:
            out.append(f'-- Also entered as {alias}.')
        out.append(f'function S.{primary}()')
        depth = 1
        n = index[start_address]
        loop_stack = []
        while True:
            if n >= len(instructions):
                break
            ins = instructions[n]
            if ins.address != start_address and (ins.address in boundaries or ins.module != module.name):
                if loop_stack:
                    raise ValueError(f'{module.name}: entry {labels_at.get(ins.address)} splits a counted loop in {primary}')
                target = entries[ins.address][0] if ins.address in entries else None
                if target is None:
                    raise ValueError(f'{module.name}: stream {primary} runs into another module')
                out.append('  ' * depth + f'return S.{target}()')
                break
            statistics['instructions'] += 1
            stop = emit_instruction(module, ins, depth, loop_stack)
            if stop == 'loop':
                depth += 1
            elif stop == 'repeat':
                depth -= 1
            elif stop == 'end':
                break
            n += 1
            if n < len(instructions) and instructions[n].address != ins.address + ins.size:
                break  # a guard word separates this stream from the next
        if loop_stack:
            raise ValueError(f'{module.name}: unterminated counted loop in {primary}')
        out.append('end')
        for alias in names[1:]:
            out.append(f'S.{alias} = S.{primary}')

    def emit_instruction(module, ins, depth, loop_stack):
        out = module.out
        pad = '  ' * depth
        op = ins.opcode
        if op in NOPS:
            return None
        if op not in SPEC:
            raise ValueError(f'{module.name}:{ins.line}: opcode {op:02x} has no Lua form')
        name, inline_kind, kinds, form = SPEC[op]
        args = []
        if inline_kind == 'a':
            if ins.inline > 255:
                statistics['inline_ignored'] += 1
            args.append(str(ins.inline & 255))
        elif inline_kind == 'w':
            args.append(str(ins.inline))
        elif inline_kind == 's':
            args.append(str(ins.inline & 0xfff))
            args.append(str((ins.inline >> 12) & 0xfff))
        elif inline_kind == 'p':
            prefix = (ins.inline >> 8) & 255
            if prefix not in (0, 2) or ins.inline >> 16:
                raise ValueError(f'{module.name}:{ins.line}: unexpected prefix field {ins.inline:06x}')
            if op not in (0x48, 0x68):
                args.append(str(ins.inline & 255))
            elif ins.inline & 255:
                raise ValueError(f'{module.name}:{ins.line}: {name} with a sample argument')
            kinds = ['clip'] if prefix == 2 else []
        elif ins.inline != 0:
            statistics['inline_ignored'] += 1
        if len(kinds) != len(ins.operands):
            raise ValueError(f'{module.name}:{ins.line}: {name} operand count {len(ins.operands)} != {len(kinds)}')
        rendered = [render_operand(k, v, raw, symbols, targets, clip_table) for k, v, raw in zip(kinds, ins.operands, ins.raw_args)]
        if form == 'stmt':
            if op in (0x1d, 0x12) and ins.inline > 255:
                raise ValueError(f'{module.name}:{ins.line}: sample prefix bits set')
            if op == 0x6c:
                rendered = [args[0], args[1]] + rendered
                args = []
            out.append(pad + f'mk4.{name}({", ".join(args + rendered)})')
            return None
        if form == 'finish':
            out.append(pad + f'return mk4.{name}({", ".join(args + rendered)})')
            return 'end'
        if form == 'jump':
            out.append(pad + f'return {rendered[0]}()')
            return 'end'
        if form == 'branch':
            target = rendered[-1]
            call_args = args + rendered[:-1]
            out.append(pad + f'if mk4.{name}({", ".join(call_args)}) then')
            out.append(pad + f'  return {target}()')
            out.append(pad + 'end')
            return None
        if form == 'select':
            out.append(pad + 'return mk4.select_roster {')
            for roster, target in zip(ROSTER, rendered):
                out.append(pad + f'  {roster} = {target},')
            out.append(pad + '}')
            return 'end'
        if form == 'loop':
            count = ins.inline & 255
            if count == 0:
                raise ValueError(f'{module.name}:{ins.line}: zero loop counter')
            loop_stack.append(count)
            out.append(pad + f'for _ = 1, {count} do')
            return 'loop'
        if form == 'repeat':
            if not loop_stack:
                raise ValueError(f'{module.name}:{ins.line}: repeat without a loop in the same stream')
            loop_stack.pop()
            out.append('  ' * (depth - 1) + 'end')
            return 'repeat'
        raise ValueError(form)

    for address in sorted(entries):
        ins = by_address.get(address)
        if ins is None:
            raise ValueError(f'entry {entries[address]} at {address:08x} is not an instruction')
        emit_function(modules[ins.module], entries[address], address)
    emitted = statistics['instructions']
    if emitted != len(instructions):
        raise ValueError(f'{len(instructions) - emitted} instructions are reachable from no entry or were emitted twice')
    for alias, target in alias_targets.items():
        module = modules[by_address[symbols[target]].module] if symbols[target] in by_address else None
        if module is None:
            raise ValueError(f'alias {alias} continues into a non-instruction')
        module.out.append('')
        module.out.append(f'-- Entered one word into the stream above; the wordcode skips a no-op word.')
        module.out.append(f'function S.{alias}()')
        module.out.append(f'  return S.{target}()')
        module.out.append('end')

    files = {}
    for name in module_order:
        module = modules[name]
        source_comment = ''
        asm = project.parent / 'code' / (name + '.asm')
        for raw in asm.read_text().splitlines() if asm.exists() else []:
            if raw.strip().startswith(';'):
                source_comment = raw.strip()[1:].strip()
                break
        header = [f'-- {source_comment}' if source_comment else f'-- {name}',
                  f'-- Translated from scripts/mk4/code/{name}.asm by tools/mk4_asm_to_lua.py; edit as Lua.',
                  '-- Operations: scripts/lua/README.md. Natives use their stable IDs (scripts/mk4/native.inc).',
                  'local mk4 = mk4',
                  'local native, labels = mk4.native_ids, mk4.labels',
                  'local S = mk4.module()']
        files[name + '.lua'] = '\n'.join(header + module.out + ['', 'return S', ''])
    program = ['-- Script modules in link order. Each returns the functions it defines.', 'return {']
    program += [f'  "{name}.lua",' for name in module_order]
    program += ['}', '']
    files['program.lua'] = '\n'.join(program)

    output_dir = Path(output_dir)
    changed = []
    for filename, text in files.items():
        path = output_dir / filename
        if check:
            if not path.exists() or path.read_text() != text:
                changed.append(str(path))
        else:
            output_dir.mkdir(parents=True, exist_ok=True)
            if not path.exists() or path.read_text() != text:
                path.write_text(text)
                changed.append(str(path))
    return statistics, changed


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('project', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--check', action='store_true', help='compare instead of writing')
    args = parser.parse_args()
    try:
        statistics, changed = translate(args.project, args.output, args.check)
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, str(exc) + '\n')
    print(f'{statistics["functions"]} functions, {statistics["instructions"]} instructions, '
          f'{statistics["inline_ignored"]} ignored inline fields')
    if args.check:
        if changed:
            parser.exit(1, 'Lua modules differ from the assembly translation:\n  ' + '\n  '.join(changed) + '\n')
        print('Lua modules match the assembly translation')
    elif changed:
        print('wrote ' + ', '.join(changed))


if __name__ == '__main__':
    main()
