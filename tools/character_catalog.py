"""Character context and compact research roots; never implementation status."""
from scorpion_catalog import MOVES as SCORPION, move

CHARACTERS = ('scorpion', 'raiden', 'sonya', 'liu_kang', 'sub_zero', 'fujin',
              'shinnok', 'reiko', 'quan_chi', 'tanya', 'reptile', 'kai',
              'jarek', 'jax', 'cage', 'goro', 'kitana', 'noob_saibot')


def character_id(value):
    value = str(value).lower().replace('-', '_').replace(' ', '_')
    if value in CHARACTERS:
        return CHARACTERS.index(value)
    try:
        result = int(value, 0)
    except ValueError:
        raise ValueError('Unknown character %r; use a roster name or ID 0..17' % value) from None
    if not 0 <= result < len(CHARACTERS):
        raise ValueError('Character ID must be 0..17')
    return result


def character_name(character):
    return CHARACTERS[character].replace('_', ' ').title().replace('Sub Zero', 'Sub-Zero')


def moves_for(character):
    if character == 0:
        return SCORPION
    # These entry scripts contain the original character selectors. Native
    # follow-ups still need research for the requested character.
    shared = {key: SCORPION[key] for key in ('jab', 'uppercut', 'sweep', 'air_hp', 'air_lp',
                                           'air_kick', 'air_diagonal_hk', 'air_diagonal_lk',
                                           'air_throw', 'fan_fatality', 'spike_fatality')}
    shared.update(high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5c70))),
                  low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),)),
                  roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800),)))
    if character != 4:
        return shared
    shared.update(
        high_kick=move(0x46a680, (('Sub-Zero attacker', 0x4e5c80), ('recovery', 0x4e6998)), (('B', 2),)),
        low_kick=move(0x46a7b0, (('Sub-Zero attacker', 0x4e6da0),)),
        close_hp=SCORPION['close_hp'], close_hk=SCORPION['close_hk'],
        combo_hp=move(0x46c2a0, (('attacker', 0x4e69c8),)),
        crouch_lp=SCORPION['crouch_lp'], crouch_lk=SCORPION['crouch_lk'],
        crouch_hk=move(0x46b5a0, (('attacker', 0x4e5fd8),)),
        ice=move(0x49bc60, (('attacker', 0x4ed588),), contacts=(('C', 3),), native=(('projectile', 0x49bcb0), ('freeze', 0x486230), ('fast startup predicate', 0x45b650))),
        clone=move(0x47f4d0, contacts=(('D', 5),), native=(('retreat', 0x47f6d0), ('apex/retraction', 0x47f880), ('clone object', 0x47f9d0))),
        slide=move(0x47dc00, (('entry', 0x4e84d8), ('hit', 0x4e84f8), ('block', 0x4e8530)), native=(('contact/recovery', 0x47dc40),)),
        sidestep=move(0x469250, native=(('direction/playback', 0x4692e0),)),
        draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('return', 0x4e62d8)), native=(('follow-up draw', 0x494610), ('victim setup', 0x4946f0), ('scepter follow-up', 0x495710))),
        scepter_hp=move(0x4956c0, contacts=(('record', 0x4eb7b0),), native=(('shared phases', 0x494a00), ('hit recovery', 0x495110))),
        scepter_lp=move(0x495720, contacts=(('record', 0x4eb630),), native=(('shared phases', 0x494a00), ('stance blend', 0x495300))),
        scepter_back_hp=move(0x4956f0, contacts=(('record', 0x4eb930),), native=(('strike without travel setup', 0x495710),)),
        scepter_back_lp=move(0x495740, contacts=(('record', 0x4eb618),), native=(('freeze/hit cap', 0x486210),)),
        weapon_throw=SCORPION['sword_throw'],
        fatality_1=move(0x467ce0, (('setup', 0x4e0368), ('first pull', 0x4e0378), ('victim', 0x4e03f0), ('victim finish', 0x4e0420), ('victim cleanup', 0x4e0430)),
                        native=(('Sub-Zero scene', 0x445100), ('victim blends', 0x445510), ('scene objects', 0x445740), ('camera', 0x445920))),
        fatality_2=move(0x467cd0, (('breath', 0x4d2c10), ('victim', 0x4d2bf0), ('approach', 0x4d2c78), ('uppercut', 0x4d2ca0), ('finish', 0x4d2cc8)),
                        native=(('Sub-Zero scene', 0x41b3a0), ('current-range approach', 0x41b450), ('shatter', 0x41b610))),
    )
    return shared
