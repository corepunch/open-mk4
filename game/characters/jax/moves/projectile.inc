static void jax_script_cpu_projectile_step(Projectile *projectile) {
  Fighter *owner = &g_battle.fighters[projectile->owner];
  if (projectile->ticks >= 36 || g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  projectile->pos[0] += projectile->vel[0];
  projectile->pos[1] += projectile->vel[1];
  if (battle_projectile_overlaps(projectile, &s_jax_jab_record)) {
    projectile->kind = 0;
    battle_damage((Fighter *)fighter_opponent(owner), 0x170a);
  }
}

static void jax_script_cpu_projectile_spawn(Fighter *fighter, int task_type) {
  (void)task_type;
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 36, jax_script_cpu_projectile_step);
  if (projectile != NULL) {
    projectile->vel[0] = (q16)(fighter->facing * 0x1800);
    projectile->vel[1] = 0;
  }
}
