static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

static void script_lightning_spawn(Fighter *fighter, int task_type) {
  require(task_type == 65, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0x28f5, 0, 48);
}

static void raiden_lightning(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  fighter_start_fiber(fighter, retail_script_004ed5f0, NULL);
}
