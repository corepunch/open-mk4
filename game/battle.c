#include "game.h"
#include "native.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

Battle g_battle;

static void finish_round(void);

static bool locked(const Fighter *fighter) {
  return (fighter_move(fighter)->flags & SF_LOCK) != 0 || (fighter->flags & FF_REACTING) != 0;
}

/* Round-start X from 0x422ea0 / 0x422f50: P1 -1.3, P2 +1.3, Z = 0. */
#define START_X 0x14ccc

void battle_set_arena(int arena) {
  /* 423690 / 4dad60, in the archive stage order. */
  static const q16 radii[] = {0x98000, 0x98000, 0x98000, 0x98000, 0x98000, 0x98000, 0x88000, 0x98000, 0x98000, 0xa0000, 0x70000};
  require(arena >= 0 && arena < (int)(sizeof radii / sizeof *radii), "Invalid battle arena");
  g_battle.arena = arena;
  g_battle.arena_radius = radii[arena];
}

static void place(Fighter *fighter, int roster_index, int wins, q16 x, int facing) {
  const CharacterDef *def = roster_def(roster_index);
  bool human = fighter->human;
  *fighter = (Fighter){0};
  fighter->assets = g_battle.assets;
  fighter->heading[0] = facing * Q16_ONE;
  fighter->roster = roster_index;
  fighter->wins = wins;
  fighter->health = Q16_ONE;
  fighter->damage_scale = Q16_ONE;
  fighter->run_energy = Q16_ONE;
  fighter->human = human;
  fighter->body.ground = def->ground;
  fighter->body.pos[AXIS_X] = x;
  fighter->body.pos[AXIS_Y] = def->ground;
  fighter->body.last_inside[AXIS_X] = x;
  fighter->body.last_inside[AXIS_Y] = def->ground;
  fighter->facing = facing;
  fighter_enter_state(fighter, S_IDLE);
}

void battle_init(int p1, int p2) {
  fighter_cancel_all_fibers();
  fighter_cancel_all_mmoves();
  memset(&g_battle, 0, sizeof g_battle);
  battle_set_arena(0);
  g_battle.cpu_level = 3;
  g_battle.fighters[0].roster = p1;
  g_battle.fighters[1].roster = p2;
  g_battle.fighters[0].human = g_battle.fighters[1].human = true;
  roster_def(p1);
  roster_def(p2);
  battle_reset_match();
}

void battle_reset_match(void) {
  fighter_cancel_all_fibers();
  fighter_cancel_all_mmoves();
  g_battle.previous_animation.valid = false;
  g_battle.current_animation.valid = false;
  int i;
  for (i = 0; i < 2; ++i)
    g_battle.fighters[i].wins = 0;
  g_battle.round = 0;
  g_battle.ticks = 0;
  g_battle.random[0] = 0xbc4ff2;
  g_battle.random[1] = 0xc83e33;
  battle_next_round();
}

void battle_next_round(void) {
  fighter_cancel_all_fibers();
  fighter_cancel_all_mmoves();
  battle_event_cancel_all();
  g_battle.previous_animation.valid = false;
  g_battle.current_animation.valid = false;
  size_t i;
  battle_set_arena(g_battle.arena);
  g_battle.round++;
  for (i = 0; i < 2; ++i) {
    int roster_index = g_battle.fighters[i].roster;
    int wins = g_battle.fighters[i].wins;
    int facing = i ? -1 : 1;
    place(&g_battle.fighters[i], roster_index, wins, i ? START_X : -START_X, facing);
  }
  g_battle.axis[0] = Q16_ONE;
  g_battle.axis[1] = 0;
  g_battle.distance = 2 * START_X;
  g_battle.ko_winner = 0;
  g_battle.finisher = 0;
  g_battle.contacts_off = false;
  g_battle.contacted = false;
  memset(g_battle.effects, 0, sizeof g_battle.effects);
  memset(g_battle.projectiles, 0, sizeof g_battle.projectiles);
  memset(g_battle.pieces, 0, sizeof g_battle.pieces);
  memset(g_battle.attachments, 0, sizeof g_battle.attachments);
  memset(g_battle.particles, 0, sizeof g_battle.particles);
  memset(g_battle.blood, 0, sizeof g_battle.blood);
  memset(g_battle.splats, 0, sizeof g_battle.splats);
  g_battle.splat_next = 0;
  memset(g_battle.sprays, 0, sizeof g_battle.sprays);
  g_battle.camera = (SceneCamera){0};
  g_battle.sound_count = 0;
  g_battle.phase = PHASE_READY;
  g_battle.phase_ticks = 0;
  g_battle.finish_waiting = false;
  g_battle.time_ticks = 99 * 60;
  g_battle.winner = -1;
  battle_capture_render_snapshot();
}

void battle_capture_render_snapshot(void) {
  if (g_battle.assets == NULL) {
    return;
  }
  if (g_battle.current_animation.valid && g_battle.current_animation.tick > g_battle.ticks) {
    g_battle.previous_animation.valid = false;
    g_battle.current_animation.valid = false;
  }
  if (g_battle.current_animation.valid && g_battle.ticks - g_battle.current_animation.tick < 2) {
    return;
  }
  AnimationSnapshot next = {.tick = g_battle.ticks, .valid = true};
  for (int i = 0; i < 2; ++i) {
    const Fighter *fighter = &g_battle.fighters[i];
    const Character *model = fighter_model(g_battle.assets, fighter);
    const Clip *clip = fighter_clip(g_battle.assets, fighter);
    next.fighters[i] = (FighterAnimationFrame){model, (int)(clip - g_battle.assets->clips.data), fighter_pose_sample(fighter), true};
  }
  if (!g_battle.current_animation.valid) {
    g_battle.previous_animation = next;
  } else {
    g_battle.previous_animation = g_battle.current_animation;
  }
  g_battle.current_animation = next;
}

static const MoveDef dizzy = {.clip = 13, .last = 55, .flags = SF_LOCK, .action = 0x1009};
static const MoveDef defeated = {.clip = 128, .first = 109, .last = 109, .flags = SF_LOCK | SF_HOLD, .action = 0x4003};

static void stop_fighter(Fighter *fighter) {
  native_motion_stop(fighter);
  memset(fighter->body.vel, 0, sizeof fighter->body.vel);
  fighter->body.gravity = 0;
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
}

bool battle_can_finish(const Fighter *fighter) {
  return g_battle.phase == PHASE_FINISH && !g_battle.finish_waiting && !g_battle.contacted && g_battle.phase_ticks <= 300 && g_battle.finisher == 0 &&
         g_battle.winner >= 0 && fighter == &g_battle.fighters[g_battle.winner] && fighter_opponent(fighter)->action == 0x1009;
}

bool battle_begin_finisher(Fighter *fighter, int id) {
  if (id <= 0 || !battle_can_finish(fighter)) {
    return false;
  }
  g_battle.finisher = id;
  g_battle.phase = PHASE_FATALITY;
  g_battle.phase_ticks = 0;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &g_battle.fighters[i];
    if (f->armed && f->disarm != NULL) {
      f->disarm(f);
    }
    stop_fighter(f);
    f->pending = f->script = NULL;
    f->buttons = NULL;
    f->flags = FF_BUSY | FF_UNTOUCHABLE;
  }
  memset(g_battle.projectiles, 0, sizeof g_battle.projectiles);
  return true;
}

void battle_end_finisher(void) {
  g_battle.phase = PHASE_MATCH_OVER;
  g_battle.contacts_off = true;
  g_battle.phase_ticks = 0;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &g_battle.fighters[i];
    q16 y = f->body.pos[AXIS_Y];
    stop_fighter(f);
    if (g_battle.finisher) {
      f->body.pos[AXIS_Y] = y;
    }
    f->pending = f->script = NULL;
    f->flags &= ~(unsigned)FF_PAIRED;
    if (g_battle.finisher == 0) {
      if (i == g_battle.winner) {
        fighter_enter_state(f, S_VICTORY);
      } else {
        fighter_start_move(f, S_KNOCKDOWN, &defeated);
      }
    }
  }
}

static void begin_finish_window(void) {
  g_battle.phase = PHASE_FINISH;
  g_battle.phase_ticks = 0;
  g_battle.contacts_off = false;
  g_battle.contacted = false;
  g_battle.finish_waiting = false;
  for (int i = 0; i < 2; ++i) {
    Fighter *f = &g_battle.fighters[i];
    stop_fighter(f);
    f->flags = 0;
    if (i == g_battle.winner) {
      fighter_start_move(f, S_IDLE, roster_def(f->roster)->moves[S_IDLE]);
    } else {
      fighter_start_move(f, S_HIT, &dizzy);
    }
    memset(&f->directions, 0, sizeof f->directions);
  }
}

/* 48fc20 dispatches the defeated controller only after its reaction tail.
 * Keep native continuations and airborne bodies alive until that boundary. */
static bool defeat_settled(const Fighter *fighter) {
  const MoveDef *move = fighter_move(fighter);
  if (fighter->pending != NULL || fighter->script != NULL || fighter->current_mmove != NULL || fighter_airborne(fighter) || fighter->body.gravity != 0) {
    return false;
  }
  if (fighter->sequence != NULL) {
    return fighter->ticks >= fighter->sequence->count;
  }
  return !(move->flags & SF_REACTION) || fighter->ticks >= move->recovery;
}

static void finish_round(void) {
  int i;
  if (g_battle.fighters[0].health == g_battle.fighters[1].health)
    g_battle.winner = -1;
  else if (g_battle.fighters[0].health > g_battle.fighters[1].health)
    g_battle.winner = 0;
  else
    g_battle.winner = 1;
  if (g_battle.winner >= 0) {
    g_battle.fighters[g_battle.winner].wins++;
    fighter_enter_state(&g_battle.fighters[g_battle.winner], S_VICTORY);
  }
  for (i = 0; i < 2; ++i) {
    if (g_battle.fighters[i].health <= 0 && g_battle.fighters[i].state != S_KNOCKDOWN && g_battle.fighters[i].pending == NULL &&
        !(g_battle.fighters[i].flags & FF_REACTING))
      fighter_enter_state(&g_battle.fighters[i], S_KNOCKDOWN);
  }
  g_battle.contacts_off = true;
  memset(g_battle.projectiles, 0, sizeof g_battle.projectiles);
  if (g_battle.winner >= 0 && g_battle.fighters[g_battle.winner].wins >= 2)
    g_battle.phase = PHASE_MATCH_OVER;
  else
    g_battle.phase = PHASE_ROUND_OVER;
  g_battle.phase_ticks = 0;
  if (g_battle.phase == PHASE_MATCH_OVER && g_battle.ko_winner != 0 && g_battle.time_ticks > 0) {
    if (defeat_settled(&g_battle.fighters[1 - g_battle.winner])) {
      begin_finish_window();
    } else {
      g_battle.phase = PHASE_FINISH;
      g_battle.finish_waiting = true;
    }
  }
}

static bool held(const Fighter *fighter, Button button) {
  return (fighter->held & BUTTON_BIT(button)) != 0;
}

static void land_idle(Fighter *fighter) {
  if (fighter->state == S_JUMP)
    fighter_enter_state(fighter, S_IDLE);
}

static void advance_fighter(Fighter *fighter, const Fighter *other) {
  fighter_fiber_pump_begin(fighter);
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
    entry(fighter);
  } else {
    const MoveDef *state = fighter_move(fighter);
    fighter->ticks++;
    if (fighter->script != NULL) {
      fighter->script(fighter);
    } else if (fighter->current_mmove != NULL) {
      fighter_step_mmove(fighter);
    } else if (fighter->sequence != NULL) {
      fighter_step_sequence(fighter);
    } else if (state->flags & SF_ATTACK) {
      int total = state->startup + state->active + state->recovery;
      if (fighter->ticks >= total)
        fighter_enter_state(fighter, fighter_airborne(fighter) ? S_JUMP : S_IDLE);
    } else if (state->flags & SF_REACTION) {
      bool landed = !(state->flags & SF_WAIT_LAND) || !fighter_airborne(fighter);
      if (fighter->ticks >= state->recovery && landed)
        fighter_enter_state(fighter, S_IDLE);
    }
  }
  if (locked(fighter) || fighter->pending != NULL)
    return;

  grounded = !fighter_airborne(fighter);
  fighter->crouching = held(fighter, BUTTON_DOWN) && grounded;
  if (held(fighter, BUTTON_BLOCK) && grounded) {
    fighter_set_speed(fighter, 0);
    fighter_enter_state(fighter, S_BLOCK);
    return;
  }
  dir = (int)held(fighter, BUTTON_RIGHT) - (int)held(fighter, BUTTON_LEFT);
  if ((g_battle.phase == PHASE_FIGHT || battle_can_finish(fighter)) && held(fighter, BUTTON_UP) && grounded) {
    if (def->jump != NULL) {
      def->jump(fighter);
      return;
    }
    /* Straight jump 0x46f7e0 (vy 0x30a3 up, gravity 0x28f). Holding a
     * direction approximates the flips with their 0xe14 speed. */
    fighter->body.vel[AXIS_Y] = -motion->jump_velocity;
    fighter_set_speed(fighter, dir * fighter->facing * 0xe14);
    fighter->body.gravity = motion->gravity;
    fighter->body.on_land = land_idle;
    fighter_enter_state(fighter, S_JUMP);
    return;
  }
  if (grounded && !fighter->crouching) {
    if (dir != 0 && def->walk != NULL) {
      def->walk(fighter);
      return;
    }
    fighter->body.pos[AXIS_X] += dir * (held(fighter, BUTTON_RUN) ? motion->run : motion->walk);
  }
  if (grounded) {
    fighter_set_speed(fighter, 0);
  }
  if (!grounded)
    fighter_enter_state(fighter, S_JUMP);
  else if (fighter->crouching)
    fighter_enter_state(fighter, S_CROUCH);
  else if (dir)
    fighter_enter_state(fighter, S_WALK);
  else
    fighter_enter_state(fighter, S_IDLE);
}

/* Switch scan 0x45c560: a press needs the switch up in both previous samples. */
static void sample_switches(Fighter *fighter, Input in) {
  fighter->held = in.held;
  fighter->pressed = in.held & ~fighter->prev[0] & ~fighter->prev[1];
  fighter->released = fighter->prev[0] & ~in.held;
  unsigned group_b = BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_RUN);
  if (fighter->pressed & group_b) {
    fighter->released &= ~group_b;
  }
  fighter->prev[1] = fighter->prev[0];
  fighter->prev[0] = in.held;
}

/* Press handler tasks run after the fighter tasks in the same pump. */
static void dispatch_presses(void) {
  for (int i = 0; i < 2; ++i) {
    Fighter *fighter = &g_battle.fighters[i];
    for (int button = 0; button < BUTTON_COUNT; ++button) {
      if (fighter->pressed & BUTTON_BIT(button)) {
        fighter_press(fighter, (Button)button);
      }
    }
    if (fighter->released & BUTTON_BIT(BUTTON_LOW_KICK)) {
      fighter_release(fighter, BUTTON_LOW_KICK);
    }
  }
}

static q16 to_q16(double value) {
  return (q16)floor(value * Q16_ONE);
}

/* Our world update: arena bounds, pair axis, body separation, callbacks,
 * gravity and integration. Script motion is expressed in units per tick. */
void battle_world(void) {
  Fighter *a = &g_battle.fighters[0], *b = &g_battle.fighters[1];
  double r2 = (double)g_battle.arena_radius * g_battle.arena_radius;
  for (int i = 0; i < 2; ++i) {
    Body *body = &g_battle.fighters[i].body;
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
    g_battle.axis[0] = to_q16(dx / length);
    g_battle.axis[1] = to_q16(dz / length);
  }
  g_battle.distance = (q16)floor(length);
  q16 dy = a->body.pos[AXIS_Y] - b->body.pos[AXIS_Y];
  if (!(a->flags & FF_PAIRED) && !(b->flags & FF_PAIRED) && g_battle.distance < 0x8000 && (dy < 0 ? -dy : dy) <= 0xb333) {
    q16 push = (0x8000 - g_battle.distance) / 2 + 1;
    for (int k = 0; k < 2; ++k) {
      q16 step = q16_mul(g_battle.axis[k], push);
      a->body.pos[k == 0 ? AXIS_X : AXIS_Z] -= step;
      b->body.pos[k == 0 ? AXIS_X : AXIS_Z] += step;
    }
  }
  for (int i = 0; i < 2; ++i) {
    Fighter *fighter = &g_battle.fighters[i];
    Body *body = &fighter->body;
    int sign = (dx < 0 ? -1 : 1) * fighter->facing;
    fighter->heading[0] = g_battle.axis[0] * sign;
    fighter->heading[1] = g_battle.axis[1] * sign;
    if (body->physics != NULL)
      body->physics(fighter);
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
          land(fighter);
      }
    }
  }
  for (int i = 0; i < 2; ++i) {
    Body *body = &g_battle.fighters[i].body;
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
static void resolve_hits(const Fighter snapshot[2]) {
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
    if (g_battle.fighters[i].current_move == attacker->current_move) {
      fighter_note_contact(&g_battle.fighters[i], blocked ? CONTACT_BLOCK : defender_height > 0.f ? CONTACT_AIR_HIT : CONTACT_HIT);
    }
    Fighter *target = &g_battle.fighters[1 - i];
    if (!blocked) {
      battle_damage(target, move->damage * Q16_ONE / 100);
      float height = defender_height + ((move->flags & SF_LOW) ? 0.25f : 1.2f);
      battle_blood_hit(attacker, target, (Vec3){(fighter_x(attacker) + fighter_x(target)) * .5f, -height, fighter_z(target)}, move->damage * Q16_ONE / 100);
    }
    target->body.pos[AXIS_X] += to_q16((double)attacker->facing * move->push * (blocked ? 0.4 : 1.0));
    if (!blocked) {
      if ((move->flags & SF_LAUNCH) && target->armed && target->disarm != NULL) {
        target->disarm(target);
      }
      fighter_enter_state(target, (move->flags & SF_LAUNCH) ? S_KNOCKDOWN : S_HIT);
      target->ticks = fighter_move(target)->recovery - move->stun;
      if (move->flags & SF_LAUNCH) {
        target->body.vel[AXIS_Y] = -0x140e; /* the prototype's 4.7 units/s */
        target->body.gravity = 0x111;
      }
    }
    g_battle.effects[i] =
        (HitEffect){(fighter_x(attacker) + fighter_x(defender)) * 0.5f, defender_height + ((move->flags & SF_LOW) ? 0.25f : 1.2f), 10, blocked};
  }
}

static void step_projectiles(void) {
  for (int p = 0; p < MAX_PROJECTILES; ++p) {
    Projectile *projectile = &g_battle.projectiles[p];
    if (projectile->kind != 0) {
      projectile->ticks++;
      projectile->step(projectile);
      if (--projectile->lifetime <= 0) {
        projectile->kind = 0;
      }
    }
  }
}

static int battle_event_owner_index(const Fighter *owner) {
  if (owner == &g_battle.fighters[0]) {
    return 0;
  }
  if (owner == &g_battle.fighters[1]) {
    return 1;
  }
  require(false, "Battle event owner must be an active fighter");
  return 0;
}

BattleEvent *battle_event_start(Fighter *owner, BattleEventFn step, bool cancel_on_owner_change, bool cancel_on_ko) {
  require(owner != NULL && step != NULL, "Battle event requires an owner and callback");
  int owner_index = battle_event_owner_index(owner);
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (!event->active) {
      *event = (BattleEvent){.step = step,
          .generation = ++g_battle.event_generation,
          .due_tick = g_battle.ticks + 1,
          .owner_continuation = owner->continuation,
          .owner = owner_index,
          .active = true,
          .cancel_on_owner_change = cancel_on_owner_change,
          .cancel_on_ko = cancel_on_ko};
      return event;
    }
  }
  require(false, "Battle event pool exhausted");
  return NULL;
}

void battle_event_cancel(BattleEvent *event) {
  if (event != NULL) {
    *event = (BattleEvent){0};
  }
}

void battle_event_cancel_all(void) {
  memset(g_battle.events, 0, sizeof g_battle.events);
}

void battle_event_delay(BattleEvent *event, int ticks) {
  require(event != NULL && event->active && ticks > 0, "Battle event delay must be positive");
  event->due_tick = g_battle.ticks + (uint64_t)ticks;
}

void battle_event_program(BattleEvent *event, int frames) {
  require(event != NULL && event->active && frames > 0 && frames <= EVENT_FRAME_CAPACITY, "Invalid event frame range");
  event->generation = ++g_battle.event_generation;
  event->frame = 0;
  event->frame_count = frames;
  memset(event->frames, 0, (size_t)frames * sizeof event->frames[0]);
  event->due_tick = g_battle.ticks + 1;
}

void battle_event_frame(BattleEvent *event, int frame, int value, BattleEventFn action) {
  require(frame >= 0 && frame < event->frame_count, "Invalid event action frame");
  event->frames[frame] = (BattleEventFrame){action, value};
}

static bool event_step_frame(BattleEvent *event, Fighter *owner, Fighter *other) {
  uint64_t generation = event->generation;
  const BattleEventFrame *frame = &event->frames[event->frame];
  event->frame_value = frame->value;
  if (frame->action != NULL && !frame->action(event, owner, other)) {
    return false;
  }
  if (!event->active || event->generation != generation) {
    return true;
  }
  if (++event->frame == event->frame_count) {
    event->frame_count = 0;
    return event->step(event, owner, other);
  }
  return true;
}

void battle_event_pump(void) {
  for (int i = 0; i < MAX_BATTLE_EVENTS; ++i) {
    BattleEvent *event = &g_battle.events[i];
    if (!event->active) {
      continue;
    }
    if (event->owner < 0 || event->owner >= 2) {
      battle_event_cancel(event);
      continue;
    }
    Fighter *owner = &g_battle.fighters[event->owner];
    if ((event->cancel_on_ko && owner->health <= 0) || (event->cancel_on_owner_change && owner->continuation != event->owner_continuation)) {
      battle_event_cancel(event);
      continue;
    }
    if (event->due_tick > g_battle.ticks) {
      continue;
    }
    event->due_tick = g_battle.ticks + 1;
    uint64_t generation = event->generation;
    bool keep = event->frame_count > 0 ? event_step_frame(event, owner, &g_battle.fighters[1 - event->owner])
                                       : event->step(event, owner, &g_battle.fighters[1 - event->owner]);
    if (event->active && event->generation == generation && !keep) {
      battle_event_cancel(event);
    }
  }
}

void battle_step(const Input input[2]) {
  int i;
  battle_capture_render_snapshot();
  g_battle.ticks++;
  g_battle.sound_count = 0;
  battle_step_presentation();
  for (i = 0; i < 2; ++i)
    sample_switches(&g_battle.fighters[i], input[i]);
  for (i = 0; i < 2; ++i) {
    battle_cpu_step(&g_battle.fighters[i]);
  }
  for (i = 0; i < 2; ++i) {
    Fighter *fighter = &g_battle.fighters[i];
    /* 422800: decrement the recovery delay, then restore 5% per tick. */
    fighter->run_delay = maxi(0, fighter->run_delay - 1);
    if (fighter->run_delay == 0) {
      fighter->run_energy = mini(Q16_ONE, fighter->run_energy + 0xccc);
    }
  }
  g_battle.phase_ticks++;
  for (i = 0; i < 2; ++i) {
    if (g_battle.effects[i].ticks > 0)
      g_battle.effects[i].ticks--;
  }
  if (g_battle.phase == PHASE_READY) {
    if (g_battle.phase_ticks >= 90) {
      g_battle.phase = PHASE_FIGHT;
      g_battle.phase_ticks = 0;
    }
    return;
  }
  if (g_battle.phase == PHASE_FINISH) {
    Fighter *winner = &g_battle.fighters[g_battle.winner];
    Fighter *victim = &g_battle.fighters[1 - g_battle.winner];
    if (g_battle.finish_waiting) {
      advance_fighter(victim, winner);
      battle_event_pump();
      battle_world();
      if (defeat_settled(victim)) {
        begin_finish_window();
      }
      return;
    }
    advance_fighter(winner, victim);
    advance_fighter(victim, winner);
    battle_event_pump();
    for (int button = 0; button < BUTTON_COUNT; ++button) {
      if (g_battle.phase_ticks < 300 && (winner->pressed & BUTTON_BIT(button))) {
        fighter_press(winner, (Button)button);
      }
    }
    battle_world();
    /* 421d40: 60-tick grace, clear the contact latch, then 240 ticks
     * ending early on contact. A press on expiry cannot extend the clock. */
    if (g_battle.phase_ticks == 60) {
      g_battle.contacted = false;
    }
    if (g_battle.phase == PHASE_FINISH && ((g_battle.phase_ticks > 60 && g_battle.contacted) || g_battle.phase_ticks >= 300)) {
      battle_end_finisher();
    }
    return;
  }
  if (g_battle.phase != PHASE_FIGHT) {
    /* Poses keep playing to their last sample and bodies keep falling. */
    for (i = 0; i < 2; ++i) {
      Fighter *fighter = &g_battle.fighters[i];
      fighter_fiber_pump_begin(fighter);
      if (fighter->pending != NULL) {
        FighterFn entry = fighter->pending;
        fighter->pending = NULL;
        entry(fighter);
      } else {
        fighter->ticks++;
        if (fighter->script != NULL) {
          fighter->script(fighter);
        } else if (g_battle.phase == PHASE_MATCH_OVER && fighter_fiber_active(fighter)) {
          /* Finisher presentation may keep a generated pose loop after the
           * result. The next round/reset retires its actor-owned stack. */
          fighter_step_fiber(fighter);
        } else if (fighter->current_mmove != NULL) {
          fighter_step_mmove(fighter);
        } else if (fighter->sequence != NULL) {
          fighter_step_sequence(fighter);
        }
        if (g_battle.phase == PHASE_MATCH_OVER && fighter->scene.clips != NULL) {
          scene_step(fighter);
        }
      }
    }
    if (g_battle.phase == PHASE_FATALITY) {
      step_projectiles();
    }
    battle_event_pump();
    battle_world();
    if (g_battle.phase == PHASE_ROUND_OVER && g_battle.phase_ticks >= 180)
      battle_next_round();
    return;
  }

  g_battle.time_ticks--;
  /* Retail tick 0x41fd70: task pump (fighters, then press tasks), world pump. */
  for (i = 0; i < 2; ++i)
    advance_fighter(&g_battle.fighters[i], &g_battle.fighters[1 - i]);
  battle_event_pump();
  step_projectiles();
  dispatch_presses();
  battle_world();
  {
    Fighter snapshot[2];
    memcpy(snapshot, g_battle.fighters, sizeof snapshot);
    resolve_hits(snapshot);
  }
  if (g_battle.fighters[0].health <= 0 || g_battle.fighters[1].health <= 0 || g_battle.time_ticks <= 0) {
    finish_round();
  }
}
