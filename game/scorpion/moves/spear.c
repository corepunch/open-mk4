#include "../character.h"
#include <math.h>
#include "retail_vm.h"

static bool spear_not_pulling(const Fighter *fighter);
static void spear_cleanup(Fighter *fighter);
static void spear_projectile(Projectile *p);
static void spear_spawn(Fighter *fighter);
static const MoveDef s_spear = {.clip = 14, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0300};
static const MoveDef s_snared = {.clip = 199, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_4000};
static const MoveDef s_spear_stun = {.clip = 13, .last = MOVE_END, .flags = SF_LOCK, .action = ACTION_RETAIL_1008};

void scorpion_spear_stun_script(Fighter *fighter) {
  retail_vm_wait_native(fighter, 64, scorpion_special_ready);
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

/* Runs when the spear task is cancelled or completes. Pull
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
    Fighter *victim = fighter_peer(fighter);
    if (pulling && fighter_move(victim)->clip == 199) {
      scorpion_special_ready(victim);
    }
  }
}

static void spear_projectile(Projectile *p) {
  Fighter *owner = &g_battle.fighters[p->owner], *victim = fighter_peer(owner);
  bool interrupted = fighter_move(owner)->clip != 14 || !owner->retail_vm.active || owner->pending != NULL;
  if (interrupted || g_battle.contacts_off) {
    if (p->phase == 1 && fighter_move(victim)->clip == 199) {
      scorpion_special_ready(victim);
    }
    p->kind = 0;
    return;
  }
  if (p->phase == 1) {
    if (fighter_move(victim)->clip != 199 || victim->health <= 0) {
      p->kind = 0;
      return;
    }
    /* 49b170/487350: pull along the throw direction until the pair is
     * within 0.8. */
    if (fighter_distance(owner) <= q16_float(0xcccc)) {
      mk4_script_stop_body(victim);
      fighter_start_move(victim, S_HIT, &s_spear_stun);
      victim->sample_override = 0;
      victim->flags |= FF_REACTING;
      scorpion_entry_retail_script_004e9f20(victim);
      fighter_start_move(owner, S_PUNCH, &s_spear);
      owner->sample_override = 31;
      scorpion_entry_retail_script_004ed4a8(owner);
      owner->retail_vm.cleanup = spear_cleanup;
      p->kind = 0;
    } else {
      victim->body.pos[AXIS_X] -= q16_mul(p->heading[0], 0x3333);
      victim->body.pos[AXIS_Z] -= q16_mul(p->heading[1], 0x3333);
      victim->body.pos[AXIS_Y] = victim->body.ground;
      memcpy(p->pos, victim->body.pos, sizeof p->pos);
    }
    return;
  }
  if (p->phase == 2) {
    double dx = owner->body.pos[AXIS_X] - p->pos[AXIS_X], dz = owner->body.pos[AXIS_Z] - p->pos[AXIS_Z];
    double distance = sqrt(dx * dx + dz * dz);
    if (distance <= 0x3333) {
      p->kind = 0;
    } else {
      p->pos[AXIS_X] += (q16)lround(dx / distance * 0x3333);
      p->pos[AXIS_Z] += (q16)lround(dz / distance * 0x3333);
    }
    return;
  }
  for (int k = 0; k < 3; ++k) {
    p->pos[k] += p->vel[k];
  }
  AttackRecord record = retail_vm_attack_record(retail_vm_image(), 0x4ebeb8);
  ContactResult result = battle_contact_from(owner, &record, p->pos, p->facing);
  if ((result == CONTACT_HIT || result == CONTACT_AIR_HIT) && victim->health <= 0) {
    /* Retain the lethal contact's ordinary reaction; no live pull may park
     * the defeated controller after the round retires projectile objects. */
    p->kind = 0;
  } else if (result == CONTACT_HIT || result == CONTACT_AIR_HIT) {
    mk4_script_stop_body(victim);
    fighter_start_move(victim, S_HIT, &s_snared);
    victim->sample_override = 0;
    victim->flags |= FF_REACTING;
    scorpion_entry_retail_script_004e9d00(victim);
    victim->damage_scale = 0x9999; /* 0x485fb0: subsequent combo damage scales to 60%. */
    p->phase = 1;
    p->lifetime = 100;
  } else if (result == CONTACT_BLOCK || p->ticks >= 30 || hypot(p->pos[AXIS_X], p->pos[AXIS_Z]) > g_battle.arena_radius) {
    p->phase = 2;
    p->lifetime = 45;
  }
}

static void spear_spawn(Fighter *fighter) {
  Projectile *p = battle_spawn_projectile(fighter, PROJECTILE_SPEAR, 90, spear_projectile);
  if (p != NULL) {
    /* 49a6a0 launches 0.75 ahead along the owner's heading at 0.2/tick. */
    p->pos[AXIS_X] += q16_mul(p->heading[0], 0xc000);
    p->pos[AXIS_Z] += q16_mul(p->heading[1], 0xc000);
    p->pos[AXIS_Y] = -0x16666;
    p->vel[AXIS_X] = q16_mul(p->heading[0], 0x3333);
    p->vel[AXIS_Z] = q16_mul(p->heading[1], 0x3333);
  }
}

/* 4ed448/4ed47c -> 4ed470/4ed498. Normal/fast launch is at tick 10/8;
 * sample 30 follows at 15/11. Keep the existing paired-pull policy: arrival
 * replaces this program with the 4ed4a8 return, while cancellation cleans up. */

bool scorpion_can_spear(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return g_battle.phase == PHASE_FIGHT && g_battle.ticks >= fighter->spear_ready && victim->hits <= 3 && victim->action != ACTION_RETAIL_1008;
}

void scorpion_spear(Fighter *fighter) {
  bool followup = (fighter->flags & FF_FOLLOW_UP) != 0;
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_spear);
  fighter->script_phase = followup;
  fighter->sample_override = followup ? 9 : 0;
  fighter->spear_ready = g_battle.ticks + 90;
  scorpion_entry_retail_script_004ed448(fighter);
  fighter->retail_vm.cleanup = spear_cleanup;
}

void scorpion_script_spear_done(Fighter *fighter) {
  if (spear_not_pulling(fighter)) {
    scorpion_special_ready(fighter);
  } else {
    retail_vm_wait_native(fighter, 1, scorpion_script_spear_done);
  }
}

void scorpion_script_spear_return(Fighter *fighter) {
  fighter->spear_ready = g_battle.ticks + 90;
  scorpion_special_ready(fighter);
}

void scorpion_script_spawn_spear(Fighter *fighter, int type) {
  require(type == 67, "Unexpected spear task");
  spear_spawn(fighter);
}

void scorpion_script_spear_pull(Fighter *fighter) {
  fighter->damage_scale = 0x9999;
  retail_vm_wait_native(fighter, 0, NULL);
}
