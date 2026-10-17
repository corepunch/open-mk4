"""Typed, reviewed native boundaries for the script compiler (not x86 decompilation).

Keys are local EXE addresses. Roles prevent invoking a physics callback as a
predicate or a spawn as a synchronous native call. Arguments describe the
actual engine signature, never a guessed retail C ABI. The pinned RetailImage
hash establishes the binary revision; evidence names its inspected routines.
"""
from dataclasses import dataclass
import re


@dataclass(frozen=True)
class Binding:
    symbol: str
    source: str
    roles: tuple
    arguments: tuple = ('fighter',)
    result: str = 'void'
    flow: str = 'immediate'
    evidence: str = ''
    adaptation: str = ''

    def __post_init__(self):
        if not re.fullmatch(r'[A-Za-z_]\w*', self.symbol):
            raise ValueError('Invalid C binding symbol')
        if not self.roles or not set(self.roles) <= {'call', 'tail', 'predicate', 'physics', 'spawn', 'handler'}:
            raise ValueError('Invalid binding roles')
        if not set(self.arguments) <= {'fighter', 'arg_u8', 'task_type', 'selector_u32', 'word0_i32', 'word1_i32', 'word2_i32'}:
            raise ValueError('Invalid binding argument type')
        if self.result not in ('void', 'flags') or self.flow not in ('immediate', 'suspend', 'terminal', 'independent'):
            raise ValueError('Invalid binding result/flow')
        if not self.source or not self.evidence:
            raise ValueError('Binding requires source and evidence')
        if 'predicate' in self.roles and (self.result != 'flags' or self.flow != 'immediate'):
            raise ValueError('Predicate must return flags immediately')
        if 'physics' in self.roles and (self.arguments != ('fighter',) or self.result != 'void'):
            raise ValueError('Physics callback must have FighterFn signature')
        if 'spawn' in self.roles and self.flow != 'independent':
            raise ValueError('Spawn must create an independent lifetime')


BINDINGS = {
    0x46a950: Binding('mk4_script_avoid_clear', 'game/script.c', ('call',), evidence='46a950 clears task +5c'),
    0x46a970: Binding('mk4_script_avoid_set', 'game/script.c', ('call',), evidence='46a970 sets task +5c'),
    0x46a8d0: Binding('mk4_script_reverse_return', 'game/script.c', ('tail',), flow='terminal',
                      evidence='46a8d0 -> 429380/429600 decrements through sample zero then 46f650'),
    0x459800: Binding('mk4_script_attack_return', 'game/script.c', ('handler',),
                      evidence='459800 -> 470ed0 -> 470f00 clears busy when follow-up flag is set',
                      adaptation='CPU continuation choices remain in the shared portable CPU controller'),
    0x4596e0: Binding('mk4_script_indexed_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='4596e0 -> 48a100, indexed sound bank'),
}

# Character-specific boundaries reuse existing object and landing helpers.
# Entries are added with the actual integration, not inferred from HELPERS prose.
CHARACTER_BINDINGS = {
    3: {
        0x49c110: Binding('liu_script_spawn_low', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'),
                         flow='independent', evidence='49c110/4ed740, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c160: Binding('liu_script_spawn_high', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'),
                         flow='independent', evidence='49c160/4ed760, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c0d0: Binding('liu_script_spawn_air', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'),
                         flow='independent', evidence='49c0d0/4ed730, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c010: Binding('liu_script_air_return', 'game/c_liu_kang.c', ('tail',), flow='terminal',
                         evidence='49c010 restores gravity 0x20c, waits for the floor and returns through shared landing'),
    },
}


def bindings_for(character):
    return {**BINDINGS, **CHARACTER_BINDINGS.get(character, {})}
