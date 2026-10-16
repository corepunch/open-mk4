#!/usr/bin/env python3
"""Repeat the bounded script/metadata extraction without an emulator dependency."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from retail_moves import RetailImage, clip_inventory, kick_graph
from scorpion_moves import inventory

image = RetailImage('MK4/MK-4.EXE')
clips = clip_inventory(image, 'MK4/FILESYS.DAT')
assert len(clips) == 612 and sum(c['frame_count'] for c in clips) == 36723
assert clips[66]['frame_count'] == 18 and clips[71]['frame_count'] == 28
for character, script, clip, recovery, direction in [
    (0, '0x4e6c90', 66, '0x4e5c70', 'finish backwards'),
    (4, '0x4e5c80', 71, '0x4e6998', 'finish forwards'),
]:
    graph = kick_graph(image, character)
    assert graph['selected_script'] == script
    assert [i['clip'] for i in graph['instructions'] if 'clip' in i] == [clip]
    assert graph['native'][0]['next_script'] == recovery
    assert any(i['operation'] == direction for i in graph['instructions'])
    assert graph['instructions'][-1]['target'] == '0x46f650'
print('PASS: 612 clip descriptors/payloads; original high-kick scripts and native recovery edges')

scorpion = inventory(image)
buttons = {row['button']: row['records'] for row in scorpion['command_records']}
# Cross-check against the move list in the retail sc_m_geo.geo texture. These
# are directions in input order, not reversed bytecode or display text guesses.
for button, pattern, window, target in [
    ('high_punch', [2, 0xaa], 24, '0x47fc90'),
    ('low_punch', [0xaa, 0xaa], 27, '0x49a630'),
    ('low_punch', [2, 0xdd], 20, '0x468610'),
    ('high_kick', [0xdd, 0xdd], 24, '0x46d1d0'),
    ('block', [0xaa, 0xdd, 0xdd, 0xaa], 50, '0x467ce0'),
    ('high_punch', [0xaa, 0xdd, 2, 1], 50, '0x467cd0'),
    ('low_kick', [0xdd, 2, 2], 40, '0x467e00'),
    ('low_kick', [0xaa, 0xdd, 0xdd], 40, '0x467e70'),
]:
    matches = [row for row in buttons[button] if row['tokens'] == pattern]
    assert len(matches) == 1
    assert (matches[0]['window'], matches[0]['target']) == (window, target)
assert buttons['up'] == []
assert buttons['block'][1]['unconditional_pattern']
assert buttons['block'][1]['raw_words'][:2] == ['0xffffffff', '0xffffffff']
for contact, address in zip(scorpion['kick_contacts'], ['0x4ebae0', '0x4ebaf8']):
    assert contact['va'] == address
    assert contact['damage_q16'] == 0x2147
    assert (contact['reaction'], contact['alternate_reaction'], contact['blocked_reaction']) == (16, 13, 6)
print('PASS: both players\' Scorpion commands match retail move-list directions; contact records decoded')
