#include "retail_vm.h"
#include "../character.h"

static void projectile_step(Projectile *object);
static const MoveDef s_projectile = {.clip = 15, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x313};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 68, 10, 2, 2, 7864};
static const MeshAnimation projectile_mesh = {.first = 42, .count = 2, .period = 1, .scale = Q16_ONE};

static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

void sonya_script_ring_spawn(Fighter *fighter, int task_type) {
  require(task_type == 68, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0xccc, -0x1999, 48);
}

void sonya_ring(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  retail_vm_enter_owned(fighter, 0x4ed4c0, fighter_ready, 2);
}

void sonya_script_ring_flash(Fighter *fighter, int task_type) {
  require(task_type == 38, "Incorrect ring flash task");
  battle_particles(fighter, 2, 3, true);
}
