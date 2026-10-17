"""Actionable helper contracts, decoded data and existing portable C sources."""
from dataclasses import dataclass
from pathlib import Path
import re
import struct

from retail_helpers import HELPERS
from retail_moves import EXE_SHA256
from retail_symbols import native_name, reference_rows
from scorpion_moves import signed_word

ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Table:
    address: int
    count: int
    stride: int
    fields: tuple  # (name, byte offset, encoding)
    note: str = ''
    header: tuple = ()  # (name, address, encoding), outside the repeated rows


TABLES = {
    'ice_spawn': Table(0x4ed5d0, 1, 16, (('rig', 0, 'packed'), ('forward', 4, 'q16'), ('dy', 8, 'q16'), ('initial_track', 12, 'packed'))),
    'ice_meshes': Table(0x4ed898, 8, 8, (('mesh', 0, 'mesh_root'), ('ticks', 0, 'duration'), ('callback', 4, 'hex')), 'shared_g.geo; spawn rig is mesh 74, track is 75..82; GEO 0x100 is additive.'),
    'ice_trail': Table(0x4d1b18, 1, 4, (('mesh', 0, 'mesh_root'), ('ticks', 0, 'duration')), '40d720 adds ten nodes; quarter scale, shrink 0x51e/tick; separate decoration clock'),
    'camera_shakes': Table(0x4e3638, 6, 12, (('pulses', 0, 'int'), ('half_cycle_ticks', 4, 'int'), ('dy', 8, 'q16'))),
    'camera_first': Table(0x4e0f04, 7, 12, (('x', 0, 'q16'), ('y', 8, 'q16'), ('z', 4, 'q16')), 'Converted from retail X,Z,Y to engine X,Y,Z; step 0xccc then hold 50 ticks', (('header_q16', 0x4e0f00, 'q16'),)),
    'camera_orbit': Table(0x4e0f5c, 25, 12, (('x', 0, 'q16'), ('y', 8, 'q16'), ('z', 4, 'q16')), 'Converted X,Z,Y; step 0x45a; target joint 3 with 80-tick transition', (('header_q16', 0x4e0f58, 'q16'),)),
    'clone_rig': Table(0x50686c, 16, 28, (('flags', 0, 'hex'), ('x', 12, 'q16'), ('y', 16, 'q16'), ('z', 20, 'q16'), ('mesh', 24, 'int')), 'shared_g.geo; root is an invisible control node; visible meshes 58..72'),
    'spine_rig': Table(0x5145bc, 5, 28, (('flags', 0, 'hex'), ('x', 12, 'q16'), ('y', 16, 'q16'), ('z', 20, 'q16'), ('mesh', 24, 'int')), 'sz_geo.geo; channels 1..5 of clip 386; head occupies channel 0; native root Z adjustment 0x13b6'),
    'weapon_descriptor': Table(0x4ecdb8, 1, 32, (('type', 0, 'hex'), ('rig', 4, 'packed'), ('grip_y', 28, 'q16'))),
    'weapon_bounces': Table(0x4ecaf4, 3, 12, (('spin_z', 0, 'q16'), ('launch_y', 4, 'q16'), ('rest_angle', 8, 'q16')), 'Angles are Q16 radians; gravity 0x28f; impact sound C[21]', (('floor', 0x4ecaf0, 'q16'),)),
    'blood_splash': Table(0x4e7ab0, 9, 4, (('mesh', 0, 'mesh_root'), ('ticks', 0, 'duration')), 'Final one-tick mesh-12 tail invokes 405dd0; complete splash lasts nineteen ticks'),
    'indexed_sounds': Table(0x4ead30, 66, 2, (('token', 0, 'u16'),), 'All 66 selectors accepted by 48a0d0; 0x25 uses this table, NOT bank B; tokens below 100 are control events'),
}


def table_values(image, name):
    table = TABLES[name]
    result = []
    for index in range(table.count):
        record = {}
        for field, offset, encoding in table.fields:
            at = table.address + index * table.stride + offset
            value = struct.unpack_from('<H', image.memory, at - 0x400000)[0] if encoding == 'u16' else image.word(at)
            if encoding == 'q16':
                value = signed_word(value)
            elif encoding == 'packed':
                value *= 4
            elif encoding == 'mesh_root':
                value = image.word((value & 0xffffff) + 24)
            elif encoding == 'duration':
                value = (value >> 24) & 0x7f
            record[field] = value
        result.append(record)
    return result


def table_rows(image, names, limit=512):
    rows = []
    remaining = limit
    for name in sorted(set(names)):
        table = TABLES[name]
        rows += ['', '### %s — `%08x`' % (name, table.address), '', 'Fields: ' + ', '.join(field[0] for field in table.fields) + '.', '']
        if table.note:
            rows += [table.note, '']
        if remaining <= 0:
            rows.append('All %d records omitted by --limit; follow the table address for the complete data.' % table.count)
            continue
        values = table_values(image, name)
        rows += ['```c']
        for field, at, encoding in table.header:
            value = signed_word(image.word(at)) if encoding == 'q16' else image.word(at)
            rows.append('static const int32_t %s_%s = %s; // %08x%s' %
                        (name, field, hex(value), at, ', Q16 %.6g' % (value / 65536) if encoding == 'q16' else ''))
        rows.append('struct %s_record {' % name)
        for field, _, encoding in table.fields:
            c_type = 'uint16_t' if encoding == 'u16' else 'int32_t' if encoding in ('q16', 'int') else 'uint32_t'
            rows.append('  %s %s;' % (c_type, field))
        rows += ['};', 'static const struct %s_record %s[] = {' % (name, name)]
        for record in values[:remaining]:
            fields = []
            for field, _, encoding in table.fields:
                value = record[field]
                if encoding == 'q16':
                    fields.append('%s /* %.6g */' % (hex(value), value / 65536))
                elif encoding in ('hex', 'packed'):
                    fields.append(hex(value))
                else:
                    fields.append(str(value))
            rows.append('  {' + ', '.join(fields) + '},')
        rows += ['};', '```']
        shown = min(remaining, len(values))
        remaining -= shown
        if shown < len(values):
            rows += ['', '%d table records omitted by --limit; do not treat this initializer as complete.' % (len(values) - shown)]
    return rows


def port_source(port):
    """Extract one local C definition, preserving its real source and line."""
    path, name = port
    text = (ROOT / path).read_text()
    # Mask comments/strings while preserving offsets for balanced-brace lookup.
    masked = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                    lambda match: re.sub(r'[^\n]', ' ', match[0]), text, flags=re.S)
    match = re.search(r'^([\w* ]+\b' + re.escape(name) + r'\s*\([^;{}]*\)\s*)\{', masked, re.M)
    if match is None:
        raise ValueError('Missing portable C definition %s in %s' % (name, path))
    depth, end = 1, match.end()
    while depth and end < len(masked):
        depth += (masked[end] == '{') - (masked[end] == '}')
        end += 1
    if depth:
        raise ValueError('Unbalanced portable C definition ' + name)
    return text.count('\n', 0, match.start()) + 1, text[match.start():end]


def helper_rows(addresses, image=None, include_related=False, include_tables=True, limit=512):
    """Only one explicit related-helper level; never recursive graph discovery."""
    selected = set(addresses)
    if include_related:
        selected.update(target for address in list(selected) if address in HELPERS for target in HELPERS[address].related)
    if not selected:
        return []
    rows = ['', '## Helper contracts', '',
            'Local EXE semantics; semantic aliases are not recovered C signatures. Reuse links point to engine adaptations.',
            'Data initializers preserve exact Q16 integers. Raw virtual addresses are research identifiers, not host pointers.', '']
    tables = set()
    unresolved = []
    for address in sorted(selected):
        helper = HELPERS.get(address)
        if helper is None:
            unresolved.append(address)
            continue
        rows += ['### `%08x` — %s' % (address, helper.name), '',
                 '- Inputs: ' + helper.inputs, '- Effects/data: ' + helper.effects, '- Timing/control: ' + helper.flow]
        if helper.port:
            line, _ = port_source(helper.port)
            path, function = helper.port
            rows.append('- Reuse: [`%s`](%s:%d).' % (function, ROOT / path, line))
        else:
            rows.append('- Reuse: no direct engine adapter recorded; preserve the contract above.')
        if helper.related:
            rows.append('- Related: ' + ', '.join('`--native %08x`' % value for value in helper.related) + '.')
        tables.update(helper.tables)
        rows.append('')
    if unresolved:
        rows += ['### Remaining native boundaries', '',
                 'Mapped names alone do not establish behavior. These need their exact native/source query:', '']
        rows += ['- `%08x` %s — `--native %08x`' % (address, native_name(address), address) for address in unresolved]
    if image is not None and include_tables:
        rows += table_rows(image, tables, limit)
    return rows


def native_c_text(image, address, limit=512):
    if not any(base == 0x401000 and base <= address < base + len(data) for base, data in image.sections):
        raise ValueError('Native entry is outside executable sections')
    rows = ['# Native %08x — %s' % (address, native_name(address)), '',
            'Contracts are researched for EXE SHA-256 `%s`.' % EXE_SHA256]
    rows += helper_rows([address], image, include_related=True, limit=limit)
    helper = HELPERS.get(address)
    if helper is not None and helper.port:
        line, code = port_source(helper.port)
        rows += ['', '## Existing portable C adaptation', '',
                 'Read from [%s:%d](%s:%d). This is our engine implementation, not automatically decompiled retail x86.' %
                 (helper.port[0], line, ROOT / helper.port[0], line)]
        if len(code.splitlines()) <= limit:
            rows += ['', '```c', code, '```']
        else:
            rows += ['', 'C definition exceeds --limit; follow the source link for the complete body.']
    else:
        rows += ['', 'No portable C body is recorded for this entry. Use `--native %08x` for its bounded x86 graph;' % address,
                 'do not implement an unresolved helper as a no-op.']
    rows += reference_rows([address])
    return '\n'.join(rows) + '\n'
