static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

static void script_ring_spawn(Fighter *fighter, int task_type) {
  require(task_type == 68, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0xccc, -0x1999, 48);
}

static void sonya_ring(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  fighter_start_fiber(fighter, retail_script_004ed4c0, NULL);
}

static void script_ring_flash(Fighter *fighter, int task_type) {
  require(task_type == 38, "Incorrect ring flash task");
  battle_particles(fighter, 2, 3, true);
}
