#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void scorpion_ready(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, fighter_airborne(fighter) ? S_JUMP : S_IDLE);
}

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord jab_record = {0xf333, -0x47ae, 0xcccc, 0x7333, 37, 11, 1, 0, 0xa3d};            /* 0x4ebd38, query A selector 1 */
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Battle *battle, const Fighter *fighter) {
  return fighter_opponent(battle, fighter)->action;
}

/* 0x46ddd0: one query per strike visit, skipped against a crouching victim
 * (0x2001), actions 0x106/0x107, or an early sweep (0x10c before sample 5).
 * The 0x102 gate reads victim +0x5c, which is not modeled. */
static void scorpion_jab_contact(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int action = victim->action;
  if (fighter->connected || action == 0x2001 || action == 0x106 || action == 0x107) {
    return;
  }
  if (action == 0x10c && fighter_sample(victim) < 5) {
    return;
  }
  battle_contact(battle, fighter, &jab_record);
}

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void scorpion_kick_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected && victim_action(battle, fighter) != 0x2001) {
    battle_contact(battle, fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void scorpion_sweep_contact(Battle *battle, Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(battle, fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void scorpion_uppercut_contact(Battle *battle, Fighter *fighter) {
  if (fighter->connected || victim_action(battle, fighter) == 0x2001) {
    return;
  }
  if (battle_contact(battle, fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

// clang-format off: Keep table entries on one row, including long rows.
static const MoveEvent kick_events[] = {{11, scorpion_kick_contact}, {12, scorpion_kick_contact}, {13, scorpion_kick_contact}};
static const MoveSegment kick_segments[] = {
  {0, 0, 1, 1},
  {3, 4, 2, 1},
  {10, 17, 0, 1},
  {13, 16, -1, 2},
  {16, 13, -1, 1},
};
// clang-format on

static const MoveSequence kick_sequence = {kick_segments, (int)(sizeof kick_segments / sizeof kick_segments[0]), kick_events, 3, 30, scorpion_ready, NULL};

static void scorpion_jab_next(Battle *battle, Fighter *fighter);
static void scorpion_jab_second_end(Battle *battle, Fighter *fighter);
static void scorpion_jab_finish_end(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter);
static void scorpion_uppercut_loop_end(Battle *battle, Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const MoveSegment jab_segments[] = {
  {0, 0, 1, 1},
  {8, 7, 0, 1},
};
static const MoveSegment jab_recover_segments[] = {
  {0, 9, 2, 1},
  {3, 17, 0, 1},
  {5, 19, 2, 1},
};
static const MoveSegment jab_second_segments[] = {{0, 0, 1, 1}};
static const MoveSegment jab_stop_segments[] = {{0, 10, 1, 1}};
static const MoveSegment jab_finish_segments[] = {
  {0, 11, 1, 1},
  {8, 18, 0, 1},
};
static const MoveSegment sweep_segments[] = {
  {0, 0, 2, 1},
  {4, 10, 0, 1},
  {5, 10, 1, 1},
};
static const MoveSegment uppercut_segments[] = {
  {0, 0, 1, 1},
  {6, 5, 0, 1},
  {7, 6, 1, 1},
};
static const MoveSegment uppercut_loop_segments[] = {
  {0, 8, -1, 1},
  {3, 7, 1, 1},
};
static const MoveSegment uppercut_late_segments[] = {
  {0, 9, 0, 1},
  {2, 8, -1, 1},
  {5, 7, 1, 1},
};
static const MoveSegment uppercut_miss_end_segments[] = {{0, 8, 1, 1}};
static const MoveSegment uppercut_hit_end_segments[] = {{0, 9, 1, 1}};
static const MoveEvent jab_events[] = {{8, scorpion_jab_contact}};
static const MoveEvent jab_strike_events[] = {{0, scorpion_jab_contact}};
static const MoveEvent sweep_events[] = {{18, scorpion_sweep_contact}, {19, scorpion_sweep_contact}};
static const MoveEvent uppercut_events[] = {{7, scorpion_uppercut_contact}, {8, scorpion_uppercut_contact}, {9, scorpion_uppercut_contact}};
// clang-format on

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}
#define EVENT_SEQUENCE(segments, events, count, end) {segments, COUNT(segments), events, COUNT(events), count, end, NULL}

static const MoveSequence jab_sequence = EVENT_SEQUENCE(jab_segments, jab_events, 9, scorpion_jab_next);
static const MoveSequence jab_recover_sequence = SEQUENCE(jab_recover_segments, 7, scorpion_ready);
static const MoveSequence jab_second_sequence = SEQUENCE(jab_second_segments, 10, scorpion_jab_second_end);
static const MoveSequence jab_stop_sequence = EVENT_SEQUENCE(jab_stop_segments, jab_strike_events, 3, scorpion_ready);
static const MoveSequence jab_finish_sequence = EVENT_SEQUENCE(jab_finish_segments, jab_strike_events, 9, scorpion_jab_finish_end);
static const MoveSequence sweep_sequence = EVENT_SEQUENCE(sweep_segments, sweep_events, 41, scorpion_ready);
static const MoveSequence uppercut_sequence = EVENT_SEQUENCE(uppercut_segments, uppercut_events, 10, scorpion_uppercut_recover);
static const MoveSequence uppercut_loop_sequence = SEQUENCE(uppercut_loop_segments, 5, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_late_sequence = SEQUENCE(uppercut_late_segments, 7, scorpion_uppercut_loop_end);
static const MoveSequence uppercut_miss_end_sequence = SEQUENCE(uppercut_miss_end_segments, 12, scorpion_ready);
static const MoveSequence uppercut_hit_end_sequence = SEQUENCE(uppercut_hit_end_segments, 11, scorpion_ready);

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .sequence = &jab_sequence, .action = 0x10b};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .sequence = &kick_sequence, .action = 0x100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .sequence = &sweep_sequence, .action = 0x10c};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .sequence = &uppercut_sequence, .action = 0x109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */
static const MoveDef s_jab_recover = {.clip = 294, .flags = RETAIL, .sequence = &jab_recover_sequence, .action = 0x10b};
static const MoveDef s_jab_second = {.clip = 295, .flags = RETAIL, .sequence = &jab_second_sequence, .action = 0x10b};
static const MoveDef s_jab_stop = {.clip = 295, .flags = RETAIL, .sequence = &jab_stop_sequence, .action = 0x10b};
static const MoveDef s_jab_finish = {.clip = 295, .flags = RETAIL, .sequence = &jab_finish_sequence, .action = 0x10b};
static const MoveDef s_uppercut_loop = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_loop_sequence, .action = 0x109};
static const MoveDef s_uppercut_late = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_late_sequence, .action = 0x109};
static const MoveDef s_uppercut_miss_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_miss_end_sequence, .action = 0x109};
static const MoveDef s_uppercut_hit_end = {.clip = 293, .flags = RETAIL, .sequence = &uppercut_hit_end_sequence, .action = 0x109};

static const MoveDef s_throw = {75, 0, MOVE_END, 6, 2, 25, 14, 42, 0.95f, 0.45f, SF_ATTACK | SF_LOCK | SF_LAUNCH | SF_UNBLOCKABLE, NULL, 0};
static const MoveDef s_block = {322, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_hit = {247, 0, MOVE_END, 0, 0, 24, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_HOLD, NULL, 0};
static const MoveDef s_knockdown = {186, 0, MOVE_END, 0, 0, 54, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION | SF_WAIT_LAND, NULL, 0};
static const MoveDef s_victory = {115, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};

// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef *const moves[S_COUNT] = {
  [S_IDLE] = &s_idle,
  [S_WALK] = &s_walk,
  [S_CROUCH] = &s_crouch,
  [S_JUMP] = &s_jump,
  [S_PUNCH] = &s_jab,
  [S_KICK] = &s_kick,
  [S_SWEEP] = &s_sweep,
  [S_UPPERCUT] = &s_uppercut,
  [S_THROW] = &s_throw,
  [S_BLOCK] = &s_block,
  [S_HIT] = &s_hit,
  [S_KNOCKDOWN] = &s_knockdown,
  [S_VICTORY] = &s_victory,
};
// clang-format on

static bool scorpion_repeat_jab(const Battle *battle, const Fighter *fighter) {
  const Fighter *other = fighter_opponent(battle, fighter);
  q16 dx = other->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  return (fighter->buffered & BUTTON_BIT(BUTTON_LOW_PUNCH)) != 0 && (fighter->facing > 0 ? dx >= 0 : dx <= 0);
}

static void scorpion_jab_next(Battle *battle, Fighter *fighter) {
  const MoveDef *next = scorpion_repeat_jab(battle, fighter) ? &s_jab_second : &s_jab_recover;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void scorpion_jab_second_end(Battle *battle, Fighter *fighter) {
  const MoveDef *next = scorpion_repeat_jab(battle, fighter) ? &s_jab_finish : &s_jab_stop;
  fighter_start_move(battle, fighter, S_PUNCH, next);
}

static void scorpion_jab_finish_end(Battle *battle, Fighter *fighter) {
  /* Native 0x46ddd0 queries contact before 0x46de60 consumes the next press. */
  scorpion_jab_contact(battle, fighter);
  scorpion_jab_next(battle, fighter);
}

static void scorpion_uppercut_recover(Battle *battle, Fighter *fighter) {
  bool late = fighter->contact_tick == 9;
  ContactResult result = fighter->contact;
  fighter->move_loops = result == CONTACT_MISS ? 3 : result == CONTACT_BLOCK ? 2 : 8;
  fighter_start_move(battle, fighter, S_UPPERCUT, late ? &s_uppercut_late : &s_uppercut_loop);
  /* Only the no-contact path holds sample 8 before final forward recovery. */
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

static void scorpion_uppercut_loop_end(Battle *battle, Fighter *fighter) {
  ContactResult result = fighter->contact;
  const MoveDef *next;
  fighter->move_loops--;
  if (fighter->move_loops > 0) {
    next = &s_uppercut_loop;
  } else {
    next = result == CONTACT_MISS ? &s_uppercut_miss_end : &s_uppercut_hit_end;
  }
  fighter_start_move(battle, fighter, S_UPPERCUT, next);
  fighter->contact = result;
  fighter->connected = result != CONTACT_MISS;
  if (result != CONTACT_MISS) {
    fighter->action = 0x1006;
  }
}

static void scorpion_jab(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
}

static void scorpion_kick(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_KICK, &s_kick);
}

static void scorpion_idle(Battle *battle, Fighter *fighter);
static void scorpion_walk(Battle *battle, Fighter *fighter);
static void scorpion_crouch(Battle *battle, Fighter *fighter);
static void scorpion_block(Battle *battle, Fighter *fighter);
static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = scorpion_idle,
    [S_WALK] = scorpion_walk,
    [S_CROUCH] = scorpion_crouch,
    [S_BLOCK] = scorpion_block,
    [S_PUNCH] = scorpion_jab,
    [S_KICK] = scorpion_kick,
};

/* Specials translated from spear_entry/return, teleport_entry/landing and
 * breathe_fire in build/retail-disassembly/scripts. Native loops are ordinary
 * C continuations. The camera-dependent teleport exit uses a fixed ten-tick
 * vanish in our engine; it retains the distance/wall exits and contact branches. */
static const AttackRecord spear_record = {0x3333, -0x1eb8, 0x947a, 0x3ae1, 70, 70, 112, ATTACK_MARK, 0xccc};
static const AttackRecord teleport_record = {0x128f5, -0x3ae1, 0xab85, 0x91eb, 90, 90, 0, ATTACK_MARK, 0x170a};
static const AttackRecord fire_record = {0x24e56, -0xc978, 0x23cac, 0xb7ce, 173, 173, 0, ATTACK_UNBLOCKABLE, 0x1eb8};
static const MoveDef s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x300};
static const MoveDef s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = 0x4000};
static const MoveDef s_spear_stun = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK, .action = 0x1008};
static const MoveDef s_teleport = {.clip = 312, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x209};
static const MoveDef s_teleport_block = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = 0x1011};
static const MoveDef s_teleport_land = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = 0x2008};
static const MoveDef s_fire = {.clip = 448, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x115};

static Fighter *opponent(Battle *battle, Fighter *fighter) {
  return &battle->fighters[fighter == &battle->fighters[0] ? 1 : 0];
}

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static void special_ready(Battle *battle, Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  scorpion_ready(battle, fighter);
}

static void spear_stun_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = 0;
  /* 0x487420: a 0x40-tick opening, cancelled naturally by a new hit. */
  if (fighter->ticks >= 64) {
    special_ready(battle, fighter);
  }
}

static void spear_return_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(79, 31 + fighter->ticks);
  if (fighter->ticks >= 49) {
    fighter->spear_ready = battle->ticks + 90;
    special_ready(battle, fighter);
  }
}

static void spear_tick(Battle *battle, Fighter *fighter);

static void spear_projectile(Battle *battle, Projectile *p) {
  Fighter *owner = &battle->fighters[p->owner], *victim = opponent(battle, owner);
  bool interrupted = owner->script != spear_tick && owner->script != spear_return_tick;
  if (interrupted || battle->contacts_off) {
    if (p->phase == 1 && victim->current_move == &s_snared) {
      special_ready(battle, victim);
    }
    p->kind = 0;
    return;
  }
  if (p->phase == 1) {
    if (victim->current_move != &s_snared || victim->health <= 0) {
      p->kind = 0;
      return;
    }
    q16 dx = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
    if (abs(dx) <= 0xcccc) {
      stop_body(victim);
      fighter_start_move(battle, victim, S_HIT, &s_spear_stun);
      victim->script = spear_stun_tick;
      victim->sample_override = 0;
      victim->flags |= FF_REACTING;
      fighter_start_move(battle, owner, S_PUNCH, &s_spear);
      owner->script = spear_return_tick;
      owner->sample_override = 31;
      p->kind = 0;
    } else {
      victim->body.pos[AXIS_X] += dx > 0 ? 0x3333 : -0x3333;
      victim->body.pos[AXIS_Y] = victim->body.ground;
      memcpy(p->pos, victim->body.pos, sizeof p->pos);
    }
    return;
  }
  if (p->phase == 2) {
    q16 dx = owner->body.pos[AXIS_X] - p->pos[AXIS_X];
    if (abs(dx) <= 0x3333) {
      p->kind = 0;
    } else {
      p->pos[AXIS_X] += dx > 0 ? 0x3333 : -0x3333;
    }
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  ContactResult result = battle_contact_from(battle, owner, &spear_record, p->pos, p->facing);
  if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    stop_body(victim);
    fighter_start_move(battle, victim, S_HIT, &s_snared);
    victim->sample_override = 5;
    victim->flags |= FF_REACTING;
    victim->damage_scale = 0x9999; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || abs(p->pos[AXIS_X]) > battle->arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

static void spear_tick(Battle *battle, Fighter *fighter) {
  int spawn = fighter->script_phase ? 8 : 10;
  int first = fighter->script_phase ? 9 : 0;
  fighter->sample_override = mini(30, first + 2 * fighter->ticks);
  if (fighter->ticks == spawn) {
    Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_SPEAR, 90, spear_projectile);
    if (p != NULL) {
      p->pos[AXIS_X] += fighter->facing * 0xc000;
      p->pos[AXIS_Y] = -0x16666;
      p->vel[AXIS_X] = fighter->facing * 0x3333;
    }
  }
  if (fighter->ticks >= 60) {
    bool pulling = false;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      const Projectile *p = &battle->projectiles[i];
      pulling |= p->kind == PROJECTILE_SPEAR && &battle->fighters[p->owner] == fighter && p->phase == 1;
    }
    if (!pulling) {
      special_ready(battle, fighter);
    }
  }
}

static bool can_spear(const Battle *battle, const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  return battle->phase == PHASE_FIGHT && battle->ticks >= fighter->spear_ready && victim->hits <= 3 && victim->action != 0x1008;
}

static void scorpion_spear(Battle *battle, Fighter *fighter) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_PUNCH, &s_spear);
  fighter->script = spear_tick;
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = battle->ticks + 90;
}

static void teleport_land_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(27, 18 + fighter->ticks);
  if (fighter->ticks >= 10) {
    special_ready(battle, fighter);
  }
}

static void teleport_block_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->ticks < 21 ? 8 : maxi(0, 29 - fighter->ticks);
  if (fighter->ticks >= 29 && !fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, &s_teleport_land);
    fighter->script = teleport_land_tick;
    fighter->sample_override = 18;
  }
}

static void teleport_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  fighter->sample_override = mini(9, fighter->ticks);
  if (fighter->script_phase == 0) {
    if (fighter->ticks < 10 && fighter_distance(battle, fighter) < 5.9f && abs(fighter->body.pos[AXIS_X]) < battle->arena_radius - Q16_ONE) {
      return;
    }
    q16 distance = abs(victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]);
    int direction = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? 1 : -1;
    q16 offset = 0x26666 + maxi(0, 4 * Q16_ONE - distance);
    q16 x = victim->body.pos[AXIS_X] + direction * offset;
    fighter->body.pos[AXIS_X] = abs(x) > battle->arena_radius ? victim->body.pos[AXIS_X] : x;
    fighter->facing = -direction;
    fighter->body.vel[AXIS_X] = -direction * 0x2666;
    fighter->sample_override = 9;
    fighter->script_phase = 1;
    return;
  }
  fighter->sample_override = 9;
  if (fighter->script_phase == 2) {
    if (++fighter->script_timer == 6) {
      fighter->body.gravity = 0x1999;
    }
    if (fighter->script_timer >= 6 && !fighter_airborne(fighter)) {
      special_ready(battle, fighter);
    }
    return;
  }
  if (!fighter_airborne(fighter)) {
    special_ready(battle, fighter);
    return;
  }
  ContactResult result = battle_contact(battle, fighter, &teleport_record);
  if (result == CONTACT_BLOCK) {
    fighter_start_move(battle, fighter, S_JUMP, &s_teleport_block);
    fighter->script = teleport_block_tick;
    fighter->sample_override = 8;
    fighter->body.vel[AXIS_X] = -fighter->facing * 0x28f;
    fighter->body.vel[AXIS_Y] = -0x2147;
    fighter->body.gravity = 0x1ca;
  } else if (result != CONTACT_MISS || fighter->ticks >= 70) {
    stop_body(fighter);
    fighter->flags &= ~(unsigned)FF_TELEPORTING;
    fighter->script_phase = 2;
    fighter->script_timer = 0;
  }
}

static bool can_teleport(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && battle->ticks >= fighter->teleport_ready && fighter_opponent(battle, fighter)->hits < 3;
}

static void scorpion_teleport(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_teleport);
  fighter->script = teleport_tick;
  fighter->sample_override = 0;
  fighter->teleport_ready = battle->ticks + 69;
  fighter->flags |= FF_TELEPORTING;
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(battle, fighter)->body.pos[AXIS_X] ? -1 : 1;
  fighter_set_speed(battle, fighter, -0x2666);
  fighter->body.vel[AXIS_Y] = -0x170a;
  fighter->body.gravity = 0x1ca;
}

static void fire_tick(Battle *battle, Fighter *fighter) {
  int tick = fighter->ticks;
  bool fast = fighter->script_phase != 0;
  if (tick < 4) {
    fighter->sample_override = tick + 1;
  } else if (tick < 9) {
    fighter->sample_override = tick;
  } else if (tick < 13) {
    fighter->sample_override = tick - 1;
  } else if (fast) {
    fighter->sample_override = mini(34, tick < 19 ? tick - 2 : tick < 32 ? tick - 3 : tick - 4);
  } else {
    fighter->sample_override = mini(34, tick < 48 ? 11 + (tick - 13) / 2 : tick - 19);
  }
  if (tick == (fast ? 19 : 27)) {
    battle_contact(battle, fighter, &fire_record);
  }
  if (tick >= (fast ? 39 : 54)) {
    special_ready(battle, fighter);
  }
}

static void scorpion_fire(Battle *battle, Fighter *fighter) {
  stop_body(fighter);
  fighter_start_move(battle, fighter, S_PUNCH, &s_fire);
  fighter->script = fire_tick;
  fighter->sample_override = 1;
  fighter->script_phase = (fighter_opponent(battle, fighter)->flags & FF_REACTING) != 0;
}

static bool fighting(const Battle *battle, const Fighter *fighter) {
  (void)fighter;
  return battle->phase == PHASE_FIGHT;
}

static void sword_hp(Battle *battle, Fighter *fighter);
static void sword_lp(Battle *battle, Fighter *fighter);
static void sword_spin(Battle *battle, Fighter *fighter);
static void scorpion_weapon(Battle *battle, Fighter *fighter);
static bool can_weapon(const Battle *battle, const Fighter *fighter);
static bool can_pickup(const Battle *battle, const Fighter *fighter);
static void scorpion_pickup(Battle *battle, Fighter *fighter);

static void press_jab(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_hp : scorpion_jab);
}

static void press_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, scorpion_kick);
}

static void enter_uppercut(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_UPPERCUT);
}

static void enter_sweep(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_SWEEP);
}

static void enter_throw(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_THROW);
}

static void press_lp(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_distance(battle, fighter) <= 0.7f ? enter_throw : fighter->armed ? sword_lp : scorpion_jab);
}

static void press_lk(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? enter_sweep : scorpion_kick);
}

static void press_air_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_lp : scorpion_jab);
}

static void press_crouch_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_spin : enter_uppercut);
}

static void press_uppercut(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_sweep(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_sweep);
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 24, CMD_AIR, can_teleport, scorpion_teleport},
  {BUTTON_LOW_PUNCH, {TOKEN_BACK, TOKEN_BACK}, 2, 27, 0, can_spear, scorpion_spear},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 20, 0, fighting, scorpion_fire},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, can_weapon, scorpion_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, scorpion_pickup},
};
static const ButtonTable scorpion_standing = {{[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable scorpion_crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_sweep, [BUTTON_LOW_KICK] = press_sweep}};
static const ButtonTable scorpion_jumping = {{[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_kick}};
static const Controls scorpion_controls = {commands, COUNT(commands), {&scorpion_standing, &scorpion_crouching, &scorpion_jumping}};
// clang-format on

/* Weapon scripts 0x4e62c0/0x4e62d8, 0x4ec8b0/0x4ec918 and 0x4e8850;
 * slash parameter blocks 0x4ec880/0x4ec898; native throw 0x498290. */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord sword_records[] = {
  {98304, -35389, 57016, 39321, 48, 12, 119, 0, 13107}, /* HP / LP */
  {124518, -34078, 83230, 51118, 49, 12, 119, 0, 13107}, /* back LP */
  {111411, -34078, 83230, 51118, 50, 12, 119, 0, 13107}, /* back HP */
  {78643, 1900, 193331, 24903, 99, 10, 118, 0, 3276}, /* spin */
  {110690, -60882, 68157, 62914, 156, 156, 4, 0, 11796}, /* air HP */
  {101580, -4587, 64225, 53739, 155, 155, 4, 0, 11796}, /* air LP */
  {57606, -37945, 73990, 35979, 154, 154, 0, ATTACK_MARK, 16384}, /* throw */
};
// clang-format on
#define SWORD_MOVE(name, clip_id, action_id) static const MoveDef name = {.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = action_id}
SWORD_MOVE(s_draw, 244, 0x1004);
SWORD_MOVE(s_sword_hp, 235, 0x10e);
SWORD_MOVE(s_sword_lp, 236, 0x10e);
SWORD_MOVE(s_sword_back_lp, 237, 0x112);
SWORD_MOVE(s_sword_back_hp, 238, 0x112);
SWORD_MOVE(s_spin_start, 223, 0x407);
SWORD_MOVE(s_spin, 224, 0x407);
SWORD_MOVE(s_sword_throw, 208, 0x30f);
SWORD_MOVE(s_pickup, 209, 0x2006);
SWORD_MOVE(s_sword_air_hp, 252, 0x603);
SWORD_MOVE(s_sword_air_lp, 251, 0x604);
static const MoveDef s_armed_idle = {.clip = 286, .last = MOVE_END};
static const MoveDef s_armed_walk = {.clip = 277, .last = MOVE_END};
static const MoveDef s_armed_crouch = {.clip = 219, .last = MOVE_END, .flags = SF_HOLD};
static const MoveDef s_armed_block = {.clip = 245, .last = MOVE_END, .flags = SF_HOLD};

static void scorpion_idle(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_IDLE, fighter->armed ? &s_armed_idle : &s_idle);
}

static void scorpion_walk(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_WALK, fighter->armed ? &s_armed_walk : &s_walk);
}

static void scorpion_crouch(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_CROUCH, fighter->armed ? &s_armed_crouch : &s_crouch);
}

static void scorpion_block(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_BLOCK, fighter->armed ? &s_armed_block : &s_block);
}

static void sword_object_tick(Battle *battle, Projectile *p) {
  if (p->phase == 2) {
    p->lifetime = 2; /* Resting weapons last until pickup or the round reset. */
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  if (p->phase == 0) {
    ContactResult contact = battle_contact_from(battle, &battle->fighters[p->owner], &sword_records[6], p->pos, p->facing);
    if (contact != CONTACT_MISS) {
      p->phase = 1;
      p->vel[AXIS_X] = 0;
      p->vel[AXIS_Y] = -0x2666;
      p->lifetime = 180;
    } else if (p->ticks >= 30 || abs(p->pos[AXIS_X]) >= battle->arena_radius) {
      p->kind = 0;
    }
  } else {
    p->vel[AXIS_Y] += 0x28f;
    if (p->pos[AXIS_Y] >= -0x1999 && p->vel[AXIS_Y] >= 0) {
      p->pos[AXIS_Y] = -0x1999;
      memset(p->vel, 0, sizeof p->vel);
      p->phase = 2;
      p->lifetime = 2;
    }
  }
}

static void sword_drop(Battle *battle, Fighter *fighter) {
  if (!fighter->armed) {
    return;
  }
  Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_SWORD, 180, sword_object_tick);
  if (p != NULL) {
    p->phase = 1;
    p->vel[AXIS_Y] = -0x2666;
  }
  fighter->armed = false;
}

static void equip_sword(Fighter *fighter) {
  fighter->armed = true;
  fighter->weapon_used = true;
  fighter->disarm = sword_drop;
}

static void sword_slash_tick(Battle *battle, Fighter *fighter) {
  int clip = fighter_move(fighter)->clip, tick = fighter->ticks;
  int start = clip == 238 ? 26 : 17;
  int active = clip == 235 || clip == 236 ? 4 : 3;
  int record = clip == 237 ? 1 : clip == 238 ? 2 : 0;
  if ((clip == 237 && tick == 9) || (clip == 238 && tick == 16)) {
    fighter_set_speed(battle, fighter, 0x1999);
  }
  if (tick >= start && tick < start + active && !fighter->connected) {
    battle_contact(battle, fighter, &sword_records[record]);
  }
  if (fighter->connected || tick >= start + active) {
    fighter->body.vel[AXIS_X] = fighter->body.vel[AXIS_Z] = 0;
    fighter->action = 0x1006;
  }
  int end = clip == 237 ? 48 : clip == 238 ? 41 : 43;
  if (fighter->connected) {
    end -= record ? 4 : 1;
    fighter->sample_override = tick + (clip == 238 ? 0 : 1);
  } else if (clip == 237) {
    fighter->sample_override = tick < 21 ? tick : tick < 25 ? 20 : tick - 3;
  } else if (clip == 238) {
    fighter->sample_override = tick < 26 ? tick : tick < 30 ? tick - 1 : tick < 34 ? 28 : tick - 4;
  } else {
    fighter->sample_override = tick;
  }
  if (tick >= end) {
    special_ready(battle, fighter);
  }
}

static void sword_air_tick(Battle *battle, Fighter *fighter) {
  int hp = fighter_move(fighter)->clip == 252;
  int start = hp ? 8 : 9;
  fighter->sample_override = mini(9, fighter->ticks);
  if (fighter->ticks >= start && fighter->ticks <= 10 && !fighter->connected) {
    battle_contact(battle, fighter, &sword_records[hp ? 4 : 5]);
  }
  if (!fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, &s_teleport_land);
    fighter->script = teleport_land_tick;
    fighter->sample_override = 18;
  }
}

static void sword_attack(Battle *battle, Fighter *fighter, bool hp) {
  bool back = fighter_holds_back(battle, fighter), air = fighter_airborne(fighter);
  const MoveDef *move = air ? (hp ? &s_sword_air_hp : &s_sword_air_lp) : back ? (hp ? &s_sword_back_hp : &s_sword_back_lp) : (hp ? &s_sword_hp : &s_sword_lp);
  fighter_start_move(battle, fighter, air ? S_JUMP : S_PUNCH, move);
  fighter->script = air ? sword_air_tick : sword_slash_tick;
  fighter->sample_override = 0;
}

static void sword_hp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, true);
}

static void sword_lp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, false);
}

static void spin_tick(Battle *battle, Fighter *fighter) {
  if (fighter->script_phase != 0) {
    if (++fighter->script_timer >= 12) {
      special_ready(battle, fighter);
    }
    return;
  }
  fighter->sample_override = fighter->ticks % 12;
  ContactResult result = CONTACT_MISS;
  if (fighter->ticks > 0 && fighter->ticks % 12 == 0) {
    result = battle_contact(battle, fighter, &sword_records[3]);
  }
  if (result == CONTACT_BLOCK || fighter_opponent(battle, fighter)->hits >= 3) {
    stop_body(fighter);
    fighter->script_phase = 1;
    fighter->script_timer = 0;
    return;
  }
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    special_ready(battle, fighter);
    return;
  }
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  fighter->body.vel[AXIS_X] = direction * 0xf5c;
}

static void spin_start_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(3, fighter->ticks);
  if (fighter->ticks >= 4) {
    fighter_start_move(battle, fighter, S_PUNCH, &s_spin);
    fighter->script = spin_tick;
    fighter->sample_override = 0;
  }
}

static void sword_spin(Battle *battle, Fighter *fighter) {
  fighter_start_move(battle, fighter, S_PUNCH, &s_spin_start);
  fighter->script = spin_start_tick;
  fighter->sample_override = 0;
}

static void draw_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->ticks + (fighter->ticks >= 35);
  if (fighter->ticks == 22) {
    equip_sword(fighter);
  }
  if (fighter->ticks >= 48) {
    special_ready(battle, fighter);
  }
}

static void sword_throw_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = fighter->ticks - (fighter->ticks >= 11);
  if (fighter->ticks == 22 && fighter->armed) {
    Projectile *p = battle_spawn_projectile(battle, fighter, PROJECTILE_SWORD, 90, sword_object_tick);
    if (p != NULL) {
      p->pos[AXIS_X] += fighter->facing * 0x9999;
      p->pos[AXIS_Y] = -0x13333;
      p->vel[AXIS_X] = fighter->facing * 0x3333;
    }
    fighter->armed = false;
  }
  if (fighter->ticks >= 62) {
    special_ready(battle, fighter);
  }
}

static bool can_weapon(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && (fighter->armed || !fighter->weapon_used);
}

static void scorpion_weapon(Battle *battle, Fighter *fighter) {
  if (fighter->armed) {
    fighter_start_move(battle, fighter, S_PUNCH, &s_sword_throw);
    fighter->script = sword_throw_tick;
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_sword(fighter);
    fighter_start_move(battle, fighter, S_PUNCH, &s_sword_back_lp);
    fighter->script = sword_slash_tick;
  } else {
    fighter_start_move(battle, fighter, S_PUNCH, &s_draw);
    fighter->script = draw_tick;
  }
  fighter->sample_override = 0;
}

static int pickup_index(const Battle *battle, const Fighter *fighter) {
  if (fighter->armed || !fighter->crouching) {
    return -1;
  }
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &battle->projectiles[i];
    q16 dx = p->pos[AXIS_X] - fighter->body.pos[AXIS_X], dz = p->pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
    if (p->kind == PROJECTILE_SWORD && p->phase == 2 && q16_mul(dx, dx) + q16_mul(dz, dz) <= Q16_ONE) {
      return i;
    }
  }
  return -1;
}

static bool can_pickup(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FIGHT && pickup_index(battle, fighter) >= 0;
}

static void pickup_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(14, fighter->ticks);
  if (fighter->ticks >= 15) {
    special_ready(battle, fighter);
  }
}

static void scorpion_pickup(Battle *battle, Fighter *fighter) {
  int index = pickup_index(battle, fighter);
  if (index < 0) {
    special_ready(battle, fighter);
    return;
  }
  battle->projectiles[index].kind = 0;
  equip_sword(fighter);
  fighter_start_move(battle, fighter, S_PUNCH, &s_pickup);
  fighter->script = pickup_tick;
  fighter->sample_override = 0;
}

const CharacterDef mk4_scorpion = {GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, &scorpion_controls, enter, -76873};
