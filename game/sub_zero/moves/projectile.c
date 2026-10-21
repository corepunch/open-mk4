#include "native_services.h"

static const AttackRecord ice_record = {0x570a, -0x1eb8, 0xa147, 0x3ae1, 67, 67, 0, ATTACK_MARK, 0};
static const ParticleVisual ice_trail_visual = {
    .animation = {.first = 339, .count = 1, .period = 1, .scale = 0x4000}, .billboard = true, .additive = true, .shrink = 0x51e};
static const ParticleSpray ice_trail = {.visual = &ice_trail_visual,
    .count = 10,
    .lifetime = 10,
    .velocity = {0, -0x7ae, 0},
    .spread = {0, 0x28f, 0},
    .offset = {0x1000, 0xc00, 0},
    .position_spread = {0x6666, 0x6666, 0x6666}};

void sub_zero_ice_projectile(Projectile *p) {
  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int i = 0; i < 3; ++i) {
    p->pos[i] += p->vel[i];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  if (fighter_peer(owner)->action != ACTION_RETAIL_2001 && battle_projectile_contact(p, &ice_record) != CONTACT_MISS) {
    p->kind = 0;
  }
  int64_t radius2 = (int64_t)g_battle.arena_radius * g_battle.arena_radius + (int64_t)0xc0000 * Q16_ONE;
  if ((int64_t)p->pos[AXIS_X] * p->pos[AXIS_X] + (int64_t)p->pos[AXIS_Z] * p->pos[AXIS_Z] > radius2) {
    p->kind = 0;
  }
  if (p->kind != 0) {
    battle_spray(&ice_trail, (Vec3){q16_float(p->pos[0]), q16_float(p->pos[1]), q16_float(p->pos[2])}, (Vec3){0}, 0, 0);
  }
}
