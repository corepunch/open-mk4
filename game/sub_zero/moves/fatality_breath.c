#include "../character.h"

static void breath_object(Projectile *p);
static void breath_freeze_object(Projectile *object);
static void breath_resume(Fighter *fighter);

void sub_zero_script_breath_victim_end(Fighter *victim) {
  fighter_finish_mmove(victim, NULL);
}

static void breath_object(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner];
  if (owner->action != ACTION_RETAIL_0700 || fighter_move(owner)->clip != 552) {
    p->kind = 0;
    return;
  }
  battle_particles(owner, 9 + (p->ticks & 1), 2, true);
}

void sub_zero_script_breath_object(Fighter *fighter, int type) {
  require(type == 0, "Unexpected breath task");
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 92, breath_object);
}

static void breath_freeze_object(Projectile *object) {
  Fighter *victim = sub_zero_opponent(&g_battle.fighters[object->owner]);
  if (victim->appearance != APPEARANCE_FROZEN || g_battle.phase != PHASE_FATALITY) {
    object->kind = 0;
  } else if (object->ticks % 8 == 0) {
    battle_particles(victim, 9, 1, true);
  }
}

void sub_zero_script_breath_freeze(Fighter *fighter, int type) {
  require(type == 0, "Unexpected freeze task");
  sub_zero_opponent(fighter)->appearance = APPEARANCE_FROZEN;
  battle_spawn_projectile(fighter, PROJECTILE_EFFECT, 120, breath_freeze_object);
}

void sub_zero_script_breath_approach(Fighter *fighter) {
  sub_zero_entry_retail_script_004d2c78(fighter);
}

static void breath_resume(Fighter *fighter) {
  fighter_set_speed(fighter, 0);
  fighter->script_phase = 3;
  sub_zero_entry_retail_script_004d2ca0(fighter);
}

void sub_zero_breath_travel(Fighter *fighter) {
  q16 distance = (q16)(fighter_distance(fighter) * Q16_ONE);
  int frames = maxi(0, q16_mul(distance - 0xcccc, 0x69999) / Q16_ONE) + 1;
  fighter->script_phase = 2;
  fighter->sample_override = 9;
  native_hold(fighter, frames, breath_resume);
}

void sub_zero_script_breath_shatter(Fighter *fighter) {
  Fighter *victim = sub_zero_opponent(fighter);
  battle_particles(victim, 3, 32, true);
  battle_shatter(victim);
  sub_zero_entry_retail_script_004d2cc8(fighter);
}
