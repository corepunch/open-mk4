#include "game.h"
#include "math_util.h"
#include <stdlib.h>

/* Retail contact, block and damage rules: docs/scorpion/combat-core.md.
 * Contact runs inside the attacker's move, so a hit replaces the victim's
 * move before the victim's own update in the same tick. */

static int fighter_index(const Battle *battle, const Fighter *fighter) {
  return fighter == &battle->fighters[0] ? 0 : 1;
}

/* Our gameplay hull is independent of rendering: 0.6 units wide, 1.8 high
 * standing and 1.0 crouched. Script attack boxes and damage keep their
 * original units. No sampled animation joints or retail pose emulation. */
static bool contact_geometry(const Battle *battle, const q16 origin[3], int facing, const Fighter *victim, const AttackRecord *record) {
  (void)battle;
  q16 dx = (victim->body.pos[AXIS_X] - origin[AXIS_X]) * facing;
  q16 dz = victim->body.pos[AXIS_Z] - origin[AXIS_Z];
  q16 feet = victim->body.pos[AXIS_Y] - victim->body.ground;
  q16 head = feet - (victim->crouching ? Q16_ONE : 0x1cccc);
  q16 top = origin[AXIS_Y] + record->top;
  q16 bottom = top + record->height;
  return abs(dz) <= 0x8000 && dx + 0x4ccc >= record->reach - record->length && dx - 0x4ccc <= record->reach && head <= bottom && feet >= top;
}

/* 0x48e890 / 0x48e800. Fighters driven by input frames block like retail
 * humans; others block through their action codes like retail CPUs. */
static bool blocks(const Fighter *victim, const AttackRecord *record) {
  if ((record->flags & ATTACK_UNBLOCKABLE) || fighter_airborne(victim)) {
    return false;
  }
  if (!victim->human) {
    return victim->action == 0x2002 || victim->action == 0x2005;
  }
  if (victim->flags & FF_BUSY) {
    return false;
  }
  if (!(victim->held & BUTTON_BIT(BUTTON_BLOCK))) {
    return false;
  }
  return !(record->flags & ATTACK_LOW) || (victim->held & BUTTON_BIT(BUTTON_DOWN));
}

/* 0x48de80 */
void battle_damage(Battle *battle, Fighter *victim, q16 damage) {
  if (battle->ko_winner != 0) {
    return;
  }
  if (victim->roster == 15) {
    damage = q16_mul(Q16_ONE / 2, damage);
  }
  q16 scale = victim->damage_scale;
  if (scale < 0 || scale > Q16_ONE) {
    scale = Q16_ONE;
  }
  damage = q16_mul(scale, damage);
  victim->damage_total += damage;
  victim->health -= damage;
  if (victim->health <= 0) {
    victim->health = 0;
    battle->ko_winner = fighter_index(battle, victim) == 0 ? 2 : 1;
  }
}

/* 0x48d4a0 */
ContactResult battle_contact_from(Battle *battle, Fighter *attacker, const AttackRecord *record, const q16 origin[3], int facing) {
  Fighter *victim = (Fighter *)fighter_opponent(battle, attacker);
  if (battle->contacts_off || !contact_geometry(battle, origin, facing, victim, record)) {
    return CONTACT_MISS;
  }
  battle->contacted = true;
  if (victim->flags & FF_UNTOUCHABLE) {
    return CONTACT_MISS;
  }
  victim->flags = (record->flags & ATTACK_MARK) ? victim->flags | FF_MARK : victim->flags & ~(unsigned)FF_MARK;
  ContactResult result = CONTACT_HIT;
  int reaction = record->blocked;
  bool chip_ko = false;
  if (!(victim->flags & FF_NO_GUARD) && blocks(victim, record)) {
    battle_damage(battle, victim, q16_mul(record->damage, 0x3333));
    if (victim->health > 0) {
      result = CONTACT_BLOCK;
    } else {
      chip_ko = true; /* a lethal chip becomes a hit without full damage */
    }
  }
  if (result == CONTACT_HIT) {
    if (!chip_ko) {
      battle_damage(battle, victim, record->damage);
    }
    victim->hits++;
    if (fighter_airborne(victim)) {
      result = CONTACT_AIR_HIT;
      reaction = record->air;
    } else {
      if (victim->body.pos[AXIS_Y] > victim->body.ground) {
        victim->body.pos[AXIS_Y] = victim->body.ground;
      }
      reaction = record->hit;
    }
  }
  fighter_note_contact(attacker, result);
  int side = fighter_index(battle, attacker);
  /* Absolute retail Y has the soles at 0, so height above the floor is -Y. */
  float x = q16_float(attacker->body.pos[AXIS_X] / 2 + victim->body.pos[AXIS_X] / 2);
  float height = -q16_float(origin[AXIS_Y] + record->top + record->height / 2);
  battle->effects[side] = (HitEffect){x, height, 10, result == CONTACT_BLOCK};
  battle_react(battle, victim, reaction);
  return result;
}

ContactResult battle_contact(Battle *battle, Fighter *attacker, const AttackRecord *record) {
  return battle_contact_from(battle, attacker, record, attacker->body.pos, attacker->facing);
}

Projectile *battle_spawn_projectile(Battle *battle, Fighter *owner, int kind, int lifetime, ProjectileFn step) {
  require(step != NULL && lifetime > 0, "Invalid projectile");
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    Projectile *p = &battle->projectiles[i];
    if (p->kind == 0) {
      *p = (Projectile){.kind = kind, .owner = fighter_index(battle, owner), .facing = owner->facing, .lifetime = lifetime, .step = step};
      for (int k = 0; k < 3; ++k) {
        p->pos[k] = owner->body.pos[k];
      }
      return p;
    }
  }
  return NULL;
}
