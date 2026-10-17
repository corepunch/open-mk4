"""Build-specific aliases plus structurally mapped mk4decomp symbols.

Upstream numeric addresses alone never establish a local correspondence.
"""
import json
from pathlib import Path
from retail_moves import EXE_SHA256
from retail_helpers import HELPERS

MAP = json.loads(Path(__file__).with_name('retail_symbol_map.json').read_text())
if MAP['exe_sha256'] != EXE_SHA256:
    raise ValueError('Symbol map targets another retail revision')
MAPPED = MAP['mapped']

# Descriptive research aliases for THIS build, never imported upstream addresses.
NAMES = {
    0x407920: 'pose_submit', 0x408600: 'geometry_resource_bind',
    0x41f290: 'task_allocate', 0x41f570: 'task_scheduler',
    0x41f710: 'task_unlink', 0x41f780: 'tagged_continuation_pop',
    0x41f830: 'task_finish_and_pause', 0x41f840: 'task_mark_finished',
    0x429830: 'advance_cursor_then_submit', 0x429990: 'submit_current_pose',
    0x43cbb0: 'fatality_1_scene_entry', 0x43bd70: 'fatality_2_scene_entry',
    0x43d040: 'fan_fatality_scene_entry',
    0x459490: 'script_enter', 0x4594b0: 'script_dispatch',
    0x45bfc0: 'script_fetch_word', 0x45dd30: 'command_list_select',
    0x45de00: 'command_match', 0x45e0a0: 'command_predicate_call',
    0x45f5f0: 'action_dispatch', 0x46de60: 'jab_repeat_predicate',
    0x46def0: 'jab_press_latch_select', 0x46ddd0: 'jab_contact',
    0x46cde0: 'uppercut_continuation', 0x46f080: 'sweep_contact_continuation',
    0x46a8d0: 'wait_pose_then_controller_return',
    0x46a950: 'task_slot_5c_clear', 0x46a970: 'task_slot_5c_set_one',
    0x46f650: 'fighter_controller_return', 0x470c60: 'kick_contact_parameter_runner',
    0x47fcf0: 'teleport_wait_for_group_callback', 0x480040: 'teleport_group_callback',
    0x48d3d0: 'contact_query_a', 0x48d3f0: 'contact_query_b',
    0x48d410: 'contact_query_c', 0x48d450: 'contact_query_d',
    0x48de80: 'health_subtract_q16', 0x489f00: 'sound_event_a',
    0x489f30: 'sound_event_b', 0x49a6a0: 'spear_task',
    0x49ae90: 'spear_task_contact_wait', 0x49b170: 'spear_victim_continuation',
    0x49c810: 'spear_contact_query', 0x49c610: 'special_input_snapshot',
}

for _symbol in MAPPED:
    NAMES.setdefault(int(_symbol['local'], 16), _symbol['name'])
NAMES.update({address: helper.name for address, helper in HELPERS.items()})


def mapped_symbol(address):
    """Prefer an exact entry, otherwise the narrowest mapped container."""
    rows = [row for row in MAPPED if int(row['local'], 16) <= address < int(row['end'], 16)]
    return min(rows, key=lambda row: (int(row['local'], 16) != address,
                                     int(row['end'], 16) - int(row['local'], 16))) if rows else None


def native_name(address):
    if address in NAMES:
        return NAMES[address]
    row = mapped_symbol(address)
    return '%s_at_%08x' % (row['name'], address) if row else 'unresolved_native_%08x' % address

# Manually compared with the supplied EXE's exact-entry listings. The source
# checkout is af88007; offsets below are within its matching assembly cluster,
# not the NON_MATCHING C twin (which omits these embedded entry points).
# Each tuple is (upstream entry, enclosing symbol, file, correspondence evidence).
REFERENCES = {
    0x459490: (0x4594f0, 'ArgSarStoreJmp', 'src/engine/arg_sar_store_jmp.c',
               'Argument >> 2 -> task +0x48, then tail to the VM.'),
    0x46a8d0: (0x46a8b0, 'PendingMatch_ArgSarStoreJmp_0046a6e0 +0x1d0',
               'src/engine/five_block_dispatch_variants.c',
               'Task +0x84 re-entry gate, tagged self continuation, pose helper, pause, controller tail.'),
    0x46a950: (0x46a9b0, 'PendingMatch_ArgSarStoreJmp_0046a6e0 +0x2d0',
               'src/engine/five_block_dispatch_variants.c',
               'Zero -> native argument scratch and task +0x5c, then return.'),
    0x46a970: (0x46a9d0, 'PendingMatch_ArgSarStoreJmp_0046a6e0 +0x2f0',
               'src/engine/five_block_dispatch_variants.c',
               'One -> native argument scratch and task +0x5c, then return.'),
}


def target_text(value):
    """Keep the original address and annotate only known local names."""
    address = int(value, 16) if isinstance(value, str) else value
    name = NAMES.get(address)
    if name is None and mapped_symbol(address):
        name = native_name(address)
    return '%s (%s)' % (value, name) if name else str(value)


def reference_rows(addresses):
    """Link checked correspondences without importing upstream addresses."""
    selected = sorted(address for address in set(addresses) if address in REFERENCES or mapped_symbol(address))
    if not selected:
        return []
    root = Path(__file__).resolve().parents[1] / 'reference/mk4decomp'
    rows = ['', '### mk4decomp source references', '',
            'Checked correspondences to reference af88007; upstream addresses belong to a different EXE revision.',
            'Names are research annotations; embedded entries may exist only in the matching assembly branch.', '']
    for address in selected:
        if address not in REFERENCES:
            row = mapped_symbol(address)
            offset = address - int(row['local'], 16)
            rows.append('- `%08x` -> [`%s`](%s), upstream container `%08x`, local container `%08x`%s. %s. '
                        'Matching assembly: [%s:%d](%s:%d).' %
                        (address, row['name'], root / row['source'], int(row['upstream'], 16), int(row['local'], 16),
                         ' +%#x' % offset if offset else '', row['evidence'], row['assembly_source'], row['assembly_line'],
                         root / row['assembly_source'], row['assembly_line']))
            continue
        upstream, symbol, source, evidence = REFERENCES[address]
        rows.append('- `%08x` (%s) -> [`%s`](%s), upstream `%08x`. %s' %
                    (address, NAMES[address], symbol, root / source, upstream, evidence))
    return rows
