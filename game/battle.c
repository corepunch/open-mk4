#include "game.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

static void finish_round(Battle *battle);

static bool locked(const Fighter *fighter) {
  return (fighter_move(fighter)->flags & SF_LOCK) != 0 || (fighter->flags & FF_REACTING) != 0;
}

/* Round-start X from 0x422ea0 / 0x422f50: P1 -1.3, P2 +1.3, Z = 0. */
#define START_X 0x14ccc

void battle_set_arena(Battle *battle, int arena) {
  /* 423690 / 4dad60, in the archive stage order. */
  static const q16 radii[] = {0x98000, 0x98000, 0x98000, 0x98000, 0x98000, 0x98000, 0x88000, 0x98000, 0x98000, 0xa0000, 0x70000};
  require(arena >= 0 && arena < (int)(sizeof radii / sizeof *radii), "Invalid battle arena");
  battle->arena = arena;
  battle->arena_radius = radii[arena];
}

static void place(Battle *battle, Fighter *fighter, int roster_index, int wins, q16 x, int facing) {
  const CharacterDef *def = roster_def(roster_index);
  *fighter = (Fighter){0};
  fighter->assets = battle->assets;
  fighter->heading[0] = facing * Q16_ONE;
  fighter->roster = roster_index;
  fighter->wins = wins;
  fighter->health = Q16_ONE;
  fighter->damage_scale = Q16_ONE;
  fighter->run_energy = Q16_ONE;
  fighter->human = true;
  fighter->body.ground = def->ground;
  fighter->body.pos[AXIS_X] = x;
  fighter->body.pos[AXIS_Y] = def->ground;
  fighter->body.last_inside[AXIS_X] = x;
  fighter->body.last_inside[AXIS_Y] = def->ground;
  fighter->facing = facing;
  fighter_enter_state(battle, fighter, S_IDLE);
}

void battle_init(Battle *battle, int p1, int p2) {
  memset(battle, 0, sizeof *battle);
  battle_set_arena(battle, 0);
  battle->fighters[0].roster = p1;
  battle->fighters[1].roster = p2;
  roster_def(p1);
  roster_def(p2);
  battle_reset_match(battle);
}

void battle_reset_match(Battle *battle) {
  int i;
  for (i = 0; i < 2; ++i)
    battle->fighters[i].wins = 0;
  battle->round = 0;
  battle->ticks = 0;
  battle_next_round(battle);
}

void battle_next_round(Battle *battle) {
  size_t i;
  battle_set_arena(battle, battle->arena);
  battle->round++;
  for (i = 0; i < 2; ++i) {
    int roster_index = battle->fighters[i].roster;
    int wins = battle->fighters[i].wins;
    int facing = i ? -1 : 1;
    place(battle, &battle->fighters[i], roster_index, wins, i ? START_X : -START_X, facing);
  }
  battle->axis[0] = Q16_ONE;
  battle->axis[1] = 0;
  battle->distance = 2 * START_X;
  battle->ko_winner = 0;
  battle->finisher = 0;
  battle->contacts_off = false;
  battle->contacted = false;
  memset(battle->effects, 0, sizeof battle->effects);
  memset(battle->projectiles, 0, sizeof battle->projectiles);
  memset(battle->pieces, 0, sizeof battle->pieces);
  memset(battle->particles, 0, sizeof battle->particles);
  battle->camera = (SceneCamera){0};
  battle->sound_count = 0;
  battle->phase = PHASE_READY;
  battle->phase_ticks = 0;
  battle->finish_waiting = false;
  battle->time_ticks = 99 * 60;
  battle->winner = -1;
}

static const MoveDef dizzy = {.clip = 13, .last = 55, .flags = SF_LOCK, .action = 0x1009};
static const MoveDef defeated = {.clip = 128, .first = 109, .last = 109, .flags = SF_LOCK | SF_HOLD, .action = 0x4003};

static void stop_fighter(Fighter *fighter) {
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
}

bool battle_can_finish(const Battle *battle, const Fighter *fighter) {
  return battle->phase == PHASE_FINISH && !battle->finish_waiting && !battle->contacted && battle->phase_ticks <= 300 && battle->finisher == 0 &&
         battle->winner >= 0 && fighter == &battle->fighters[battle->winner] && fighter_opponent(battle, fighter)->action == 0x1009;
}

bool battle_begin_finisher(Battle *battle, Fighter *fighter, int id) {
  if (id <= 0 || !battle_can_finish(battle, fighter)) {
    return false;
  }
  battle->finisher = id;
  battle->phase = PHASE_FATALITY;
  battle->phase_ticks = 0;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &battle->fighters[i];
    if (f->armed && f->disarm != NULL) {
      f->disarm(battle, f);
    }
    stop_fighter(f);
    f->pending = f->script = NULL;
    f->buttons = NULL;
    f->flags = FF_BUSY | FF_UNTOUCHABLE;
  }
  memset(battle->projectiles, 0, sizeof battle->projectiles);
  return true;
}

void battle_end_finisher(Battle *battle) {
  battle->phase = PHASE_MATCH_OVER;
  battle->contacts_off = true;
  battle->phase_ticks = 0;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &battle->fighters[i];
    q16 y = f->body.pos[AXIS_Y];
    stop_fighter(f);
    if (battle->finisher) {
      f->body.pos[AXIS_Y] = y;
    }
    f->pending = f->script = NULL;
    f->flags &= ~(unsigned)FF_PAIRED;
    if (battle->finisher == 0) {
      if (i == battle->winner) {
        fighter_enter_state(battle, f, S_VICTORY);
      } else {
        fighter_start_move(battle, f, S_KNOCKDOWN, &defeated);
      }
    }
  }
}

static void begin_finish_window(Battle *battle) {
  battle->phase = PHASE_FINISH;
  battle->phase_ticks = 0;
  battle->contacts_off = false;
  battle->contacted = false;
  battle->finish_waiting = false;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &battle->fighters[i];
    stop_fighter(f);
    f->flags = 0;
    if (i == battle->winner) {
      fighter_start_move(battle, f, S_IDLE, roster_def(f->roster)->moves[S_IDLE]);
    } else {
      fighter_start_move(battle, f, S_HIT, &dizzy);
    }
    memset(&f->directions, 0, sizeof f->directions);
  }
}

/* 48fc20 dispatches the defeated controller only after its reaction tail.
 * Keep native continuations and airborne bodies alive until that boundary. */
static bool defeat_settled(const Fighter *fighter) {
  const MoveDef *move = fighter_move(fighter);
  if (fighter->pending != NULL || fighter->script != NULL || fighter_airborne(fighter) || fighter->body.gravity != 0) {
    return false;
  }
  if (move->sequence != NULL) {
    return fighter->ticks >= move->sequence->count;
  }
  return !(move->flags & SF_REACTION) || fighter->ticks >= move->recovery;
}

static void finish_round(Battle *battle) {
  int i;
  if (battle->fighters[0].health == battle->fighters[1].health)
    battle->winner = -1;
  else if (battle->fighters[0].health > battle->fighters[1].health)
    battle->winner = 0;
  else
    battle->winner = 1;
  if (battle->winner >= 0) {
    battle->fighters[battle->winner].wins++;
    fighter_enter_state(battle, &battle->fighters[battle->winner], S_VICTORY);
  }
  for (i = 0; i < 2; ++i) {
    if (battle->fighters[i].health <= 0 && battle->fighters[i].state != S_KNOCKDOWN && battle->fighters[i].pending == NULL &&
        !(battle->fighters[i].flags & FF_REACTING))
      fighter_enter_state(battle, &battle->fighters[i], S_KNOCKDOWN);
  }
  battle->contacts_off = true;
  memset(battle->projectiles, 0, sizeof battle->projectiles);
  if (battle->winner >= 0 && battle->fighters[battle->winner].wins >= 2)
    battle->phase = PHASE_MATCH_OVER;
  else
    battle->phase = PHASE_ROUND_OVER;
  battle->phase_ticks = 0;
  if (battle->phase == PHASE_MATCH_OVER && battle->ko_winner != 0 && battle->time_ticks > 0) {
    if (defeat_settled(&battle->fighters[1 - battle->winner])) {
      begin_finish_window(battle);
    } else {
      battle->phase = PHASE_FINISH;
      battle->finish_waiting = true;
    }
  }
}

static bool held(const Fighter *fighter, Button button) {
  return (fighter->held & BUTTON_BIT(button)) != 0;
}

static void land_idle(Battle *battle, Fighter *fighter) {
  if (fighter->state == S_JUMP)
    fighter_enter_state(battle, fighter, S_IDLE);
}

static void advance_fighter(Battle *battle, Fighter *fighter, const Fighter *other) {
  const CharacterDef *def = roster_def(fighter->roster);
  const Motion *motion = def->motion;
  bool grounded = !fighter_airborne(fighter);
  int dir;

  if (!locked(fighter) && grounded)
    fighter->facing = fighter->body.pos[AXIS_X] <= other->body.pos[AXIS_X] ? 1 : -1;
  if (fighter->pending != NULL) {
    /* A press or a reaction interrupted this fighter; its entry runs now. */
    FighterFn entry = fighter->pending;
    fighter->pending = NULL;
    entry(battle, fighter);
  } else {
    const MoveDef *state = fighter_move(fighter);
    fighter->ticks++;
    if (fighter->script != NULL) {
      fighter->script(battle, fighter);
    } else if (state->sequence != NULL) {
      fighter_step_sequence(battle, fighter);
    } else if (state->flags & SF_ATTACK) {
      int total = state->startup + state->active + state->recovery;
      if (fighter->ticks >= total)
        fighter_enter_state(battle, fighter, fighter_airborne(fighter) ? S_JUMP : S_IDLE);
    } else if (state->flags & SF_REACTION) {
      bool landed = !(state->flags & SF_WAIT_LAND) || !fighter_airborne(fighter);
      if (fighter->ticks >= state->recovery && landed)
        fighter_enter_state(battle, fighter, S_IDLE);
    }
  }
  if (locked(fighter) || fighter->pending != NULL)
    return;

  grounded = !fighter_airborne(fighter);
  fighter->crouching = held(fighter, BUTTON_DOWN) && grounded;
  if (held(fighter, BUTTON_BLOCK) && grounded) {
    fighter_set_speed(battle, fighter, 0);
    fighter_enter_state(battle, fighter, S_BLOCK);
    return;
  }
  dir = (int)held(fighter, BUTTON_RIGHT) - (int)held(fighter, BUTTON_LEFT);
  if ((battle->phase == PHASE_FIGHT || battle_can_finish(battle, fighter)) && held(fighter, BUTTON_UP) && grounded) {
    if (def->jump != NULL) {
      def->jump(battle, fighter);
      return;
    }
    /* Straight jump 0x46f7e0 (vy 0x30a3 up, gravity 0x28f). Holding a
     * direction approximates the flips with their 0xe14 speed. */
    fighter->body.vel[AXIS_Y] = -motion->jump_velocity;
    fighter_set_speed(battle, fighter, dir * fighter->facing * 0xe14);
    fighter->body.gravity = motion->gravity;
    fighter->body.on_land = land_idle;
    fighter_enter_state(battle, fighter, S_JUMP);
    return;
  }
  if (grounded && !fighter->crouching) {
    if (dir != 0 && def->walk != NULL) {
      def->walk(battle, fighter);
      return;
    }
    fighter->body.pos[AXIS_X] += dir * (held(fighter, BUTTON_RUN) ? motion->run : motion->walk);
  }
  if (grounded) {
    fighter_set_speed(battle, fighter, 0);
  }
  if (!grounded)
    fighter_enter_state(battle, fighter, S_JUMP);
  else if (fighter->crouching)
    fighter_enter_state(battle, fighter, S_CROUCH);
  else if (dir)
    fighter_enter_state(battle, fighter, S_WALK);
  else
    fighter_enter_state(battle, fighter, S_IDLE);
}

/* Switch scan 0x45c560: a press needs the switch up in both previous samples. */
static void sample_switches(Fighter *fighter, Input in) {
  fighter->held = in.held;
  fighter->pressed = in.held & ~fighter->prev[0] & ~fighter->prev[1];
  fighter->prev[1] = fighter->prev[0];
  fighter->prev[0] = in.held;
}

/* Press handler tasks run after the fighter tasks in the same pump. */
static void dispatch_presses(Battle *battle) {
  for (int i = 0; i < 2; ++i) {
    Fighter *fighter = &battle->fighters[i];
    for (int button = 0; button < BUTTON_COUNT; ++button) {
      if (fighter->pressed & BUTTON_BIT(button)) {
        fighter_press(battle, fighter, (Button)button);
      }
    }
  }
}

static q16 to_q16(double value) {
  return (q16)floor(value * Q16_ONE);
}

/* Our world update: arena bounds, pair axis, body separation, callbacks,
 * gravity and integration. Script motion is expressed in units per tick. */
void battle_world(Battle *battle) {
  Fighter *a = &battle->fighters[0], *b = &battle->fighters[1];
  double r2 = (double)battle->arena_radius * battle->arena_radius;
  for (int i = 0; i < 2; ++i) {
    Body *body = &battle->fighters[i].body;
    if (body->walking) {
      body->pos[AXIS_X] += body->vel[AXIS_X];
      body->pos[AXIS_Z] += body->vel[AXIS_Z];
    }
    double x = body->pos[AXIS_X], z = body->pos[AXIS_Z];
    double next_x = x + body->vel[AXIS_X], next_z = z + body->vel[AXIS_Z];
    if (x * x + z * z >= r2 && next_x * next_x + next_z * next_z > x * x + z * z) {
      body->pos[AXIS_X] = body->last_inside[AXIS_X];
      body->pos[AXIS_Z] = body->last_inside[AXIS_Z];
      body->vel[AXIS_X] = 0;
      body->vel[AXIS_Z] = 0;
    } else if (x * x + z * z < r2) {
      memcpy(body->last_inside, body->pos, sizeof body->pos);
    }
  }
  double dx = (double)b->body.pos[AXIS_X] - a->body.pos[AXIS_X];
  double dz = (double)b->body.pos[AXIS_Z] - a->body.pos[AXIS_Z];
  double length = sqrt(dx * dx + dz * dz);
  if (length > 0.) {
    battle->axis[0] = to_q16(dx / length);
    battle->axis[1] = to_q16(dz / length);
  }
  battle->distance = (q16)floor(length);
  q16 dy = a->body.pos[AXIS_Y] - b->body.pos[AXIS_Y];
  if (!(a->flags & FF_PAIRED) && !(b->flags & FF_PAIRED) && battle->distance < 0x8000 && (dy < 0 ? -dy : dy) <= 0xb333) {
    q16 push = (0x8000 - battle->distance) / 2 + 1;
    for (int k = 0; k < 2; ++k) {
      q16 step = q16_mul(battle->axis[k], push);
      a->body.pos[k == 0 ? AXIS_X : AXIS_Z] -= step;
      b->body.pos[k == 0 ? AXIS_X : AXIS_Z] += step;
    }
  }
  for (int i = 0; i < 2; ++i) {
    Fighter *fighter = &battle->fighters[i];
    Body *body = &fighter->body;
    int sign = (dx < 0 ? -1 : 1) * fighter->facing;
    fighter->heading[0] = battle->axis[0] * sign;
    fighter->heading[1] = battle->axis[1] * sign;
    if (body->physics != NULL)
      body->physics(battle, fighter);
    if (body->gravity != 0) {
      body->vel[AXIS_Y] += body->gravity;
      if (body->vel[AXIS_Y] >= 0 && body->pos[AXIS_Y] + body->root_y + body->vel[AXIS_Y] >= body->ground) {
        memset(body->vel, 0, sizeof body->vel);
        memset(body->spin, 0, sizeof body->spin);
        body->root_y = 0;
        body->gravity = 0;
        body->pos[AXIS_Y] = body->ground;
        FighterFn land = body->on_land;
        body->on_land = NULL;
        if (land != NULL)
          land(battle, fighter);
      }
    }
  }
  for (int i = 0; i < 2; ++i) {
    Body *body = &battle->fighters[i].body;
    for (int k = 0; k < 3; ++k) {
      if (!body->walking || k == AXIS_Y) {
        body->pos[k] += body->vel[k];
      }
    }
    for (int k = 0; k < 3; ++k) {
      body->angle[k] = (body->angle[k] + body->spin[k]) % 0x6487e;
    }
    double x = body->pos[AXIS_X], z = body->pos[AXIS_Z];
    double old_x = x - body->vel[AXIS_X], old_z = z - body->vel[AXIS_Z];
    if (x * x + z * z >= r2 && ((body->vel[AXIS_X] == 0 && body->vel[AXIS_Z] == 0) || x * x + z * z > old_x * old_x + old_z * old_z)) {
      /* 489cb0 restores the last inside point; inward motion is allowed. */
      body->pos[AXIS_X] = body->last_inside[AXIS_X];
      body->pos[AXIS_Z] = body->last_inside[AXIS_Z];
      body->vel[AXIS_X] = 0;
      body->vel[AXIS_Z] = 0;
    }
  }
}

/* Approximate rules for moves without retail attack records. Both attacks
 * are judged from the positions at the start of resolution, so two such
 * moves that become active on the same tick can trade. */
static void resolve_hits(Battle *battle, const Fighter snapshot[2]) {
  for (int i = 0; i < 2; ++i) {
    const Fighter *attacker = &snapshot[i];
    const Fighter *defender = &snapshot[1 - i];
    const MoveDef *move = fighter_move(attacker);
    if (!(move->flags & SF_ATTACK) || (move->flags & SF_RECORD) || attacker->connected)
      continue;
    if (attacker->ticks < move->startup || attacker->ticks >= move->startup + move->active)
      continue;
    float distance = (fighter_x(defender) - fighter_x(attacker)) * (float)attacker->facing;
    float attacker_height = fighter_height(attacker), defender_height = fighter_height(defender);
    if (distance < 0.f || distance > move->reach)
      continue;
    if (fabsf(attacker_height - defender_height) > 0.9f)
      continue;
    if ((move->flags & SF_LOW) && defender_height > 0.2f)
      continue;
    if ((move->flags & SF_UNBLOCKABLE) && (defender_height > 0.f || defender->state == S_KNOCKDOWN || defender->state == S_HIT))
      continue;
    if (defender->state == S_KNOCKDOWN)
      continue;

    bool blocked = defender->state == S_BLOCK && !(move->flags & SF_UNBLOCKABLE) && (!(move->flags & SF_LOW) || defender->crouching);
    /* A simultaneous incoming reaction may already have interrupted this
     * fighter. The outgoing hit still trades, but cannot restart its move. */
    if (battle->fighters[i].current_move == attacker->current_move) {
      fighter_note_contact(&battle->fighters[i], blocked ? CONTACT_BLOCK : defender_height > 0.f ? CONTACT_AIR_HIT : CONTACT_HIT);
    }
    Fighter *target = &battle->fighters[1 - i];
    if (!blocked)
      battle_damage(battle, target, move->damage * Q16_ONE / 100);
    target->body.pos[AXIS_X] += to_q16((double)attacker->facing * move->push * (blocked ? 0.4 : 1.0));
    if (!blocked) {
      if ((move->flags & SF_LAUNCH) && target->armed && target->disarm != NULL) {
        target->disarm(battle, target);
      }
      fighter_enter_state(battle, target, (move->flags & SF_LAUNCH) ? S_KNOCKDOWN : S_HIT);
      target->ticks = fighter_move(target)->recovery - move->stun;
      if (move->flags & SF_LAUNCH) {
        target->body.vel[AXIS_Y] = -0x140e; /* the prototype's 4.7 units/s */
        target->body.gravity = 0x111;
      }
    }
    battle->effects[i] =
        (HitEffect){(fighter_x(attacker) + fighter_x(defender)) * 0.5f, defender_height + ((move->flags & SF_LOW) ? 0.25f : 1.2f), 10, blocked};
  }
}

void battle_step(Battle *battle, const Input input[2]) {
  int i;
  battle->ticks++;
  battle->sound_count = 0;
  battle_step_presentation(battle);
  for (i = 0; i < 2; ++i)
    sample_switches(&battle->fighters[i], input[i]);
  for (i = 0; i < 2; ++i) {
    Fighter *fighter = &battle->fighters[i];
    /* 422800: decrement the recovery delay, then restore 5% per tick. */
    fighter->run_delay = maxi(0, fighter->run_delay - 1);
    if (fighter->run_delay == 0) {
      fighter->run_energy = mini(Q16_ONE, fighter->run_energy + 0xccc);
    }
  }
  battle->phase_ticks++;
  for (i = 0; i < 2; ++i) {
    if (battle->effects[i].ticks > 0)
      battle->effects[i].ticks--;
  }
  if (battle->phase == PHASE_READY) {
    if (battle->phase_ticks >= 90) {
      battle->phase = PHASE_FIGHT;
      battle->phase_ticks = 0;
    }
    return;
  }
  if (battle->phase == PHASE_FINISH) {
    Fighter *winner = &battle->fighters[battle->winner];
    Fighter *victim = &battle->fighters[1 - battle->winner];
    if (battle->finish_waiting) {
      advance_fighter(battle, victim, winner);
      battle_world(battle);
      if (defeat_settled(victim)) {
        begin_finish_window(battle);
      }
      return;
    }
    advance_fighter(battle, winner, victim);
    advance_fighter(battle, victim, winner);
    for (int button = 0; button < BUTTON_COUNT; ++button) {
      if (battle->phase_ticks < 300 && (winner->pressed & BUTTON_BIT(button))) {
        fighter_press(battle, winner, (Button)button);
      }
    }
    battle_world(battle);
    /* 421d40: 60-tick grace, clear the contact latch, then 240 ticks
     * ending early on contact. A press on expiry cannot extend the clock. */
    if (battle->phase_ticks == 60) {
      battle->contacted = false;
    }
    if (battle->phase == PHASE_FINISH && ((battle->phase_ticks > 60 && battle->contacted) || battle->phase_ticks >= 300)) {
      battle_end_finisher(battle);
    }
    return;
  }
  if (battle->phase != PHASE_FIGHT) {
    /* Poses keep playing to their last sample and bodies keep falling. */
    for (i = 0; i < 2; ++i) {
      Fighter *fighter = &battle->fighters[i];
      if (fighter->pending != NULL) {
        FighterFn entry = fighter->pending;
        fighter->pending = NULL;
        entry(battle, fighter);
      } else {
        fighter->ticks++;
        if (fighter->script != NULL) {
          fighter->script(battle, fighter);
        } else if (fighter_move(fighter)->sequence != NULL) {
          fighter_step_sequence(battle, fighter);
        }
        if (battle->phase == PHASE_MATCH_OVER && fighter->scene.clips != NULL) {
          scene_step(fighter);
        }
      }
    }
    battle_world(battle);
    if (battle->phase == PHASE_ROUND_OVER && battle->phase_ticks >= 180)
      battle_next_round(battle);
    return;
  }

  battle->time_ticks--;
  /* Retail tick 0x41fd70: task pump (fighters, then press tasks), world pump. */
  for (i = 0; i < 2; ++i)
    advance_fighter(battle, &battle->fighters[i], &battle->fighters[1 - i]);
  for (int p = 0; p < MAX_PROJECTILES; ++p) {
    Projectile *projectile = &battle->projectiles[p];
    if (projectile->kind != 0) {
      projectile->ticks++;
      projectile->step(battle, projectile);
      if (--projectile->lifetime <= 0) {
        projectile->kind = 0;
      }
    }
  }
  dispatch_presses(battle);
  battle_world(battle);
  {
    Fighter snapshot[2];
    memcpy(snapshot, battle->fighters, sizeof snapshot);
    resolve_hits(battle, snapshot);
  }
  if (battle->fighters[0].health <= 0 || battle->fighters[1].health <= 0 || battle->time_ticks <= 0) {
    finish_round(battle);
  }
}
