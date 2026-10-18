#include "game.h"
#include "native.h"
#include "mk4_characters.h"
#include "math_util.h"
#include <stdlib.h>
#include <string.h>

/* Native controller return used by generated VM programs and native runners. */
static void scorpion_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

static void script_spear_done(Fighter *fighter);
static void scorpion_air_throw(Fighter *fighter);
static void fan_start(Fighter *fighter);
static void spike_start(Fighter *fighter);
#include "generated/scorpion.inc"
#include "generated/frames_scorpion.inc"

/* Retail attack records (docs/scorpion/combat-core.md section 2). */
// clang-format off: Keep table entries on one row, including long rows.
static const AttackRecord uppercut_record = {0xe3d7, -0xfd70, 0x947a, 0x10518, 34, 34, 6, 0, 0x28f5};     /* 0x4ebd50, A selector 2 */
static const AttackRecord uppercut_alt_record = {0x12106, -0x111a9, 0xfa9f, 0x17a9f, 34, 34, 6, 0, 0xa3d}; /* 0x4ebd68, A selector 6 */
static const AttackRecord kick_record = {0x15e76, -0xd999, 0x151a9, 0x111eb, 16, 13, 6, 0, 0x2147};       /* 0x4ebae0, B selector 1 */
static const AttackRecord sweep_record = {0x175c2, 0x7a9f, 0x10ccc, 0x8a3d, 18, 18, 4, ATTACK_LOW, 0x23d7}; /* 0x4ebb58, B selector 35 */
// clang-format on

static int victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
}

/* Runner 0x470c60 with block 0x4e5d30: the first contact latches. */
static void scorpion_kick_contact(Fighter *fighter) {
  if (!fighter->connected && victim_action(fighter) != ACTION_RETAIL_2001) {
    battle_contact(fighter, &kick_record);
  }
}

/* 0x46f080: no gate; queries each visit until a contact. */
static void scorpion_sweep_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, &sweep_record);
  }
}

/* 0x46cde0: selector 6 inside a follow-up window; contact sets action 0x1006. */
static void scorpion_uppercut_contact(Fighter *fighter) {
  if (fighter->connected || victim_action(fighter) == ACTION_RETAIL_2001) {
    return;
  }
  if (battle_contact(fighter, (fighter->flags & FF_FOLLOW_UP) ? &uppercut_alt_record : &uppercut_record) != CONTACT_MISS) {
    fighter->action = ACTION_RETAIL_1006;
  }
}

#define COUNT(array) ((int)(sizeof array / sizeof array[0]))
#define SEQUENCE(segments, count, end) {segments, COUNT(segments), NULL, 0, count, end, NULL}

/* Recovered moves query retail records from their events (SF_RECORD). The
 * remaining moves keep the approximate damage, reach and push columns. */
static const MoveDef s_idle = {CLIP_ASSET_IDLE, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_walk = {269, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
static const MoveDef s_crouch = {259, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_HOLD, NULL, 0};
static const MoveDef s_jump = {304, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, 0, NULL, 0};
#define RETAIL SF_ATTACK | SF_LOCK | SF_RECORD
static const MoveDef s_jab = {.clip = 294, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010B};
static const MoveDef s_kick = {.clip = 66, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0100};
static const MoveDef s_sweep = {.clip = 63, .last = MOVE_END, .flags = RETAIL | SF_LOW, .action = ACTION_RETAIL_010C};
static const MoveDef s_uppercut = {.clip = 293, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0109};
/* The jab graph alternates an opening/recovery with a second clip. Contact
 * requests at phase boundaries execute before the next branch is committed. */

static const MoveDef s_throw = {.clip = 75, .last = 64, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0500};
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

/* Phase branches are ordinary C; blocking playback owns only samples/events. */

static void uppercut_script(Fighter *fighter) {
  native_uppercut_tail(fighter, scorpion_uppercut_contact, scorpion_ready);
}

static void scorpion_uppercut(Fighter *fighter) {
  fighter_start_move(fighter, S_UPPERCUT, &s_uppercut);
  fighter_start_fiber(fighter, retail_script_004e62a0, NULL);
}

static void scorpion_sweep(Fighter *fighter) {
  fighter_start_move(fighter, S_SWEEP, &s_sweep);
  fighter_start_fiber(fighter, retail_script_004e6798, NULL);
}

static void scorpion_jab(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_jab);
  /* The jab entry clears the busy bit (0x490330): commands stay live. */
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, retail_script_004e62f0, NULL);
}

static void scorpion_kick(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &s_kick);
  fighter_start_fiber(fighter, retail_script_004e5ca0, NULL);
}

static void scorpion_idle(Fighter *fighter);
static void scorpion_walk(Fighter *fighter);
static void scorpion_crouch(Fighter *fighter);
static void scorpion_block(Fighter *fighter);
static const FighterFn enter[S_COUNT] = {
    [S_IDLE] = scorpion_idle,
    [S_WALK] = scorpion_walk,
    [S_CROUCH] = scorpion_crouch,
    [S_BLOCK] = scorpion_block,
    [S_PUNCH] = scorpion_jab,
    [S_KICK] = scorpion_kick,
    [S_SWEEP] = scorpion_sweep,
    [S_UPPERCUT] = scorpion_uppercut,
};

/* Specials translated from spear_entry/return, teleport_entry/landing and
 * breathe_fire in build/retail-disassembly/scripts. Native loops are ordinary
 * C continuations. The camera-dependent teleport exit uses a fixed ten-tick
 * vanish in our engine; it retains the distance/wall exits and contact branches. */
static const AttackRecord spear_record = {0x3333, -0x1eb8, 0x947a, 0x3ae1, 70, 70, 112, ATTACK_MARK, 0xccc};
static const AttackRecord teleport_record = {0x128f5, -0x3ae1, 0xab85, 0x91eb, 90, 90, 0, ATTACK_MARK, 0x170a};
static const AttackRecord fire_record = {0x24e56, -0xc978, 0x23cac, 0xb7ce, 173, 173, 0, ATTACK_UNBLOCKABLE, 0x1eb8};
static const MoveDef s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0300};
static const MoveDef s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4000};
static const MoveDef s_spear_stun = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1008};
static const MoveDef s_teleport = {.clip = 312, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0209};
static const MoveDef s_teleport_block = {.clip = 301, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1011};
static const MoveDef s_teleport_land = {.clip = 304, .first = 18, .last = 27, .flags = SF_LOCK, .action = ACTION_RETAIL_2008};
static const MoveDef s_fire = {.clip = 448, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0115};

static Fighter *opponent(Fighter *fighter) {
  return &g_battle.fighters[fighter == &g_battle.fighters[0] ? 1 : 0];
}

static void stop_body(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = NULL;
  fighter->body.on_land = NULL;
}

static void special_ready(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  scorpion_ready(fighter);
}

static void spear_stun_script(Fighter *fighter) {
  (void)fighter;
  /* 487420: independent 0x40-tick follow-up opening, cancelled by a hit. */
  fighter_wait_ticks(64);
  fighter_fiber_finish(special_ready);
}

static bool spear_not_pulling(const Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    const Projectile *p = &g_battle.projectiles[i];
    if (p->kind == PROJECTILE_SPEAR && &g_battle.fighters[p->owner] == fighter && p->phase == 1) {
      return false;
    }
  }
  return true;
}

/* Runs on the scheduler stack, after the old fiber has been retired. Pull
 * arrival installs stun before replacing the owner's move, so that victim
 * is already released from s_snared and must retain its follow-up opening. */
static void spear_cleanup(Fighter *fighter) {
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &g_battle.projectiles[i];
    if (p->kind != PROJECTILE_SPEAR || &g_battle.fighters[p->owner] != fighter) {
      continue;
    }
    bool pulling = p->phase == 1;
    p->kind = 0;
    Fighter *victim = opponent(fighter);
    if (pulling && fighter_move(victim)->clip == 199) {
      special_ready(victim);
    }
  }
}

static void spear_projectile(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner], *victim = opponent(owner);
  bool interrupted = fighter_move(owner)->clip != 14 || !fighter_fiber_active(owner) || owner->pending != NULL;
  if (interrupted || g_battle.contacts_off) {
    if (p->phase == 1 && fighter_move(victim)->clip == 199) {
      special_ready(victim);
    }
    p->kind = 0;
    return;
  }
  if (p->phase == 1) {
    if (fighter_move(victim)->clip != 199 || victim->health <= 0) {
      p->kind = 0;
      return;
    }
    q16 dx = owner->body.pos[AXIS_X] - victim->body.pos[AXIS_X];
    if (abs(dx) <= 0xcccc) {
      stop_body(victim);
      fighter_start_move(victim, S_HIT, &s_spear_stun);
      victim->sample_override = 0;
      victim->flags |= FF_REACTING;
      fighter_start_fiber(victim, retail_script_004e9f20, NULL);
      fighter_start_move(owner, S_PUNCH, &s_spear);
      owner->sample_override = 31;
      fighter_start_fiber(owner, retail_script_004ed4a8, spear_cleanup);
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
  ContactResult result = battle_contact_from(owner, &spear_record, p->pos, p->facing);
  if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    stop_body(victim);
    fighter_start_move(victim, S_HIT, &s_snared);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    fighter_start_fiber(victim, retail_script_004e9d00, NULL);
    victim->damage_scale = 0x9999; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || abs(p->pos[AXIS_X]) > g_battle.arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

/* This helper suspends inside an ordinary C loop. Its call frame, and the
 * caller's fast-start local, survive each nested one-tick wait. */

static void spear_spawn(Fighter *fighter) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SPEAR, 90, spear_projectile);
  if (p != NULL) {
    p->pos[AXIS_X] += fighter->facing * 0xc000;
    p->pos[AXIS_Y] = -0x16666;
    p->vel[AXIS_X] = fighter->facing * 0x3333;
  }
}

/* 4ed448/4ed47c -> 4ed470/4ed498. Normal/fast launch is at tick 10/8;
 * sample 30 follows at 15/11. Keep the existing paired-pull policy: arrival
 * replaces this fiber with spear_return_script, while cancellation cleans up. */

static bool can_spear(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->spear_ready && victim->hits <= 3 && victim->action != ACTION_RETAIL_1008;
}

static void scorpion_spear(Fighter *fighter) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_spear);
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = g_battle.ticks + 90;
  fighter_start_fiber(fighter, retail_script_004ed448, spear_cleanup);
}

static void teleport_land_script(Fighter *fighter) {
  native_begin(fighter, 1, 10, special_ready);
  for (int frame = 1; frame <= 10; ++frame) {
    native_frame(fighter, frame, mini(27, 18 + frame), NULL);
  }
  native_play(fighter, false);
}

static void teleport_drop(Fighter *fighter);
static void teleport_pause(Fighter *fighter);

static void teleport_land_gate(Fighter *fighter) {
  fighter->mmove_hold = fighter_airborne(fighter);
}

static void teleport_drop(Fighter *fighter) {
  fighter->body.gravity = 0x1999;
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, special_ready);
    return;
  }
  native_begin(fighter, 0, 0, special_ready);
  native_frame(fighter, 0, fighter_sample(fighter), teleport_land_gate);
  native_play(fighter, false);
}

static void teleport_pause(Fighter *fighter) {
  stop_body(fighter);
  fighter->flags &= ~(unsigned)FF_TELEPORTING;
  fighter->script_phase = 2;
  native_hold(fighter, 6, teleport_drop);
}

static void teleport_block_return(Fighter *fighter) {
  fighter_start_fiber(fighter, retail_script_004e8818, NULL);
}

static void teleport_query(Fighter *fighter) {
  if (!fighter_airborne(fighter)) {
    fighter_finish_mmove(fighter, special_ready);
    return;
  }
  ContactResult result = battle_contact(fighter, &teleport_record);
  if (result == CONTACT_MISS) {
    return;
  }
  if (result == CONTACT_BLOCK) {
    fighter_start_move(fighter, S_JUMP, &s_teleport_block);
    fighter->sample_override = 8;
    fighter->body.vel[AXIS_X] = -fighter->facing * 0x28f;
    fighter->body.vel[AXIS_Y] = -0x2147;
    fighter->body.gravity = 0x1ca;
    native_hold(fighter, 21, teleport_block_return);
  } else {
    teleport_pause(fighter);
  }
}

static void teleport_vanish(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  int first = fighter->mmove_frame + 1;
  q16 distance = abs(victim->body.pos[AXIS_X] - fighter->body.pos[AXIS_X]);
  int direction = victim->body.pos[AXIS_X] >= fighter->body.pos[AXIS_X] ? 1 : -1;
  q16 offset = 0x26666 + maxi(0, 4 * Q16_ONE - distance);
  q16 x = victim->body.pos[AXIS_X] + direction * offset;
  fighter->body.pos[AXIS_X] = abs(x) > g_battle.arena_radius ? victim->body.pos[AXIS_X] : x;
  fighter->facing = -direction;
  fighter->body.vel[AXIS_X] = -direction * 0x2666;
  fighter->sample_override = 9;
  fighter->script_phase = 1;
  native_begin(fighter, first, maxi(first, 70), teleport_pause);
  for (int frame = first; frame <= maxi(first, 70); ++frame) {
    native_frame(fighter, frame, 9, teleport_query);
  }
  native_play(fighter, false);
}

static void teleport_range(Fighter *fighter) {
  script_teleport_motion(fighter);
  if (fighter->native_signal) {
    teleport_vanish(fighter);
  }
}

static void teleport_script(Fighter *fighter) {
  int first = fighter->ticks + 1;
  native_begin(fighter, first, maxi(first, 10), teleport_vanish);
  for (int frame = first; frame <= maxi(first, 10); ++frame) {
    native_frame(fighter, frame, mini(9, frame), teleport_range);
  }
  native_play(fighter, false);
}

static bool can_teleport(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->teleport_ready && fighter_opponent(fighter)->hits < 3;
}

static void scorpion_teleport(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_teleport);
  fighter->sample_override = 0;
  fighter->teleport_ready = g_battle.ticks + 69;
  fighter->native_signal = false;
  fighter->flags |= FF_TELEPORTING;
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? -1 : 1;
  fighter_start_fiber(fighter, retail_script_004e87b0, NULL);
}

static void fire_contact(Fighter *fighter) {
  battle_contact(fighter, &fire_record);
}

static void fire_script(Fighter *fighter) {
  bool fast = fighter->script_phase != 0;
  int first = fighter->ticks + 1, last = fast ? 39 : 54;
  native_begin(fighter, first, last, special_ready);
  for (int frame = first; frame <= last; ++frame) {
    int pose = fast ? mini(34, frame < 19 ? frame - 2 : frame < 32 ? frame - 3 : frame - 4) : mini(34, frame < 48 ? 11 + (frame - 13) / 2 : frame - 19);
    native_frame(fighter, frame, pose, frame == (fast ? 19 : 27) ? fire_contact : NULL);
  }
  native_play(fighter, false);
}

static void scorpion_fire(Fighter *fighter) {
  stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_fire);
  fighter->sample_override = 1;
  fighter->script_phase = (fighter_opponent(fighter)->flags & FF_REACTING) != 0;
  fighter_start_fiber(fighter, retail_script_004e5b58, NULL);
}

static bool fighting(const Fighter *fighter) {
  (void)fighter;
  return g_battle.phase == PHASE_FIGHT;
}

static void sword_hp(Fighter *fighter);
static void sword_lp(Fighter *fighter);
static void sword_back_hp(Fighter *fighter);
static void sword_back_lp(Fighter *fighter);
static void sword_spin(Fighter *fighter);
static void scorpion_weapon(Fighter *fighter);

static bool can_weapon(const Fighter *fighter);
static bool can_pickup(const Fighter *fighter);
static void scorpion_pickup(Fighter *fighter);

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

// clang-format off: Keep table entries on one row, including long rows.
static const GroundAttack ground_attacks[] = {
  {{.clip = 67, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0102}, {94371, -13762, 72024, 23592, 17, 10, 6, 0, 7208}, 10, 33, 36},
  {{.clip = 260, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0107}, {65536, -3932, 42532, 20971, 29, 10, 4, 0, 2621}, 8, 19, 22},
  {{.clip = 315, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0106}, {81264, 34734, 67502, 32768, 28, 10, 4, 0, 3932}, 9, 19, 22},
  {{.clip = 54, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0104}, {48496, -24903, 44564, 30146, 25, 10, 5, 0, 7864}, 9, 29, 29},
  {{.clip = 262, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 7, 21, 27},
  {{.clip = 261, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0108}, {65536, -37945, 57016, 51707, 43, 10, 3, 0, 2621}, 9, 26, 29},
  {{.clip = 314, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0103}, {60293, -11796, 43909, 72024, 27, 10, 3, 0, 6553}, 4, 25, 28},
  {{.clip = 313, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010D}, {69468, -43909, 61603, 27525, 20, 20, 5, 0, 11796}, 16, 33, 61},
};
// clang-format on
static const ButtonTable close_hp_followup, combo_hp_followup, close_hk_followup;
static const AttackRecord hp_record = {51118, -45809, 38666, 28180, 32, 11, 1, 0, 3276};
static const MoveDef s_hp = {.clip = 296, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};
static const MoveDef s_hp_second = {.clip = 297, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_010A};

static const NativeGroundProfile ground_profile = {
    ground_attacks, {&close_hp_followup, &combo_hp_followup, &close_hk_followup}, scorpion_ready, 5, 0, 28, 21, 22, 38, NULL};

static void ground_native_tail(Fighter *fighter) {
  native_ground_tail(fighter, &ground_profile);
}

static void ground_attack(Fighter *fighter, int index) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, index == G_CLOSE_HP || index == G_COMBO_HP || index == G_CROUCH_LP ? S_PUNCH : S_KICK, &ground_attacks[index].move);
  fighter->script_phase = index;
  fighter->native_followup = followup;
  fighter->sample_override = index == G_CLOSE_HK ? 5 : index == G_COMBO_HP || index == G_CROUCH_LP ? 2 : index == G_CROUCH_LK ? 1 : 0;
  static const FighterFn scripts[] = {retail_script_004e6d08, ground_native_tail, retail_script_004e6158, retail_script_004e60b8, retail_script_004e61e0,
      retail_script_004e69b0, retail_script_004e6190, retail_script_004e6844};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static const NativeHPProfile hp_profile = {&hp_record, &s_hp, &s_hp_second, scorpion_ready};

static void hp_script(Fighter *fighter) {
  native_hp_start(fighter, &hp_profile);
}

static void scorpion_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && !(victim->flags & FF_REACTING)) {
    ground_attack(fighter, G_CLOSE_HP);
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_hp);
  fighter->sample_override = 0;
  fighter->flags &= ~(unsigned)FF_BUSY;
  fighter_start_fiber(fighter, hp_script, NULL);
}

static void scorpion_lk(Fighter *fighter) {
  ground_attack(fighter, G_LOW_KICK);
}

static void scorpion_crouch_lp(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LP);
}

static void scorpion_crouch_lk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_LK);
}

static void scorpion_crouch_hk(Fighter *fighter) {
  ground_attack(fighter, G_CROUCH_HK);
}

static void scorpion_combo_hp(Fighter *fighter) {
  ground_attack(fighter, G_COMBO_HP);
}

static void scorpion_roundhouse(Fighter *fighter) {
  ground_attack(fighter, G_ROUNDHOUSE);
}

static void scorpion_hk(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if (fighter_holds_back(fighter)) {
    scorpion_roundhouse(fighter);
  } else if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(victim) && victim->action != ACTION_RETAIL_4001 && victim->action != ACTION_RETAIL_4005) {
    ground_attack(fighter, G_CLOSE_HK);
  } else {
    scorpion_kick(fighter);
  }
}

static bool can_ground_throw(const Fighter *fighter, bool low_kick) {
  const Fighter *victim = fighter_opponent(fighter);
  int a = victim->action;
  float distance = fighter_distance(fighter);
  return (low_kick ? distance < 0.7f : distance <= 0.7f) && !(fighter->flags & FF_FOLLOW_UP) && !fighter_airborne(victim) && !(victim->flags & FF_REACTING) &&
         a != ACTION_RETAIL_010A && a != ACTION_RETAIL_010B && a != ACTION_RETAIL_0100 && a != ACTION_RETAIL_0102 && a != ACTION_RETAIL_010E &&
         !(a >= ACTION_RETAIL_0111 && a <= ACTION_RETAIL_0113);
}

static void press_jab(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_hp : fighter->weapon->hp) : scorpion_hp;
  fighter_interrupt(fighter, fighter->armed ? entry : scorpion_hp);
}

static void press_kick(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_hk);
}

static void enter_uppercut(Fighter *fighter) {
  fighter_enter_state(fighter, S_UPPERCUT);
}

static void enter_sweep(Fighter *fighter) {
  fighter_enter_state(fighter, S_SWEEP);
}

static void enter_throw(Fighter *fighter);
static void enter_lk_throw(Fighter *fighter);

static void press_lp(Fighter *fighter) {
  FighterFn entry = fighter->armed ? (fighter_holds_back(fighter) ? fighter->weapon->back_lp : fighter->weapon->lp) : scorpion_jab;
  fighter_interrupt(fighter, can_ground_throw(fighter, false) ? enter_throw : fighter->armed ? entry : scorpion_jab);
}

static void press_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? enter_sweep : can_ground_throw(fighter, true) ? enter_lk_throw : scorpion_lk);
}

/* Air scripts 0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500 and 0x4e63d8.
 * Each extends, queries while holding its strike pose, then retracts. The
 * native contact continuations freeze the body and optionally open a cancel. */

// clang-format off: Keep table entries on one row, including long rows.
static const AirAttack air_attacks[] = {
  {{.clip = 97, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0603}, {67502, -39976, 56360, 43253, 38, 10, 143, 0, 5898}, 6, 10, 9},
  {{.clip = 96, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0604}, {76677, -20316, 68157, 62914, 39, 39, 4, 0, 7864}, 10, 10, 11},
  {{.clip = 327, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0600}, {67502, -18350, 58982, 43253, 21, 10, 8, 0, 8519}, 8, 4, 8},
  {{.clip = 55, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0602}, {74055, -7864, 60293, 24903, 23, 23, 4, ATTACK_MARK, 6553}, 6, 10, 6},
  {{.clip = 58, .last = MOVE_END, .flags = RETAIL, .action = ACTION_RETAIL_0601}, {75366, -2621, 67502, 34078, 22, 22, 4, ATTACK_MARK, 9830}, 10, 10, 10},
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

static void air_land(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_FOLLOW_UP;
  fighter_start_move(fighter, S_JUMP, &s_teleport_land);
  fighter->sample_override = 18;
  teleport_land_script(fighter);
}

static const NativeAirProfile air_profile = {air_attacks, &air_followup, &air_ground_followup, air_land};

static void air_native_tail(Fighter *fighter, int index, int elapsed) {
  native_air_tail(fighter, &air_profile, index, elapsed);
}

static void start_air_attack(Fighter *fighter, int index) {
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  fighter_start_move(fighter, S_JUMP, &air_attacks[index].move);
  if (combo) {
    fighter->body.gravity = 0;
  }
  fighter_upright(fighter);
  fighter->sample_override = 0;
  fighter->script_phase = combo ? AIR_COMBO : AIR_FLIGHT;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter->facing = fighter->body.pos[AXIS_X] <= opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->flags &= ~(unsigned)FF_BUSY; /* 0x46ea40 -> 0x490330 */
  static const FighterFn scripts[] = {retail_script_004e6658, retail_script_004e6710, retail_script_004e61f8, retail_script_004e6500, retail_script_004e63d8};
  fighter_start_fiber(fighter, scripts[index], NULL);
}

static void air_hp(Fighter *fighter) {
  start_air_attack(fighter, 0);
}

static void air_lp(Fighter *fighter) {
  start_air_attack(fighter, 1);
}

static void air_kick(Fighter *fighter) {
  start_air_attack(fighter, 2);
}

static void air_hk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 3);
}

static void air_lk_diagonal(Fighter *fighter) {
  start_air_attack(fighter, 4);
}

static void press_air_hp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->hp : air_hp);
}

static void press_air_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed ? fighter->weapon->lp : air_lp);
}

static void press_air_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_hk_diagonal : air_kick);
}

static void press_air_lk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->body.vel[AXIS_X] || fighter->body.vel[AXIS_Z] ? air_lk_diagonal : air_kick);
}

static void press_air_followup_kick(Fighter *fighter) {
  fighter_interrupt(fighter, air_kick);
}

static const ButtonTable air_followup = {
    {[BUTTON_LOW_PUNCH] = press_air_lp, [BUTTON_HIGH_KICK] = press_air_followup_kick, [BUTTON_LOW_KICK] = press_air_followup_kick}};

static void press_crouch_lp(Fighter *fighter) {
  fighter_interrupt(fighter, fighter->armed && fighter->weapon->crouch_lp != NULL ? fighter->weapon->crouch_lp : scorpion_crouch_lp);
}

static void press_uppercut(Fighter *fighter) {
  fighter_interrupt(fighter, enter_uppercut);
}

static void press_crouch_hk(Fighter *fighter) {
  fighter_interrupt(fighter, scorpion_crouch_hk);
}

static void press_crouch_lk(Fighter *fighter) {
  fighter_interrupt(fighter, scorpion_crouch_lk);
}

static void press_combo_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, scorpion_combo_hp);
  }
}

/* 0x46c3f0 permits close HP against a reacting, grounded victim after a
 * low air contact. Ordinary standing HP uses a stricter entry predicate. */
static void air_ground_hp(Fighter *fighter) {
  if (fighter_distance(fighter) <= 0.8f && !fighter_airborne(fighter_opponent(fighter))) {
    ground_attack(fighter, G_CLOSE_HP);
  } else {
    scorpion_hp(fighter);
  }
}

static void press_air_ground_hp(Fighter *fighter) {
  if (fighter->armed) {
    press_jab(fighter);
  } else {
    fighter_interrupt(fighter, air_ground_hp);
  }
}

static const ButtonTable air_ground_followup = {
    {[BUTTON_HIGH_PUNCH] = press_air_ground_hp, [BUTTON_LOW_PUNCH] = press_lp, [BUTTON_HIGH_KICK] = press_kick, [BUTTON_LOW_KICK] = press_lk}};

static void press_combo_hk(Fighter *fighter) {
  fighter_interrupt(fighter, fighter_holds_back(fighter) ? scorpion_roundhouse : scorpion_kick);
}

static void press_knee_hp(Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  if ((victim->action == ACTION_RETAIL_4004 || victim->hits > 1) && fighter_height(fighter) <= 0.5f && (fighter->held & BUTTON_BIT(BUTTON_DOWN))) {
    fighter_interrupt(fighter, enter_uppercut);
  } else {
    press_jab(fighter);
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
static const MoveDef s_lk_throw = {.clip = 37, .last = 65, .flags = SF_LOCK | SF_RECORD | SF_UNBLOCKABLE, .action = ACTION_RETAIL_0501};
static const MoveDef s_lp_thrown = {.clip = 152, .last = 41, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown = {.clip = 157, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};
static const MoveDef s_lk_thrown_end = {.clip = 586, .first = 65, .last = 130, .flags = SF_LOCK, .action = ACTION_RETAIL_4003};

static void thrown_end_script(Fighter *victim) {
  fighter_fiber_play_to(victim, 130, 1);
  fighter_wait_ticks(1);
  fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
}

static void ground_thrown_script(Fighter *victim) {
  Fighter *owner = opponent(victim);
  for (;;) {
    fighter_wait_ticks(1);
    if ((owner->action != ACTION_RETAIL_0500 && owner->action != ACTION_RETAIL_0501) || owner->pending != NULL) {
      victim->flags &= ~(unsigned)FF_PAIRED;
      fighter_fiber_finish(victim->health > 0 ? scorpion_ready : NULL);
    }
    memcpy(victim->body.pos, owner->body.pos, sizeof victim->body.pos);
    victim->body.pos[AXIS_Y] = victim->body.ground;
  }
}

static void script_lk_rebase(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  memcpy(victim->body.pos, fighter->body.pos, sizeof victim->body.pos);
  victim->body.pos[AXIS_Y] = victim->body.ground;
  victim->facing = fighter->facing;
  memcpy(victim->heading, fighter->heading, sizeof victim->heading);
  retail_script_004de318(fighter);
}

static void script_lk_damage(Fighter *fighter) {
  retail_script_004de328(fighter);
}

static void script_lk_return(Fighter *fighter) {
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter_fiber_finish(scorpion_ready);
}

static void script_lp_return(Fighter *fighter) {
  fighter_fiber_finish(scorpion_ready);
  (void)fighter;
}

static void script_opponent_reaction(Fighter *fighter, int reaction) {
  require(reaction == 77, "Unexpected Scorpion pair release");
  Fighter *victim = opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lk_thrown_end);
  victim->sample_override = 65;
  fighter_start_fiber(victim, thrown_end_script, NULL);
}

static void ground_throw(Fighter *fighter, bool lk) {
  if (!can_ground_throw(fighter, lk)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  stop_body(fighter);
  stop_body(victim);
  fighter_start_move(fighter, S_THROW, lk ? &s_lk_throw : &s_throw);
  fighter_start_move(victim, S_KNOCKDOWN, lk ? &s_lk_thrown : &s_lp_thrown);
  fighter->flags |= FF_PAIRED;
  victim->flags |= FF_PAIRED | FF_REACTING;
  fighter->sample_override = victim->sample_override = 0;
  fighter_start_fiber(fighter, lk ? retail_script_004de2f8 : retail_script_004de798, NULL);
  fighter_start_fiber(victim, ground_thrown_script, NULL);
}

static void enter_throw(Fighter *fighter) {
  ground_throw(fighter, false);
}

static void enter_lk_throw(Fighter *fighter) {
  ground_throw(fighter, true);
}

/* Generated VM finishers use shared scene, blend and object helpers. */
#define FATAL_MOVE(c, last_sample) {.clip = c, .last = last_sample, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}
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

static void fatal_pose(Fighter *fighter, int clip, int sample, bool owner) {
  if (fighter_move(fighter)->clip != clip) {
    for (int i = 0; i < COUNT(fatal_moves); ++i) {
      if (fatal_moves[i].clip == clip) {
        if (owner && fighter_fiber_running(fighter)) {
          fighter_fiber_move(fighter, S_VICTORY, &fatal_moves[i]);
        } else {
          fighter_start_move(fighter, owner ? S_VICTORY : S_KNOCKDOWN, &fatal_moves[i]);
        }
        break;
      }
    }
  }
  fighter->sample_override = sample;
  fighter->flags |= FF_UNTOUCHABLE | FF_BUSY;
}

static bool can_burn_fatality(const Fighter *fighter) {
  float distance = fighter_distance(fighter);
  return battle_can_finish(fighter) && distance >= q16_float(0x1cccc) && distance <= 3.f;
}

static bool can_beast_fatality(const Fighter *fighter) {
  return battle_can_finish(fighter) && fighter_distance(fighter) <= 1.f;
}

static bool can_fan_fatality(const Fighter *fighter) {
  return g_battle.arena == 8 && can_beast_fatality(fighter);
}

static bool can_spike_fatality(const Fighter *fighter) {
  return g_battle.arena == 0 && can_beast_fatality(fighter);
}

static void finish_scene(Fighter *fighter) {
  (void)fighter;
  battle_end_finisher();
}

static void burn_start(Fighter *fighter) {
  while (g_battle.phase_ticks < 8) {
    fighter_wait_ticks(1);
  }
  retail_script_004e0278(fighter);
}

static void beast_start(Fighter *fighter) {
  retail_script_004d2e58(fighter);
}

static void start_fatality(Fighter *fighter, int id) {
  if (!battle_begin_finisher(fighter, id)) {
    scorpion_ready(fighter);
    return;
  }
  Fighter *victim = opponent(fighter);
  fatal_pose(victim, 13, 0, false);
  fatal_pose(fighter, id == FINISH_BEAST ? 12 : id == FINISH_FAN ? 415 : id == FINISH_SPIKE ? 293 : 390, 0, true);
  if (id == FINISH_FAN) {
    fatal_pose(victim, 416, 0, false);
  }
  fighter_start_fiber(fighter, id == FINISH_BURN ? burn_start : id == FINISH_BEAST ? beast_start : id == FINISH_FAN ? fan_start : spike_start, NULL);
}

static void scorpion_burn_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_BURN);
}

static void scorpion_beast_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_BEAST);
}

static void scorpion_fan_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_FAN);
}

static void scorpion_spike_fatality(Fighter *fighter) {
  start_fatality(fighter, FINISH_SPIKE);
}

// clang-format off: Keep table entries on one row, including long rows.
static const CommandDef commands[] = {
  {BUTTON_BLOCK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD, TOKEN_BACK}, 4, 50, CMD_BLOCK_OK, can_burn_fatality, scorpion_burn_fatality},
  {BUTTON_HIGH_PUNCH, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_DOWN, TOKEN_UP}, 4, 50, 0, can_beast_fatality, scorpion_beast_fatality},
  {BUTTON_LOW_KICK, {TOKEN_FORWARD, TOKEN_DOWN, TOKEN_DOWN}, 3, 40, 0, can_fan_fatality, scorpion_fan_fatality},
  {BUTTON_LOW_KICK, {TOKEN_BACK, TOKEN_FORWARD, TOKEN_FORWARD}, 3, 40, 0, can_spike_fatality, scorpion_spike_fatality},
  {BUTTON_BLOCK, {0}, 0, 0, CMD_AIR | CMD_BLOCK_OK, mk4_can_air_throw, scorpion_air_throw},
  {BUTTON_HIGH_PUNCH, {TOKEN_DOWN, TOKEN_BACK}, 2, 24, CMD_AIR, can_teleport, scorpion_teleport},
  {BUTTON_LOW_PUNCH, {TOKEN_BACK, TOKEN_BACK}, 2, 27, 0, can_spear, scorpion_spear},
  {BUTTON_LOW_PUNCH, {TOKEN_DOWN, TOKEN_FORWARD}, 2, 20, 0, fighting, scorpion_fire},
  {BUTTON_HIGH_KICK, {TOKEN_FORWARD, TOKEN_FORWARD}, 2, 24, 0, can_weapon, scorpion_weapon},
  {BUTTON_RUN, {0}, 0, 0, 0, mk4_can_sidestep, mk4_sidestep},
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
SWORD_MOVE(s_sword_hp, 235, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_lp, 236, ACTION_RETAIL_010E);
SWORD_MOVE(s_sword_back_lp, 237, ACTION_RETAIL_0112);
SWORD_MOVE(s_sword_back_hp, 238, ACTION_RETAIL_0112);
SWORD_MOVE(s_spin_start, 223, ACTION_RETAIL_0407);

static void scorpion_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

static void scorpion_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

static void scorpion_crouch(Fighter *fighter) {
  weapon_stance(fighter, S_CROUCH);
}

static void scorpion_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

static const WeaponDef sword;

static void equip_sword(Fighter *fighter) {
  fighter_equip_weapon(fighter, &sword);
}

static void script_weapon_ready(Fighter *fighter) {
  if (fighter_fiber_running(fighter)) {
    fighter_fiber_finish(fighter_ready);
  }
  fighter_finish_mmove(fighter, fighter_ready);
}

static void sword_slash_build(Fighter *fighter, int first, bool hit);

static void sword_slash_recover(Fighter *fighter) {
  fighter->body.vel[AXIS_X] = fighter->body.vel[AXIS_Z] = 0;
  fighter->action = ACTION_RETAIL_1006;
}

static void sword_slash_query(Fighter *fighter) {
  if (!fighter->connected && battle_contact(fighter, &sword_records[0]) != CONTACT_MISS) {
    sword_slash_recover(fighter);
    sword_slash_build(fighter, fighter->mmove_frame, true);
  }
}

static void sword_slash_build(Fighter *fighter, int first, bool hit) {
  int last = hit ? 42 : 43;
  native_begin(fighter, first, last, special_ready);
  for (int frame = first; frame <= last; ++frame) {
    FighterFn action = !hit && frame >= 17 && frame < 21 ? sword_slash_query : !hit && frame == 21 ? sword_slash_recover : NULL;
    native_frame(fighter, frame, frame + hit, action);
  }
  native_play(fighter, hit);
}

static void sword_attack(Fighter *fighter, bool hp, bool back) {
  if (fighter_airborne(fighter)) {
    weapon_air_frames(fighter, hp, (hp) ? retail_script_004e66d0 : retail_script_004e66f0);
    return;
  }
  const MoveDef *move = back ? (hp ? &s_sword_back_hp : &s_sword_back_lp) : (hp ? &s_sword_hp : &s_sword_lp);
  fighter_start_move(fighter, S_PUNCH, move);
  fighter->sample_override = 0;
  if (back) {
    (hp ? retail_script_004ec918 : retail_script_004ec8b0)(fighter);
  } else {
    sword_slash_build(fighter, 1, false);
  }
}

static void sword_hp(Fighter *fighter) {
  sword_attack(fighter, true, false);
}

static void sword_lp(Fighter *fighter) {
  sword_attack(fighter, false, false);
}

static void sword_back_hp(Fighter *fighter) {
  sword_attack(fighter, true, true);
}

static void sword_back_lp(Fighter *fighter) {
  sword_attack(fighter, false, true);
}

static void sword_spin_recover(Fighter *fighter) {
  stop_body(fighter);
  fighter->script_phase = 1;
  native_hold(fighter, 12, special_ready);
}

static void sword_spin_control(Fighter *fighter) {
  if (fighter_opponent(fighter)->hits >= 3) {
    sword_spin_recover(fighter);
    return;
  }
  if (!(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    fighter_finish_mmove(fighter, special_ready);
    return;
  }
  int direction = !!(fighter->held & BUTTON_BIT(BUTTON_RIGHT)) - !!(fighter->held & BUTTON_BIT(BUTTON_LEFT));
  fighter->body.vel[AXIS_X] = direction * 0xf5c;
}

static void sword_spin_query(Fighter *fighter) {
  if (battle_contact(fighter, &sword_records[3]) == CONTACT_BLOCK) {
    sword_spin_recover(fighter);
  } else {
    sword_spin_control(fighter);
  }
}

static void spin_script(Fighter *fighter) {
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 12, NULL);
  for (int frame = 1; frame <= 12; ++frame) {
    native_frame(fighter, frame, frame % 12, frame == 12 ? sword_spin_query : sword_spin_control);
  }
  fighter->native.poses[0] = 0;
  native_play(fighter, false);
  fighter->sample_override = pose;
}

static void sword_spin(Fighter *fighter) {
  fighter_start_move(fighter, S_PUNCH, &s_spin_start);
  fighter->sample_override = 0;
  fighter->mmove_cleanup = stop_body;
  retail_script_004e8850(fighter);
}

static const WeaponDef sword = {0x60, sword_hp, sword_lp, sword_back_hp, sword_back_lp, sword_spin, weapon_throw, equip_sword, SCENE_MODEL_SWORD};

static bool can_weapon(const Fighter *fighter) {
  return g_battle.phase == PHASE_FIGHT && (fighter->armed || (!fighter->weapon_used && !(fighter->flags & FF_WEAPON_LOST)));
}

static void scorpion_weapon(Fighter *fighter) {
  if (fighter->armed) {
    fighter->weapon->throw_move(fighter);
  } else if (fighter->flags & FF_FOLLOW_UP) {
    equip_sword(fighter);
    weapon_followup_victim(fighter);
    fighter_start_move(fighter, S_PUNCH, &s_sword_back_lp);
    fighter->sample_override = 0;
    retail_script_004ec8b0(fighter);
  } else {
    weapon_draw(fighter, &sword);
    fighter_start_fiber(fighter, retail_script_004e62c0, NULL);
  }
  fighter->sample_override = 0;
}

static bool can_pickup(const Fighter *fighter) {
  return weapon_can_pickup(fighter);
}

static void scorpion_pickup(Fighter *fighter) {
  weapon_pickup(fighter);
}

const CharacterDef mk4_scorpion = {GAME_MK4, 0, "SCORPION", &mk4_motion, moves, S_COUNT, &scorpion_controls, enter, -76873, mk4_walk, mk4_jump, NULL};

static void script_crouch_lk_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_knee_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_close_hp_tail(Fighter *fighter) {
  ground_native_tail(fighter);
}

static void script_combo_hp_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_roundhouse_tail(Fighter *fighter) {
  fighter_wait_ticks(1);
  ground_native_tail(fighter);
}

static void script_roundhouse_start(Fighter *fighter) {
  if (fighter->native_followup) {
    retail_script_004e6860(fighter);
  } else {
    retail_script_004e6878(fighter);
  }
}

static void script_air_hp_tail(Fighter *fighter) {
  air_native_tail(fighter, 0, 0);
}

static void script_air_lp_tail(Fighter *fighter) {
  air_native_tail(fighter, 1, 0);
}

static void script_air_kick_tail(Fighter *fighter) {
  air_native_tail(fighter, 2, 8);
}

static void script_air_lk_tail(Fighter *fighter) {
  air_native_tail(fighter, 4, 10);
}

static void script_air_diagonal_tail(Fighter *fighter) {
  air_native_tail(fighter, 3, 0);
}

static void script_sweep_tail(Fighter *fighter) {
  for (int visit = 0; visit < 2; ++visit) {
    fighter_wait_ticks(1);
    scorpion_sweep_contact(fighter);
  }
  mk4_script_finish_clip(fighter, 1);
  fighter_fiber_finish(scorpion_ready);
}

static void script_fan_cycle(Fighter *fighter) {
  mk4_script_fan_cycle(fighter);
}

static void script_fan_victim_end(Fighter *fighter) {
  mk4_script_fan_victim_end(fighter);
}

static void script_fan_victory(Fighter *fighter) {
  retail_script_004e0158(fighter);
}

static void script_fan_tail(Fighter *fighter) {
  mk4_script_fan_tail(fighter, retail_script_004e0138, retail_script_004e0148, retail_script_004e01a0);
}

static void script_spike_tail(Fighter *fighter) {
  mk4_script_spike_tail(fighter);
}

static void fan_start(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004e0188, NULL);
  retail_script_004e0118(fighter);
}

static void spike_start(Fighter *fighter) {
  while (g_battle.phase_ticks < 30) {
    fighter_wait_ticks(1);
  }
  retail_script_004e6950(fighter);
}

static void script_air_release(Fighter *fighter) {
  mk4_script_air_throw_release(fighter);
  retail_script_004e5e50(fighter);
}

static void script_kick_tail(Fighter *fighter) {
  for (int i = 0; i < 3; ++i) {
    scorpion_kick_contact(fighter);
    if (i < 2) {
      fighter_wait_ticks(1);
    }
  }
  retail_script_004e5c70(fighter);
}

static void scorpion_air_throw(Fighter *fighter) {
  mk4_air_throw_generated(fighter, retail_script_004e5e28, retail_script_004e9910);
}

static void script_draw_equip(Fighter *fighter) {
  equip_sword(fighter);
  retail_script_004e62d8(fighter);
}

static void spear_wait_native(Fighter *fighter) {
  while (!spear_not_pulling(fighter)) {
    fighter_wait_ticks(1);
  }
  fighter_fiber_finish(special_ready);
}

static void script_spear_done(Fighter *fighter) {
  if (spear_not_pulling(fighter)) {
    special_ready(fighter);
  } else {
    fighter_start_fiber(fighter, spear_wait_native, spear_cleanup);
  }
}

static void script_spear_return(Fighter *fighter) {
  fighter->spear_ready = g_battle.ticks + 90;
  fighter_fiber_finish(special_ready);
}

static void script_spawn_spear(Fighter *fighter, int type) {
  require(type == 67, "Unexpected spear task");
  spear_spawn(fighter);
}

static void script_spear_pull(Fighter *fighter) {
  fighter->damage_scale = 0x9999;
  for (;;) {
    fighter_wait_ticks(1);
  }
}

static void script_reaction(Fighter *fighter, int reaction) {
  Fighter *victim = opponent(fighter);
  if (reaction == 175) {
    return; /* Native arena continuation owns the impalement and completion. */
  }
  if (reaction == 71) {
    if (victim->action == ACTION_RETAIL_1008) {
      victim->reaction = 71;
    }
    return; /* Pull arrival already started the independently interruptible stun. */
  }
  require(reaction == 100, "Unexpected generated Scorpion reaction");
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  battle_react(victim, reaction);
}

static void script_teleport_motion(Fighter *fighter) {
  /* 480040 checks the portable arena/range limits before the native vanish
   * continuation. World physics owns integration of generated launch motion. */
  if (fighter->script_phase == 0 && (fighter_distance(fighter) >= 5.9f || abs(fighter->body.pos[AXIS_X]) >= g_battle.arena_radius - Q16_ONE ||
                                        (fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0))) {
    fighter->native_signal = true;
  }
}

static void script_teleport_land(Fighter *fighter) {
  fighter_fiber_move(fighter, S_JUMP, &s_teleport_land);
  fighter->sample_override = 18;
  teleport_land_script(fighter);
}

static void script_fire_decor(Fighter *fighter) {
  battle_particles(fighter, 8, 4, true);
}

static void script_fire_flame(Fighter *fighter) {
  battle_particles(fighter, 9, 4, true);
}

static void script_burn_tail(Fighter *victim) {
  victim->body.physics = fighter_friction;
  mk4_script_finish_clip(victim, 1);
  fighter_fiber_finish(mk4_script_thrown_ready);
}

static void script_teleport_victim_tail(Fighter *victim) {
  retail_script_004e9970(victim);
}

void scorpion_generated_reaction(Fighter *victim) {
  fighter_start_fiber(victim, victim->reaction == 173 ? retail_script_004e97f8 : retail_script_004e9938, NULL);
}

// clang-format off: Keep table entries on one row, including long rows.
/* Native-only blends; no VM timing tables. */
#define BLEND(clip_id, cursor) {{.clip = clip_id, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0700}, cursor, cursor, 1, 0x3333, 0x3333}
static const SceneClip burn_repeat_blend[] = {BLEND(390, 105)};
static const SceneClip burn_victory_blend[] = {BLEND(115, 0)};
static const SceneClip burn_victim_blend[] = {BLEND(206, 0)};
#undef BLEND
// clang-format on

static void native_blend(Fighter *fighter, const SceneClip *blend) {
  scene_begin(fighter, blend, 1, -1);
  do {
    fighter_wait_ticks(1);
  } while (!scene_step(fighter));
  fighter->scene.clips = NULL;
}

static void script_burn_begin(Fighter *fighter) {
  battle_sound(SOUND_B, 14);
  battle_sound(SOUND_B, 46);
  retail_script_004e0288(fighter);
}

static void script_burn_victim_start(Fighter *fighter) {
  Fighter *victim = opponent(fighter);
  victim->appearance = APPEARANCE_BURNED;
  fighter_start_fiber(victim, retail_script_004e0300, NULL);
  battle_sound(SOUND_B, 15);
  retail_script_004e0298(fighter);
}

static void script_burn_repeat1(Fighter *fighter) {
  native_blend(fighter, burn_repeat_blend);
  retail_script_004e02a8(fighter);
}

static void script_burn_repeat2(Fighter *fighter) {
  native_blend(fighter, burn_repeat_blend);
  retail_script_004e02b8(fighter);
}

static void script_burn_victory(Fighter *fighter) {
  native_blend(fighter, burn_victory_blend);
  retail_script_004e02d0(fighter);
}

static void script_burn_victim_cue(Fighter *victim) {
  battle_sound(SOUND_B, 16);
  retail_script_004e0310(victim);
}

static void script_burn_victim_repeat(Fighter *victim) {
  retail_script_004e0320(victim);
}

static void script_burn_victim_blend(Fighter *victim) {
  native_blend(victim, burn_victim_blend);
  retail_script_004e0338(victim);
}

static void script_burn_victim_impact(Fighter *victim) {
  mk4_script_shake(victim, 0);
  battle_particles(victim, 0, 4, true);
  retail_script_004e0348(victim);
}

static void script_burn_victim_settle(Fighter *victim) {
  mk4_script_shake(victim, 0);
  retail_script_004e0358(victim);
}

static void script_burn_victim_end(Fighter *victim) {
  fighter_fiber_finish(finish_scene);
  (void)victim;
}

static void script_transform_effect(Fighter *fighter, int type) {
  require(type == 0, "Unexpected transform effect");
  mk4_script_effect(fighter, 18);
}

static void beast_watch(Fighter *fighter) {
  if (fighter->script_phase == 9 && opponent(fighter)->script_phase == 9) {
    fighter_cancel_all_fibers();
    battle_end_finisher();
  }
}

static void script_beast_begin(Fighter *fighter) {
  fighter->appearance = APPEARANCE_BEAST;
  fighter->body.physics = beast_watch;
  retail_script_004d2e88(fighter);
}

static void script_beast_attack(Fighter *fighter) {
  fighter_start_fiber(opponent(fighter), retail_script_004d2ef8, NULL);
  retail_script_004d2eb0(fighter);
}

static void script_beast_victim_pieces(Fighter *victim) {
  battle_particles(victim, 0, 12, true);
  retail_script_004d2f10(victim);
}

static void script_beast_voice(Fighter *fighter) {
  battle_sound(SOUND_IMPACT_VOICE, roster_def(fighter->roster)->asset);
}

static void script_beast_park(Fighter *fighter) {
  fighter->script_phase = 9;
  for (;;) {
    fighter_wait_ticks(60);
  }
}

static void beast_gore_object(Projectile *object) {
  Fighter *victim = &g_battle.fighters[object->owner];
  if (g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 4 == 0) {
    battle_particles(victim, 0, 2, true);
  }
}

static void script_beast_gore(Fighter *victim, int type) {
  require(type == 0, "Unexpected beast gore task");
  battle_spawn_projectile(victim, PROJECTILE_EFFECT, 60, beast_gore_object);
}
