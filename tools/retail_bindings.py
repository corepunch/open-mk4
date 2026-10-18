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
    mmove_lowering: str = ''  # explicit offline lowering for a yielding native primitive
    mmove: bool = False  # reviewed nonblocking native callback for frame-table lowering

    def __post_init__(self):
        if self.mmove_lowering not in ('', 'pair_to'):
            raise ValueError('Unknown mmove native lowering')
        if self.mmove_lowering and (self.flow != 'suspend' or self.roles != ('handler',)):
            raise ValueError('Mmove playback lowering requires a yielding handler')
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
    0x45b010: Binding('mk4_script_stop', 'game/script.c', ('handler',),
                      evidence='45b010 clears the current actor velocity, gravity and spin through stop_motion'),
    0x46a950: Binding('mk4_script_avoid_clear', 'game/script.c', ('call',), evidence='46a950 clears task +5c'),
    0x46a970: Binding('mk4_script_avoid_set', 'game/script.c', ('call',), evidence='46a970 sets task +5c'),
    0x46a8d0: Binding('mk4_script_reverse_return', 'game/script.c', ('tail',), flow='terminal',
                      evidence='46a8d0 -> 429380/429600 decrements through sample zero then 46f650'),
    0x459800: Binding('mk4_script_attack_return', 'game/script.c', ('handler',),
                      evidence='459800 -> 470ed0 -> 470f00 clears busy when follow-up flag is set',
                      adaptation='CPU continuation choices remain in the shared portable CPU controller'),
    0x4596e0: Binding('mk4_script_indexed_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='4596e0 -> 48a100, indexed sound bank'),
    0x45c150: Binding('mk4_script_guard_block_sound_26', 'game/script.c', ('handler',),
                      evidence='45c150 tests the selected player state for S_BLOCK (9), then conditionally plays indexed sound 26'),
    0x45c1c0: Binding('mk4_script_guard_block_sound_28', 'game/script.c', ('handler',),
                      evidence='45c1c0 tests the selected player state for S_BLOCK (9), then conditionally plays indexed sound 28'),
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
BINDINGS[0x45abf0] = Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', mmove=True, evidence='45abf0 -> 431520 -> descriptor4de9f0, native camera task431700', adaptation='Portable camera tracks live paired actors; do not allocate the retail camera task', inline_values=(1, 2, 3, 4, 6, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62))
BINDINGS[0x45ae50] = Binding('mk4_script_pair_adjust', 'game/script.c', ('handler',), ('fighter', 'word0_i32'), result='void', mmove=True, evidence='45ae50; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b030] = Binding('mk4_script_disarm', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45b030; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b1b0] = Binding('mk4_script_impact', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45b1b0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b1d0] = Binding('mk4_script_impact_callback', 'game/script.c', ('handler',), ('fighter',), result='void', mmove=True, evidence='45b1d0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45b350] = Binding('mk4_script_damage', 'game/script.c', ('handler',), ('fighter', 'word0_i32'), result='void', mmove=True, evidence='45b350; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45baf0] = Binding('mk4_script_scene_active', 'game/script.c', ('handler',), ('fighter',), result='void', evidence='45baf0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x45bb10] = Binding('mk4_script_pair_to', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), result='void', flow='suspend', mmove_lowering='pair_to', cursor_effect='unknown', evidence='45bb10; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x46ddd0] = Binding('mk4_script_jab_contact', 'game/script.c', ('call',), ('fighter',), result='void', evidence='46ddd0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x46de60] = Binding('mk4_script_jab_gate', 'game/script.c', ('call', 'predicate'), ('fighter',), result='flags', evidence='46de60; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x470420] = Binding('mk4_script_ready', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='470420; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x47ed40] = Binding('mk4_script_land', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47ed40; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x47ef10] = Binding('mk4_script_land', 'game/script.c', ('tail',), ('fighter',), result='void', flow='terminal', evidence='47ef10; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x48f6e0] = Binding('mk4_script_save_motion', 'game/script.c', ('call',), ('fighter',), result='void', evidence='48f6e0; bounded native listing and docs/liu-kang-scripts.md')
BINDINGS[0x48f750] = Binding('mk4_script_restore_motion', 'game/script.c', ('call',), ('fighter',), result='void', evidence='48f750; bounded native listing and docs/liu-kang-scripts.md')
CHARACTER_BINDINGS[4] = {
    0x470420: Binding('script_ready', 'game/c_sub_zero.c', ('tail',), flow='terminal', mmove=True, evidence='470420 controller return; reaction 102 routes to native 488b80 getup'),
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
    0x46f080: Binding('script_sweep_tail', 'game/c_sub_zero.c', ('tail',), mmove=True, evidence='46f080; bounded native listing; docs/character-script-port.md'),
    0x46f220: Binding('script_roundhouse_start', 'game/c_sub_zero.c', ('tail',), evidence='46f220; bounded native listing; docs/character-script-port.md'),
    0x46f270: Binding('script_roundhouse_tail', 'game/c_sub_zero.c', ('tail',), evidence='46f270; bounded native listing; docs/character-script-port.md'),
    0x42c200: Binding('script_lk_rebase', 'game/c_sub_zero.c', ('tail',), evidence='42c200; bounded native listing; docs/character-script-port.md'),
    0x42c220: Binding('script_lk_damage', 'game/c_sub_zero.c', ('tail',), evidence='42c220; bounded native listing; docs/character-script-port.md'),
    0x42c300: Binding('script_lk_owner_return', 'game/c_sub_zero.c', ('tail',), evidence='42c300; bounded native listing; docs/character-script-port.md'),
    0x42ce80: Binding('script_lp_owner_return', 'game/c_sub_zero.c', ('tail',), mmove=True, evidence='42ce80; bounded native listing; docs/character-script-port.md'),
    0x487220: Binding('script_lk_victim_rise', 'game/c_sub_zero.c', ('tail',), evidence='487220; bounded native listing; docs/character-script-port.md'),
    0x47dc40: Binding('slide_script', 'game/c_sub_zero.c', ('tail',), mmove=True, evidence='47dc40; bounded native listing; docs/character-script-port.md'),
    0x47de00: Binding('script_slide_return', 'game/c_sub_zero.c', ('tail',), mmove=True, evidence='47de00; bounded native listing; docs/character-script-port.md'),
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
CHARACTER_BINDINGS[4][0x45b1f0] = Binding('script_reaction', 'game/c_sub_zero.c', ('handler',), ('fighter', 'arg_u8'), mmove=True, evidence='45b1f0 native contract; explicit engine body/scene context')
BINDINGS[0x45af60] = Binding('mk4_script_reaction_motion', 'game/script.c', ('handler',), ('fighter', 'word0_i32', 'word1_i32', 'word2_i32', 'word3_i32'), mmove=True, evidence='45af60 native contract; explicit engine body/scene context')
BINDINGS[0x4596a0] = Binding('mk4_script_context_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), mmove=True, evidence='4596a0 native contract; explicit engine body/scene context')
BINDINGS[0x4913a0] = Binding('fighter_friction', 'game/reactions.c', ('physics',), ('fighter',), evidence='4913a0 native contract; explicit engine body/scene context')
BINDINGS[0x4707e0] = Binding('mk4_script_landing_motion', 'game/script.c', ('physics',), ('fighter',), mmove=True, evidence='4707e0 native contract; explicit engine body/scene context')
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
BINDINGS[0x45b310] = Binding('mk4_script_effect', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), evidence='45b310 -> 40a470 independent decorative effect selector, including Goro throw selector 21', inline_values=tuple(range(22)))

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
BINDINGS[0x45abf0] = Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'), mmove=True, evidence='45abf0 -> 431520 context-camera selector, including Fujin throw contexts 11/22 and finisher contexts 75..79', inline_values=(1, 2, 3, 4, 6, 11, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62, 75, 76, 77, 79))
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

# Reiko's selected roots share native continuations with the established
# roster ports. Keep the fighter-specific behavior in c_reiko.c and reuse
# only reviewed shared scene, landing and air-throw primitives.
CHARACTER_BINDINGS[7] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/reiko.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 only enters Reiko paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/reiko.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 only enters Reiko release script 4de328'),
    0x42be10: Binding('reiko_script_lk_return', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='42be10 exact-entry state return and 4de358 branch; bounded native listing'),
    0x42cc00: Binding('reiko_script_lp_select', 'game/c_reiko.c', ('tail',),
                      evidence='42cc00 compares saved pair ordering/range then enters 4de640 or 4de6b8'),
    0x42cc40: Binding('reiko_script_lp_end', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='42cc40 enters generated Reiko throw continuations 4de664 and 4de668 on the pair actors'),
    0x42ce80: Binding('reiko_script_lp_return', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='42ce80 stops motion and closes Reiko LP-throw pair state before controller return'),
    0x45b1f0: Binding('reiko_script_opponent_reaction', 'game/c_reiko.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Reiko LP-throw current-context reactions 102/107 through generated victim fibers'),
    0x485ad0: Binding('mk4_script_thrown_finish', 'game/script.c', ('tail',), flow='terminal',
                      evidence='485ad0 stops the released victim and continues the shared KO-aware thrown recovery'),
    0x43d460: Binding('reiko_script_fan_tail', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='43d460 fan owner scene coordinator; shared mk4_script_fan_tail helper'),
    0x43d530: Binding('reiko_script_fan_cycle', 'game/c_reiko.c', ('tail',),
                      evidence='43d530 native cycle returns to scene coordinator; shared scene helper'),
    0x43d6f0: Binding('reiko_script_fan_victory', 'game/c_reiko.c', ('tail',),
                      evidence='43d6f0 clears scene motion and dispatches generated 4e0158'),
    0x43db50: Binding('reiko_script_fan_victim_end', 'game/c_reiko.c', ('tail',),
                      evidence='43db50 hides victim and retires its scene fiber; shared scene helper'),
    0x45b230: Binding('reiko_script_opponent_reaction', 'game/c_reiko.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches opponent reaction selector from Reiko throw continuation'),
    0x46a760: Binding('reiko_script_kick_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46a760 kick contact-parameter runner after Reiko 4e5ca0 startup'),
    0x46aae0: Binding('reiko_script_air_release', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46aae0 applies reaction 89 and damage 0x2147; shared air-throw release helper'),
    0x46c970: Binding('reiko_script_air_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46c970 completes Reiko air-kick continuation through shared landing'),
    0x46cde0: Binding('reiko_script_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46cde0 uppercut continuation after selected 4e62a0 stream'),
    0x46e070: Binding('reiko_script_air_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46e070 terminates Reiko airborne low-kick continuation'),
    0x46e4d0: Binding('reiko_script_air_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46e4d0 terminates Reiko diagonal air-kick continuation'),
    0x46e730: Binding('reiko_script_air_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46e730 terminates Reiko air high-punch continuation'),
    0x46eea0: Binding('reiko_script_air_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46eea0 terminates Reiko air low-punch continuation'),
    0x46efd0: Binding('reiko_script_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46efd0 completes Reiko sweep contact/recovery continuation'),
    0x46f270: Binding('reiko_script_finish', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Reiko roundhouse contact/recovery continuation'),
    0x46f390: Binding('reiko_script_roundhouse_start', 'game/c_reiko.c', ('tail',),
                      evidence='46f390 selects fast or ordinary roundhouse startup scripts'),
    0x467fc0: Binding('reiko_script_spike_tail', 'game/c_reiko.c', ('tail',), flow='terminal',
                      evidence='467fc0 stage-fatality coordinator; shared spike helper'),
}

# Tanya follows the same common decoded roots, with her own native character
# continuations. The VM tails enter her generated CFG directly where the EXE
# only dispatches to a selected script; portable scene and contact behavior
# stays in c_tanya.c and shared engine helpers.
CHARACTER_BINDINGS[9] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/tanya.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 only enters Tanya paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/tanya.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 only enters Tanya release stream 4de328'),
    0x42be10: Binding('tanya_script_lk_return', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='42be10 clears owner/victim pair state and returns through the Tanya controller boundary'),
    0x43d460: Binding('tanya_script_fan_tail', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='43d460 fan owner scene coordinator; shared mk4_script_fan_tail helper'),
    0x43d530: Binding('tanya_script_fan_cycle', 'game/c_tanya.c', ('tail',),
                      evidence='43d530 native cycle returns to scene coordinator; shared scene helper'),
    0x43d6f0: Binding('tanya_script_fan_victory', 'game/c_tanya.c', ('tail',),
                      evidence='43d6f0 clears scene motion and dispatches generated Tanya 4e0158'),
    0x43db50: Binding('tanya_script_fan_victim_end', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the victim and retires its independent scene fiber'),
    0x45b1f0: Binding('tanya_script_current_reaction', 'game/c_tanya.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches the current Tanya-context reaction through battle_react'),
    0x45b230: Binding('tanya_script_opponent_reaction', 'game/c_tanya.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Tanya throw opponent reaction 77 and releases the pair'),
    0x46a760: Binding('tanya_script_kick_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46a760 enters Tanya high-kick recovery stream 4e5c70'),
    0x46aae0: Binding('tanya_script_air_release', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46aae0 starts the independent Tanya victim release and advances owner release'),
    0x46c970: Binding('tanya_script_air_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46c970 completes Tanya straight air-kick continuation through shared landing'),
    0x46cde0: Binding('tanya_script_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Tanya uppercut continuation'),
    0x46df90: Binding('tanya_script_air_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46df90 completes Tanya diagonal low air-kick continuation through shared landing'),
    0x46e3d0: Binding('tanya_script_air_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46e3d0 completes Tanya diagonal high air-kick continuation through shared landing'),
    0x46e730: Binding('tanya_script_air_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Tanya air high-punch continuation through shared landing'),
    0x46eea0: Binding('tanya_script_air_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Tanya air low-punch continuation through shared landing'),
    0x46efd0: Binding('tanya_script_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46efd0 completes Tanya sweep contact and recovery'),
    0x46f270: Binding('tanya_script_finish', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Tanya roundhouse contact and recovery'),
    0x46f390: Binding('tanya_script_roundhouse_start', 'game/c_tanya.c', ('tail',),
                      evidence='46f390 selects Tanya fast or ordinary roundhouse startup streams'),
    0x467fc0: Binding('tanya_script_spike_tail', 'game/c_tanya.c', ('tail',), flow='terminal',
                      evidence='467fc0 stage-fatality coordinator; shared spike helper'),
}

# Shinnok's throw and fan continuations enter selected generated streams
# directly; retain those edges in the generated CFG instead of adding C
# translations of the corresponding VM bodies.
CHARACTER_BINDINGS[6] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/shinnok.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 only enters Shinnok paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/shinnok.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 only enters Shinnok release stream 4de328'),
    0x43d6f0: Binding('retail_script_004e0158', 'game/generated/shinnok.inc', ('tail',),
                      script_target=0x4e0158, evidence='43d6f0 enters Shinnok fan victory stream 4e0158'),
    0x42be10: Binding('mk4_script_ready', 'game/script.c', ('tail',),
                      evidence='42be10 returns the held Shinnok throw actor through the common controller-ready boundary'),
    0x42ce60: Binding('mk4_script_ready', 'game/script.c', ('tail',),
                      evidence='42ce60 performs its final phase reset then returns through the shared ready boundary'),
    0x43d530: Binding('mk4_script_fan_cycle', 'game/script.c', ('tail',),
                      evidence='43d530 returns to the Shinnok fan scene coordinator without actor-local work'),
    0x43db50: Binding('mk4_script_fan_victim_end', 'game/script.c', ('tail',),
                      evidence='43db50 hides the fan victim and retires its scene fiber'),
    0x467fc0: Binding('mk4_script_spike_tail', 'game/script.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes Shinnok stage-fatality victim rise and owner recovery'),
    0x46aae0: Binding('shinnok_script_air_release', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46aae0 performs shared victim release then advances the Shinnok owner release stream 4e5e50'),
    0x46ab20: Binding('mk4_script_land', 'game/script.c', ('tail',),
                      evidence='46ab20 hands the air-throw continuation to the common landing controller'),
    0x46a760: Binding('shinnok_script_kick_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46a760 high-kick contact continuation enters generated recovery stream 4e5c70'),
    0x46c7f0: Binding('shinnok_script_air_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes straight air-kick continuation through shared landing'),
    0x46cde0: Binding('shinnok_script_uppercut_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46cde0 applies Shinnok uppercut contact samples then returns to ready'),
    0x46df90: Binding('shinnok_script_air_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46df90 completes diagonal low air-kick continuation through shared landing'),
    0x46e3d0: Binding('shinnok_script_air_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46e3d0 completes diagonal high air-kick continuation through shared landing'),
    0x46e730: Binding('shinnok_script_air_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Shinnok air high-punch continuation through shared landing'),
    0x46eea0: Binding('shinnok_script_air_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Shinnok air low-punch continuation through shared landing'),
    0x46f080: Binding('shinnok_script_sweep_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46f080 applies Shinnok sweep contact record then returns to ready'),
    0x46f220: Binding('shinnok_script_roundhouse_start', 'game/c_shinnok.c', ('tail',),
                      evidence='46f220 selects generated fast or ordinary Shinnok roundhouse startup stream'),
    0x46f270: Binding('shinnok_script_roundhouse_finish', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='46f270 applies Shinnok roundhouse contact record then returns to ready'),
    0x43d460: Binding('shinnok_fan_tail', 'game/c_shinnok.c', ('tail',), flow='terminal',
                      evidence='43d460 starts the Shinnok fan cycle/release coordinator through shared scene helper'),
    0x45b1f0: Binding('shinnok_script_current_reaction', 'game/c_shinnok.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches the low-byte current-context Shinnok reaction selector'),
    0x45b230: Binding('shinnok_script_opponent_reaction', 'game/c_shinnok.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches paired-throw victim reaction 77 and releases the pair'),
}

# Quan Chi shares these VM roots but enters roster-specific native tails.
CHARACTER_BINDINGS[8] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/quan_chi.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Quan Chi paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/quan_chi.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Quan Chi release stream 4de328'),
    0x42be10: Binding('quan_chi_script_lk_return', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='42be10 clears paired state and returns through Quan Chi controller continuation'),
    0x43d460: Binding('quan_chi_script_fan_tail', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='43d460 coordinates fan-fatality owner and victim continuations'),
    0x43d530: Binding('quan_chi_script_fan_cycle', 'game/c_quan_chi.c', ('tail',),
                      evidence='43d530 advances the fan scene cycle through shared scene helper'),
    0x43d6f0: Binding('quan_chi_script_fan_victory', 'game/c_quan_chi.c', ('tail',),
                      evidence='43d6f0 clears scene motion and enters generated Quan Chi victory stream'),
    0x43db50: Binding('quan_chi_script_fan_victim_end', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the released fatality victim and retires its scene fiber'),
    0x45b1f0: Binding('quan_chi_script_current_reaction', 'game/c_quan_chi.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Quan Chi LP-throw current-context reaction 103'),
    0x45b230: Binding('quan_chi_script_opponent_reaction', 'game/c_quan_chi.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches paired-throw opponent reaction 77 and releases the pair'),
    0x46a790: Binding('quan_chi_script_kick_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46a790 completes Quan Chi high-kick continuation after generated startup'),
    0x46aae0: Binding('quan_chi_script_air_release', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases the victim and starts generated Quan Chi air-throw continuation'),
    0x46c7f0: Binding('quan_chi_script_air_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes Quan Chi straight air-kick continuation through shared landing'),
    0x46cde0: Binding('quan_chi_script_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Quan Chi uppercut continuation'),
    0x46e150: Binding('quan_chi_script_air_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46e150 completes Quan Chi diagonal low air-kick continuation through shared landing'),
    0x46e5c0: Binding('quan_chi_script_air_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46e5c0 completes Quan Chi diagonal high air-kick continuation through shared landing'),
    0x46e730: Binding('quan_chi_script_air_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Quan Chi air high-punch continuation through shared landing'),
    0x46eea0: Binding('quan_chi_script_air_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Quan Chi air low-punch continuation through shared landing'),
    0x46f080: Binding('quan_chi_script_sweep_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46f080 applies Quan Chi sweep contact record then returns to ready'),
    0x46f220: Binding('quan_chi_script_roundhouse_start', 'game/c_quan_chi.c', ('tail',),
                      evidence='46f220 selects generated fast or ordinary Quan Chi roundhouse startup stream'),
    0x46f270: Binding('quan_chi_script_finish', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Quan Chi roundhouse contact/recovery continuation'),
    0x467fc0: Binding('quan_chi_script_spike_tail', 'game/c_quan_chi.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes stage-fatality continuation through shared scene helper'),
}

# Fujin reuses the shared decoded moves, with character-local throw, fan and
# recovery adapters so its generated roots preserve the selected roster path.
CHARACTER_BINDINGS[5] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/fujin.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Fujin paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/fujin.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Fujin throw damage/release stream 4de328'),
    0x42be10: Binding('fujin_script_lk_return', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='42be10 clears paired state and returns through Fujin controller continuation'),
    0x42cb80: Binding('fujin_script_lp_select', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='42cb80 compares saved pair direction/distance and selects Fujin LP throw roots 4de610/4de6b8'),
    0x42cbc0: Binding('fujin_script_lp_extended_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='42cbc0 releases the extended Fujin throw pair through controller recovery'),
    0x42ce80: Binding('fujin_script_lp_short_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='42ce80 releases the short Fujin LP throw and applies reaction 102'),
    0x43d460: Binding('fujin_script_fan_tail', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='43d460 coordinates Fujin fan owner/victim continuations through shared scene helper'),
    0x43d530: Binding('mk4_script_fan_cycle', 'game/script.c', ('tail',),
                      evidence='43d530 returns to the Fujin fan scene coordinator'),
    0x43d6f0: Binding('retail_script_004e0158', 'game/generated/fujin.inc', ('tail',),
                      script_target=0x4e0158, evidence='43d6f0 enters Fujin fan victory stream 4e0158'),
    0x43db50: Binding('mk4_script_fan_victim_end', 'game/script.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the fan victim and retires its scene fiber'),
    0x45b1f0: Binding('fujin_script_current_reaction', 'game/c_fujin.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Fujin LP-throw current-context reaction 109'),
    0x45b230: Binding('fujin_script_opponent_reaction', 'game/c_fujin.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches paired Fujin throw victim reaction 77 and releases the pair'),
    0x46a6b0: Binding('retail_script_004e5c70', 'game/generated/fujin.inc', ('tail',),
                      script_target=0x4e5c70, evidence='46a6b0 enters Fujin high-kick recovery stream 4e5c70'),
    0x46aae0: Binding('fujin_script_air_release', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases the Fujin air-throw victim then advances the owner release stream'),
    0x46c970: Binding('fujin_script_air_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46c970 completes Fujin straight air-kick landing through shared clip/landing helpers'),
    0x46cde0: Binding('fujin_script_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Fujin uppercut recovery and returns to ready'),
    0x46e150: Binding('fujin_script_air_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46e150 completes Fujin diagonal low air-kick landing'),
    0x46e5c0: Binding('fujin_script_air_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46e5c0 completes Fujin diagonal high air-kick landing'),
    0x46e730: Binding('fujin_script_air_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Fujin air high-punch landing'),
    0x46eea0: Binding('fujin_script_air_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Fujin air low-punch landing'),
    0x46efd0: Binding('fujin_script_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46efd0 completes Fujin sweep recovery and returns to ready'),
    0x46f270: Binding('fujin_script_finish', 'game/c_fujin.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Fujin roundhouse recovery and returns to ready'),
    0x46f390: Binding('fujin_script_roundhouse_start', 'game/c_fujin.c', ('tail',),
                      evidence='46f390 selects Fujin fast or ordinary roundhouse startup root'),
    0x467fc0: Binding('mk4_script_spike_tail', 'game/script.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes the stage-fatality continuation through the shared scene helper'),
}

CHARACTER_BINDINGS[10] = {
    0x40eb80: Binding('reptile_script_visibility_end', 'game/c_reptile.c', ('call',),
                      evidence='40eb80 closes the Reptile invisibility stream; adapter restores the saved appearance and clears teleport presentation state'),
    0x40ec40: Binding('reptile_script_visibility_start', 'game/c_reptile.c', ('call',),
                      evidence='40ec40 begins the clip-driven Reptile visibility event after the opening clip samples'),
    0x40ed20: Binding('reptile_script_visibility_phase', 'game/c_reptile.c', ('call',),
                      evidence='40ed20 marks the next observed Reptile visibility phase'),
    0x40ee00: Binding('reptile_script_visibility_extend', 'game/c_reptile.c', ('call',),
                      evidence='40ee00 extends the Reptile visibility event for its sample-30 loop and task76 continuation'),
    0x498920: Binding('reptile_task_76_spawn', 'game/c_reptile.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(76,),
                      evidence='498920 copies the actor transform and launches the clip26 contact waiter; the adapter uses a timed clone event and geometry-only contact probe'),
    0x47ab50: Binding('retail_script_004e8174', 'game/generated/reptile.inc', ('tail',),
                      script_target=0x4e8174,
                      evidence='47ab50 callback state enters the selected clip 106/22 continuation 4e8174'),
    0x47b010: Binding('reptile_script_47b010', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='47b010 owns the stateful D[21] contact window and six-tick retry callback'),
    0x488b40: Binding('reptile_script_reaction_side_getup', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='488b40 selects Reptile reaction 40 side-getup continuation after generated 4e9620'),
}

# Reptile's common combat roots use the same VM streams as the earlier ports,
# but its throw, contact, and paired reaction boundaries remain local to the
# slot-10 controller.
CHARACTER_BINDINGS[10].update({
    0x485ad0: Binding('mk4_script_thrown_finish', 'game/script.c', ('tail',), flow='terminal',
                      evidence='485ad0 stops the released Reptile LP-throw victim and runs shared KO-aware thrown recovery'),
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/reptile.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Reptile paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/reptile.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Reptile paired release stream 4de328'),
    0x42be10: Binding('reptile_script_lk_return', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='42be10 clears pair state and returns Reptile through its controller-ready boundary'),
    0x42cc00: Binding('reptile_script_lp_select', 'game/c_reptile.c', ('tail',),
                      evidence='42cc00 selects Reptile extended or short LP throw from pair ordering and range'),
    0x42cc40: Binding('reptile_script_lp_end', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='42cc40 releases the Reptile extended LP throw pair through owner recovery'),
    0x42ce80: Binding('reptile_script_lp_return', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='42ce80 closes Reptile short LP throw pair state before controller return'),
    0x45b1f0: Binding('reptile_script_current_reaction', 'game/c_reptile.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Reptile current-context throw reaction selectors'),
    0x45b230: Binding('reptile_script_opponent_reaction', 'game/c_reptile.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Reptile paired victim and fatality reaction selectors'),
    0x43d460: Binding('reptile_script_fan_tail', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='43d460 starts Reptile fan owner/victim scene coordination'),
    0x43d530: Binding('reptile_script_fan_cycle', 'game/c_reptile.c', ('tail',),
                      evidence='43d530 advances Reptile fan scene through the shared scene helper'),
    0x43d6f0: Binding('retail_script_004e0158', 'game/generated/reptile.inc', ('tail',),
                      script_target=0x4e0158, evidence='43d6f0 enters Reptile fan victory stream 4e0158'),
    0x43db50: Binding('reptile_script_fan_victim_end', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the fan victim and retires its scene fiber'),
    0x46a760: Binding('retail_script_004e5c70', 'game/generated/reptile.inc', ('tail',),
                      script_target=0x4e5c70, evidence='46a760 enters Reptile high-kick recovery stream 4e5c70'),
    0x46aae0: Binding('reptile_script_air_release', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases Reptile air-throw victim and advances owner release'),
    0x46c7f0: Binding('reptile_script_air_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes Reptile straight air-kick landing'),
    0x46cde0: Binding('reptile_script_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Reptile uppercut contact and recovery'),
    0x46e070: Binding('reptile_script_air_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46e070 completes Reptile diagonal low air-kick landing'),
    0x46e4d0: Binding('reptile_script_air_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46e4d0 completes Reptile diagonal high air-kick landing'),
    0x46e730: Binding('reptile_script_air_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Reptile air high-punch landing'),
    0x46eea0: Binding('reptile_script_air_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Reptile air low-punch landing'),
    0x46f080: Binding('reptile_script_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46f080 completes Reptile sweep contact and recovery'),
    0x46f220: Binding('reptile_script_roundhouse_start', 'game/c_reptile.c', ('tail',),
                      evidence='46f220 selects Reptile fast or normal roundhouse stream'),
    0x46f270: Binding('reptile_script_finish', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Reptile roundhouse contact and recovery'),
    0x467fc0: Binding('reptile_script_spike_tail', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes Reptile stage-fatality continuation'),
    0x47a6d0: Binding('reptile_script_47a6d0', 'game/c_reptile.c', ('tail',), flow='terminal',
                      evidence='47a6d0 probes Reptile D[23] during the 47a690 contact loop'),
})

CHARACTER_BINDINGS[11] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/kai.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Kai paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/kai.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Kai paired release stream 4de328'),
    0x42be10: Binding('kai_script_lk_return', 'game/c_kai.c', ('tail',),
                      evidence='42be10 enters Kai follow-up root 4de358 after the paired continuation'),
    0x42bf00: Binding('kai_script_lk_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='42bf00 closes Kai paired fibers and returns through its controller boundary'),
    0x42cb80: Binding('kai_script_lp_select', 'game/c_kai.c', ('tail',),
                      evidence='42cb80 selects Kai 4de610 or 4de6b8 from the pair-distance bit'),
    0x42cbc0: Binding('kai_script_lp_extended_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='42cbc0 releases Kai extended LP throw and applies reaction 109'),
    0x42ce80: Binding('kai_script_lp_short_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='42ce80 releases Kai short LP throw and applies reaction 102'),
    0x45b1f0: Binding('kai_script_current_reaction', 'game/c_kai.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Kai current-context reactions from generated throw roots'),
    0x43d460: Binding('kai_script_fan_tail', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='43d460 coordinates Kai fan owner and victim scene fibers'),
    0x43d530: Binding('kai_script_fan_cycle', 'game/c_kai.c', ('tail',),
                      evidence='43d530 advances Kai fan scene through the shared scene helper'),
    0x43d6f0: Binding('kai_script_fan_victory', 'game/c_kai.c', ('tail',),
                      evidence='43d6f0 completes Kai fan owner release sequence'),
    0x43db50: Binding('kai_script_fan_victim_end', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the Kai fan victim and retires its scene fiber'),
    0x45b230: Binding('kai_script_opponent_reaction', 'game/c_kai.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Kai paired victim reactions 77 and 175'),
    0x46a790: Binding('kai_script_kick_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46a790 completes Kai high-kick contact and recovery'),
    0x46aae0: Binding('kai_script_air_release', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases Kai air-throw victim and advances owner recovery'),
    0x46c970: Binding('kai_script_air_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46c970 completes Kai straight air-kick landing'),
    0x46cde0: Binding('kai_script_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Kai uppercut contact and recovery'),
    0x46d260: Binding('kai_script_draw_equip', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46d260 executes guarded Kai weapon follow-up then enters 4e62d8'),
    0x46e150: Binding('kai_script_air_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46e150 completes Kai diagonal low air-kick landing'),
    0x46e5c0: Binding('kai_script_air_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46e5c0 completes Kai diagonal high air-kick landing'),
    0x46e730: Binding('kai_script_air_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Kai air high-punch landing'),
    0x46eea0: Binding('kai_script_air_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Kai air low-punch landing'),
    0x46efd0: Binding('kai_script_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46efd0 completes Kai sweep recovery'),
    0x46f270: Binding('kai_script_finish', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Kai roundhouse contact and recovery'),
    0x46f390: Binding('kai_script_roundhouse_start', 'game/c_kai.c', ('tail',),
                      evidence='46f390 selects Kai fast or normal roundhouse startup'),
    0x467fc0: Binding('kai_script_spike_tail', 'game/c_kai.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes Kai stage-fatality continuation'),
    0x47c580: Binding('kai_script_stage_event', 'game/c_kai.c', ('tail',), flow='independent',
                      evidence='47c580 enters StageEventComplexFsm after the Kai clip-1 or clip-9 phase; the portable BattleEvent resumes that clip from its observed sample boundary'),
    0x47cc10: Binding('retail_script_004e8358', 'game/generated/kai.inc', ('tail',),
                      script_target=0x4e8358, evidence='47cc10 enters the selected Kai continuation 4e8358'),
}
CHARACTER_BINDINGS[11][0x47bd40] = Binding('kai_script_aerial_transition', 'game/c_kai.c', ('tail',), flow='terminal',
                                           evidence='47bd40 explicitly resumes Kai aerial continuation at generated root 4e8238')
CHARACTER_BINDINGS[11][0x47bf90] = Binding('kai_script_aerial_recovery', 'game/c_kai.c', ('tail',), flow='terminal',
                                           evidence='47bf90 clears aerial motion, waits eight ticks, restores gravity and lands through the portable recovery helper')
CHARACTER_BINDINGS[11][0x47cc50] = Binding('kai_script_contact_b10_continuation', 'game/c_kai.c', ('tail',), flow='terminal',
                                           evidence='47cc50 dispatches Kai table-B selector 10 at samples 10, 11 and 11, then enters the generated wait-and-rewind continuation 4e8390')
CHARACTER_BINDINGS[11][0x478e20] = Binding('kai_script_contact_b9_continuation', 'game/c_kai.c', ('tail',), flow='terminal',
                                           evidence='478e20 dispatches Kai table-B selector 9 at samples 30 through 34, then enters generated continuation 4e88c8')

CHARACTER_BINDINGS[12] = {
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/jarek.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Jarek paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/jarek.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Jarek paired release stream 4de328'),
    0x42be10: Binding('jarek_script_lk_return', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='42be10 clears Jarek paired state and returns the owner'),
    0x42cc00: Binding('jarek_script_lp_select', 'game/c_jarek.c', ('tail',),
                      evidence='42cc00 selects Jarek LP throw variant from pair distance and orientation'),
    0x42cc40: Binding('jarek_script_lp_end', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='42cc40 starts the victim continuation and advances owner recovery'),
    0x42ce80: Binding('jarek_script_lp_return', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='42ce80 clears Jarek paired state and returns through ready'),
    0x45b1f0: Binding('jarek_script_current_reaction', 'game/c_jarek.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Jarek current-context reactions'),
    0x45b230: Binding('jarek_script_opponent_reaction', 'game/c_jarek.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Jarek paired victim reactions'),
    0x43d460: Binding('retail_script_004e0128', 'game/generated/jarek.inc', ('tail',),
                      script_target=0x4e0128, evidence='43d460 enters Jarek fan owner continuation 4e0128'),
    0x43d530: Binding('mk4_script_fan_cycle', 'game/script.c', ('tail',),
                      evidence='43d530 advances the fan scene through the shared scene helper'),
    0x43d6f0: Binding('jarek_fan_victory', 'game/c_jarek.c', ('tail',),
                      evidence='43d6f0 enters Jarek fan victory root 4e0158'),
    0x43d510: Binding('jarek_script_fan_scene', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='43d510 begins the observed Jarek fan scene continuation through a BattleEvent phase sequence'),
    0x43db50: Binding('jarek_script_fan_victim_scene', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='43db50 continues from victim clip 4e01a0 into owner release clip 4e01b0'),
    0x43dc90: Binding('jarek_script_fan_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='43dc90 enters a clip-driven BattleEvent for Jarek finisher camera and scene cleanup'),
    0x479b70: Binding('jarek_script_contact_25', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='479b70 dispatches Jarek contact selector 25 and its powered-punch continuation as BattleEvent phases'),
    0x479f00: Binding('jarek_script_contact_26', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='479f00 dispatches Jarek contact selector 26 and powered-punch finish continuation as BattleEvent phases'),
    0x47a3b0: Binding('jarek_script_contact_24', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='47a3b0 dispatches Jarek contact selector 24 and final recovery as a BattleEvent phase'),
    0x497580: Binding('jarek_ai_projectile_spawn', 'game/c_jarek.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(78,),
                      evidence='497580 creates heading object 4ed170 and starts the observed 48-tick projectile contact waiter; descriptor payload is unavailable'),
    0x46a6b0: Binding('jarek_script_kick_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46a6b0 resolves Jarek high-kick contact and recovery'),
    0x46a950: Binding('jarek_script_low_kick_avoid', 'game/c_jarek.c', ('call',),
                      evidence='46a950 enables Jarek low-kick high-attack avoidance'),
    0x46a970: Binding('jarek_script_low_kick_clear', 'game/c_jarek.c', ('call',),
                      evidence='46a970 clears Jarek low-kick avoidance state'),
    0x46aae0: Binding('jarek_script_air_release', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases Jarek air-throw victim and advances owner recovery'),
    0x46c7f0: Binding('jarek_script_air_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes Jarek straight air-kick landing'),
    0x46cde0: Binding('jarek_script_uppercut_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46cde0 resolves Jarek uppercut contact and recovery'),
    0x46d260: Binding('jarek_script_weapon_draw_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46d260 advances Jarek weapon draw continuation'),
    0x46df90: Binding('jarek_script_air_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46df90 completes Jarek diagonal low air-kick landing'),
    0x46e3d0: Binding('jarek_script_air_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46e3d0 completes Jarek diagonal high air-kick landing'),
    0x46e730: Binding('jarek_script_air_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Jarek air high-punch landing'),
    0x46eea0: Binding('jarek_script_air_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Jarek air low-punch landing'),
    0x46f080: Binding('jarek_script_sweep_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46f080 resolves Jarek sweep contact and recovery'),
    0x46f220: Binding('jarek_script_roundhouse_start', 'game/c_jarek.c', ('tail',),
                      evidence='46f220 selects Jarek fast or normal roundhouse startup'),
    0x46f270: Binding('jarek_script_roundhouse_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='46f270 resolves Jarek roundhouse contact and recovery'),
    0x467fc0: Binding('mk4_script_spike_tail', 'game/script.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes the stage-fatality continuation'),
    0x47b600: Binding('jarek_script_47b600', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='47b600 applies Jarek follow-up contact damage and reaction'),
    0x497b60: Binding('jarek_script_sweep_finish', 'game/c_jarek.c', ('tail',), flow='terminal',
                      evidence='497b60 enters Jarek sweep contact and recovery through the existing portable contact helper'),
}

CHARACTER_BINDINGS[15] = {
    0x43afc0: Binding('goro_script_event_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='43afc0 is the generated progress clip tail; its actor-local adapter finishes the pose event without controller scheduler effects'),
    0x43b1a0: Binding('goro_script_event_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='43b1a0 is the generated counter-kick clip tail; its actor-local adapter finishes the pose event without controller scheduler effects'),
    0x43b370: Binding('goro_script_event_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='43b370 follows the generated counter-sweep clip; the actor-local adapter finishes the pose event and omits versus/controller state'),
    0x42b680: Binding('goro_script_lp_pair_start', 'game/c_goro.c', ('call',), ('fighter',),
                      evidence='42b680 starts Goro paired LP-throw victim context before owner/victim clips are bound'),
    0x42c770: Binding('retail_script_004de7e8', 'game/generated/goro.inc', ('tail',),
                      script_target=0x4de7e8, evidence='42c770 schedules the first Goro repeated damage phase at 4de7e8'),
    0x42c7b0: Binding('goro_script_lp_phase_repeat', 'game/c_goro.c', ('tail',), ('fighter',), flow='terminal',
                      evidence="42c7b0 decrements Goro's five-phase LP-throw sequence and selects repeat or final damage animation"),
    0x42ce60: Binding('goro_script_lk_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='42ce60 closes Goro paired actor state after the generated LP-throw finish phase'),
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/goro.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Goro paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/goro.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Goro paired release stream 4de328'),
    0x42be10: Binding('goro_script_lk_return', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='42be10 branches to Goro follow-up stream 4de358 after the paired throw state check'),
    0x42bf00: Binding('goro_script_lk_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='42bf00 completes the Goro paired throw follow-up and returns the owner'),
    0x45abf0: Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                      mmove=True, evidence='45abf0 enqueues a Goro paired camera context; selector 81 is passed as a low byte',
                      adaptation='Portable camera already tracks both live actors',
                      inline_values=(1, 2, 3, 4, 6, 11, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62, 75, 76, 77, 79, 81)),
    0x45b1f0: Binding('goro_script_current_reaction', 'game/c_goro.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches the selected current-context reaction through Goro roster reaction handling'),
    0x45b230: Binding('goro_script_opponent_reaction', 'game/c_goro.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Goro paired victim reactions from the opponent context'),
    0x43d460: Binding('goro_script_fan_tail', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='43d460 starts Goro fan owner/victim scene coordination'),
    0x43d530: Binding('goro_script_fan_cycle', 'game/c_goro.c', ('tail',),
                      evidence='43d530 advances the Goro fan scene through the shared scene helper'),
    0x43d6f0: Binding('goro_script_fan_victory', 'game/c_goro.c', ('tail',),
                      evidence='43d6f0 enters Goro fan victory sequence'),
    0x43db50: Binding('goro_script_fan_victim_end', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the Goro fan victim and retires its scene fiber'),
    0x46a760: Binding('goro_script_kick_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46a760 enters Goro generated high-kick recovery stream 4e5c70'),
    0x46aae0: Binding('goro_script_air_release', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases Goro air-throw victim and advances owner recovery'),
    0x46c7f0: Binding('goro_script_air_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes Goro straight air-kick landing'),
    0x46cde0: Binding('goro_script_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Goro uppercut contact and recovery'),
    0x46e150: Binding('goro_script_air_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46e150 completes Goro diagonal low air-kick landing'),
    0x46e4d0: Binding('goro_script_air_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46e4d0 completes Goro diagonal high air-kick landing'),
    0x46e730: Binding('goro_script_air_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Goro air high-punch landing'),
    0x46eea0: Binding('goro_script_air_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Goro air low-punch landing'),
    0x46f080: Binding('goro_script_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46f080 completes Goro sweep contact and recovery'),
    0x46f220: Binding('goro_script_roundhouse_start', 'game/c_goro.c', ('tail',),
                      evidence='46f220 selects Goro fast or normal roundhouse stream'),
    0x46f270: Binding('goro_script_finish', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Goro roundhouse contact and recovery'),
    0x467fc0: Binding('goro_script_spike_tail', 'game/c_goro.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes Goro stage-fatality continuation'),
    0x49ca90: Binding('goro_cpu_projectile_spawn', 'game/c_goro.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(83,),
                      evidence='49ca90 launches task83 from Goro script 4ed790; portable adapter captures heading, advances clip592, and resolves contact through the shared projectile helper'),
}

from dataclasses import replace as _clone_binding

CHARACTER_BINDINGS[17] = {
    _address: _clone_binding(
        _binding,
        symbol=_binding.symbol.replace('reiko_', 'noob_saibot_'),
        source=_binding.source.replace('reiko', 'noob_saibot'),
        evidence=_binding.evidence.replace('Reiko', 'Noob Saibot').replace('reiko', 'Noob Saibot'),
    )
    for _address, _binding in CHARACTER_BINDINGS[7].items()
}
CHARACTER_BINDINGS[17][0x45abf0] = Binding(
    'mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
    evidence='45abf0 Noob paired throw context 21 uses the shared live-actor camera context',
    adaptation='Portable camera tracks live paired actors; the retail camera task has no local lifetime',
    inline_values=(1, 2, 3, 4, 6, 11, 12, 14, 21, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62, 68, 75, 76, 77, 79),
)

CHARACTER_BINDINGS[16] = {
    _address: _clone_binding(
        _binding,
        source=_binding.source.replace('game/c_sonya.c', 'game/c_kitana.c')
                                .replace('game/generated/sonya.inc', 'game/generated/kitana.inc'),
        evidence=_binding.evidence.replace('Sonya', 'Kitana'),
    )
    for _address, _binding in CHARACTER_BINDINGS[2].items()
}
CHARACTER_BINDINGS[16].update({
    0x498bb0: Binding('kitana_shadow_task_low', 'game/c_kitana.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(75,), evidence='498bb0 starts the clip-26 paired task and contact-wait phase for Kitana'),
    0x498ed0: Binding('kitana_shadow_task_up', 'game/c_kitana.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(75,), evidence='498ed0 starts Kitana child clip 22, action 0x23d7 and the paired contact phase'),
    0x498eb0: Binding('kitana_shadow_install_physics', 'game/c_kitana.c', ('tail',), flow='terminal',
                      evidence='498eb0 installs gravity 0x20c and clears vertical position for the Kitana paired actor'),
})
CHARACTER_BINDINGS[17].update({
    0x498bb0: Binding('noob_saibot_shadow_task_low', 'game/c_noob_saibot.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(75,), evidence='498bb0 starts the clip-26 paired task and contact-wait phase for Noob Saibot'),
    0x498ed0: Binding('noob_saibot_shadow_task_up', 'game/c_noob_saibot.c', ('spawn',), ('fighter', 'task_type'),
                      flow='independent', task_types=(75,), evidence='498ed0 starts Noob Saibot child clip 22, action 0x23d7 and the paired contact phase'),
    0x498eb0: Binding('noob_saibot_shadow_install_physics', 'game/c_noob_saibot.c', ('tail',), flow='terminal',
                      evidence='498eb0 installs gravity 0x20c and clears vertical position for the Noob Saibot paired actor'),
})

CHARACTER_BINDINGS[13] = {
    0x46a9f0: Binding('retail_script_004e5df8', 'game/generated/jax.inc', ('tail',),
                      script_target=0x4e5df8,
                      evidence='46a9f0 enters continuation 4e5df8 through script_enter'),
    0x42be10: Binding('jax_script_lk_return', 'game/c_jax.c', ('tail',),
                      evidence='42be10 first-entry branch resumes Jax paired-throw follow-up root 4de358'),
    0x42bf00: Binding('jax_script_lk_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='42bf00 closes the Jax paired throw after its follow-up VM stream'),
    0x46a760: Binding('retail_script_004e5c70', 'game/generated/jax.inc', ('tail',),
                      script_target=0x4e5c70, evidence='46a760 high-kick parameter runner selects Jax recovery entry 4e5c70'),
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/jax.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 enters Jax paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/jax.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 enters Jax paired release stream 4de328'),
    0x43d6f0: Binding('retail_script_004e0158', 'game/generated/jax.inc', ('tail',),
                      script_target=0x4e0158, evidence='43d6f0 clears scene motion and enters the Jax fan victory stream 4e0158'),
    0x43d460: Binding('jax_fan_tail', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='43d460 coordinates Jax fan performer, cycle, release and victim scene streams'),
    0x43d530: Binding('mk4_script_fan_cycle', 'game/script.c', ('tail',),
                      evidence='43d530 fan owner cycle returns to the Jax scene coordinator'),
    0x43db50: Binding('mk4_script_fan_victim_end', 'game/script.c', ('tail',), flow='terminal',
                      evidence='43db50 retires the Jax fan victim scene stream'),
    0x45b1f0: Binding('jax_script_current_reaction', 'game/c_jax.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches the low-byte reaction in the current fighter context through reaction table 4ea4e8',
                      inline_values=(23, 67, 80, 183)),
    0x45b230: Binding('jax_script_opponent_reaction', 'game/c_jax.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches the low-byte reaction in the opponent context through reaction table 4ea4e8'),
    0x46f390: Binding('jax_script_roundhouse_start', 'game/c_jax.c', ('tail',),
                      evidence='46f390 chooses Jax roundhouse root 4e6890 or 4e68a8 from actor flag 0x200'),
    0x46f3e0: Binding('jax_script_roundhouse_tail', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46f3e0 advances the Jax clip, queries B[34], and the portable roundhouse phase owns contact and recovery'),
    0x46c970: Binding('jax_script_air_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46c970 finishes the straight air-kick clip and returns through the landing controller'),
    0x46cde0: Binding('jax_script_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes the Jax uppercut continuation and returns the fighter to ready'),
    0x46e070: Binding('jax_script_air_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46e070 selects the generated diagonal-low air-kick clip and completes through landing'),
    0x46e4d0: Binding('jax_script_air_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46e4d0 selects the generated diagonal-high air-kick clip and completes through landing'),
    0x46e730: Binding('jax_script_air_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46e730 selects the generated air high-punch clip and completes through landing'),
    0x46eea0: Binding('jax_script_air_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46eea0 selects the generated air low-punch clip and completes through landing'),
    0x46efd0: Binding('jax_script_finish', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46efd0 completes Jax sweep clip and returns the fighter to ready'),
    0x46aae0: Binding('jax_script_air_throw_release', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46aae0 dispatches victim reaction 89 and damage 0x2147, then enters owner recovery 4e5e50'),
    0x467fc0: Binding('mk4_script_spike_tail', 'game/script.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes the stage-fatality continuation through the shared scene helper'),
}

# Jax's known task and contact boundaries use BattleEvent callbacks so task
# state remains explicit and portable while the VM bodies stay generated.
CHARACTER_BINDINGS[13].update({
    0x48a220: Binding('battle_event_pump', 'game/battle.c', ('call',), (), evidence='48a220 pumps the native task callback queue; the portable scene callback queue is pumped by battle_event_pump'),
    0x42c7f0: Binding('retail_script_004de4f0', 'game/generated/jax.inc', ('tail',), script_target=0x4de4f0,
                      evidence='42c7f0 enters Jax paired-throw contact root 4de4f0'),
    0x42c820: Binding('mk4_script_pair_cleanup', 'game/script.c', ('tail',), flow='terminal',
                      evidence='42c820 closes the Jax paired throw and returns to the actor controller'),
    0x449890: Binding('retail_script_004e13d0', 'game/generated/jax.inc', ('tail',), script_target=0x4e13d0,
                      evidence='449890 advances the selected Jax scene actor from sample 49 into its next generated phase'),
    0x4498d0: Binding('retail_script_004e13e0', 'game/generated/jax.inc', ('tail',), script_target=0x4e13e0,
                      evidence='4498d0 advances the selected Jax scene actor from sample 78 into its final generated phase'),
    0x449940: Binding('jax_script_fatality_1_finish', 'game/c_jax.c', ('tail',), flow='independent',
                      evidence='449940 starts the portable F1 scene completion event after clip 204/400 sample 128'),
    0x449620: Binding('retail_script_004e13a0', 'game/generated/jax.inc', ('tail',), script_target=0x4e13a0,
                      evidence='449620 starts the generated Jax clip-443 performer loop'),
    0x412d80: Binding('jax_script_fatality_2_task', 'game/c_jax.c', ('spawn',), ('fighter', 'task_type'), flow='independent',
                     task_types=(0x26b,), evidence='412d80 creates the Jax F2 scene task for the clip-driven BattleEvent adapter'),
    0x412ec0: Binding('jax_script_fatality_2_task', 'game/c_jax.c', ('spawn',), ('fighter', 'task_type'), flow='independent',
                     task_types=(0x26b,), evidence='412ec0 creates a second Jax F2 scene task at the generated clip boundary'),
    0x45abf0: Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                     mmove=True, evidence='45abf0 selects the shared paired-camera context for Jax throw and fatality scenes',
                     adaptation='Portable camera tracks live paired actors; selectors 67 and 80 share the same actor context',
                     inline_values=(3, 4, 23, 67, 80)),
    0x466b40: Binding('jax_script_fatality_2_task', 'game/c_jax.c', ('spawn',), ('fighter', 'task_type'), flow='independent',
                     task_types=(0x26b,), evidence='466b40 creates the Jax F2 scene reaction task and its clip-driven follow-up'),
    0x464bd0: Binding('jax_script_fatality_2_task', 'game/c_jax.c', ('spawn',), ('fighter', 'task_type'), flow='independent',
                     task_types=(0x26b,), evidence='464bd0 creates the Jax F2 victim scene keepalive task'),
    0x466a90: Binding('jax_script_fatality_2_tail', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='466a90 resumes the generated Jax F2 owner script at clip phase 555'),
    0x466bb0: Binding('jax_script_fatality_2_scene_finish', 'game/c_jax.c', ('tail',), flow='independent',
                      evidence='466bb0 completes the Jax F2 victim scene task through BattleEvent phase state'),
    0x466df0: Binding('jax_script_fatality_2_scene_finish', 'game/c_jax.c', ('tail',), flow='independent',
                      evidence='466df0 retires the Jax F2 victim scene after its clip 556 phase'),
    0x46aa40: Binding('jax_script_air_throw_ground_release', 'game/c_jax.c', ('tail',), flow='terminal',
                      evidence='46aa40 applies the Jax paired throw victim reaction and damage before closing the pair'),
    0x46d260: Binding('retail_script_004e62d8', 'game/generated/jax.inc', ('tail',), script_target=0x4e62d8,
                      evidence='46d260 returns Jax weapon draw to its generated clip-244 follow-up'),
    0x47ab50: Binding('jax_script_jab_contact', 'game/c_jax.c', ('tail',), flow='independent',
                      evidence='47ab50 queries Jax jab selector 21 and schedules contact/recovery phases'),
    0x47b010: Binding('jax_script_jab_followup', 'game/c_jax.c', ('tail',), flow='independent',
                      evidence='47b010 owns the Jax follow-up contact window and recovery phase'),
    0x496fc0: Binding('jax_script_cpu_projectile_spawn', 'game/c_jax.c', ('spawn',), ('fighter', 'task_type'), flow='independent',
                      task_types=(80,), evidence='496fc0 spawns the CPU projectile from its scene task callback'),
    0x00000000: Binding('jax_script_fatality_1_scene_signal', 'game/c_jax.c', ('handler',), ('fighter',),
                        evidence='Jax F1 scene task uses a portable BattleEvent at the unsupported task-signal opcode'),
})

# Cage's generated streams share the fan, paired-throw, and air-throw
# continuations with the established ports. Contact and stage callbacks stay
# in the Cage integration so their move-specific recovery remains explicit.
CHARACTER_BINDINGS[14] = {
    0x448480: Binding('cage_script_fatality_1_signal', 'game/c_cage.c', ('tail',), flow='independent',
                      evidence='448480 consumes the signal before entering 4e12b0; Cage models the scene signal with a clip-driven BattleEvent'),
    0x4658a0: Binding('cage_script_fatality_2_continue', 'game/c_cage.c', ('tail',), flow='independent',
                      evidence='4658a0 resumes the selected scene after the 30-tick context setup; Cage continues through its clip-driven BattleEvent'),
    0x479b70: Binding('cage_script_cpu_kick_tail', 'game/c_cage.c', ('tail',), flow='independent',
                      evidence='479b70 checks Cage contact selector 0x19 and enters its powered-punch branch on a miss'),
    0x479f00: Binding('cage_script_powered_punch_tail', 'game/c_cage.c', ('tail',), flow='independent',
                      evidence='479f00 checks Cage powered-punch selector 0x1a and starts the final follow-up on contact'),
    0x47a3b0: Binding('cage_script_powered_punch_finish_tail', 'game/c_cage.c', ('tail',), flow='independent',
                      evidence='47a3b0 resolves Cage final follow-up selector 0x18 and its contact/recovery phase'),
    0x42bdf0: Binding('retail_script_004de318', 'game/generated/cage.inc', ('tail',),
                      script_target=0x4de318, evidence='42bdf0 only enters Cage paired continuation 4de318'),
    0x42be00: Binding('retail_script_004de328', 'game/generated/cage.inc', ('tail',),
                      script_target=0x4de328, evidence='42be00 only enters Cage release stream 4de328'),
    0x42be10: Binding('cage_script_lk_throw_return', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='42be10 clears Cage owner/victim pair state and returns through controller-ready'),
    0x42ce60: Binding('cage_script_lp_return', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='42ce60 stops the Cage LP throw and closes paired actor state'),
    0x43d460: Binding('cage_script_fan_tail', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='43d460 Cage fan coordinator reuses the shared fan-cycle scene helper'),
    0x43d530: Binding('cage_script_fan_cycle', 'game/c_cage.c', ('tail',),
                      evidence='43d530 Cage fan cycle continuation returns to its coordinator'),
    0x43d6f0: Binding('cage_script_fan_victory', 'game/c_cage.c', ('tail',),
                      evidence='43d6f0 Cage fan victory enters generated continuation 4e0158'),
    0x43db50: Binding('cage_script_fan_victim_end', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='43db50 hides the Cage fan victim and retires its scene fiber'),
    0x45b1f0: Binding('cage_script_opponent_reaction', 'game/c_cage.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b1f0 dispatches Cage paired-throw current-context reactions', inline_values=(100,)),
    0x45b230: Binding('cage_script_opponent_reaction', 'game/c_cage.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='45b230 dispatches Cage paired-throw opponent-context reactions'),
    0x45abf0: Binding('mk4_script_context', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                      mmove=True, evidence='45abf0 Cage selector 68 enters the shared paired-camera context handler',
                      adaptation='Portable camera tracks live paired actors; selector 68 has no retail task lifetime',
                      inline_values=(1, 2, 3, 4, 6, 12, 14, 22, 23, 34, 36, 37, 56, 57, 58, 60, 61, 62, 68, 75, 76, 79)),
    0x4596e0: Binding('mk4_script_indexed_sound', 'game/script.c', ('handler',), ('fighter', 'arg_u8'),
                      evidence='4596e0 Cage air-throw indexed context sound selector 13', inline_values=(13,)),
    0x467fc0: Binding('cage_script_spike_tail', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='467fc0 completes the shared stage-spike scene and Cage finisher recovery'),
    0x46a790: Binding('cage_script_kick_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46a790 enters Cage high-kick recovery stream 4e5c70'),
    0x46aae0: Binding('cage_script_air_release', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46aae0 releases the paired air-throw victim then advances Cage stream 4e5e50'),
    0x46ab20: Binding('mk4_script_land', 'game/script.c', ('tail',),
                      evidence='46ab20 hands air-throw owner recovery to the common landing controller'),
    0x46c7f0: Binding('cage_script_air_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46c7f0 completes straight Cage air-kick continuation through landing'),
    0x46cde0: Binding('cage_script_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46cde0 completes Cage uppercut contact recovery through controller-ready'),
    0x46e150: Binding('cage_script_air_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46e150 completes Cage diagonal low air-kick continuation through landing'),
    0x46e5c0: Binding('cage_script_air_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46e5c0 completes Cage diagonal high air-kick continuation through landing'),
    0x46e730: Binding('cage_script_air_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46e730 completes Cage air high-punch continuation through landing'),
    0x46eea0: Binding('cage_script_air_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46eea0 completes Cage air low-punch continuation through landing'),
    0x46f080: Binding('cage_script_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46f080 completes Cage sweep contact continuation'),
    0x46f220: Binding('cage_script_roundhouse_start', 'game/c_cage.c', ('tail',),
                      evidence='46f220 selects Cage fast or ordinary roundhouse startup'),
    0x46f270: Binding('cage_script_finish', 'game/c_cage.c', ('tail',), flow='terminal',
                      evidence='46f270 completes Cage roundhouse contact recovery through controller-ready'),
}

CHARACTER_BINDINGS[17][0x46ac10] = Binding(
    'noob_saibot_script_air_throw_continue', 'game/c_noob_saibot.c', ('tail',),
    flow='terminal', evidence='46ac10 sets both paired bodies to gravity 0x147a and vertical velocity -0x1999 before entering 4e5ea0')
CHARACTER_BINDINGS[17][0x46ac90] = Binding(
    'noob_saibot_script_air_throw_continue', 'game/c_noob_saibot.c', ('tail',),
    script_target=0x4e5eb0, evidence='46ac90 directly enters the Noob paired landing continuation 4e5eb0')
CHARACTER_BINDINGS[17][0x46aca0] = Binding(
    'noob_saibot_script_air_throw_end', 'game/c_noob_saibot.c', ('tail',),
    flow='terminal', evidence='46aca0 subtracts 0x30a3 from the victim, releases the pair and returns to the fighter controller')


def bindings_for(character):
    from character_catalog import CHARACTERS
    links = {address: Binding('retail_script_%08x' % target, 'game/generated/%s.inc' % CHARACTERS[character], ('tail',),
                              script_target=target, evidence='%x: script_enter(%x); exact-entry native listing' % (address, target))
             for address, target in ((0x46dd00, 0x4e6330), (0x46dd10, 0x4e6360), (0x46dd20, 0x4e63b0))}
    return {**BINDINGS, **links, **CHARACTER_BINDINGS.get(character, {})}
