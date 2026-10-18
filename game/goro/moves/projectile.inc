static void goro_cpu_projectile_step(Projectile *projectile) {
  if (g_battle.contacts_off) {
    projectile->kind = 0;
    return;
  }
  for (int axis = 0; axis < 3; ++axis) {
    projectile->pos[axis] += projectile->vel[axis];
  }
  projectile->sample = projectile->ticks < 24 ? projectile->ticks : 24;
  if (battle_projectile_overlaps(projectile, &goro_cpu_projectile_record) &&
      battle_projectile_contact(projectile, &goro_cpu_projectile_record) != CONTACT_MISS) {
    projectile->kind = 0;
  }
}

static void goro_cpu_projectile_spawn(Fighter *fighter, int task_type) {
  require(task_type == 83, "Unexpected Goro cooperative task");
  Projectile *projectile = battle_spawn_projectile(fighter, PROJECTILE_FIREBALL, 48, goro_cpu_projectile_step);
  if (projectile == NULL) {
    return;
  }
  projectile->clip = 592;
  projectile->sample = 14;
  memcpy(projectile->pos, fighter->body.pos, sizeof projectile->pos);
  memcpy(projectile->heading, fighter->heading, sizeof projectile->heading);
  projectile->facing = fighter->facing;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    projectile->vel[axis] = q16_mul(fighter_axis(fighter, axis), 0x3333);
  }
}

static void goro_cpu_projectile(Fighter *fighter) {
  goro_start_script(fighter, S_PUNCH, &s_cpu_projectile, retail_script_004ed790);
}
