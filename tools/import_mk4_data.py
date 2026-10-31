#!/usr/bin/env python3
"""Export MK4 tables/wordcode for research, or regenerate named fighter bytecode.

The original fixup records identify packed references. Raw asset references
are decoded through the rig/slot/descriptor/name layouts used by our loader.
Numeric lookalikes are never inferred to be pointers from their magnitude.
"""
import argparse
import json
from pathlib import Path
import re
import struct
from retail_moves import RetailImage
from scorpion_moves import script_program
from mk4_asm import NAMES
from mk4_link import link, BASE, HEADER

ROOT = Path(__file__).resolve().parents[1]
LO, HI = 0x4ce000, 0x580000
LAYOUT = Path(__file__).with_name('mk4_import_layout.json')
SCRIPT_LAYOUT = Path(__file__).with_name('mk4_script_layout.json')


def decode_scripts(image, roots):
    """Decode the shared corpus once, retaining instruction ownership for aliases."""
    rows = {}
    for root in roots:
        for character in range(16):
            for row in script_program(image, root, character)['instructions']:
                if 'operands' not in row:
                    raise ValueError('Undecoded script at '+row['va'])
                rows[int(row['va'],16)] = row
    code, owners = {}, {}
    for a,row in sorted(rows.items()):
        if a in owners:
            continue
        for i,v in enumerate([row['word'],*row['operands']]):
            at=a+4*i
            if at in owners:raise ValueError('Overlapping instructions')
            owners[at]=a;code[at]=int(v,16)
    return rows, code, owners


def export_scripts(image, destination, check=False):
    """Generate named wordcode by fighter, with one shared runtime/native ABI.

    Names come from the established import metadata. Ownership is
    derived from those names and the existing per-roster script catalogs, never
    a handwritten fighter definition. Explicit jumps preserve fall-through when
    blocks move between files; embedded entries retain their operand offsets.
    """
    from bisect import bisect_right
    from character_catalog import CHARACTERS, moves_for
    from retail_moves import EXE_SHA256

    layout = json.loads(SCRIPT_LAYOUT.read_text())
    if layout['exe_sha256'] != EXE_SHA256:
        raise ValueError('Script entry map targets another retail revision')
    rows, code, owners = decode_scripts(image, json.loads(LAYOUT.read_text())['roots'])
    entries = {}
    names = {}
    for module in layout['modules'].values():
        for name, address in module['entries'].items():
            address = int(address, 16)
            if address not in owners or name in names:
                raise ValueError(f'Invalid or duplicate script entry {name} at {address:#x}')
            entries.setdefault(address, []).append(name)
            names[name] = address
    native_ids = {name: int(value, 16) for name, value in layout['natives'].items()}
    natives = {}
    for name, value in native_ids.items():
        natives.setdefault(value, name)
    addresses = sorted(a for a in rows if owners[a] == a)
    starts = sorted(a for a in entries if owners[a] == a)
    blocks = {a: [] for a in starts}
    for address in addresses:
        index = bisect_right(starts, address) - 1
        if index < 0:
            raise ValueError(f'Instruction {address:#x} has no named block')
        blocks[starts[index]].append(address)

    def named_owner(values):
        found = {fighter for fighter in CHARACTERS for name in values
                 if re.sub(r'^mk4_native_', '', name.lower()).startswith(fighter + '_')
                 or name.lower().endswith('_' + fighter)}
        return next(iter(found)) if len(found) == 1 else None

    # Catalog ownership is a fallback for generic labels, not a dispatch policy.
    users = {address: set() for address in starts}
    for character, fighter in enumerate(CHARACTERS):
        roots = {root for move in moves_for(character).values() for _, root in move['scripts']}
        for root in sorted(roots):
            for row in script_program(image, root, character)['instructions']:
                address = int(row['va'], 16)
                if address in owners:
                    block = starts[bisect_right(starts, owners[address]) - 1]
                    users[block].add(fighter)
    block_owners = {}
    for address in starts:
        block_owners[address] = named_owner(entries[address]) or (next(iter(users[address])) if len(users[address]) == 1 else 'shared')

    def target(value):
        address = value * 4
        if address not in entries:
            raise ValueError(f'Unnamed script target {address:#x}')
        return f'word({entries[address][0]})'

    def operand(row, index):
        value = int(row['operands'][index], 16)
        op = int(row['opcode'], 16)
        if 'clip' in row and index == 0:
            return f'clip({row["clip"]})'
        if op in (0x2f, 0x33, 0x3c, 0x3d, 0x42, 0x44, 0x45, 0x50, 0x6f) or (op in (0x36, 0x43, 0x55) and index == 1):
            return target(value)
        if (op in (0x32, 0x35, 0x36, 0x38, 0x43, 0x55) and index == 0) or (op == 0x40 and index == 1):
            return natives.get(value, hex(value))
        return hex(value)

    files = {fighter: [f'; Generated {fighter} bytecode and native IDs. Do not edit; run make scripts-export.',
                       '; The shared C VM executes these words; native IDs select shared C services.', '']
             for fighter in (*CHARACTERS, 'shared')}
    for name, value in native_ids.items():
        owner = named_owner([name]) or 'shared'
        files[owner].append(f'.export_equ {name} {value:#x}')
    aliases = {}
    for address, labels in entries.items():
        owner = owners[address]
        if owner != address:
            block = starts[bisect_right(starts, owner) - 1]
            for name in labels:
                aliases[name] = (block, owner, address - owner)
    bridges = 0
    for start, instructions in blocks.items():
        out = files[block_owners[start]]
        out.extend(['', f'; Retail source {start:#010x}; branches use linked names.'])
        for name in entries[start]:
            out.append(name + ':')
        for address in instructions:
            row = rows[address]
            args = [hex(row['inline'])] + [operand(row, n) for n in range(len(row['operands']))]
            out.append('  ' + NAMES[int(row['opcode'], 16)] + ' ' + ', '.join(args))
        last = instructions[-1]
        after = last + 4 * (1 + len(rows[last]['operands']))
        # Preserve contiguous retail words within a file. Only a cross-file
        # continuation needs a jump, including an entry inside selector operands.
        embedded = any(last < address < after for address in entries)
        adjacent = after in blocks and block_owners[after] == block_owners[start]
        if after in blocks and not adjacent and (embedded or int(rows[last]['opcode'], 16) not in (0x1b, 0x30, 0x33, 0x3c, 0x3f)):
            out.append('  jump 0, ' + target(after // 4))
            bridges += 1
        if not adjacent:
            out.append('  .zero 4 ; non-executable guard')
        for name, (block, owner, offset) in aliases.items():
            if block == start:
                # The owner may be an instruction inside this named block.
                out.append(f'.alias {name} {entries[start][0]} + {owner - start + offset}')
    rendered = {f'fighters/{fighter}.asm': '\n'.join(files[fighter]) + '\n' for fighter in CHARACTERS}
    rendered['fighters/shared.asm'] = '\n'.join(files['shared']) + '\n'
    program = ['; Generated script link order. Do not edit; run make scripts-export.', '.section script']
    program += [f'.include "../../fighters/{fighter}.asm"' for fighter in (*CHARACTERS, 'shared')]
    rendered['scripts/mk4/program.asm'] = '\n'.join(program) + '\n'
    # This include is kept as the shared ABI entry point for the project.
    rendered['scripts/mk4/native.inc'] = '; Native IDs are declared/exported beside bytecode in fighters/*.asm.\n'
    opcodes = ['/* Generated from tools/mk4_asm.py. Do not edit; run make scripts-export. */',
               '#ifndef MK4_SCRIPT_OPCODES_H', '#define MK4_SCRIPT_OPCODES_H', '', 'typedef enum ScriptOpcode {']
    opcodes += [f'  MK4_OP_{name.upper()} = 0x{value:02x},' for value, name in enumerate(NAMES)]
    opcodes += ['} ScriptOpcode;', '', '#endif', '']
    rendered['game/script/opcodes.h'] = '\n'.join(opcodes)
    destination = Path(destination)
    changed = []
    for filename, text in rendered.items():
        path = destination / filename
        if not path.exists() or path.read_text() != text:
            changed.append(str(path))
            if not check:
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
    return {'entries': len(names), 'instructions': len(addresses), 'bridges': bridges}, changed


def verify_scripts(image, project):
    """Compare every original instruction word/typed operand after relocation.

    This checks the linked result against the executable, independently of
    source formatting and file ownership. Extra cross-block jumps are checked
    separately; they may only express the original adjacent fall-through.
    """
    from bisect import bisect_right
    layout = json.loads(SCRIPT_LAYOUT.read_text())
    rows, code, owners = decode_scripts(image, json.loads(LAYOUT.read_text())['roots'])
    entries = {}
    for module in layout['modules'].values():
        for name, value in module['entries'].items():
            entries.setdefault(int(value, 16), []).append(name)
    starts = sorted(address for address in entries if owners[address] == address)
    package, _, symbols = link(project)
    payload = package[HEADER.size:]
    for address, expected in code.items():
        owner = owners[address]
        start = starts[bisect_right(starts, owner) - 1]
        linked = symbols[entries[start][0]] + address - start
        index = (address - owner) // 4 - 1
        if index >= 0:
            row = rows[owner]
            op = int(row['opcode'], 16)
            if 'clip' in row and index == 0:
                expected = (symbols['animation_table'] + 12 * row['clip']) // 4
            elif op in (0x2f, 0x33, 0x3c, 0x3d, 0x42, 0x44, 0x45, 0x50, 0x6f) or (op in (0x36, 0x43, 0x55) and index == 1):
                expected = symbols[entries[expected * 4][0]] // 4
        actual = struct.unpack_from('<I', payload, linked - BASE)[0]
        if actual != expected:
            raise ValueError(f'Retail word mismatch at {address:#x} ({entries[start][0]}): {actual:#x} != {expected:#x}')
    for address, names in entries.items():
        start = starts[bisect_right(starts, owners[address]) - 1]
        expected = symbols[entries[start][0]] + address - start
        if any(symbols[name] != expected for name in names):
            raise ValueError(f'Embedded entry relocation mismatch at {address:#x}')
    for i, start in enumerate(starts):
        following = starts[i + 1] if i + 1 < len(starts) else HI
        instructions = [address for address in rows if owners[address] == address and start <= address < following]
        last = max(instructions)
        after = last + 4 * (1 + len(rows[last]['operands']))
        embedded = any(last < address < after for address in entries)
        if after in entries and owners[after] == after and (embedded or int(rows[last]['opcode'], 16) not in (0x1b, 0x30, 0x33, 0x3c, 0x3f)):
            linked = symbols[entries[start][0]] + after - start
            if linked == symbols[entries[after][0]]:
                continue  # The original words remain adjacent in one file.
            bridge = struct.unpack_from('<II', payload, linked - BASE)
            if bridge != (0x33000000, symbols[entries[after][0]] // 4):
                raise ValueError(f'Fall-through changed after {entries[start][0]}')
    return len(code)


def export(image, destination):
    layout = json.loads(LAYOUT.read_text())
    names = {int(a,16):name for a,name in layout['names'].items()}
    used = set()
    for address, name in sorted(names.items()):
        if name in used:
            name = name + "_" + str(len(used))
            names[address] = name
        used.add(name)
    rows, code, owners = decode_scripts(image, layout['roots'])
    # Typed relocation sites. Packed pointers come directly from original fixups.
    refs = {a:('word',value*4) for a,value in image.fixups.items() if value and a not in code}
    def raw(at):
        target=image.word(at)
        if target:
            if not LO<=target<HI:raise ValueError(f'Bad raw data reference {at:x}: {target:x}')
            refs[at]=('byte',target)
        return target
    def asset(slot):
        descriptor=raw(slot)
        if descriptor:
            raw(descriptor)
    seen=set()
    def rig(at,depth=0):
        if depth>32:raise ValueError('Rig depth')
        flags=image.word(at)
        slot=raw(at+4);asset(slot)
        # The optional matrix reference is data, even though our pose builder
        # computes matrices itself instead of consuming this original field.
        raw(at+8)
        seen.add(at)
        cursor=at+28
        if not flags&8:
            while True:
                child=image.word(cursor)
                cursor=rig(cursor,depth+1)
                if child&16:break
        return cursor
    roots=set()
    for i in range(18):
        rec=0x4eb2e8+i*32
        roots.update([image.word(rec+12)*4,image.word(rec+20)*4])
        asset(image.word(rec)*4)
        raw(0x4ee1f0+i*4)
    for i in range(11):roots.add(image.word(0x4dd960+i*64+12)*4)
    roots.update(layout['rig_roots'])
    for descriptor in layout['weapon_descriptors']:
        roots.add(image.word(descriptor + 4) * 4)
    for i in range(18):
        inventory=image.word(0x53d5c0+i*4)*4
        if inventory:roots.add(image.word(inventory+4)*4)
    for i in range(14):roots.add(image.word(0x4ea424+i*12)*4)
    for at in sorted(roots):
        if at:rig(at)
    # Instruction references are typed by opcode, never by the numeric value.
    for a,row in rows.items():
        if owners[a]!=a:continue
        op=int(row['opcode'],16)
        for i,v in enumerate(row['operands']):
            value=int(v,16);at=a+4*(i+1)
            packed=(op in (0x2f,0x33,0x3c,0x3d,0x42,0x44,0x45,0x50,0x6f) or
                    (op in (0x36,0x43,0x55) and i==1) or ('clip' in row and i==0))
            if packed and value:refs[at]=('word',value*4)
    # Native identifiers retain their stable ABI values. They are not data addresses.
    native={int(value, 16): name for name, value in json.loads(SCRIPT_LAYOUT.read_text())['natives'].items()}
    for at,(_,target) in refs.items():
        if not LO<=target<HI:raise ValueError(f'Reference out of data: {at:x} -> {target:x}')
        names.setdefault(target,('script_' if target in code else 'data_')+format(target,'06x'))
    for at in rows:names.setdefault(at,'script_'+format(at,'06x'))
    # A reference may name a byte inside a string or an operand inside an instruction.
    aliases=[]
    for at,name in sorted(list(names.items())):
        if at in owners and owners[at]!=at:
            parent=owners[at];names.setdefault(parent,'script_'+format(parent,'06x'))
            aliases.append(f'.alias {name} {names[parent]} + {at-parent}')
        elif at%4:
            parent=at//4*4;names.setdefault(parent,'data_'+format(parent,'06x'))
            aliases.append(f'.alias {name} {names[parent]} + {at-parent}')
    def value(at):
        if at in refs:
            kind,target=refs[at];label=names[target]
            return f'word({label})' if kind=='word' else label
        v=image.word(at)
        return native.get(v,hex(v))
    data=['; MK4 source data. Labels and references are linked, not image addresses.','.section data']
    run=[]
    def flush():
        if run:data.append('  .word '+', '.join(run));run.clear()
    a=LO
    while a<HI:
        if a in names and a not in code:
            flush();data.append(names[a]+':')
        # Original fixup tables and extracted instruction storage are no longer
        # runtime data. Retain only anonymous padding between surrounding tables.
        if a in code or 0x4d3e80<=a<0x4d9d60 or image.word(a)==0:
            flush();start=a;a+=4
            while a<HI and (a not in names or a in code) and (a in code or 0x4d3e80<=a<0x4d9d60 or image.word(a)==0):a+=4
            data.append(f'  .zero {a-start}')
            continue
        run.append(value(a));a+=4
        if len(run)==6:flush()
    flush()
    if HI in names:data.append(names[HI]+':')
    program=['; Editable MK4 wordcode. Insert/remove instructions freely; references follow labels.','.section script']
    previous=None
    for a,row in sorted(rows.items()):
        if owners[a]!=a:continue
        if previous is not None and a!=previous:program+=['  .zero 4',''] # reject accidental fallthrough between independent blocks
        program.append(names[a]+':')
        program.append('  '+NAMES[int(row['opcode'],16)]+' '+', '.join([hex(row['inline'])]+[value(a+4*(i+1)) for i in range(len(row['operands']))]))
        previous=a+4*(1+len(row['operands']))
    destination.mkdir(parents=True)
    (destination/'native.inc').write_text('; Stable native ABI identifiers; these are never host pointers.\n'+''.join(f'.equ {name} {v:#x}\n' for v,name in sorted(native.items())))
    (destination/'tables.asm').write_text('\n'.join(data + aliases)+'\n')
    (destination/'program.asm').write_text('\n'.join(program)+'\n')
    (destination/'project.asm').write_text('.include "native.inc"\n.include "tables.asm"\n.include "program.asm"\n')
    package,header,symbols=link(destination/'project.asm')
    # Verify every retained byte, every typed reference and every instruction.
    payload=package[HEADER.size:HEADER.size+struct.unpack_from('<I',package,8)[0]]
    for a,v in code.items():
        owner=owners[a];linked=symbols[names[owner]]+a-owner
        expected=v
        if a in refs:
            kind,target=refs[a];expected=symbols[names[target]]//(4 if kind=='word' else 1)
        assert struct.unpack_from('<I',payload,linked-BASE)[0]==expected,hex(a)
    for a in range(LO,HI,4):
        if a in code or 0x4d3e80<=a<0x4d9d60:continue
        expected=image.word(a)
        if a in refs:
            kind,target=refs[a];expected=symbols[names[target]]//(4 if kind=='word' else 1)
        assert struct.unpack_from('<I',payload,a-LO)[0]==expected,hex(a)
    print(f'Imported {len(code)} script words, {len(refs)} typed references, {len(seen)} rig nodes; all retained words verified.')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--exe', type=Path, default=ROOT / 'MK4/MK-4.EXE')
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--scripts', action='store_true', help='generate named bytecode under fighters/ instead of the flat research import')
    p.add_argument('--check', action='store_true', help='compare generated bytecode without writing (requires --scripts)')
    args = p.parse_args()
    if args.check and not args.scripts:
        p.error('--check requires --scripts')
    if not args.scripts and args.output.exists():
        p.error('Destination exists; import into a new directory and review')
    try:
        image = RetailImage(args.exe)
        if args.scripts:
            statistics, changed = export_scripts(image, args.output, args.check)
            print(f'{statistics["entries"]} named entries, {statistics["instructions"]} retail instructions, '
                  f'{statistics["bridges"]} explicit fall-through jumps.')
            if args.check and changed:
                p.exit(1, 'Bytecode differs from the retail export:\n  ' + '\n  '.join(changed) + '\n')
            print('Bytecode matches the retail export.' if args.check else f'Bytecode written under {args.output}.')
        else:
            export(image, args.output)
    except (OSError, ValueError, KeyError) as exc:
        p.exit(1, str(exc) + '\n')


if __name__ == '__main__':
    main()
