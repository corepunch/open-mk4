#!/usr/bin/env python3
"""Repeat the bounded script/metadata extraction without an emulator dependency."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from retail_moves import RetailImage, clip_inventory, kick_graph

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
