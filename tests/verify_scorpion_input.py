#!/usr/bin/env python3
"""Optional x86 oracle for Scorpion's input sampling, command matching and
normal-move selection.

Every tick executes the ORIGINAL switch scan 0x45c560, the tick increment
done by 0x41fdfb, the switch-event queue drain 0x45c7c0 and the original task
pump 0x41f570. Switch handlers therefore run as real type-0x11 tasks and
call the original command matcher 0x45dd30/0x45de00, the predicates and the
normal-button fallback 0x45da20. Only the raw switch words 0x4d00a4/0x4d00a8
are injected (active low). After input, the interrupted fighter's new
callback (0x45f5f0 action_dispatch or a stance button handler) runs in the
fighter context until it reaches an action-table entry; that entry then runs
until its first write of the fighter task action (+0x74).

Stubbed: 0x45f870 begin-action (busy flag/button table/motion side effects),
0x46a180 weapon-pickup side effect inside predicate 0x45efc0, pose 0x407920
and sound 0x489f00/0x489f30/0x489f90/0x489fc0/0x489ff0. Injected world
fields are listed in world(). The fighter distance 0x53019c is injected, not
computed. Fighter tasks are not linked into the pump: this does not measure
move playback, contact, collision or the fighter's dispatch latency.
"""
import argparse
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from retail_moves import EXE_SHA256, RetailImage

NODE1, NODE2 = 0x538728, 0x538728 + 0xe8   # real task-pool slots 0 and 1
G1, G2 = 0x1001000, 0x1003000              # P1/P2 fighter groups
OBJ = 0x1007000                            # one world object (weapon list)
SENTINEL = 0x1700000

# P1 switch bits, active low: word 0 = 0x4d00a4, word 1 = 0x4d00a8.
SWITCH = {'U': (0, 0x01), 'D': (0, 0x02), 'L': (0, 0x04), 'R': (0, 0x08),
          'HP': (0, 0x10), 'BLK': (0, 0x20), 'HK': (0, 0x40),
          'LP': (1, 0x01), 'LK': (1, 0x02), 'RUN': (1, 0x04)}
# Action tables selected by command flag bits 01, 02, 04, 08 and 20.
TABLES = [(0x4e6ef8, 0x48), (0x4e7008, 0x14), (0x4ed828, 0x1b), (0x4e8910, 0x23), (0x4dfc90, 0x40)]
BUTTON_TABLES = {'standing': 0x4e5058, 'crouching': 0x4e5070, 'jump_up': 0x4e6ec8,
                 'jump_up_after_punch': 0x4e6ee0, 'jump_diagonal': 0x4e70d8,
                 'followup_4e7078': 0x4e7078, 'followup_4e7090': 0x4e7090,
                 'followup_4e70a8': 0x4e70a8, 'followup_4e70c0': 0x4e70c0, 'locked': 0x53d370,
                 'armed_standing': 0x4ecd08}
SWORD = 0x4ecdf8   # Scorpion's held-weapon descriptor [0x53d5c0 + 0]: type 0x60, word 3 = button table


class Machine:
    def __init__(self, image):
        from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.uc.mem_map(0x400000, 0x1600000)
        for base, payload in image.sections:
            self.uc.mem_write(base, payload)
        for dst, value in image.fixups.items():
            self.put(dst, value)
        self.hooks = {}
        self.uc.hook_add(UC_HOOK_CODE, self._code)

    def _code(self, uc, va, size, data):
        handler = self.hooks.get(va)
        if handler is not None:
            handler(va)

    def put(self, va, value):
        self.uc.mem_write(va, struct.pack('<I', value & 0xffffffff))

    def get(self, va):
        return struct.unpack('<I', self.uc.mem_read(va, 4))[0]

    def reg(self, name):
        from unicorn import x86_const
        return self.uc.reg_read(getattr(x86_const, 'UC_X86_REG_' + name))

    def ret(self, va=None):
        from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP
        sp = self.reg('ESP')
        self.uc.reg_write(UC_X86_REG_EIP, self.get(sp))
        self.uc.reg_write(UC_X86_REG_ESP, sp + 4)

    def call(self, va, budget=400000):
        from unicorn.x86_const import UC_X86_REG_ESP
        stack = 0x1800000
        self.put(stack, SENTINEL)
        self.uc.reg_write(UC_X86_REG_ESP, stack)
        self.stopped = False
        self.uc.emu_start(va, SENTINEL, count=budget)
        if not self.stopped:
            assert self.reg('EIP') == SENTINEL, 'Instruction budget exhausted in %#x' % va
            assert self.reg('ESP') == stack + 4, 'Unbalanced CPU stack in %#x' % va

    def stop(self):
        self.stopped = True
        self.uc.emu_stop()


def world(m, p):
    """Seed the minimal real structures. Every seeded field is listed here."""
    tick0 = 1000
    for va, value in [
            (0x4d07ac, 0x1100000 // 4),                 # native scratch stack (packed)
            (0x5323f8, NODE1 // 4), (0x5323fc, NODE2 // 4),  # P1/P2 fighter tasks
            (0x532518, G1 // 4), (0x53251c, G2 // 4),   # P1/P2 fighter groups
            (0x5347c8, 1), (0x532248, 1),               # both players human-controlled
            (0x524e84, 4),                              # game mode 4 (fight) handler slot
            (0x534af0, 0), (0x532454, 0), (0x53dbc0, 0), (0x53dad4, 0),
            (0x534858, tick0),                          # input/game tick counter
            (0x53019c, p.get('distance', 0x30000)),     # fighter distance, Q16 (injected)
            (0x532354, p.get('winner', 0)), (0x532358, 0),  # round winner / finisher done
            (0x5348dc, p.get('stage', 3)),              # arena id
            (0x5301b0, OBJ // 4)]:                      # world object list head
        m.put(va, value)
    m.uc.mem_write(0x4ee624, bytes([p.get('finisher_byte', 1)]))
    # Fighter tasks occupy real pool slots but are NOT linked into the pump
    # list; the interrupt that a handler applies to them is recorded instead.
    for node, group, other, ogroup, action, counter, weapon in [
            (NODE1, G1, NODE2, G2, p.get('self_action', 0x200b), 0, p.get('weapon', 0)),
            (NODE2, G2, NODE1, G1, p.get('opp_action', 0x200b), p.get('opp_7c', 0), 0)]:
        for off, value in [(0x04, 0x1101000 // 4), (0x08, 0x46f740), (0x30, weapon),
                           (0x34, 0), (0x38, ogroup // 4), (0x3c, other // 4),
                           (0x74, action), (0x7c, counter),
                           (0xd8, 0x46f740), (0xdc, 0x7fff), (0xe0, 1)]:
            m.put(node + off, value)
    if p.get('opp_dazed'):
        m.put(NODE2 + 0x08, 0x46a580)           # finish-him dazed loop callback
    facing_left = p.get('facing_left', False)
    for group, number, x, air, flags in [
            (G1, 1, p.get('opp_x', 0x30000) if facing_left else 0, p.get('self_air', False), p.get('flags40', 0)),
            (G2, 2, 0 if facing_left else p.get('opp_x', 0x30000), p.get('opp_air', False), 0)]:
        for off, value in [(0x10, 0), (0x30, number), (0x34, 0), (0x40, flags), (0x44, 0),
                           (0x48, 0), (0x54, x), (0x58, -0x30000 if air else 0), (0x5c, 0)]:
            m.put(group + off, value)
    for index, ago in p.get('stamps', {}).items():
        m.put(0x53c2a8 + index * 8, tick0 - ago)  # P1 time stamp slot
    if p.get('loose_weapon'):
        for off, value in [(0x30, 0x60), (0x54, 0x8000), (0x5c, 0), (0x6c, 0), (0x70, 0), (0x74, 0)]:
            m.put(OBJ + off, value)
    m.put(0x530228, BUTTON_TABLES[p.get('stance', 'standing')] // 4)


def press(events, facing_left):
    """Expand (tick, switch[, hold]) into per-tick active-low words. B/F are
    facing relative; the raw switches themselves are absolute L/R."""
    rows = []
    for event in events:
        tick, name = event[0], event[1]
        hold = event[2] if len(event) > 2 else 2
        name = {'B': 'R' if facing_left else 'L', 'F': 'L' if facing_left else 'R'}.get(name, name)
        rows.append((tick, SWITCH[name], hold))
    return rows


def run_inputs(m, scenario):
    log, tick = [], [0]
    rows = press(scenario['events'], scenario.get('params', {}).get('facing_left', False))
    hooks = {
        0x45de03: lambda va: log.append('%d record %#x' % (tick[0], m.get(0x53c404) * 4)),
        0x45df60: lambda va: log.append('%d reject tokens_or_block' % tick[0]),
        0x45dfab: lambda va: log.append('%d window elapsed=%d limit=%d' % (tick[0], m.reg('EAX'), m.reg('ECX'))),
        0x45e024: lambda va: log.append('%d no_match_stop' % tick[0]),
        0x45dff8: lambda va: log.append('%d reject not_grounded' % tick[0]),
        0x45e109: lambda va: log.append('%d predicate %#x' % (tick[0], m.reg('EAX'))),
        0x45e10b: lambda va: log.append('%d predicate_result %d' % (tick[0], m.get(0x53c44c) & 1)),
        0x41fa50: lambda va: log.append('%d interrupt callback=%#x word=%#x' % (
            tick[0], m.get(0x53c408), m.get(m.get(0x53c404) * 4 + 0x14))),
        0x45d900: lambda va: log.append('%d history %#x token=%d' % (tick[0], m.get(0x53c404) * 4, m.get(0x53c42c))),
    }
    m.hooks = dict(hooks)
    for va in (0x45f870, 0x46a180, 0x489f90):
        m.hooks[va] = m.ret
    m.call(0x45c880)                      # original switch/history initialisation
    for t in range(scenario.get('ticks', 64)):
        tick[0] = t
        words = [0xffffffff, 0xffffffff]
        for start, (word, bit), hold in rows:
            if start <= t < start + hold:
                words[word] &= ~bit
        m.put(0x4d00a4, words[0])
        m.put(0x4d00a8, words[1])
        m.call(0x45c560)                  # original switch scan / event queue
        m.put(0x534858, m.get(0x534858) + 1)  # 0x41fdfb inc [0x534858]
        m.call(0x45c7c0)                  # original queue drain (spawns handler tasks)
        m.call(0x41f570)                  # original task pump runs the handlers
    return log


def resolve(image, m, callback, word):
    """Run the fighter's new callback in the fighter context. Record each
    action-table entry reached, then the entry's first action write."""
    from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
    entries = {image.word(t + 4 * i) for t, n in TABLES for i in range(n)} - {0}
    entries.add(0x42b6b0)                 # shared close-LK entry reached by jmp from 0x46a847
    out = dict(callback=hex(callback), chain=[], action=None)
    m.hooks = {va: m.ret for va in (0x45f870, 0x407920, 0x489f00, 0x489f30, 0x489f90, 0x489fc0, 0x489ff0)}

    def index(va):
        out.setdefault('index', [])
        out['index'].append('%#x[%d]' % (va, m.get(0x53c42c)))
    m.hooks[0x471130] = index
    m.hooks[0x4711f0] = index
    for va in (0x46c73a, 0x46c4f0):       # weapon descriptor button-table entry
        m.hooks[va] = lambda va: out['chain'].append('weapon_entry %#x' % m.reg('EAX'))
    m.hooks[0x46c281] = lambda va: out['chain'].append('character_entry %#x' % m.reg('EAX'))
    m.put(0x53c420, NODE1 // 4)
    m.put(0x53c41c, G1 // 4)
    m.put(0x53c43c, word)                 # task +0x14 restored by the pump
    m.put(0x53c22c, 0)

    def code(uc, va, size, data):
        if va in entries and va != callback and hex(va) not in out['chain']:
            out['chain'].append(hex(va))

    def write(uc, access, address, size, value, data):
        if out['chain'] and out['action'] is None:
            out['action'] = hex(value & 0xffffffff)
            m.stop()
    h1 = m.uc.hook_add(UC_HOOK_CODE, code)
    h2 = m.uc.hook_add(UC_HOOK_MEM_WRITE, write, begin=NODE1 + 0x74, end=NODE1 + 0x77)
    try:
        m.call(callback, budget=60000)
        out['status'] = 'action_written' if out['action'] else 'returned_without_action'
    except Exception as error:
        out['status'] = 'action_written' if out['action'] else 'unresolved_dependency'
        out['unresolved'] = str(error).split('(')[0].strip()
    if out['action']:
        out.pop('unresolved', None)
    m.uc.hook_del(h1)
    m.uc.hook_del(h2)
    return out


def scenario_result(image, scenario):
    m = Machine(image)
    world(m, scenario.get('params', {}))
    log = run_inputs(m, scenario)
    interrupts = [line for line in log if ' interrupt ' in line]
    result = dict(name=scenario['name'], events=[list(e) for e in scenario['events']],
                  params=scenario.get('params', {}), trace=log)
    if interrupts:
        fields = dict(item.split('=') for item in interrupts[-1].split()[2:])
        word = int(fields['word'], 16)
        callback = int(fields['callback'], 16)
        result['matched_word'] = hex(word) if callback == 0x45f5f0 else None
        result['resolution'] = resolve(image, m, callback, word if callback == 0x45f5f0 else 0)
    else:
        result['matched_word'] = None
        result['resolution'] = None
    return result


def S(name, events, ticks=64, **params):
    return dict(name=name, events=events, ticks=ticks, params=params)


FINISH = dict(winner=1, opp_dazed=True, distance=0x10000)
SCENARIOS = [
    # Teleport punch: D,B + HP, window 24, flags 0x14 (air allowed), predicate 0x45f0c0.
    S('teleport_window_24', [(10, 'D'), (14, 'B'), (34, 'HP')]),
    S('teleport_window_25', [(10, 'D'), (14, 'B'), (35, 'HP')]),
    S('teleport_wrong_order', [(10, 'B'), (14, 'D'), (20, 'HP')]),
    S('teleport_extra_direction_between', [(10, 'D'), (13, 'F'), (16, 'B'), (20, 'HP')]),
    S('teleport_extra_direction_after', [(10, 'D'), (13, 'B'), (16, 'U'), (20, 'HP')]),
    S('teleport_buttons_between', [(10, 'D'), (12, 'LK'), (14, 'B'), (16, 'HK'), (20, 'HP')]),
    S('teleport_diagonal_roll', [(10, 'D', 6), (13, 'B', 6), (20, 'HP')]),
    S('teleport_wrong_button_lp', [(10, 'D'), (14, 'B'), (20, 'LP')]),
    S('teleport_block_held', [(10, 'D'), (14, 'B'), (18, 'BLK', 6), (20, 'HP')]),
    S('teleport_airborne', [(10, 'D'), (14, 'B'), (20, 'HP')], self_air=True),
    S('teleport_busy_flag', [(10, 'D'), (14, 'B'), (20, 'HP')], flags40=1),
    S('teleport_opp_counter_2', [(10, 'D'), (14, 'B'), (20, 'HP')], opp_7c=2),
    S('teleport_opp_counter_3', [(10, 'D'), (14, 'B'), (20, 'HP')], opp_7c=3),
    S('teleport_cooldown_68', [(10, 'D'), (14, 'B'), (20, 'HP')], stamps={1: 68 - 21}),
    S('teleport_cooldown_69', [(10, 'D'), (14, 'B'), (20, 'HP')], stamps={1: 69 - 21}),
    S('teleport_facing_left', [(10, 'D'), (14, 'B'), (20, 'HP')], facing_left=True),
    S('teleport_absolute_left_while_facing_left', [(10, 'D'), (14, 'L'), (20, 'HP')], facing_left=True),
    # Spear: B,B + LP, window 27, predicate 0x45ea60.
    S('spear_window_27', [(10, 'B'), (14, 'B'), (37, 'LP')]),
    S('spear_window_28', [(10, 'B'), (14, 'B'), (38, 'LP')]),
    S('spear_single_hold', [(10, 'B', 8), (20, 'LP')]),
    S('spear_retap_after_1_tick', [(10, 'B', 1), (12, 'B'), (20, 'LP')]),
    S('spear_retap_after_2_ticks', [(10, 'B', 1), (13, 'B'), (20, 'LP')]),
    S('spear_cooldown_89', [(10, 'B'), (14, 'B'), (20, 'LP')], stamps={0x13: 89 - 21}),
    S('spear_cooldown_90', [(10, 'B'), (14, 'B'), (20, 'LP')], stamps={0x13: 90 - 21}),
    S('spear_opp_action_1008', [(10, 'B'), (14, 'B'), (20, 'LP')], opp_action=0x1008),
    S('spear_opp_counter_3', [(10, 'B'), (14, 'B'), (20, 'LP')], opp_7c=3),
    S('spear_opp_counter_4', [(10, 'B'), (14, 'B'), (20, 'LP')], opp_7c=4),
    S('spear_airborne', [(10, 'B'), (14, 'B'), (20, 'LP')], self_air=True),
    S('spear_block_held', [(10, 'B'), (14, 'B'), (18, 'BLK', 6), (20, 'LP')]),
    # Breathe fire: D,F + LP, window 20, no predicate.
    S('fire_window_20', [(10, 'D'), (14, 'F'), (30, 'LP')]),
    S('fire_window_21', [(10, 'D'), (14, 'F'), (31, 'LP')]),
    S('fire_wrong_order', [(10, 'F'), (14, 'D'), (20, 'LP')]),
    S('fire_airborne', [(10, 'D'), (14, 'F'), (20, 'LP')], self_air=True),
    S('fire_crouching_table', [(10, 'D'), (14, 'F'), (20, 'LP')], stance='crouching'),
    # Draw weapon: F,F + HK, window 24, predicate 0x45ef50.
    S('weapon_window_24', [(10, 'F'), (14, 'F'), (34, 'HK')]),
    S('weapon_window_25', [(10, 'F'), (14, 'F'), (35, 'HK')]),
    S('weapon_lost_flag', [(10, 'F'), (14, 'F'), (20, 'HK')], flags40=2),
    S('weapon_already_armed', [(10, 'F'), (14, 'F'), (20, 'HK')], weapon=OBJ // 4, flags40=2),
    # Finishers.
    S('fatality2_window_50', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (60, 'HP')], **FINISH),
    S('fatality2_window_51', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (61, 'HP')], **FINISH),
    S('fatality2_no_winner', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (30, 'HP')], opp_dazed=True, distance=0x10000),
    S('fatality2_not_dazed', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (30, 'HP')], winner=1, distance=0x10000),
    S('fatality2_distance_10001', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (30, 'HP')], winner=1, opp_dazed=True, distance=0x10001),
    S('fatality2_finisher_byte_0', [(10, 'B'), (14, 'F'), (18, 'D'), (22, 'U'), (30, 'HP')], finisher_byte=0, **FINISH),
    S('fatality1_distance_1cccc', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (30, 'BLK')], winner=1, opp_dazed=True, distance=0x1cccc),
    S('fatality1_distance_1cccb', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (30, 'BLK')], winner=1, opp_dazed=True, distance=0x1cccb),
    S('fatality1_distance_30000', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (30, 'BLK')], winner=1, opp_dazed=True, distance=0x30000),
    S('fatality1_distance_30001', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (30, 'BLK')], winner=1, opp_dazed=True, distance=0x30001),
    S('fatality1_window_51', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (61, 'BLK')], winner=1, opp_dazed=True, distance=0x20000),
    S('fan_fatality_stage_8', [(10, 'F'), (14, 'D'), (18, 'D'), (30, 'LK')], stage=8, **FINISH),
    S('fan_fatality_stage_0', [(10, 'F'), (14, 'D'), (18, 'D'), (30, 'LK')], stage=0, **FINISH),
    S('fan_fatality_window_41', [(10, 'F'), (14, 'D'), (18, 'D'), (51, 'LK')], stage=8, **FINISH),
    S('spike_fatality_stage_0', [(10, 'B'), (14, 'F'), (18, 'F'), (30, 'LK')], stage=0, **FINISH),
    S('spike_fatality_stage_8', [(10, 'B'), (14, 'F'), (18, 'F'), (30, 'LK')], stage=8, **FINISH),
    # Unconditional block and run records.
    S('block_plain', [(20, 'BLK')]),
    S('air_throw_both_airborne_close', [(20, 'BLK')], self_air=True, opp_air=True, opp_x=0x10000),
    S('air_throw_too_far', [(20, 'BLK')], self_air=True, opp_air=True, opp_x=0x20000),
    S('air_throw_shadowed_by_slow_fatality_tokens', [(10, 'B'), (14, 'F'), (18, 'F'), (22, 'B'), (61, 'BLK')],
      self_air=True, opp_air=True, opp_x=0x10000),
    S('run_single_press', [(20, 'RUN')]),
    S('run_double_tap_8', [(10, 'RUN'), (18, 'RUN')]),
    S('run_double_tap_9', [(10, 'RUN'), (19, 'RUN')]),
    S('run_double_tap_cooldown_79', [(10, 'RUN'), (18, 'RUN')], stamps={0xa: 79 - 19}),
    S('run_weapon_pickup_crouching', [(20, 'RUN')], self_action=0x2001, loose_weapon=True),
    S('run_weapon_pickup_standing', [(20, 'RUN')], loose_weapon=True),
]
# Plain buttons per stance; no direction history. 'B' holds back to the end.
for stance in BUTTON_TABLES:
    air = stance.startswith('jump')
    armed = dict(weapon=SWORD // 4) if stance == 'armed_standing' else {}
    for button in ('HP', 'LP', 'BLK', 'HK', 'LK', 'RUN'):
        SCENARIOS.append(S('%s_%s_far' % (stance, button), [(20, button)], ticks=24, stance=stance, self_air=air, **armed))
for stance in ('standing', 'followup_4e7078', 'followup_4e7090', 'followup_4e70a8', 'followup_4e70c0'):
    for button in ('HP', 'LP', 'HK', 'LK'):
        SCENARIOS.append(S('%s_%s_close' % (stance, button), [(20, button)], ticks=24, stance=stance, distance=0x8000))
        SCENARIOS.append(S('%s_back_%s' % (stance, button), [(5, 'B', 19), (20, button)], ticks=24, stance=stance))
SCENARIOS += [
    S('standing_HP_close_opp_airborne', [(20, 'HP')], ticks=24, distance=0x8000, opp_air=True),
    S('standing_HP_close_opp_reacting_4001', [(20, 'HP')], ticks=24, distance=0x8000, opp_action=0x4001),
    S('standing_HK_close_opp_4005', [(20, 'HK')], ticks=24, distance=0x8000, opp_action=0x4005),
    S('standing_HK_close_opp_4002', [(20, 'HK')], ticks=24, distance=0x8000, opp_action=0x4002),
    S('standing_LP_close_opp_jabbing_10b', [(20, 'LP')], ticks=24, distance=0x8000, opp_action=0x10b),
    S('standing_LP_distance_b333', [(20, 'LP')], ticks=24, distance=0xb333),
    S('standing_LP_distance_b334', [(20, 'LP')], ticks=24, distance=0xb334),
    S('standing_HP_distance_cccc', [(20, 'HP')], ticks=24, distance=0xcccc),
    S('standing_HP_distance_cccd', [(20, 'HP')], ticks=24, distance=0xcccd),
    S('standing_LK_distance_b333', [(20, 'LK')], ticks=24, distance=0xb333),
    S('standing_LK_distance_b332', [(20, 'LK')], ticks=24, distance=0xb332),
    S('standing_back_LP_close', [(5, 'B', 19), (20, 'LP')], ticks=24, distance=0x8000),
    S('standing_back_LK_close', [(5, 'B', 19), (20, 'LK')], ticks=24, distance=0x8000),
    S('standing_toward_LP_close', [(5, 'F', 19), (20, 'LP')], ticks=24, distance=0x8000),
    S('standing_toward_LK', [(5, 'F', 19), (20, 'LK')], ticks=24),
    S('standing_toward_HK', [(5, 'F', 19), (20, 'HK')], ticks=24),
    S('crouching_LP_armed', [(20, 'LP')], ticks=24, stance='crouching', weapon=SWORD // 4),
    S('followup_4e7078_HP_armed', [(20, 'HP')], ticks=24, stance='followup_4e7078', weapon=SWORD // 4),
    S('followup_4e7078_LP_armed', [(20, 'LP')], ticks=24, stance='followup_4e7078', weapon=SWORD // 4),
    S('armed_standing_back_HK', [(5, 'B', 19), (20, 'HK')], ticks=24, stance='armed_standing', weapon=SWORD // 4),
    S('armed_standing_back_LK', [(5, 'B', 19), (20, 'LK')], ticks=24, stance='armed_standing', weapon=SWORD // 4),
]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--exe', default='MK4/MK-4.EXE')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    image = RetailImage(args.exe)
    result = dict(exe_sha256=EXE_SHA256, limits=__doc__,
                  scenarios=[scenario_result(image, scenario) for scenario in SCENARIOS])
    result = json.loads(json.dumps(result))   # JSON object keys are strings
    fixture = Path(__file__).parent / 'fixtures' / 'original_scorpion_input.json'
    if args.record:
        fixture.write_text(json.dumps(result, indent=1) + '\n')
    else:
        assert result == json.loads(fixture.read_text()), 'Scorpion input fixture changed'
    rows = {row['name']: row for row in result['scenarios']}

    def target(name):
        resolution = rows[name]['resolution']
        return resolution and (resolution['chain'] or [None])[0]
    checks = [('teleport_window_24', '0x47fc90'), ('spear_window_27', '0x49a630'),
              ('fire_window_20', '0x468610'), ('weapon_window_24', '0x46d1d0'),
              ('fatality2_window_50', '0x467cd0'), ('fatality1_distance_1cccc', '0x467ce0'),
              ('fan_fatality_stage_8', '0x467e00'), ('spike_fatality_stage_0', '0x467e70'),
              ('air_throw_both_airborne_close', '0x46aa80'), ('run_double_tap_8', '0x469250')]
    for name, va in checks:
        assert target(name) == va, (name, target(name))
        late = name.replace('24', '25').replace('27', '28').replace('20', '21').replace('_50', '_51')
        if late != name and late in rows:
            assert rows[late]['matched_word'] is None, late
    print('PASS: %d original input scenarios (switch scan, queue, task pump, matcher, predicates)' % len(rows))
    for row in result['scenarios']:
        resolution = row['resolution'] or {}
        print('  %-48s word=%-10s -> %s action=%s' % (row['name'], row['matched_word'], (resolution.get('chain') or ['-'])[0],
                                                     resolution.get('action')))


if __name__ == '__main__':
    main()
