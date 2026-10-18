static void ice_projectile(Projectile *p) {
  if (g_battle.contacts_off) {
    p->kind = 0;
    return;
  }
  for (int i = 0; i < 3; ++i) {
    p->pos[i] += p->vel[i];
  }
  Fighter *owner = &g_battle.fighters[p->owner];
  if (opponent(owner)->action != ACTION_RETAIL_2001 && battle_projectile_contact(p, &ice_record) != CONTACT_MISS) {
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
