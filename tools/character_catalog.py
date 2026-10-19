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


def type69_weapon_moves():
    return dict(
        draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('return', 0x4e62d8), ('air HP', 0x4e66d0), ('air LP', 0x4e66f0)),
                         native=(('equip', 0x46d260),)),
        axe_hp=move(0x496cd0, contacts=(('WEAPON', 26),)),
        axe_lp=move(0x496cf0, contacts=(('WEAPON', 24),)),
        axe_back_hp=move(0x496ce0, contacts=(('WEAPON', 23),)),
        axe_back_lp=move(0x496d10, (('first victim', 0x4ea040), ('follow-up victim', 0x4ea0b8)), (('WEAPON', 25), ('WEAPON', 27)),
                         native=(('input motion', 0x496d20), ('secondary strike', 0x494f30))),
        weapon_throw=SCORPION['sword_throw'],
    )


def _moves_for(character):
    if character == 0:
        return SCORPION
    # These entry scripts contain the original character selectors. Native
    # follow-ups still need research for the requested character.
    shared = {key: SCORPION[key] for key in ('high_punch', 'lp_throw', 'lk_throw', 'jab', 'uppercut', 'sweep', 'air_hp', 'air_lp',
                                           'air_kick', 'air_diagonal_hk', 'air_diagonal_lk',
                                           'air_throw', 'fan_fatality', 'spike_fatality')}
    shared.update(high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5c70))),
                  low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),)),
                  roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800), ('fast start', 0x4e6860), ('normal start', 0x4e6878))))
    if character in (5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17):
        shared.update(close_hp=SCORPION['close_hp'], combo_hp=SCORPION['combo_hp'], close_hk=SCORPION['close_hk'])
    if character == 16:
        # Kitana's roster slot aliases Sonya's retail script slot. Keep a
        # distinct Kitana profile while auditing the same selected roots.
        kitana_moves = moves_for(2)
        kitana_moves['shadow_low'] = move(
            0x498b70,
            (('shadow low playback', 0x4ed268),),
        )
        kitana_moves['shadow_up'] = move(
            0x498e70,
            (('shadow up playback', 0x4ed2c0),),
        )
        return kitana_moves
    if character == 17:
        # Noob Saibot's roster slot aliases Reiko's retail script slot. Keep
        # the same selected roots while auditing the slot under its own ID.
        noob_moves = moves_for(7)
        noob_moves['air_throw'] = move(
            0x46aa80,
            (
                ('victim release', 0x4e9910),
                ('Noob air-throw opener', 0x4e5e78),
                ('Noob air-throw release', 0x4e5e50),
                ('Noob pair continuation', 0x4e5ea0),
                ('Noob landing continuation', 0x4e5eb0),
            ),
            native=(('command gate', 0x45ed70), ('release damage', 0x46aae0),
                    ('landing', 0x46ab20), ('victim continuation', 0x4878c0),
                    ('victim getup', 0x484b60)),
        )
        noob_moves['shadow_low'] = move(
            0x498b70,
            (('shadow low playback', 0x4ed268),),
        )
        noob_moves['shadow_up'] = move(
            0x498e70,
            (('shadow up playback', 0x4ed2c0),),
        )
        return noob_moves
    if character == 5:
        # Fujin's roster slot uses the same decoded VM entry set as Reiko for
        # these common attacks, but its selectors and contact records are
        # roster-specific. Keep the roots explicit for compiler coverage.
        shared.update(
            fatality_1=move(0x467ce0, (("wind opening", 0x4e1f58), ("wind cycle", 0x4e1f68), ("crossbow draw", 0x4e1f80), ("crossbow shot", 0x4e1f98)), native=(("scene entry", 0x44f4f0), ("victim lift", 0x450090), ("three shots", 0x44f6f0), ("bolt", 0x44fa10))),
            fatality_2=move(0x467cd0, (("owner wind", 0x4dfed0), ("victim wind hold", 0x4ea2d8), ("victim fall", 0x4ea2a8)), native=(("scene entry", 0x43bea0), ("victim", 0x4816b0), ("limb separation", 0x4818f0))),
            wind_grab=move(0x480360, (("opening", 0x4e8830), ("victim release", 0x4e9fb0)), native=(("held control", 0x4803a0), ("capture", 0x480510), ("blocked", 0x480660), ("victim orbit", 0x487a10))),
            triple_kick=move(0x47f010, (("startup", 0x4e8740), ("fast startup", 0x4e8770), ("first victim", 0x4ea040), ("middle victim", 0x4ea050), ("final victim", 0x4ea0b8)), (("D", 9),), native=(("contact", 0x47f070), ("paired follow-through", 0x47f390), ("blocked recoil", 0x47f480))),
            wind=move(0x46a620, (("wind caster", 0x4e5c50), ("caster recovery", 0x4e5c20)), (("D", 8),), native=(("vortex victim", 0x487ed0),)),
            wind_slam=move(0x46a5f0, (("slam owner", 0x4e5c00), ("victim bounce", 0x4e9ff0)), native=(("slam victim", 0x488310),)),
            wind_victim=move(0x487ed0, (("vortex landing", 0x4e9fe0),)),
            draw_weapon=move(0x46d1d0, (("draw", 0x4e62c0), ("equip return", 0x4e62d8), ("air HP", 0x4e66d0), ("air LP", 0x4e66f0)), native=(("equip", 0x46d260),)),
            crossbow_hp=move(0x495c30, native=(("upward shot", 0x499db0), ("independent bolt", 0x499f90))),
            crossbow_lp=move(0x495c60, (("shot recovery", 0x4ed408),), (("C", 16),), native=(("straight shot", 0x49a370), ("downward shot", 0x499c30))),
            crossbow_followup=move(0x495cf0, (("short shot", 0x4ed788),), native=(("four-shot sequence", 0x49a190),)),
            crossbow_reaction=move(0x486770, (("bolt victim", 0x4e9e10),)),
            aerial_dive=move(0x47bf60, (("startup", 0x4e8258), ("recoil", 0x4e8298)), (("D", 15),), native=(("contact table", 0x47bf90), ("land", 0x47bfd0))),
            lp_throw=move(0x42c560, (('Fujin selector', 0x4de798), ('extended throw', 0x4de610), ('short throw', 0x4de6b8))),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328))),
            jab=move(0x46dca0, (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)), (('A', 1),)),
            uppercut=move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            sweep=move(0x46ef90, (('attacker', 0x4e6798),), (('B', 36),)),
            air_hp=move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),)),
            air_lp=move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),)),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 39),)),
            air_diagonal_hk=move(0x46e380, (('attacker', 0x4e6500),), (('B', 32),)),
            air_diagonal_lk=move(0x46df50, (('attacker', 0x4e63d8),), (('B', 29),)),
            air_throw=move(0x46aa80, (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release damage', 0x46aae0), ('landing', 0x46ab20), ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
            fan_fatality=move(0x467e00, (('performer', 0x4e0118), ('victim', 0x4e0188), ('owner cycle', 0x4e0138), ('owner release', 0x4e0148), ('victory', 0x4e0158), ('victim release', 0x4e01a0)), native=(('scene entry', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),)),
            high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5d20)), (('B', 0),)),
            low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),), (('B', 5),)),
            roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800), ('fast start', 0x4e6890), ('normal start', 0x4e68a8)), (('B', 34),)),
        )
    if character == 7:
        # Reiko's retail slot uses these exact script entries. Several streams
        # share addresses with Scorpion, but their task context and native
        # continuations are selected with Reiko's roster ID.
        shared.update(
            lp_throw=move(
                0x42c560,
                (
                    ('Reiko attacker/victim', 0x4de798),
                    ('extended throw', 0x4de640),
                    ('short throw', 0x4de6b8),
                    ('throw owner continuation', 0x4de668),
                    ('throw victim recovery', 0x4e9b40),
                    ('throw victim recovery', 0x4e9bd0),
                ),
            ),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328))),
            jab=move(0x46dca0, (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)), (('A', 1),)),
            uppercut=move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            sweep=move(0x46ef90, (('attacker', 0x4e6798),), (('B', 35),)),
            air_hp=move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),)),
            air_lp=move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),)),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 40),)),
            air_diagonal_hk=move(0x46e380, (('attacker', 0x4e6500),), (('B', 30),)),
            air_diagonal_lk=move(0x46df50, (('attacker', 0x4e63d8),), (('B', 27),)),
            air_throw=move(0x46aa80, (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release damage', 0x46aae0), ('landing', 0x46ab20), ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
            fan_fatality=move(0x467e00, (('performer', 0x4e0118), ('victim', 0x4e0188), ('owner cycle', 0x4e0138), ('owner release', 0x4e0148), ('victory', 0x4e0158), ('victim release', 0x4e01a0)), native=(('scene entry', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),)),
            high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5c70)), (('B', 1),)),
            low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),), (('B', 6),)),
            roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800), ('fast start', 0x4e6890), ('normal start', 0x4e68a8)), (('B', 33),)),
        )
    if character == 7:
        shared.update(type69_weapon_moves())
    if character == 9:
        # Tanya uses the same decoded common VM entry set, but the selector
        # branches in these streams land on Tanya's own body clips and attack
        # descriptors. Keep her roster selection explicit: the character
        # tables are verified by `--script ... --raw --character tanya`.
        shared.update(
            lp_throw=move(0x42c560, (('Tanya attacker/victim', 0x4de798),)),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328))),
            jab=move(0x46dca0, (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)), (('A', 1),)),
            uppercut=move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            sweep=move(0x46ef90, (('attacker', 0x4e6798),), (('B', 36),)),
            air_hp=move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),)),
            air_lp=move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),)),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 39),)),
            air_diagonal_hk=move(0x46e380, (('attacker', 0x4e6500),), (('B', 30),)),
            air_diagonal_lk=move(0x46df50, (('attacker', 0x4e63d8),), (('B', 27),)),
            air_throw=move(0x46aa80, (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release damage', 0x46aae0), ('landing', 0x46ab20), ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
            fan_fatality=move(0x467e00, (('performer', 0x4e0118), ('victim', 0x4e0188), ('owner cycle', 0x4e0138), ('owner release', 0x4e0148), ('victory', 0x4e0158), ('victim release', 0x4e01a0)), native=(('scene entry', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),)),
            high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5c70)), (('B', 1),)),
            low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),), (('B', 5),)),
            roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800), ('fast start', 0x4e6890), ('normal start', 0x4e68a8)), (('B', 34),)),
        )
    if character == 10:
        # Reptile's input and CPU tables expose three additional script
        # families beyond the common combat roots. The 0x47a690 loop can
        # enter a second generated stream after its D-contact window.
        shared.update(
            lp_throw=move(0x42c560, (('selected attacker/victim', 0x4de798), ('extended throw', 0x4de640),
                                     ('short throw', 0x4de6b8),
                                     ('owner return', 0x4de668), ('victim reaction 107', 0x4e9b40),
                                     ('victim reaction 102', 0x4e9bd0)),
                          native=(('selector', 0x42cc00), ('extended continuation', 0x42cc40),
                                  ('short continuation', 0x42ce80))),
            input_47a690=move(0x47a690,
                              (('selected stream', 0x4e8128), ('loop idle stream', 0x4e8148),
                               ('loop timeout stream', 0x4e8174)),
                              native=(('contact-loop callback', 0x47a6d0), ('loop idle tail', 0x47ab50))),
            input_47afe0=move(0x47afe0, (('selected stream', 0x4e8178),),
                              native=(('contact query', 0x48d450),)),
            input_4988d0=move(0x4988d0, (('selected stream', 0x4ed1e8),)),
            reaction_40=move(0x483550, (('reaction continuation', 0x4e9620),),
                             native=(('shared landing', 0x488b40),)),
        )
        return shared
    if character == 11:
        # Kai's selected input/CPU targets include additional kick, recovery,
        # victory and draw streams beyond the shared fighter roots.
        shared.update(
            input_47bd10=move(0x47bd10, (('selected continuation', 0x4e8210),),
                              native=(('motion finish', 0x47bd40),)),
            input_47bd40=move(0x47bd40, (('selected continuation', 0x4e8238),),
                              native=(('recovery finish', 0x47bf90),)),
            input_47c520=move(0x47c520, (('selected continuation', 0x4e8320),),
                              native=(('handstand controls', 0x47c580),)),
            handstand_spin=move(0x47c850, (('spin startup', 0x4e8338),), contacts=(('D', 13),), native=(('spin controls', 0x47c880),)),
            handstand_rear=move(0x47cc80, (('rear startup', 0x4e83a0), ('rear recovery', 0x4e83d8)), contacts=(('B', 11),), native=(('rear contact', 0x47ccc0),)),
            handstand_exit=move(0x47ccf0, (('reverse intro', 0x4e83e8),)),
            handstand_reaction=move(0x482de0, (('standing stagger', 0x4e9308), ('crouching stagger', 0x4e9360), ('knockdown', 0x4ea170), ('rear stagger', 0x4ea1c0))),
            input_47cc10=move(0x47cc10, (('selected continuation', 0x4e8358), ('scheduler continuation', 0x4e8390)),
                              contacts=(('B', 10),), native=(('clip finish', 0x47cc50),)),
            input_478df0=move(0x478df0, (('selected continuation', 0x4e8870),),
                              native=(('clip finish', 0x478e20),)),
            input_47cbf0=move(0x47cbf0, (('selected continuation', 0x4e88c8),),
                              native=(('clip finish', 0x47cc10),)),
            draw_weapon=move(0x46d1d0, (('weapon draw', 0x4e62c0), ('draw continuation', 0x4e62d8), ('air HP', 0x4e66d0), ('air LP', 0x4e66f0), ('back strike', 0x4ec5a8), ('HP victim', 0x4e8b78), ('LP victim', 0x4e8f50), ('back victim', 0x4e8bc0)), contacts=(('weapon', 31), ('weapon', 32), ('weapon', 33)),
                             native=(('follow-up draw', 0x46d260),)),
            lk_throw=move(0x42b6b0,
                          (('paired opening', 0x4de2f8), ('damage', 0x4de318),
                           ('release', 0x4de328)),
                          native=(('victim continuation', 0x42be10),)),
            weapon_followup=move(0x4959f0, contacts=(('WEAPON', 31),)),
            lp_throw=move(0x42c560,
                          (('selected attacker/victim', 0x4de798),
                           ('extended throw', 0x4de610), ('short throw', 0x4de6b8)),
                          native=(('variant selector', 0x42cb80),)),
            high_kick=move(0x46a680, (('selected startup', 0x4e5ca0), ('recovery', 0x4e6998)), (('B', 2),)),
            roundhouse=move(0x46f1f0,
                            (('Kai attacker', 0x4e6800), ('fast start', 0x4e6890), ('normal start', 0x4e68a8)),
                            (('B', 34),)),
        )
        return shared
    if character == 12:
        # Jarek's LP throw branches into four separately reached VM streams;
        # its high/low kick and roundhouse selectors use Jarek's roster slot.
        shared.update(
            fan_fatality=move(0x467e00,
                              (('performer', 0x4e0118), ('victim', 0x4e0188),
                               ('owner cycle', 0x4e0138), ('owner release', 0x4e0148),
                               ('victory', 0x4e0158), ('victim release', 0x4e01a0),
                               ('owner setup continuation', 0x4e0128),
                               ('victory setup continuation', 0x4e01b0)),
                              native=(('scene entry', 0x43d040), ('owner setup', 0x43d460),
                                      ('victory setup', 0x43db50))),
            lp_throw=move(0x42c560,
                          (('Jarek selected attacker/victim', 0x4de798),
                           ('extended throw', 0x4de640), ('short throw', 0x4de6b8),
                           ('owner return', 0x4de668)),
                          native=(('selector', 0x42cc00), ('extended continuation', 0x42cc40),
                                  ('short continuation', 0x42ce80), ('peer facing toggle', 0x490350))),
            draw_weapon=move(0x46d1d0, (('weapon draw', 0x4e62c0), ('draw continuation', 0x4e62d8), ('air HP', 0x4e66d0), ('air LP', 0x4e66f0)),
                             native=(('follow-up draw', 0x46d260),)),
            weapon_hp=move(0x495830, contacts=(('WEAPON', 37),)),
            weapon_lp=move(0x495980, contacts=(('WEAPON', 36),)),
            weapon_back_hp=move(0x495860, (('upper', 0x4ec4f0),), (('WEAPON', 33),), native=(('contact', 0x4958b0),)),
            weapon_back_lp=move(0x4959c0, contacts=(('WEAPON', 35),)),
            weapon_reaction=move(0x4821e0, (('knife victim', 0x4e8c08),)),
            input_47b5d0=move(0x47b5d0, (('high-kick continuation', 0x4e81a8),),
                              native=(('clip finish', 0x47b600),)),
            input_4797f0=move(0x4797f0, (('standing HP special', 0x4e80a0),)),
            cpu_479b20=move(0x479b20, (('CPU kick', 0x4e80a8), ('high-kick follow-up', 0x4e80c8),
                                       ('second follow-up', 0x4e80f4)),
                            native=(('contact selector', 0x479b70), ('first follow-up tail', 0x479f00),
                                    ('second follow-up selector', 0x47a030), ('final follow-up selector', 0x47a160))),
            cpu_497540=move(0x497540,
                            (('projectile response', 0x4ed118), ('projectile impact', 0x4ed190)),
                            native=(('projectile callback', 0x497580), ('contact completion', 0x497b60),
                                    ('impact setup', 0x497b00))),
            high_kick=move(0x46a680, (('Jarek attacker', 0x4e5ca0), ('recovery', 0x4e5d20)), (('B', 0),)),
            low_kick=move(0x46a7b0, (('Jarek attacker', 0x4e5d90),), (('B', 5),)),
            sweep=move(0x46ef90, (('Jarek attacker', 0x4e6798),), (('B', 35),)),
            uppercut=move(0x46ccd0, (('Jarek attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            roundhouse=move(0x46f1f0,
                            (('Jarek attacker', 0x4e6800), ('fast start', 0x4e6860), ('normal start', 0x4e6878)),
                            (('B', 33),)),
        )
        return shared
    if character == 13:
        # Jax's throw selector enters a separate attacker/victim stream and
        # the heavy punch command exposes the common stage finisher path.
        shared.update(
            lp_throw=move(0x42c560, (('Jax selector', 0x4de798), ('selected throw', 0x4de4f0)),
                          native=(('throw continuation', 0x42c7f0),)),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328))),
            draw_weapon=move(0x46d1d0, (('weapon draw', 0x4e62c0), ('draw continuation', 0x4e62d8)),
                             native=(('follow-up draw', 0x46d260),)),
            # 4e8174 is the 47af90 callback word read by 490ae0, not a VM
            # program; 4e8178 behind it is Jarek's 47afe0 stream.
            dash_punch=move(0x47ab10, (('dash', 0x4e8150), ('hit victim 157', 0x4ea028),
                                       ('grounded victim 168', 0x4e9e38), ('airborne victim 168', 0x4e89d8)), (('D', 22),),
                            native=(('contact window', 0x47ab50), ('dash callback', 0x47af90), ('contact branches', 0x47acc0),
                                    ('blocked', 0x47af10), ('final reaction', 0x47ae90), ('victim 157', 0x4883f0),
                                    ('victim 168', 0x4867b0), ('air victim 168', 0x481ce0), ('roll getup', 0x488c00))),
            missile=move(0x496f80, (('cast', 0x4ed090),), (('C', 4),),
                         native=(('heading snapshot', 0x49c610), ('independent task 80', 0x496fc0),
                                 ('contact waiter', 0x49c690), ('contact removal', 0x49bb00), ('arena bounds', 0x49bbd0))),
            # Jax's BLOCK air record targets 46a990, not the shared 46aa80
            # runner: 4e5dd8 opens owner 84/victim 144, 46a9f0 drops the pair
            # at gravity 0x3d7 and 46aa40 releases reaction 166 into 4e98e8.
            air_throw=move(0x46a990, (('paired capture', 0x4e5dd8), ('falling pair', 0x4e5df8),
                                      ('owner release', 0x4e5e18), ('victim release', 0x4e98e8)),
                           native=(('command gate', 0x45ed70), ('falling pair', 0x46a9f0), ('release damage', 0x46aa40),
                                   ('victim reaction 166', 0x484a10), ('victim getup', 0x484a30))),
            spike_fatality=move(0x467e70, (('selected special', 0x4e6950),),
                                native=(('stage continuation', 0x467fc0),)),
            fatality_1=move(0x467ce0, (('scene opening', 0x4e1390), ('performer loop', 0x4e13a0),
                                      ('victim presentation A', 0x4e13b0), ('victim presentation B', 0x4e13c0),
                                      ('victim continuation A', 0x4e13d0), ('victim continuation B', 0x4e13e0),
                                      ('scene actor task A', 0x4e18f0), ('scene actor task B', 0x4e1958)),
                            native=(('Jax roster-selected scene entry', 0x4494c0), ('scene callback', 0x449620),
                                    ('performer continuation', 0x4496d0), ('secondary actor continuation', 0x44a9f0),
                                    ('victim task continuation', 0x449890), ('victim final continuation', 0x4498d0),
                                    ('scene actor setup', 0x43d720), ('global F1 dispatcher', 0x43cbb0))),
            fatality_2=move(0x467cd0, (('scene opening', 0x4e5a80), ('performer loop', 0x4e5a98),
                                      ('victim continuation', 0x4e5b38), ('victim reaction task', 0x4e5b10)),
                            native=(('Jax roster-selected scene entry', 0x4669c0), ('scene callback', 0x466a90),
                                    ('victim task callback', 0x466dd0), ('victim task finish', 0x466df0),
                                    ('reaction task setup', 0x466b40), ('reaction task continuation', 0x466bb0),
                                    ('global F2 dispatcher', 0x43bd70))),
            input_468f00=move(0x468f00, native=(('persistent action controller', 0x428750),
                                                 ('controller return', 0x46f650))),
            roundhouse=move(0x46f1f0,
                            (('Jax selected attacker', 0x4e6800),
                             ('fast start', 0x4e6890), ('normal start', 0x4e68a8)),
                            ),
        )
        shared.update(type69_weapon_moves())
        return shared
    if character == 14:
        # Cage uses the shared combat VM roots, with two distinct fatality
        # scene entries selected by the roster's F1/F2 tables. His command
        # tables also select a powered punch, a shadow kick, and a CPU kick.
        shared.update(
            fatality_1=move(0x467ce0, (('scene setup', 0x4e12a0), ('scene loop', 0x4e12b0), ('continuation 4e12c0', 0x4e12c0), ('continuation 4e12d0', 0x4e12d0), ('continuation 4e12e0', 0x4e12e0), ('continuation 4e12f0', 0x4e12f0), ('continuation 4e1300', 0x4e1300), ('continuation 4e1310', 0x4e1310), ('continuation 4e1320', 0x4e1320), ('continuation 4e1330', 0x4e1330), ('continuation 4e1340', 0x4e1340), ('continuation 4e1350', 0x4e1350), ('continuation 4e1360', 0x4e1360), ('continuation 4e1370', 0x4e1370), ('continuation 4e1380', 0x4e1380)),
                            native=(('roster dispatch', 0x448160), ('scene continuation', 0x448480))),
            fatality_2=move(0x467cd0, (('scene entry', 0x4e5870), ('uppercut', 0x4e5880), ('victory', 0x4e58d8), ('victim start', 0x4e5b38), ('victim hit', 0x4e5900)),
                            native=(('roster dispatch', 0x465790), ('scene continuation', 0x4658a0), ('three repeats', 0x4658f0), ('head release', 0x465a80))),
            draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('equip return', 0x4e62d8), ('air HP', 0x4e66d0), ('air LP', 0x4e66f0)), native=(('equip', 0x46d260),)),
            weapon_upper=move(0x495860, (('upper strike', 0x4ec4f0),), (('WEAPON', 33),), native=(('contact', 0x4958b0),)),
            weapon_back_hp=move(0x4957b0, contacts=(('WEAPON', 37),)),
            weapon_lp=move(0x495800, contacts=(('WEAPON', 18),)),
            weapon_back_lp=move(0x496920, (('back LP', 0x4ec7f0),), (('WEAPON', 11),), native=(('contact', 0x496970),)),
            weapon_reaction=move(0x482300, (('back HP victim', 0x4e8cc0), ('LP victim', 0x4e8d38))),
            high_orb=move(0x497070, (('caster', 0x4ed0d8),), (('C', 0),), native=(('independent projectile', 0x4970d0),)),
            low_orb=move(0x497080, (('caster', 0x4ed0d8),), (('C', 0),), native=(('independent projectile', 0x4970d0),)),
            groin_punch=move(0x468aa0, (('owner reverse recovery', 0x4e5ba8), ('victim stun', 0x4e9f78)), (('D', 27),),
                             native=(('contact loop', 0x48d450), ('victim reaction', 0x4879c0))),
            powered_punch=move(0x479ec0, (('attack', 0x4e80d0), ('first victim', 0x4ea040), ('blocked rewind', 0x4e84b8)), (('D', 26),),
                               native=(('contact', 0x479f00), ('paired hit', 0x47a030), ('miss recovery', 0x47a160), ('paired cycle', 0x47a280), ('blocked recoil', 0x47d940))),
            cpu_kick=move(0x479b20, (('attack', 0x4e80a8), ('victim', 0x4e9718)), (('D', 25),),
                          native=(('return continuation', 0x479b70), ('contact recovery', 0x479de0), ('motion decay', 0x47bf50))),
        )
        shared['high_kick'] = move(0x46a680, (('selected startup', 0x4e5ca0), ('recovery', 0x4e6998)), (('B', 2),))
        return shared
    if character == 15:
        # Goro shares the common combat streams, while roster-selected
        # contact records and scene tables keep their own character context.
        shared.update(
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328)),
                          native=(('roster continuation', 0x42bdd0), ('victim continuation', 0x42bdf0),
                                  ('follow-up return', 0x42be10))),
            lp_throw=move(0x42c560, (('selected attacker/victim', 0x4de798), ('repeated damage', 0x4de7e8),
                                     ('final damage', 0x4de6e4)),
                          native=(('selector', 0x42c3d0), ('throw continuation', 0x42c590), ('victim continuation', 0x42ceb0),
                                  ('five-phase sequence', 0x42c770), ('phase repeat', 0x42c7b0))),
            jab=move(0x46dca0, (('attacker', 0x4e62f0), ('second punch', 0x4e6330),
                                ('repeat', 0x4e6360), ('recovery', 0x4e63b0)), (('A', 1),),
                     native=(('recovery callback', 0x46de60),)),
            uppercut=move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            sweep=move(0x46ef90, (('attacker', 0x4e6798),), (('B', 35),)),
            air_hp=move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),),
                        native=(('recovery callback', 0x46e7c0),)),
            air_lp=move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),),
                        native=(('recovery callback', 0x46ef10),)),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 40),),
                          native=(('recovery callback', 0x46cb10),)),
            air_diagonal_hk=move(0x46e380, (('attacker', 0x4e6500),), (('B', 31),)),
            air_diagonal_lk=move(0x46df50, (('attacker', 0x4e63d8),), (('B', 29),)),
            air_throw=move(0x46aa80, (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50),
                                      ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release damage', 0x46aae0),
                                   ('landing', 0x46ab20), ('victim continuation', 0x4878c0),
                                   ('victim getup', 0x484b60))),
            fan_fatality=move(0x467e00, (('performer', 0x4e0118), ('victim', 0x4e0188),
                                         ('owner cycle', 0x4e0138), ('owner release', 0x4e0148),
                                         ('victory', 0x4e0158), ('victim release', 0x4e01a0)),
                              native=(('scene entry', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),)),
            high_kick=move(0x46a680, (('character-selected attacker', 0x4e5ca0), ('recovery', 0x4e5c70)), (('B', 1),)),
            low_kick=move(0x46a7b0, (('character-selected attacker', 0x4e5d90),), (('B', 6),)),
            roundhouse=move(0x46f1f0, (('character-selected attacker', 0x4e6800),
                                       ('fast start', 0x4e6860), ('normal start', 0x4e6878)), (('B', 33),)),
            # Goro's own command handlers (4e6ef8[48..52]) and their victims.
            special_punch=move(0x43af90, (('attacker', 0x4dfb60), ('189 victim', 0x4e8dd0)), (('D', 32),),
                               native=(('contact rows', 0x43afc0), ('189 entry', 0x482420))),
            special_kick=move(0x43b170, (('attacker', 0x4dfb70), ('190 victim', 0x4ea3e0)), (('D', 33),),
                              native=(('contact rows', 0x43b1a0), ('190 entry', 0x483570))),
            special_uppercut=move(0x43b640, (('192 victim', 0x4e8e50), ('192 victim slide', 0x4e8e80)), (('D', 35),),
                                  native=(('clip 600 rows', 0x43b640), ('192 entry', 0x482480))),
            leap_stomp=move(0x43b320, (('rise', 0x4dfbb8), ('stomp hit', 0x4dfbd0), ('miss landing', 0x4dfc38),
                                       ('191 victim', 0x4ea400)), (('D', 34),),
                            native=(('rise/drop/contact', 0x43b370), ('return', 0x43b590), ('191 victim hold', 0x484330))),
            quake_stomp=move(0x43b7e0, (('jump', 0x4dfc50), ('landing', 0x4dfc60), ('136 victim', 0x4e95c8)),
                             native=(('pose blend', 0x48fde0), ('landing damage', 0x43b8c0), ('136 entry', 0x4834d0))),
            fireball=move(0x49ca70, (('clip 592 launch program', 0x4ed790),), (('C', 22),),
                          native=(('heading snapshot and launch', 0x49ca70), ('task 83 object', 0x49ca90))),
        )
        return shared
    if character in (1, 2):
        # Exact roster dispatch: 4ddfb8[1/2] -> 42bce0/42bed0.
        # Shared selector roots remain explicit; native-entered recoveries are
        # listed alongside them rather than borrowing Scorpion's streams.
        shared.update(
            lk_throw=move(0x42b6b0, (("Raiden paired throw", 0x4de218), ("released victim", 0x4e99c0)) if character == 1 else (("Sonya paired throw", 0x4de360),),
                          native=(("roster entry", 0x42bce0 if character == 1 else 0x42bed0),)),
            high_kick=move(0x46a680, (("roster selector", 0x4e5ca0), ("recovery", 0x4e6998 if character == 1 else 0x4e5d20)),
                           (("B", 2 if character == 1 else 0),)),
            close_hp=SCORPION['close_hp'], close_hk=SCORPION['close_hk'], combo_hp=SCORPION['combo_hp'],
            crouch_lp=SCORPION['crouch_lp'], crouch_lk=SCORPION['crouch_lk'],
            crouch_hk=move(0x46b5a0, (("roster selector", 0x4e6020),)),
            roundhouse=move(0x46f1f0, (("roster selector", 0x4e6800),
                ("fast start", 0x4e6890 if character == 1 else 0x4e6860),
                ("normal start", 0x4e68a8 if character == 1 else 0x4e6878))),
            draw_weapon=SCORPION['draw_weapon'],
            weapon_throw=SCORPION['sword_throw'],
            sidestep=move(0x469250, native=(("shared direction/playback", 0x4692e0),)),
            victory=move(0x4712e0, (("roster selector", 0x4e6a40),)),
        )
        shared.update(
            sweep=move(0x46ef90, (("roster selector", 0x4e6798),), (("B", 36 if character == 1 else 35),)),
            air_kick=move(0x46c7a0, (("roster selector", 0x4e61f8),), (("B", 39 if character == 1 else 40),)),
            air_diagonal_hk=move(0x46e380, (("roster selector", 0x4e6500),), (("B", 31 if character == 1 else 32),)),
            air_diagonal_lk=move(0x46df50, (("roster selector", 0x4e63d8),), (("B", 28 if character == 1 else 29),)),
        )
        if character == 1:
            shared.update(
                lp_throw=move(0x42c560, (("roster selector", 0x4de798), ("extended throw", 0x4de640), ("short throw", 0x4de6b8), ("extended owner recovery", 0x4de668), ("extended victim", 0x4e9b40), ("short victim", 0x4e9bd0))),
                fatality_1=move(0x467ce0, (("performer", 0x4e0210), ("owner release", 0x4e0220)), native=(("scene", 0x440fc0),)),
                fatality_2=move(0x467cd0, (("scene entry", 0x4e56c8), ("first playback", 0x4e56d8), ("repeated playback", 0x4e56f0), ("held pose", 0x4e5728), ("ending", 0x4e5740), ("owner hold", 0x4e5760), ("victim", 0x4e5770), ("victim hold", 0x4e5788)), native=(("scene", 0x4647f0),)),
                staff_hp=move(0x496a40, contacts=(("weapon", 4),)), staff_lp=move(0x496ba0, contacts=(("weapon", 5),)), staff_back_hp=move(0x496a70, contacts=(("weapon", 7),)), staff_back_lp=move(0x496be0, contacts=(("weapon", 6),)),
                lightning=move(0x49bdd0, (("startup and recovery", 0x4ed5f0),), (("C", 2),), (("independent projectile", 0x49be10),)),
                torpedo=move(0x47d5f0, (("fast startup", 0x4e8440), ("ordinary startup", 0x4e8458)), (("D", 0),)),
                teleport=move(0x47cd00, native=(("rise and relocation", 0x47cd00),)),
            )
        else:
            shared.update(
                lp_throw=move(0x42c560, (("roster selector", 0x4de798), ("victim release", 0x4e9b88))),
                air_throw=move(0x46ae60, (("paired owner", 0x4e5ed0), ("victim release", 0x4e98a0)), native=(("release", 0x46aed0), ("victim getup", 0x4847e0))),
                fatality_1=move(0x467ce0, (("performer", 0x4e01c0), ("owner loop", 0x4e01d0)), native=(("scene", 0x43f420),)),
                fatality_2=move(0x467cd0, (("scene entry", 0x4e5930), ("opening", 0x4e5940), ("cycle", 0x4e5960), ("first cycle", 0x4e5980), ("second cycle", 0x4e5990), ("owner ending", 0x4e59a8), ("victim", 0x4e59c8), ("victim end", 0x4e5a20)), native=(("scene", 0x465cb0),)),
                weapon_hp=move(0x4966b0, contacts=(("weapon", 8),)), weapon_lp=move(0x4966e0, contacts=(("weapon", 9),)),
                weapon_back_hp=move(0x496720, (("attacker", 0x4ec7a0),), (("weapon", 10),)),
                weapon_back_lp=move(0x496920, (("attacker", 0x4ec7f0),), (("weapon", 11),)),
                ring=move(0x49b9a0, (("startup and recovery", 0x4ed4c0),), (("C", 4),), (("independent projectile", 0x49ba50),)),
                leg_grab=move(0x47dee0, (("paired grab", 0x4e8578), ("miss or block", 0x4e85a8), ("victim recovery", 0x4e9a68)), (("D", 3),)),
                bicycle=move(0x47e650, native=(("rise and contact", 0x47e7a0),)),
                square_wave=move(0x47e180, (("launch", 0x4e85c0),)),
                cartwheel=move(0x47b690, (("startup", 0x4e81d0),)),
            )
    # Reactions reached by these records execute on arbitrary victim rosters.
    # Bind their generated bodies into the shared reaction dispatch explicitly.
    if character in (1, 2):
        reached = {
            'staff_hp': (("staff victim", 0x4ea068), ("weapon victim", 0x4e8e18)),
            'lightning': (("victim", 0x4e9d50),),
            'torpedo': (("held victim", 0x4ea108), ("ground release", 0x4e9ad0), ("air release", 0x4e9ae8)),
        } if character == 1 else {
            'crouch_hk': (("victim", 0x4e9110),),
            'weapon_hp': (("victim", 0x4e8b38),),
            'weapon_lp': (("victim", 0x4e8f50),),
            'ring': (("victim", 0x4e9de8),),
            'bicycle': (("victim", 0x4ea130),),
            'cartwheel': (("victim", 0x4ea0b8),),
        }
        for name, scripts in reached.items():
            shared[name] = {**shared[name], 'scripts': (*shared[name]['scripts'], *scripts)}
        if character == 2:
            for name, selector in (('bicycle', 1), ('square_wave', 2), ('cartwheel', 18)):
                shared[name] = {**shared[name], 'contacts': (("D", selector),)}
    if character == 3:
        shared.update(
            fireball_reaction=move(0x486730, (('native C0 victim', 0x4e9de8),)),
            fatality_1=move(0x467ce0, native=(('Liu Kang dragon scene', 0x450930), ('victim', 0x451190), ('blood/cuts', 0x451300))),
            fatality_2=move(0x467cd0, (('setup events', 0x4dfff8), ('paired opening/projectile', 0x4e0020)), native=(('Liu Kang scene', 0x43c3d0), ('fireball object', 0x43c540), ('victim hold', 0x481360), ('victim explosion', 0x481420))),
            sidestep=move(0x469250, native=(('direction/playback', 0x4692e0),)),
            draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('return', 0x4e62d8), ('air HP selector', 0x4e66d0), ('air LP selector', 0x4e66f0)), native=(('follow-up draw', 0x494610), ('Liu Kang weapon', 0x4964d0))),
            sword_hp=move(0x4963e0, (('repeat program', 0x4ec680),), (('weapon', 15),)),
            sword_lp=move(0x4964d0, contacts=(('weapon', 13), ('weapon', 16)), native=(('second strike', 0x496500),)),
            sword_back_hp=move(0x496670, contacts=(('weapon', 17),)),
            sword_back_lp=move(0x496920, (('attacker', 0x4ec7f0),), (('weapon', 11),)),
            weapon_throw=SCORPION['sword_throw'],
            high_fireball=move(0x49bf90, (('high', 0x4ed6c0),), (('C', 0),), (('spawn', 0x49c160),)),
            low_fireball=move(0x49bf50, (('low', 0x4ed678),), (('C', 1),), (('spawn', 0x49c110),)),
            air_fireball=move(0x49bfe0, (('air', 0x4ed6f8),), (('C', 0),), (('spawn', 0x49c0d0), ('landing', 0x49c010))),
            bicycle_kick=move(0x47eaf0, (('startup', 0x4e8610), ('finish', 0x4e8678), ('blocked', 0x4e8688), ('released ground victim', 0x4e9ad0), ('released air victim', 0x4e9ae8)), (('D', 6),), (('hold charge predicate', 0x45eab0), ('paired hit', 0x47eb50), ('victim hold', 0x488870), ('victim release', 0x4859b0))),
            flying_kick=move(0x47edc0, (('launch', 0x4e86a8), ('miss', 0x4e86d0), ('hit', 0x4e86e8), ('block', 0x4e8718)), (('D', 12),), (('contact loop', 0x47ee30),)),
            lp_throw=move(0x42c560, (('selected LP', 0x4de798), ('victim release', 0x4e9b68)), native=(('victim reaction 104', 0x485af0),)),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de3b0), ('paired continuation', 0x4de410), ('damage/release', 0x4de428), ('retreat entry', 0x4de468), ('retreat', 0x4de478), ('victim', 0x4e9a90)), native=(('Liu Kang entry', 0x42c040), ('damage', 0x42c0d0), ('victim reaction 80', 0x485910))),
            fan_fatality=move(shared['fan_fatality']['entry'], (*shared['fan_fatality']['scripts'][:2], ('owner cycle', 0x4e0138), ('owner release', 0x4e0148), ('owner victory', 0x4e0158), ('victim release', 0x4e01a0)), native=shared['fan_fatality']['native']),
            high_kick=move(0x46a680, (('Liu Kang attacker', 0x4e6c90), ('recovery', 0x4e5c70)), (('B', 1),)),
            low_kick=move(0x46a7b0, (('Liu Kang attacker', 0x4e6d08),), (('B', 6),)),
            sweep=move(0x46ef90, (('Liu Kang attacker', 0x4e67d8),), (('B', 36),)),
            roundhouse=move(0x46f1f0, (('Liu Kang attacker', 0x4e6e90), ('normal startup', 0x4e68a8), ('fast startup', 0x4e6890)), (('B', 34),)),
            air_kick=move(0x46c7a0, (('Liu Kang straight kick', 0x4e6df0),), (('B', 39),)),
            close_hp=SCORPION['close_hp'], combo_hp=SCORPION['combo_hp'], close_hk=SCORPION['close_hk'],
            crouch_lp=SCORPION['crouch_lp'], crouch_lk=SCORPION['crouch_lk'], crouch_hk=SCORPION['crouch_hk'],
        )
        shared.pop('air_throw')  # No Liu Kang BLOCK air-throw command record.
        return shared
    if character == 6:
        # Shinnok uses the common script roots below, selected through his
        # native move entries. Keep his roster entry and paired throw flow
        # explicit instead of inheriting Scorpion's descriptive labels.
        shared.update(
            draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('equip', 0x4e62d8), ('air HP', 0x4e66d0), ('air LP', 0x4e66f0))),
            weapon_hp=move(0x495bb0, contacts=(('WEAPON', 28),)),
            weapon_lp=move(0x495be0, contacts=(('WEAPON', 29),)),
            weapon_back_hp=move(0x496670, contacts=(('WEAPON', 17),)),
            weapon_back_lp=move(0x495c20, contacts=(('WEAPON', 30),)),
            weapon_followup=move(0x496c50, (('follow-up HP', 0x4ec8b0),), (('WEAPON', 2),)),
            weapon_reaction=move(0x482350, (('LP victim', 0x4e8d08), ('sweep victim', 0x4e9460), ('sweep getup', 0x4e9498))),
            lp_throw=move(0x42c560, (('Shinnok selected attacker/victim', 0x4de798),),
                          native=(('precondition', 0x42c3d0), ('roster entry', 0x42c590), ('release adjustment', 0x42ceb0))),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328)),
                          native=(('Shinnok entry', 0x42bdd0), ('paired continuation', 0x42bdf0))),
            jab=move(0x46dca0,
                     (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)),
                     (('A', 1),), (('repeat predicate', 0x46de60),)),
            high_kick=move(0x46a680, (('Shinnok attacker', 0x4e5ca0), ('recovery', 0x4e5c70)), (('B', 1),)),
            low_kick=move(0x46a7b0, (('Shinnok attacker', 0x4e5d90),), (('B', 6),)),
            sweep=move(0x46ef90, (('Shinnok attacker', 0x4e6798),), (('B', 35),)),
            uppercut=move(0x46ccd0, (('Shinnok attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            roundhouse=move(0x46f1f0,
                            (('Shinnok attacker', 0x4e6800), ('fast start', 0x4e6860), ('normal start', 0x4e6878)),
                            (('B', 33),)),
            air_throw=move(0x46aa80,
                           (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release adjustment', 0x46aae0), ('landing', 0x46ab20),
                                   ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), native=(('contact continuation', 0x46cb10),)),
            air_diagonal_lk=move(0x46df50, (('Shinnok attacker', 0x4e63d8),)),
            air_diagonal_hk=move(0x46e380, (('Shinnok attacker', 0x4e6500),)),
            air_hp=move(0x46e6c0, (('Shinnok attacker', 0x4e6658),), native=(('contact continuation', 0x46e7c0),)),
            air_lp=move(0x46ee30, (('Shinnok attacker', 0x4e6710),), native=(('contact continuation', 0x46ef10),)),
            fan_fatality=move(0x467e00,
                             (('performer', 0x4e0118), ('victim', 0x4e0188), ('owner cycle', 0x4e0138),
                              ('owner release', 0x4e0148), ('victory', 0x4e0158), ('victim release', 0x4e01a0)),
                             native=(('Shinnok scene', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),), native=(('Shinnok stage entry', 0x467fc0),)),
            fatality_1=move(0x467ce0,
                            (('owner vanish', 0x4e2ef0), ('owner hidden', 0x4e2f00), ('owner return', 0x4e2f10), ('owner landing', 0x4e2f20),
                             ('victory', 0x4e2f30), ('hand grab', 0x4e2f40), ('hand squeeze', 0x4e2f58), ('hand release', 0x4e2f68),
                             ('victim lift', 0x4e2f78), ('victim hold', 0x4e2f88), ('victim squeeze', 0x4e2f98), ('victim crush', 0x4e2fa8),
                             ('victim end', 0x4e2fb8)),
                            native=(('roster dispatch', 0x455000), ('victim', 0x4558f0), ('hand', 0x4556d0), ('owner return', 0x455190))),
            fatality_2=move(0x467cd0,
                            (('camera intro', 0x4e0088), ('owner vanish', 0x4e2fc8), ('owner hidden', 0x4e2fd8), ('first hand', 0x4e00b8),
                             ('second hand', 0x4e00e0), ('second hand tail', 0x4e00f8), ('victim', 0x4ea270)),
                            native=(('roster dispatch', 0x43c6d0), ('hands', 0x43c820), ('clap', 0x43ca30))),
        )
        return shared
    if character == 8:
        # Quan Chi shares the decoded VM roots used by the common fighter
        # entries, while his typed move table selects his own native runners
        # and contact selectors. Keep the slot-specific boundaries explicit.
        shared.update(
            high_punch=move(0x46d290, native=(('contact/repeat/recovery', 0x46d9b3),)),
            lp_throw=move(0x42c560, (('Quan Chi selected attacker/victim', 0x4de798),),
                          native=(('precondition', 0x42c3d0), ('entry/roster selection', 0x42c590), ('release adjustment', 0x42ceb0))),
            lk_throw=move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328)),
                          native=(('Quan Chi entry', 0x42bdd0), ('paired continuation', 0x42bdf0))),
            jab=move(0x46dca0,
                     (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)),
                     (('A', 1),), (('repeat predicate', 0x46de60),)),
            uppercut=move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
            sweep=move(0x46ef90, (('attacker', 0x4e6798),), (('B', 35),)),
            air_kick=move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 40),), (('contact continuation', 0x46cb10),)),
            air_hp=move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),), (('contact continuation', 0x46e7c0),)),
            air_lp=move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),), (('contact continuation', 0x46ef10),)),
            air_diagonal_hk=move(0x46e380, (('attacker', 0x4e6500),), (('B', 30),)),
            air_diagonal_lk=move(0x46df50, (('attacker', 0x4e63d8),), (('B', 27),)),
            air_throw=move(0x46aa80,
                           (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
                           native=(('command gate', 0x45ed70), ('release damage', 0x46aae0), ('landing', 0x46ab20),
                                   ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
            fan_fatality=move(0x467e00,
                              (('performer', 0x4e0118), ('victim', 0x4e0188), ('owner cycle', 0x4e0138),
                               ('owner release', 0x4e0148), ('victory', 0x4e0158), ('victim release', 0x4e01a0)),
                              native=(('Quan Chi scene entry', 0x43d040),)),
            spike_fatality=move(0x467e70, (('stage script', 0x4e6950),), native=(('stage continuation', 0x467fc0),)),
            high_kick=move(0x46a680, (('selected startup', 0x4e5ca0), ('recovery', 0x4e6998)), (('B', 2),)),
            low_kick=move(0x46a7b0, (('Quan Chi attacker', 0x4e5d90),), (('B', 5),)),
            roundhouse=move(0x46f1f0,
                            (('Quan Chi attacker', 0x4e6800), ('fast start', 0x4e6860), ('normal start', 0x4e6878)),
                            (('B', 34),)),
        )
        return shared
    if character != 4:
        return shared
    shared.update(
        lp_throw=move(0x42c560, (('roster-selected attacker/victim', 0x4de798), ('reaction 102 tumble body', 0x4e9bd0)),
                      native=(('precondition', 0x42c3d0), ('entry/roster selection', 0x42c590), ('owner return', 0x42ce80),
                              ('reaction 102', 0x485b70), ('victim getup', 0x488b80))),
        lk_throw=move(0x42b6b0, (('Sub-Zero paired opening', 0x4de490), ('paired counts', 0x4de4b8),
                               ('final five samples', 0x4de4e0), ('victim recovery', 0x4e9ee0)),
                      native=(('roster dispatch', 0x42b6e0), ('Sub-Zero entry', 0x42c180), ('paired continuation', 0x42c200),
                              ('damage/momentum', 0x42c220), ('owner return', 0x42c300), ('saved momentum', 0x48f6e0),
                              ('victim release', 0x4871e0), ('victim recovery end', 0x487220))),
        high_kick=move(0x46a680, (('Sub-Zero attacker', 0x4e5c80), ('recovery', 0x4e6998)), (('B', 2),)),
        low_kick=move(0x46a7b0, (('Sub-Zero attacker', 0x4e6da0),)),
        close_hp=SCORPION['close_hp'], close_hk=SCORPION['close_hk'],
        combo_hp=move(0x46c2a0, (('attacker', 0x4e69c8),)),
        crouch_lp=SCORPION['crouch_lp'], crouch_lk=SCORPION['crouch_lk'],
        crouch_hk=move(0x46b5a0, (('attacker', 0x4e5fd8),)),
        fan_fatality=move(shared['fan_fatality']['entry'], (*shared['fan_fatality']['scripts'][:2], ('owner cycle', 0x4e0138), ('owner release', 0x4e0148), ('owner victory', 0x4e0158), ('victim release', 0x4e01a0)), native=shared['fan_fatality']['native']),
        ice=move(0x49bc60, (('attacker', 0x4ed588),), contacts=(('C', 3),), native=(('projectile', 0x49bcb0), ('freeze', 0x486230), ('fast startup predicate', 0x45b650))),
        clone=move(0x47f4d0, contacts=(('D', 5),), native=(('retreat', 0x47f6d0), ('apex/retraction', 0x47f880), ('clone object', 0x47f9d0))),
        slide=move(0x47dc00, (('entry', 0x4e84d8), ('hit', 0x4e84f8), ('block', 0x4e8530)), native=(('contact/recovery', 0x47dc40),)),
        sidestep=move(0x469250, native=(('direction/playback', 0x4692e0),)),
        draw_weapon=move(0x46d1d0, (('draw', 0x4e62c0), ('return', 0x4e62d8), ('air HP selector', 0x4e66d0), ('air LP selector', 0x4e66f0)), native=(('follow-up draw', 0x494610), ('victim setup', 0x4946f0), ('scepter follow-up', 0x495710))),
        scepter_hp=move(0x4956c0, contacts=(('record', 0x4eb7b0),), native=(('shared phases', 0x494a00), ('hit recovery', 0x495110))),
        scepter_lp=move(0x495720, contacts=(('record', 0x4eb630),), native=(('shared phases', 0x494a00), ('stance blend', 0x495300))),
        scepter_back_hp=move(0x4956f0, contacts=(('record', 0x4eb930),), native=(('strike without travel setup', 0x495710),)),
        scepter_back_lp=move(0x495740, contacts=(('record', 0x4eb618),), native=(('freeze/hit cap', 0x486210),)),
        weapon_throw=SCORPION['sword_throw'],
        fatality_1=move(0x467ce0, (('setup', 0x4e0368), ('first pull', 0x4e0378), ('victim', 0x4e03f0), ('victim finish', 0x4e0420), ('victim cleanup', 0x4e0430), ('pull forward', 0x4e0388), ('pull reverse', 0x4e0398), ('second pull', 0x4e03a8), ('second reverse', 0x4e03b8), ('head release', 0x4e03c8), ('owner loop', 0x4e03d8)),
                        native=(('Sub-Zero scene', 0x445100), ('victim blends', 0x445510), ('scene objects', 0x445740), ('camera', 0x445920))),
        fatality_2=move(0x467cd0, (('breath', 0x4d2c10), ('victim', 0x4d2bf0), ('approach', 0x4d2c78), ('uppercut', 0x4d2ca0), ('finish', 0x4d2cc8), ('victory', 0x4e6a40)),
                        native=(('Sub-Zero scene', 0x41b3a0), ('current-range approach', 0x41b450), ('shatter', 0x41b610))),
    )
    return shared


# Roster slots whose retail BLOCK command list has no airborne-throw record.
# Sub-Zero, Raiden and Goro keep theirs as portable extensions.
NO_AIR_THROW = (5, 6, 7, 9, 10, 11, 12, 14)


def moves_for(character):
    result = dict(_moves_for(character))
    if character in NO_AIR_THROW:
        result.pop('air_throw', None)
    if character in (6, 8, 10, 12, 14, 15):
        entry = dict(result['air_kick'])
        result['air_kick'] = entry
        entry['scripts'] = tuple(entry['scripts']) + (('narrow miss reversal', 0x4e6270),)
        entry['contacts'] = (('B', 40),)
    result['high_punch'] = dict(result['high_punch'], contacts=(('A', 0),))
    return result
