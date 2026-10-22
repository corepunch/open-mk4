#!/usr/bin/env python3
"""One-time import of MK4 tables and scripts into relocatable source.

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


def export(image, destination):
    layout = json.loads(LAYOUT.read_text())
    names = {int(a,16):name for a,name in layout['names'].items()}
    for row in layout['exports']:
        names.setdefault(row['address'], 'data_' + format(row['address'], '06x'))
    used = set()
    for address, name in sorted(names.items()):
        if name in used:
            name = name + "_" + str(len(used))
            names[address] = name
        used.add(name)
    rows = {}
    for root in layout['roots']:
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
    for e in layout['exports']:
        if e.get('original_name','').endswith('_RIG'):roots.add(e['address'])
        if e.get('original_name','').startswith('MK4_WEAPON_DESCRIPTOR_'):roots.add(image.word(e['address']+4)*4)
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
    native={}
    for p in (ROOT/'game/script/native_ids.h', ROOT/'game/native/actions.h'):
        for name,value in re.findall(r'\b(\w+)\s*=\s*(0x[0-9a-fA-F]+)',p.read_text()):
            v=int(value,16)
            if 0x401000<=v<LO:native.setdefault(v,name)
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
    exports=aliases[:]
    for e in layout['exports']:
        if e['address']==HI:target=names[HI]
        else:target=names[e['address']]
        exports.append(f'.export {e["name"]} {target}')
    # clip() uses this name when editing a program, independent of C export spelling.
    if names[0x4fad7c]!='MK4_DATA_ANIMATION_TABLE':exports.append('.alias MK4_DATA_ANIMATION_TABLE '+names[0x4fad7c])
    destination.mkdir(parents=True)
    (destination/'native.inc').write_text('; Stable native ABI identifiers; these are never host pointers.\n'+''.join(f'.equ {name} {v:#x}\n' for v,name in sorted(native.items())))
    (destination/'tables.asm').write_text('\n'.join(data)+'\n')
    (destination/'program.asm').write_text('\n'.join(program)+'\n')
    (destination/'exports.inc').write_text('\n'.join(exports)+'\n')
    (destination/'project.asm').write_text('.include "native.inc"\n.include "tables.asm"\n.include "program.asm"\n.include "exports.inc"\n')
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
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--exe',type=Path,default=ROOT/'MK4/MK-4.EXE')
    p.add_argument('--output',type=Path,required=True)
    args=p.parse_args()
    if args.output.exists():p.error('Destination exists; import into a new directory and review')
    export(RetailImage(args.exe),args.output)

if __name__=='__main__':main()
