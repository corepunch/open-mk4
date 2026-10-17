#!/usr/bin/env python3
"""Inspect supplied retail move scripts, contacts and exact-entry native code.

Queries print bounded reports; full bundle generation is explicit. Native
queries and bundles require Capstone 5. No retail code is executed. Examples:
  python3 tools/disassemble_retail.py --script 0x4e6d08
  python3 tools/disassemble_retail.py --bundle --output build/retail-disassembly
"""
import argparse
from collections import defaultdict, deque
import json
from pathlib import Path
import struct

from retail_moves import DESCRIPTORS, EXE_SHA256, RetailImage, clip_inventory
from scorpion_moves import SCRIPT_OPS, VM_HANDLER_COUNT, inventory, script_program
from retail_symbols import NAMES, reference_rows, native_name

GLOBALS = {
    0x4d07ac: 'scratch_stack_top (packed word index)',
    0x53c420: 'current_task (packed word index)',
    0x53c41c: 'current_group (packed word index)',
    0x53c22c: 'cooperative_pause_flag',
    0x53c42c: 'VM/native argument and result scratch',
    0x53c430: 'VM instruction/argument scratch',
    0x53c44c: 'native result flags', 0x53c448: 'scratch; contact result in contact context',
    0x524efc: 'task-list head (byte address)',
    0x534858: 'input/game tick counter',
    0x53c204: 'player-one LP press latch', 0x524e48: 'player-two LP press latch',
    0x4e3988: 'VM opcode handler table', 0x4e4ec0: 'command predicate table',
    0x4e6ef8: 'action dispatch table, flag 1',
    0x4ed828: 'action dispatch table, flag 2',
    0x4e8910: 'action dispatch table, flag 4',
    0x4e7008: 'action dispatch table, flag 8',
    0x4dfc90: 'action dispatch table, flag 0x20',
    0x53d2b8: 'kick contact-record pointer table',
    0x4fad7c: 'animation descriptor bank',
}
SCRIPTS = {
    'jab': 0x4e62f0, 'jab_second': 0x4e6330, 'jab_repeat': 0x4e6360,
    'jab_recovery': 0x4e63b0, 'high_kick': 0x4e5ca0,
    'high_kick_recovery': 0x4e5c70, 'sweep': 0x4e6798,
    'uppercut': 0x4e62a0, 'spear_entry': 0x4ed448,
    'spear_return': 0x4ed4a8, 'breathe_fire': 0x4e5b58,
    'teleport_entry': 0x4e87b0, 'teleport_landing': 0x4e8818,
    'weapon_draw': 0x4e62c0, 'weapon_draw_return': 0x4e62d8,
}


class Disassembly:
    def __init__(self, path):
        import capstone
        self.cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.cs.detail = True
        self.image = RetailImage(path)
        self.raw = Path(path).read_bytes()
        pe = struct.unpack_from('<I', self.raw, 0x3c)[0]
        optional = struct.unpack_from('<H', self.raw, pe + 20)[0]
        self.sections = []
        for index in range(struct.unpack_from('<H', self.raw, pe + 6)[0]):
            at = pe + 24 + optional + index * 40
            name = self.raw[at:at + 8].rstrip(b'\0').decode('ascii')
            _, rva, size, offset = struct.unpack_from('<IIII', self.raw, at + 8)
            flags = struct.unpack_from('<I', self.raw, at + 36)[0]
            self.sections.append(dict(name=name, base=0x400000 + rva, size=size,
                                      offset=offset, executable=bool(flags & 0x20000000)))
        self.entries = set(NAMES)
        self.decoded = {}
        self.names = dict(NAMES)
        self.programs = {name: script_program(self.image, root) for name, root in SCRIPTS.items()}
        self.script_sources = defaultdict(list)
        self.script_errors = {}
        self.retail = inventory(self.image)
        for root in self.retail['entry_points']:
            self.names[int(root['native'], 16)] = 'scorpion_' + root['name']
        self.entries.update(self.names)
        self.imports = self.read_imports(pe + 24)

    def read_imports(self, optional):
        result = {}
        rva, = struct.unpack_from('<I', self.raw, optional + 104)
        if not rva:
            return result
        address = 0x400000 + rva

        def string(va):
            data = self.raw_bytes(va, 256).split(b'\0', 1)[0]
            return data.decode('ascii', 'replace')

        while True:
            words = struct.unpack('<IIIII', self.raw_bytes(address, 20))
            original, _, _, name, thunk = words
            if not any(words):
                break
            dll = string(0x400000 + name)
            index = 0
            while True:
                value = self.image.word(0x400000 + (original or thunk) + index * 4)
                if not value:
                    break
                symbol = '#%d' % (value & 0xffff) if value & 0x80000000 else string(0x400002 + value)
                result[0x400000 + thunk + index * 4] = dll + '!' + symbol
                index += 1
            address += 20
        return result

    def code(self, address):
        return any(section['executable'] and section['base'] <= address < section['base'] + section['size']
                   for section in self.sections)

    def raw_bytes(self, address, size):
        for section in self.sections:
            if section['base'] <= address < section['base'] + section['size']:
                size = min(size, section['base'] + section['size'] - address)
                offset = section['offset'] + address - section['base']
                return self.raw[offset:offset + size]
        return b''

    def instruction(self, address):
        if address not in self.decoded:
            self.decoded[address] = next(self.cs.disasm(self.raw_bytes(address, 15), address, count=1), None)
        return self.decoded[address]

    def direct(self, insn):
        from capstone.x86_const import X86_OP_IMM
        return insn.operands[0].imm & 0xffffffff if insn.operands and insn.operands[0].type == X86_OP_IMM else None

    def discover_entries(self):
        """Direct CALL destinations are entry candidates, not recovered symbols.

        A linear pass only proposes boundaries. Reachable routines below are
        decoded again from exact entry/basic-block addresses, never by slicing
        this pass (which can decode embedded jump-table data as instructions).
        """
        self.cs.skipdata = True
        for section in self.sections:
            if not section['executable']:
                continue
            payload = self.raw_bytes(section['base'], section['size'])
            for insn in self.cs.disasm(payload, section['base']):
                if insn.mnemonic == 'call':
                    target = self.direct(insn)
                    if target is not None and self.code(target):
                        self.entries.add(target)
        self.cs.skipdata = False
        for program in self.programs.values():
            for row in program['instructions']:
                for key in ('native_target', 'callback', 'handler'):
                    if row.get(key) and self.code(int(row[key], 16)):
                        self.entries.add(int(row[key], 16))
        # Function pointers in the VM/predicate/action tables close indirect
        # dispatch edges without interpreting arbitrary words as code.
        self.seed_tables = [(0x4e3988, VM_HANDLER_COUNT), (0x4e4ec0, 0x34),
                            (0x4e6ef8, 0x44), (0x4ed828, 0x10),
                            (0x4e8910, 0x10), (0x4e7008, 0x10)]
        self.table_targets = set()
        for base, count in self.seed_tables:
            for index in range(count):
                value = self.image.word(base + index * 4)
                if self.code(value):
                    self.table_targets.add(value)
                    self.entries.add(value)

    def switch(self, insn, history):
        """Follow only a bounded, absolute indexed JMP table.

        The index CMP must have an unsigned JA guard before this dispatch.
        Other computed dispatches remain unresolved and keep their expression.
        """
        from capstone.x86_const import X86_OP_MEM, X86_OP_REG, X86_OP_IMM
        if not insn.operands or insn.operands[0].type != X86_OP_MEM:
            return None
        mem = insn.operands[0].mem
        if mem.base or not mem.index or mem.scale != 4 or not mem.disp:
            return None
        for offset in range(len(history) - 1, -1, -1):
            cmp = history[offset]
            if (cmp.mnemonic == 'cmp' and len(cmp.operands) == 2 and
                    cmp.operands[0].type == X86_OP_REG and cmp.operands[0].reg == mem.index and
                    cmp.operands[1].type == X86_OP_IMM and 0 <= cmp.operands[1].imm < 256):
                guards = [row for row in history[offset + 1:] if row.mnemonic == 'ja']
                if not guards or any(row.mnemonic == 'call' for row in history[offset + 1:]):
                    return None
                guard = guards[0]
                guard_index = history.index(guard)
                following = history[guard_index + 1].address if guard_index + 1 < len(history) else insn.address
                if following != guard.address + guard.size:
                    return None
                aliases = {'eax': {'eax', 'ax', 'al', 'ah'}, 'ecx': {'ecx', 'cx', 'cl', 'ch'},
                           'edx': {'edx', 'dx', 'dl', 'dh'}, 'ebx': {'ebx', 'bx', 'bl', 'bh'}}
                index_names = aliases.get(insn.reg_name(mem.index), {insn.reg_name(mem.index)})
                for row in history[offset + 1:]:
                    _, written = row.regs_access()
                    if any(row.reg_name(register) in index_names for register in written):
                        return None
                    if row.address < guard.address and any(row.reg_name(register) == 'eflags' for register in written):
                        return None
                count = cmp.operands[1].imm + 1
                try:
                    targets = [self.image.word(mem.disp + index * 4) for index in range(count)]
                except ValueError:
                    return None
                if all(self.code(target) for target in targets):
                    return dict(va=mem.disp, count=count, targets=targets,
                                compare=cmp.address, guard=guards[0].address)
                return None
        return None

    def function(self, entry, limit=8192):
        from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
        pending = deque([(entry, [])])
        rows, edges, tables, unresolved, refs, literals, scripts = {}, set(), {}, [], set(), set(), []
        while pending:
            pc, history = pending.popleft()
            while pc not in rows:
                if len(rows) >= limit:
                    unresolved.append(dict(at=pc, reason='instruction budget reached'))
                    pending.clear()
                    break
                if not self.code(pc):
                    unresolved.append(dict(at=pc, reason='control flow left executable section'))
                    break
                if pc != entry and pc in self.entries:
                    edges.add(('tail_or_shared_entry', pc))
                    break
                insn = self.instruction(pc)
                if insn is None or insn.mnemonic in ('int3', 'ud2'):
                    unresolved.append(dict(at=pc, reason='invalid instruction or trap/padding'))
                    break
                rows[pc] = insn
                history = (history + [insn])[-32:]
                # Literal writes into callback slots are entry references;
                # arbitrary immediate code-range constants are not guessed.
                if (insn.mnemonic == 'mov' and len(insn.operands) == 2 and
                        insn.operands[0].type == X86_OP_MEM and
                        insn.operands[0].mem.disp in (8, 0x44) and
                        insn.operands[1].type == X86_OP_IMM and self.code(insn.operands[1].imm)):
                    target = insn.operands[1].imm
                    edges.add(('callback_store', target))
                    self.entries.add(target)
                for operand in insn.operands:
                    if operand.type == X86_OP_MEM and not operand.mem.base and not operand.mem.index:
                        address = operand.mem.disp & 0xffffffff
                        if 0x400000 <= address < 0x580000:
                            refs.add(address)
                    if operand.type == X86_OP_IMM and 0x4d0000 <= operand.imm < 0x580000:
                        literals.add(operand.imm)
                following = pc + insn.size
                if insn.mnemonic.startswith('ret') or insn.mnemonic in ('iret', 'iretd'):
                    break
                if insn.mnemonic == 'call':
                    target = self.direct(insn)
                    if target is not None:
                        edges.add(('call', target))
                        if self.code(target):
                            self.entries.add(target)
                        if target == 0x459490:
                            # Exact direct C-call pattern: one constant pushed
                            # argument immediately preceding script_enter.
                            argument = history[-2] if len(history) > 1 else None
                            if argument and argument.mnemonic == 'push':
                                script = self.direct(argument)
                                if script is not None and 0x4d0000 <= script < 0x580000:
                                    scripts.append((pc, script))
                    else:
                        operand = insn.operands[0]
                        iat = (operand.mem.disp if operand.type == X86_OP_MEM and
                               not operand.mem.base and not operand.mem.index else None)
                        if iat in self.imports:
                            edges.add(('import_call', iat))
                        else:
                            unresolved.append(dict(at=pc, reason='indirect call', expression=insn.op_str))
                elif insn.mnemonic == 'jmp':
                    target = self.direct(insn)
                    if target is not None:
                        if target in self.entries and target != entry:
                            edges.add(('tail_or_shared_entry', target))
                        else:
                            pending.append((target, history))
                    else:
                        operand = insn.operands[0]
                        iat = (operand.mem.disp if operand.type == X86_OP_MEM and
                               not operand.mem.base and not operand.mem.index else None)
                        if iat in self.imports:
                            edges.add(('import_tail', iat))
                            break
                        table = self.switch(insn, history[:-1])
                        if table:
                            tables[table['va']] = table
                            for target in table['targets']:
                                pending.append((target, []))
                        else:
                            unresolved.append(dict(at=pc, reason='indirect jump', expression=insn.op_str))
                    break
                elif insn.mnemonic.startswith('j') or insn.mnemonic.startswith('loop'):
                    target = self.direct(insn)
                    if target is not None:
                        pending.append((target, history))
                pc = following
        return dict(entry=entry, instructions=rows, edges=sorted(edges), tables=list(tables.values()),
                    unresolved=unresolved, data_refs=sorted(refs), literal_refs=sorted(literals), scripts=scripts)

    def closure(self):
        self.discover_entries()
        roots = set(self.names) | self.table_targets
        for command_list in self.retail['command_records']:
            for command in command_list['records']:
                for key in ('target', 'predicate'):
                    if command[key]:
                        roots.add(int(command[key], 16))
        for program in self.programs.values():
            for row in program['instructions']:
                roots.update(int(row[key], 16) for key in ('handler', 'native_target', 'callback') if row.get(key))
        functions, pending = {}, deque(sorted(roots))
        while pending:
            entry = pending.popleft()
            if entry in functions or not self.code(entry):
                continue
            function = self.function(entry)
            functions[entry] = function
            for site, script in function['scripts']:
                self.script_sources[script].append(dict(routine=entry, at=site))
                if any(int(program['entry'], 16) == script for program in self.programs.values()):
                    continue
                if script in self.script_errors:
                    continue
                try:
                    program = script_program(self.image, script)
                except ValueError as error:
                    self.script_errors[script] = str(error)
                    continue
                self.programs['script_%08x' % script] = program
                for row in program['instructions']:
                    for key in ('handler', 'native_target', 'callback'):
                        if row.get(key):
                            target = int(row[key], 16)
                            if self.code(target):
                                self.entries.add(target)
                                pending.append(target)
            for _, target in function['edges']:
                if self.code(target) and target not in functions:
                    pending.append(target)
        return functions

    def label(self, address):
        return self.names.get(address, self.imports.get(address, native_name(address)))

    def native_link(self, address, prefix=''):
        return '[%s (`%08x`)](%snative/%08x.md)' % (self.label(address), address, prefix, address)

    def function_markdown(self, function, functions, callers):
        entry = function['entry']
        rows = ['# %s — `%08x`' % (self.label(entry), entry), '',
                'Research alias; original source name and C signature are unknown.', '',
                'Incoming references: ' + (', '.join(self.native_link(value, '../') for value in sorted(callers[entry])) or 'seed entry'), '',
                'Outgoing native references:', '']
        for kind, target in function['edges']:
            link = (self.native_link(target, '../') if target in functions else
                    '`%s` (IAT `%08x`)' % (self.imports[target], target) if target in self.imports else
                    '`%08x` (outside decoded closure)' % target)
            rows.append('- %s: %s' % (kind, link))
        rows += ['', '```asm']
        previous = None
        labels = {target for insn in function['instructions'].values()
                  if insn.mnemonic.startswith('j') and (target := self.direct(insn)) is not None}
        labels.update(target for table in function['tables'] for target in table['targets'])
        for address, insn in sorted(function['instructions'].items()):
            if address != previous or address in labels:
                rows.append('L_%08x:' % address)
            comment = []
            if insn.mnemonic in ('call', 'jmp'):
                target = self.direct(insn)
                if target is not None:
                    comment.append(self.label(target) if target in self.entries else 'L_%08x' % target)
            for global_address, alias in (GLOBALS | self.imports).items():
                if '0x%x' % global_address in insn.op_str:
                    comment.append(alias)
            rows.append('  %08x  %-30s %-8s %s%s' %
                        (address, insn.bytes.hex(' '), insn.mnemonic, insn.op_str,
                         ' ; ' + '; '.join(comment) if comment else ''))
            previous = address + insn.size
        rows += ['```', '', 'Bounded native switch tables:', '']
        for table in function['tables']:
            rows += ['- `%08x`: %d entries; CMP `%08x`, unsigned guard `%08x`.' %
                     (table['va'], table['count'], table['compare'], table['guard'])]
            for index, target in enumerate(table['targets']):
                rows.append('  - `%d` → `%08x` (%s)' %
                            (index, target, self.label(target) if target in self.entries else 'local block'))
        rows += ['', 'Unresolved control flow:', '']
        for row in function['unresolved']:
            rows.append('- `%08x`: %s%s.' % (row['at'], row['reason'],
                                            ' `' + row['expression'] + '`' if 'expression' in row else ''))
        if not function['unresolved']:
            rows.append('None encountered in this routine; this does not establish a whole-move closure.')
        rows += ['', 'Absolute data references (post-startup pointer fixups):', '',
                 '| Address | Word | Research alias |', '| --- | --- | --- |']
        for address in function['data_refs']:
            try:
                value = '%08x' % self.image.word(address)
            except ValueError:
                value = 'unavailable'
            rows.append('| `%08x` | `%s` | %s |' % (address, value, GLOBALS.get(address, '')))
        rows += ['', 'Data-range immediate literals (candidate pointers; no type inferred):', '']
        for address in function['literal_refs']:
            values = [self.image.word(address + index * 4) for index in range(8)
                      if address + index * 4 + 4 <= 0x580000]
            rows.append('- `%08x`: `%s`' % (address, ' '.join('%08x' % value for value in values)))
        rows += ['', 'Direct script entry references:', '']
        for site, script in function['scripts']:
            name = next((name for name, program in self.programs.items() if int(program['entry'], 16) == script), None)
            rows.append('- `%08x` → %s' %
                        (site, '[%08x](../scripts/%s.md)' % (script, name) if name else '`%08x` (decode failed)' % script))
        rows += reference_rows([entry, *(target for _, target in function['edges'])])
        return '\n'.join(rows) + '\n'

    def script_markdown(self, name, program, functions):
        rows = ['# Script %s — `%s`' % (name, program['entry']), '',
                'Binary VM words from the EXE after startup pointer fixups. Descriptive names are research annotations.', '',
                '```text']
        for row in program['instructions']:
            extra = []
            if 'clip' in row:
                extra.append('clip %d, stored samples 0..%d' % (row['clip'], row['stored_count'] - 1))
            if 'start_sample' in row:
                extra.append('start %d' % row['start_sample'])
            for key in ('native_target', 'callback', 'branch'):
                if key in row:
                    address = int(row[key], 16)
                    extra.append('%s %08x%s' % (key, address, ' ' + self.label(address) if address in functions else ''))
            rows.append('%08x  %-10s  %-48s inline=%s%s' %
                        (int(row['va'], 16), row['word'], row['operation'], hex(row.get('inline', 0)),
                         ' ; ' + '; '.join(extra) if extra else ''))
            for index, operand in enumerate(row.get('operands', [])):
                address = int(row['va'], 16) + (index + 1) * 4
                fixed = self.image.word(address)
                raw = struct.unpack('<I', self.raw_bytes(address, 4))[0]
                rows.append('  %08x  %08x%s' % (address, fixed, ' ; raw %08x, startup fixup' % raw if fixed != raw else ''))
        rows += ['```', '', 'Native handlers and dependencies:', '']
        dependencies = sorted({int(row[key], 16) for row in program['instructions']
                               for key in ('handler', 'native_target', 'callback') if row.get(key)})
        for target in dependencies:
            rows.append('- ' + (self.native_link(target, '../') if target in functions else '`%08x` (not decoded)' % target))
        rows += reference_rows(dependencies)
        rows += ['', 'Open edges:', '']
        rows += ['- `%s`: %s → `%s`.' % (edge['source'], edge['kind'], edge['target'])
                 for edge in program['unresolved_native_edges']]
        return '\n'.join(rows) + '\n'

    def write(self, output, archive, full_text=True):
        output = Path(output)
        (output / 'native').mkdir(parents=True, exist_ok=True)
        (output / 'scripts').mkdir(exist_ok=True)
        functions = self.closure()
        callers = defaultdict(set)
        for entry, function in functions.items():
            for _, target in function['edges']:
                callers[target].add(entry)
        for entry, function in functions.items():
            (output / 'native' / ('%08x.md' % entry)).write_text(self.function_markdown(function, functions, callers))
        for name, program in self.programs.items():
            (output / 'scripts' / (name + '.md')).write_text(self.script_markdown(name, program, functions))
        manifest = dict(exe_sha256=EXE_SHA256, scope='Scorpion seeds, shared dispatch tables and static native closure',
                        sections=self.sections, roots={self.label(address): hex(address) for address in self.names},
                        imports={hex(address): name for address, name in self.imports.items()},
                        script_sources={hex(address): sources for address, sources in self.script_sources.items()},
                        script_errors={hex(address): error for address, error in self.script_errors.items()},
                        scripts=self.programs, scorpion=self.retail,
                        functions=[dict(entry=hex(entry), alias=self.label(entry),
                                        path='native/%08x.md' % entry, instructions=len(function['instructions']),
                                        edges=[dict(kind=kind, target=hex(target)) for kind, target in function['edges']],
                                        switch_tables=function['tables'], unresolved=function['unresolved'],
                                        data_refs=function['data_refs'], literal_refs=function['literal_refs'])
                                   for entry, function in sorted(functions.items())])
        (output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
        (output / 'clips.json').write_text(json.dumps(clip_inventory(self.image, archive), indent=2) + '\n')
        # The data fallback preserves every initialized non-executable byte,
        # including streams that stop at unknown VM opcodes or computed roots.
        with (output / 'data-words.txt').open('w') as stream:
            stream.write('# Initialized non-executable sections; words after startup fixups.\n')
            stream.write('# Candidate pointers are not automatically classified as scripts.\n')
            for section in self.sections:
                if section['executable']:
                    continue
                stream.write('# Section %s at %08x, %d bytes\n' %
                             (section['name'], section['base'], section['size']))
                for offset in range(0, section['size'], 16):
                    address = section['base'] + offset
                    data = self.raw_bytes(address, min(16, section['size'] - offset))
                    words, changes = [], []
                    for index in range(len(data) // 4):
                        at = address + index * 4
                        raw = struct.unpack_from('<I', data, index * 4)[0]
                        fixed = self.image.fixups.get(at, raw)
                        words.append('%08x' % fixed)
                        if fixed != raw:
                            changes.append('%08x raw=%08x' % (at, raw))
                    tail = data[len(data) // 4 * 4:]
                    stream.write('%08x  %s%s%s\n' %
                                 (address, ' '.join(words), ' bytes:' + tail.hex(' ') if tail else '',
                                  ' ; ' + ', '.join(changes) if changes else ''))
        self.write_index(output, functions)
        opcodes = ['# VM handler reference', '',
                   'The original VM indexes the table directly without a bounds check. '
                   'This lists the researched contiguous region through `0x4e3b60`; it is not a claim of VM validation.', '',
                   '| Opcode | Table slot | Native handler | Decoded operation | Following words |',
                   '| --- | --- | --- | --- | --- |']
        for opcode in range(VM_HANDLER_COUNT):
            slot = 0x4e3988 + opcode * 4
            handler = self.image.word(slot)
            operation, count = SCRIPT_OPS.get(opcode, ('unresearched; stop structured decode', 'unknown'))
            if count == -1:
                count = '1 only if inline byte 1 is 2; otherwise 0'
            opcodes.append('| `%02x` | `%08x` | %s | %s | %s |' %
                           (opcode, slot, self.native_link(handler) if handler in functions else '`%08x`' % handler, operation, count))
        (output / 'vm-opcodes.md').write_text('\n'.join(opcodes) + '\n')
        if full_text:
            self.cs.skipdata = True
            with (output / 'linear-code.asm').open('w') as stream:
                stream.write('; Linear recovery fallback. Embedded data may decode as instructions.\n')
                stream.write('; Use exact-entry native/*.md and unresolved edges first.\n')
                for section in self.sections:
                    if section['executable']:
                        stream.write('; Section %s at %08x\n' % (section['name'], section['base']))
                        for insn in self.cs.disasm(self.raw_bytes(section['base'], section['size']), section['base']):
                            stream.write('%08x  %-44s %-8s %s\n' %
                                         (insn.address, insn.bytes.hex(' '), insn.mnemonic, insn.op_str))
            self.cs.skipdata = False
        return manifest

    def write_index(self, output, functions):
        unresolved = sum(len(function['unresolved']) for function in functions.values())
        tables = sum(len(function['tables']) for function in functions.values())
        rows = ['# Supplied MK4 retail: Scorpion research disassembly', '',
                'EXE SHA-256: `%s`.' % EXE_SHA256, '',
                'Generated by `tools/disassemble_retail.py`. Read-only; original game code is not executed.', '',
                '**The scripts are binary bytecode in the executable.** No original text script source has been recovered here. '
                'The reference checkout contains reconstructed C and assembly, not the original script authoring files. '
                'Whether those authoring files were text, macros or another format is unknown.', '',
                '## Start here', '',
                '- [Scorpion commands, moves and native roots](scorpion.md)',
                '- [Native address index](native-index.md)',
                '- [All discovered script entries](script-index.md)',
                '- [VM opcode handlers and operand layouts](vm-opcodes.md)',
                '- [Machine-readable graph and raw operands](manifest.json)',
                '- [All 612 animation descriptors and archive ranges](clips.json)',
                '- [All initialized non-executable section words](data-words.txt), including unknown script paths.',
                '- [Unresolved indirect control flow](unresolved.md)',
                '- `linear-code.asm`: complete linear decode of executable sections as a recovery fallback.', '',
                '## Scripts', '']
        for name, entry in SCRIPTS.items():
            rows.append('- [%s](scripts/%s.md), entry `%08x`' % (name, name, entry))
        rows += ['', '## Interpretation and limits', '',
                 '- %d script entries, %d native entry graphs, %d bounded switch tables, %d unresolved native sites.' %
                 (len(self.programs), len(functions), tables, unresolved),
                 '- Addresses refer to the supplied executable, not the different upstream revision.',
                 '- Native instructions retain original bytes. Script operands retain fixed and original raw words.',
                 '- Descriptive aliases are research names; original symbols and C signatures are unknown.',
                 '- Direct calls, callback-slot writes and bounded native switch tables are followed. '
                 'Computed calls/jumps without a proven bound stay unresolved; see their original expressions.',
                 '- A branch into another known entry is labeled `tail_or_shared_entry`; '
                 'this is a navigation boundary, not a claim about original function boundaries.',
                 '- The dispatch-table seed set includes shared and other-character entries. '
                 'Reachability in this bundle is a research superset, not proof that Scorpion can execute every routine.',
                 '- Absolute data-reference words are context, not automatic structure/argument decoding.',
                 '- Direct `push script; call script_enter` references discover additional script entries. '
                 'Register-computed script pointers are not inferred. IAT calls retain imported DLL/symbol names.',
                 '- Stored clip length is not move duration. VM waits, native branches, spawned tasks, physics and '
                 'victim/scene programs jointly determine behavior.',
                 '- This disassembly does not implement or verify the remaining moves. No unresolved native routine '
                 'is replaced by a stub, guessed timing, or synthesized C.',
                 '- The linear fallback can interpret embedded data as instructions; it does not prove executable reachability.', '',
                 '## Verified context fields', '',
                 'Retail pointers are often word indices: byte address = packed value × 4. '
                 'The task/group layouts are polymorphic; the offsets below apply only in the stated fighter context.', '',
                 '| Context | Offset | Meaning |', '| --- | --- | --- |',
                 '| Fighter controller task | `+04` | tagged continuation stack (packed) |',
                 '| Task | `+08` | callback VA |',
                 '| Fighter task | `+34` | character ID |',
                 '| Fighter task | `+38 / +3c` | victim group / victim controller (packed) |',
                 '| Fighter task | `+48` | script PC (packed) |',
                 '| Fighter task | `+68 / +74` | attack selector / action |',
                 '| Task | `+84` | continuation phase/tag |',
                 '| Task header | `+d8 / +dc / +e4` | callback VA / signed 16-bit sleep / next task byte address |',
                 '| Fighter group | `+24 / +28` | clip descriptor (packed) / animation cursor |',
                 '| Fighter group | `+44` | world/physics callback VA |',
                 '| Animation descriptor | `+04 / +08` | stored count / flags and word stride |', '']
        (output / 'README.md').write_text('\n'.join(rows))
        index = ['# Native address index', '', '| Entry | Research alias | Instructions | Incoming entries |', '| --- | --- | --- | --- |']
        incoming = defaultdict(set)
        for entry, function in functions.items():
            for _, target in function['edges']:
                incoming[target].add(entry)
        for entry, function in sorted(functions.items()):
            index.append('| [`%08x`](native/%08x.md) | %s | %d | %d |' %
                         (entry, entry, self.label(entry), len(function['instructions']), len(incoming[entry])))
        (output / 'native-index.md').write_text('\n'.join(index) + '\n')
        script_index = ['# Script address index', '',
                        'Named Scorpion roots plus constant script arguments reached from the shared native closure.', '',
                        '| Entry | Name | Instructions | Unknown opcodes | Native entry callers |', '| --- | --- | --- | --- | --- |']
        for name, program in sorted(self.programs.items(), key=lambda item: int(item[1]['entry'], 16)):
            entry = int(program['entry'], 16)
            unknown = sum('unresearched' in row['operation'] for row in program['instructions'])
            references = ', '.join(self.native_link(source['routine']) for source in self.script_sources.get(entry, []))
            script_index.append('| `%08x` | [%s](scripts/%s.md) | %d | %d | %s |' %
                                (entry, name, name, len(program['instructions']), unknown, references))
        script_index += ['', 'Script roots whose structured decode failed (raw bytes remain in the EXE):', '']
        script_index += ['- `%08x`: %s' % (address, error) for address, error in sorted(self.script_errors.items())]
        script_index += ['', 'Raw words for unknown or failed streams remain address-searchable in [data-words.txt](data-words.txt).']
        (output / 'script-index.md').write_text('\n'.join(script_index) + '\n')
        unresolved_rows = ['# Unresolved native sites', '', 'These expressions remain unresolved; they are not no-ops.', '']
        for entry, function in sorted(functions.items()):
            for site in function['unresolved']:
                unresolved_rows.append('- %s at `%08x`: %s%s.' %
                                       (self.native_link(entry), site['at'], site['reason'],
                                        ' `' + site['expression'] + '`' if 'expression' in site else ''))
        (output / 'unresolved.md').write_text('\n'.join(unresolved_rows) + '\n')
        scorpion = ['# Scorpion command entry points', '',
                    'Compiled command metadata, decoded from both player tables. Window values are raw tick counts; '
                    'native predicates and state gates still apply.', '',
                    '| Button | Raw direction tokens (input order) | Window | Flags | Predicate | Target |',
                    '| --- | --- | --- | --- | --- | --- |']
        for commands in self.retail['command_records']:
            for row in commands['records']:
                target = int(row['target'], 16) if row['target'] else 0
                predicate = int(row['predicate'], 16) if row['predicate'] else 0
                scorpion.append('| %s | `%s` | %d | `%s` | %s | %s |' %
                                (commands['button'], ' '.join('%02x' % token for token in row['tokens']) or 'unconditional',
                                 row['window'], row['flags'], self.native_link(predicate) if predicate else 'none',
                                 self.native_link(target) if target else 'unresolved'))
        scorpion += ['', 'Direction tokens: `AA` back, `DD` forward, `02` down, `01` up.', '', 'Native move roots:', '']
        for root in self.retail['entry_points']:
            scorpion.append('- %s: %s; runtime coverage: %s.' %
                            (root['name'], self.native_link(int(root['native'], 16)), root['implementation']))
        (output / 'scorpion.md').write_text('\n'.join(scorpion) + '\n')


def main():
    from retail_query import address, contact_text, native_text, script_text, move_text, move_list_text
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--character', default='scorpion', help='Roster name or ID 0..17; selects scripts and command tables')
    parser.add_argument('--archive', default='MK4/FILESYS.DAT', help='Used only for --bundle')
    parser.add_argument('--output', type=Path, help='A Markdown file for queries, or a directory for --bundle')
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--script', metavar='ADDRESS', help='Decode one script; addresses are hexadecimal')
    mode.add_argument('--native', metavar='ADDRESS', help='Inspect one exact-entry native graph (requires Capstone)')
    mode.add_argument('--attack', nargs=2, metavar=('TABLE', 'SELECTOR'), help='Inspect a contact record: A/B/C/D/weapon + decimal/0x selector, or record + 0x address')
    mode.add_argument('--move', metavar='NAME', help='Compact character move page; use --list-moves for names')
    mode.add_argument('--list-moves', action='store_true', help='List curated move roots for the selected character')
    mode.add_argument('--bundle', action='store_true', help='Explicitly generate the full research bundle')
    parser.add_argument('--limit', type=int, default=80, help='Query instruction limit, 1..512 (default 80)')
    parser.add_argument('--raw', action='store_true', help='Include fixed script words or native bytes in queries')
    parser.add_argument('--c', action='store_true', help='Readable C-style pseudocode for --script or --move')
    parser.add_argument('--no-linear', action='store_true', help='For --bundle, omit the large linear recovery fallback')
    args = parser.parse_args()
    if not 1 <= args.limit <= 512:
        parser.error('--limit must be between 1 and 512')
    if args.c and (not (args.script or args.move) or args.raw):
        parser.error('--c requires --script or --move and cannot be combined with --raw')
    if not any((args.script, args.native, args.attack, args.move, args.list_moves, args.bundle)):
        parser.print_help()
        return
    try:
        from character_catalog import character_id
        character = character_id(args.character)
        if args.bundle and character != 0:
            raise ValueError('The existing bundle has Scorpion seeds; use bounded character queries instead')
        if args.bundle:
            output = args.output or Path('build/retail-disassembly')
            result = Disassembly(args.exe).write(output, args.archive, not args.no_linear)
            print('Wrote %d scripts and %d native entry graphs to %s' %
                  (len(result['scripts']), len(result['functions']), output / 'README.md'))
            return
        if args.list_moves:
            report = move_list_text(character)
        elif args.native:
            # x86 entries are byte-aligned, unlike VM/data words.
            entry = int(args.native, 16)
            report = native_text(Disassembly(args.exe), entry, args.limit, args.raw)
        else:
            image = RetailImage(args.exe)
            if args.c:
                from retail_script_c import move_c_text, script_c_text
                report = move_c_text(image, args.move, args.limit, character) if args.move else script_c_text(image, address(args.script), args.limit, character)
            elif args.move:
                report = move_text(image, args.move, args.limit, args.raw, character)
            elif args.script:
                report = script_text(image, address(args.script), args.limit, args.raw, character)
            else:
                report = contact_text(image, args.attack[0], int(args.attack[1], 0))
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(report)
            print('Wrote ' + str(args.output))
        else:
            print(report, end='')
    except (ValueError, OSError, ImportError) as error:
        parser.error(str(error))


if __name__ == '__main__':
    main()
