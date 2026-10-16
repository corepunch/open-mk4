#include "game.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

static void finish_round(Battle *battle);

static bool locked(const Fighter *fighter) {
  return (fighter_move(fighter)->flags & SF_LOCK) != 0;
}

static void place(Battle *battle, Fighter *fighter, int roster_index, int wins, float x, int facing) {
  *fighter = (Fighter){0};
  fighter->roster = roster_index;
  fighter->wins = wins;
  fighter->health = 100;
  fighter->x = x;
  fighter->facing = facing;
  fighter_enter_state(battle, fighter, S_IDLE);
}

void battle_init(Battle *battle, int p1, int p2) {
  memset(battle, 0, sizeof *battle);
  battle->wall = 5.f;
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
  battle->round++;
  for (i = 0; i < 2; ++i) {
    int roster_index = battle->fighters[i].roster;
    int wins = battle->fighters[i].wins;
    float x = i ? 2.f : -2.f;
    int facing = i ? -1 : 1;
    place(battle, &battle->fighters[i], roster_index, wins, x, facing);
  }
  memset(battle->effects, 0, sizeof battle->effects);
  battle->phase = PHASE_READY;
  battle->phase_ticks = 0;
  battle->time_ticks = 99 * 60;
  battle->winner = -1;
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
    if (battle->fighters[i].health <= 0)
      fighter_enter_state(battle, &battle->fighters[i], S_KNOCKDOWN);
  }
  if (battle->winner >= 0 && battle->fighters[battle->winner].wins >= 2)
    battle->phase = PHASE_MATCH_OVER;
  else
    battle->phase = PHASE_ROUND_OVER;
  battle->phase_ticks = 0;
}

static bool held(const Fighter *fighter, Button button) {
  return (fighter->held & BUTTON_BIT(button)) != 0;
}

static void advance_fighter(Battle *battle, Fighter *fighter, const Fighter *other) {
  const CharacterDef *def = roster_def(fighter->roster);
  const Motion *motion = def->motion;
  int dir;

  fighter->contact_ready = false;
  if (!locked(fighter) && fighter->height == 0.f)
    fighter->facing = fighter->x <= other->x ? 1 : -1;
  if (fighter->pending != NULL) {
    /* The press task interrupted this fighter last tick; its entry runs now. */
    FighterFn entry = fighter->pending;
    fighter->pending = NULL;
    entry(battle, fighter);
  } else {
    const MoveDef *state = fighter_move(fighter);
    fighter->ticks++;
    if (state->sequence != NULL) {
      fighter_step_sequence(battle, fighter);
    } else if (state->flags & SF_ATTACK) {
      int total = state->startup + state->active + state->recovery;
      if (fighter->ticks >= total)
        fighter_enter_state(battle, fighter, fighter->height > 0.f ? S_JUMP : S_IDLE);
    } else if (state->flags & SF_REACTION) {
      bool landed = !(state->flags & SF_WAIT_LAND) || fighter->height == 0.f;
      if (fighter->ticks >= state->recovery && landed)
        fighter_enter_state(battle, fighter, S_IDLE);
    }
  }

  if (fighter->height > 0.f || fighter->velocity > 0.f) {
    fighter->height += fighter->velocity * TICK_SECONDS;
    fighter->velocity -= motion->gravity * TICK_SECONDS;
    if (fighter->height <= 0.f) {
      fighter->height = 0.f;
      fighter->velocity = 0.f;
      if (fighter->state == S_JUMP)
        fighter_enter_state(battle, fighter, S_IDLE);
    }
  }
  if (locked(fighter) || fighter->pending != NULL)
    return;

  fighter->crouching = held(fighter, BUTTON_DOWN) && fighter->height == 0.f;
  if (held(fighter, BUTTON_BLOCK) && fighter->height == 0.f) {
    fighter_enter_state(battle, fighter, S_BLOCK);
    return;
  }
  if (held(fighter, BUTTON_UP) && fighter->height == 0.f) {
    fighter->velocity = motion->jump_velocity;
    fighter->height = 0.01f;
    fighter_enter_state(battle, fighter, S_JUMP);
  }
  dir = (int)held(fighter, BUTTON_RIGHT) - (int)held(fighter, BUTTON_LEFT);
  if (!fighter->crouching) {
    float speed = held(fighter, BUTTON_RUN) ? motion->run : motion->walk;
    fighter->x += (float)dir * speed * TICK_SECONDS;
  }
  if (fighter->height > 0.f)
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

static void separate_bodies(Battle *battle) {
  Fighter *a = &battle->fighters[0];
  Fighter *b = &battle->fighters[1];
  float sign, mid;
  int i;
  for (i = 0; i < 2; ++i)
    battle->fighters[i].x = clampf(battle->fighters[i].x, -battle->wall, battle->wall);
  if (fabsf(a->height - b->height) >= 1.25f || fabsf(a->x - b->x) >= 0.7f)
    return;
  sign = a->x <= b->x ? 1.f : -1.f;
  mid = (a->x + b->x) * 0.5f;
  mid = clampf(mid, -battle->wall + 0.35f, battle->wall - 0.35f);
  a->x = mid - sign * 0.35f;
  b->x = mid + sign * 0.35f;
}

/* Both attacks are judged from the positions at the start of resolution, so
 * two moves that become active on the same tick can trade. */
static void resolve_hits(Battle *battle, const Fighter snapshot[2]) {
  size_t i;
  for (i = 0; i < 2; ++i) {
    const Fighter *attacker = &snapshot[i];
    const Fighter *defender = &snapshot[1 - i];
    const MoveDef *move = fighter_move(attacker);
    Fighter *target;
    float distance;
    bool blocked;
    int damage;
    int reaction;

    if (!(move->flags & SF_ATTACK) || attacker->connected)
      continue;
    if (move->flags & SF_SCRIPT_CONTACT) {
      if (!attacker->contact_ready) {
        continue;
      }
    } else if (attacker->ticks < move->startup || attacker->ticks >= move->startup + move->active) {
      continue;
    }
    distance = (defender->x - attacker->x) * (float)attacker->facing;
    if (distance < 0.f || distance > move->reach)
      continue;
    if (fabsf(attacker->height - defender->height) > 0.9f)
      continue;
    if ((move->flags & SF_LOW) && defender->height > 0.2f)
      continue;
    if ((move->flags & SF_UNBLOCKABLE) && (defender->height > 0.f || defender->state == S_KNOCKDOWN || defender->state == S_HIT))
      continue;
    if (defender->state == S_KNOCKDOWN)
      continue;

    blocked = defender->state == S_BLOCK && !(move->flags & SF_UNBLOCKABLE) && (!(move->flags & SF_LOW) || defender->crouching);
    /* A simultaneous incoming reaction may already have interrupted this
     * fighter. The outgoing hit still trades, but cannot restart its move. */
    if (battle->fighters[i].current_move == attacker->current_move) {
      fighter_note_contact(&battle->fighters[i], blocked ? CONTACT_BLOCK : defender->height > 0.f ? CONTACT_AIR_HIT : CONTACT_HIT);
    }
    target = &battle->fighters[1 - i];
    damage = blocked ? 0 : move->damage;
    target->health = (int)fmaxf(0.f, (float)target->health - (float)damage);
    target->x = clampf(target->x + (float)attacker->facing * move->push * (blocked ? 0.4f : 1.f), -battle->wall, battle->wall);
    if (!blocked) {
      reaction = (move->flags & SF_LAUNCH) ? S_KNOCKDOWN : S_HIT;
      fighter_enter_state(battle, target, reaction);
      target->ticks = fighter_move(target)->recovery - move->stun;
      if (move->flags & SF_LAUNCH) {
        target->velocity = 4.7f;
        target->height = fmaxf(0.01f, target->height);
      }
    }
    battle->effects[i] = (HitEffect){(attacker->x + defender->x) * 0.5f, defender->height + ((move->flags & SF_LOW) ? 0.25f : 1.2f), 10, blocked};
  }
}

void battle_step(Battle *battle, const Input input[2]) {
  int i;
  battle->ticks++;
  for (i = 0; i < 2; ++i)
    sample_switches(&battle->fighters[i], input[i]);
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
  if (battle->phase != PHASE_FIGHT) {
    for (i = 0; i < 2; ++i)
      battle->fighters[i].ticks++;
    if (battle->phase == PHASE_ROUND_OVER && battle->phase_ticks >= 180)
      battle_next_round(battle);
    return;
  }

  battle->time_ticks--;
  for (i = 0; i < 2; ++i)
    advance_fighter(battle, &battle->fighters[i], &battle->fighters[1 - i]);
  separate_bodies(battle);
  {
    Fighter snapshot[2];
    memcpy(snapshot, battle->fighters, sizeof snapshot);
    resolve_hits(battle, snapshot);
  }
  for (i = 0; i < 2; ++i) {
    Fighter *fighter = &battle->fighters[i];
    FighterFn next = fighter->after_contact;
    fighter->after_contact = NULL;
    if (next != NULL) {
      next(battle, fighter);
    }
  }
  if (battle->fighters[0].health <= 0 || battle->fighters[1].health <= 0 || battle->time_ticks <= 0) {
    finish_round(battle);
    return;
  }
  dispatch_presses(battle);
}
