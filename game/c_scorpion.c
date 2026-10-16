#include "game.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Original no-contact VM/native recovery trace. Steps, not stored clip length.
 * See docs/scorpion-retail-moves.md and tests/fixtures/original_kicks.json. */
static void scorpion_ready(Battle *battle, Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(battle, fighter, S_IDLE);
  }
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
 * The 0x102 gate is the low-kick crouching interval (native 0x46a970). */
static void scorpion_jab_contact(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int action = victim->action;
  if (fighter->connected || action == 0x2001 || action == 0x106 || action == 0x107) {
    return;
  }
  if ((action == 0x10c && fighter_sample(victim) < 5) || (action == 0x102 && victim->avoids_high)) {
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

static const MoveDef s_throw = {.clip = 75, .last = 64, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = 0x500};
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
static void sword_back_hp(Battle *battle, Fighter *fighter);
static void sword_back_lp(Battle *battle, Fighter *fighter);
static void sword_spin(Battle *battle, Fighter *fighter);
static void scorpion_weapon(Battle *battle, Fighter *fighter);
static bool can_weapon(const Battle *battle, const Fighter *fighter);
static bool can_pickup(const Battle *battle, const Fighter *fighter);
static void scorpion_pickup(Battle *battle, Fighter *fighter);

/* Ground scripts and their native tails: 4e6d08/46a8d0, 4e60b8,
 * 4e6158/46b740, 46b7f0, 4e61e0/46bf10, 4e69b0/46c2c0,
 * 4e6190/46bb60 and 4e6844/46f220. Times are our script ticks. */
enum {
  G_LOW_KICK,
  G_CROUCH_LP,
  G_CROUCH_LK,
  G_CROUCH_HK,
  G_CLOSE_HP,
  G_COMBO_HP,
  G_CLOSE_HK,
  G_ROUNDHOUSE
};

typedef struct GroundAttack {
  MoveDef move;
  AttackRecord record;
  int active, end_miss, end_contact;
} GroundAttack;

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = 0x102}, {94371, -13762, 72024, 23592, 17, 10, 6, 0, 7208}, 10, 33, 36},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = 0x107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = 0x106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = 0x104}, {48496, -24903, 44564, 30146, 25, 10, 5, 0, 7864}, 9, 29, 29},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = 0x108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = 0x108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = 0x103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = 0x10d}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = 0x10a};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = 0x10a};

static void open_followup(Fighter *fighter, const ButtonTable *buttons) {
  fighter->buttons = buttons;
  fighter->flags = (fighter->flags | FF_FOLLOW_UP) & ~(unsigned)FF_BUSY;
  fighter->action = 0x1006;
}

static void ground_attack_tick(Battle *battle, Fighter *fighter) {
  int index = fighter->script_phase;
  const GroundAttack *attack = &ground_attacks[index];
  int t = fighter->ticks, fast = index == G_ROUNDHOUSE && fighter->script_timer ? 5 : 0;
  int active = attack->active - fast;
  fighter->avoids_high = index == G_LOW_KICK && t >= 7 && t < 10;
  bool gate = (index == G_CROUCH_LP || index == G_CROUCH_LK) && victim_action(battle, fighter) == 0x2011;
  if (index == G_CLOSE_HP) {
    int action = victim_action(battle, fighter);
    gate = action == 0x2001 || action == 0x106 || action == 0x107;
  }
  if (!gate && !fighter->connected && t >= active && t <= active + (index == G_ROUNDHOUSE ? 2 : 0)) {
    if (battle_contact(battle, fighter, &attack->record) != CONTACT_MISS) {
      if (index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CLOSE_HK) {
        open_followup(fighter, index == G_CLOSE_HP ? &close_hp_followup : index == G_COMBO_HP ? &combo_hp_followup : &close_hk_followup);
      }
    }
  }
  if (t >= active) {
    fighter->action = 0x1006;
  }
  int end = (fighter->connected ? attack->end_contact : attack->end_miss) - fast;
  switch (index) {
    case G_LOW_KICK: {
      int reverse = fighter->connected ? 18 : 15;
      fighter->sample_override = t < 9 ? mini(17, t * 2) : t < reverse ? 17 : maxi(0, 17 - t + reverse);
      break;
    }
    case G_CROUCH_LP:
    case G_CROUCH_LK: {
      int reverse = active + (fighter->connected ? 4 : 1);
      fighter->sample_override = t < active ? mini(9, t + (index == G_CROUCH_LP ? 2 : 1)) : maxi(0, 9 - maxi(0, t - reverse));
      break;
    }
    case G_CROUCH_HK: {
      static const int hold[] = {11, 10, 11, 12};
      fighter->sample_override = t < 13 ? t : hold[((t - 13) / 2) % 4];
      break;
    }
    case G_CLOSE_HP: {
      int reverse = fighter->connected ? 18 : 12;
      fighter->sample_override = t < 10 ? t : t < reverse ? 9 : maxi(0, 8 - t + reverse);
      if (t >= reverse) {
        fighter->buttons = NULL;
        fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
      }
      break;
    }
    case G_COMBO_HP:
      fighter->sample_override = mini(27, t + 2 - (fighter->connected && t > active ? mini(3, t - active) : 0));
      break;
    case G_CLOSE_HK: {
      int reverse = fighter->connected ? 10 : 12;
      fighter->sample_override = t < 8 ? t + 5 : t < reverse ? 12 : maxi(0, 12 - t + reverse);
      if (t >= reverse) {
        fighter->buttons = NULL;
        fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
      }
      break;
    }
    case G_ROUNDHOUSE:
      fighter->sample_override = t <= 6 ? t * 2 : t < active ? mini(21, 12 + (t - 6) * (fast ? 2 : 1)) : mini(38, 22 + t - active);
      if (fighter->contact == CONTACT_BLOCK) {
        end = attack->end_miss - fast;
      }
      break;
  }
  if (t >= end) {
    scorpion_ready(battle, fighter);
  }
}

static void ground_attack(Battle *battle, Fighter *fighter, int index) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(battle, fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script = ground_attack_tick;
  fighter->script_phase = index;
  fighter->script_timer = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_COMBO_HP || index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
}

static void hp_tick(Battle *battle, Fighter *fighter) {
  int phase = fighter->script_phase, t = fighter->ticks;
  if (phase >= 4) {
    fighter->sample_override = mini(phase == 4 ? 20 : 18, 11 + t);
    if (t >= (phase == 4 ? 10 : 8)) {
      scorpion_ready(battle, fighter);
    }
    return;
  }
  int first = phase == 0 ? 0 : phase == 1 ? 1 : phase == 2 ? 11 : 2;
  int active = phase == 0 ? 8 : phase == 1 ? 7 : phase == 2 ? 6 : 6;
  fighter->sample_override = phase == 2 && t == active + 2 ? 17 : first + t;
  const Fighter *victim = fighter_opponent(battle, fighter);
  bool ducking_kick = victim->action == 0x102 && victim->avoids_high;
  if (t >= active && t <= active + 2 && !fighter->connected && !ducking_kick) {
    battle_contact(battle, fighter, &hp_record);
  }
  if (t >= active + 3) {
    bool repeat = (fighter->buffered & BUTTON_BIT(BUTTON_HIGH_PUNCH)) && (victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]) * fighter->facing >= 0;
    int next = repeat ? (phase == 0 ? 1 : phase == 1 || phase == 3 ? 2 : 3) : phase == 0 || phase == 2 ? 4 : 5;
    fighter_start_move(battle, fighter, S_PUNCH, next == 4 ? &s_hp : &s_hp_second);
    fighter->script = hp_tick;
    fighter->script_phase = next;
    fighter->sample_override = next == 1 ? 1 : next == 3 ? 2 : 11;
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

static void scorpion_hp(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(battle, fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(battle, fighter, S_PUNCH, &s_hp);
  fighter->script = hp_tick;
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
}

static void scorpion_lk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_LOW_KICK);
}

static void scorpion_crouch_lp(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_LP);
}

static void scorpion_crouch_lk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_LK);
}

static void scorpion_crouch_hk(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_CROUCH_HK);
}

static void scorpion_combo_hp(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_COMBO_HP);
}

static void scorpion_roundhouse(Battle *battle, Fighter *fighter) {
  ground_attack(battle, fighter, G_ROUNDHOUSE);
}

static void scorpion_hk(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if (fighter_holds_back(battle, fighter)) {
    scorpion_roundhouse(battle, fighter);
  } else if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != 0x4001 && victim->action != 0x4005) {
    ground_attack(battle, fighter, G_CLOSE_HK);
  } else {
    scorpion_kick(battle, fighter);
  }
}

static bool can_ground_throw(const Battle *battle, const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int a = victim->action;
  float distance = fighter_distance(battle, fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != 0x10a && a != 0x10b && a != 0x100 && a != 0x102 && a != 0x10e && !(a >= 0x111 && a <= 0x113);
}

static void press_jab(Battle *battle, Fighter *fighter) {
  FighterFn entry = fighter_holds_back(battle, fighter) ? sword_back_hp : sword_hp;
  fighter_interrupt(fighter, fighter->armed ? entry : scorpion_hp);
}

static void press_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? scorpion_roundhouse : scorpion_hk);
}

static void enter_uppercut(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_UPPERCUT);
}

static void enter_sweep(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_SWEEP);
}

static void enter_throw(Battle *battle, Fighter *fighter);
static void enter_lk_throw(Battle *battle, Fighter *fighter);

static void press_lp(Battle *battle, Fighter *fighter) {
  FighterFn entry = fighter_holds_back(battle, fighter) ? sword_back_lp : sword_lp;
  fighter_interrupt(fighter, can_ground_throw(battle, fighter, false) ? enter_throw : fighter->armed ? entry : scorpion_jab);
}

static void press_lk(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? enter_sweep : can_ground_throw(battle, fighter, true) ? enter_lk_throw : scorpion_lk);
}

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */
typedef struct AirAttack {
  MoveDef move;
  AttackRecord record;
  int extend, hold, retract;
} AirAttack;

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = 0x603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = 0x604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = 0x600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = 0x602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = 0x601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
};
// clang-format on

enum {
  AIR_FLIGHT,
  AIR_COMBO,
  AIR_FREEZE,
  AIR_COMBO_FREEZE,
  AIR_FALL
};

static const ButtonTable air_followup;
static const ButtonTable air_ground_followup;
static const ButtonTable scorpion_standing;

static void air_land(Battle *battle, Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  fighter_start_move(battle, fighter, S_JUMP, &s_teleport_land);
  fighter->script = teleport_land_tick;
  fighter->sample_override = 18;
}

static void air_attack_tick(Battle *battle, Fighter *fighter) {
  int index = 0;
  while (&air_attacks[index].move != fighter_move(fighter)) {
    ++index;
  }
  const AirAttack *attack = &air_attacks[index];
  int tick = fighter->ticks;
  if (fighter->script_phase == AIR_FREEZE || fighter->script_phase == AIR_COMBO_FREEZE) {
    if (--fighter->script_timer > 0) {
      return;
    }
    fighter->buttons = NULL;
    fighter->body.gravity = index >= 3 || fighter->script_phase == AIR_COMBO_FREEZE ? 0x51e : 0x28f;
    fighter->script_phase = AIR_FALL;
  }
  if (!fighter_airborne(fighter)) {
    air_land(battle, fighter);
    return;
  }
  if (fighter->script_phase == AIR_FALL) {
    return;
  }
  int retract_at = attack->extend + attack->hold;
  fighter->sample_override = tick < attack->extend ? tick : tick < retract_at ? attack->extend - 1 : maxi(0, attack->retract - 1 - tick + retract_at);
  if (tick >= attack->extend && tick < retract_at && !fighter->connected && battle_contact(battle, fighter, &attack->record) != CONTACT_MISS) {
    bool low = fighter->body.ground - fighter->body.pos[AXIS_Y] <= 0xcccc;
    bool combo = fighter->script_phase == AIR_COMBO;
    stop_body(fighter);
    fighter->action = 0x1006;
    fighter->script_phase = combo ? AIR_COMBO_FREEZE : AIR_FREEZE;
    fighter->script_timer = index >= 3 ? 8 : 12;
    if (index == 0 || index == 2) {
      if (low) {
        fighter->body.pos[AXIS_Y] = fighter->body.ground;
        if (!combo) {
          fighter->buttons = &air_ground_followup;
          fighter->flags |= FF_FOLLOW_UP;
        }
      } else if (index == 0 && !combo) {
        fighter->buttons = &air_followup;
      }
    }
  } else if (tick >= retract_at + attack->retract) {
    if (fighter->script_phase == AIR_COMBO) {
      fighter->body.gravity = 0x51e;
    }
    fighter->script_phase = AIR_FALL;
  }
}

static void start_air_attack(Battle *battle, Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(battle, fighter, S_JUMP, &air_attacks[index].move);
  fighter->script = air_attack_tick;
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(battle, fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
}

static void air_hp(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 0);
}

static void air_lp(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 1);
}

static void air_kick(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 2);
}

static void air_hk_diagonal(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 3);
}

static void air_lk_diagonal(Battle *battle, Fighter *fighter) {
  start_air_attack(battle, fighter, 4);
}

static void press_air_hp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_hp : air_hp);
}

static void press_air_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_lp : air_lp);
}

static void press_air_hk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

static void press_air_lk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, air_kick);
}

static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static void press_crouch_lp(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, fighter->armed ? sword_spin : scorpion_crouch_lp);
}

static void press_uppercut(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, scorpion_crouch_hk);
}

static void press_crouch_lk(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter_interrupt(fighter, scorpion_crouch_lk);
}

static void press_combo_hp(Battle *battle, Fighter *fighter) {
  if (fighter->armed) {
    press_jab(battle, fighter);
  } else {
    fighter_interrupt(fighter, scorpion_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Battle *battle, Fighter *fighter) {
  if (fighter_distance(battle, fighter) <= 0.8f && !fighter_airborne(fighter_opponent(battle, fighter))) {
    ground_attack(battle, fighter, G_CLOSE_HP);
  } else {
    scorpion_hp(battle, fighter);
  }
}

static void press_air_ground_hp(Battle *battle, Fighter *fighter) {
  if (fighter->armed) {
    press_jab(battle, fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

static const ButtonTable air_ground_followup = {
    {[BUTTON_HIGH_PUNCH] = press_air_ground_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};

static void press_combo_hk(Battle *battle, Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(battle, fighter) ? scorpion_roundhouse : scorpion_kick);
}

static void press_knee_hp(Battle *battle, Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  if ((victim->action == 0x4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, enter_uppercut);
  } else {
    press_jab(battle, fighter);
  }
}

static const ButtonTable close_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_combo_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable combo_hp_followup = {
    {[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable close_hk_followup = {
    {[BUTTON_HIGH_PUNCH] = press_knee_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_combo_hk, [BUTTON_LOW_KICK] = press_lk}};

/* Paired ground throws: 0x4de6e4 (LP), 0x4de2f8/318/328 (LK).
 * Native opponent-root alignment is represented by a shared world origin. */
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = 0x501};
static const MoveDef s_lp_thrown = {.clip = 152, .last = 41, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_lk_thrown_end = {.clip = 586, .first = 65, .last = 130, .flags = SF_LOCK, .action = 0x4003};

static void ground_thrown_tick(Battle *battle, Fighter *victim) {
  Fighter *owner = opponent(battle, victim);
  if (victim->current_move == &s_lk_thrown_end) {
    victim->sample_override = mini(130, 65 + victim->ticks);
    if (victim->ticks >= 66 && victim->health > 0) {
      scorpion_ready(battle, victim);
    }
    return;
  }
  if ((owner->current_move != &s_throw && owner->current_move != &s_lk_throw) || owner->pending != NULL) {
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (victim->health > 0) {
      scorpion_ready(battle, victim);
    } else {
      victim->script = NULL;
    }
    return;
  }
  victim->sample_override = mini(fighter_move(victim)->last, owner->ticks);
  memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
}

static void ground_throw_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  bool lk = fighter->current_move == &s_lk_throw;
  int tick = fighter->ticks, release = lk ? 65 : 42;
  if (tick < release && (victim->script != ground_thrown_tick || victim->pending != NULL)) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    scorpion_ready(battle, fighter);
    return;
  }
  fighter->sample_override = mini(lk ? 65 : 64, tick);
  if (tick == 42) {
    if (victim->armed && victim->disarm != NULL) {
      victim->disarm(battle, victim);
    }
    battle_damage(battle, victim, lk ? 0x2e14 : 0x2666);
  }
  if (tick == release) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    victim->flags &= ~(unsigned)FF_PAIRED;
    if (lk) {
      fighter_start_move(battle, victim, S_KNOCKDOWN, &s_lk_thrown_end);
      victim->script = ground_thrown_tick;
      victim->sample_override = 65;
    } else {
      battle_react(battle, victim, 100);
    }
  }
  if (tick >= (lk ? 66 : 65)) {
    scorpion_ready(battle, fighter);
  }
}

static void ground_throw(Battle *battle, Fighter *fighter, bool lk) {
  if (!can_ground_throw(battle, fighter, lk)) {
    scorpion_ready(battle, fighter);
    return;
  }
  Fighter *victim = opponent(battle, fighter);
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(battle, fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(battle, victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->script = ground_throw_tick;
  victim->script = ground_thrown_tick;
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
}

static void enter_throw(Battle *battle, Fighter *fighter) {
  ground_throw(battle, fighter, false);
}

static void enter_lk_throw(Battle *battle, Fighter *fighter) {
  ground_throw(battle, fighter, true);
}

/* Air throw: scripts 0x4e5e28/0x4e5e50, native 0x46aae0.
 * The victim owns its release/recovery, so interrupting either participant
 * cannot leave a suspended body behind. Shared getup replaces scheduler parking. */
static const MoveDef s_air_throw = {.clip = 82, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = 0x502};
static const MoveDef s_air_thrown = {.clip = 202, .last = 44, .flags = SF_LOCK, .action = 0x4003};
static const MoveDef s_throw_getup = {.clip = 131, .first = 46, .last = 79, .flags = SF_LOCK, .action = 0x200e};

static void thrown_getup_tick(Battle *battle, Fighter *fighter) {
  fighter->sample_override = mini(79, 46 + fighter->ticks);
  if (fighter->ticks >= 34 && fighter->health > 0) {
    scorpion_ready(battle, fighter);
  }
}

static void air_thrown_tick(Battle *battle, Fighter *victim) {
  Fighter *owner = opponent(battle, victim);
  if (victim->script_phase == 0) {
    if (owner->current_move == &s_air_throw && owner->pending == NULL && owner->ticks < 28) {
      memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
      victim->sample_override = mini(27, owner->ticks);
      return;
    }
    victim->flags &= ~(unsigned)FF_PAIRED;
    victim->script_phase = 1;
    victim->body.vel[AXIS_Y] = 0x3333;
    victim->body.gravity = 0x28f;
  }
  if (victim->script_phase == 1 && !fighter_airborne(victim)) {
    victim->script_phase = 2;
    victim->script_timer = 0;
  }
  if (victim->script_phase == 2) {
    victim->sample_override = mini(44, 29 + victim->script_timer++);
    if (victim->script_timer >= 20 && victim->health > 0) {
      fighter_start_move(battle, victim, S_KNOCKDOWN, &s_throw_getup);
      victim->script = thrown_getup_tick;
      victim->sample_override = 46;
    }
  }
}

static bool can_air_throw(const Battle *battle, const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(battle, fighter);
  int64_t distance2 = 0;
  for (int i = 0; i < 3; ++i) {
    int64_t d = (int64_t)victim->body.pos[i] - fighter->body.pos[i];
    distance2 += d * d;
  }
  return battle->phase == PHASE_FIGHT && fighter->body.pos[AXIS_Y] <= -0x23333 && victim->body.pos[AXIS_Y] <= -0x23333 &&
         distance2 <= 2LL * Q16_ONE * Q16_ONE && !(victim->flags & (FF_REACTING | FF_UNTOUCHABLE | FF_PAIRED));
}

static void air_throw_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  int tick = fighter->ticks;
  if (tick < 28 && (victim->current_move != &s_air_thrown || victim->pending != NULL)) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    fighter->body.gravity = 0x28f;
    scorpion_ready(battle, fighter);
    return;
  }
  fighter->sample_override = tick < 29 ? tick : tick < 39 ? 28 : mini(44, tick - 9);
  if (tick == 28) {
    fighter->flags &= ~(unsigned)FF_PAIRED;
    battle_damage(battle, victim, 0x2147);
  }
  if (tick == 29) {
    fighter_set_speed(battle, fighter, 0x7ae);
    fighter->body.vel[AXIS_Y] = -0x1999;
    fighter->body.gravity = 0x28f;
  }
  if (tick >= 54 && !fighter_airborne(fighter)) {
    fighter_start_move(battle, fighter, S_JUMP, &s_teleport_land);
    fighter->script = teleport_land_tick;
    fighter->sample_override = 18;
  }
}

static void scorpion_air_throw(Battle *battle, Fighter *fighter) {
  if (!can_air_throw(battle, fighter)) {
    scorpion_ready(battle, fighter);
    return;
  }
  Fighter *victim = opponent(battle, fighter);
  if (victim->armed && victim->disarm != NULL) {
    victim->disarm(battle, victim);
  }
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(battle, fighter, S_THROW, &s_air_throw);
  fighter_start_move(battle, victim, S_KNOCKDOWN, &s_air_thrown);
  fighter->script = air_throw_tick;
  victim->script = air_thrown_tick;
  fighter->sample_override = victim->sample_override = 0;
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
}

/* Finishers: character entries 0x467cd0/ce0/e00/e70. Presentation runs
 * through bounded C phase tables; no retail task/camera/object scheduler. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = 0x700}
// clang-format off: Keep table entries on one row, including long rows.
static const MoveDef fatal_moves[] = {
  FATAL_MOVE(13, 55), FATAL_MOVE(390, 144), FATAL_MOVE(391, 169), FATAL_MOVE(206, 99),
  FATAL_MOVE(115, 18), FATAL_MOVE(116, 49), FATAL_MOVE(117, 18), FATAL_MOVE(118, 49),
  FATAL_MOVE(12, 57), FATAL_MOVE(578, 48), FATAL_MOVE(579, 34), FATAL_MOVE(580, 77), FATAL_MOVE(581, 37),
  FATAL_MOVE(584, 77), FATAL_MOVE(585, 47), FATAL_MOVE(415, 129), FATAL_MOVE(416, 107), FATAL_MOVE(412, 48),
  FATAL_MOVE(293, 19), FATAL_MOVE(128, 109),
};
// clang-format on
#undef FATAL_MOVE

typedef struct FatalPhase {
  int clip, first, last, repeats;
} FatalPhase;

// clang-format off: Keep table entries on one row, including long rows.
/* 0x4e0278..2f0 and victim 0x4e0300..35c. Blend durations are presentation-owned. */
static const FatalPhase burn_owner[] = {{390, 0, 123, 1}, {390, 105, 123, 2}, {390, 124, 144, 1}, {115, 0, 18, 1}, {116, 0, 49, 1}, {117, 0, 18, 1}, {118, 0, 49, 1}};
static const FatalPhase burn_victim[] = {{391, 0, 169, 1}, {391, 63, 169, 1}, {206, 0, 99, 1}};
/* 0x4d2e58/88/b0, 0x4d2ef8/2f10: five backward/forward eight-frame bites. */
static const FatalPhase beast_owner[] = {{12, 0, 57, 1}, {578, 0, 48, 1}, {579, 0, 34, 1}, {580, 0, 77, 1}, {580, 77, 70, 1}, {580, 70, 77, 1}, {580, 77, 70, 1}, {580, 70, 77, 1}, {580, 77, 70, 1}, {580, 70, 77, 1}, {580, 77, 70, 1}, {580, 70, 77, 1}, {580, 77, 70, 1}, {580, 70, 77, 1}, {581, 0, 37, 1}, {578, 0, 0, 1}};
static const FatalPhase beast_victim[] = {{584, 0, 77, 1}, {584, 77, 70, 1}, {584, 70, 77, 1}, {584, 77, 70, 1}, {584, 70, 77, 1}, {584, 77, 70, 1}, {584, 70, 77, 1}, {584, 77, 70, 1}, {584, 70, 77, 1}, {584, 77, 70, 1}, {584, 70, 77, 1}, {585, 0, 47, 1}};
/* 0x43d3d0 sets 31 repetitions; 0x4e0118..1a8 owns the paired poses. */
static const FatalPhase fan_owner[] = {{415, 0, 68, 1}, {415, 69, 77, 31}, {415, 78, 129, 1}, {412, 0, 48, 1}};
static const FatalPhase fan_victim[] = {{416, 0, 68, 1}, {416, 69, 77, 31}, {416, 78, 107, 1}};
// clang-format on

static void fatality_tick(Battle *battle, Fighter *fighter);

static void fatal_pose(Battle *battle, Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        fighter_start_move(battle, fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        fighter->script = owner ? fatality_tick : NULL;
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool fatal_phases(Battle *battle, Fighter *fighter, int tick, const FatalPhase *phases, int count, bool owner) {
  for (int i = 0; i < count; ++i) {
    const FatalPhase *p = &phases[i];
    int length = abs(p->last - p->first) + 1;
    if (tick < length * p->repeats || i == count - 1) {
      bool done = tick >= length * p->repeats;
      int sample = done ? p->last : p->first + (p->last >= p->first ? 1 : -1) * (tick % length);
      fatal_pose(battle, fighter, p->clip, sample, owner);
      return done;
    }
    tick -= length * p->repeats;
  }
  return true;
}

static bool can_burn_fatality(const Battle *battle, const Fighter *fighter) {
  float distance = fighter_distance(battle, fighter);
  return battle_can_finish(battle, fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_beast_fatality(const Battle *battle, const Fighter *fighter) {
  return battle_can_finish(battle, fighter) && fighter_distance(battle, fighter) <= 1.f;
}

static bool can_fan_fatality(const Battle *battle, const Fighter *fighter) {
  return battle->arena == 8 && can_beast_fatality(battle, fighter);
}

static bool can_spike_fatality(const Battle *battle, const Fighter *fighter) {
  return battle->arena == 0 && can_beast_fatality(battle, fighter);
}

static void fatality_tick(Battle *battle, Fighter *fighter) {
  Fighter *victim = opponent(battle, fighter);
  int tick = battle->phase_ticks;
  bool owner_done = false, victim_done = false;
  if (battle->finisher == FINISH_BURN) {
    if (tick < 8) {
      return;
    }
    owner_done = fatal_phases(battle, fighter, tick - 8, burn_owner, COUNT(burn_owner), true);
    if (tick >= 133) {
      victim_done = fatal_phases(battle, victim, tick - 133, burn_victim, COUNT(burn_victim), false);
      victim->appearance = APPEARANCE_BURNED;
    }
  } else if (battle->finisher == FINISH_BEAST) {
    owner_done = fatal_phases(battle, fighter, tick, beast_owner, COUNT(beast_owner), true);
    fighter->appearance = tick >= 58 ? APPEARANCE_BEAST : APPEARANCE_NORMAL;
    if (tick >= 142) {
      victim_done = fatal_phases(battle, victim, tick - 142, beast_victim, COUNT(beast_victim), false);
    }
  } else if (battle->finisher == FINISH_FAN) {
    owner_done = fatal_phases(battle, fighter, tick, fan_owner, COUNT(fan_owner), true);
    victim_done = fatal_phases(battle, victim, tick, fan_victim, COUNT(fan_victim), false);
    if (tick >= 348) {
      victim->body.pos[AXIS_Y] = victim->body.ground - mini(30, tick - 348) * 0x3333;
    }
    if (victim_done) {
      victim->appearance = APPEARANCE_HIDDEN;
    }
  } else if (battle->finisher == FINISH_SPIKE) {
    /* 30-tick setup + script wait 28, uppercut to sample 5, reaction 175.
     * Stop at Y=-5.6, then the victim script waits 128 + 50 + 60 ticks. */
    int attack = tick - 58;
    if (attack >= 0) {
      fatal_pose(battle, fighter, 293, mini(19, attack), true);
    }
    if (tick >= 64) {
      int rise = mini(22, tick - 64);
      fatal_pose(battle, victim, 128, mini(31, rise * 2), false);
      victim->body.pos[AXIS_Y] = rise == 22 ? -0x59999 : victim->body.ground - rise * 0x3333;
    }
    if (tick >= 324) {
      fatal_pose(battle, fighter, 115, 18, true);
      owner_done = victim_done = true;
    }
  }
  if (owner_done && victim_done) {
    battle_end_finisher(battle);
  }
}

static void start_fatality(Battle *battle, Fighter *fighter, int id) {
  if (!battle_begin_finisher(battle, fighter, id)) {
    scorpion_ready(battle, fighter);
    return;
  }
  Fighter *victim = opponent(battle, fighter);
  fatal_pose(battle, victim, 13, 0, false);
  fatal_pose(battle, fighter, id == FINISH_BEAST ? 12 : id == FINISH_FAN ? 415 : id == FINISH_SPIKE ? 293 : 390, 0, true);
  fighter->script = fatality_tick;
}

static void scorpion_burn_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_BURN);
}

static void scorpion_beast_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_BEAST);
}

static void scorpion_fan_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_FAN);
}

static void scorpion_spike_fatality(Battle *battle, Fighter *fighter) {
  start_fatality(battle, fighter, FINISH_SPIKE);
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_BLOCK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 4, 50, CMD_BLOCK_OK, can_burn_fatality, scorpion_burn_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0, can_beast_fatality, scorpion_beast_fatality},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, can_fan_fatality, scorpion_fan_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD}, 3, 40, 0, can_spike_fatality, scorpion_spike_fatality},
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, can_air_throw, scorpion_air_throw},
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 24, CMD_AIR, can_teleport, scorpion_teleport},
  {BUTTON_LOW_PUNCH, {TOKEN_BACK, TOKEN_BACK}, 2, 27, 0, can_spear, scorpion_spear},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 20, 0, fighting, scorpion_fire},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, can_weapon, scorpion_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, can_pickup, scorpion_pickup},
};
static const ButtonTable scorpion_standing = {{[BUTTON_HIGH_PUNCH] = press_jab, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};
static const ButtonTable scorpion_crouching = {{[BUTTON_HIGH_PUNCH] = press_uppercut, [BUTTON_LOW_PUNCH] = press_crouch_lp, [BUTTON_HIGH_KICK] = press_crouch_hk, [BUTTON_LOW_KICK] = press_crouch_lk}};
static const ButtonTable scorpion_jumping = {{[BUTTON_HIGH_PUNCH] = press_air_hp, [BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_hk, [BUTTON_LOW_KICK] = press_air_lk}};
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

static void sword_attack(Battle *battle, Fighter *fighter, bool hp, bool back) {
  bool air = fighter_airborne(fighter);
  const MoveDef *move = air ? (hp ? &s_sword_air_hp : &s_sword_air_lp) : back ? (hp ? &s_sword_back_hp : &s_sword_back_lp) : (hp ? &s_sword_hp : &s_sword_lp);
  fighter_start_move(battle, fighter, air ? S_JUMP : S_PUNCH, move);
  fighter->script = air ? sword_air_tick : sword_slash_tick;
  if (air) {
    fighter->body.on_land = NULL;
  }
  fighter->sample_override = 0;
}

static void sword_hp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, true, false);
}

static void sword_lp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, false, false);
}

static void sword_back_hp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, true, true);
}

static void sword_back_lp(Battle *battle, Fighter *fighter) {
  sword_attack(battle, fighter, false, true);
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
