static void jarek_ai_projectile_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  if (battle_projectile_overlaps(projectile, &jarek_selector_25_record)) {
    Fighter *owner = &g_battle.fighters[projectile->owner];
    projectile->kind = 0;
    fighter_start_fiber(owner, retail_script_004ed190, NULL);
  }
}

static void jarek_ai_projectile_spawn(Fighter *fighter, int task_type) {
  require(task_type == 78, "Unexpected Jarek cooperative task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, jarek_ai_projectile_step);
  if (projectile == NULL) {
    return;
  }
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}
