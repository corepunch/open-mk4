#!/usr/bin/env python3
"""Extract Scorpion's retail command records, dispatch targets and attack records.

This is a source inventory, not a claim that every native path is implemented.
Run from the workspace root. The supplied EXE's exact hash is checked.
"""
import argparse
import json
from pathlib import Path

from retail_moves import EXE_SHA256, RetailImage


# Button names confirmed by the retail sc_m_geo.geo move-list texture as well
# as its command records: D,B,HP; B,B,LP; F,F,HK; D,F,LP and the finishers.
COMMAND_TABLES = [
    ('up', 0x53c888, 0x53ca80),
    ('high_punch', 0x53c8d0, 0x53cb58),
    ('low_punch', 0x53c918, 0x53cba0),
    ('block', 0x53c960, 0x53cbe8),
    ('high_kick', 0x53c9a8, 0x53cc30),
    ('low_kick', 0x53c9f0, 0x53cc78),
    ('run', 0x53cac8, 0x53ccc0),
]
DISPATCH = [(1, 0x4e6ef8), (2, 0x4ed828), (4, 0x4e8910),
            (8, 0x4e7008), (32, 0x4dfc90)]


def commands(image):
    result = []
    for button, p1, p2 in COMMAND_TABLES:
        root = image.word(p1) * 4
        assert root == image.word(p2) * 4, 'P1/P2 Scorpion command tables differ'
        records = []
        pc = root
        for _ in range(256):
            pattern, extra, word = [image.word(pc + k * 4) for k in range(3)]
            if pattern == 0:
                break
            flags, action, gate = (word >> 16) & 255, word & 255, word >> 24
            table = next((base for bit, base in DISPATCH if flags & bit), None)
            tokens = []
            if pattern != 0xffffffff:
                remaining = pattern
                while remaining:
                    tokens.append(remaining & 255)
                    remaining >>= 8
            records.append(dict(va=hex(pc), raw_words=[hex(pattern), hex(extra), hex(word)],
                                tokens=tokens, unconditional_pattern=pattern == 0xffffffff,
                                window=(word >> 8) & 255, flags=hex(flags), action=action,
                                predicate_index=gate,
                                predicate=hex(image.word(0x4e4ec0 + gate * 4)) if gate else None,
                                dispatch_table=hex(table) if table else None,
                                target=hex(image.word(table + action * 4)) if table else None))
            pc += 12
        else:
            raise ValueError('Unterminated command list')
        result.append(dict(button=button, p1_table=hex(p1), p2_table=hex(p2),
                           selected_list=hex(root), records=records))
    return result


def attack_record(image, table, selector):
    va = image.word(table + selector * 4) * 4
    words = [image.word(va + k * 4) for k in range(6)]

    def signed(value):
        return value if value < 0x80000000 else value - 0x100000000

    return dict(table=hex(table), selector=selector, va=hex(va),
                raw_words=[hex(word) for word in words],
                rectangle_q16=[signed(word) for word in words[:4]],
                reaction=words[4] & 255, alternate_reaction=(words[4] >> 8) & 255,
                blocked_reaction=(words[4] >> 16) & 255, flags=words[4] >> 24,
                damage_q16=words[5])


def inventory(image):
    # These roots were followed directly in the supplied EXE. Several native
    # routines use shared animation/motion/weapon/scene machinery beyond a VM
    # script. Listing a root does not close that dependency graph.
    roots = [
        ('jab', 0x46dca0, 0x4e62f0),
        ('high_kick', 0x46a680, 0x4e5ca0),
        ('sweep', 0x46ef90, 0x4e6798),
        ('uppercut', 0x46ccd0, 0x4e62a0),
        ('spear', 0x49a630, 0x4ed448),
        ('breathe_fire', 0x468610, 0x4e5b58),
        ('teleport_punch', 0x47fc90, 0x4e87b0),
        ('draw_weapon', 0x46d1d0, 0x4e62c0),
        ('fatality_1', 0x467ce0, None),
        ('fatality_2', 0x467cd0, None),
        ('spike_fatality', 0x467e70, None),
        ('fan_fatality', 0x467e00, None),
    ]
    return dict(exe_sha256=EXE_SHA256, character=0,
                move_list_asset='geogfx/sc_m_geo.geo',
                command_records=commands(image),
                entry_points=[dict(name=name, native=hex(native), script=hex(script) if script else None,
                                   implementation='unverified except high_kick no-contact playback')
                              for name, native, script in roots],
                kick_contacts=[attack_record(image, 0x53d2b8, i) for i in (1, 2)],
                coverage=dict(command_records='extracted; both players checked',
                              native_predicates='identified; not yet ported',
                              combos='native jab repeat helper identified at 0x46de60; graph not closed',
                              weapons='draw command identified; weapon-specific graph not closed',
                              fatalities='four command targets identified; scene graphs not closed'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    report = json.dumps(inventory(RetailImage(args.exe)), indent=2) + '\n'
    if args.output:
        args.output.write_text(report)
    else:
        print(report, end='')


if __name__ == '__main__':
    main()
