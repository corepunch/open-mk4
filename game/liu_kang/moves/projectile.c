#include "../character.h"

static void fireball_projectile(Projectile *p);
static void fireball(Fighter *fighter, bool low);
static const MoveDef s_fireball_high = {.clip = 18, .last = 44, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030C};
static const MoveDef s_fireball_low = {.clip = 19, .last = 46, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030D};
static const MoveDef s_fireball_air = {.clip = 20, .last = 5, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_030E};
static const AttackRecord fireball_high_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 64, 10, 2, ATTACK_MARK, 0x1999};
static const AttackRecord fireball_low_record = {0x3d70, -0x1eb8, 0x7d70, 0x3ae1, 65, 10, 2, ATTACK_MARK, 0x1999};

static void fireball_projectile(Projectile *p) {
  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    p->pos[axis] += p->vel[axis];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  bool low = p->phase != 0;
  if ((low || fighter_peer(owner)->action != ACTION_RETAIL_2001) &&
      battle_projectile_contact(p, low ? &fireball_low_record : &fireball_high_record) != CONTACT_MISS) {
    p->kind = 0;
  }
}

void liu_kang_fireball_spawn(Fighter *fighter, int type) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, fireball_projectile);
  if (p == NULL) {
    return;
  }
  p->phase = type == 1;
  p->mesh_animation = &liu_kang_fireball_mesh;
  p->initial_mesh = 50; /* 4ed730/740/760 -> rig 50678c +24. */
  q16 forward = type == 2 ? 0x9999 : type == 1 ? 0x28f5 : 0x1999;
  p->pos[AXIS_Y] += type == 2 ? -0x2147 : type == 1 ? 0x35c2 : -0x7333;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    p->pos[axis] += q16_mul(fighter_axis(fighter, axis), forward);
    p->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void fireball(Fighter *fighter, bool low) {
  bool air = !low && fighter_airborne(fighter);
  if (g_battle.phase != PHASE_FIGHT || (low && fighter_airborne(fighter))) {
    return;
  }
  if (!air) {
    mk4_script_stop_body(fighter);
  }
  fighter_start_move(fighter, S_PUNCH, air ? &s_fireball_air : low ? &s_fireball_low : &s_fireball_high);
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  fighter_upright(fighter);
  fighter->script_phase = air ? 2 : low ? 1 : 0;
  fighter->sample_override = 0;
  FighterFn script = air ? liu_kang_entry_retail_script_004ed6f8 : low ? liu_kang_entry_retail_script_004ed678 : liu_kang_entry_retail_script_004ed6c0;
  script(fighter);
}

void liu_kang_high_fireball(Fighter *fighter) {
  fireball(fighter, false);
}

void liu_kang_low_fireball(Fighter *fighter) {
  fireball(fighter, true);
}
