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
    task_types: tuple = ()
    script_target: int = 0
    inline_values: tuple = ()
    cursor_effect: str = 'preserve'

    def __post_init__(self):
        if self.cursor_effect not in ('preserve', 'finish', 'unknown'):
            raise ValueError('Invalid binding cursor effect')
        if self.script_target and (self.roles != ('tail',) or self.flow != 'immediate'):
            raise ValueError('Script link must be an immediate static native tail')
        if not re.fullmatch(r'[A-Za-z_]\w*', self.symbol):
            raise ValueError('Invalid C binding symbol')
        if not self.roles or not set(self.roles) <= {'call', 'tail', 'predicate', 'physics', 'spawn', 'handler'}:
            raise ValueError('Invalid binding roles')
        if not set(self.arguments) <= {'fighter', 'arg_u8', 'task_type', 'selector_u32', 'word0_i32', 'word1_i32', 'word2_i32', 'inline_lo_u12', 'inline_hi_u12'}:
            raise ValueError('Invalid binding argument type')
        if self.result not in ('void', 'flags') or self.flow not in ('immediate', 'suspend', 'terminal', 'independent'):
            raise ValueError('Invalid binding result/flow')
        if not self.source or not self.evidence:
            raise ValueError('Binding requires source and evidence')
        if 'predicate' in self.roles and (self.result != 'flags' or self.flow != 'immediate'):
            raise ValueError('Predicate must return flags immediately')
        if 'physics' in self.roles and (self.arguments != ('fighter',) or self.result != 'void'):
            raise ValueError('Physics callback must have FighterFn signature')
        if 'spawn' in self.roles and (self.flow != 'independent' or not self.task_types):
            raise ValueError('Spawn must declare independent lifetime and accepted task types')
        if 'task_type' in self.arguments and set(self.roles) != {'spawn'}:
            raise ValueError('Task type is only valid for a spawn binding')
        if self.flow == 'terminal' and set(self.roles) != {'tail'}:
            raise ValueError('Terminal flow must be a native tail')


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
                         flow='independent', task_types=(64,), evidence='49c110/4ed740, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c160: Binding('liu_script_spawn_high', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'),
                         flow='independent', task_types=(64,), evidence='49c160/4ed760, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c0d0: Binding('liu_script_spawn_air', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'),
                         flow='independent', task_types=(64,), evidence='49c0d0/4ed730, independent task 64; see docs/liu-kang-scripts.md'),
        0x49c010: Binding('liu_script_air_return', 'game/c_liu_kang.c', ('tail',), flow='terminal',
                         evidence='49c010 restores gravity 0x20c, waits for the floor and returns through shared landing'),
    },
}


# Native continuations retain only native work. Script-only native thunks are
# linked statically into the generated CFG, including cycles, without C recursion.
CHARACTER_BINDINGS[3][0x42c0b0] = Binding('liu_script_lk_continue', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='42c0b0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x42c0d0] = Binding('liu_script_lk_damage', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='42c0d0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x43c500] = Binding('liu_script_fatal_setup', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='43c500; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x43d460] = Binding('liu_script_fan_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='43d460; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45a780] = Binding('mk4_script_current', 'game/script.c', ('handler',), ('fighter',), result='void', flow='suspend', cursor_effect='finish', evidence='45a780; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x45ac30] = Binding('liu_script_scene_event', 'game/c_liu_kang.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='45ac30; bounded native listing and docs/liu-kang-scripts.md', inline_values=(42, 43, 44))
CHARACTER_BINDINGS[3][0x45b050] = Binding('liu_script_motion', 'game/c_liu_kang.c', ('handler',), ('fighter', 'word0_i32', 'inline_lo_u12', 'inline_hi_u12'), result='void', evidence='45b050; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x45b1f0] = Binding('liu_script_reaction', 'game/c_liu_kang.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='45b1f0; bounded native listing and docs/liu-kang-scripts.md', inline_values=(80, 104, 176))
CHARACTER_BINDINGS[3][0x467fc0] = Binding('liu_script_spike_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='467fc0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46a760] = Binding('liu_script_kick_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46a760; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46b740] = Binding('liu_script_crouch_lk_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46b740; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46bb60] = Binding('liu_script_knee_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46bb60; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46bf10] = Binding('liu_script_close_hp_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46bf10; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46c2c0] = Binding('liu_script_combo_hp_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46c2c0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46c970] = Binding('liu_script_air_kick_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46c970; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46cde0] = Binding('uppercut_script', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46cde0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46d260] = Binding('liu_script_draw_equip', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46d260; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46db00] = Binding('liu_script_sword_gate', 'game/c_liu_kang.c', ('predicate',), ('fighter',), result='flags', evidence='46db00; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46df90] = Binding('liu_script_air_lk_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46df90; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46e3d0] = Binding('liu_script_air_diagonal_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46e3d0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46e730] = Binding('liu_script_air_hp_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46e730; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46eea0] = Binding('liu_script_air_lp_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46eea0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46efd0] = Binding('liu_script_sweep_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46efd0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46f390] = Binding('liu_script_roundhouse_start', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46f390; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x46f3e0] = Binding('liu_script_roundhouse_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='46f3e0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x47eb30] = Binding('bicycle_script', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47eb30; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x47ee00] = Binding('flying_kick_script', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47ee00; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x480e90] = Binding('liu_script_bicycle_motion', 'game/c_liu_kang.c', ('physics',), ('fighter',), result='void', evidence='480e90; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x485930] = Binding('liu_script_lk_thrown_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='485930; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x485b10] = Binding('liu_script_lp_getup', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='485b10; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x488b40] = Binding('liu_script_bicycle_getup', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='488b40; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x48f9f0] = Binding('liu_script_fatal_align', 'game/c_liu_kang.c', ('call',), ('fighter',), result='void', evidence='48f9f0; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x496430] = Binding('liu_script_sword_blend', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='496430; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x496970] = Binding('liu_script_back_lp_tail', 'game/c_liu_kang.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='496970; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[3][0x43c540] = Binding('liu_script_fatal_spawn', 'game/c_liu_kang.c', ('spawn',), ('fighter', 'task_type'), flow='independent', task_types=(38,), evidence='43c540 independent fireball/explosion task; 4891c0/489110 completion')

CHARACTER_BINDINGS[3][0x42c170] = Binding('retail_script_004de478', 'game/generated/liu_kang.inc', ('tail',), evidence='42c170 script_enter(4de478)', script_target=0x4de478)
CHARACTER_BINDINGS[3][0x43d530] = Binding('liu_script_fan_cycle', 'game/c_liu_kang.c', ('tail',), evidence='43d530/43d540 decrements native scene loop counter; C caller owns repetition')
CHARACTER_BINDINGS[3][0x43d6f0] = Binding('liu_script_fan_victory', 'game/c_liu_kang.c', ('tail',), evidence='43d6f0 clears motion and enters 4e0158')
CHARACTER_BINDINGS[3][0x43db50] = Binding('liu_script_fan_victim_end', 'game/c_liu_kang.c', ('tail',), evidence='43db50 victim arena release/hide native continuation')

BINDINGS[0x4595e0] = Binding('mk4_script_draw_voice', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='4595e0; bounded native listing and docs/liu-kang-scripts.md', inline_values=(0,))
BINDINGS[0x459720] = Binding('mk4_script_roster_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='459720 -> 489ff0, selector8 table53d000 and remap4ead18', adaptation='Shared audio loader resolves roster throw voices from the same table', inline_values=(8,))
BINDINGS[0x45abf0] = Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='45abf0 -> 431520 -> descriptor4de9f0, native camera task431700', adaptation='Portable camera tracks live paired actors; do not allocate the retail camera task', inline_values=(1, 2, 4, 22, 23, 36, 56, 57, 58, 60, 62))
BINDINGS[0x45ae50] = Binding('mk4_script_pair_adjust', 'game/script.c', ('handler',), ('fighter', 'word0_i32'), result='void', evidence='45ae50; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b030] = Binding('mk4_script_disarm', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45b030; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b1b0] = Binding('mk4_script_impact', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45b1b0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b1d0] = Binding('mk4_script_impact_callback', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45b1d0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b350] = Binding('mk4_script_damage', 'game/script.c', ('handler',), ('fighter', 'word0_i32'), result='void', evidence='45b350; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45baf0] = Binding('mk4_script_scene_active', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45baf0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45bb10] = Binding('mk4_script_pair_to', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', flow='suspend', cursor_effect='unknown', evidence='45bb10; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x46ddd0] = Binding('mk4_script_jab_contact', 'game/script.c', ('call',), ('fighter',), result='void', evidence='46ddd0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x46de60] = Binding('mk4_script_jab_gate', 'game/script.c', ('call', 'predicate'), ('fighter',), result='flags', evidence='46de60; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x470420] = Binding('mk4_script_ready', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='470420; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x47ed40] = Binding('mk4_script_land', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47ed40; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x47ef10] = Binding('mk4_script_land', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47ef10; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x48f6e0] = Binding('mk4_script_save_motion', 'game/script.c', ('call',), ('fighter',), result='void', evidence='48f6e0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x48f750] = Binding('mk4_script_restore_motion', 'game/script.c', ('call',), ('fighter',), result='void', evidence='48f750; bounded native listing and docs/liu-kang-scripts.md')

def bindings_for(character):
    from character_catalog import CHARACTERS
    links = {address: Binding('retail_script_%08x' % target, 'game/generated/%s.inc' % CHARACTERS[character], ('tail',),
                              script_target=target, evidence='%x: script_enter(%x); exact-entry native listing' % (address, target))
             for address, target in ((0x46dd00, 0x4e6330), (0x46dd10, 0x4e6360), (0x46dd20, 0x4e63b0))}
    return {**BINDINGS, **links, **CHARACTER_BINDINGS.get(character, {})}
