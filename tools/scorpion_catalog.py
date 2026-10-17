"""Small curated navigation roots for Scorpion in the supplied EXE revision.

This is an entry map, not a complete call graph or a port-status database.
Script addresses may be character-selected streams reached through native code.
See docs/scorpion/ for research context; executable bytes remain authoritative.
"""


def move(entry, scripts=(), contacts=(), native=(), buttons=()):
    return dict(entry=entry, scripts=scripts, contacts=contacts, native=native, buttons=buttons)


MOVES = {
    'high_punch': move(0x46d290, native=(('contact/repeat/recovery', 0x46d9b3),)),
    'lp_throw': move(0x42c560, (('roster-selected attacker/victim', 0x4de798),),
        native=(('precondition', 0x42c3d0), ('entry/roster selection', 0x42c590), ('release adjustment', 0x42ceb0))),
    'lk_throw': move(0x42b6b0, (('paired opening', 0x4de2f8), ('damage', 0x4de318), ('release', 0x4de328)),
        native=(('Scorpion entry', 0x42bdd0), ('paired continuation', 0x42bdf0),)),
    'jab': move(0x46dca0,
        (('attacker', 0x4e62f0), ('second punch', 0x4e6330), ('repeat', 0x4e6360), ('recovery', 0x4e63b0)),
        (('A', 1),), (('repeat predicate', 0x46de60),)),
    'high_kick': move(0x46a680, (('attacker', 0x4e5ca0), ('recovery', 0x4e5c70)), (('B', 1),)),
    'low_kick': move(0x46a7b0, (('Scorpion attacker', 0x4e6d08),), (('B', 6),)),
    'sweep': move(0x46ef90, (('attacker', 0x4e6798),), (('B', 35),)),
    'uppercut': move(0x46ccd0, (('attacker', 0x4e62a0),), (('A', 2), ('A', 6))),
    'roundhouse': move(0x46f1f0, (('attacker', 0x4e6844),), (('B', 33),)),
    'close_hp': move(0x46be40, (('attacker', 0x4e61e0),),
        buttons=(('contact follow-up', 0x4e7078),)),
    'combo_hp': move(0x46c290, (('attacker', 0x4e69b0),),
        buttons=(('contact follow-up', 0x4e7090),)),
    'close_hk': move(0x46ba60, (('attacker', 0x4e6190),),
        buttons=(('contact follow-up', 0x4e70a8),)),
    'crouch_lp': move(0x46b7f0),
    'crouch_hk': move(0x46b5a0, (('attacker', 0x4e60b8),), (('B', 25),)),
    'crouch_lk': move(0x46b710, (('attacker', 0x4e6158),), (('B', 23),)),
    'air_hp': move(0x46e6c0, (('attacker', 0x4e6658),), (('A', 4),),
        (('contact continuation', 0x46e7c0),), (('high contact', 0x4e6ee0), ('low contact', 0x4e70c0))),
    'air_lp': move(0x46ee30, (('attacker', 0x4e6710),), (('A', 5),),
        (('contact continuation', 0x46ef10),)),
    'air_kick': move(0x46c7a0, (('straight kick', 0x4e61f8),), (('B', 40),),
        (('contact continuation', 0x46cb10),), (('low contact', 0x4e70c0),)),
    'air_diagonal_hk': move(0x46e380, (('attacker', 0x4e6500),), (('B', 30),)),
    'air_diagonal_lk': move(0x46df50, (('attacker', 0x4e63d8),), (('B', 27),)),
    'air_throw': move(0x46aa80, (('paired attacker/victim', 0x4e5e28), ('release', 0x4e5e50), ('victim release', 0x4e9910)),
        native=(('command gate', 0x45ed70), ('release damage', 0x46aae0), ('landing', 0x46ab20), ('victim continuation', 0x4878c0), ('victim getup', 0x484b60))),
    'spear': move(0x49a630,
        (('attacker', 0x4ed448), ('return', 0x4ed4a8), ('victim hit', 0x4e9d00), ('victim stun', 0x4e9f20)),
        (('record', 0x4ebeb8),),
        (('projectile task', 0x49a6a0), ('victim pull', 0x49b170), ('arrival', 0x487350), ('stun counter', 0x487420))),
    'breathe_fire': move(0x468610, (('attacker', 0x4e5b58), ('grounded victim', 0x4e97f8)),
        (('record', 0x4ebf90),), (('playback branches', 0x468640), ('active query', 0x4689b0))),
    'teleport_punch': move(0x47fc90, (('attacker', 0x4e87b0), ('landing', 0x4e8818), ('grounded victim', 0x4e9938)),
        (('record', 0x4ebf48),), (('motion callback', 0x480040), ('contact continuation', 0x47fd90), ('block branch', 0x47fec0))),
    'draw_weapon': move(0x46d1d0, (('draw', 0x4e62c0), ('return', 0x4e62d8)),
        native=(('follow-up draw', 0x494610), ('armed branch: throw', 0x498270), ('HP slash', 0x496c20), ('LP slash', 0x496c30))),
    'sword_back_lp': move(0x496c50, (('attacker', 0x4ec8b0),), (('record', 0x4eb9c0),)),
    'sword_back_hp': move(0x496c90, (('attacker', 0x4ec918),), (('record', 0x4eb9d8),)),
    'sword_spin': move(0x480770, contacts=(('record', 0x4eb978),)),
    'sword_throw': move(0x498270, contacts=(('record', 0x4ebe10),), native=(('throw continuation', 0x498290),)),
    'fatality_1': move(0x467ce0, (('performer', 0x4e0278), ('victim', 0x4e0300)),
        native=(('scene entry', 0x43cbb0), ('Scorpion scene', 0x443840))),
    'fatality_2': move(0x467cd0, (('transformation', 0x4d2e58), ('creature attack', 0x4d2eb0), ('victim', 0x4d2ef8)),
        native=(('scene entry', 0x43bd70), ('creature rig setup', 0x41ea50))),
    'fan_fatality': move(0x467e00, (('performer', 0x4e0118), ('victim', 0x4e0188)),
        native=(('scene entry', 0x43d040),)),
    'spike_fatality': move(0x467e70, (('stage script', 0x4e6950),)),
}
