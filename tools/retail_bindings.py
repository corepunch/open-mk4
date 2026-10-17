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
        if not set(self.arguments) <= {'fighter', 'arg_u8', 'task_type', 'selector_u32', 'word0_i32', 'word1_i32', 'word2_i32', 'word3_i32', 'inline_lo_u12', 'inline_hi_u12'}:
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
BINDINGS[0x459720] = Binding('mk4_script_roster_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='459720 -> 489ff0, selector8 table53d000 and remap4ead18', adaptation='Shared audio loader resolves roster throw voices from the same table', inline_values=(1, 8))
BINDINGS[0x45abf0] = Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', evidence='45abf0 -> 431520 -> descriptor4de9f0, native camera task431700', adaptation='Portable camera tracks live paired actors; do not allocate the retail camera task', inline_values=(1, 2, 3, 4, 6, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62))
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
CHARACTER_BINDINGS[4] = {
    0x470420: Binding('script_ready', 'game/c_sub_zero.c', ('tail',), flow='terminal', evidence='470420 controller return; reaction 102 routes to native 488b80 getup'),
    0x46a790: Binding('script_kick_tail', 'game/c_sub_zero.c', ('tail',), evidence='46a790; bounded native listing; docs/character-script-port.md'),
    0x46b740: Binding('script_crouch_lk_tail', 'game/c_sub_zero.c', ('tail',), evidence='46b740; bounded native listing; docs/character-script-port.md'),
    0x46bb60: Binding('script_knee_tail', 'game/c_sub_zero.c', ('tail',), evidence='46bb60; bounded native listing; docs/character-script-port.md'),
    0x46bf10: Binding('script_close_hp_tail', 'game/c_sub_zero.c', ('tail',), evidence='46bf10; bounded native listing; docs/character-script-port.md'),
    0x46c2c0: Binding('script_combo_hp_tail', 'game/c_sub_zero.c', ('tail',), evidence='46c2c0; bounded native listing; docs/character-script-port.md'),
    0x46c7f0: Binding('script_air_kick_tail', 'game/c_sub_zero.c', ('tail',), evidence='46c7f0; bounded native listing; docs/character-script-port.md'),
    0x46cde0: Binding('uppercut_script', 'game/c_sub_zero.c', ('tail',), evidence='46cde0; bounded native listing; docs/character-script-port.md'),
    0x46d260: Binding('script_draw_equip', 'game/c_sub_zero.c', ('tail',), evidence='46d260; bounded native listing; docs/character-script-port.md'),
    0x46e070: Binding('script_air_lk_tail', 'game/c_sub_zero.c', ('tail',), evidence='46e070; bounded native listing; docs/character-script-port.md'),
    0x46e4d0: Binding('script_air_diagonal_tail', 'game/c_sub_zero.c', ('tail',), evidence='46e4d0; bounded native listing; docs/character-script-port.md'),
    0x46e730: Binding('script_air_hp_tail', 'game/c_sub_zero.c', ('tail',), evidence='46e730; bounded native listing; docs/character-script-port.md'),
    0x46eea0: Binding('script_air_lp_tail', 'game/c_sub_zero.c', ('tail',), evidence='46eea0; bounded native listing; docs/character-script-port.md'),
    0x46f080: Binding('script_sweep_tail', 'game/c_sub_zero.c', ('tail',), evidence='46f080; bounded native listing; docs/character-script-port.md'),
    0x46f220: Binding('script_roundhouse_start', 'game/c_sub_zero.c', ('tail',), evidence='46f220; bounded native listing; docs/character-script-port.md'),
    0x46f270: Binding('script_roundhouse_tail', 'game/c_sub_zero.c', ('tail',), evidence='46f270; bounded native listing; docs/character-script-port.md'),
    0x42c200: Binding('script_lk_rebase', 'game/c_sub_zero.c', ('tail',), evidence='42c200; bounded native listing; docs/character-script-port.md'),
    0x42c220: Binding('script_lk_damage', 'game/c_sub_zero.c', ('tail',), evidence='42c220; bounded native listing; docs/character-script-port.md'),
    0x42c300: Binding('script_lk_owner_return', 'game/c_sub_zero.c', ('tail',), evidence='42c300; bounded native listing; docs/character-script-port.md'),
    0x42ce80: Binding('script_lp_owner_return', 'game/c_sub_zero.c', ('tail',), evidence='42ce80; bounded native listing; docs/character-script-port.md'),
    0x487220: Binding('script_lk_victim_rise', 'game/c_sub_zero.c', ('tail',), evidence='487220; bounded native listing; docs/character-script-port.md'),
    0x47dc40: Binding('slide_script', 'game/c_sub_zero.c', ('tail',), evidence='47dc40; bounded native listing; docs/character-script-port.md'),
    0x47de00: Binding('script_slide_return', 'game/c_sub_zero.c', ('tail',), evidence='47de00; bounded native listing; docs/character-script-port.md'),
    0x445150: Binding('script_first_begin', 'game/c_sub_zero.c', ('tail',), evidence='445150; bounded native listing; docs/character-script-port.md'),
    0x445220: Binding('script_first_pull', 'game/c_sub_zero.c', ('tail',), evidence='445220; bounded native listing; docs/character-script-port.md'),
    0x445240: Binding('script_first_cues1', 'game/c_sub_zero.c', ('tail',), evidence='445240; bounded native listing; docs/character-script-port.md'),
    0x445280: Binding('script_first_cues2', 'game/c_sub_zero.c', ('tail',), evidence='445280; bounded native listing; docs/character-script-port.md'),
    0x4452b0: Binding('script_first_cues3', 'game/c_sub_zero.c', ('tail',), evidence='4452b0; bounded native listing; docs/character-script-port.md'),
    0x4452e0: Binding('script_first_cues4', 'game/c_sub_zero.c', ('tail',), evidence='4452e0; bounded native listing; docs/character-script-port.md'),
    0x445310: Binding('script_first_head', 'game/c_sub_zero.c', ('tail',), evidence='445310; bounded native listing; docs/character-script-port.md'),
    0x445510: Binding('script_first_victim_blends', 'game/c_sub_zero.c', ('tail',), evidence='445510; bounded native listing; docs/character-script-port.md'),
    0x4456c0: Binding('script_first_victim_collapse', 'game/c_sub_zero.c', ('tail',), evidence='4456c0; bounded native listing; docs/character-script-port.md'),
    0x445700: Binding('script_first_victim_end', 'game/c_sub_zero.c', ('tail',), evidence='445700; bounded native listing; docs/character-script-port.md'),
    0x41b080: Binding('script_breath_victim_end', 'game/c_sub_zero.c', ('tail',), evidence='41b080; bounded native listing; docs/character-script-port.md'),
    0x41b440: Binding('script_breath_approach', 'game/c_sub_zero.c', ('tail',), evidence='41b440; bounded native listing; docs/character-script-port.md'),
    0x41b450: Binding('script_breath_slide', 'game/c_sub_zero.c', ('tail',), evidence='41b450; bounded native listing; docs/character-script-port.md'),
    0x41b570: Binding('script_breath_shatter', 'game/c_sub_zero.c', ('tail',), evidence='41b570; bounded native listing; docs/character-script-port.md'),
    0x4712e0: Binding('script_victory', 'game/c_sub_zero.c', ('tail',), evidence='4712e0; bounded native listing; docs/character-script-port.md'),
    0x43d460: Binding('script_fan_tail', 'game/c_sub_zero.c', ('tail',), evidence='43d460; bounded native listing; docs/character-script-port.md'),
    0x43d530: Binding('script_fan_cycle', 'game/c_sub_zero.c', ('tail',), evidence='43d530; bounded native listing; docs/character-script-port.md'),
    0x43d6f0: Binding('script_fan_victory', 'game/c_sub_zero.c', ('tail',), evidence='43d6f0; bounded native listing; docs/character-script-port.md'),
    0x43db50: Binding('script_fan_victim_end', 'game/c_sub_zero.c', ('tail',), evidence='43db50; bounded native listing; docs/character-script-port.md'),
    0x467fc0: Binding('script_spike_tail', 'game/c_sub_zero.c', ('tail',), evidence='467fc0; bounded native listing; docs/character-script-port.md'),
}
CHARACTER_BINDINGS[4][0x45b1f0] = Binding('script_reaction', 'game/c_sub_zero.c', ('handler',), ('fighter', 'arg_u8'), evidence='45b1f0 native contract; explicit engine body/scene context')
BINDINGS[0x45af60] = Binding('mk4_script_reaction_motion', 'game/script.c', ('handler',), ('fighter', 'word0_i32', 'word1_i32', 'word2_i32', 'word3_i32'), evidence='45af60 native contract; explicit engine body/scene context')
BINDINGS[0x4596a0] = Binding('mk4_script_context_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), evidence='4596a0 native contract; explicit engine body/scene context')
BINDINGS[0x4913a0] = Binding('fighter_friction', 'game/reactions.c', ('physics',), ('fighter',), evidence='4913a0 native contract; explicit engine body/scene context')
BINDINGS[0x4707e0] = Binding('mk4_script_landing_motion', 'game/script.c', ('physics',), ('fighter',), evidence='4707e0 native contract; explicit engine body/scene context')
CHARACTER_BINDINGS[4][0x49bcb0] = Binding('script_spawn_ice', 'game/c_sub_zero.c', ('spawn',), ('fighter', 'task_type'), evidence='49bcb0 native contract; explicit engine body/scene context', flow='independent', task_types=(66,))
CHARACTER_BINDINGS[4][0x41bbb0] = Binding('script_breath_object', 'game/c_sub_zero.c', ('spawn',), ('fighter', 'task_type'), evidence='41bbb0 native contract; explicit engine body/scene context', flow='independent', task_types=(0,))
CHARACTER_BINDINGS[4][0x41b860] = Binding('script_breath_freeze', 'game/c_sub_zero.c', ('spawn',), ('fighter', 'task_type'), evidence='41b860 native contract; explicit engine body/scene context', flow='independent', task_types=(0,))

CHARACTER_BINDINGS[4][0x46aae0] = Binding('script_air_release', 'game/c_sub_zero.c', ('tail',), evidence='46aae0 releases pair and applies D-record damage before 4e5e50')
BINDINGS[0x46ab20] = Binding('mk4_script_land', 'game/script.c', ('tail',), flow='terminal', evidence='46ab20 shared landing return')
BINDINGS[0x484b60] = Binding('mk4_script_air_getup', 'game/mk4_common.c', ('tail',), flow='terminal', evidence='484b60 released air victim native getup')


CHARACTER_BINDINGS[0] = {
    address: Binding(binding.symbol, 'game/c_scorpion.c', binding.roles, binding.arguments,
                     result=binding.result, flow=binding.flow, cursor_effect=binding.cursor_effect,
                     evidence=binding.evidence, inline_values=binding.inline_values)
    for address, binding in CHARACTER_BINDINGS[4].items()
    if address in (0x46b740, 0x46bb60, 0x46bf10, 0x46c2c0, 0x46c7f0, 0x46cde0, 0x46d260,
                   0x46e730, 0x46eea0, 0x46f080, 0x46f220, 0x46f270, 0x46aae0,
                   0x43d460, 0x43d530, 0x43d6f0, 0x43db50, 0x467fc0)
}

CHARACTER_BINDINGS[0][0x46a760] = Binding('script_kick_tail', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='46a760 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x46df90] = Binding('script_air_lk_tail', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='46df90 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x46e3d0] = Binding('script_air_diagonal_tail', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='46e3d0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x42bdf0] = Binding('script_lk_rebase', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='42bdf0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x42be00] = Binding('script_lk_damage', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='42be00 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x42be10] = Binding('script_lk_return', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='42be10 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x42ce60] = Binding('script_lp_return', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='42ce60 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x49a680] = Binding('script_spear_return', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='49a680 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x486000] = Binding('script_spear_pull', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='486000 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x487420] = Binding('spear_stun_script', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='487420 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x47fcf0] = Binding('teleport_script', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='47fcf0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x47ffc0] = Binding('script_teleport_land', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='47ffc0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x468640] = Binding('fire_script', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='468640 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x483df0] = Binding('script_burn_tail', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='483df0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x484c00] = Binding('script_teleport_victim_tail', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='484c00 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x443930] = Binding('script_burn_begin', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='443930 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x443a30] = Binding('script_burn_victim_start', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='443a30 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x443a70] = Binding('script_burn_repeat1', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='443a70 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x443b40] = Binding('script_burn_repeat2', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='443b40 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x443c10] = Binding('script_burn_victory', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='443c10 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x444000] = Binding('script_burn_victim_cue', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='444000 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x444030] = Binding('script_burn_victim_repeat', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='444030 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x444060] = Binding('script_burn_victim_blend', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='444060 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x444100] = Binding('script_burn_victim_impact', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='444100 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x444150] = Binding('script_burn_victim_settle', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='444150 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x41ea50] = Binding('script_beast_begin', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='41ea50 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x41ebd0] = Binding('script_beast_attack', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='41ebd0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x41ec70] = Binding('script_beast_victim_pieces', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='41ec70 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x41f1b0] = Binding('script_beast_park', 'game/c_scorpion.c', ('tail',), ('fighter',), evidence='41f1b0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x416ae0] = Binding('script_fire_decor', 'game/c_scorpion.c', ('call',), ('fighter',), evidence='416ae0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x416d40] = Binding('script_fire_flame', 'game/c_scorpion.c', ('call',), ('fighter',), evidence='416d40 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x48a1f0] = Binding('script_beast_voice', 'game/c_scorpion.c', ('call',), ('fighter',), evidence='48a1f0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x480040] = Binding('script_teleport_motion', 'game/c_scorpion.c', ('physics',), ('fighter',), evidence='480040 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x45b1f0] = Binding('script_reaction', 'game/c_scorpion.c', ('handler',), ('fighter', 'arg_u8'), evidence='45b1f0 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x45b230] = Binding('script_opponent_reaction', 'game/c_scorpion.c', ('handler',), ('fighter', 'arg_u8'), evidence='45b230 exact-entry native contract; docs/scorpion-retail-moves.md')
CHARACTER_BINDINGS[0][0x49a6a0] = Binding('script_spawn_spear', 'game/c_scorpion.c', ('spawn',), ('fighter', 'task_type'), evidence='49a6a0 exact-entry native contract; docs/scorpion-retail-moves.md', flow='independent', task_types=(67,))
CHARACTER_BINDINGS[0][0x450d40] = Binding('script_transform_effect', 'game/c_scorpion.c', ('spawn',), ('fighter', 'task_type'), evidence='450d40 exact-entry native contract; docs/scorpion-retail-moves.md', flow='independent', task_types=(0,))
CHARACTER_BINDINGS[0][0x41ed80] = Binding('script_beast_gore', 'game/c_scorpion.c', ('spawn',), ('fighter', 'task_type'), evidence='41ed80 exact-entry native contract; docs/scorpion-retail-moves.md', flow='independent', task_types=(0,))
BINDINGS[0x45afe0] = Binding('fighter_set_speed', 'game/moves.c', ('handler',), ('fighter', 'word0_i32'), evidence='45afe0 -> 490c60 horizontal signed velocity')
BINDINGS[0x4595a0] = Binding('mk4_script_shake', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), evidence='4595a0 -> 457b50 camera pulse selector table 4e3638', inline_values=(0, 1, 2, 3, 4, 5, 6))
BINDINGS[0x45b310] = Binding('mk4_script_effect', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), evidence='45b310 -> 40a470 independent decorative effect selector', inline_values=tuple(range(20)))

CHARACTER_BINDINGS[0][0x444190] = Binding('script_burn_victim_end', 'game/c_scorpion.c', ('tail',), evidence='444190 native scene completion after 4e0358')

for _roster in (0, 4):
    for _address, _binding in tuple(CHARACTER_BINDINGS[_roster].items()):
        if _binding.roles == ('tail',) and _address != 0x43d530:
            from dataclasses import replace as _replace_binding
            CHARACTER_BINDINGS[_roster][_address] = _replace_binding(_binding, flow='terminal')


BINDINGS[0x46eab0] = Binding('weapon_air_native', 'game/weapons.c', ('tail',), ('fighter', 'selector_u32'), flow='terminal', evidence='46eab0: weapon air HP native runner, selector 0x104')
BINDINGS[0x46ec10] = Binding('weapon_air_native', 'game/weapons.c', ('tail',), ('fighter', 'selector_u32'), flow='terminal', evidence='46ec10: weapon air LP native runner, selector 0x108')
CHARACTER_BINDINGS[0][0x4807b0] = Binding('spin_script', 'game/c_scorpion.c', ('tail',), flow='terminal', evidence='4807b0 native contact/travel loop after VM startup 4e8850')

# Raiden and Sonya: shared native runners retain contact, physics and scene
# ownership; their VM bodies are generated from the selected roster streams.
for _character, _slug in ((1, 'raiden'), (2, 'sonya')):
    _source = 'game/c_%s.c' % _slug
    _normal_tails = {
        0x46b740: 'script_crouch_lk_tail', 0x46bb60: 'script_knee_tail',
        0x46bf10: 'script_close_hp_tail', 0x46c2c0: 'script_combo_hp_tail',
        0x46cde0: 'uppercut_script', 0x46d260: 'script_draw_equip',
        0x46e730: 'script_air_hp_tail', 0x46eea0: 'script_air_lp_tail',
        0x43d460: 'script_fan_tail', 0x43d6f0: 'script_fan_victory',
        0x467fc0: 'script_spike_tail', 0x46aae0: 'script_air_release',
    }
    _normal_tails.update({0x46a790: 'script_kick_tail', 0x46c970: 'script_air_kick_tail',
        0x46e070: 'script_air_lk_tail', 0x46e4d0: 'script_air_diagonal_tail',
        0x46efd0: 'script_sweep_tail', 0x46f390: 'script_roundhouse_start',
        0x46f3e0: 'script_roundhouse_tail', 0x42cc00: 'script_lp_select',
        0x42cc40: 'script_lp_end', 0x42ce80: 'script_lp_return',
        0x47d680: 'script_torpedo_miss', 0x47d7f0: 'script_torpedo_contact',
    } if _character == 1 else {
        0x46a6b0: 'script_kick_tail', 0x46c7f0: 'script_air_kick_tail',
        0x46e150: 'script_air_lk_tail', 0x46e5c0: 'script_air_diagonal_tail',
        0x46f080: 'script_sweep_tail', 0x46f220: 'script_roundhouse_start',
        0x46f270: 'script_roundhouse_tail', 0x42bf00: 'script_lk_return',
        0x47b6c0: 'script_cartwheel_tail', 0x47e0a0: 'script_leg_grab_tail',
        0x47e1d0: 'script_square_wave_tail',
    })
    CHARACTER_BINDINGS[_character] = {
        _address: Binding(_name, _source, ('tail',), evidence='%x exact native entry; docs/raiden-sonya-scripts.md' % _address)
        for _address, _name in _normal_tails.items()
    }
    for _address in (0x45b1f0, 0x45b230):
        CHARACTER_BINDINGS[_character][_address] = Binding('script_reaction', _source, ('handler',), ('fighter', 'arg_u8'),
            evidence='%x reaction dispatch; native pair adapter owns independent victim recovery' % _address)
    CHARACTER_BINDINGS[_character][0x43d530] = Binding('mk4_script_fan_cycle', 'game/script.c', ('tail',), evidence='43d530 returns to the C scene coordinator')
    CHARACTER_BINDINGS[_character][0x43db50] = Binding('mk4_script_fan_victim_end', 'game/script.c', ('tail',), evidence='43db50 arena release/hide; existing shared scene helper')
    CHARACTER_BINDINGS[_character][0x484b60] = Binding('mk4_script_air_getup', 'game/mk4_common.c', ('tail',), evidence='484b60 airborne throw victim getup; shared native continuation')
    if _character == 1:
        CHARACTER_BINDINGS[_character][0x47d6e0] = Binding('script_torpedo_motion', _source, ('physics',), evidence='47d6e0 installed torpedo horizontal motion callback')
        CHARACTER_BINDINGS[_character][0x47d720] = Binding('script_torpedo_step', _source, ('call',), evidence='47d720 floor clamp and torpedo pose advancement')
        CHARACTER_BINDINGS[_character][0x48d450] = Binding('script_torpedo_query', _source, ('predicate',), result='flags', evidence='48d450 D-table contact; selector 0 supplied by torpedo script')
        CHARACTER_BINDINGS[_character][0x49be10] = Binding('script_lightning_spawn', _source, ('spawn',), ('fighter', 'task_type'), flow='independent', task_types=(65,), evidence='49be10 independent lightning object, task 65')
    else:
        CHARACTER_BINDINGS[_character][0x49ba50] = Binding('script_ring_spawn', _source, ('spawn',), ('fighter', 'task_type'), flow='independent', task_types=(68,), evidence='49ba50 independent Sonya ring object, task 68')
        CHARACTER_BINDINGS[_character][0x49b9e0] = Binding('script_ring_flash', _source, ('spawn',), ('fighter', 'task_type'), flow='independent', task_types=(38,), evidence='49b9e0 independent launch flash, task 38')

for _character, _address in ((1, 0x485ad0), (2, 0x485b50)):
    CHARACTER_BINDINGS[_character][_address] = Binding('script_throw_getup', 'game/c_%s.c' % ('raiden' if _character == 1 else 'sonya'), ('tail',), evidence='%x native victim release/getup; bounded listing' % _address)
CHARACTER_BINDINGS[1][0x470420] = Binding('script_thrown_return', 'game/c_raiden.c', ('tail',), flow='terminal', evidence='470420 reaction 102 getup adaptation after generated 4e9bd0')
CHARACTER_BINDINGS[1][0x485510] = Binding('script_lk_victim_end', 'game/c_raiden.c', ('tail',), evidence='485510 native throw victim landing/recovery after 4e99c0')
CHARACTER_BINDINGS[2][0x4858d0] = Binding('script_leg_victim_end', 'game/c_sonya.c', ('tail',), evidence='4858d0 binds landing pose, waits for ground then enters native getup 328')
# Scene and weapon continuations reached by the two complete profiles.
for _cid, _slug, _tails in (
    (1, 'raiden', {0x441140: 'script_first_release', 0x441180: 'script_first_end',
        0x464960: 'script_second_blend', 0x464a60: 'script_second_effect',
        0x464aa0: 'script_second_release', 0x464bb0: 'script_second_end'}),
    (2, 'sonya', {0x43f510: 'script_first_release', 0x465e10: 'script_second_open',
        0x465e70: 'script_second_repeat', 0x465ea0: 'script_second_branch',
        0x465ed0: 'script_second_pause', 0x466000: 'script_second_cycle_end',
        0x466190: 'script_second_victim_end', 0x496770: 'script_weapon_hp_tail',
        0x496970: 'script_weapon_lp_tail'}),
):
    for _address, _name in _tails.items():
        CHARACTER_BINDINGS[_cid][_address] = Binding(_name, 'game/c_%s.c' % _slug, ('tail',), evidence='%x exact native scene/weapon entry; docs/raiden-sonya-scripts.md' % _address)
    CHARACTER_BINDINGS[_cid][0x464bd0] = Binding('mk4_script_scene_task', 'game/script.c', ('spawn',), ('fighter', 'task_type'),
        flow='independent', task_types=(619,), evidence='464bd0 allocates a task that only waits one tick and repeats; portable object pool retains its scene lifetime')
CHARACTER_BINDINGS[1][0x427b70] = Binding('script_second_particles', 'game/c_raiden.c', ('call',), evidence='427b70 spawns task 619; 427b90 independent scene emitter')
CHARACTER_BINDINGS[1][0x464a90] = Binding('retail_script_004e5728', 'game/generated/raiden.inc', ('tail',), script_target=0x4e5728, evidence='464a90 only enters script 4e5728')
BINDINGS[0x45abf0] = Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), evidence='45abf0 -> 431520 context-camera selector, including finisher contexts 75..79', inline_values=(1, 2, 3, 4, 6, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62, 75, 76, 77, 79))
CHARACTER_BINDINGS[1][0x464ee0] = Binding('script_second_victim_end', 'game/c_raiden.c', ('tail',), evidence='464ee0 independent scene task then script 4e5788')
for _cid, _slug in ((1, 'raiden'), (2, 'sonya')):
    CHARACTER_BINDINGS[_cid][0x4712e0] = Binding('script_victory', 'game/c_%s.c' % _slug, ('tail',), evidence='4712e0 selects roster victory script 4e6a40')
CHARACTER_BINDINGS[1][0x410380] = Binding('script_victory_effect', 'game/c_raiden.c', ('call',), evidence='410380 allocates decorative task 410390; particles use existing independent presentation lifetimes')
del CHARACTER_BINDINGS[2][0x46aae0]
CHARACTER_BINDINGS[2][0x46aed0] = Binding('script_air_release', 'game/c_sonya.c', ('tail',), evidence='46aed0 releases reaction 85 at paired count 24, damage 0x2666, owner gravity 0x20c')
CHARACTER_BINDINGS[2][0x4847e0] = Binding('script_air_getup', 'game/c_sonya.c', ('tail',), evidence='4847e0 finishes clip 198 and selects 53cf70 getup')
BINDINGS[0x490290] = Binding('mk4_script_touchable', 'game/script.c', ('call',), evidence='490290 clears group flag 0x80')
BINDINGS[0x494520] = Binding('mk4_script_disarm_self', 'game/script.c', ('call',), evidence='494520 drops the active context weapon; 483ae0 invokes it in opponent context')
BINDINGS[0x45b2f0] = Binding('mk4_script_wait_land', 'game/script.c', ('handler',), flow='suspend', evidence='45b2f0 waits through 48f390 landing continuation')
for _cid, _slug in ((1, 'raiden'), (2, 'sonya')):
    CHARACTER_BINDINGS[_cid][0x482790] = Binding('script_weapon_victim_getup', 'game/c_%s.c' % _slug, ('tail',), evidence='482790 completes native reverse/recovery and getup 328')
    CHARACTER_BINDINGS[_cid][0x488b40] = Binding('script_reaction_side_getup', 'game/c_%s.c' % _slug, ('tail',), evidence='488b40 native side getup clip 185; skip standing after KO')
CHARACTER_BINDINGS[1][0x488590] = Binding('script_staff_victim_tail', 'game/c_raiden.c', ('tail',), evidence='488590 twenty visits of native weapon-victim motion before recovery')
CHARACTER_BINDINGS[1][0x4889b0] = Binding('script_torpedo_victim_motion', 'game/c_raiden.c', ('physics',), evidence='4889b0 paired torpedo victim follows attacker and retains its own pose loop')
CHARACTER_BINDINGS[2][0x488c10] = Binding('script_weapon_victim_getup', 'game/c_sonya.c', ('tail',), evidence='488c10 KO-aware front/back getup; uses shared native getup')
BINDINGS[0x46a850] = Binding('mk4_script_reverse_return', 'game/script.c', ('tail',), flow='terminal', evidence='46a850 -> 429380 reverses the current clip then 46f650 controller return')
BINDINGS[0x46a6e0] = Binding('mk4_script_reverse_return', 'game/script.c', ('tail',), flow='terminal', evidence='46a6e0 kick reversal -> 429380 then 46f650 controller return')
BINDINGS[0x45bbc0] = Binding('mk4_script_pair_back_to', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), flow='suspend', cursor_effect='unknown', evidence='45bbc0 -> 45beb0 decrements owner and victim cursors to the inline target')

def bindings_for(character):
    from character_catalog import CHARACTERS
    links = {address: Binding('retail_script_%08x' % target, 'game/generated/%s.inc' % CHARACTERS[character], ('tail',),
                              script_target=target, evidence='%x: script_enter(%x); exact-entry native listing' % (address, target))
             for address, target in ((0x46dd00, 0x4e6330), (0x46dd10, 0x4e6360), (0x46dd20, 0x4e63b0))}
    return {**BINDINGS, **links, **CHARACTER_BINDINGS.get(character, {})}
